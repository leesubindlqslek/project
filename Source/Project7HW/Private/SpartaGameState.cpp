#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "SpartaCharacter.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    CurrentLevelIndex = 0;
    MaxLevels = 3;
    CurrentWaveIndex = 0;
    MaxWaves = 3;
    WaveDuration = 20.f;
    CoinsToSpawnPerWave = TArray<int32>{ 20, 30, 40 };

LevelMapNames = { TEXT("BasicLevel"), TEXT("IntermediateLevel"), TEXT("AdvancedLevel") };
}


void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    StartLevel();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 ASpartaGameState::GetScore() const
{
    return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
    {
        SpartaGameInstance->AddToScore(Amount);
        Score += Amount;
    }
}

void ASpartaGameState::StartLevel()
{
    if (ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
    {
        SpartaPlayerController->ShowGameHUD();
    }

    if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
    {
        CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
    }

    CurrentWaveIndex = 0;
    StartWave();
}

void ASpartaGameState::StartWave()
{
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    // 기존 Wave 아이템 삭제
    for (AActor* Item : CurrentWaveItems)
    {
        if (Item && Item->IsValidLowLevelFast())
        {
            Item->Destroy();
        }
    }
    CurrentWaveItems.Empty();

    // 이번 Wave에 스폰할 코인 개수 결정
    int32 CoinsToSpawn = (CoinsToSpawnPerWave.IsValidIndex(CurrentWaveIndex)) ? CoinsToSpawnPerWave[CurrentWaveIndex] : 20;

    // SpawnVolume을 이용해 코인 스폰
    if (ASpawnVolume* SpawnVolume = GetSpawnVolume())
    {
        for (int32 i = 0; i < CoinsToSpawn; i++)
        {
            if (AActor* SpawnedActor = SpawnVolume->SpawnRandomItem())
            {
                if (SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                    CurrentWaveItems.Add(SpawnedActor);
                }
            }
        }
    }

    // Wave별 환경 변화
    if (CurrentWaveIndex == 1)
    {
        EnableWave2Environment();
    }
    else if (CurrentWaveIndex == 2)
    {
        EnableWave3Environment();
    }

    // Wave 타이머 시작
    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &ASpartaGameState::OnWaveTimeUp,
        WaveDuration,
        false
    );
}

void ASpartaGameState::EndWave()
{
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);

    // Wave3에서 활성화한 코인 궤도 회전 비활성화
    if (CurrentWaveIndex == 2)
    {
        SetAllCoinsOrbit(false);
    }

    ++CurrentWaveIndex;
    if (CurrentWaveIndex >= MaxWaves)
    {
        EndLevel();
    }
    else
    {
        StartWave();
    }
}

void ASpartaGameState::OnWaveTimeUp()
{
    EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCount++;
    UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount);

    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
    {
        EndWave();
    }
}

void ASpartaGameState::EndLevel()
{
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);

    if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
    {
        AddScore(Score);
        CurrentLevelIndex++;
        SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

        if (CurrentLevelIndex >= MaxLevels)
        {
            OnGameOver();
            return;
        }

        if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
        {
            UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
        }
        else
        {
            OnGameOver();
        }
    }
}

void ASpartaGameState::OnGameOver()
{
    if (ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
    {
        SpartaPlayerController->SetPause(true);
        SpartaPlayerController->ShowMainMenu(true);
    }
}

void ASpartaGameState::UpdateHUD()
{
    if (ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
    {
        if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
        {
            // 남은 시간 업데이트
            if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
            {
                float RemainingTime = 0.f;
                if (GetWorldTimerManager().IsTimerActive(WaveTimerHandle))
                {
                    RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
                }
                TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
            }

            // Score 업데이트
            if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
            {
                if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
                {
                    ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                }
            }

            // Wave 업데이트
            if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
            {
                WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d / %d"), CurrentWaveIndex + 1, MaxWaves)));
            }

            // Level 업데이트
            if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
            {
                LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
            }

            // 체력 ProgressBar 업데이트
            if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HPBar"))))
            {
                if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn()))
                {
                    // 체력 값 가져오기
                    float CurrentHealth = PlayerCharacter->GetHealth();

                }
            }
        }
    }
}

void ASpartaGameState::EnableWave2Environment()
{
    const FString Msg = TEXT("Wave2: Spawning and activating 5 Spike Traps!");
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Msg);
    }
}

void ASpartaGameState::EnableWave3Environment()
{
    const FString Msg = TEXT("Wave3: Activating coin orbit!");
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, Msg);
    }

    SetAllCoinsOrbit(true);
}

void ASpartaGameState::SetAllCoinsOrbit(bool bActive)
{
    TArray<AActor*> AllCoins;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoinItem::StaticClass(), AllCoins);

    for (AActor* CoinActor : AllCoins)
    {
        if (ACoinItem* Coin = Cast<ACoinItem>(CoinActor))
        {
            Coin->SetWave3OrbitActive(bActive);
        }
    }
}

ASpawnVolume* ASpartaGameState::GetSpawnVolume() const
{
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
    return (FoundVolumes.Num() > 0) ? Cast<ASpawnVolume>(FoundVolumes[0]) : nullptr;
}

ASpartaPlayerController* ASpartaGameState::GetSpartaPlayerController() const
{
    return Cast<ASpartaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

USpartaGameInstance* ASpartaGameState::GetSpartaGameInstance() const
{
    return Cast<USpartaGameInstance>(GetGameInstance());
}

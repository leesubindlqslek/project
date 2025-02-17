#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

class ASpawnVolume;
class ACoinItem;
class ASpikeTrap;
class ASpartaPlayerController;
class USpartaGameInstance;

UCLASS()
class PROJECT7HW_API ASpartaGameState : public AGameState
{
    GENERATED_BODY()

public:
    ASpartaGameState();

    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadWrite, Category = "Score")
    int32 Score;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 SpawnedCoinCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 CollectedCoinCount;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    float LevelDuration;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 CurrentLevelIndex;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 MaxLevels;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    TArray<FName> LevelMapNames;

    FTimerHandle LevelTimerHandle;
    FTimerHandle HUDUpdateTimerHandle;
    FTimerHandle WaveTimerHandle;



    // Wave 관리
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWaveIndex;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 MaxWaves;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    float WaveDuration;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    TArray<int32> CoinsToSpawnPerWave;





    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);
    UFUNCTION(BlueprintCallable, Category = "Level")
    void OnGameOver();
  
    void StartLevel();
    void EndLevel();
    void StartWave();
    void EndWave();
    void OnLevelTimeUp();
    void OnCoinCollected();
    void UpdateHUD();
    void OnWaveTimeUp();

private:
    // 현재 Wave에 스폰된 아이템들 (Wave 종료 시 파괴)
    UPROPERTY()
    TArray<AActor*> CurrentWaveItems;

    // Wave별 환경 변화 함수
    void EnableWave2Environment();
    void EnableWave3Environment();
    void SetAllCoinsOrbit(bool bActive);

    // 헬퍼 함수들
    ASpawnVolume* GetSpawnVolume() const;
    ASpartaPlayerController* GetSpartaPlayerController() const;
    USpartaGameInstance* GetSpartaGameInstance() const;
};

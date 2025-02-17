#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"

ACoinItem::ACoinItem()
    : SpinSpeed(180.f)
    , bOrbitMode(false)
    , OrbitAngle(0.f)
    , OrbitRadius(100.f)
    , PointValue(10)
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACoinItem::BeginPlay()
{
    Super::BeginPlay();
}

void ACoinItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
            {
                GameState->AddScore(PointValue);
                GameState->OnCoinCollected();
            }
        }

        if (ExplosionSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                GetWorld(),
                ExplosionSound,
                GetActorLocation()
            );
        }

        Destroy();  // DestroyItem() ¡æ Destroy()·Î ¼öÁ¤
    }
}

void ACoinItem::SetWave3OrbitActive(bool bActive)
{
    bOrbitMode = bActive;
}

void ACoinItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bOrbitMode)
    {
        OrbitAngle += SpinSpeed * DeltaTime;
        FVector CenterLocation = GetActorLocation();
        FVector NewLocation = CenterLocation + FVector(OrbitRadius * FMath::Cos(OrbitAngle), OrbitRadius * FMath::Sin(OrbitAngle), 0.f);
        SetActorLocation(NewLocation);
    }
}

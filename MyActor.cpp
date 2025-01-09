// MyActor.cpp
#include "MyActor.h"
#include "Engine/World.h"

AMyActor::AMyActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Hello, World!"));
}

void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveRandomly();
}

void AMyActor::MoveRandomly()
{
	FVector NewLocation = GetActorLocation() + FVector(FMath::RandRange(-1, 1), FMath::RandRange(-1, 1), 0);

	SetActorLocation(NewLocation);

	PrintLocation();
}

void AMyActor::PrintLocation()
{
	FVector CurrentLocation = GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Current Location: X = %f, Y = %f, Z = %f"), CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
}

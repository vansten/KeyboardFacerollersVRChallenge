// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorGameMode.h"
#include "KitesurfingSimulatorCharacter.h"
#include "KitesurfingSimulatorFishBase.h"

AKitesurfingSimulatorGameMode::AKitesurfingSimulatorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/Player_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AKitesurfingSimulatorFishBase> FishBPClass(TEXT("/Game/Blueprints/Fish_BP"));
	if (FishBPClass.Class != NULL)
	{
		FishClass = FishBPClass.Class;
	}
}

void AKitesurfingSimulatorGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GWorld)
	{
		UClass* fishClass = FishClass;
		FVector Location;
		for (int i = 0; i < 1000; ++i)
		{
			Location = FVector(FMath::FRandRange(29580.f, 85950.f), FMath::FRandRange(42310.f, 11600.f), FMath::FRandRange(40.f, 180.f));
			GWorld->SpawnActor(fishClass, &Location, &FRotator::ZeroRotator);
		}
	}
}
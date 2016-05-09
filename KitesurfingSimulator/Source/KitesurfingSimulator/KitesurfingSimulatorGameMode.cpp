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
			float x = FMath::FRandRange(3.0f, 8.0f) * 10000.0f;
			float y = FMath::FRandRange(4.5f, 11.0f) * 10000.0f;
			float z = FMath::FRandRange(60.0f, 140.0f);
			Location = FVector(x, y, z);
			GWorld->SpawnActor(fishClass, &Location, &FRotator::ZeroRotator);
		}
	}
}
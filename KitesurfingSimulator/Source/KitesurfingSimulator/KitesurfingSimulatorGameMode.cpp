// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorGameMode.h"
#include "KitesurfingSimulatorCharacter.h"

AKitesurfingSimulatorGameMode::AKitesurfingSimulatorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/Player_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "KitesurfingSimulatorGameMode.generated.h"

UCLASS(minimalapi)
class AKitesurfingSimulatorGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	TSubclassOf<class AKitesurfingSimulatorFishBase> FishClass;

public:
	AKitesurfingSimulatorGameMode();

	virtual void BeginPlay() override;
};




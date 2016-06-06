// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"


IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, KitesurfingSimulator, "KitesurfingSimulator" );

DEFINE_LOG_CATEGORY(Wiimote);

void OnScreenMessage(int32 key, float duration, FColor color, const FString& message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(key, duration, color, message);
	}
}
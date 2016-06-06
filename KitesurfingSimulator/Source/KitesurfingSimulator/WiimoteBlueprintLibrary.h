// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "WiiYourself/wiimote.h"
#include "WiimoteBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class KITESURFINGSIMULATOR_API UWiimoteBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static wiimote _remote;
	static bool _bIsConnected;

public:
	~UWiimoteBlueprintLibrary();

	UFUNCTION(BlueprintCallable, Category = Wiimote)
		static bool Connect();

	UFUNCTION(BlueprintCallable, Category = Wiimote)
		static void Disconnect();

	UFUNCTION(BlueprintCallable, Category = Wiimote)
		static FVector GetAcceleration();

	UFUNCTION(BlueprintCallable, Category = Wiimote)
		static FVector GetOrientation();

	UFUNCTION(BlueprintCallable, Category = Wiimote)
		static FRotator GetMotionPlusRaw();

	UFUNCTION(BlueprintCallable, Category = Wiimote)
		static FRotator GetMotionPlusSpeed();
};

static void on_state_change(wiimote& remote, state_change_flags changed, const wiimote_state &new_state);
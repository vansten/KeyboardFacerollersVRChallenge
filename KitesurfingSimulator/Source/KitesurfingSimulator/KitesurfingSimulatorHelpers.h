// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "KitesurfingSimulatorHelpers.generated.h"

/**
 * 
 */
UCLASS()
class KITESURFINGSIMULATOR_API UKitesurfingSimulatorHelpers : public UObject
{
	GENERATED_BODY()

public:
	static FString FloatToStringWithPrecision(float Value, int32 Precision, bool IncludeLeadingZero = true);
	
};

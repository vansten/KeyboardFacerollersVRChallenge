// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorHelpers.h"



FString UKitesurfingSimulatorHelpers::FloatToStringWithPrecision(float Value, int32 Precision, bool IncludeLeadingZero /* = true */)
{
	FNumberFormattingOptions NumberFormat;					
	NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
	return FText::AsNumber(Value, &NumberFormat).ToString();
}


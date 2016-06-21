// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "Resetable.h"

UResetable::UResetable(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void IResetable::ResetObject()
{
	OnScreenMessage(-1, 5.0f, FColor::Red, FString("Base implementation of ResetObject method called"));
}



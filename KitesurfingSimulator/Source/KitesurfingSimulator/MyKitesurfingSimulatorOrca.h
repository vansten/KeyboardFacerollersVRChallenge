// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KitesurfingSimulatorFishBase.h"
#include "MyKitesurfingSimulatorOrca.generated.h"

/**
 * 
 */
UCLASS()
class KITESURFINGSIMULATOR_API AMyKitesurfingSimulatorOrca : public AKitesurfingSimulatorFishBase
{
	GENERATED_BODY()
	
protected:
	UAnimInstance* _animInstance;
	UBoolProperty* _jumpingProperty;
	float _timeToJump;
	bool _bSwimming;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	void Jump();
};

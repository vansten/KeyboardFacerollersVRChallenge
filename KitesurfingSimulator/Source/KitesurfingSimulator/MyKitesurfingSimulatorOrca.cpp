// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "MyKitesurfingSimulatorOrca.h"

void AMyKitesurfingSimulatorOrca::BeginPlay()
{
	Super::BeginPlay();
	_timeToJump = FMath::FRand() * 20.0f;
	_bSwimming = true;
	_animInstance = Mesh->GetAnimInstance();
	_zMultiplier = 0.0f;
	_jumpingProperty = FindField<UBoolProperty>(_animInstance->GetClass(), "Jumping");
}

void AMyKitesurfingSimulatorOrca::Tick(float DeltaSeconds)
{
	if (_bSwimming)
	{
		Super::Tick(DeltaSeconds);

		_timeToJump -= DeltaSeconds;
		if (_timeToJump <= 0.0f)
		{
			Jump();
		}
	}
	else
	{
		MoveInCurrentDirection(DeltaSeconds);
		_bSwimming = !_jumpingProperty->GetPropertyValue_InContainer(_animInstance);
		if (_bSwimming)
		{
			_speed *= 0.5f;
			//Spawn water particles at current actor location
		}
	}
}

void AMyKitesurfingSimulatorOrca::Jump()
{
	_speed *= 2.0f;
	_bSwimming = false;
	_jumpingProperty->SetPropertyValue_InContainer(_animInstance, true);
	_timeToJump = FMath::FRand() * 30.0f;
}
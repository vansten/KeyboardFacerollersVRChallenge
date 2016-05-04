// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "KitesurfingSimulatorCharacter.generated.h"

UCLASS(config=Game)
class AKitesurfingSimulatorCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Follow camera */
	UPROPERTY(EditAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

protected:
	float _yawRotation;
	float _prevYawRotation;
	float _minimumYaw;
	float _maximumYaw;
	float _currentSpeed;

public:
	AKitesurfingSimulatorCharacter();

	UPROPERTY(EditAnywhere, Category = Movement)
	float Speed;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	// AActor interface
	virtual void Tick(float DeltaSeconds) override;
	// End of AActor interface

	void Turn(float value);

public:
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


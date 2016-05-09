// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "../Plugins/OceanPlugin/Source/OceanPlugin/Classes/OceanManager.h"
#include "KitesurfingSimulatorCharacter.generated.h"

UCLASS(config=Game)
class AKitesurfingSimulatorCharacter : public ACharacter
{
	GENERATED_BODY()

	// Follow camera
	UPROPERTY(EditAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	// Meshes
	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Board;

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Bar;

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Lines;

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Kite;

protected:
	// Ocean manager instance
	class AOceanManager* _oceanManager;

	// Camera controls
	float _yawRotation;
	float _minimumYaw;
	float _maximumYaw;
	float _pitchRotation;
	float _minimumPitch;
	float _maximumPitch;

	// Character speed
	float _currentSpeed;
	float _minSpeed;
	float _maxSpeedMinusMinSpeed;
	float _barRollMultiplier;

	// Bar controls
	FRotator _barRotation;
	float _barMinYaw;
	float _barMaxYaw;
	const float _pitchToDirectionX = 1.0f / 80.0f;
	bool _bRotatesManually;

	// Character movement
	FVector _currentDirection;

public:
	AKitesurfingSimulatorCharacter();

	// X - min speed, Y - max speed
	UPROPERTY(EditAnywhere, Category = Movement)
		FVector SpeedConstraints;

	// Determines how quickly bar should return to 0 pitch rotation
	UPROPERTY(EditAnywhere, Category = BarControls, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float ReturnToBasePitchSpeed;

protected:
	// Overriden functions

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// End of overriden functions

	// Input handlers
	
	void Turn(float value);
	void LookUp(float value);

	void TiltBarHorizontal(float value);
	void TiltBarVertical(float value);

	// End of input handlers

public:
	// Returns FollowCamera subobject
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


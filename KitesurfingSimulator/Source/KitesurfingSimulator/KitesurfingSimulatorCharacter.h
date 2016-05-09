// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "../Plugins/OceanPlugin/Source/OceanPlugin/Classes/OceanManager.h"
#include "KitesurfingSimulatorCharacter.generated.h"

UCLASS(config=Game)
class AKitesurfingSimulatorCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Follow camera */
	UPROPERTY(EditAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Board;

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Bar;

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Lines;

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Kite;

protected:
	class AOceanManager* _oceanManager;

	//Camera controls
	float _yawRotation;
	float _prevYawRotation;
	float _minimumYaw;
	float _maximumYaw;

	//Character speed
	float _currentSpeed;
	float _minSpeed;
	float _maxSpeedMinusMinSpeed;
	float _barYawMultiplier;
	float _barRollMultiplier;

	//Bar controls
	FRotator _barRotation;
	const float _pitchToDirectionX = 1.0f / 80.0f;

	//Character movement
	FVector _currentDirection;

public:
	AKitesurfingSimulatorCharacter();

	//X - min speed, Y - max speed
	UPROPERTY(EditAnywhere, Category = Movement)
	FVector SpeedConstraints;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void BeginPlay() override;

	// AActor interface
	virtual void Tick(float DeltaSeconds) override;
	// End of AActor interface

	void Turn(float value);

	void TiltBarHorizontal(float value);
	void TiltBarVertical(float value);

public:
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


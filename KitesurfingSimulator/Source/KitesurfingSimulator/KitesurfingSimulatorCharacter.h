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
	float _barYawMultiplier;

	// Bar controls
	FRotator _barRotation;
	float _prevBarYawNormalized;
	float _currentBarYawNormalized;
	float _barYawRotation;
	const float _yawToDirectionX = -1.0f / 45.0f;
	bool _bRotatesManually;

	// Statistics
	static int32 _colaCansCollected;

	// Wiimote stuff
	FVector _prevTilt;

public:
	AKitesurfingSimulatorCharacter();

	// X - min speed, Y - max speed
	UPROPERTY(EditAnywhere, Category = Movement)
		FVector SpeedConstraints;

	// Determines how quickly bar should return to 0 yaw rotation
	UPROPERTY(EditAnywhere, Category = BarControls, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float ReturnToBaseYawSpeed;

	// Returns number of cola cans collected
	static int32 GetColaCansCollectedNumber() { return _colaCansCollected; }

protected:
	// Overriden functions

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// End of overriden functions

	// Helper functions
	
	void CollectCans();

	// End of helper functions

	// Input handlers
	
	void Turn(float value);
	void LookUp(float value);

	void TiltBarHorizontal(float value);
	void TiltBarVertical(float value);

	void Tilt(FVector tilt);

	// End of input handlers

public:
	// Returns FollowCamera subobject
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
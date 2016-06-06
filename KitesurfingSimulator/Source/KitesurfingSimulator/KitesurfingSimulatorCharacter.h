// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "../Plugins/OceanPlugin/Source/OceanPlugin/Classes/OceanManager.h"
#include "Runtime/Engine/Classes/Engine/TextRenderActor.h"

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
	float _barRollRotation;
	float _prevBarYawNormalized;
	float _currentBarYawNormalized;
	float _barYawRotation;
	const float _yawToDirectionX = -1.0f / 45.0f;

	// Statistics
	static float _timePassed;
	static int32 _colaCansCollected;

	// State
	bool _bSurfing;

	// Wiimote stuff
	float _baseYaw;
	
	// HMD stuff
	FRotator _baseRotation;
	FVector _hmdLocation; /* Unused but necessary */

public:
	AKitesurfingSimulatorCharacter();

	UPROPERTY(EditAnywhere, Category = Text)
		ATextRenderActor* TextRender;

	// X - min speed, Y - max speed
	UPROPERTY(EditAnywhere, Category = Movement)
		FVector SpeedConstraints;

	// Determinces absoulte values of x and z axis that should be dismissed to remove noise
	UPROPERTY(EditAnywhere, Category = BarControls, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Tolerance;

	UPROPERTY(EditAnywhere, Category = BarControls)
		bool bUsesWiimote;

	UPROPERTY(EditAnywhere, Category = VR)
		bool bUsesHMD;

	// Ocean manager instance (static because other classes also may want to use it)
	static class AOceanManager* OceanManager;

protected:
	// Overriden functions

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	// End of overriden functions

	// Helper functions
	
	void CollectCans();

	void UpdateTextRender(bool bCongratulations = false);

	// End of helper functions

	// Input handlers
	
	void Turn(float value);
	void LookUp(float value);

	void TiltBarHorizontal(float value);
	void TiltBarVertical(float value);

	// End of input handlers

	// State functions

	void Surf(float DeltaSeconds);

	void Party(float DeltaSeconds);

	// End of state functions

public:
	// Returns FollowCamera subobject
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Returns number of cola cans collected
	static int32 GetColaCansCollectedNumber() { return _colaCansCollected; }

	static float GetTimePassed() { return _timePassed; }

	void EndSurfing();
};
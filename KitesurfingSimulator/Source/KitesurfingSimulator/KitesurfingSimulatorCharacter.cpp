// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AKitesurfingSimulatorCharacter

AKitesurfingSimulatorCharacter::AKitesurfingSimulatorCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	 
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetEnableGravity(false);

	//Turn off gravity for mesh
	GetMesh()->SetEnableGravity(false);

	bSimGravityDisabled = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.0f;
	GetCharacterMovement()->FallingLateralFriction = 8.0f;
	GetCharacterMovement()->GravityScale = 0.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AKitesurfingSimulatorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("Turn", this, &AKitesurfingSimulatorCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	_yawRotation = GetActorRotation().Yaw;
	_prevYawRotation = _yawRotation;
	_minimumYaw = _yawRotation - 60.0f;
	_maximumYaw = _yawRotation + 60.0f;
}

void AKitesurfingSimulatorCharacter::Turn(float value)
{
	_yawRotation += value;
	_yawRotation = FMath::Clamp(_yawRotation, _minimumYaw, _maximumYaw);
	float diff = _yawRotation - _prevYawRotation;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Black, FString::SanitizeFloat(_yawRotation));
	}
	AddControllerYawInput(diff);
	_prevYawRotation = _yawRotation;
}

void AKitesurfingSimulatorCharacter::Tick(float DeltaSeconds)
{
	AddMovementInput(-FVector::RightVector, _currentSpeed * DeltaSeconds);

	if (GWorld)
	{
		_currentSpeed = Speed * FMath::Pow(FMath::Sin(GWorld->GetRealTimeSeconds()), 2);
	}
}
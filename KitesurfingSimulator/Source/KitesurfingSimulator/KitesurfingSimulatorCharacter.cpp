// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorCharacter.h"
#include "KitesurfingSimulatorPickable.h"
#include "EngineUtils.h"

//////////////////////////////////////////////////////////////////////////
// AKitesurfingSimulatorCharacter

// Static variables
int32 AKitesurfingSimulatorCharacter::_colaCansCollected = 0;

AKitesurfingSimulatorCharacter::AKitesurfingSimulatorCharacter()
{
	// Allow actor to tick
	PrimaryActorTick.bCanEverTick = true;
	 
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetEnableGravity(false);

	// Disable gravity
	bSimGravityDisabled = true;

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.0f;
	GetCharacterMovement()->FallingLateralFriction = 8.0f;
	GetCharacterMovement()->GravityScale = 0.0f;

	// Get mesh, will be needed later
	USkeletalMeshComponent* mesh = GetMesh();

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->bUsePawnControlRotation = false;

	// Create board
	Board = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board"));
	Board->AttachTo(RootComponent);
	Board->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	Board->SetRelativeRotation(FRotator(0.0f, 25.0f, 0.0f));

	// Disable gravity for mesh
	mesh->SetEnableGravity(false);
	mesh->AttachTo(RootComponent);

	FVector meshForward = mesh->GetForwardVector();

	// Create bar
	Bar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bar"));
	Bar->AttachTo(RootComponent);
	Bar->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f) + meshForward * 25.0f);

	// Create lines
	Lines = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lines"));
	Lines->AttachTo(Bar);

	// Create kite
	Kite = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Kite"));
	Kite->AttachTo(Lines);

	// Attach follow camera to mesh
	FollowCamera->AttachTo(mesh);
	FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f) + meshForward * 35.0f);
	FollowCamera->SetRelativeRotation(meshForward.Rotation());
}

void AKitesurfingSimulatorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("Turn", this, &AKitesurfingSimulatorCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &AKitesurfingSimulatorCharacter::LookUp);
	InputComponent->BindAxis("TiltBarHorizontal", this, &AKitesurfingSimulatorCharacter::TiltBarHorizontal);
	InputComponent->BindAxis("TiltBarVertical", this, &AKitesurfingSimulatorCharacter::TiltBarVertical);
}

void AKitesurfingSimulatorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Find ocean manager instance
	if (GWorld)
	{
		for (TActorIterator<AOceanManager> ActorItr(GWorld); ActorItr; ++ActorItr)
		{
			if ((*ActorItr) != NULL)
			{
				_oceanManager = (*ActorItr);
				break;
			}
		}
	}

	check(_oceanManager != NULL && "Have you placed ocean manager on map somewhere?");

	// Find camera yaw and pitch restrictions
	FRotator followCameraRotation = FollowCamera->GetComponentRotation();
	FRotator actorRotation = GetActorRotation();

	_yawRotation = followCameraRotation.Yaw - actorRotation.Yaw;
	_minimumYaw = _yawRotation - 105.0f;
	_maximumYaw = _yawRotation + 105.0f;

	_pitchRotation = followCameraRotation.Pitch - actorRotation.Pitch;
	_minimumPitch = _pitchRotation - 89.9f;
	_maximumPitch = _pitchRotation + 89.9f;

	// Set proper camera yaw and pitch rotations
	_yawRotation = FMath::Clamp(_yawRotation - 90.0f, _minimumYaw, _maximumYaw);
	followCameraRotation.Yaw = _yawRotation + GetActorRotation().Yaw;
	
	_pitchRotation = FMath::Clamp(_pitchRotation, _minimumPitch, _maximumPitch);
	followCameraRotation.Pitch = _pitchRotation;

	FollowCamera->SetWorldRotation(followCameraRotation);

	// Find speed constraints
	_minSpeed = SpeedConstraints.X;
	_maxSpeedMinusMinSpeed = SpeedConstraints.Y - _minSpeed;
	
	// Find bar rotation restrictions
	_barRotation = Bar->GetComponentRotation();
	_barRotation.Pitch = FMath::Clamp(_barRotation.Pitch, -80.0f, 80.0f);
	_barRotation.Roll = FMath::Clamp(_barRotation.Roll, -75.0f, -5.0f);
	_barRotation.Yaw = 90.0f + actorRotation.Yaw;
	Bar->SetWorldRotation(_barRotation);

	// Zero cola cans number
	_colaCansCollected = 0;
}

void AKitesurfingSimulatorCharacter::Turn(float value)
{
	if (value != 0.0f)
	{
		// Add camera yaw rotation
		_yawRotation = FMath::Clamp(_yawRotation + value, _minimumYaw, _maximumYaw);

		FRotator followCameraRotation = FollowCamera->GetComponentRotation();
		followCameraRotation.Yaw = _yawRotation + GetActorRotation().Yaw;
		FollowCamera->SetWorldRotation(followCameraRotation);
	}
}

void AKitesurfingSimulatorCharacter::LookUp(float value)
{
	if (value != 0.0f)
	{
		// Add camera pitch rotation
		_pitchRotation = FMath::Clamp(_pitchRotation + value, _minimumPitch, _maximumPitch);

		FRotator followCameraRotation = FollowCamera->GetComponentRotation();
		followCameraRotation.Pitch = _pitchRotation;
		FollowCamera->SetWorldRotation(followCameraRotation);
	}
}

void AKitesurfingSimulatorCharacter::TiltBarHorizontal(float value)
{
	if (value != 0.0f)
	{
		// Add bar pitch rotation
		_barRotation.Pitch = FMath::Clamp(_barRotation.Pitch - value * 2.0f, -80.0f, 80.0f);
		_bRotatesManually = true;
	}
	else
	{
		_bRotatesManually = false;
	}
}

void AKitesurfingSimulatorCharacter::TiltBarVertical(float value)
{
	if (value != 0.0f)
	{
		// Add bar roll rotation
		_barRotation.Roll = FMath::Clamp(_barRotation.Roll - value * 2.0f, -75.0f, -5.0f);
		_barRollMultiplier = FMath::Sin(FMath::DegreesToRadians(FMath::Abs(_barRotation.Roll)) * 2.0f);
	}
}

void AKitesurfingSimulatorCharacter::Tick(float DeltaSeconds)
{
	CollectCans();

	//Calculate bar rotation pitch normalized
	_prevBarPitchNormalized = _currentBarPitchNormalized;
	_currentBarPitchNormalized = _barRotation.Pitch * _pitchToDirectionX;

	// Calculate new actor forward vector
	FVector actorForward = GetActorForwardVector();
	actorForward.X += _currentBarPitchNormalized * DeltaSeconds;
	actorForward.Normalize();
	SetActorRotation(actorForward.Rotation());

	// Calculate bar yaw rotation and bar yaw multiplier
	_barRotation.Yaw = GetActorRotation().Yaw + 90.0f;
	_barYawMultiplier = 1.0f - (FMath::Abs(actorForward.X));

	// If bar pitch wasn't affected in this frame then return to 0.0f
	if (!_bRotatesManually)
	{
		_barRotation.Pitch = FMath::Lerp(_barRotation.Pitch, 0.0f, ReturnToBasePitchSpeed);
	}
	Bar->SetWorldRotation(_barRotation);

	// Calculate current speed depending on bar roll rotation. Sin(realTimeSeconds)^2 is supposed to simulate wind somehow
	_currentSpeed = _maxSpeedMinusMinSpeed * _barRollMultiplier * FMath::Pow(FMath::Sin(GWorld->GetRealTimeSeconds()), 2) + _minSpeed;
	_currentSpeed *= _barYawMultiplier;

	// Affect actor's location depending on waves
	if (_oceanManager)
	{
		FVector currentActorLocation = GetActorLocation();
		currentActorLocation.Z = _oceanManager->GetWaveHeightValue(currentActorLocation, GWorld, true, true).Z + 90.0f;
		SetActorLocation(currentActorLocation);
	}

	// Add speed in real direction
	AddMovementInput(actorForward, _currentSpeed * DeltaSeconds);

	// Debug messages
	OnScreenMessage(3, 5.0f, FColor::Green, FString("Actor forward: ").Append(actorForward.ToString()));
	OnScreenMessage(1, 5.0f, FColor::Red, FString("Current speed: ").Append(FString::SanitizeFloat(_currentSpeed)));
}

void AKitesurfingSimulatorCharacter::CollectCans()
{
	TArray<AActor*> OverlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, AKitesurfingSimulatorPickable::StaticClass());

	AKitesurfingSimulatorPickable* pickable = NULL;
	int32 size = OverlappingActors.Num();
	for (int32 i = 0; i < size; ++i)
	{
		pickable = Cast<AKitesurfingSimulatorPickable>(OverlappingActors[i]);
		if (pickable != NULL)
		{
			_colaCansCollected += 1;
			pickable->Collect();
		}
	}

	OnScreenMessage(4, 5.0f, FColor::Black, FString("Cola cans collected: ").Append(FString::FromInt(GetColaCansCollectedNumber())));
}
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorCharacter.h"
#include "EngineUtils.h"

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

	Board = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board"));
	Board->AttachTo(RootComponent);

	Lines = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lines"));
	Lines->AttachTo(RootComponent);

	Kite = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Kite"));
	Kite->AttachTo(Lines);

	GetMesh()->AttachTo(Board);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
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
	_minimumYaw = 45.0f;
	_maximumYaw = 135.0f;
}

void AKitesurfingSimulatorCharacter::BeginPlay()
{
	Super::BeginPlay();

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

	_twoThirdSpeed = 2.0f * Speed / 3.0f;
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

	_currentSpeed = _twoThirdSpeed * FMath::Pow(FMath::Sin(GWorld->GetRealTimeSeconds()), 2) + 0.5f * _twoThirdSpeed;

	if (_oceanManager)
	{
		FVector actorLocation = GetActorLocation();
		actorLocation.Z = _oceanManager->GetWaveHeightValue(actorLocation, GWorld, true, true).Z + 90.0f;
		SetActorLocation(actorLocation);
	}
}
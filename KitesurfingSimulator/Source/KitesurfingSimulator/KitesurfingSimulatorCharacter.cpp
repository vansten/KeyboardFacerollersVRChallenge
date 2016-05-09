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

	//Get mesh, will be needed later
	USkeletalMeshComponent* mesh = GetMesh();

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	Board = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board"));
	Board->AttachTo(RootComponent);
	Board->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	Board->SetRelativeRotation(FRotator(0.0f, 25.0f, 0.0f));

	mesh->SetEnableGravity(false);
	mesh->AttachTo(Board);
	//mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

	FVector meshForward = mesh->GetForwardVector();

	Bar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bar"));
	Bar->AttachTo(RootComponent);
	Bar->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f) + meshForward * 25.0f);

	Lines = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lines"));
	Lines->AttachTo(Bar);

	Kite = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Kite"));
	Kite->AttachTo(Lines);

	FollowCamera->AttachTo(mesh);
	FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f) + meshForward * 35.0f);
	FollowCamera->SetRelativeRotation(meshForward.Rotation());
}

//////////////////////////////////////////////////////////////////////////
// Input

void AKitesurfingSimulatorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("Turn", this, &AKitesurfingSimulatorCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("TiltBarHorizontal", this, &AKitesurfingSimulatorCharacter::TiltBarHorizontal);
	InputComponent->BindAxis("TiltBarVertical", this, &AKitesurfingSimulatorCharacter::TiltBarVertical);

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

	_minSpeed = SpeedConstraints.X;
	_maxSpeedMinusMinSpeed = SpeedConstraints.Y - _minSpeed;
	
	_barRotation = Bar->GetComponentRotation();
	_barRotation.Pitch = FMath::Clamp(_barRotation.Pitch, -80.0f, 80.0f);
	_barRotation.Roll = FMath::Clamp(_barRotation.Roll, 5.0f, 85.0f);
	Bar->SetWorldRotation(_barRotation);

	_barYawMultiplier = 1.0f;
	_currentDirection = -FVector::RightVector;
}

void AKitesurfingSimulatorCharacter::Turn(float value)
{
	_yawRotation += value;
	_yawRotation = FMath::Clamp(_yawRotation, _minimumYaw, _maximumYaw);
	
	AddControllerYawInput(_yawRotation - _prevYawRotation);
	_prevYawRotation = _yawRotation;
}

void AKitesurfingSimulatorCharacter::TiltBarHorizontal(float value)
{
	_barRotation.Pitch = FMath::Clamp(_barRotation.Pitch + value, -80.0f, 80.0f);
	Bar->SetWorldRotation(_barRotation);
}

void AKitesurfingSimulatorCharacter::TiltBarVertical(float value)
{
	_barRotation.Roll = FMath::Clamp(_barRotation.Roll + value, 5.0f, 85.0f);
	Bar->SetWorldRotation(_barRotation);

	_barRollMultiplier = FMath::Sin(FMath::DegreesToRadians(_barRotation.Roll) * 2.0f);
}

void AKitesurfingSimulatorCharacter::Tick(float DeltaSeconds)
{
	_currentDirection.X = _barRotation.Pitch * _pitchToDirectionX;
	_currentDirection.Y = -1.0f;
	_currentDirection.Z = 0.0f;
	_currentDirection.Normalize();

	OnScreenMessage(0, 5.0f, FColor::Blue, FString("Current direction: ").Append(_currentDirection.ToString()));
	AddMovementInput(_currentDirection, _currentSpeed * DeltaSeconds);

	_currentSpeed = _maxSpeedMinusMinSpeed * _barRollMultiplier * _barYawMultiplier * FMath::Pow(FMath::Sin(GWorld->GetRealTimeSeconds()), 2) + _minSpeed;

	OnScreenMessage(1, 5.0f, FColor::Red, FString("Current speed: ").Append(FString::SanitizeFloat(_currentSpeed)));

	if (_oceanManager)
	{
		FVector actorLocation = GetActorLocation();
		actorLocation.Z = _oceanManager->GetWaveHeightValue(actorLocation, GWorld, true, true).Z + 90.0f;
		SetActorLocation(actorLocation);
	}
}
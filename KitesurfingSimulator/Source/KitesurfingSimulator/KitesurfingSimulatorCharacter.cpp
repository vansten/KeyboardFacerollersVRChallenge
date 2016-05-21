// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorCharacter.h"
#include "KitesurfingSimulatorPickable.h"
#include "EngineUtils.h"

// Wiimote include
#include "../Plugins/Wiimote/Source/Wiimote/Public/WiimoteFunctionLibrary.h"

// Helpers include
#include "KitesurfingSimulatorHelpers.h"

//////////////////////////////////////////////////////////////////////////
// AKitesurfingSimulatorCharacter

// Static variables
int32 AKitesurfingSimulatorCharacter::_colaCansCollected = 0;
float AKitesurfingSimulatorCharacter::_timePassed = 0.0f;

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
	FollowCamera->SetRelativeRotation(GetActorForwardVector().Rotation());

	UWiimoteFunctionLibrary::SetMotionSensingEnabled(0, true);
	UWiimoteFunctionLibrary::SetMotionPlusEnabled(0, true);
}

void AKitesurfingSimulatorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("Turn", this, &AKitesurfingSimulatorCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &AKitesurfingSimulatorCharacter::LookUp);
	
	if (bUsesWiimote)
	{
		InputComponent->BindVectorAxis(EKeys::Tilt, this, &AKitesurfingSimulatorCharacter::Tilt);
		InputComponent->BindVectorAxis(EKeys::RotationRate, this, &AKitesurfingSimulatorCharacter::RotationRate);
	}
	else
	{
		InputComponent->BindAxis("TiltBarHorizontal", this, &AKitesurfingSimulatorCharacter::TiltBarHorizontal);
		InputComponent->BindAxis("TiltBarVertical", this, &AKitesurfingSimulatorCharacter::TiltBarVertical);
	}
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

		for (TActorIterator<ATextRenderActor> Itr(GWorld); Itr; ++Itr)
		{
			TextRender = (*Itr);
			if (TextRender != NULL)
			{
				break;
			}
		}
	}

	check(_oceanManager != NULL && "Have you placed ocean manager on map somewhere?");
	check(TextRender != NULL && "Have you placed actor with TextRender component on map somewhere?");

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
	_yawRotation = FMath::Clamp(_yawRotation - 75.0f, _minimumYaw, _maximumYaw);
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
	_barRotation.Roll = -65.0f;
	_barRollRotation = -65.0f;
	_barRollMultiplier = FMath::Sin(FMath::DegreesToRadians(FMath::Abs(_barRotation.Roll)) * 2.0f);
	_barYawRotation = 0.0f;
	Bar->SetWorldRotation(_barRotation);

	// Zero cola cans number && timer
	_colaCansCollected = 0;
	_timePassed = 0.0f;

	// Start surfing
	_bSurfing = true;
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
		_barYawRotation = FMath::Clamp(_barYawRotation + value * 2.0f, -45.0f, 45.0f);
	}
}

void AKitesurfingSimulatorCharacter::TiltBarVertical(float value)
{
	if (value != 0.0f)
	{
		// Add bar roll rotation
		_barRollRotation = FMath::Clamp(_barRollRotation - value * 2.0f, -75.0f, -5.0f);
		_barRotation.Roll = _barRollRotation;
		_barRollMultiplier = FMath::Sin(FMath::DegreesToRadians(FMath::Abs(_barRotation.Roll)) * 2.0f);
	}
}

void AKitesurfingSimulatorCharacter::Tick(float DeltaSeconds)
{
	if (_bSurfing)
	{
		Surf(DeltaSeconds);
	}
	else
	{
		Party(DeltaSeconds);
	}
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

	UpdateTextRender();
}

void AKitesurfingSimulatorCharacter::Surf(float DeltaSeconds)
{
	_timePassed += DeltaSeconds;

	CollectCans();

	float barYawClamped = FMath::Clamp(_barYawRotation, -45.0f, 45.0f);

	//Calculate bar rotation pitch normalized
	_prevBarYawNormalized = _currentBarYawNormalized;
	_currentBarYawNormalized = barYawClamped * _yawToDirectionX;

	// Calculate new actor forward vector
	FVector actorForward = GetActorForwardVector();
	actorForward.X += _currentBarYawNormalized * DeltaSeconds;
	actorForward.Normalize();
	SetActorRotation(actorForward.Rotation());

	_barYawMultiplier = 1.0f - (FMath::Abs(actorForward.X));

	_barRotation.Yaw = barYawClamped + GetActorRotation().Yaw + 90.0f;
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

void AKitesurfingSimulatorCharacter::Party(float DeltaSeconds)
{
	OnScreenMessage(0, 5.0f, FColor::Blue, FString("It's time to party"));
}

void AKitesurfingSimulatorCharacter::Tilt(FVector tilt)
{
	_tilt = tilt;

	_barRollRotation = -_tilt.X - 90.0f;
	_barRotation.Roll = FMath::Clamp(_barRollRotation, -75.0f, -5.0f);
	_barRollMultiplier = FMath::Sin(FMath::DegreesToRadians(FMath::Abs(_barRotation.Roll)) * 2.0f);
}

void AKitesurfingSimulatorCharacter::RotationRate(FVector rotationRate)
{
	static float normalize = 1.0f / 180.0f;
	float z = -(rotationRate.Z) * normalize;
	
	if (FMath::Abs(z) < Tolerance)
	{
		z = 0.0f;
	}

	if (_barRotation.Roll < -50.0f)
	{
		z *= -(_barRotation.Roll + 50.0f) * 0.3f;
	}

	z *= -FMath::Sign(_tilt.X);
	_barYawRotation += z;
}

void AKitesurfingSimulatorCharacter::EndSurfing()
{
	_bSurfing = false;

	GetMesh()->AttachTo(RootComponent);
	Board->SetHiddenInGame(true);
	Bar->SetHiddenInGame(true);
	Lines->SetHiddenInGame(true);
	Kite->SetHiddenInGame(true);

	UpdateTextRender(true);
}

void AKitesurfingSimulatorCharacter::UpdateTextRender(bool bCongratulations /* = false */)
{
	FString text = FString("Collected ");
	text.Append(FString::FromInt(GetColaCansCollectedNumber()));
	text.Append(" cans");
	text.Append(FString("<br>"));
	text.Append(FString("Time: "));
	text.Append(UKitesurfingSimulatorHelpers::FloatToStringWithPrecision(GetTimePassed(), 2));
	text.Append(" ms");
	if (bCongratulations)
	{
		text.Append(FString("<br>Congratulations!"));
	}

	if (TextRender != NULL)
	{
		TextRender->GetTextRender()->SetText(text);
	}
}
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorCharacter.h"
#include "KitesurfingSimulatorPickable.h"
#include "EngineUtils.h"

// HMD include
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"

// Helpers include
#include "KitesurfingSimulatorHelpers.h"

// Wiimote include
#include "WiimoteBlueprintLibrary.h"

//////////////////////////////////////////////////////////////////////////
// AKitesurfingSimulatorCharacter

// Static variables
int32 AKitesurfingSimulatorCharacter::_colaCansCollected = 0;
float AKitesurfingSimulatorCharacter::_timePassed = 0.0f;
AOceanManager* AKitesurfingSimulatorCharacter::OceanManager = NULL;

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
}

void AKitesurfingSimulatorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	
	InputComponent->BindAxis("Turn", this, &AKitesurfingSimulatorCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &AKitesurfingSimulatorCharacter::LookUp);

	if (bUsesWiimote)
	{
		bUsesWiimote = UWiimoteBlueprintLibrary::Connect();
		if (bUsesWiimote)
		{
			_baseYaw = UWiimoteBlueprintLibrary::GetMotionPlusSpeed().Pitch;
		}
	}

	if (!bUsesWiimote)
	{
		InputComponent->BindAxis("TiltBarHorizontal", this, &AKitesurfingSimulatorCharacter::TiltBarHorizontal);
		InputComponent->BindAxis("TiltBarVertical", this, &AKitesurfingSimulatorCharacter::TiltBarVertical);
		OnScreenMessage(98765, 10.0f, FColor::Red, FString("Wiimote not connected"));
	}
}

void AKitesurfingSimulatorCharacter::BeginPlay()
{
	Super::BeginPlay();

	Setup();

	FRotator followCameraRotation = FollowCamera->GetComponentRotation();
	FRotator actorRotation = GetActorRotation();

	_initRotation = actorRotation;
	_initLocation = GetActorLocation();
	_initScale = GetActorScale();

	_yawRotation = followCameraRotation.Yaw - actorRotation.Yaw;
	_minimumYaw = _yawRotation - 105.0f;
	_maximumYaw = _yawRotation + 105.0f;

	_pitchRotation = followCameraRotation.Pitch - actorRotation.Pitch;
	_minimumPitch = _pitchRotation - 89.9f;
	_maximumPitch = _pitchRotation + 89.9f;

	// Set proper camera yaw and pitch rotations
	_yawRotation = FMath::Clamp(_yawRotation - 75.0f, _minimumYaw, _maximumYaw);
	followCameraRotation.Yaw = _yawRotation + GetActorRotation().Yaw;
	FollowCamera->SetWorldRotation(followCameraRotation);
}

void AKitesurfingSimulatorCharacter::Setup()
{
	if (bUsesHMD)
	{
		if (GEngine->HMDDevice.IsValid())
		{
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(_baseRotation, _hmdLocation);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("You're trying to use HMD and didn't connect one. Are you stupid or something? HMD functionality disabled"));
			bUsesHMD = false;
		}
	}

	// Find camera yaw and pitch restrictions
	FRotator followCameraRotation = FollowCamera->GetComponentRotation();
	FRotator actorRotation = GetActorRotation();

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

	// Set components visible
	Board->SetHiddenInGame(false);
	Bar->SetHiddenInGame(false);
	Lines->SetHiddenInGame(false);
	Kite->SetHiddenInGame(false);

}

void AKitesurfingSimulatorCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UWiimoteBlueprintLibrary::Disconnect();
	OceanManager = NULL;
	TextRender = NULL;
}

void AKitesurfingSimulatorCharacter::Turn(float value)
{
	if (bUsesHMD)
	{
		return;
	}

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
	if (bUsesHMD)
	{
		return;
	}

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
	if (bUsesHMD)
	{
		FRotator rotation;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(rotation, _hmdLocation);
		
		float add = GetActorRotation().Yaw + 45.0f;
		rotation.Yaw += add;
		FollowCamera->SetWorldRotation(rotation);
	}

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
	if (bUsesWiimote)
	{
		static float normalize = 1.0f / 90.0f;
		float toAdd = (UWiimoteBlueprintLibrary::GetMotionPlusSpeed().Pitch - _baseYaw) * normalize;
		if (FMath::Abs(toAdd) < Tolerance)
		{
			toAdd = 0.0f;
		}
		_barYawRotation += toAdd;
	}

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
	if (OceanManager != NULL)
	{
		FVector currentActorLocation = GetActorLocation();
		currentActorLocation.Z = OceanManager->GetWaveHeightValue(currentActorLocation, GWorld, true, true).Z + 90.0f;
		SetActorLocation(currentActorLocation);
	}
	else
	{
		for (TActorIterator<AOceanManager> ActorItr(GWorld); ActorItr; ++ActorItr)
		{
			if ((*ActorItr) != NULL)
			{
				OceanManager = (*ActorItr);
				break;
			}
		}
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

void AKitesurfingSimulatorCharacter::EndSurfing()
{
	_bSurfing = false;

	Board->SetHiddenInGame(true);
	Bar->SetHiddenInGame(true);
	Lines->SetHiddenInGame(true);
	Kite->SetHiddenInGame(true);

	UpdateTextRender(true);
}

void AKitesurfingSimulatorCharacter::UpdateTextRender(bool bCongratulations /* = false */)
{
	if (TextRender == NULL)
	{
		for (TActorIterator<ATextRenderActor> Itr(GWorld); Itr; ++Itr)
		{
			TextRender = (*Itr);
			if (TextRender != NULL)
			{
				break;
			}
		}
	}

	FString text = FString("Collected ");
	text.Append(FString::FromInt(GetColaCansCollectedNumber()));
	text.Append(" cans");
	text.Append(FString("<br>"));
	text.Append(FString("Time: "));
	text.Append(UKitesurfingSimulatorHelpers::FloatToStringWithPrecision(GetTimePassed(), 2));
	text.Append(" s");
	if (bCongratulations)
	{
		text.Append(FString("<br>Congratulations!"));
	}

	if (TextRender != NULL)
	{
		TextRender->GetTextRender()->SetText(FText::FromString(text));
	}
}

void AKitesurfingSimulatorCharacter::ResetObject()
{
	SetActorLocation(_initLocation);
	SetActorRotation(_initRotation);
	SetActorScale3D(_initScale);
	Setup();
}
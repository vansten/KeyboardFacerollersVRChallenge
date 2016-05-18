// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorFishBase.h"


// Sets default values
AKitesurfingSimulatorFishBase::AKitesurfingSimulatorFishBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->AttachTo(RootComponent);
	Mesh->SetRelativeRotation(FVector(0.0f, 90.0f, 0.0f).Rotation().Quaternion());
}

void AKitesurfingSimulatorFishBase::BeginPlay()
{
	Super::BeginPlay();

	check(SkeletalMeshes.Num() > 0 && "SkeletalMeshes property wasn't set");
	int32 randomMesh = FMath::Rand() % SkeletalMeshes.Num();
	Mesh->SetSkeletalMesh(SkeletalMeshes[randomMesh]);

	_speed = FMath::FRandRange(SpeedMin, SpeedMax);
	_lerpRate = 0.0f;
	_timeToCalculateTargetDirection = FMath::SRand() * 10.0f;
	CalculateTargetDirection();
	_zMultiplier = 0.05f;
}

// Called every frame
void AKitesurfingSimulatorFishBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	_lerpRate += DeltaTime;
	_timeToCalculateTargetDirection -= DeltaTime;

	_currentDirection = FMath::Lerp(_currentInitDirection, _currentTargetDirection, _lerpRate);
	SetActorRotation(_currentDirection.Rotation());

	MoveInCurrentDirection(DeltaTime);

	if (_timeToCalculateTargetDirection <= 0.0f)
	{
		CalculateTargetDirection();
	}
}

void AKitesurfingSimulatorFishBase::CalculateTargetDirection()
{
	_currentInitDirection = GetActorForwardVector();
	float xDiff = FMath::SRand() - 0.5f;
	xDiff *= 0.5f;
	float yDiff = FMath::SRand() - 0.5f;
	yDiff *= 0.5f;
	FVector actorLocation = GetActorLocation();
	float zDiff = 0.0f;
	if (actorLocation.Z > 140.0f)
	{
		zDiff = -FMath::SRand();
	}
	else if (actorLocation.Z < 60.0f)
	{
		zDiff = FMath::SRand();
	}
	else
	{
		zDiff = FMath::SRand() - 0.5f;
	}
	zDiff *= _zMultiplier;
	_currentTargetDirection = _currentInitDirection;
	_currentTargetDirection.X += xDiff;
	_currentTargetDirection.Y += yDiff;
	_currentTargetDirection.Z += zDiff;
	_currentTargetDirection.Normalize();
	_timeToCalculateTargetDirection = FMath::SRand() * 10.0f;
	_lerpRate = 0.0f;
}

void AKitesurfingSimulatorFishBase::MoveInCurrentDirection(float DeltaTime)
{
	FVector currentLocation = GetActorLocation();
	currentLocation += _currentDirection * _speed * DeltaTime;
	currentLocation.Z = FMath::Clamp(currentLocation.Z, 40.0f, 160.0f);
	SetActorLocation(currentLocation);
}
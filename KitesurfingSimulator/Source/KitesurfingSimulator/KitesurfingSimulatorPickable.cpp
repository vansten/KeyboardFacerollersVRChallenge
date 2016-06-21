// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorPickable.h"
#include "KitesurfingSimulatorCharacter.h"


// Sets default values
AKitesurfingSimulatorPickable::AKitesurfingSimulatorPickable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionComponent->SetSphereRadius(30.0f);
	RootComponent = CollisionComponent;

	// Create sphere
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlueSphere"));
	Sphere->AttachTo(RootComponent);

	// Create can object
	ColaCan = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cola can"));
	ColaCan->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AKitesurfingSimulatorPickable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKitesurfingSimulatorPickable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (AKitesurfingSimulatorCharacter::OceanManager)
	{
		FVector currentActorLocation = GetActorLocation();
		currentActorLocation.Z = AKitesurfingSimulatorCharacter::OceanManager->GetWaveHeightValue(currentActorLocation, GWorld, true, true).Z - 50.0f;
		SetActorLocation(currentActorLocation);
	}
}

void AKitesurfingSimulatorPickable::Collect()
{
	// Destroy object
	Destroy();
}
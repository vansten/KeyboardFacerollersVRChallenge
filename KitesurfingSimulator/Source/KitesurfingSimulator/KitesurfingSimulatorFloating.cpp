// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorFloating.h"
#include "KitesurfingSimulatorCharacter.h"


// Sets default values
AKitesurfingSimulatorFloating::AKitesurfingSimulatorFloating()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKitesurfingSimulatorFloating::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKitesurfingSimulatorFloating::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (AKitesurfingSimulatorCharacter::OceanManager)
	{
		FVector currentActorLocation = GetActorLocation();
		currentActorLocation.Z = AKitesurfingSimulatorCharacter::OceanManager->GetWaveHeightValue(currentActorLocation, GWorld, true, true).Z + 20.0f;
		SetActorLocation(currentActorLocation);
	}
}


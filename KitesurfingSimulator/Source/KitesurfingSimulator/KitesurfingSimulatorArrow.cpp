// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorArrow.h"


// Sets default values
AKitesurfingSimulatorArrow::AKitesurfingSimulatorArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AKitesurfingSimulatorArrow::BeginPlay()
{
	Super::BeginPlay();
	
	_baseLocation = GetActorLocation();
}

// Called every frame
void AKitesurfingSimulatorArrow::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector offset = FVector(0.0f, 0.0f, FMath::Sin(GWorld->GetRealTimeSeconds() * Speed) * Amplitude);
	SetActorLocation(_baseLocation + offset);
	
	FRotator rotation = GetActorRotation();
	rotation.Yaw += DeltaTime * 60.0f;
	SetActorRotation(rotation);

}

void AKitesurfingSimulatorArrow::ResetObject()
{
	SetActorHiddenInGame(false);
}
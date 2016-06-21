// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "KitesurfingSimulatorPickable.generated.h"

UCLASS(Blueprintable)
class KITESURFINGSIMULATOR_API AKitesurfingSimulatorPickable : public AActor
{
	GENERATED_BODY()

	//Collision component
	UPROPERTY(VisibleDefaultsOnly, Category = Collision)
		USphereComponent* CollisionComponent;

	// Meshes
	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Sphere;
	
	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* ColaCan;

public:	
	// Sets default values for this actor's properties
	AKitesurfingSimulatorPickable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called when collected by player
	void Collect();
};

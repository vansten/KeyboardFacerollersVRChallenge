// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "KitesurfingSimulatorArrow.generated.h"

UCLASS()
class KITESURFINGSIMULATOR_API AKitesurfingSimulatorArrow : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Meshes)
		UStaticMeshComponent* Mesh;
	
private:
	FVector _baseLocation;

public:	
	// Sets default values for this actor's properties
	AKitesurfingSimulatorArrow();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = Movement)
		float Speed;

	UPROPERTY(EditAnywhere, Category = Movement)
		float Amplitude;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "KitesurfingSimulatorFishBase.generated.h"

UCLASS()
class KITESURFINGSIMULATOR_API AKitesurfingSimulatorFishBase : public AActor
{
	GENERATED_BODY()
		
protected:
	FVector _currentInitDirection;
	FVector _currentTargetDirection;
	FVector _currentDirection;
	float _lerpRate;
	float _timeToCalculateTargetDirection;
	float _speed;
	float _zMultiplier;

public:
	UPROPERTY(EditAnywhere, Category = Mesh)
	TArray<USkeletalMesh*> SkeletalMeshes;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category = Movement)
	float SpeedMin;

	UPROPERTY(EditAnywhere, Category = Movement)
	float SpeedMax;

protected:
	void CalculateTargetDirection();
	void MoveInCurrentDirection(float DeltaTime);

public:	
	// Sets default values for this actor's properties
	AKitesurfingSimulatorFishBase();

	// Called on begin play
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
};

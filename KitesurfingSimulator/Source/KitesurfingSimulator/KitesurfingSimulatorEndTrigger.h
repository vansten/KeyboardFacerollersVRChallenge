// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "KitesurfingSimulatorArrow.h"
#include "KitesurfingSimulatorEndTrigger.generated.h"

UCLASS()
class KITESURFINGSIMULATOR_API AKitesurfingSimulatorEndTrigger : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Root)
		USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = CollisionComponent)
		UBoxComponent* BoxCollision;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = References)
		AKitesurfingSimulatorArrow* Arrow;
	
public:	
	// Sets default values for this actor's properties
	AKitesurfingSimulatorEndTrigger();

	virtual void BeginPlay() override;

	UFUNCTION()
		void OnBeginOverlap(class AActor* otherActor, class UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bSweep, const FHitResult& hitResult);
};

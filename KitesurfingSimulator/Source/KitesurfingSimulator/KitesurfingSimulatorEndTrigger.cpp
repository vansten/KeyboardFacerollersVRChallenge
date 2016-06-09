// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "KitesurfingSimulatorEndTrigger.h"

#include "KitesurfingSimulatorCharacter.h"

// Sets default values
AKitesurfingSimulatorEndTrigger::AKitesurfingSimulatorEndTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collision component"));
	BoxCollision->AttachTo(RootComponent);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AKitesurfingSimulatorEndTrigger::OnBeginOverlap);
}

void AKitesurfingSimulatorEndTrigger::OnBeginOverlap(AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bSweep, const FHitResult& hitResult)
{
	AKitesurfingSimulatorCharacter* player = Cast<AKitesurfingSimulatorCharacter>(otherActor);
	if (player != NULL)
	{
		UCapsuleComponent* playerCapsule = Cast<UCapsuleComponent>(otherComponent);
		if (playerCapsule != NULL)
		{
			player->EndSurfing();

			for (TActorIterator<AKitesurfingSimulatorArrow> Itr(GetWorld()); Itr; ++Itr)
			{
				Arrow = (*Itr);
				if (Arrow != NULL)
				{
					break;
				}
			}

			if (Arrow != NULL)
			{
				Arrow->SetActorHiddenInGame(true);
			}
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "RoomOverlap.generated.h"

/**
 * 
 */
UCLASS()
class AGP_API ARoomOverlap : public APickupBase
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;

	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									 UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
									 const FHitResult& SweepResult) override;
};

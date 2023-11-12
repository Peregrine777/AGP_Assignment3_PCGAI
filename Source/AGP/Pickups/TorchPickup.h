﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "TorchPickup.generated.h"
class ABaseCharacter;

UCLASS()
class AGP_API ATorchPickup : public APickupBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATorchPickup();

	virtual bool ShouldTickIfViewportsOnly() const override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsLit = true;
	void SetTorchLit(bool bLit);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHeld = false;


	void OnPickedUp(ABaseCharacter* BaseCharacter);

	void OnInteract();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							 UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
							 const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnProximityExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

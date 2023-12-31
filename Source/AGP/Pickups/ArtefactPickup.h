// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "ArtefactPickup.generated.h"


class ABaseCharacter;
/**
 * 
 */
UCLASS()
class AGP_API AArtefactPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AArtefactPickup();

	void AttemptPickUp(ABaseCharacter* BaseCharacter);

	//Int between 1 and 4 for ArtefactID
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int ArtefactID = 1;

	int GetArtefactID();
	void SetArtefactID(int ID);


	UFUNCTION(BlueprintCallable)
	void SetIsPickup(bool bPickup);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Int between 1 and 4 for ArtefactID
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsPickup = true;
	
	void DestroyArtefact();

protected:

	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						 UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
						 const FHitResult& SweepResult) override;


	UFUNCTION(Server, Reliable)
	void ServerDestroyArtefact(AArtefactPickup* ArtefactPickup);
	
	void OnPickedUp(ABaseCharacter* BaseCharacter);



	
};

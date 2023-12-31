// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/Character.h"
#include "../Pathfinding/Subsystems/PathfindingSubsystem.h"
#include "../Landscape/ProceduralCaveGen.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType) // Allows us to use this enum in blueprints.
enum class EEnemyState : uint8
{
	Patrol,
	// This essentially gets set to 0

	Engage,
	// set to 1

	Investigate,

	Ambush,

	Protect,
};

UCLASS()
class AGP_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();
	
	UPROPERTY(EditAnywhere, Category = "Target")
	int32 DefaultTargetIndex = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void FindPlayerCharacters();
	int32 OtherTargetPlayerIndex();

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	UPathfindingSubsystem* PathfindingSubsystem;
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> CurrentPath;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY()
	APlayerCharacter* TargetPlayer;
	UPROPERTY(VisibleAnywhere)
	APlayerCharacter* SensedCharacter = nullptr;
	UPROPERTY(VisibleAnywhere)
	ANavigationNode* EndNode;

	UPROPERTY(EditAnywhere)
	EEnemyState CurrentState = EEnemyState::Patrol;

	void MoveAlongPath();

	void TickPatrol();
	void TickEngage();
	void TickInvestigate();
	void TickAmbush();
	void TickProtect();

	UFUNCTION()
	virtual void OnSensedPawn(APawn* Pawn);

	// Array to hold references to players
	UPROPERTY()
	TArray<APlayerCharacter*> PlayersArray;
	// Field to determine which player to target (0 or 1)
	UPROPERTY()
	int32 TargetPlayerIndex;

	void UpdateSight();

	UPROPERTY()
	TArray<ANavigationNode*> WalkableNodes;
	UPROPERTY() 
	TArray<ANavigationNode*> RoomNodes;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

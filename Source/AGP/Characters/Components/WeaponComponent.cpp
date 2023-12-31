﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "../BaseCharacter.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UWeaponComponent::Fire(const FVector& BulletStart, const FVector& FireAtLocation)
{
	if (TimeSinceLastShot < WeaponStats.FireRate || RoundsRemainingInMagazine <= 0)
	{
		return false;
	}

	if (bIsReloading)
	{
		UE_LOG(LogTemp, Display, TEXT("Unable to Fire! Reloading!"));
		return false;
	}

	FHitResult HitResult;
	const FVector StartLocation = BulletStart;
	const FVector FireDirection = (FireAtLocation - StartLocation).GetSafeNormal();
	// Calculate the initial firing direction

	// Calculate a random deviation within the accuracy cone
	const float HalfConeAngle = FMath::DegreesToRadians(90.0f * (1.0f - WeaponStats.Accuracy));
	// Adjust the 90.0f value as needed
	const FVector RandomDeviation = FMath::VRandCone(FireDirection, HalfConeAngle);
	float MaxRange = 10000.0f;

	const FVector EndLocation = StartLocation + (RandomDeviation * MaxRange);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, QueryParams);

	UE_LOG(LogTemp, Display, TEXT("Fire!"));
	if (AActor* HitActor = HitResult.GetActor())
	{
		if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(HitActor))
		{
			// The hit result actor is of type ABaseCharacter
			// Draw a green debug line
			DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Green, false, 1.0f, 0, 1.0f);
			HitActor->GetComponentByClass<UHealthComponent>()->ApplyDamage(WeaponStats.BaseDamage);
		}
		else
		{
			// The hit result actor is NOT of type ABaseCharacter
			// Draw an orange debug line
			DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Orange, false, 1.0f, 0, 1.0f);
		}
	}
	else
	{
		// The hit result actor is null
		// Draw a red debug line
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
	}

	RoundsRemainingInMagazine -= 1;
	TimeSinceLastShot = 0.0f;
	return true;
}

bool UWeaponComponent::Reload()
{
	//Can put in logic for when gun can't be reloaded here.
	TimeSinceReload = 0.0f;
	RoundsRemainingInMagazine = WeaponStats.MagazineSize;
	return true;
}

int32 UWeaponComponent::GetRoundsRemainingInMagazine() const
{
	return RoundsRemainingInMagazine;
}

float UWeaponComponent::GetReloadTime() const
{
	return WeaponStats.ReloadTime;
}

void UWeaponComponent::ApplyWeaponStats(const FWeaponStats& NewWeaponStats)
{
	WeaponStats = NewWeaponStats;
	RoundsRemainingInMagazine = WeaponStats.MagazineSize;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsReloading)
	{
		TimeSinceReload += DeltaTime;
	}

	TimeSinceLastShot += DeltaTime;
}

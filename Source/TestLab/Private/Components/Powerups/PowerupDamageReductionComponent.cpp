// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Powerups/PowerupDamageReductionComponent.h"


UPowerupDamageReductionComponent::UPowerupDamageReductionComponent()
{
	// Class values
	DamageReductionMultiplier = 2.0f;
	DamageReductionReverseMultiplier = 1.0f / DamageReductionMultiplier;

	// Derived values
	TimeBetweenTicks = 30.0f;

	SetHaveTimeAfterLastTick(true);
}

void UPowerupDamageReductionComponent::OnPowerupTick()
{
	Super::OnPowerupTick();
	
//	DamageTakenReduction(DamageReductionMultiplier);
}

void UPowerupDamageReductionComponent::OnPowerupExpire()
{
	Super::OnPowerupExpire();
	
//	DamageTakenReduction(DamageReductionReverseMultiplier);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Powerups/PowerupQuadDamageComponent.h"


UPowerupQuadDamageComponent::UPowerupQuadDamageComponent()
{
	// Class Values
	DamageUpMultiplier = 4.0f;
	DamageDownMultiplier = 1.0f / DamageUpMultiplier;

	// Derived values
	TimeBetweenTicks = 10.0f;
	
	SetHaveTimeAfterLastTick(true);
}

void UPowerupQuadDamageComponent::OnPowerupTick()
{
	Super::OnPowerupTick();
	
//	DamageGivenIncrease(DamageUpMultiplier);
}

void UPowerupQuadDamageComponent::OnPowerupExpire()
{
	Super::OnPowerupExpire();
	
//	DamageGivenIncrease(DamageDownMultiplier);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Powerups/PowerupHealthRegenComponent.h"

UPowerupHealthRegenComponent::UPowerupHealthRegenComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Class values
	HealthHealedPerTick = 20.0f;

	// Derived values
	NumOfTicks = 4;
	TimeBetweenTicks = 1.0f;
}

void UPowerupHealthRegenComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPowerupHealthRegenComponent::OnPowerupTick()
{
	Super::OnPowerupTick();
	
	Heal(HealthHealedPerTick);
}
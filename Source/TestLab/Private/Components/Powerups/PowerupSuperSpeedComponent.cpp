// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Powerups/PowerupSuperSpeedComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
//#include "SCharacter.h"


UPowerupSuperSpeedComponent::UPowerupSuperSpeedComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Class values
	SpeedUpMultiplier = 2.0f;
	SpeedDownMultiplier = 1 / SpeedUpMultiplier;

	// Derived values
	NumOfTicks = 1;
	TimeBetweenTicks = 10.0f;
	
	SetHaveTimeAfterLastTick(true);

	SetIsReplicatedByDefault(true);
	SetClientReplicatesTick(true);
}

void UPowerupSuperSpeedComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPowerupSuperSpeedComponent::OnPowerupTick()
{
	Super::OnPowerupTick();
	
//	MovementSpeedIncrease(SpeedUpMultiplier);
}

void UPowerupSuperSpeedComponent::OnPowerupExpire()
{
	Super::OnPowerupExpire();
	
//	MovementSpeedIncrease(SpeedDownMultiplier);
}
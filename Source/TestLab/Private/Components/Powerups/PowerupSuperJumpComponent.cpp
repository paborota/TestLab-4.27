// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Powerups/PowerupSuperJumpComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
//#include "SCharacter.h"

UPowerupSuperJumpComponent::UPowerupSuperJumpComponent()
{
	// Class values
	JumpHeightUpMultiplier = 2.0f;
	JumpHeightDownMultiplier = 1 / JumpHeightUpMultiplier;

	// Derived values
	TimeBetweenTicks = 10.0f;

	SetHaveTimeAfterLastTick(true);
	
	SetIsReplicated(true);
	SetClientReplicatesTick(true);
}

void UPowerupSuperJumpComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPowerupSuperJumpComponent::OnPowerupTick()
{
	JumpHeightIncrease(JumpHeightUpMultiplier);

	Super::OnPowerupTick();
}

void UPowerupSuperJumpComponent::OnPowerupExpire()
{
	Super::OnPowerupExpire();
	
	JumpHeightIncrease(JumpHeightDownMultiplier);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Powerups/InstantDoubleJumpCharge.h"

#include "Components/Powerups/PowerupInterface.h"

UInstantDoubleJumpCharge::UInstantDoubleJumpCharge()
{
	ChargesToAdd = 1;
}

void UInstantDoubleJumpCharge::BeginPlay()
{
	Super::BeginPlay();
}

void UInstantDoubleJumpCharge::ActivatePowerup(IPowerupInterface* ActorToPowerup)
{
	AddCharges(ActorToPowerup);
}

void UInstantDoubleJumpCharge::AddCharges(IPowerupInterface* ActorToPowerup)
{
	ActorToPowerup->AddDoubleJumpCharge(ChargesToAdd);
}

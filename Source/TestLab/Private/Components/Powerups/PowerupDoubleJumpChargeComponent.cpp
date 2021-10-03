// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Powerups/PowerupDoubleJumpChargeComponent.h"
#include "TestLab/Public/Components/Powerups/PowerupInterface.h"

UPowerupDoubleJumpChargeComponent::UPowerupDoubleJumpChargeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Class Values
	NumberOfJumpsToAdd = 1;

	// Derived Values
	NumOfTicks = 1;
}

void UPowerupDoubleJumpChargeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPowerupDoubleJumpChargeComponent::OnPowerupTick()
{
	IPowerupInterface* MyOwner = Cast<IPowerupInterface>(GetOwner());
	if (!ensure(MyOwner != nullptr)) return;

	MyOwner->AddDoubleJumpCharge(NumberOfJumpsToAdd);
	
	Super::OnPowerupTick();
}
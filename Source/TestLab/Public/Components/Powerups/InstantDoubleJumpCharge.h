// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstantPowerupBaseComponent.h"
#include "InstantDoubleJumpCharge.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API UInstantDoubleJumpCharge : public UInstantPowerupBaseComponent
{
	GENERATED_BODY()
	
public:
	
	UInstantDoubleJumpCharge();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ChargesToAdd;

	void AddCharges(IPowerupInterface* ActorToPowerup);

public:

	virtual void ActivatePowerup(IPowerupInterface* ActorToPowerup) override;
	
};

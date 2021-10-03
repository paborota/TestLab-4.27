// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PowerupBaseComponent.h"
#include "PowerupDoubleJumpChargeComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API UPowerupDoubleJumpChargeComponent : public UPowerupBaseComponent
{
	GENERATED_BODY()

public:

	UPowerupDoubleJumpChargeComponent();

protected:

	virtual void BeginPlay() override;

	int32 NumberOfJumpsToAdd;

	virtual void OnPowerupTick() override;
};

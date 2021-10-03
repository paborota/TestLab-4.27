// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PowerupBaseComponent.h"
#include "PowerupHealthRegenComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API UPowerupHealthRegenComponent : public UPowerupBaseComponent
{
	GENERATED_BODY()

public:

	UPowerupHealthRegenComponent();

protected:

	virtual void BeginPlay() override;
	
	float HealthHealedPerTick;
	
	virtual void OnPowerupTick() override;
};

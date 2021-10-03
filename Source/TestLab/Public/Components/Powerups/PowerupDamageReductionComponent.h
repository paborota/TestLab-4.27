// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PowerupBaseComponent.h"
#include "PowerupDamageReductionComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API UPowerupDamageReductionComponent : public UPowerupBaseComponent
{
	GENERATED_BODY()

public:
	
	UPowerupDamageReductionComponent();

protected:

	float DamageReductionMultiplier;

	float DamageReductionReverseMultiplier;


	virtual void OnPowerupTick() override;

	virtual void OnPowerupExpire() override;
};

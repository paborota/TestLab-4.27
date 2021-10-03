// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PowerupBaseComponent.h"
#include "PowerupQuadDamageComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API UPowerupQuadDamageComponent : public UPowerupBaseComponent
{
	GENERATED_BODY()

public:

	UPowerupQuadDamageComponent();
	
protected:
	

	float DamageUpMultiplier;

	float DamageDownMultiplier;
	
	
	virtual void OnPowerupTick() override;

	virtual void OnPowerupExpire() override;
};

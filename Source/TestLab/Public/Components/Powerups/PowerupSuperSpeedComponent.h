// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PowerupBaseComponent.h"
#include "PowerupSuperSpeedComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API UPowerupSuperSpeedComponent : public UPowerupBaseComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UPowerupSuperSpeedComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
		
	float SpeedUpMultiplier;

	float SpeedDownMultiplier;
	
	
	virtual void OnPowerupTick() override;
	
	virtual void OnPowerupExpire() override;
};

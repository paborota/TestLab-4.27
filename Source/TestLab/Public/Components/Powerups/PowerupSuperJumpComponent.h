// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PowerupBaseComponent.h"
#include "PowerupSuperJumpComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API UPowerupSuperJumpComponent : public UPowerupBaseComponent
{
	GENERATED_BODY()

public:
	
	UPowerupSuperJumpComponent();

protected:
	
	virtual void BeginPlay() override;

	
	float JumpHeightUpMultiplier;
	
	float JumpHeightDownMultiplier;

	
	virtual void OnPowerupTick() override;

	virtual void OnPowerupExpire() override;
};

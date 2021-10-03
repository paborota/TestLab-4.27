// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PowerupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPowerupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TESTLAB_API IPowerupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void SetCurrentDamageMultiplier(const float& Multiplier) { return; }
	virtual float GetCurrentDamageMultiplier() const { return -1.0f; }

	virtual void SetCurrentDamageReduction(const float& Multiplier) { return; }
	virtual float GetCurrentDamageReduction() const { return -1.0f; }

	virtual void AddDoubleJumpCharge(const float& Amount) { return; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WallJumpComponentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWallJumpComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TESTLAB_API IWallJumpComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual USceneComponent* GetLineCasterRef() const { return nullptr; }
	
	virtual void CheckOtherFallingUtil(const float& DeltaTime) { return; }

	virtual float GetMaxWalkSpeed() const { return 0.0f; }

	virtual float GetDefaultGravityScale() const { return 1.0f; }

	virtual void ResetUsedDoubleJump() { return; }
};

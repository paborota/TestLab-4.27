// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CheckpointInterface.h"
#include "GameFramework/GameModeBase.h"
#include "TestLabGameModeBase.generated.h"


class AProtoMine;
class ASPickupActor;

/**
 * 
 */
UCLASS()
class TESTLAB_API ATestLabGameModeBase : public AGameModeBase, public ICheckpointInterface
{
	GENERATED_BODY()

	
protected:
	
	virtual void BeginPlay() override;
	
public:

	void RespawnObjects();
};

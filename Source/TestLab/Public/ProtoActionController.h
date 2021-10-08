// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckpointInterface.h"
#include "GameFramework/PlayerController.h"
#include "ProtoActionController.generated.h"

/**
 * 
 */
UCLASS()
class TESTLAB_API AProtoActionController : public APlayerController, public ICheckpointInterface
{
	GENERATED_BODY()

protected:
	
	FVector LastCheckpointLocation;
	
	FRotator LastCheckpointRotation;
	
public:
	
	virtual void RestartLevel() override;
	
	virtual void UpdateSpawnPoint(const FVector& PlayerStartLocation, const FRotator& PlayerStartRotation) override;
};

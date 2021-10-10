// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoActionController.h"

#include "GameFramework/PawnMovementComponent.h"
#include "TestLab/TestLabGameModeBase.h"


void AProtoActionController::RestartLevel()
{
	//Super::RestartLevel();

	if (GetPawn() == nullptr) return;

	GetPawn()->GetMovementComponent()->StopMovementImmediately();
	GetPawn()->SetActorLocation(LastCheckpointLocation);
	SetControlRotation(LastCheckpointRotation);
	
	ATestLabGameModeBase* GameMode = Cast<ATestLabGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->RespawnObjects();
}

void AProtoActionController::UpdateSpawnPoint(const FVector& PlayerStartLocation, const FRotator& PlayerStartRotation)
{
	LastCheckpointLocation = PlayerStartLocation;
	LastCheckpointRotation = PlayerStartRotation;
}

// Copyright Epic Games, Inc. All Rights Reserved.


#include "TestLabGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "ProtoMine.h"
#include "SPickupActor.h"


void ATestLabGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProtoMine::StaticClass(), Mines);
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASPickupActor::StaticClass(), Powerups);
}

void ATestLabGameModeBase::RespawnObjects()
{
	for (TActorIterator<AProtoMine> MinePtr(GetWorld()); MinePtr; ++MinePtr)
	{
		MinePtr->Respawn();
	}
	for (TActorIterator<ASPickupActor> PickupPtr(GetWorld()); PickupPtr; ++PickupPtr)
	{
		PickupPtr->RespawnPowerup();
	}
}
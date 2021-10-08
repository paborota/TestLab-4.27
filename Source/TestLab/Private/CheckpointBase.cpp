// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckpointBase.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"

#include "CheckpointInterface.h"

// Sets default values
ACheckpointBase::ACheckpointBase()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACheckpointBase::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpointBase::OnBeginOverlap_BoxComponent);
}

void ACheckpointBase::OnBeginOverlap_BoxComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Cast other actor to interface to verify it's the player
	UE_LOG(LogTemp, Warning, TEXT("Overlapped."));
	ACharacter* ActorAsCharacter = Cast<ACharacter>(OtherActor);
	if (!ActorAsCharacter) return;

	// Set players new respawn point to be the new spawn point
	UE_LOG(LogTemp, Warning, TEXT("Checkpoint reached."));
	ICheckpointInterface* PlayerControllerAsInterface = Cast<ICheckpointInterface>(GetWorld()->GetFirstPlayerController());
	if (!PlayerControllerAsInterface) return;
	
	PlayerControllerAsInterface->UpdateSpawnPoint(PlayerStartLocation + GetActorLocation(), PlayerStartRotation);
}
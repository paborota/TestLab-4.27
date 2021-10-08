// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointBase.generated.h"


class UBoxComponent;
class UCapsuleComponent;
class APlayerStart;

UCLASS()
class TESTLAB_API ACheckpointBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpointBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Checkpoint")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Checkpoint", Meta = (MakeEditWidget = true))
	FVector PlayerStartLocation;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Checkpoint")
	FRotator PlayerStartRotation;
	
	UFUNCTION()
	void OnBeginOverlap_BoxComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProtoInspectable.generated.h"

UCLASS()
class TESTLAB_API AProtoInspectable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProtoInspectable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	FVector SpawnLocation;
	FQuat SpawnRotation;
	FVector SpawnRotationVector;
	
	FVector PlayerInspectLocation;
	FQuat PlayerInspectRotation;

	float InterpSpeed = 13.0f;

	bool bAbleToBePickedUp;
	bool bPickedUp;

	bool bMoving;
	bool bRotating;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	void TransformToLocation(const FVector& Location, const FQuat& Rotation, const float& DeltaTime);
	void MoveToLocation(const FVector& Location, const float& DeltaTime);
	void RotateToLocation(const FQuat& Rotation, const float& DeltaTime);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Inspect(const FVector& InspectLocation, const FQuat& InspectRotation);
	void PutDown();
	void SetInspectLocation(const FVector& InspectLocation);
	void SetInspectRotation(const FQuat& InspectRotation);
	
};

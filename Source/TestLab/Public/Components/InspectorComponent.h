// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InspectorComponent.generated.h"


class AProtoCharacter;
class AProtoInspectable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTLAB_API UInspectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInspectorComponent();

	//virtual void PostInitProperties() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	AProtoCharacter* Owner;
	UPROPERTY()
	APlayerController* OwnerController;
	UPROPERTY()
	AProtoInspectable* InspectedActor;
	UPROPERTY()
	USceneComponent* InspectPositionX;
	UPROPERTY()
	USceneComponent* InspectPositionY;
	UPROPERTY()
	USceneComponent* InspectPositionZ;
	UPROPERTY()
	USceneComponent* InspectRotationProxy;

	bool bIsInspecting;
	bool bAugmentRotation;
	bool bWantsToManipulate;

	float RotateUpVal;
	float RotateRightVal;
	float MouseXValue;
	float MouseYValue;
	FVector ZoomOffsetLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Controls")
	float ObjectRotationSpeed = 2.0f;			// Mouse
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Controls")
	float ObjectRotationSpeedGamepad = 150.0f;	// Gamepad
	
	void CalculateTransform();
	void CalculateLocation();
	void CalculateRotation();
	void RotateInspectedObjectUp(const float& Val);
	void RotateInspectedObjectRight(const float& Val);
	void RotateInspectedObjectClockWise(const float& Val);

	void ResetInspectComponents();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInspectedActor(AProtoInspectable* ActorToInspect) { InspectedActor = ActorToInspect; }
	void Inspect(const FHitResult& Hit);
	void BeginInspectManipulation();
	void EndInspectManipulation();
	void PutDownInspectedActor();
	
	FORCEINLINE void SetIsInspecting(const bool& bNewVal) { bIsInspecting = bNewVal; }
	FORCEINLINE bool GetIsInspecting() const { return bIsInspecting; }
	
	FORCEINLINE void SetAugmentRotation(const bool& bNewVal) { bAugmentRotation = bNewVal; RotateRightVal = 0.0f; RotateUpVal = 0.0f; }
	FORCEINLINE bool GetAugmentRotation() const { return bAugmentRotation; }

	FORCEINLINE void SetRotateUpVal(const float& NewVal) { RotateUpVal += (bWantsToManipulate) ? NewVal * ObjectRotationSpeed : 0.0f; }
	FORCEINLINE void SetRotateRightVal(const float& NewVal) { RotateRightVal += (bWantsToManipulate) ? NewVal * ObjectRotationSpeed : 0.0f; }
	FORCEINLINE void SetRotateUpValGamepad(const float& NewVal) { RotateUpVal += NewVal * ObjectRotationSpeedGamepad; }
	FORCEINLINE void SetRotateRightValGamepad(const float& NewVal) { RotateRightVal += NewVal * ObjectRotationSpeedGamepad; }

	void ZoomInspectedObject();
	void ZoomOutInspectedObject();
	
};

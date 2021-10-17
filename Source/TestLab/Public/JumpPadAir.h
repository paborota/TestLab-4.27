// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JumpPadBase.h"
#include "JumpPadAir.generated.h"

class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class TESTLAB_API AJumpPadAir : public AJumpPadBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AJumpPadAir();

protected:

	virtual void BeginPlay() override;
	
	virtual void OnCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void EndManipulation();
	
	ACharacter* CharacterToManipulate;
	UCharacterMovementComponent* CharacterToManipMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="JumpPad")
	float HoldTimeBeforeLaunch;
	float TimeElapseSinceCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="JumpPad")
	float SlowDownInterpSpeed;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="JumpPad")
	//float GravityDampenMultiplier;

	float DefaultGravityScale;

public:

	virtual void Tick(float DeltaSeconds) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProtoActionCharacter.generated.h"


UENUM(BlueprintType)
enum class EWallScanHit : uint8
{
	Forward,
	Backward,
	Left,
	Right,
	ForwardRight,
	ForwardLeft,
	BackwardRight,
	BackwardLeft,
	None
};

UCLASS()
class TESTLAB_API AProtoActionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProtoActionCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	FVector SpawnLocation;
	FRotator SpawnRotation;

	void ResetPlayer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	float MouseSensitivity;

	float DefaultGravityScale;
	//float DefaultMaxAcceleration;

	void MoveForward(const float Val);
	void MoveRight(const float Val);
	void LookUp(const float Val);
	void LookRight(const float Val);
	
	void PlayerClicked();
	bool bPlayerHoldingClick;
	float HoldingMeleeDuration;
	float MaxHoldingMeleeDuration;
	void ClickReleased();
	void ResetGravityParams();

	virtual void Jump() override;
	void DoubleJump();
	void WallJump();
	void CalcWallJumpVelocity(FVector& LaunchVelocity);
	void CalcWallJumpDirectionAfterRotation(FVector& LaunchVelocity);
	void CalcVelocity(FVector& LaunchVelocity) const;
	FVector WallJumpDirection;
	FVector VelocityDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump")
	float DeltaRotationClamp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump")
	float WallJumpVelocityAwayMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump")
	float WallJumpVelocityUp;
	bool bWantsToHover;
	void JumpReleased();
	void InterpHoverFall(const float& DeltaTime);
	float HoldingHoverDuration;
	float MaxHoldingHoverDuration;
	bool bStartedHovering;
	virtual void Landed(const FHitResult& Hit) override;
	void DoubleJumpReset() { bUsedDoubleJump = false; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump")
	float DoubleJumpVelocity;
	bool bUsedDoubleJump;

	void Shift();
	void ShiftReleased();

	void Sprint();
	void Walk();
	float MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Player")
	float MaxSprintSpeed;
	bool bWantsToSprintWhenLanded;
	
	void Dash();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	float DashVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	float DashTimeLength;
	bool bDashedRecently;
	void StopDash();
	void DashReset() { bDashedRecently = false; }

	void InterpHaltMovement(const float& DeltaTime);
	float HaltInterpSpeed;
	float HaltInputMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	USceneComponent* MainWallLineCaster;
	
	void CheckForNearbyWall();
	void CheckForward(const FCollisionQueryParams& Params);
	void CheckBackward(const FCollisionQueryParams& Params);
	void CheckRight(const FCollisionQueryParams& Params);
	void CheckLeft(const FCollisionQueryParams& Params);
	void CheckForwardRight(const FCollisionQueryParams& Params);
	void CheckForwardLeft(const FCollisionQueryParams& Params);
	void CheckBackwardRight(const FCollisionQueryParams& Params);
	void CheckBackwardLeft(const FCollisionQueryParams& Params);
	void DoWallTrace(const FCollisionQueryParams& Params, const FVector& TraceDirection);
	bool bTraceInfoCached;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump")
	float VelocityCachedTimeLength;
	FTimerHandle TimerHandle_ResetCachedVelocity;
	void ResetCachedInfo() { bTraceInfoCached = false; }
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player")
	float WallCheckDistance;
	FHitResult CachedHit;
	// EWallScanHit EWallHit;
	bool bCanWallJump;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
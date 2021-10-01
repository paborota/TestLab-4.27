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
	void DoubleJumpOLD();
	void RightClick();

	void WallJump();
	void WallJumpOLD();
	void ValidateCanWallJump();
	void CalcWallJumpVelocity(FVector& LaunchVelocity);
	FRotator CalcWallJumpDirection(FVector& LaunchVelocity);
	void CalcVelocity(FVector& LaunchVelocity);
	void CalcWallJumpDirectionAfterRotationOLD(FVector& LaunchVelocity);
	void CalcVelocityOLD(FVector& LaunchVelocity) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump")
	bool bUseOldWallJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (ClampMin="0", EditCondition="!bUseOldWallJump"))
	int32 StartingNumberOfDoubleJumps;
	int32 NumOfDoubleJumps;
	bool bMovementStopped;
	FVector WallJumpDirection;
	FVector VelocityDirection;
	float MaxWallJumpSpeedMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="bUseOldWallJump"))
	float DeltaRotationClamp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="bUseOldWallJump"))
	float WallJumpVelocityAwayMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="bUseOldWallJump"))
	float WallJumpVelocityUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="!bUseOldWallJump"))
	float WallJumpVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="!bUseOldWallJump"))
	float WallJumpUpwardsForce;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="!bUseOldWallJump"))
	float SlidingSpeedMultiplier;
	bool bWantsToHover;
	void JumpReleased();
	void InterpHoverFall(const float& DeltaTime);
	float HoldingHoverDuration;
	float MaxHoldingHoverDuration;
	bool bStartedHovering;
	virtual void Landed(const FHitResult& Hit) override;
	void DoubleJumpReset() { bUsedDoubleJump = false; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="bUseOldWallJump"))
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

	void Ctrl();
	void CtrlReleased();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	USceneComponent* MainWallLineCaster;
	
	bool CheckForNearbyWall();
	bool CheckForward(const FCollisionQueryParams& Params);
	bool CheckBackward(const FCollisionQueryParams& Params);
	bool CheckRight(const FCollisionQueryParams& Params);
	bool CheckLeft(const FCollisionQueryParams& Params);
	bool CheckForwardRight(const FCollisionQueryParams& Params);
	bool CheckForwardRightForward(const FCollisionQueryParams& Params);
	bool CheckForwardRightRight(const FCollisionQueryParams& Params);
	bool CheckForwardLeft(const FCollisionQueryParams& Params);
	bool CheckForwardLeftForward(const FCollisionQueryParams& Params);
	bool CheckForwardLeftLeft(const FCollisionQueryParams& Params);
	bool CheckBackwardRight(const FCollisionQueryParams& Params);
	bool CheckBackwardRightBackward(const FCollisionQueryParams& Params);
	bool CheckBackwardRightRight(const FCollisionQueryParams& Params);
	bool CheckBackwardLeft(const FCollisionQueryParams& Params);
	bool CheckBackwardLeftBackward(const FCollisionQueryParams& Params);
	bool CheckBackwardLeftLeft(const FCollisionQueryParams& Params);
	FVector GetTraceDirection(const FVector& BaseDirection, const FRotator& RotateOffset);
	bool DoWallTrace(const FCollisionQueryParams& Params, const FVector& TraceDirection);
	bool DoWallTraceOLD(const FCollisionQueryParams& Params, const FVector& TraceDirection);
	void CacheWallInfo(const FHitResult& Hit);
	bool bTraceInfoCached;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (EditCondition="bUseOldWallJump"))
	float VelocityCachedTimeLength;
	FTimerHandle TimerHandle_ResetCachedVelocity;
	void ResetCachedInfo() { bTraceInfoCached = false; }
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player")
	float WallCheckDistance;
	FHitResult CachedHit;
	// EWallScanHit EWallHit;
	bool bCanWallJump;
	bool bAttachedToWall;

	void UsingNewWallJumpTick(const float& DeltaTime);
	void UsingOldWallJumpTick(const float& DeltaTime);

	void CheckOtherFallingUtil(const float& DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void AddDoubleJumpCharges(const int32& NumOfCharges) { NumOfDoubleJumps += NumOfCharges; }

};
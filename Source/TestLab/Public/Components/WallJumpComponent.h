// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallJumpComponent.generated.h"


class IWallJumpComponentInterface;
class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTLAB_API UWallJumpComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this component's properties
	UWallJumpComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	IWallJumpComponentInterface* OwnerAsInterface;
	ACharacter* OwnerAsCharacter;
	UCharacterMovementComponent* OwnerCharacterMovement;
	
	void WallJumpOLD();
	void ValidateCanWallJump();
	void CheckIfShouldUseLeftHand();
	bool bLeftHandDetermined;
	bool bUsingLeftHand;
	void CalcWallJumpVelocity(FVector& LaunchVelocity);
	void CalcWallJumpDirection(FVector& LaunchVelocity);
	void CalcVelocity(FVector& LaunchVelocity);
	void CalcWallJumpDirectionAfterRotationOLD(FVector& LaunchVelocity);
	void CalcVelocityOLD(FVector& LaunchVelocity) const;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump")
	bool bUseOldWallJump;
	bool bMovementStopped;
	FVector WallJumpDirection;
	FVector VelocityDirection;
	float MaxWallJumpSpeedMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump", meta = (EditCondition="bUseOldWallJump"))
	float DeltaRotationClamp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump", meta = (EditCondition="bUseOldWallJump"))
	float WallJumpVelocityAwayMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump", meta = (EditCondition="bUseOldWallJump"))
	float WallJumpVelocityUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump", meta = (EditCondition="!bUseOldWallJump"))
	float WallJumpVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump", meta = (EditCondition="!bUseOldWallJump"))
	float WallJumpUpwardsForce;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump", meta = (EditCondition="!bUseOldWallJump"))
	float SlidingSpeedMultiplier;

	UPROPERTY()
	USceneComponent* MainWallLineCaster;
	
	bool CheckForNearbyWall();
	FVector CurrentTickForwardVector;
	FVector CurrentTickRightVector;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump", meta = (EditCondition="bUseOldWallJump"))
	float VelocityCachedTimeLength;
	FTimerHandle TimerHandle_ResetCachedVelocity;
	void ResetCachedInfo() { bTraceInfoCached = false; }
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump")
	float WallCheckDistance;
	FHitResult CachedHit;
	// EWallScanHit EWallHit;
	bool bCanWallJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WallJump")
	float HopVelocity;
	bool bCanHopUp;
	bool bHasHopped;

	void AttachToWall();
	bool bCanAttachToWall;
	bool bWasAlreadyAttachedToWall;
	bool bAttachedToWall;
	bool bWantsToGrabWall;
	float TimeAttachedToWall;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WallJump")
	float MaxTimeCanBeAttachedToWall;

	void UsingNewWallJumpTick(const float& DeltaTime);
	void UsingOldWallJumpTick(const float& DeltaTime);

	float MaxWalkSpeedFromOwner;
	float DefaultGravityScaleFromOwner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WallJump")
	float GravityScaleWhenAttaching;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WallJump")
	float VelocitySlowdownMultiplierWhenAttaching;
	
	float SlowDownMultiplierWhenAttached;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WallJump")
	float SlowDownInterpSpeed;
	
public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void WallJumpTick(const float& DeltaTime);
	
	void DetachFromWall();

	void WallJump();
	void WallHop();

	bool CanWallJump() const { return bCanWallJump; }
	void SetCanWallJump(const bool& bNewVal) { bCanWallJump = bNewVal; }

	bool IsAttachedToWall() const { return bAttachedToWall; }
	void SetIsAttachedToWall(const bool& bNewVal) { bAttachedToWall = bNewVal; }

	void SetWantsToGrabWall(const bool& bNewVal) { bWantsToGrabWall = bNewVal; }
	bool GetWantsToGrabWall() const { return bWantsToGrabWall; }

	bool CanHopUp() const { return bCanHopUp; }
	// void SetCanHopUp(const bool& NewVal) { bCanHopUp = NewVal; }

	void ResetWallParams();

	bool UsingOldWallJump() const { return bUseOldWallJump; }
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckpointInterface.h"
#include "Components/WallJumpComponentInterface.h"
#include "Components/Powerups/PowerupInterface.h"
#include "GameFramework/Character.h"
#include "ProtoActionCharacter.generated.h"


class UWallJumpComponent;
class UHealthComponent;
class UDashComponent;

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
class TESTLAB_API AProtoActionCharacter : public ACharacter, public IPowerupInterface, public IWallJumpComponentInterface, public ICheckpointInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProtoActionCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UWallJumpComponent* WallJumpComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UDashComponent* DashComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UHealthComponent* HealthComponent;

	void ResetPlayer();
	void ResetFromDeath();
	void ResetLevel();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	float MouseSensitivity;
	float DefaultMouseSensitivity;

	float DefaultGravityScale;

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
	bool bWasNormalJump;
	void DoubleJump();
	void DoubleJumpOLD();
	void RightClick();
	
	bool bWantsToHover;
	void JumpReleased();
	void InterpHoverFall(const float& DeltaTime);
	float HoldingHoverDuration;
	float MaxHoldingHoverDuration;
	bool bStartedHovering;
	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump")
	float DoubleJumpVelocity;
	bool bUsedDoubleJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (ClampMin="0"))
	int32 StartingNumberOfDoubleJumps;
	int32 NumOfDoubleJumps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Jump", meta = (ClampMin="0"))
	float AirControlSidewaysMultiplier;

	void Shift();
	void ShiftReleased();

	// void Sprint();
	// void Walk();
	float MaxWalkSpeed;
	// UPROPERTY(EditDefaultsOnly, Category="Player")
	// float MaxSprintSpeed;
	// bool bWantsToSprintWhenLanded;
	
	void Dash();

	void InterpHaltMovement(const float& DeltaTime);
	float HaltInterpSpeed;
	float HaltInputMultiplier;

	void Ctrl();
	void CtrlReleased();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	USceneComponent* MainWallLineCaster;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void HandleDeath();

	UPROPERTY(BlueprintReadOnly, Category="Death")
	bool bIsDead;
	bool bWantsToRespawn;
	bool bWasDead;
	float UnAlteredDeltaTime;

	bool bWantsToSlowDown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Death")
	float SlowDownInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Death")
	float MouseSlowDownInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Death")
	float RespawnInterpSpeed;

	bool bIsResetting;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Death")
	void RespawnReminder(const float& DeltaTime, const float& TimeOfDeath);
	UFUNCTION(BlueprintImplementableEvent, Category="Death")
	void DeleteRespawnReminder();

	void Respawn();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual USceneComponent* GetLineCasterRef() const override { return MainWallLineCaster; }

	virtual float GetMaxWalkSpeed() const override { return MaxWalkSpeed; }
	
	virtual float GetDefaultGravityScale() const override { return DefaultGravityScale; }
	
	virtual void AddDoubleJumpCharge(const float& Amount) override { NumOfDoubleJumps += Amount; }

	virtual void ResetUsedDoubleJump() override { bUsedDoubleJump = false; }

	virtual void CheckOtherFallingUtil(const float& DeltaTime) override;
};
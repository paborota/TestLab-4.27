// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerupBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTLAB_API UPowerupBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPowerupBaseComponent();

	// Overriden for the potential to have components that last forever, or until forcefully destroyed by something else
	// Destroy component is used to sync when a powerup is set to expire, to all clients
	// Powerup then cleans up values that it changed, if needed, before calling to Super
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetClientReplicatesTick(const bool& ShouldReplicate) { bAllowClientTick = ShouldReplicate; }

	void SetHaveTimeAfterLastTick(const bool& Val) { bHaveTimeBetweenTickAfterFinalTick = Val; }

	void SetKeepAliveUntilDestroyed(const bool& Val) { bKeepAliveUntilDestroyed = Val; }
	
	// Used specifically to make sure that values are cleaned up before component gets destroyed
	//virtual void PreDestroyFromReplication() override;
	
	bool bValuesModified;

	// 1 is the lowest this can be set.
	// Set to 1 by default.
	// DISREGARDED IF bLastUntilDestroyed is set to TRUE
	int32 NumOfTicks;
	
	// Time inbetween ticks or how long the only tick stays alive
	// Smallest value allowed is 0.01
	// Set to 0.01 by default
	float TimeBetweenTicks;

	// To be used only by the server to determine how the powerup should behave
	void DetermineHowToTick();

	// To be used only by the server to determine how the powerup should finish
	void DetermineHowToExpire();

	// Only will be called by server, unless using OnPowerupTickClient
	// Call to Super as last thing in inherited classes
	virtual void OnPowerupTick();
	FTimerHandle TimerHandle_TimerTillNextTick;

	// Override this and use it if code is needed to be ticked on client end to replicate values
	UFUNCTION(Client, Reliable)
	virtual void OnPowerupTickClient();

	// Call Super::OnPowerupExpired() at end of override
	virtual void OnPowerupExpire();
	FTimerHandle TimerHandle_TimerTillExpire;

	// Increase damage multiplier of actor that overlapped powerup
	void DamageGivenIncrease(const float& Multiplier);
	// Increase REDUCTION of damage TAKEN of the actor that overlapped powerup
	// Higher multiplier means less damage taken
	void DamageTakenReduction(const float& Multiplier);
	void MovementSpeedIncrease(const float& Multiplier);
	void JumpHeightIncrease(const float& Multiplier);
	// Called to heal actor that overlapped powerup
	void Heal(const float& Amount);
	// Called to deal damage to actor that overlapped powerup
	void DealDamage(const float& Amount);

private:
	// Whether or not the powerup should also tick on client
	bool bAllowClientTick;
	
	// Whether or not the powerup should end as soon as the last tick completes, or until the tick lifetime afterwards
	bool bHaveTimeBetweenTickAfterFinalTick;

	// Whether or not the powerup should stay alive until the actor, or component, is destroyed.
	// If powerup ticks, will continually keep ticking until destroyed.
	bool bKeepAliveUntilDestroyed;

	int32 TicksProcessed;

	void ModifyValues(const float& Multiplier);
	//void ModifyValues(const FString& NewFString);
	
	void PowerupExpire();
};

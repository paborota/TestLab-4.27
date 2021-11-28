// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PowerupBaseComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TestLab/Public/Components/Powerups/PowerupInterface.h"


// Sets default values for this component's properties
UPowerupBaseComponent::UPowerupBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	NumOfTicks = 1;
	TimeBetweenTicks = 0.5f;
	TicksProcessed = 0;

	bValuesModified = false;
	bHaveTimeBetweenTickAfterFinalTick = false;
	if (NumOfTicks == 1)
		bHaveTimeBetweenTickAfterFinalTick = true;
	bAllowClientTick = false;
	bKeepAliveUntilDestroyed = false;

	// Change this to true if manipulating something such as materials to visualize active powerups
	// Or if the effect needs to be done on owning clients end, as well
	SetIsReplicatedByDefault(false);
}

void UPowerupBaseComponent::DestroyComponent(bool bPromoteChildren)
{
	// Cleanup before marking for kill
	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_TimerTillNextTick);
		if (bValuesModified)
		{
			OnPowerupExpire();
		}
	}
	
	Super::DestroyComponent(bPromoteChildren);
}

// Called when the game starts
void UPowerupBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (NumOfTicks < 1)
	{
		NumOfTicks = 1;
	}

	if (TimeBetweenTicks < 0.01f)
	{
		TimeBetweenTicks = 0.01f;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		DetermineHowToTick();
	}
}

void UPowerupBaseComponent::OnPowerupTick()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Tick being processed."));
	
	if (bAllowClientTick)
	{
		OnPowerupTickClient();
	}

	if (bKeepAliveUntilDestroyed)
	{
		UE_LOG(LogTemp,Warning, TEXT("Keep alive until destroyed enabled."));
		return;
	}
	
	if (NumOfTicks == 1)
	{
	 	// We've ran once, and that's all the component wanted. Expire.
	 	DetermineHowToExpire();
		return;
	}
	
	++TicksProcessed;
	if (TicksProcessed >= NumOfTicks)
	{
		// We've ran the required amount of Ticks. End the Timer loop. Expire.
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_TimerTillNextTick);
		DetermineHowToExpire();
	}
}

void UPowerupBaseComponent::OnPowerupTickClient_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		return;
	}
	
	OnPowerupTick();
}

void UPowerupBaseComponent::OnPowerupExpire()
{
	UE_LOG(LogTemp, Warning, TEXT("Powerup has expired."));
}

void UPowerupBaseComponent::DetermineHowToTick()
{
	UE_LOG(LogTemp, Warning, TEXT("Tick determined."));
	if (NumOfTicks > 1)
	{
		// If there are more than 1 tick required, we set a looping timer
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_TimerTillNextTick, this, &UPowerupBaseComponent::OnPowerupTick, TimeBetweenTicks, true, 0.0f);
		return;
	}
	
	OnPowerupTick();
}

void UPowerupBaseComponent::DetermineHowToExpire()
{
	if (bHaveTimeBetweenTickAfterFinalTick)
	{
		// We want there to be time after the final tick before the component dies, set timer for expiration
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_TimerTillExpire, this, &UPowerupBaseComponent::PowerupExpire, TimeBetweenTicks);
		return;
	}

	// At this point we've done multiple ticks,
	// It should be fine to go straight into destroying the component
	PowerupExpire();
}

void UPowerupBaseComponent::PowerupExpire()
{
	DestroyComponent();
}







// ------------------------------------------------------------
// Stats manipulation functions, used only by inherited classes
// ------------------------------------------------------------

void UPowerupBaseComponent::DamageGivenIncrease(const float& Multiplier)
{
	IPowerupInterface* MyOwner = Cast<IPowerupInterface>(GetOwner());
	if (!ensure(MyOwner != nullptr)) return;

	const float NewMultiplier = MyOwner->GetCurrentDamageMultiplier() * Multiplier;
	MyOwner->SetCurrentDamageMultiplier(NewMultiplier);
	ModifyValues(Multiplier);
}

void UPowerupBaseComponent::DamageTakenReduction(const float& Multiplier)
{
	IPowerupInterface* MyOwner = Cast<IPowerupInterface>(GetOwner());
	if (!ensure(MyOwner != nullptr)) return;

	const float NewMultiplier = MyOwner->GetCurrentDamageReduction() / Multiplier;
	MyOwner->SetCurrentDamageReduction(NewMultiplier);
	ModifyValues(Multiplier);
}

void UPowerupBaseComponent::MovementSpeedIncrease(const float& Multiplier)
{
	ACharacter* MyOwner = Cast<ACharacter>(GetOwner());
	if (!ensure(MyOwner != nullptr)) return;

	MyOwner->GetCharacterMovement()->MaxWalkSpeed *= Multiplier;
	ModifyValues(Multiplier);
}

void UPowerupBaseComponent::JumpHeightIncrease(const float& Multiplier)
{
	ACharacter* MyOwner = Cast<ACharacter>(GetOwner());
	if (!ensure(MyOwner != nullptr)) return;

	MyOwner->GetCharacterMovement()->JumpZVelocity *= Multiplier;
	ModifyValues(Multiplier);
}

void UPowerupBaseComponent::Heal(const float& Amount)
{
	DealDamage(-Amount);
}

void UPowerupBaseComponent::DealDamage(const float& Amount)
{
	UGameplayStatics::ApplyDamage(GetOwner(), Amount, nullptr, nullptr, nullptr);
}

void UPowerupBaseComponent::ModifyValues(const float& Multiplier)
{
	UE_LOG(LogTemp, Warning, TEXT("Values have been modified."));
	bValuesModified = (Multiplier > 1) ? true : false;
}

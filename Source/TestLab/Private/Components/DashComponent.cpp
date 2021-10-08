// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DashComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	// ...
	DashVelocity = 5000.0f;
	DashTimeLength = .2f;
	bDashedRecently = false;
}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerAsCharacter = Cast<ACharacter>(GetOwner());
}

void UDashComponent::Dash()
{
	if (bDashedRecently) return;
	
	if (!OwnerAsCharacter) return;
	
	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerAsCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector LaunchVelocity = EyeRotation.Vector() * DashVelocity;
	OwnerAsCharacter->LaunchCharacter(LaunchVelocity, true, true);
	bDashedRecently = true;
	
	FTimerHandle TimerHandle_StopDash;
	OwnerAsCharacter->GetWorldTimerManager().SetTimer(TimerHandle_StopDash, this, &UDashComponent::EndDash, DashTimeLength);
}

void UDashComponent::EndDash()
{
	if (!OwnerAsCharacter) return;
	
	const FVector CurrentDirection = OwnerAsCharacter->GetCharacterMovement()->Velocity.GetSafeNormal();
	const float CurrentVDirectionVSUpDirection = FVector::DotProduct(CurrentDirection, OwnerAsCharacter->GetActorUpVector());
	float VelocityDampener = .15f;
	if (CurrentVDirectionVSUpDirection < 0)
	{
		VelocityDampener = .85 * pow(-CurrentVDirectionVSUpDirection, 1.6) + VelocityDampener;
	}
	OwnerAsCharacter->GetCharacterMovement()->Velocity = OwnerAsCharacter->GetCharacterMovement()->Velocity * VelocityDampener;
}
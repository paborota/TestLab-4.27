// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MantleComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UMantleComponent::UMantleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIsMantling = false;

	MantleTraceHeight = 120.0f;
	MantleDownTraceHeight = -128.0f;
	MantleTraceForwardDistance = 35.0f;
	
	Mantle1MInterpSpeed = 1.67f;
	Mantle2MInterpSpeed = 0.0f;
	MantleDown1MInterpSpeed = 1.33f;
	MantleDown2MInterpSpeed = 0.0f;

	bEnableMantleOffset = true;
	CurrentOffset = 0.0f;
	MantleOffset = 0.0f;
	InterpSpeed = 0.0f;
}


// Called when the game starts
void UMantleComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);

	Owner = GetOwner<ACharacter>();
}


// Called every frame
void UMantleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OffsetMantleTransform(DeltaTime);
}

void UMantleComponent::OffsetMantleTransform(const float& DeltaTime)
{
	if (!Owner)
		return;
	if (!Owner->GetCapsuleComponent())
		return;
	
	float Temp = CurrentOffset;
	CurrentOffset = FMath::FInterpTo(CurrentOffset, MantleOffset, DeltaTime, InterpSpeed);
	float OffsetToApply = CurrentOffset - Temp;

	FVector DeltaLocation = FVector(0.0f, 0.0f, OffsetToApply);
	Owner->GetCapsuleComponent()->AddWorldOffset(DeltaLocation);
}

void UMantleComponent::MantleTrace()
{
	if (!Owner)
		return;
	
	ResetMantleValues();
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	
	FHitResult Hit;
	// Check if we hit an up-mantle-able ledge
	if (!MantleTraceUp(Hit, Params))
	{
		// We didn't hit an up-mantle-able ledge, check for down-mantle-able
		// If we don't hit, just immediately return
		if (!MantleTraceDown(Hit, Params))
            return;

		MantleDownCalc(Hit);
		OnMantleStart.Broadcast(MantleType);
		return;
	}
	MantleUpCalc(Hit);
	OnMantleStart.Broadcast(MantleType);
}

bool UMantleComponent::MantleTraceUp(FHitResult& Hit, const FCollisionQueryParams& Params)
{
	FVector Location = Owner->GetActorLocation() + FVector(0.0f, 0.0f, MantleTraceHeight);
	FVector StartLocation = Location + Owner->GetActorForwardVector() * MantleTraceForwardDistance;
	FVector EndLocation = StartLocation + FVector(0.0f, 0.0f, -160.0f);
	FQuat Rotation = FQuat( 0.0f, 0.0f, 0.0f, 0.0f);

	return GetWorld()->SweepSingleByChannel(Hit, StartLocation, EndLocation, Rotation, ECC_Visibility, FCollisionShape::MakeSphere(10.0f), Params);
}

bool UMantleComponent::MantleTraceDown(FHitResult& Hit, const FCollisionQueryParams& Params)
{
	FVector Location = Owner->GetActorLocation() + FVector(0.0f, 0.0f, MantleDownTraceHeight);
	FVector StartLocation = Location + Owner->GetActorForwardVector() * MantleTraceForwardDistance;
	FVector EndLocation = StartLocation + FVector(0.0f, 0.0f, -180.0f);
	FQuat Rotation = FQuat( 0.0f, 0.0f, 0.0f, 0.0f);
	
	return GetWorld()->SweepSingleByChannel(Hit, StartLocation, EndLocation, Rotation, ECC_Visibility, FCollisionShape::MakeSphere(10.0f), Params);
}

void UMantleComponent::MantleUpCalc(const FHitResult& Hit)
{
	float Height = Hit.Location.Z - (Owner->GetActorLocation().Z - (Owner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 5.0f));
	if (Height >= 180.0f)
	{
		MantleType = EMantleType::Mantle2M;
		InterpSpeed = Mantle2MInterpSpeed * 0.0f;	// NEEDS TO BE UPDATED IF Mantle2M ANIMATION IS ADDED
		MantleOffset = Height - 200.0f;
	}
	else if (Height >= 80.0f)
	{
		MantleType = EMantleType::Mantle1M;
		InterpSpeed = Mantle1MInterpSpeed * 4.0f;	// NEEDS TO BE UPDATED IF Mantle1M ANIMATION IS UPDATED
		MantleOffset = Height - 100.0f;
	}
}

void UMantleComponent::MantleDownCalc(const FHitResult& Hit)
{
	float Height = Hit.Location.Z - (Owner->GetActorLocation().Z - (Owner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 12.0f));
		
	if (Height <= -180.0f)
	{
		MantleType = EMantleType::MantleDown2M;
		InterpSpeed = MantleDown2MInterpSpeed * 0.0f; // NEEDS TO BE UPDATED IF MantleDown2M ANIMATION IS ADDED
		MantleOffset = Height + 200.0f;
	}
	else if (Height <= -80.0f)
	{
		MantleType = EMantleType::MantleDown1M;
		InterpSpeed = MantleDown1MInterpSpeed * 4.5f;	// NEEDS TO BE UPDATED IF MantleDown1M ANIMATION IS UPDATED
		MantleOffset = Height + 100.0f;
	}
}

void UMantleComponent::ResetMantleValues()
{
	MantleType = EMantleType::None;
	CurrentOffset = 0.0f;
	MantleOffset = 0.0f;
	InterpSpeed = 0.0f;
}

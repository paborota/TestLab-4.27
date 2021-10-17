// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPadAir.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AJumpPadAir::AJumpPadAir()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	HoldTimeBeforeLaunch = 0.5f;
	SlowDownInterpSpeed = 2.0f;
}

void AJumpPadAir::BeginPlay()
{
	Super::BeginPlay();
}

void AJumpPadAir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TimeElapseSinceCollision += DeltaSeconds;
	if (TimeElapseSinceCollision >= HoldTimeBeforeLaunch)
	{
		// At this point we've hit the length of time we want to use to slow down the character
		// Disable tick so this won't run again, and then launch the character.
		EndManipulation();
		return;
	}

	if (!ensure(CharacterToManipMovement != nullptr)) return;
	
	const float NewGravityScale = FMath::FInterpTo(CharacterToManipMovement->GravityScale, 0.0f, DeltaSeconds, SlowDownInterpSpeed);
	CharacterToManipMovement->GravityScale = NewGravityScale;
	const FVector NewVelocity = FMath::VInterpTo(CharacterToManipMovement->Velocity, FVector(0.0f), DeltaSeconds, SlowDownInterpSpeed);
	CharacterToManipMovement->Velocity = NewVelocity;
}

void AJumpPadAir::EndManipulation()
{
	PrimaryActorTick.SetTickFunctionEnable(false);
	
	if (!ensure(CharacterToManipMovement != nullptr)) return;
	CharacterToManipMovement->GravityScale = DefaultGravityScale;
	
	if (!ensure(CharacterToManipulate != nullptr)) return;
	Launch(CharacterToManipulate);
	
	CharacterToManipulate = nullptr;
	CharacterToManipMovement = nullptr;
}

void AJumpPadAir::OnCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UE_LOG(LogTemp, Warning, TEXT("Overlapped."));
	CharacterToManipulate = Cast<ACharacter>(OtherActor);
	if (!ensure(CharacterToManipulate != nullptr)) return;
	
	CharacterToManipMovement = CharacterToManipulate->GetCharacterMovement();
	if (!ensure(CharacterToManipMovement != nullptr)) return;
	
	TimeElapseSinceCollision = 0.0f;
	DefaultGravityScale = CharacterToManipMovement->GravityScale;
	PrimaryActorTick.SetTickFunctionEnable(true);
}
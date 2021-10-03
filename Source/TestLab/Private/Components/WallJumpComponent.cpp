// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WallJumpComponent.h"

#include "Components/WallJumpComponentInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UWallJumpComponent::UWallJumpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	WallCheckDistance = 65.0f;
	bCanWallJump = false;
	bAttachedToWall = false;
	bTraceInfoCached = false;
	VelocityCachedTimeLength = .45f;
	WallJumpVelocityUp = 450.0f;
	WallJumpVelocityAwayMultiplier = 1.1f;
	DeltaRotationClamp = .65f;
	MaxWallJumpSpeedMultiplier = 1.8f;
	bUseOldWallJump = false;
	
	WallJumpVelocity = 1200.0f;
	WallJumpUpwardsForce = 400.0f;
	SlidingSpeedMultiplier = 1.0f;
}


// Called when the game starts
void UWallJumpComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerAsCharacter = Cast<ACharacter>(GetOwner());
	OwnerAsInterface = Cast<IWallJumpComponentInterface>(GetOwner());
	if (!ensure(OwnerAsInterface != nullptr)) return;

	MainWallLineCaster = OwnerAsInterface->GetLineCasterRef();
	MaxSprintSpeedFromOwner = OwnerAsInterface->GetMaxSprintSpeed();
	DefaultGravityScaleFromOwner = OwnerAsInterface->GetDefaultGravityScale();
}

void UWallJumpComponent::WallJumpTick(const float& DeltaTime)
{
	bCanWallJump = false;
	bAttachedToWall = false;
	if (!ensure(OwnerAsCharacter != nullptr)) return;
	if (OwnerAsCharacter->GetCharacterMovement()->IsFalling())
	{
		if (bUseOldWallJump)
		{
			UsingOldWallJumpTick(DeltaTime);
		}
		else
		{
			UsingNewWallJumpTick(DeltaTime);
		}
	}
	else
	{
		bTraceInfoCached = false;
	}
}

void UWallJumpComponent::UsingNewWallJumpTick(const float& DeltaTime)
{
	UCharacterMovementComponent* OwnerCharacterMovement = (Cast<ACharacter>(GetOwner()))->GetCharacterMovement();
	if (!ensure(OwnerCharacterMovement != nullptr)) return;
	if (CheckForNearbyWall())
	{
		// Stop current movement, ONCE
		if (!bMovementStopped)
		{
			OwnerCharacterMovement->StopMovementImmediately();
			OwnerCharacterMovement->GravityScale = 0.1f;
			bMovementStopped = true;
		}
		else
		{
			// Start sliding down, getting progressively faster
			OwnerCharacterMovement->GravityScale = FMath::InterpEaseIn(OwnerCharacterMovement->GravityScale, DefaultGravityScaleFromOwner, DeltaTime * SlidingSpeedMultiplier, 2.0f);
		}
		bAttachedToWall = true;
		ValidateCanWallJump();
	}
	else
	{
		bTraceInfoCached = false;
		OwnerCharacterMovement->GravityScale = DefaultGravityScaleFromOwner;
		if (!ensure(OwnerAsInterface != nullptr)) return;
		OwnerAsInterface->CheckOtherFallingUtil(DeltaTime);
		bMovementStopped = false;
	}
}

void UWallJumpComponent::ValidateCanWallJump()
{
	APawn* OwnerAsPawn = Cast<APawn>(GetOwner());
	if (!ensure(OwnerAsPawn != nullptr)) return;
	
	// Check if players look direction is away from wall, if looking towards wall, don't allow to wall jump
	FVector PlayerLookLocation;
	FRotator PlayerLookAngle;
	OwnerAsPawn->GetController()->GetPlayerViewPoint(PlayerLookLocation, PlayerLookAngle);
	const float LookDirectionValidation = FVector::DotProduct(PlayerLookAngle.Vector(), WallJumpDirection);
	if (LookDirectionValidation > 0)
	{
		bCanWallJump = true;
	}
}

void UWallJumpComponent::UsingOldWallJumpTick(const float& DeltaTime)
{
	CheckForNearbyWall();
	
	if (!ensure(OwnerAsInterface != nullptr)) return;
	OwnerAsInterface->CheckOtherFallingUtil(DeltaTime);
}

void UWallJumpComponent::WallJump()
{
	if (bUseOldWallJump)
	{
		WallJumpOLD();
		return;
	}

	FVector LaunchVelocity;
	CalcWallJumpVelocity(LaunchVelocity);
	if (!ensure(OwnerAsInterface != nullptr)) return;
	// Make sure velocity does not exceed the max sprint speed too greatly
	LaunchVelocity = LaunchVelocity.GetClampedToMaxSize(MaxSprintSpeedFromOwner * 1.8f);
	
	if (!ensure(OwnerAsCharacter != nullptr)) return;
	OwnerAsCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void UWallJumpComponent::WallJumpOLD()
{
	FVector LaunchVelocity;
	CalcWallJumpVelocity(LaunchVelocity);
	
	// Make sure velocity does not exceed the max sprint speed too greatly
	LaunchVelocity = LaunchVelocity.GetClampedToMaxSize(MaxSprintSpeedFromOwner * 1.8f);

	if (!ensure(OwnerAsCharacter != nullptr)) return;
	OwnerAsCharacter->LaunchCharacter(LaunchVelocity, true, true);

	// Reset everything
	OwnerAsCharacter->GetWorldTimerManager().ClearTimer(TimerHandle_ResetCachedVelocity);
	ResetCachedInfo();

	if (!ensure(OwnerAsInterface != nullptr)) return;
	OwnerAsInterface->ResetUsedDoubleJump();
}

void UWallJumpComponent::CalcWallJumpVelocity(FVector& LaunchVelocity)
{
	if (!bUseOldWallJump)
	{
		CalcVelocity(LaunchVelocity);
	}
	else
	{
		CalcWallJumpDirectionAfterRotationOLD(LaunchVelocity);
		CalcVelocityOLD(LaunchVelocity);
	}
}

void UWallJumpComponent::CalcVelocity(FVector& LaunchVelocity)
{
	const FRotator PlayerLookAngle = CalcWallJumpDirection(LaunchVelocity);
	LaunchVelocity *= WallJumpVelocity;

	const FVector PlayerLookDirection = PlayerLookAngle.Vector();
	const float ForwardPitchExponent = FVector::DotProduct(CurrentTickForwardVector, PlayerLookDirection);
	const float ForwardPitchMultiplier = 1.25f * (1.0f - FMath::Pow(0.2f, ForwardPitchExponent));
	const FVector PlayerLookUpDirection = (PlayerLookDirection.Rotation() + FRotator(90.0f, 0.0f, 0.0f)).Vector();
	LaunchVelocity += PlayerLookUpDirection * WallJumpUpwardsForce * ForwardPitchMultiplier;
}

FRotator UWallJumpComponent::CalcWallJumpDirection(FVector& LaunchVelocity)
{
	APawn* OwnerAsPawn = Cast<APawn>(GetOwner());
	if (!ensure(OwnerAsPawn != nullptr)) return FRotator(0.0f);
	FVector PlayerLookLocation;
	FRotator PlayerLookAngle;
	OwnerAsPawn->GetController()->GetPlayerViewPoint(PlayerLookLocation, PlayerLookAngle);
	LaunchVelocity = PlayerLookAngle.Vector().GetSafeNormal();
	return PlayerLookAngle;
}


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//------------------------Old Wall Jump Calculations-------------------------------
void UWallJumpComponent::CalcWallJumpDirectionAfterRotationOLD(FVector& LaunchVelocity)
{
	const FVector VelocityDirectionNormalized = VelocityDirection.GetSafeNormal();
	FVector WallNormalRightVector = (CachedHit.Normal.Rotation() + FRotator(0.0f, 90.0f, 0.0f)).Vector();

	if (FVector::DotProduct(VelocityDirectionNormalized, WallNormalRightVector) < 0 )
	{
		WallNormalRightVector *= -1.0f;
	}

	if (!ensure(OwnerAsCharacter != nullptr)) return;
	const float ActorYawRotation = OwnerAsCharacter->GetActorRotation().Yaw;
	
	float Delta = FMath::Fmod((FMath::Fmod((ActorYawRotation - WallNormalRightVector.Rotation().Yaw), 360.0f) + 540.0f), 360.0f) - 180.0f;
	
	// Exponent should not be higher than 1.0
	const float Exponent = abs(Delta / 180.0f);
	
	// Rotation influence should be dampened based on how fast the character is moving
	const float SpeedExponent = OwnerAsCharacter->GetCharacterMovement()->Velocity.Size() / (MaxSprintSpeedFromOwner * MaxWallJumpSpeedMultiplier);
	const float DeltaRotationDampen = DeltaRotationClamp * -(1.0f - pow(.09f, SpeedExponent)) + 1.0f;
	
	const float DeltaMultiplier = DeltaRotationDampen * (1.0f - pow(.1f, Exponent));
	
	float OriginalDirectionInfluence = 1.0f - abs(FVector::DotProduct(VelocityDirectionNormalized, WallJumpDirection));
	OriginalDirectionInfluence = 2.0f * pow(OriginalDirectionInfluence, 2.0f);
	
	Delta *= DeltaMultiplier * OriginalDirectionInfluence;

	const float NewRotationYaw = VelocityDirectionNormalized.Rotation().Yaw + Delta;
	const FRotator NewDirectionRotation = //UKismetMathLibrary::ComposeRotators(Delta, VelocityDirectionNormalized.Rotation());
		FRotator(0.0f, NewRotationYaw, 0.0f);

	FVector NewDirection = NewDirectionRotation.Vector();
	NewDirection.Z = 0.0f;
	VelocityDirection = NewDirection * VelocityDirection.Size();
}

void UWallJumpComponent::CalcVelocityOLD(FVector& LaunchVelocity) const
{
	const FVector VelocityDirectionNormalized = VelocityDirection.GetSafeNormal();

	const float AngleInfluenceUp = FMath::Abs(FVector::DotProduct(WallJumpDirection, VelocityDirectionNormalized));
	const float AngleInfluence = 1.0f - AngleInfluenceUp;
	const float AngleInfluenceExp = 1.6f;
	
	// Takes into account how perpendicular the players velocity is with the wall
	const float VelocityAwayMultiplier = VelocityDirection.Size() * AngleInfluence * 0.8f;
	// Takes into account how parallel the players velocity is with the wall
	const float VelocityAwayMultiplierUp = VelocityDirection.Size() * AngleInfluenceUp * 1.35;

	FVector VelocityAway = (WallJumpDirection * VelocityAwayMultiplier + WallJumpDirection * VelocityAwayMultiplierUp) * WallJumpVelocityAwayMultiplier;

	if (VelocityAway.Size() < 200.0f)
	{
		// If for some reason the velocity pushing away from the wall is less than 150, set to 150
		// No matter what we want the wall jump to push from the wall
		VelocityAway.Normalize();
		VelocityAway *= 200.0f;
	}

	const float VelocityMultiplier = pow(AngleInfluence, AngleInfluenceExp) + 1.0f;
	const float VelocityUpMultiplier = 0.9f * pow(AngleInfluenceUp, AngleInfluenceExp * AngleInfluenceExp) + 1.1f;

	AActor* Owner = GetOwner();
	if (!ensure(Owner != nullptr)) return;
	FVector VelocityUp = Owner->GetActorUpVector() * WallJumpVelocityUp * VelocityUpMultiplier;
	
	LaunchVelocity = VelocityDirection * VelocityMultiplier								// Keep and slightly increase current momentum
					+ VelocityAway														// Push player away from wall based on how perpendicular and parallel their velocity is with the wall
					+ VelocityUp;	// Push player up based on how parallel their velocity is with the wall
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------


bool UWallJumpComponent::CheckForNearbyWall()
{
	AActor* Owner = GetOwner();
	if (!ensure(Owner != nullptr)) return false;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	
	CurrentTickForwardVector = Owner->GetActorForwardVector();
	CurrentTickRightVector = Owner->GetActorRightVector();

	bool bHitWall = false;
	
	// Every wall check needs to be done, and if any of them returns true, then we must set bHitWall to true.
	// Never set bHitWall back to false, as long as we hit a wall at least once, it must be true.
	
	if (CheckForward(Params)) bHitWall = true;
	if (CheckBackward(Params)) bHitWall = true;
	if (CheckRight(Params)) bHitWall = true;
	if (CheckLeft(Params)) bHitWall = true;
	if (CheckForwardRight(Params)) bHitWall = true;
	if (CheckForwardRightForward(Params)) bHitWall = true;
	if (CheckForwardRightRight(Params)) bHitWall = true;
	if (CheckForwardLeft(Params)) bHitWall = true;
	if (CheckForwardLeftForward(Params)) bHitWall = true;
	if (CheckForwardLeftLeft(Params)) bHitWall = true;
	if (CheckBackwardRight(Params)) bHitWall = true;
	if (CheckBackwardRightBackward(Params)) bHitWall = true;
	if (CheckBackwardRightRight(Params)) bHitWall = true;
	if (CheckBackwardLeft(Params)) bHitWall = true;
	if (CheckBackwardLeftBackward(Params)) bHitWall = true;
	if (CheckBackwardLeftLeft(Params)) bHitWall = true;

	return bHitWall;
}

bool UWallJumpComponent::CheckForward(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, CurrentTickForwardVector);
}

bool UWallJumpComponent::CheckBackward(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, -CurrentTickForwardVector);
}

bool UWallJumpComponent::CheckRight(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, CurrentTickRightVector);
}

bool UWallJumpComponent::CheckLeft(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, -CurrentTickRightVector);
}

bool UWallJumpComponent::CheckForwardRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = CurrentTickForwardVector + CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckForwardRightForward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = CurrentTickForwardVector + CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckForwardRightRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = CurrentTickForwardVector + CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckForwardLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = CurrentTickForwardVector - CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckForwardLeftForward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = CurrentTickForwardVector - CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckForwardLeftLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = CurrentTickForwardVector - CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckBackwardRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = -CurrentTickForwardVector + CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckBackwardRightBackward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = -CurrentTickForwardVector + CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckBackwardRightRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = -CurrentTickForwardVector + CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckBackwardLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = -CurrentTickForwardVector - CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckBackwardLeftBackward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = -CurrentTickForwardVector - CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool UWallJumpComponent::CheckBackwardLeftLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = -CurrentTickForwardVector - CurrentTickRightVector;
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

FVector UWallJumpComponent::GetTraceDirection(const FVector& BaseDirection, const FRotator& RotateOffset)
{
	FVector NewTraceDirection = BaseDirection;
	NewTraceDirection.Normalize();
	NewTraceDirection = (NewTraceDirection.Rotation() + RotateOffset).Vector();
	
	return NewTraceDirection;
}

bool UWallJumpComponent::DoWallTrace(const FCollisionQueryParams& Params, const FVector& TraceDirection)
{
	if (bUseOldWallJump)
	{
		return DoWallTraceOLD(Params, TraceDirection);
	}
	
	if (!ensure(MainWallLineCaster != nullptr)) return false;
	const FVector TraceOrigin = MainWallLineCaster->GetComponentLocation();
	const FVector TraceEnd = TraceOrigin + TraceDirection * WallCheckDistance;
	FHitResult HitTemp;
	if (GetWorld()->LineTraceSingleByChannel(HitTemp, TraceOrigin, TraceEnd, ECC_Visibility, Params))
	{
		if (!bTraceInfoCached)
		{
			CacheWallInfo(HitTemp);
			bTraceInfoCached = true;
			return true;
		}

		

		// Assuming we hit multiple things in a single frame
		// Check which surface is closer
		// Whichever surface is closer is the surface that we want to jump off of
		AActor* Owner = GetOwner();
		if (!ensure(Owner != nullptr)) return false;
		const float DistanceToHit = FVector::Distance(Owner->GetActorLocation(), CachedHit.Location);
		const float DistanceToHitTemp = FVector::Distance(Owner->GetActorLocation(), HitTemp.Location);

		if (DistanceToHit > DistanceToHitTemp)
		{
			CacheWallInfo(HitTemp);
			UE_LOG(LogTemp, Log, TEXT("Info Overwritten."));
		}
		return true;
	}
	return false;
}

bool UWallJumpComponent::DoWallTraceOLD(const FCollisionQueryParams& Params, const FVector& TraceDirection)
{
	if (!ensure(MainWallLineCaster != nullptr)) return false;
	const FVector TraceOrigin = MainWallLineCaster->GetComponentLocation();
	const FVector TraceEnd = TraceOrigin + TraceDirection * WallCheckDistance;
	FHitResult HitTemp;
	if (GetWorld()->LineTraceSingleByChannel(HitTemp, TraceOrigin, TraceEnd, ECC_Visibility, Params))
	{
		if (!ensure(OwnerAsCharacter != nullptr)) return false;
		
		bCanWallJump = true;
		if (!bTraceInfoCached)
		{
			CacheWallInfo(HitTemp);
			VelocityDirection = OwnerAsCharacter->GetCharacterMovement()->Velocity;
			VelocityDirection.Z = 0.0f;
			bTraceInfoCached = true;
			OwnerAsCharacter->GetWorldTimerManager().SetTimer(TimerHandle_ResetCachedVelocity, this, &UWallJumpComponent::ResetCachedInfo, VelocityCachedTimeLength);
			UE_LOG(LogTemp, Log, TEXT("Info Cached."));
			return true;
		}
		
		// This code only gets called if there is hits on multiple line traces, or a hit during the cache timer
		// Check which surface is closer
		// Whichever surface is closer is the surface that we want to jump off of
		
		const float DistanceToHit = FVector::Distance(OwnerAsCharacter->GetActorLocation(), CachedHit.Location);
		const float DistanceToHitTemp = FVector::Distance(OwnerAsCharacter->GetActorLocation(), HitTemp.Location);

		if (DistanceToHit > DistanceToHitTemp)
		{
			CacheWallInfo(HitTemp);
			UE_LOG(LogTemp, Log, TEXT("Info Overwritten."));
		}
		return true;
	}
	return false;
}

void UWallJumpComponent::CacheWallInfo(const FHitResult& Hit)
{
	CachedHit = Hit;
	WallJumpDirection = CachedHit.Normal;
}
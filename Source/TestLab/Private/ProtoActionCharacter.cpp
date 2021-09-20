// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoActionCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AProtoActionCharacter::AProtoActionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainWallLineCaster = CreateDefaultSubobject<USceneComponent>(TEXT("MainWallLineCaster"));
	MainWallLineCaster->SetupAttachment(RootComponent);

	MouseSensitivity = 1.5f;
	MaxHoldingMeleeDuration = 1.5f;
	HoldingMeleeDuration = 0.0f;
	MaxHoldingHoverDuration = 2.5f;
	HoldingHoverDuration = 0.0f;
	DefaultGravityScale = 1.0f;
	
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->JumpZVelocity = 450.0f;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GroundFriction = 50.0f;
	
	MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MaxSprintSpeed = MaxWalkSpeed * 2.0f;

	WallCheckDistance = 65.0f;
	bCanWallJump = false;
	bTraceInfoCached = false;
	VelocityCachedTimeLength = .3f;
	WallJumpVelocityUp = 450.0f;
	WallJumpVelocityAwayMultiplier = 1.0f;

	DashVelocity = 4000.0f;
	DashTimeLength = .2f;
	bDashedRecently = false;
	DoubleJumpVelocity = 600.0f;
	bUsedDoubleJump = false;
	bWantsToHover = false;
	bStartedHovering = false;
	bWantsToSprintWhenLanded = false;

	bPlayerHoldingClick = false;
	HaltInputMultiplier = 1.0f;
	HaltInterpSpeed = 4.5f;
}

// Called when the game starts or when spawned
void AProtoActionCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProtoActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->IsFalling())
	{
		CheckForNearbyWall();

		if (bPlayerHoldingClick)
		{
			InterpHaltMovement(DeltaTime);
		}
		else if (bWantsToHover && GetVelocity().Z < 0)
		{
			InterpHoverFall(DeltaTime);
		}
		
		return;
	}

	bCanWallJump = false;
}

// Called to bind functionality to input
void AProtoActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProtoActionCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProtoActionCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AProtoActionCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AProtoActionCharacter::LookRight);
	PlayerInputComponent->BindAction("Click", IE_Pressed, this, &AProtoActionCharacter::PlayerClicked);
	PlayerInputComponent->BindAction("Click", IE_Released, this, &AProtoActionCharacter::ClickReleased);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AProtoActionCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AProtoActionCharacter::JumpReleased);
	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &AProtoActionCharacter::Shift);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &AProtoActionCharacter::ShiftReleased);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AProtoActionCharacter::Dash);
}

void AProtoActionCharacter::MoveForward(const float Val)
{
	AddMovementInput(GetActorForwardVector() * Val * HaltInputMultiplier);
}

void AProtoActionCharacter::MoveRight(const float Val)
{
	AddMovementInput(GetActorRightVector() * Val * HaltInputMultiplier);
}

void AProtoActionCharacter::LookUp(const float Val)
{
	AddControllerPitchInput(Val * MouseSensitivity);
}

void AProtoActionCharacter::LookRight(const float Val)
{
	AddControllerYawInput(Val * MouseSensitivity);
}

void AProtoActionCharacter::Jump()
{
	Super::Jump();

	if (bCanWallJump)
	{
		WallJump();
	}
	else if (GetCharacterMovement()->IsFalling() && !bUsedDoubleJump)
	{
		DoubleJump();
	}
	bWantsToHover = true;
}

void AProtoActionCharacter::JumpReleased()
{
	bWantsToHover = false;
	bStartedHovering = false;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
}

void AProtoActionCharacter::DoubleJump()
{
	FVector LaunchVelocity = FVector(0.0f, 0.0f, DoubleJumpVelocity);
	LaunchCharacter(LaunchVelocity, false, true);
	bUsedDoubleJump = true;
}

void AProtoActionCharacter::WallJump()
{
	FVector LaunchVelocity;
	CalcWallJumpVelocity(LaunchVelocity);

	// Make sure velocity does not exceed the max sprint speed too greatly
	LaunchVelocity = LaunchVelocity.GetClampedToMaxSize(MaxSprintSpeed * 1.5);
	LaunchCharacter(LaunchVelocity, true, true);

	// Reset everything
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetCachedVelocity);
	ResetCachedInfo();
	// DashReset(); // Uncomment if you want dashes to be reset on wall jumps
	bUsedDoubleJump = false;
}

void AProtoActionCharacter::CalcWallJumpVelocity(FVector& LaunchVelocity) const
{
	FVector VelocityDirectionNormalized = VelocityDirection;
	VelocityDirectionNormalized.Normalize();

	const float AngleInfluenceUp = FMath::Abs(FVector::DotProduct(WallJumpDirection, VelocityDirectionNormalized));
	const float AngleInfluence = 1 - AngleInfluenceUp;
	const float AngleInfluenceExp = 1.6f;
	
	const float VelocityMultiplier = pow(AngleInfluence, AngleInfluenceExp) + 1.0f;
	const float VelocityUpMultiplier = 0.7f * pow(AngleInfluenceUp, AngleInfluenceExp * AngleInfluenceExp) + 1.3f;

	// Takes into account how perpendicular the players velocity is with the wall
	const float VelocityAwayMultiplier = VelocityDirection.Size() *AngleInfluence * 0.8f;
	// Takes into account how parallel the players velocity is with the wall
	const float VelocityAwayMultiplierUp = VelocityDirection.Size() * AngleInfluenceUp * 1.35;

	FVector VelocityAway = (WallJumpDirection * VelocityAwayMultiplier + WallJumpDirection * VelocityAwayMultiplierUp) * WallJumpVelocityAwayMultiplier;
	if (VelocityAway.Size() < 150.0f)
	{
		// If for some reason the velocity pushing away from the wall is less than 100, set to 100
		// No matter what we want the wall jump to push from the wall
		VelocityAway = WallJumpDirection * 150.0f;
	}
	
	LaunchVelocity = VelocityDirection * VelocityMultiplier								// Keep and slightly increase current momentum
					+ VelocityAway														// Push player away from wall based on how perpendicular and parallel their velocity is with the wall
					+ GetActorUpVector() * WallJumpVelocityUp * VelocityUpMultiplier;	// Push player up based on how parallel their velocity is with the wall
}

void AProtoActionCharacter::InterpHoverFall(const float& DeltaTime)
{
	if (!bStartedHovering)
	{
		GetCharacterMovement()->GravityScale = 0.05f;
		bStartedHovering = true;
	}
	
	if (HoldingHoverDuration < MaxHoldingHoverDuration)
	{
		//UE_LOG(LogTemp, Log, TEXT("%f"), GetCharacterMovement()->GravityScale);
		GetCharacterMovement()->GravityScale = FMath::InterpEaseIn(GetCharacterMovement()->GravityScale, DefaultGravityScale, DeltaTime, 2.0f);
		HoldingHoverDuration += DeltaTime;
	}
	else
	{
		JumpReleased();
	}
}

void AProtoActionCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	DoubleJumpReset();
	DashReset();
	HoldingHoverDuration = 0.0f;
	if (bWantsToSprintWhenLanded)
	{
		Sprint();
	}
	else
	{
		Walk();
	}
}

void AProtoActionCharacter::Shift()
{
	bWantsToSprintWhenLanded = true;
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	Sprint();
}

void AProtoActionCharacter::ShiftReleased()
{
	// Revert sprint and anything else that might need to be reverted
	bWantsToSprintWhenLanded = false;
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	Walk();
}

void AProtoActionCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
}

void AProtoActionCharacter::Walk()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AProtoActionCharacter::Dash()
{
	if (bDashedRecently)
	{
		return;
	}
	
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector LaunchVelocity = EyeRotation.Vector() * DashVelocity;
	LaunchCharacter(LaunchVelocity, true, true);
	bDashedRecently = true;
	
	FTimerHandle TimerHandle_StopDash;
	GetWorldTimerManager().SetTimer(TimerHandle_StopDash, this, &AProtoActionCharacter::StopDash, DashTimeLength);
}

void AProtoActionCharacter::StopDash()
{
	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity * .15f;
}

void AProtoActionCharacter::PlayerClicked()
{
	// wind up melee and slow gravity(?)
	bPlayerHoldingClick = true;
	GetCharacterMovement()->GravityScale = 0.05f;
}

void AProtoActionCharacter::ClickReleased()
{
	// release melee and return gravity(?)
	bPlayerHoldingClick = false;
	
	FTimerHandle TimerHandle_ResetGravityParams;
	GetWorldTimerManager().SetTimer(TimerHandle_ResetGravityParams, this, &AProtoActionCharacter::ResetGravityParams, .1f);
}

void AProtoActionCharacter::InterpHaltMovement(const float& DeltaTime)
{
	if (HoldingMeleeDuration < MaxHoldingMeleeDuration)
	{
		HaltInputMultiplier = FMath::FInterpTo(HaltInputMultiplier, 0.15f, DeltaTime, HaltInterpSpeed);
		GetCharacterMovement()->Velocity = FMath::VInterpTo(GetCharacterMovement()->Velocity, FVector(0.0f), DeltaTime, HaltInterpSpeed);
		GetCharacterMovement()->GravityScale = FMath::FInterpTo(GetCharacterMovement()->GravityScale, .05f, DeltaTime, HaltInterpSpeed);
		HoldingMeleeDuration += DeltaTime;
	}
	else
	{
		ResetGravityParams();
	}
}

void AProtoActionCharacter::ResetGravityParams()
{
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	HoldingMeleeDuration = 0.0f;
	HaltInputMultiplier = 1.0f;
	bPlayerHoldingClick = false;
}

void AProtoActionCharacter::CheckForNearbyWall()
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bCanWallJump = false;
	
	CheckForward(Params);
	CheckBackward(Params);
	CheckRight(Params);
	CheckLeft(Params);
	CheckForwardRight(Params);
	CheckForwardLeft(Params);
	CheckBackwardRight(Params);
	CheckBackwardLeft(Params);
}

void AProtoActionCharacter::CheckForward(const FCollisionQueryParams& Params)
{
	DoWallTrace(Params, GetActorForwardVector());
}

void AProtoActionCharacter::CheckBackward(const FCollisionQueryParams& Params)
{
	DoWallTrace(Params, -GetActorForwardVector());
}

void AProtoActionCharacter::CheckRight(const FCollisionQueryParams& Params)
{
	DoWallTrace(Params, GetActorRightVector());
}

void AProtoActionCharacter::CheckLeft(const FCollisionQueryParams& Params)
{
	DoWallTrace(Params, -GetActorRightVector());
}

void AProtoActionCharacter::CheckForwardRight(const FCollisionQueryParams& Params)
{
	FVector TraceDirection = GetActorForwardVector() + GetActorRightVector();
	TraceDirection.Normalize();
	DoWallTrace(Params, TraceDirection);
}

void AProtoActionCharacter::CheckForwardLeft(const FCollisionQueryParams& Params)
{
	FVector TraceDirection = GetActorForwardVector() - GetActorRightVector();
	TraceDirection.Normalize();
	DoWallTrace(Params, TraceDirection);
}

void AProtoActionCharacter::CheckBackwardRight(const FCollisionQueryParams& Params)
{
	FVector TraceDirection = -GetActorForwardVector() + GetActorRightVector();
	TraceDirection.Normalize();
	DoWallTrace(Params, TraceDirection);
}

void AProtoActionCharacter::CheckBackwardLeft(const FCollisionQueryParams& Params)
{
	FVector TraceDirection = -GetActorForwardVector() - GetActorRightVector();
	TraceDirection.Normalize();
	DoWallTrace(Params, TraceDirection);
}

void AProtoActionCharacter::DoWallTrace(const FCollisionQueryParams& Params, const FVector& TraceDirection)
{
	const FVector TraceOrigin = MainWallLineCaster->GetComponentLocation();
	const FVector TraceEnd = TraceOrigin + TraceDirection * WallCheckDistance;
	FHitResult HitTemp;
	if (GetWorld()->LineTraceSingleByChannel(HitTemp, TraceOrigin, TraceEnd, ECC_Visibility, Params))
	{
		bCanWallJump = true;
		if (!bTraceInfoCached)
		{
			CachedHit = HitTemp;
			WallJumpDirection = CachedHit.Normal;
			VelocityDirection = GetCharacterMovement()->Velocity;
			VelocityDirection.Z = 0.0f;
			bTraceInfoCached = true;
			GetWorldTimerManager().SetTimer(TimerHandle_ResetCachedVelocity, this, &AProtoActionCharacter::ResetCachedInfo, VelocityCachedTimeLength);
			UE_LOG(LogTemp, Log, TEXT("Info Cached."));
			return;
		}
		
		// This code only gets called if there is hits on multiple line traces
		// Check which wall is closer
		// Whichever wall is closer is the wall that we want to jump off of
		// Ignore if it's the same wall/actor
		if (CachedHit.GetActor() != HitTemp.GetActor())
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *CachedHit.Normal.ToString());
			UE_LOG(LogTemp, Log, TEXT("%s"), *CachedHit.Location.ToString());
			UE_LOG(LogTemp, Log, TEXT("%s"), *HitTemp.Normal.ToString());
			UE_LOG(LogTemp, Log, TEXT("%s"), *HitTemp.Location.ToString());
			const float DistanceToHit = FVector::Distance(GetActorLocation(), CachedHit.Location);
			const float DistanceToHitTemp = FVector::Distance(GetActorLocation(), HitTemp.Location);

			if (DistanceToHit > DistanceToHitTemp)
			{
				CachedHit = HitTemp;
				WallJumpDirection = CachedHit.Normal;
				UE_LOG(LogTemp, Log, TEXT("Info Overwritten."));
			}
		}
	}
}
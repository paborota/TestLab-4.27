// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoActionCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WallJumpComponent.h"


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
	GetCharacterMovement()->MaxAcceleration = 1024.0f;
	
	MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MaxSprintSpeed = MaxWalkSpeed * 2.0f;

	WallJumpComponent = CreateDefaultSubobject<UWallJumpComponent>(TEXT("WallJumpComponent"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	/*
	WallCheckDistance = 65.0f;
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
	*/
		
	StartingNumberOfDoubleJumps = 2;
	NumOfDoubleJumps = StartingNumberOfDoubleJumps;

	DashVelocity = 5000.0f;
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

	bIsDead = false;
}

// Called when the game starts or when spawned
void AProtoActionCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnHealthChanged.AddDynamic(this, &AProtoActionCharacter::OnHealthChanged);
	
	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation();
}

void AProtoActionCharacter::OnHealthChanged(UHealthComponent* HealthComp, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Took damage."));
	if (Health <= 0.0f && !bIsDead)
	{
		HandleDeath();
	}
}

void AProtoActionCharacter::HandleDeath()
{
	bIsDead = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0.0f;
	DisableInput(nullptr);
	GetMesh()->SetVisibility(false);
}

// Called every frame
void AProtoActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!ensure((WallJumpComponent != nullptr))) return;
	WallJumpComponent->WallJumpTick(DeltaTime);

	if (bIsDead)
	{
		
	}
}

void AProtoActionCharacter::CheckOtherFallingUtil(const float& DeltaTime)
{
	if (bPlayerHoldingClick)
	{
		InterpHaltMovement(DeltaTime);
	}
	else if (bWantsToHover && GetVelocity().Z < 0)
	{
		InterpHoverFall(DeltaTime);
	}
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
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &AProtoActionCharacter::RightClick);
	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &AProtoActionCharacter::Shift);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &AProtoActionCharacter::ShiftReleased);
	PlayerInputComponent->BindAction("Ctrl", IE_Pressed, this, &AProtoActionCharacter::Ctrl);
	PlayerInputComponent->BindAction("Ctrl", IE_Released, this, &AProtoActionCharacter::CtrlReleased);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AProtoActionCharacter::Dash);
	PlayerInputComponent->BindAction("ResetPlayer", IE_Pressed, this, &AProtoActionCharacter::ResetPlayer);
}

void AProtoActionCharacter::MoveForward(const float Val)
{
	if (!ensure(WallJumpComponent != nullptr)) return;
	if (WallJumpComponent->IsAttachedToWall() && !WallJumpComponent->UsingOldWallJump())
	{
		return;
	}
	AddMovementInput(GetActorForwardVector() * Val * HaltInputMultiplier);
}

void AProtoActionCharacter::MoveRight(const float Val)
{
	if (!ensure(WallJumpComponent != nullptr)) return;
	if (WallJumpComponent->IsAttachedToWall() && !WallJumpComponent->UsingOldWallJump())
	{
		return;
	}
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

	if (!ensure(WallJumpComponent != nullptr)) return;
	UE_LOG(LogTemp, Warning, TEXT("WallJumpComp verified."));
	if (WallJumpComponent->CanWallJump())
	{
		WallJumpComponent->WallJump();
	}
	else if (GetCharacterMovement()->IsFalling())
	{
		if (NumOfDoubleJumps > 0 && !WallJumpComponent->UsingOldWallJump() || !bUsedDoubleJump && WallJumpComponent->UsingOldWallJump())
		{
			DoubleJump();
		}
	}
	bWantsToHover = true;
}

void AProtoActionCharacter::JumpReleased()
{
	bWantsToHover = false;
	if (bStartedHovering)
	{
		bStartedHovering = false;
		GetCharacterMovement()->GravityScale = DefaultGravityScale;
	}
}

void AProtoActionCharacter::RightClick()
{
	/*
	if (!bAttachedToWall && !bUseOldWallJump)
	{
		DoubleJump();
	}
	*/
}

void AProtoActionCharacter::DoubleJump()
{
	if (WallJumpComponent->UsingOldWallJump())
	{
		DoubleJumpOLD();
		return;
	}

	if (NumOfDoubleJumps > 0)
	{
		if (!ensure(WallJumpComponent != nullptr)) return;
		WallJumpComponent->WallJump();
		--NumOfDoubleJumps;
	}
}

void AProtoActionCharacter::DoubleJumpOLD()
{
	const FVector LaunchVelocity = FVector(0.0f, 0.0f, DoubleJumpVelocity);
	LaunchCharacter(LaunchVelocity, false, true);
	bUsedDoubleJump = true;
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

	ResetUsedDoubleJump();
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

void AProtoActionCharacter::Ctrl()
{
	UE_LOG(LogTemp, Warning, TEXT("Crouching"));
	Crouch();
}

void AProtoActionCharacter::CtrlReleased()
{
	UnCrouch();
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
	const FVector LaunchVelocity = EyeRotation.Vector() * DashVelocity;
	LaunchCharacter(LaunchVelocity, true, true);
	bDashedRecently = true;
	
	FTimerHandle TimerHandle_StopDash;
	GetWorldTimerManager().SetTimer(TimerHandle_StopDash, this, &AProtoActionCharacter::StopDash, DashTimeLength);
}

void AProtoActionCharacter::StopDash()
{
	FVector CurrentDirection = GetCharacterMovement()->Velocity.GetSafeNormal();
	float CurrentVDirectionVSUpDirection = FVector::DotProduct(CurrentDirection, GetActorUpVector());
	float VelocityDampener = .15f;
	if (CurrentVDirectionVSUpDirection < 0)
	{
		VelocityDampener = .85 * pow(-CurrentVDirectionVSUpDirection, 1.6) + VelocityDampener;
	}
	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity * VelocityDampener;
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

void AProtoActionCharacter::ResetPlayer()
{
	GetCharacterMovement()->StopMovementImmediately();
	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);
}







/*
void AProtoActionCharacter::UsingNewWallJumpTick(const float& DeltaTime)
{
if (CheckForNearbyWall())
{
bAttachedToWall = true;
//UE_LOG(LogTemp, Warning, TEXT("Validating CanWallJump."));
ValidateCanWallJump();
// Stop current movement, ONCE
if (!bMovementStopped)
{
GetCharacterMovement()->StopMovementImmediately();
GetCharacterMovement()->GravityScale = 0.1f;
bMovementStopped = true;
}
else
{
// Start sliding down, getting progressively faster
GetCharacterMovement()->GravityScale = FMath::InterpEaseIn(GetCharacterMovement()->GravityScale, DefaultGravityScale, DeltaTime * SlidingSpeedMultiplier, 2.0f);
}
}
else
{
bTraceInfoCached = false;
GetCharacterMovement()->GravityScale = DefaultGravityScale;
CheckOtherFallingUtil(DeltaTime);
bMovementStopped = false;
}
}

void AProtoActionCharacter::ValidateCanWallJump()
{
// Check if players look direction is away from wall, if looking towards wall, don't allow to wall jump
FVector PlayerLookLocation;
FRotator PlayerLookAngle;
GetController()->GetPlayerViewPoint(PlayerLookLocation, PlayerLookAngle);
const float LookDirectionValidation = FVector::DotProduct(PlayerLookAngle.Vector(), WallJumpDirection);
if (LookDirectionValidation > 0)
{
bCanWallJump = true;
}
}

void AProtoActionCharacter::UsingOldWallJumpTick(const float& DeltaTime)
{
CheckForNearbyWall();
			
CheckOtherFallingUtil(DeltaTime);
}
*/

/*
void AProtoActionCharacter::WallJump()
{
	if (bUseOldWallJump)
	{
		WallJumpOLD();
		return;
	}

	FVector LaunchVelocity;
	CalcWallJumpVelocity(LaunchVelocity);

	// Make sure velocity does not exceed the max sprint speed too greatly
	LaunchVelocity = LaunchVelocity.GetClampedToMaxSize(MaxSprintSpeed * 1.8f);
	LaunchCharacter(LaunchVelocity, true, true);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), LaunchVelocity.Size());
}

void AProtoActionCharacter::WallJumpOLD()
{
	FVector LaunchVelocity;
	CalcWallJumpVelocity(LaunchVelocity);

	// Make sure velocity does not exceed the max sprint speed too greatly
	LaunchVelocity = LaunchVelocity.GetClampedToMaxSize(MaxSprintSpeed * 1.8f);
	LaunchCharacter(LaunchVelocity, true, true);

	// Reset everything
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetCachedVelocity);
	ResetCachedInfo();
	// DashReset(); // Uncomment if you want dashes to be reset on wall jumps
	bUsedDoubleJump = false;
}

void AProtoActionCharacter::CalcWallJumpVelocity(FVector& LaunchVelocity)
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

FRotator AProtoActionCharacter::CalcWallJumpDirection(FVector& LaunchVelocity)
{
	FVector PlayerLookLocation;
	FRotator PlayerLookAngle;
	GetController()->GetPlayerViewPoint(PlayerLookLocation, PlayerLookAngle);
	LaunchVelocity = PlayerLookAngle.Vector().GetSafeNormal();
	return PlayerLookAngle;
}

void AProtoActionCharacter::CalcVelocity(FVector& LaunchVelocity)
{
	const FRotator PlayerLookAngle = CalcWallJumpDirection(LaunchVelocity);
	LaunchVelocity *= WallJumpVelocity;

	const FVector PlayerLookDirection = PlayerLookAngle.Vector();
	const float ForwardPitchExponent = FVector::DotProduct(GetActorForwardVector(), PlayerLookDirection);
	const float ForwardPitchMultiplier = 1.25f * (1.0f - FMath::Pow(0.2f, ForwardPitchExponent));
	const FVector PlayerLookUpDirection = (PlayerLookDirection.Rotation() + FRotator(90.0f, 0.0f, 0.0f)).Vector();
	LaunchVelocity += PlayerLookUpDirection * WallJumpUpwardsForce * ForwardPitchMultiplier;
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//------------------------Old Wall Jump Calculations-------------------------------
void AProtoActionCharacter::CalcWallJumpDirectionAfterRotationOLD(FVector& LaunchVelocity)
{
	const FVector VelocityDirectionNormalized = VelocityDirection.GetSafeNormal();
	FVector WallNormalRightVector = (CachedHit.Normal.Rotation() + FRotator(0.0f, 90.0f, 0.0f)).Vector();

	if (FVector::DotProduct(VelocityDirectionNormalized, WallNormalRightVector) < 0 )
	{
		WallNormalRightVector *= -1.0f;
	}

	const float ActorYawRotation = GetActorRotation().Yaw;
	
	float Delta = FMath::Fmod((FMath::Fmod((ActorYawRotation - WallNormalRightVector.Rotation().Yaw), 360.0f) + 540.0f), 360.0f) - 180.0f;
	
	// Exponent should not be higher than 1.0
	const float Exponent = abs(Delta / 180.0f);
	
	// Rotation influence should be dampened based on how fast the character is moving
	const float SpeedExponent = GetCharacterMovement()->Velocity.Size() / (MaxSprintSpeed * MaxWallJumpSpeedMultiplier);
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

void AProtoActionCharacter::CalcVelocityOLD(FVector& LaunchVelocity) const
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

	FVector VelocityUp = GetActorUpVector() * WallJumpVelocityUp * VelocityUpMultiplier;
	
	LaunchVelocity = VelocityDirection * VelocityMultiplier								// Keep and slightly increase current momentum
					+ VelocityAway														// Push player away from wall based on how perpendicular and parallel their velocity is with the wall
					+ VelocityUp;	// Push player up based on how parallel their velocity is with the wall
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
*/


/*
bool AProtoActionCharacter::CheckForNearbyWall()
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
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

bool AProtoActionCharacter::CheckForward(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, GetActorForwardVector());
}

bool AProtoActionCharacter::CheckBackward(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, -GetActorForwardVector());
}

bool AProtoActionCharacter::CheckRight(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, GetActorRightVector());
}

bool AProtoActionCharacter::CheckLeft(const FCollisionQueryParams& Params)
{
	return DoWallTrace(Params, -GetActorRightVector());
}

bool AProtoActionCharacter::CheckForwardRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = GetActorForwardVector() + GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckForwardRightForward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = GetActorForwardVector() + GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckForwardRightRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = GetActorForwardVector() + GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckForwardLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = GetActorForwardVector() - GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckForwardLeftForward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = GetActorForwardVector() - GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckForwardLeftLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = GetActorForwardVector() - GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);

	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckBackwardRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = -GetActorForwardVector() + GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckBackwardRightBackward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = -GetActorForwardVector() + GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckBackwardRightRight(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = -GetActorForwardVector() + GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckBackwardLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f);
	const FVector BaseDirection = -GetActorForwardVector() - GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckBackwardLeftBackward(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, -22.5f, 0.0f);
	const FVector BaseDirection = -GetActorForwardVector() - GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

bool AProtoActionCharacter::CheckBackwardLeftLeft(const FCollisionQueryParams& Params)
{
	const FRotator RotateOffset = FRotator(0.0f, 22.5f, 0.0f);
	const FVector BaseDirection = -GetActorForwardVector() - GetActorRightVector();
	const FVector TraceDirection = GetTraceDirection(BaseDirection, RotateOffset);
	
	return DoWallTrace(Params, TraceDirection);
}

FVector AProtoActionCharacter::GetTraceDirection(const FVector& BaseDirection, const FRotator& RotateOffset)
{
	FVector NewTraceDirection = BaseDirection;
	NewTraceDirection.Normalize();
	NewTraceDirection = (NewTraceDirection.Rotation() + RotateOffset).Vector();
	
	return NewTraceDirection;
}

bool AProtoActionCharacter::DoWallTrace(const FCollisionQueryParams& Params, const FVector& TraceDirection)
{
	if (bUseOldWallJump)
	{
		return DoWallTraceOLD(Params, TraceDirection);
	}
	
	const FVector TraceOrigin = MainWallLineCaster->GetComponentLocation();
	const FVector TraceEnd = TraceOrigin + TraceDirection * WallCheckDistance;
	FHitResult HitTemp;
	if (GetWorld()->LineTraceSingleByChannel(HitTemp, TraceOrigin, TraceEnd, ECC_Visibility, Params))
	{
		if (!bTraceInfoCached)
		{
			CacheWallInfo(HitTemp);
			//VelocityDirection = GetCharacterMovement()->Velocity;
			bTraceInfoCached = true;
			//UE_LOG(LogTemp, Log, TEXT("Info Cached."));
			//UE_LOG(LogTemp, Warning, TEXT("%f"), VelocityDirection.Size());
			return true;
		}

		

		// Assuming we hit multiple things in a single frame
		// Check which surface is closer
		// Whichever surface is closer is the surface that we want to jump off of
		
		const float DistanceToHit = FVector::Distance(GetActorLocation(), CachedHit.Location);
		const float DistanceToHitTemp = FVector::Distance(GetActorLocation(), HitTemp.Location);

		if (DistanceToHit > DistanceToHitTemp)
		{
			CacheWallInfo(HitTemp);
			UE_LOG(LogTemp, Log, TEXT("Info Overwritten."));
		}
		return true;
	}
	return false;
}

bool AProtoActionCharacter::DoWallTraceOLD(const FCollisionQueryParams& Params, const FVector& TraceDirection)
{
	const FVector TraceOrigin = MainWallLineCaster->GetComponentLocation();
	const FVector TraceEnd = TraceOrigin + TraceDirection * WallCheckDistance;
	FHitResult HitTemp;
	if (GetWorld()->LineTraceSingleByChannel(HitTemp, TraceOrigin, TraceEnd, ECC_Visibility, Params))
	{
		bCanWallJump = true;
		if (!bTraceInfoCached)
		{
			CacheWallInfo(HitTemp);
			VelocityDirection = GetCharacterMovement()->Velocity;
			VelocityDirection.Z = 0.0f;
			bTraceInfoCached = true;
			GetWorldTimerManager().SetTimer(TimerHandle_ResetCachedVelocity, this, &AProtoActionCharacter::ResetCachedInfo, VelocityCachedTimeLength);
			UE_LOG(LogTemp, Log, TEXT("Info Cached."));
			return true;
		}
		
		// This code only gets called if there is hits on multiple line traces, or a hit during the cache timer
		// Check which surface is closer
		// Whichever surface is closer is the surface that we want to jump off of
		
		const float DistanceToHit = FVector::Distance(GetActorLocation(), CachedHit.Location);
		const float DistanceToHitTemp = FVector::Distance(GetActorLocation(), HitTemp.Location);

		if (DistanceToHit > DistanceToHitTemp)
		{
			CacheWallInfo(HitTemp);
			UE_LOG(LogTemp, Log, TEXT("Info Overwritten."));
		}
		return true;
	}
	return false;
}

void AProtoActionCharacter::CacheWallInfo(const FHitResult& Hit)
{
	CachedHit = Hit;
	WallJumpDirection = CachedHit.Normal;
}
*/
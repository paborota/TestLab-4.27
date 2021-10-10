// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoActionCharacter.h"

#include "ProtoActionController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

#include "Components/DashComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WallJumpComponent.h"
#include "CheckpointInterface.h"


// Sets default values
AProtoActionCharacter::AProtoActionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainWallLineCaster = CreateDefaultSubobject<USceneComponent>(TEXT("MainWallLineCaster"));
	MainWallLineCaster->SetupAttachment(RootComponent);

	WallJumpComponent = CreateDefaultSubobject<UWallJumpComponent>(TEXT("WallJumpComponent"));

	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	MouseSensitivity = 1.5f;
	DefaultMouseSensitivity = MouseSensitivity;
	MaxHoldingMeleeDuration = 1.5f;
	HoldingMeleeDuration = 0.0f;
	MaxHoldingHoverDuration = 2.5f;
	HoldingHoverDuration = 0.0f;
	DefaultGravityScale = 1.0f;
	
	GetCharacterMovement()->MaxWalkSpeed = 800.0f;
	GetCharacterMovement()->JumpZVelocity = 450.0f;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GroundFriction = 50.0f;
	GetCharacterMovement()->MaxAcceleration = 1024.0f;
	
	MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MaxSprintSpeed = MaxWalkSpeed;
		
	StartingNumberOfDoubleJumps = 2;
	NumOfDoubleJumps = StartingNumberOfDoubleJumps;
	
	DoubleJumpVelocity = 600.0f;
	bUsedDoubleJump = false;
	bWantsToHover = false;
	bStartedHovering = false;
	bWantsToSprintWhenLanded = false;

	bPlayerHoldingClick = false;
	HaltInputMultiplier = 1.0f;
	HaltInterpSpeed = 4.5f;

	bIsDead = false;
	SlowDownInterpSpeed = 2.0f;
	RespawnInterpSpeed = 1.8;
	MouseSlowDownInterpSpeed = 3.0f;
}

// Called when the game starts or when spawned
void AProtoActionCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnHealthChanged.AddDynamic(this, &AProtoActionCharacter::OnHealthChanged);

	ICheckpointInterface* ControllerAsInterface  = Cast<ICheckpointInterface>(GetController());
	if (!ControllerAsInterface) return;
	UE_LOG(LogTemp, Warning, TEXT("Location stored."));
	ControllerAsInterface->UpdateSpawnPoint(GetActorLocation(), GetActorRotation());
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
	if (bIsResetting) return;
	
	bIsDead = true;
	Walk();
	bWantsToSprintWhenLanded = false;
	bWantsToSlowDown = true;
	RespawnReminder(UnAlteredDeltaTime, GetWorld()->GetTimeSeconds());
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCharacterMovement()->GravityScale = 0.0f;
	
	GetMesh()->SetVisibility(false);
}

// Called every frame
void AProtoActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!ensure((WallJumpComponent != nullptr))) return;
	if (WallJumpComponent->GetWantsToGrabWall())
	{
		WallJumpComponent->WallJumpTick(DeltaTime);
	}

	if (bWantsToSlowDown)
	{
		const float NewDiliation = FMath::FInterpTo(UGameplayStatics::GetGlobalTimeDilation(GetWorld()), 0.0f, UnAlteredDeltaTime, SlowDownInterpSpeed * 2.0f);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDiliation);

		const float NewSens = FMath::FInterpTo(MouseSensitivity, 0.0f, UnAlteredDeltaTime, MouseSlowDownInterpSpeed * 2.0f);
		MouseSensitivity = NewSens;
		if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) < 0.2f && !bIsDead)
		{
			bWantsToSlowDown = false;
			Respawn();
		}
	}
	else
	{
		UnAlteredDeltaTime = DeltaTime;
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
	if (WallJumpComponent->IsAttachedToWall())
	{
		return;
	}

	if (Val < 0.0f && GetCharacterMovement()->IsFalling())
	{
		return;
	}
	AddMovementInput(GetActorForwardVector() * Val * HaltInputMultiplier);
}

void AProtoActionCharacter::MoveRight(const float Val)
{
	if (!ensure(WallJumpComponent != nullptr)) return;
	if (WallJumpComponent->IsAttachedToWall())
	{
		return;
	}

	float NewVal = Val;
	if (GetCharacterMovement()->IsFalling())
	{
		NewVal = Val * 0.35f;
	}
	
	AddMovementInput(GetActorRightVector() * NewVal * HaltInputMultiplier);
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
	UE_LOG(LogTemp, Warning, TEXT("Jump pressed."));
	if (CanJump())
	{
		Super::Jump();
		bWasNormalJump = true;
		return;
	}
	
	bWasNormalJump = false;
	if (!ensure(WallJumpComponent != nullptr)) return;
	UE_LOG(LogTemp, Warning, TEXT("WallJumpComp verified."));
	if (GetCharacterMovement()->IsFalling())
	{
		WallJumpComponent->SetWantsToGrabWall(true);
	}
}

void AProtoActionCharacter::JumpReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump released."));
	if (bWasNormalJump)
	{
		return;
	}

	WallJumpComponent->SetWantsToGrabWall(false);
	if (!ensure(WallJumpComponent != nullptr)) return;
	if (WallJumpComponent->IsAttachedToWall())
	{
		WallJumpComponent->DetachFromWall();
		if (WallJumpComponent->CanWallJump())
		{
			WallJumpComponent->WallJump();
		}
		else if (WallJumpComponent->CanHopUp())
		{
			WallJumpComponent->WallHop();
		}
	}
	else if (GetCharacterMovement()->IsFalling())
	{
		if (NumOfDoubleJumps > 0 && !WallJumpComponent->UsingOldWallJump() || !bUsedDoubleJump && WallJumpComponent->UsingOldWallJump())
		{
			DoubleJump();
		}
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
	if (ensure(DashComponent!= nullptr)) DashComponent->ResetDash();
	if (ensure(WallJumpComponent != nullptr)) WallJumpComponent->ResetWallParams();
	
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
	if (!ensure(DashComponent != nullptr)) return;
	DashComponent->Dash();
	
	if (!ensure(WallJumpComponent != nullptr)) return;
	WallJumpComponent->ResetWallParams();
}

void AProtoActionCharacter::PlayerClicked()
{
	// wind up melee and slow gravity(?)
	//bPlayerHoldingClick = true;
	//GetCharacterMovement()->GravityScale = 0.05f;
}

void AProtoActionCharacter::ClickReleased()
{
	// release melee and return gravity(?)
	//bPlayerHoldingClick = false;
	
	//FTimerHandle TimerHandle_ResetGravityParams;
	//GetWorldTimerManager().SetTimer(TimerHandle_ResetGravityParams, this, &AProtoActionCharacter::ResetGravityParams, .1f);
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
	if (bIsResetting)
	{
		return;
	}

	bIsResetting = true;
	if (bIsDead)
	{
		ResetFromDeath();
		return;
	}

	bWantsToSlowDown = true;
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager)
	{
		CameraManager->StartCameraFade(0.0f, 1.0f, 0.43f, FLinearColor::Black, true);
	}
	bWantsToRespawn = true;
}

void AProtoActionCharacter::ResetFromDeath()
{
	bIsDead = false;
	GetCharacterMovement()->StopMovementImmediately();
	DeleteRespawnReminder();
	GetMesh()->SetVisibility(true);
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	bWantsToRespawn = true;
}

void AProtoActionCharacter::ResetLevel()
{
	// @TODO
	
	AProtoActionController* PC = Cast<AProtoActionController>(GetController());
	if (!PC) return;
	PC->RestartLevel();
}

void AProtoActionCharacter::Respawn()
{
	ResetLevel();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager)
	{
		CameraManager->StartCameraFade(1.0f, 0.0f, 0.43f, FLinearColor::Black, true);
	}

	MouseSensitivity = DefaultMouseSensitivity;
	GetCapsuleComponent()->SetEnableGravity(true);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	bIsResetting = false;
	bWantsToRespawn = false;

	if (!ensure(HealthComponent != nullptr)) return;
	HealthComponent->ResetHealth();
}
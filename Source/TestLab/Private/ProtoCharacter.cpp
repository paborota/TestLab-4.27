// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoCharacter.h"

#include "ProtoInspectable.h"
#include "Camera/CameraComponent.h"
#include "Components/CameraSmoothZAxisComponent.h"
#include "Components/HealthComponent.h"
#include "Components/InspectorComponent.h"
#include "Components/MantleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProtoCharacter::AProtoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->bUsePawnControlRotation = true;
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

	CameraSmoothZComp = CreateDefaultSubobject<UCameraSmoothZAxisComponent>(TEXT("CameraSmoothZComp"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	InspectorComp = CreateDefaultSubobject<UInspectorComponent>(TEXT("InspectorComp"));
	MantleComp = CreateDefaultSubobject<UMantleComponent>(TEXT("MantleComp"));

	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	GetCharacterMovement()->bCanWalkOffLedges = false;

	MouseSensitivity = 1.0f;
	ControllerRotationRate = 75.0f;
	InteractRange = 400.0f;

	bUsingMouse = true;
	bEnableJump = true;
	bCanJump = true;
	bIsDead = false;
}

// Called when the game starts or when spawned
void AProtoCharacter::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetController());
}

// Called every frame
void AProtoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AProtoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProtoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProtoCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AProtoCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AProtoCharacter::LookRight);
	PlayerInputComponent->BindAxis("LookUpGamepad", this, &AProtoCharacter::LookUpGamepad);
	PlayerInputComponent->BindAxis("LookRightGamepad", this, &AProtoCharacter::LookRightGamepad);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AProtoCharacter::Interact);
	PlayerInputComponent->BindAction("Cancel", IE_Pressed, this, &AProtoCharacter::Cancel);
	PlayerInputComponent->BindAction("Click", IE_Pressed, this, &AProtoCharacter::PlayerClicked);
	PlayerInputComponent->BindAction("Click", IE_Released, this, &AProtoCharacter::ClickReleased);
	PlayerInputComponent->BindAction("Augment", IE_Pressed, this, &AProtoCharacter::AugmentClicked);
	PlayerInputComponent->BindAction("Augment", IE_Released, this, &AProtoCharacter::AugmentReleased);
	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &AProtoCharacter::Shift);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &AProtoCharacter::ShiftReleased);
	
	if (bEnableJump)
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AProtoCharacter::Jump);
}

void AProtoCharacter::MoveForward(const float Val)
{
	AddMovementInput(GetActorForwardVector() * Val);
}

void AProtoCharacter::MoveRight(const float Val)
{
	AddMovementInput(GetActorRightVector() * Val);
}

void AProtoCharacter::LookUp(const float Val)
{
	if (InspectorComp)
		InspectorComp->SetRotateUpVal(Val);

	AddControllerPitchInput(Val * MouseSensitivity);
	if (Val > 0.0f && !bUsingMouse)
		bUsingMouse = true;
}

void AProtoCharacter::LookRight(const float Val)
{
	if (InspectorComp)
		InspectorComp->SetRotateRightVal(Val);
	
	AddControllerYawInput(Val * MouseSensitivity);
	if (Val > 0.0f && !bUsingMouse)
		bUsingMouse = true;
}

void AProtoCharacter::LookUpGamepad(const float AxisValue)
{
	if (InspectorComp && AxisValue != 0.0f && InspectorComp->GetIsInspecting())
	{
		InspectorComp->SetRotateUpVal(AxisValue * GetWorld()->GetDeltaSeconds());
		if (bUsingMouse)
			bUsingMouse = false;
	}
	AddControllerPitchInput(AxisValue * ControllerRotationRate * GetWorld()->GetDeltaSeconds());
}

void AProtoCharacter::LookRightGamepad(const float AxisValue)
{
	if (AxisValue != 0.0f && InspectorComp->GetIsInspecting())
	{
		InspectorComp->SetRotateRightVal(AxisValue);
		if (bUsingMouse)
			bUsingMouse = false;
	}
	AddControllerYawInput(AxisValue * ControllerRotationRate * GetWorld()->GetDeltaSeconds());
}

void AProtoCharacter::Interact()
{
	// If we're inspecting, we don't want to be able to interact with anything else
	if (InspectorComp && InspectorComp->GetIsInspecting())
		return;
	FHitResult Hit;
	switch (CheckForObjectType(Hit))
	{
		case EObjectType::Interactable :
			if (InspectorComp)
				InspectorComp->Inspect(Hit);
			return;
		default :
			UE_LOG(LogTemp, Warning, TEXT("Nothing in range.")); // Replace with code to handle when there's nothing in reach, or don't.
	}
}

void AProtoCharacter::PlayerClicked()
{
	if (InspectorComp && InspectorComp->GetIsInspecting())
		InspectorComp->BeginInspectManipulation();
}

void AProtoCharacter::ClickReleased()
{
	if (InspectorComp && InspectorComp->GetIsInspecting())
		InspectorComp->EndInspectManipulation();
}

void AProtoCharacter::AugmentClicked()
{
	if (InspectorComp && InspectorComp->GetIsInspecting())
		InspectorComp->SetAugmentRotation(true);
}

void AProtoCharacter::AugmentReleased()
{
	if (InspectorComp)
		InspectorComp->SetAugmentRotation(false);
}

void AProtoCharacter::Shift()
{
	if (InspectorComp && InspectorComp->GetIsInspecting())
		InspectorComp->ZoomInspectedObject();
}

void AProtoCharacter::ShiftReleased()
{
	if (InspectorComp && InspectorComp->GetIsInspecting())
		InspectorComp->ZoomOutInspectedObject();
}

EObjectType AProtoCharacter::CheckForObjectType(FHitResult& Hit)
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

	FVector TraceEnd = PlayerViewLocation + PlayerViewRotation.Vector() * InteractRange;
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, PlayerViewLocation, TraceEnd, ECC_GameTraceChannel1)) // Linetrace for Interactable
		return EObjectType::Interactable;

	return EObjectType::None;
}

void AProtoCharacter::Cancel()
{
	if (InspectorComp)
		InspectorComp->PutDownInspectedActor();
}

void AProtoCharacter::Jump()
{
	if (!bCanJump)
		return;

	if (MantleComp)
		MantleComp->DoMantleTrace();
}

// @TODO HANDLE DEATH

void AProtoCharacter::OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead)
		return;

	if (Health <= 0)
	{
		// Time to die i guess
		UE_LOG(LogTemp, Log, TEXT("%s SHOULD BE DEAD RN."), *GetName());
	}
}
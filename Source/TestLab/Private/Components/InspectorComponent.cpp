// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InspectorComponent.h"

#include "ProtoCharacter.h"
#include "ProtoInspectable.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UInspectorComponent::UInspectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InspectPositionX = CreateDefaultSubobject<USceneComponent>(TEXT("InspectLocationX"));
	InspectPositionY = CreateDefaultSubobject<USceneComponent>(TEXT("InspectLocationY"));
	InspectPositionZ = CreateDefaultSubobject<USceneComponent>(TEXT("InspectLocationZ"));
	InspectRotationProxy = CreateDefaultSubobject<USceneComponent>(TEXT("InspectRotationProxy"));

	RotateUpVal = 0.0f;
	RotateRightVal = 0.0f;
	MouseXValue = 0.0f;
	MouseYValue = 0.0f;
	bIsInspecting = false;
	bAugmentRotation = false;
	bWantsToManipulate = false;
}


// Called when the game starts
void UInspectorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);

	Owner = GetOwner<AProtoCharacter>();
	
	if (Owner)
	{
		InspectPositionX->AttachToComponent(Owner->GetCameraComponentRef(), FAttachmentTransformRules::KeepRelativeTransform);
		InspectPositionX->SetRelativeLocation(FVector(FVector(70.0f, 0.0f, 0.0f)));
		InspectPositionY->AttachToComponent(Owner->GetCameraComponentRef(), FAttachmentTransformRules::KeepRelativeTransform);
		InspectPositionY->SetRelativeLocation(InspectPositionX->GetRelativeLocation());
		InspectPositionZ->AttachToComponent(Owner->GetCameraComponentRef(), FAttachmentTransformRules::KeepRelativeTransform);
		InspectPositionZ->SetRelativeLocation(InspectPositionX->GetRelativeLocation());
		InspectRotationProxy->AttachToComponent(Owner->GetCameraComponentRef(), FAttachmentTransformRules::KeepRelativeTransform);
		ResetInspectComponents();
	}

	if (InspectRotationProxy)
		ZoomOffsetLocation = InspectRotationProxy->GetRelativeLocation() - FVector(30.0f, 0.0f, 0.0f);
	
	OwnerController = Cast<APlayerController>(GetOwner<AProtoCharacter>()->GetController());
}


// Called every frame
void UInspectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsInspecting)
		CalculateTransform();

	if (InspectPositionX)
		UE_LOG(LogTemp, Warning, TEXT("%s"), *InspectPositionX->GetComponentLocation().ToString());
}

void UInspectorComponent::Inspect(const FHitResult& Hit)
{	
	InspectedActor = Cast<AProtoInspectable>(Hit.GetActor());
	if (!InspectedActor)
		return;
	
	InspectedActor->Inspect(InspectRotationProxy->GetComponentLocation(), InspectRotationProxy->GetComponentQuat());
	
	if (!OwnerController)
		return;
	
	OwnerController->SetIgnoreLookInput(true);
	OwnerController->SetIgnoreMoveInput(true);
	if(Owner && Owner->GetUsingMouse())
		OwnerController->SetShowMouseCursor(true);

	bIsInspecting = true;
	SetComponentTickEnabled(bIsInspecting);
	RotateUpVal = 0.0f;
	RotateRightVal = 0.0f;
}

void UInspectorComponent::PutDownInspectedActor()
{
	if (!InspectedActor)
		return;
	
	InspectedActor->PutDown();
	InspectedActor = nullptr;
	
	if (!OwnerController)
		return;

	OwnerController->SetIgnoreLookInput(false);
	OwnerController->SetIgnoreMoveInput(false);
	OwnerController->SetShowMouseCursor(false);
	
	ResetInspectComponents();
	bIsInspecting = false;
	SetComponentTickEnabled(bIsInspecting);
	RotateUpVal = 0.0f;
	RotateRightVal = 0.0f;
}

void UInspectorComponent::BeginInspectManipulation()
{
	if (!OwnerController)
		return;
	OwnerController->GetMousePosition(MouseXValue, MouseYValue);
	OwnerController->SetShowMouseCursor(false);
	bWantsToManipulate = true;
}

void UInspectorComponent::EndInspectManipulation()
{
	if (!OwnerController)
		return;
	OwnerController->SetMouseLocation(MouseXValue, MouseYValue);
	OwnerController->SetShowMouseCursor(true);
	bWantsToManipulate = false;
}
void UInspectorComponent::CalculateTransform()
{
	CalculateLocation();

	if (RotateUpVal + RotateRightVal != 0.0f)
		CalculateRotation();
}

void UInspectorComponent::CalculateLocation()
{
	InspectedActor->SetInspectLocation(InspectRotationProxy->GetComponentLocation());
}

void UInspectorComponent::CalculateRotation()
{
	float Temp = RotateRightVal;
	if (!bAugmentRotation)
	{
		RotateInspectedObjectRight(Temp);
     	RotateRightVal -= Temp;
		
		Temp = RotateUpVal;
		RotateInspectedObjectUp(Temp);
		RotateUpVal -= Temp;
	}
	else
	{
		RotateInspectedObjectClockWise(Temp);
		RotateRightVal -= Temp;
	}
	InspectedActor->SetInspectRotation(InspectRotationProxy->GetComponentQuat());
}

void UInspectorComponent::RotateInspectedObjectUp(const float& Val)
{
	FQuat OldRotation = InspectPositionY->GetComponentQuat();
	InspectPositionY->AddLocalRotation(FRotator(Val, 0.0f, 0.0f));
	FQuat NewRotation = InspectPositionY->GetComponentQuat();
	InspectRotationProxy->SetWorldRotation(NewRotation * OldRotation.Inverse() * InspectRotationProxy->GetComponentQuat());
}

void UInspectorComponent::RotateInspectedObjectRight(const float& Val)
{
	FQuat OldRotation = InspectPositionZ->GetComponentQuat();
	InspectPositionZ->AddLocalRotation(FRotator(0.0f, -Val, 0.0f));
	FQuat NewRotation = InspectPositionZ->GetComponentQuat();
	InspectRotationProxy->SetWorldRotation(NewRotation * OldRotation.Inverse() * InspectRotationProxy->GetComponentQuat());
}

void UInspectorComponent::RotateInspectedObjectClockWise(const float& Val)
{
	FQuat OldRotation = InspectPositionX->GetComponentQuat();
	InspectPositionX->AddLocalRotation(FRotator(0.0f, 0.0f, Val));
	FQuat NewRotation = InspectPositionX->GetComponentQuat();
	InspectRotationProxy->SetWorldRotation(NewRotation * OldRotation.Inverse() * InspectRotationProxy->GetComponentQuat());
}

void UInspectorComponent::ZoomInspectedObject()
{
	InspectRotationProxy->SetRelativeLocation(ZoomOffsetLocation);
}

void UInspectorComponent::ZoomOutInspectedObject()
{
	InspectRotationProxy->SetRelativeLocation(InspectPositionX->GetRelativeLocation());
}

void UInspectorComponent::ResetInspectComponents()
{
	InspectRotationProxy->SetRelativeLocation(InspectPositionX->GetRelativeLocation());
	
	FRotator DefaultPos = FRotator(0.0f);
	InspectPositionY->SetRelativeRotation(DefaultPos);
	InspectPositionZ->SetRelativeRotation(DefaultPos);
	InspectPositionX->SetRelativeRotation(DefaultPos);
	InspectRotationProxy->SetRelativeRotation(DefaultPos);
}
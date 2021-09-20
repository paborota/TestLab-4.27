// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CameraSmoothZAxisComponent.h"

#include "Camera/CameraComponent.h"
#include "ProtoCharacter.h"

// Sets default values for this component's properties
UCameraSmoothZAxisComponent::UCameraSmoothZAxisComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bEnableSmoothCameraZAxis = true;
	CameraOffset = FVector(0.0f, 0.0f, 60.0f);
	ZInterp = 0.0f;
	InterpSpeed = 9.0f;
}


// Called when the game starts
void UCameraSmoothZAxisComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(bEnableSmoothCameraZAxis);
	
	Owner = GetOwner<AProtoCharacter>();
	if (Owner)
		CameraComp = Owner->GetCameraComponentRef();
}


// Called every frame
void UCameraSmoothZAxisComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SmoothCameraZ(DeltaTime);
}

void UCameraSmoothZAxisComponent::SmoothCameraZ(const float& DeltaTime)
{
	if (!CameraComp || !Owner)
		return;

	ZInterp = FMath::FInterpTo(ZInterp, Owner->GetActorLocation().Z, DeltaTime, 9.0f);
	FVector Temp = FVector(Owner->GetActorLocation().X, Owner->GetActorLocation().Y, ZInterp);
	CameraComp->SetWorldLocation(Temp + CameraOffset);
}
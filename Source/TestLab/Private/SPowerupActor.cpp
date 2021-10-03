// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

#include "Components/PointLightComponent.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	bHaveActorRotate = true;
	if (bHaveActorRotate)
	{
		PrimaryActorTick.bCanEverTick = true;
	}
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = StaticMeshComp;

	PointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComp"));
	PointLightComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	PointLightComp->CastShadows = false;
	PointLightComp->SetupAttachment(RootComponent);

	VisualActorSpinSpeedMultiplier = 15.0f;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();

	if (!PowerupClass)
	{
		UE_LOG(LogTemp, Error, TEXT("There is currently no powerup class selected for this powerup actor."));
	}
}

void ASPowerupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float RotationMultiplier = DeltaTime * VisualActorSpinSpeedMultiplier + GetActorRotation().Yaw;
	SetActorRotation(FRotator(0.0f, RotationMultiplier, 0.0f));
}
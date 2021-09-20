// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoInspectable.h"

// Sets default values
AProtoInspectable::AProtoInspectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void AProtoInspectable::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation().Quaternion();
	SpawnRotationVector = SpawnRotation.Vector();
	bAbleToBePickedUp = true;
	bMoving = false;
	bRotating = false;
}

// Called every frame
void AProtoInspectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPickedUp)
		TransformToLocation(PlayerInspectLocation, PlayerInspectRotation, DeltaTime);
	else
		TransformToLocation(SpawnLocation, SpawnRotation, DeltaTime);
}

void AProtoInspectable::Inspect(const FVector& InspectLocation, const FQuat& InspectRotation)
{
	if (!bAbleToBePickedUp)
		return;
	
	bPickedUp = true;
	bAbleToBePickedUp = false;
	bRotating = true;
	bMoving = true;
	SetInspectLocation(InspectLocation);
	SetInspectRotation(InspectRotation);
}

void AProtoInspectable::PutDown()
{
	bPickedUp = false;
	bAbleToBePickedUp = true;
	SetInspectLocation(SpawnLocation);
	SetInspectRotation(SpawnRotation);
}

void AProtoInspectable::SetInspectLocation(const FVector& InspectLocation)
{
	PlayerInspectLocation = InspectLocation;
}

void AProtoInspectable::SetInspectRotation(const FQuat& InspectRotation)
{
	PlayerInspectRotation = InspectRotation;
}

void AProtoInspectable::TransformToLocation(const FVector& Location, const FQuat& Rotation, const float& DeltaTime)
{
	if (bMoving)
		MoveToLocation(Location, DeltaTime);

	if (bRotating)
		RotateToLocation(Rotation, DeltaTime);
}

void AProtoInspectable::MoveToLocation(const FVector& Location, const float& DeltaTime)
{
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), Location, DeltaTime, InterpSpeed));

	if (FVector::Dist(GetActorLocation(), SpawnLocation) <= 0.001f)
	{
		bMoving = false;
		bRotating = false;
	}
}

void AProtoInspectable::RotateToLocation(const FQuat& Rotation, const float& DeltaTime)
{
	SetActorRotation(FMath::QInterpTo(GetActorRotation().Quaternion(), Rotation, DeltaTime, InterpSpeed));
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPadBase.h"

#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"

// Sets default values
AJumpPadBase::AJumpPadBase()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;
	
	SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SphereCollisionComp->SetupAttachment(RootComponent);
	SphereCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	//BoxCollisionComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	
	LaunchUpVelocity = 500.0f;
	LaunchForwardVelocity = 300.0f;
}

// Called when the game starts or when spawned
void AJumpPadBase::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AJumpPadBase::OnCollisionOverlapped);
	UE_LOG(LogTemp, Warning, TEXT("Dynamic binding to Overlap Function completed."));
}

void AJumpPadBase::Launch(AActor* OtherActor)
{
	if (!ensure(OtherActor != nullptr)) return;
	ACharacter* OtherActorAsCharacter = Cast<ACharacter>(OtherActor);

	Launch(OtherActorAsCharacter);
}

void AJumpPadBase::Launch(ACharacter* OtherCharacter)
{
	if (!ensure(OtherCharacter != nullptr)) return;

	const FVector LaunchVelocity = GetActorForwardVector() * LaunchForwardVelocity + GetActorUpVector() * LaunchUpVelocity;
	OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);
}
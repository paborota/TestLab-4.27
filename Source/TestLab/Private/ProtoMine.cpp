// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoMine.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
AProtoMine::AProtoMine()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	RootComponent = StaticMeshComp;
	
	OuterSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphereCollision"));
	OuterSphereCollision->SetSphereRadius(200.0f);
	OuterSphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	OuterSphereCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	OuterSphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OuterSphereCollision->SetupAttachment(RootComponent);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->Radius = 200.0f;
	RadialForceComp->Falloff = RIF_Linear;
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->SetupAttachment(RootComponent);
	
	TimeToDetonate = 2.0f;
	BaseDamage = 1.0f;
	DamageRadius = OuterSphereCollision->GetScaledSphereRadius();
}

// Called when the game starts or when spawned
void AProtoMine::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComp->OnComponentHit.AddDynamic(this, &AProtoMine::OnHit_InnerSphereCollision);
	
	OuterSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProtoMine::OnBeginOverlap_OuterSphereCollision);
	OuterSphereCollision->OnComponentEndOverlap.AddDynamic(this, &AProtoMine::OnEndOverlap_OuterSphereCollision);
}

void AProtoMine::OnBeginOverlap_OuterSphereCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Start timer to detonation if it's the player
	UE_LOG(LogTemp, Warning, TEXT("Timer function called."));
	ACharacter* OtherActorAsCharacter = Cast<ACharacter>(OtherActor);
	if (!ensure(OtherActorAsCharacter != nullptr)) return;

	UE_LOG(LogTemp, Warning, TEXT("Timer has been set."));
	GetWorldTimerManager().SetTimer(TimerHandle_Detonation, this, &AProtoMine::Detonate, TimeToDetonate);
}

void AProtoMine::OnEndOverlap_OuterSphereCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Cancel timer to detonation if not exploded
	GetWorldTimerManager().ClearTimer(TimerHandle_Detonation);
}

void AProtoMine::OnHit_InnerSphereCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Detonate immediately
	Detonate();
}

void AProtoMine::Detonate()
{
	if (bExploded) return;
	
	// Explode
	UE_LOG(LogTemp, Warning, TEXT("Exploded."));
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRadius, nullptr, IgnoredActors, this, nullptr, true);
	RadialForceComp->FireImpulse();
	bExploded = true;
	StaticMeshComp->SetVisibility(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// @TODO particle explosion effect
}

void AProtoMine::Respawn()
{
	if (!bExploded) return;
	
	bExploded = false;
	StaticMeshComp->SetVisibility(true);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
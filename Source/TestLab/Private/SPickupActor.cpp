// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/Character.h"
#include "SPowerupActor.h"
#include "TestLab/Public/Components/Powerups/PowerupInterface.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;
	
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);

	RespawnCooldown = 0.0f;
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASPickupActor::OnSphereCompOverlap);
		// Respawn();
		SpawnPowerup();
	}
}

void ASPickupActor::SpawnPowerup()
{
	if (PowerupInstance) return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void ASPickupActor::RespawnPowerup()
{
	if (!PowerupInstance) return;

	PowerupInstance->SetActorHiddenInGame(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASPickupActor::OnSphereCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	VerifyActor(OtherActor);
}

void ASPickupActor::VerifyActor(AActor* OtherActor)
{
	IPowerupInterface* CharacterAsInterface = Cast<IPowerupInterface>(OtherActor);
	if (!ensure(CharacterAsInterface != nullptr)) return;
	UE_LOG(LogTemp, Warning, TEXT("Player overlapped pickup actor."));
	
	if (!PowerupInstance) return;
	
	ActivatePowerup(CharacterAsInterface);
	if (RespawnCooldown > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnPowerupInstance, this, &ASPickupActor::RespawnPowerup, RespawnCooldown);
	}

	PowerupInstance->SetActorHiddenInGame(true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASPickupActor::ActivatePowerup(IPowerupInterface* OtherActor)
{
    if (!PowerupInstance->HasValidPowerupClass())
    {
    	UE_LOG(LogTemp, Error, TEXT("This powerup was missing it's powerup class, cannot be registered."));
    	return;
    }
	
	if (PowerupInstance->GetInstantPowerupClass())
	{
		// If this is true then we know this is an instant-use powerup.
		PowerupInstance->ActivatePowerup(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Instant triggered."))
		return;
	}
	
	AActor* Actor = Cast<AActor>(OtherActor);
	if (!ensure(Actor != nullptr)) return;
	
	UActorComponent* Component = NewObject<UActorComponent>(Actor, PowerupInstance->GetPowerupClass());
	if (!ensure(Component != nullptr)) return;
	
	Component->RegisterComponent();
	UE_LOG(LogTemp, Warning, TEXT("Component Registered."));
}
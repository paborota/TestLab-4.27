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
		Respawn();
	}
}

void ASPickupActor::Respawn()
{
	if (PowerupInstance) return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
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
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnPowerupInstance, this, &ASPickupActor::Respawn, RespawnCooldown);
	}
				
	PowerupInstance->Destroy();
	PowerupInstance = nullptr;
}

void ASPickupActor::ActivatePowerup(IPowerupInterface* OtherActor)
{
    if (!PowerupInstance->GetPowerupClass())
    {
    	UE_LOG(LogTemp, Error, TEXT("This powerup was missing it's powerup class, cannot be registered."));
    	return;
    }
	
	AActor* Actor = Cast<AActor>(OtherActor);
	if (!ensure(Actor != nullptr)) return;
	
	UActorComponent* Component = NewObject<UActorComponent>(Actor, PowerupInstance->GetPowerupClass());
	if (!ensure(Component != nullptr)) return;
	
	Component->RegisterComponent();
	UE_LOG(LogTemp, Warning, TEXT("Component Registered."));
}
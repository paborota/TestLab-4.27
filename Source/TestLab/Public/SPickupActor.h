// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"


class ASPowerupActor;
class IPowerupInterface;
class UDecalComponent;
class USphereComponent;

UCLASS()
class TESTLAB_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category="PickupActor")
	TSubclassOf<ASPowerupActor> PowerupClass;

	UPROPERTY()
	ASPowerupActor* PowerupInstance;
	
	UPROPERTY(EditInstanceOnly, Category="PickupActor", meta = (ClampMin="0"))
	float RespawnCooldown;

	FTimerHandle TimerHandle_RespawnPowerupInstance;
	void Respawn();

	void VerifyActor(AActor* OtherActor);

	void ActivatePowerup(IPowerupInterface* OtherActor);

	UFUNCTION()
	void OnSphereCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
};

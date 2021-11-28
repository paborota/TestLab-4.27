// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstantPowerupBaseComponent.h"
#include "Components/PowerupBaseComponent.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

class ASCharacter;
class UPointLightComponent;
class UInstantDoubleJumpCharge;

UCLASS()
class TESTLAB_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UPointLightComponent* PointLightComp;

	UPROPERTY(VisibleAnywhere)
	UInstantPowerupBaseComponent* InstantPowerupComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	TSubclassOf<UPowerupBaseComponent> PowerupClass;

	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	TSubclassOf<UInstantPowerupBaseComponent> InstantPowerupClass;

	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	float VisualActorSpinSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	bool bHaveActorRotate;

public:

	virtual void Tick(float DeltaTime) override;
	
	bool HasValidPowerupClass() const;

	TSubclassOf<UActorComponent> GetInstantPowerupClass() const { return InstantPowerupClass; }

	TSubclassOf<UActorComponent> GetPowerupClass() const { return PowerupClass; }

	// Only gets called when the powerup is an instant-use
	void ActivatePowerup(IPowerupInterface* ActorToPowerup);
	
	// float GetPowerupSpawnCoolDown() const { return PowerupSpawnCooldown; };
};

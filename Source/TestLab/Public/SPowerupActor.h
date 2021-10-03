// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PowerupBaseComponent.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

class ASCharacter;
class UPointLightComponent;

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UPointLightComponent* PointLightComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	TSubclassOf<UPowerupBaseComponent> PowerupClass;

	// If set to 0.0f, will never respawn
	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	float PowerupSpawnCooldown;

	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	float VisualActorSpinSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	bool bHaveActorRotate;

public:

	virtual void Tick(float DeltaTime) override;
	
	TSubclassOf<UActorComponent> GetPowerupClass() const { return PowerupClass; };

	float GetPowerupSpawnCoolDown() const { return PowerupSpawnCooldown; };
};

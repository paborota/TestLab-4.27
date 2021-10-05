// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProtoMine.generated.h"

class USphereComponent;
class URadialForceComponent;

UCLASS()
class TESTLAB_API AProtoMine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProtoMine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Components")
	UStaticMeshComponent* StaticMeshComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	USphereComponent* OuterSphereCollision;

	UPROPERTY(EditDefaultsOnly, Category="Components")
	URadialForceComponent* RadialForceComp;

	UFUNCTION()
	void OnBeginOverlap_OuterSphereCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnEndOverlap_OuterSphereCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	FTimerHandle TimerHandle_Detonation;
	float TimeToDetonate;

	UFUNCTION()
	void OnHit_InnerSphereCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void Detonate();

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageRadius;

};
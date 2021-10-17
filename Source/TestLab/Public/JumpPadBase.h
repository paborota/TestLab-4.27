// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JumpPadBase.generated.h"

class USphereComponent;

UCLASS()
class TESTLAB_API AJumpPadBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJumpPadBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="JumpPad")
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="JumpPad")
	USphereComponent* SphereCollisionComp;
	

	UPROPERTY(EditInstanceOnly, Category="Launch", meta = (ClampMin="0"))
	float LaunchUpVelocity;

	UPROPERTY(EditInstanceOnly, Category="Launch", meta = (ClampMin="0"))
	float LaunchForwardVelocity;

	// Meant to be called only when the collision component is overlapped, needs to be overridden, otherwise it will just call to Launch().
	UFUNCTION()
	virtual void OnCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
	{ Launch(OtherActor); }
	
	// Current input is the base character class that takes advantage of the LaunchCharacter functionality
	void Launch(AActor* OtherActor);
	void Launch(ACharacter* OtherCharacter);

};

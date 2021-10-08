// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTLAB_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	ACharacter* OwnerAsCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	float DashVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player")
	float DashTimeLength;
	bool bDashedRecently;

	void EndDash();
	
public:	

	void Dash();

	void ResetDash() { bDashedRecently = false; }
};

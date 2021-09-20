// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomEnums.h"
#include "MantleComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentCustomStartSignature, EMantleType, Type);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTLAB_API UMantleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMantleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	ACharacter* Owner;
	
	UPROPERTY(BlueprintAssignable, Category="Custom")
	FComponentCustomStartSignature OnMantleStart;
	void MantleUpCalc(const FHitResult& Hit);
	void MantleDownCalc(const FHitResult& Hit);
	void MantleTrace();
	bool MantleTraceUp(FHitResult& Hit, const FCollisionQueryParams& Params);
	bool MantleTraceDown(FHitResult& Hit, const FCollisionQueryParams& Params);
	UPROPERTY(EditDefaultsOnly, Category="Mantle Parameters")
	float MantleTraceHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mantle Parameters")
	bool bIsMantling;
	UPROPERTY(EditDefaultsOnly, Category="Mantle Parameters")
	float MantleDownTraceHeight;
	UPROPERTY(EditDefaultsOnly, Category="Mantle Parameters")
	float MantleTraceForwardDistance;

	UPROPERTY(EditDefaultsOnly, Category="Mantle Parameters")
	float Mantle1MInterpSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Mantle Parameters")
	float Mantle2MInterpSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Mantle Parameters")
	float MantleDown1MInterpSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Mantle Parameters")
	float MantleDown2MInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Utility")
	bool bEnableMantleOffset = true;
	void OffsetMantleTransform(const float& DeltaTime);
	float CurrentOffset;
	float MantleOffset;
	float InterpSpeed;
	void ResetMantleValues();
	
	UPROPERTY(BlueprintReadWrite)
	EMantleType MantleType;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void SetIsMantling(const bool& bVal) { bIsMantling = bVal; }
	FORCEINLINE bool GetIsMantling() const { return bIsMantling; }

	UFUNCTION(BlueprintCallable)
	void SetTickEnabled(const bool& bVal) { SetComponentTickEnabled(bVal); }
	void DoMantleTrace() { MantleTrace(); }
};

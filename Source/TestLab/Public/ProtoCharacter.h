// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CustomEnums.h"
#include "GameFramework/Character.h"
#include "ProtoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCameraSmoothZAxisComponent;
class UHealthComponent;
class UInspectorComponent;
class UMantleComponent;
class AProtoInspectable;
class UDataTable;

UCLASS()
class TESTLAB_API AProtoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProtoCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UCameraSmoothZAxisComponent* CameraSmoothZComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UHealthComponent* HealthComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UMantleComponent* MantleComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UInspectorComponent* InspectorComp;
	

	// Player Controller reference
	UPROPERTY()
	APlayerController* PC;

	bool bUsingMouse;
	
	// Player Input
	void MoveForward(const float Val);
	void MoveRight(const float Val);
	void LookUp(const float Val);
	void LookRight(const float Val);
	void LookUpGamepad(const float AxisValue);
	void LookRightGamepad(const float AxisValue);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Controls")
	float MouseSensitivity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Controls")
	float ControllerRotationRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Controls")
	float InteractRange;

	// Actions
	void Interact();
	EObjectType CheckForObjectType(FHitResult& Hit);
	void PlayerClicked();
	void ClickReleased();
	void AugmentClicked();
	void AugmentReleased();
	
	void Cancel();

	virtual void Jump() override;

	void Shift();
	void ShiftReleased();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utility")
	bool bEnableJump;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Controls")
	bool bCanJump;

	bool bIsDead;
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UCameraComponent* GetCameraComponentRef() const { return CameraComp; }

	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetUsingMouse() const { return bUsingMouse; }
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	DefaultHealth = 100.0f;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = DefaultHealth;
	AActor* Owner = GetOwner();
	if (Owner)
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleDamage);
}


void UHealthComponent::HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
		return;

	// Update health clamped
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Warning, TEXT("Current health: %f"), CurrentHealth);
	
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

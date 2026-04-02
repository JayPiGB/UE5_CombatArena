#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent() : Health(100.0f), MaxHealth(100.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UHealthComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

void UHealthComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.0, MaxHealth);
}


void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


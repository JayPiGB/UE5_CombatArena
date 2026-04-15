#include "Components/StaminaComponent.h"

UStaminaComponent::UStaminaComponent() :
	Stamina(100.0f),
	MaxStamina(100.0f),
	StaminaRegen(5.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Stamina += StaminaRegen * DeltaTime;
}

void UStaminaComponent::SpendStamina(float Value)
{
	Stamina -= Value;
}

float UStaminaComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

bool UStaminaComponent::CanPerformAction(float StaminaCost) const
{
	return (Stamina - StaminaCost) > 0.0f;
}



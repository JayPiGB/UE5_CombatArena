#include "Components/StaggerComponent.h"

UStaggerComponent::UStaggerComponent() : StaggerBar(0.0f), MaxStagger(40.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UStaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UStaggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStaggerComponent::ReceiveDamage(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Stagger by: %f"), Damage);
	StaggerBar = FMath::Clamp(StaggerBar + Damage, 0.0f, MaxStagger);

}

bool UStaggerComponent::IsStaggered() const
{
	return StaggerBar == MaxStagger;
}

void UStaggerComponent::ResetStaggerBar()
{
	StaggerBar = 0.0f;
}

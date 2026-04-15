#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_COMBATARENA_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SpendStamina(float Value);

	float GetStaminaPercent() const;

	bool CanPerformAction(float StaminaCost) const;

protected:
	virtual void BeginPlay() override;

private:
	float Stamina;

	float MaxStamina;

	//per second
	float StaminaRegen;
};

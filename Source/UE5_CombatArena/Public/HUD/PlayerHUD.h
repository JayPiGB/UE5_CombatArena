// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class UE5_COMBATARENA_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);

	void SetStaminaPercent(float Percent);

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;
};

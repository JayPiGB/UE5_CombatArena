// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class UE5_COMBATARENA_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//bind component to a variable with matching name in Widget Blueprint
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
};

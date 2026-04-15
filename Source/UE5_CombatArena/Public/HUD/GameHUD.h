// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UPlayerHUD;
/**
 * 
 */
UCLASS()
class UE5_COMBATARENA_API AGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	FORCEINLINE UPlayerHUD* GetPlayerHUD() const { return PlayerHUD; }

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = Game)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY()
	UPlayerHUD* PlayerHUD;
};

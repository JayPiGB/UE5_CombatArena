#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterState.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class AItem;
class UHealthComponent;
class UHealthBarComponent;
UCLASS()
class UE5_COMBATARENA_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetOverlappingItem(AItem* Item);

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	FORCEINLINE ECharacterEquipState GetCharacterEquipState() const;

	UFUNCTION(BlueprintCallable)
	void SetOnComboWindow(bool Value);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void LookUp(float Value);

	void Turn(float Value);

	void EquipOneHanded();

	void Attack();

	void PlayAttackMontage();

	ECharacterEquipState EquipState;

	ECharacterActionState ActionState;

	FORCEINLINE bool CanAttack() const;
	FORCEINLINE bool CanMove() const;

	bool OnComboWindow = false;
};

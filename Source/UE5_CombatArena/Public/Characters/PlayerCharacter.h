#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterState.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class AItem;
UCLASS()
class UE5_COMBATARENA_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetOverlappingItem(AItem* Item);
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void LookUp(float Value);

	void Turn(float Value);

	void EquipOneHanded();

	ECharacterEquipState EquipState;

	ECharacterActionState ActionState;

	FORCEINLINE bool CanAttack() const;
	FORCEINLINE bool CanMove() const;
};

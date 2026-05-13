#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterState.h"
#include "Enemy.generated.h"

class UHealthComponent;
class UHealthBarComponent;
class UAnimMontage;
class UStaggerComponent;

UCLASS()
class UE5_COMBATARENA_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetHit(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;

	void Die();

	bool InTargetRange(AActor* Target, double AcceptanceRadius);

	void MoveToTarget(AActor* Target);

	AActor* ChoosePatrolTarget();

	void PlayHitReactMontage(const FName& SectionName);

	void DirectionalHitReact(const FVector& ImpactPoint);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;


private:
	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UStaggerComponent* StaggerComponent;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;


	/*
		Navigation
	*/
	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* CurrentPatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 200.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	float IdlePatrolTimeout = 5.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();
};

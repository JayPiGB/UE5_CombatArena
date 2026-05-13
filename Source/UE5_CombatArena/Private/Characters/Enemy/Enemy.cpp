#include "Characters/Enemy/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaggerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "UE5_CombatArena/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "NavigationSystemTypes.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include <Engine/TargetPoint.h>

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	StaggerComponent = CreateDefaultSubobject<UStaggerComponent>(TEXT("Stagger"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(HealthComponent->GetHealthPercent());
		HealthBarWidget->SetVisibility(false);
	}

	FActorSpawnParameters SpawnParams{};
	SpawnParams.Owner = this;
	ATargetPoint* initialLocationTarget = GetWorld()->SpawnActor<ATargetPoint>(this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
	PatrolTargets.Add(initialLocationTarget);
	DRAW_SPHERE(initialLocationTarget->GetActorLocation(), FColor::Blue);

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(CurrentPatrolTarget);
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		int32 sectionID = FMath::RandRange(1, 4);
		FName sectionName{};
		switch (sectionID)
		{
		case 1:
			sectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 2:
			sectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 3:
			sectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;
			break;
		case 4:
			sectionName = FName("Death4");
			DeathPose = EDeathPose::EDP_Death4;
		default:
			DeathPose = EDeathPose::EDP_Alive;
		}

		UE_LOG(LogTemp, Warning, TEXT("Death section to play: %s"), *sectionName.ToString());

		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(sectionName, DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

bool AEnemy::InTargetRange(AActor* Target, double AcceptanceRadius)
{
	if (!Target) { return false; }
	const double distance = (Target->GetActorLocation() - this->GetActorLocation()).Size();
	DRAW_SPHERE_SingeFrame(GetActorLocation());
	DRAW_SPHERE_SingeFrame(Target->GetActorLocation());

	return AcceptanceRadius >= distance;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController && Target)
	{
		FAIMoveRequest moveRequest{};
		moveRequest.SetGoalActor(Target);
		moveRequest.SetAcceptanceRadius(15.f);

		EnemyController->MoveTo(moveRequest);
	}
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> validTargets{};
	for (AActor* target : PatrolTargets)
	{
		if (target != CurrentPatrolTarget)
		{
			validTargets.AddUnique(target);
		}
	}

	if (validTargets.Num() > 0)
	{
		const int32 targetIndex = FMath::RandRange(0, validTargets.Num() - 1);
		return validTargets[targetIndex];
	}
	
	return nullptr;
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		CombatTarget = nullptr;
	}

	if (InTargetRange(CurrentPatrolTarget, PatrolRadius))
	{
		CurrentPatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(
			PatrolTimer,
			this,
			&AEnemy::PatrolTimerFinished,
			IdlePatrolTimeout
		);
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CombatTarget = EventInstigator->GetPawn();

	if (HealthComponent && HealthBarWidget)
	{
		HealthComponent->ReceiveDamage(DamageAmount);

		HealthBarWidget->SetHealthPercent(HealthComponent->GetHealthPercent());
	}

	if (StaggerComponent)
	{
		StaggerComponent->ReceiveDamage(DamageAmount);
	}

	return DamageAmount;
}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
	if (HealthComponent)
	{
		if (!HealthComponent->IsAlive())
		{
			Die();
		}
		else if (StaggerComponent && StaggerComponent->IsStaggered())
		{
			DirectionalHitReact(ImpactPoint);
			StaggerComponent->ResetStaggerBar();
		}
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector toHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	const double cos = FVector::DotProduct(forward, toHit);

	double angle = FMath::Acos(cos);
	angle = FMath::RadiansToDegrees(angle);

	//if crossProduct points down, angle is negative
	const FVector crossProduct = FVector::CrossProduct(forward, toHit);
	if (crossProduct.Z < 0)
	{
		angle *= -1.f;
	}

	FName section = FName("FromBack");
	if (angle >= -45.f && angle < 45.f) { section = FName("FromFront"); }
	else if (angle >= -135.f && angle < -45.f) { section = FName("FromLeft"); }
	else if (angle >= 45.f && angle < 135.f) { section = FName("FromRight"); }
	PlayHitReactMontage(section);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(CurrentPatrolTarget);
}

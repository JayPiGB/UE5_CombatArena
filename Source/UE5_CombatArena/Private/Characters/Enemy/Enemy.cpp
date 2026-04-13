#include "Characters/Enemy/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/HealthComponent.h"
#include "HUD/HealthBarComponent.h"
#include "UE5_CombatArena/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"

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
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(HealthComponent->GetHealthPercent());
	}
}

void AEnemy::Die()
{
	int32 sectionID = FMath::RandRange(1, 4);
	FName sectionName("Death" + FString::FromInt(sectionID));
	UE_LOG(LogTemp, Warning, TEXT("Death section to play: %s"), *sectionName.ToString());
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(sectionName, DeathMontage);
	}
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

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HealthComponent && HealthBarWidget)
	{
		HealthComponent->ReceiveDamage(DamageAmount);

		HealthBarWidget->SetHealthPercent(HealthComponent->GetHealthPercent());
	}
	return DamageAmount;
}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	if (HealthComponent && HealthComponent->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
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


	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + crossProduct * 100.f, 5.f, FColor::Blue, 5.f);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Angle: %f"), angle));
	}
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + forward * 60.f, 5.f, FColor::Red, 5.f);

	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + toHit * 60.f, 5.f, FColor::Green, 5.f);
}


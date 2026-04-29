#include "Characters/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "HUD/HealthBarComponent.h"
#include "HUD/GameHUD.h"
#include "HUD/PlayerHUD.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* movement = GetCharacterMovement();
	movement->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	movement->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerCharCameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCharCamera"));
	Camera->SetupAttachment(CameraBoom);

	OverlappingItem = nullptr;
	EquipState = ECharacterEquipState::ECES_Unequipped;
	ActionState = ECharacterActionState::ECAS_Unoccupied;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController)
	{
		AGameHUD* gameHUD = Cast<AGameHUD>(playerController->GetHUD());
		if (gameHUD)
		{
			PlayerHUD = gameHUD->GetPlayerHUD();
			if (PlayerHUD)
			{
				PlayerHUD->SetHealthPercent(HealthComponent->GetHealthPercent());
				PlayerHUD->SetStaminaPercent(StaminaComponent->GetStaminaPercent());
			}
		}
	}
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerHUD && StaminaComponent)
	{
		PlayerHUD->SetStaminaPercent(StaminaComponent->GetStaminaPercent());
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &APlayerCharacter::Turn);

	PlayerInputComponent->BindAction(FName("Jump"),EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("EquipOneHanded"), EInputEvent::IE_Pressed, this, &APlayerCharacter::EquipOneHanded);
	PlayerInputComponent->BindAction(FName("Attack"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Attack);
}



void APlayerCharacter::Attack()
{

	if (OnComboWindow)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, "Combo issued");
		PerformCombo = true;
		return;
	}

	const float staminaCost = 20.f;
	if (CanAttack(staminaCost))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Yellow, "Attack");

		ActionState = ECharacterActionState::ECAS_Attacking;
		StaminaComponent->SpendStamina(staminaCost);
		PlayerHUD->SetStaminaPercent(StaminaComponent->GetStaminaPercent());
		PlayAttackMontage();
	}
}

void APlayerCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);

		if (PerformCombo)
		{
			AnimInstance->Montage_JumpToSection(FName("Attack 2"), AttackMontage);
		}
		else
		{
			OnComboWindow = false;
			AnimInstance->Montage_JumpToSection(FName("Attack 1"), AttackMontage);
		}

	}
}

void APlayerCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
	if (GEngine)
	{
		if (Item)
		{
			FString debugMessage("Overlapping item set to: ");
			debugMessage = debugMessage + Item->GetName();
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Cyan, debugMessage);
		}
	}
}

void APlayerCharacter::AttackEnd()
{
	if (PerformCombo)
	{
		Attack();
		PerformCombo = false;
	}
	if (ActionState == ECharacterActionState::ECAS_Attacking)
	{
		ActionState = ECharacterActionState::ECAS_Unoccupied;
	}
}

void APlayerCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	UE_LOG(LogTemp, Warning, TEXT("set Collision Enabled called"));
	AWeapon* EquippedWeapon = Cast<AWeapon>(OverlappingItem);
	if (EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon verified"));
		UBoxComponent* WeaponBox = EquippedWeapon->GetWeaponBox();
		if (WeaponBox)
		{
			WeaponBox->SetCollisionEnabled(CollisionEnabled);
			UE_LOG(LogTemp, Warning, TEXT("Collision Enabled set to: %d"), WeaponBox->GetCollisionEnabled());
		}

		EquippedWeapon->IgnoreActors.Empty();
	}

}

ECharacterEquipState APlayerCharacter::GetCharacterEquipState() const
{
	return EquipState;
}

void APlayerCharacter::SetOnComboWindow(bool Value)
{
	OnComboWindow = Value;
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::EquipOneHanded()
{
	AWeapon* Weapon = Cast<AWeapon>(OverlappingItem);
	if (Weapon)
	{
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquipState = ECharacterEquipState::ECES_EquippedOneHandedWeapon;
	}
}

bool APlayerCharacter::CanAttack(float StaminaCost) const
{
	return (EquipState == ECharacterEquipState::ECES_EquippedOneHandedWeapon || EquipState == ECharacterEquipState::ECES_EquippedTwoHandedWeapon)
			&& (ActionState == ECharacterActionState::ECAS_Unoccupied || PerformCombo)
			&& StaminaComponent->CanPerformAction(StaminaCost);
}

bool APlayerCharacter::CanMove() const
{
	return ActionState != ECharacterActionState::ECAS_Attacking;
}

void APlayerCharacter::MoveForward(float Value)
{
	if (!CanMove()) { return; }

	if (Controller && (Value != 0.0f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation = FRotator(0.0f, ControlRotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (!CanMove()) { return; }

	if (Controller && (Value != 0.0f))
	{
		const FRotator controllerRotation = GetControlRotation();
		const FRotator yawRotation = FRotator(0.0f, controllerRotation.Yaw, 0.0f);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, Value);
	}
}


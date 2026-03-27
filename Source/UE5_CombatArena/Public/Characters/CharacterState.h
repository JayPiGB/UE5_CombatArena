#pragma once
UENUM(BlueprintType)
enum class ECharacterEquipState : uint8
{
	ECES_Unequipped UMETA(DisplayName = "Unequipped"),
	ECES_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECES_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class ECharacterActionState : uint8
{
	ECAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECAS_Attacking UMETA(DisplayName = "Attacking")
};

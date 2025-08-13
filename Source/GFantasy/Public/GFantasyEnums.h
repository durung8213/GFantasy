#pragma once

#include "CoreMinimal.h"
#include "GFantasyEnums.generated.h"

UENUM(BlueprintType)
enum class ETargetingState : uint8
{
	None,
	Focusing,
	LockOn
};

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None, 
	Attack,
	RangedAttack,
	Dodge,
	Mount,
	LimitBreak,
}; 

UENUM(BlueprintType)
enum class EAbilitySlot : uint8
{
	SlotQ	UMETA(DisplayName = "SlotQ"),
	SlotE	UMETA(DisplayName = "SlotE"),

};

UENUM(BlueprintType)
enum class EInputDir : uint8
{
	None, 
	Forward,
	Backward,
	Left,
	Right
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	NonCombat,
	InCombat
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/GASCombatAS.h"
#include "Net/UnrealNetwork.h"

void UGASCombatAS::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCombatAS, AttackPower, OldAttackPower);
}

void UGASCombatAS::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCombatAS, Defense, OldDefense);
}

void UGASCombatAS::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCombatAS, AttackSpeed, OldAttackSpeed);
}

void UGASCombatAS::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCombatAS, CriticalChance, OldCriticalChance);
}

void UGASCombatAS::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCombatAS, CriticalDamage, OldCriticalDamage);
}

void UGASCombatAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASCombatAS, AttackPower, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCombatAS, Defense, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCombatAS, AttackSpeed, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCombatAS, CriticalChance, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCombatAS, CriticalDamage, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

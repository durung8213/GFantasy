// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Boss/BossLilither.h"

#include "Controller/AI/Boss/Boss1.h"

void ABossLilither::OnNormalAttackDie(const FGameplayEffectSpec& EffectSpec,
                                      const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	ABoss1* CurrentContorller = Cast<ABoss1>( GetController());

	if (bIsDead) return;
	
	if (CurrentContorller)
	{
		CurrentContorller->BossOnDie();
	}

	AGASBaseCharacter* CurrentDamagePlayer = Cast<AGASPlayerCharacter>( EffectSpec.GetContext().GetInstigator());
	
}

void ABossLilither::OnStrongAttackDie(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	ABoss1* CurrentContorller = Cast<ABoss1>( GetController());

	if (bIsDead) return;
	
	if (CurrentContorller)
	{
		CurrentContorller->BossOnDie();
	}
}

void ABossLilither::SetIsDead(bool NewbIsDead)
{
	bIsDead = NewbIsDead;
}

bool ABossLilither::GetIsDead()
{
	return bIsDead;
}




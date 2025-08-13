// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Boss/BossSuccubusCharacter.h"

#include "AnimInstance/BossEnemyAnimInstanceBase.h"
#include "Controller/AI/Boss/BossSuccubusAIController.h"


void ABossSuccubusCharacter::OnNormalAttackDie(const FGameplayEffectSpec& EffectSpec,
                                               const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	ABossSuccubusAIController* CurrentController = Cast<ABossSuccubusAIController>(GetController());
	if (CurrentController)
	{
		CurrentController->BossOnDie();
	}
}

void ABossSuccubusCharacter::OnStrongAttackDie(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	ABossSuccubusAIController* CurrentController = Cast<ABossSuccubusAIController>(GetController());
	if (CurrentController)
	{
		CurrentController->BossOnDie();
	}
}

void ABossSuccubusCharacter::SetIsDead(bool NewbIsDead)
{
	bIsDead = NewbIsDead;
}

bool ABossSuccubusCharacter::GetIsDead()
{
	return bIsDead;
}


void ABossSuccubusCharacter::TimelineStart(UCurveFloat* TimeLineCurve)
{
}

void ABossSuccubusCharacter::TimelineInProgress(float Value)
{
}

void ABossSuccubusCharacter::TimelineComplete()
{
}

void ABossSuccubusCharacter::ChangeCharacterAnimInstance_Implementation(TSubclassOf<UAnimInstance> NewAnimInstance)
{
	ChangeMeshTwoPhase();
}

void ABossSuccubusCharacter::ChangeCharacterAnimPhase_Implementation(int Phase)
{
	if (UBossEnemyAnimInstanceBase* CurrentAnimInstance = Cast<UBossEnemyAnimInstanceBase>(GetMesh()->GetAnimInstance()))
	{
		CurrentAnimInstance->Phase = Phase;
	}
}

void ABossSuccubusCharacter::MultiChangeMeshTwoPhase_Implementation()
{
	ChangeMeshTwoPhase();
}

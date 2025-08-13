// Fill out your copyright notice in the Description page of Project Settings.
#include "DataAsset/PlayerSkillLibrary.h"
#include "GAS/GameplayAbility/GA_Base.h"


TSubclassOf<UGameplayAbility> UPlayerSkillLibrary::GetAbilityClass(const FGameplayTag Tag) const
{
	// SkillMap���� Tag�� ���� ��ų�� �����´�.

	const TSubclassOf<UGameplayAbility>* Found = SkillMap.Find(Tag);
	return Found? *Found : nullptr;
}

void UPlayerSkillLibrary::GetAllSkillTags(TArray<FGameplayTag>& OutTags) const
{
	SkillMap.GetKeys(OutTags);
}

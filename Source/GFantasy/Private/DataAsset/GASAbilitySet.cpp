// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/GASAbilitySet.h"
#include "AbilitySystemComponent.h"



void UGASAbilitySet::GiveAbilitiesToASC_Implementation(UAbilitySystemComponent* ASC) const
{
	if (!ASC || !ASC->GetOwner() || !ASC->GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Give Ability"));
		return;
	}

	// �迭�� �ִ� �����Ƽ�� ��� ASC�� ����Ѵ�.
	for (const FAbilitySetItem& Item : Abilities)
	{
		if (!Item.AbilityClass)
		{
			UE_LOG(LogTemp, Error, TEXT("AbilityClass is null in AbilitySet!"));
			continue;
		}

		int32 Level = Item.AbilityLevel > 0 ? Item.AbilityLevel : 1;

		// �ùٸ��� ������ ������ ȣ��
		FGameplayAbilitySpec Spec(
			Item.AbilityClass,				// ��ų UClass
			Item.AbilityLevel,              // ���� ����
			INDEX_NONE,						// SourceObject���� DataAsset ����
			nullptr
		);
		Spec.InputID = static_cast<int32>(Item.InputID);
		Spec.DynamicAbilityTags.AddTag(Item.InputTag);
		ASC->GiveAbility(Spec);
	}
}

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

	// 배열에 있는 어빌리티를 모두 ASC에 등록한다.
	for (const FAbilitySetItem& Item : Abilities)
	{
		if (!Item.AbilityClass)
		{
			UE_LOG(LogTemp, Error, TEXT("AbilityClass is null in AbilitySet!"));
			continue;
		}

		int32 Level = Item.AbilityLevel > 0 ? Item.AbilityLevel : 1;

		// 올바르게 수정된 생성자 호출
		FGameplayAbilitySpec Spec(
			Item.AbilityClass,				// 스킬 UClass
			Item.AbilityLevel,              // 계산된 레벨
			INDEX_NONE,						// SourceObject으로 DataAsset 지정
			nullptr
		);
		Spec.InputID = static_cast<int32>(Item.InputID);
		Spec.DynamicAbilityTags.AddTag(Item.InputTag);
		ASC->GiveAbility(Spec);
	}
}

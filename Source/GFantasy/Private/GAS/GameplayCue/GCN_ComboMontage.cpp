// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCue/GCN_ComboMontage.h"
#include "GameFramework/Character.h"

bool UGCN_ComboMontage::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	UObject* SourceObj = nullptr;
	if (Parameters.SourceObject.IsValid())
	{
		// const UObject* → UObject* 로 const 제거
		SourceObj = const_cast<UObject*>(Parameters.SourceObject.Get());
	}

	if (const UGASAbilityBase* Def = Cast<UGASAbilityBase>(SourceObj))
	{
		// tag 찾기
		for (const FComboSectionData& Step : Def->ComboSections)
		{
			if (Parameters.AggregatedSourceTags.HasTag(Step.ComboCueTag))
			{
				if (ACharacter* Char = Cast<ACharacter>(MyTarget))
				{
					Char->PlayAnimMontage(Def->ActionMontage, 1.f, Step.SectionName);
					return true;
				}
			}
		}
	}

	return false;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTD/BTD_HasTag.h"

#include "AIController.h"
#include "Character/GASCombatNPC.h"


bool UBTD_HasTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (OwnerComp.GetAIOwner() && OwnerComp.GetAIOwner()->GetPawn())
	{
		AGASCombatNPC* CurrentPawn = Cast<AGASCombatNPC>(OwnerComp.GetAIOwner()->GetPawn());
		if (CurrentPawn)
		{
			if (CurrentPawn->GetAbilitySystemComponent() && Tag.IsValid())
			{
				return CurrentPawn->GetAbilitySystemComponent()->HasMatchingGameplayTag(Tag);
			}
		}
	}
	
	return false;
}

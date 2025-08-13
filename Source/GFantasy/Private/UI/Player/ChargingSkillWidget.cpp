// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/ChargingSkillWidget.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"


void UChargingSkillWidget::BindToASC(UPlayerCharacterGASC* ASC)
{
	if (!ASC)	return;
	ASC->OnChargingTagChanged.AddDynamic(this, &UChargingSkillWidget::HandleUIChanged);
	
}

void UChargingSkillWidget::HandleUIChanged(FGameplayTag Tag, bool bAdded)
{
	if (Tag == FGameplayTag::RequestGameplayTag("State.Ability.Charged"))
	{
		OnCharagetSkillAvilabilityChanged(bAdded);
	}
}

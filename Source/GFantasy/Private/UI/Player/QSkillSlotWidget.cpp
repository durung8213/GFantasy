// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/QSkillSlotWidget.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"



void UQSkillSlotWidget::BindToASC(UPlayerCharacterGASC* ASC)
{
	if (!ASC)
		return;
	ASC->OnQSkillCooldownChanged.AddDynamic(this, &UQSkillSlotWidget::HandleUIChanged);
	
}

void UQSkillSlotWidget::HandleUIChanged(FGameplayTag Tag, bool bAdded)
{
	if (Tag == FGameplayTag::RequestGameplayTag("CoolDown.Skill.Sword.RainAttack"))
	{
		OnCooldownAvailabilityChanged(!bAdded);
	}
}
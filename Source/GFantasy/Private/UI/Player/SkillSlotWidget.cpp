// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/SkillSlotWidget.h"
#include "UI/Player/ChargingSkillWidget.h"
#include "UI/Player/LimitBreakWidget.h"
#include "UI/Player/QSkillSlotWidget.h"


void USkillSlotWidget::BindToASC(UPlayerCharacterGASC* ASC)
{
	if (!ASC)	return;

	if (WBP_LimitBreakSlot)
	{
		WBP_LimitBreakSlot->BindToASC(ASC);
		UE_LOG(LogTemp, Warning, TEXT("WBP_LimitBreakSlot is valid"));
	}

	if (WBP_ChargeSkillSlot)
	{
		WBP_ChargeSkillSlot->BindToASC(ASC);
		UE_LOG(LogTemp, Warning, TEXT("WBP_ChargeSkillSlot is valid"));
	}

	if (WBP_QSkillSlot)
	{
		WBP_QSkillSlot->BindToASC(ASC);
		UE_LOG(LogTemp, Warning, TEXT("QSkillSlot is valid"));
		
	}
}

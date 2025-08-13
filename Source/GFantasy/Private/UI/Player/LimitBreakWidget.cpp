// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/LimitBreakWidget.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"


void ULimitBreakWidget::BindToASC(UPlayerCharacterGASC* ASC)
{
	if (!ASC)	return;
	ASC->OnLimitBreakTagChanged.AddDynamic(this, &ULimitBreakWidget::HandleUIChanged);
	
	const FGameplayTag ReadyTag = FGameplayTag::RequestGameplayTag("State.Limit.Ready");
	const bool bAlreadyHasTag = ASC->HasMatchingGameplayTag(ReadyTag);
	HandleUIChanged(ReadyTag, bAlreadyHasTag);
}

void ULimitBreakWidget::HandleUIChanged(FGameplayTag Tag, bool bAdded)
{
	UE_LOG(LogTemp, Warning, TEXT("LimitBreakWidget::HandleUIChanged - Tag: %s, Added: %d"),
		*Tag.ToString(), bAdded);

	if (Tag == FGameplayTag::RequestGameplayTag("State.Limit.Ready"))
	{
		OnLimitBreakAvailabilityChanged(bAdded);
	}
}
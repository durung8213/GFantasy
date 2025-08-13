// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AN_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner())
		return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!ASC)
		return;

	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.Instigator = MeshComp->GetOwner();
	EventData.Target = MeshComp->GetOwner();

	ASC->HandleGameplayEvent(EventTag, &EventData);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/ANS_SendGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemBlueprintLibrary.h"

void UANS_SendGameplayEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) {

		UE_LOG(LogTemp, Warning, TEXT("1"));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	
	if (!Owner) {
		UE_LOG(LogTemp, Warning, TEXT("2"));
		return;

	}
	
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	if (!ASI) {
		UE_LOG(LogTemp, Warning, TEXT("3"));
		return;

	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) {

		UE_LOG(LogTemp, Warning, TEXT("4"));
		return;
	}

	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("1"));
		return;
	}
	
	//ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Input.Combo"));


	// Owner에게 태그를 보내서 어빌리티 쪽 이벤트 태스크가 실행되도록 한다.
	FGameplayEventData EventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Owner,
		StartEventTag,
		EventData);
	UE_LOG(LogTemp, Warning, TEXT("Send Start EventTag"));
}

void UANS_SendGameplayEvent::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)	return;

	AActor* Owner = MeshComp->GetOwner();

	if (!Owner)	return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	if (!ASI)	return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)	return;
	
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("1"));
		return;
	}

	// Owner에게 태그를 보내서 어빌리티 쪽 이벤트 태스크가 실행되도록 한다.
	FGameplayEventData EventData;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Owner, 
		EndEventTag,
		EventData);
	

	//ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Input.Combo"));
	UE_LOG(LogTemp, Warning, TEXT("Send End EventTag"));
}

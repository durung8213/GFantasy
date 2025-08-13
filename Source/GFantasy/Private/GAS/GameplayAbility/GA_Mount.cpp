// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_Mount.h"
#include "Actor/Riding/RidingComponent.h"
#include "Character/GASPlayerCharacter.h"

void UGA_Mount::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;


	UE_LOG(LogTemp, Warning, TEXT("0"));

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(ActorInfo->AvatarActor.Get());

	if (!Player)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	URidingComponent* RidingComp = Player->FindComponentByClass<URidingComponent>();
	if (!RidingComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (RidingComp->IsMounted())
	{

		UE_LOG(LogTemp, Warning, TEXT("1"));
		RidingComp->Server_UnMount();
	}
	else
	{

		UE_LOG(LogTemp, Warning, TEXT("2"));
		RidingComp->Server_Mount();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

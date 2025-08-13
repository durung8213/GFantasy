// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/PlayerSkill/GA_Parrying.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include <AbilitySystemBlueprintLibrary.h>

UGA_Parrying::UGA_Parrying()
{
	// �±� ����
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Parry.Active")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Parry.Active")));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Parrying::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	UKismetSystemLibrary::PrintString(this, TEXT("PARRIED"));

	AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	FVector Location = Char->GetActorLocation();

	// ���� �и��� ������ ĳ������ X �������� ( �չ��� ) 50 ��ŭ ������ ��ġ�� VFX�� ����
	FGameplayCueParameters Params;
	Params.Location = GetAvatarActorFromActorInfo()->GetActorLocation() + FVector(50, 0, 0);

	// GameplayCue �� Ȱ���ؼ� �и� VFX ����
	GetAbilitySystemComponentFromActorInfo()->AddGameplayCue(
		FGameplayTag::RequestGameplayTag("GameplayCue.Parry.HitEffect"),
		Params
	);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Parrying::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

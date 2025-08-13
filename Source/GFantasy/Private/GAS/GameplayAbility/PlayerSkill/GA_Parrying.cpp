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
	// 태그 설정
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Parry.Active")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Parry.Active")));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Parrying::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	UKismetSystemLibrary::PrintString(this, TEXT("PARRIED"));

	AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	FVector Location = Char->GetActorLocation();

	// 현재 패링을 실행할 캐릭터의 X 방향으로 ( 앞방향 ) 50 만큼 떨어진 위치에 VFX를 생성
	FGameplayCueParameters Params;
	Params.Location = GetAvatarActorFromActorInfo()->GetActorLocation() + FVector(50, 0, 0);

	// GameplayCue 을 활용해서 패링 VFX 실행
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

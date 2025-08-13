// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayAbility/GA_EndDelegateBase.h"

#include "AIController.h"
#include "Character/GASCombatNPC.h"
#include "Controller/AIControllerBase.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UGA_EndDelegateBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UGA_EndDelegateBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	EndAbilityDelegate.Broadcast();
}

AAIControllerBase* UGA_EndDelegateBase::GetCurrentController()
{
	ACharacter* CurrentCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (CurrentCharacter && CurrentCharacter->GetController())
	{
		AAIControllerBase* CurrentController = Cast<AAIControllerBase>(CurrentCharacter->GetController());
		return CurrentController;
	}

	return nullptr;
	
}

void UGA_EndDelegateBase::ApplyEffectToTarget(UAbilitySystemComponent* Source, UAbilitySystemComponent* Target, TSubclassOf<UGameplayEffect> Effect)
{
	if (Target && Source)
	{
		// 피격 Effect를 적용시키기
		if (Effect)
		{
			// 1. 효과 컨텍스트 생성
			FGameplayEffectContextHandle EffectContext = Source->MakeEffectContext();
			EffectContext.AddInstigator(Source->GetOwner(), Source->GetOwner());
			EffectContext.AddSourceObject(Source->GetOwner());

			// 2. 효과 스펙 생성
			FGameplayEffectSpecHandle SpecHandle = Source->MakeOutgoingSpec(
				Effect,
				1.0f, // 레벨
				EffectContext
			);

			if (SpecHandle.IsValid())
			{
				FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
				if (Spec)
				{
						
					FActiveGameplayEffectHandle EffectHandle = Source->ApplyGameplayEffectSpecToTarget(*Spec, Target);
					if (EffectHandle.IsValid())
					{
						UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
					}
					else
					{
						UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true, FLinearColor::Red);
					}
				}
				else
				{
					UKismetSystemLibrary::PrintString(this, TEXT("Spec 없음"), true, true, FLinearColor::Red);
				}
			}
			else
			{
				UKismetSystemLibrary::PrintString(this, TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
			}
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("AttackEffect 없음"), true, true, FLinearColor::Red);
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("피격 액터가 ASC 없음"), true, true, FLinearColor::Red);
	}

}

ABaseWeapon* UGA_EndDelegateBase::GetAvatarActorWeapon()
{
	AGASCombatNPC* CurrentPawn = Cast<AGASCombatNPC>(GetAvatarActorFromActorInfo());
	if (CurrentPawn)
	{
		return CurrentPawn->GetWeapon();
	}
	else
	{
		return nullptr;
	}
}

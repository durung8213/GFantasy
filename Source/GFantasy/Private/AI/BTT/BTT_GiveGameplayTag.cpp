// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_GiveGameplayTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(BTT_GiveGameplayTag);

UBTT_GiveGameplayTag::UBTT_GiveGameplayTag()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_GiveGameplayTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetAIOwner() && OwnerComp.GetAIOwner()->GetPawn())
	{
		UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn());
		if (OwnerASC && Effect)
		{
			// Effect를 적용시키기
			// 1. 효과 컨텍스트 생성
			FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
			EffectContext.AddInstigator(OwnerASC->GetOwner(), OwnerASC->GetOwner());
			EffectContext.AddSourceObject(OwnerASC->GetOwner());

			// 2. 효과 스펙 생성
			FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(
				Effect,
				EffectLevel, // 레벨
				EffectContext
			);

			if (SpecHandle.IsValid())
			{
				FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
				if (Spec)
				{
						
					FActiveGameplayEffectHandle EffectHandle = OwnerASC->ApplyGameplayEffectSpecToTarget(*Spec, OwnerASC);
					if (EffectHandle.IsValid())
					{
						UE_LOG(BTT_GiveGameplayTag, Warning, TEXT("ApplyGameplayEffectSpecToTarget Success"));
						//UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
					}
					else
					{
						UE_LOG(BTT_GiveGameplayTag, Error, TEXT("ApplyGameplayEffectSpecToTarget fail"));
						//UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true, FLinearColor::Red);
					}
				}
				else
				{
					UE_LOG(BTT_GiveGameplayTag, Error, TEXT("Spec is not valid"));
					//UKismetSystemLibrary::PrintString(this, TEXT("Spec 없음"), true, true, FLinearColor::Red);
				}
			}
			else
			{
				UE_LOG(BTT_GiveGameplayTag, Error, TEXT("SpecHandle is not valid"));
				//UKismetSystemLibrary::PrintString(this, TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
			}
		}
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

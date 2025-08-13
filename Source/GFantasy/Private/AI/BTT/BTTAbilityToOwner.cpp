// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTTAbilityToOwner.h"

#include "Character/GASCombatBossNPC.h"
#include "Character/GASCombatNPC.h"
#include "Controller/AIControllerBase.h"
#include "Controller/AI/Boss/BossAIControllerBase.h"
#include "GAS/GamePlayTag.h"
#include "GAS/GameplayAbility/GA_EndDelegateBase.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(BTT_AbilityToOwner);

UBTTAbilityToOwner::UBTTAbilityToOwner()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTAbilityToOwner::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TaskBehaviorTreeComponent = &OwnerComp;
	//UKismetSystemLibrary::PrintString(this, TEXT("Task Start"));
	GetWorld()->GetTimerManager().ClearTimer(ReleaseDeadlock);
	GetWorld()->GetTimerManager().SetTimer(
		ReleaseDeadlock,
		FTimerDelegate::CreateLambda([this]()
			{
				UE_LOG(BTT_AbilityToOwner, Warning, TEXT("Ability Deadlock And End"));
				//UKismetSystemLibrary::PrintString(this, TEXT("교착상태 발생으로 인한 Task 종료"));
				FinishLatentTask(*TaskBehaviorTreeComponent, EBTNodeResult::Failed);
			}),
		ReleaseDeadlockTime,
		false
		);
	
	AAIControllerBase* CurrentAIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	
	if (CurrentAIController)
	{
		if (AGASCombatNPC* CurrentPawn = Cast<AGASCombatNPC>( CurrentAIController->GetPawn()))
		{
			for (FGameplayAbilitySpec& Spec : CurrentPawn->GetAbilitySystemComponent()->GetActivatableAbilities())
			{
				if (Spec.Ability && Spec.Ability->AbilityTags.HasTagExact(AbilityTag))
				{
					// 이 Spec은 원하는 태그를 가진 어빌리티입니다
					FGameplayAbilitySpecHandle FoundHandle = Spec.Handle;
					UAbilitySystemComponent* CurrentACS =  CurrentPawn->GetAbilitySystemComponent();
					CurrentACS->TryActivateAbility(FoundHandle);
					
					UGameplayAbility* AbilityInstance = Spec.GetPrimaryInstance(); // 반드시 Instanced액터를 사용해야함
					
					if (UGA_EndDelegateBase* CurrentAbility = Cast<UGA_EndDelegateBase>(AbilityInstance))
					{
						if (!CurrentAbility->EndAbilityDelegate.IsAlreadyBound(this, &UBTTAbilityToOwner::EndAbilityDelegateBindFunc))
						{
							// 중복 바인딩 방지
							CurrentAbility->EndAbilityDelegate.AddDynamic(this, &UBTTAbilityToOwner::EndAbilityDelegateBindFunc);
						}
					}
					else {
						if (AbilityInstance)
						{
							UE_LOG(BTT_AbilityToOwner, Warning, TEXT("AbilityInstance Cast to UGA_EndDelegateBase fail %s"),  *AbilityInstance->GetClass()->GetName());
							//UKismetSystemLibrary::PrintString(this,  FString::Printf(TEXT("캐스트 실패 AbilityInstance class: %s"), *AbilityInstance->GetClass()->GetName()));
						}
						else
						{
							UE_LOG(BTT_AbilityToOwner, Warning, TEXT("Get AbilityInstance fail"));
							//UKismetSystemLibrary::PrintString(this,TEXT("AbilityInstance가 NULL임"));	
						}
					}

					return EBTNodeResult::InProgress;
				}
				else
				{
					//FinishLatentTask(*TaskBehaviorTreeComponent, EBTNodeResult::Succeeded);
					//UKismetSystemLibrary::PrintString(this, TEXT("어빌리티 없음"));
				}
			}

			
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("캐릭터 없음"));
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("컨트롤러 없음"));
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTAbilityToOwner::EndAbilityDelegateBindFunc()
{
	GetWorld()->GetTimerManager().ClearTimer(ReleaseDeadlock);
	//UKismetSystemLibrary::PrintString(this, TEXT("어빌리티 종료로 인한 타스크 종료"));
	FinishLatentTask(*TaskBehaviorTreeComponent, EBTNodeResult::Succeeded);
}

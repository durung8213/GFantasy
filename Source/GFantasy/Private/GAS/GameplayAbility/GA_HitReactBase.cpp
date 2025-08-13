// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_HitReactBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


void UGA_HitReactBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 비용 스태미나 등 자동 커밋
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	
    }

    UAnimMontage* LoadedMontage = HitReactMontage.LoadSynchronous();

    if (LoadedMontage)
    {
        // CreatePlayMontageAndWaitProxy(OwnerAbility, TaskInstanceName, Montage, Rate, Section, StartTime, bStopOnEnd)
        UAbilityTask_PlayMontageAndWait* Task =
            UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                this,
                TEXT("HitReactTask"),
                LoadedMontage,
                /*Rate=*/1.f,
                SectionName,
                /*StartTime=*/0.f,
                /*bStopWhenAbilityEnds=*/true
            );

        // 3) Task 완료/인터럽트/블렌드아웃 모두 동일하게 처리
        Task->OnCompleted.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);
        Task->OnInterrupted.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);
        Task->OnCancelled.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);
        Task->OnBlendOut.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);

        // 4) Task 발동
        Task->ReadyForActivation();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No MOntage, Instance"));
        // 몽타주 없으면 그냥 종료
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void UGA_HitReactBase::OnMontageEnded()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}




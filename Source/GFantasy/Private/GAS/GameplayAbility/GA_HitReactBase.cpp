// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_HitReactBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


void UGA_HitReactBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// ��� ���¹̳� �� �ڵ� Ŀ��
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

        // 3) Task �Ϸ�/���ͷ�Ʈ/����ƿ� ��� �����ϰ� ó��
        Task->OnCompleted.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);
        Task->OnInterrupted.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);
        Task->OnCancelled.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);
        Task->OnBlendOut.AddDynamic(this, &UGA_HitReactBase::OnMontageEnded);

        // 4) Task �ߵ�
        Task->ReadyForActivation();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No MOntage, Instance"));
        // ��Ÿ�� ������ �׳� ����
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void UGA_HitReactBase::OnMontageEnded()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}




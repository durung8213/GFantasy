// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayAbility/GA_MountCall.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actor/Riding/RidingComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "Controller/GASWorldPlayerController.h"


UGA_MountCall::UGA_MountCall()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Mount.MountCall")));
}

void UGA_MountCall::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	
	// 아무것도 실행할 수 없는 IMC 적용
	Player->ApplyIMC(IMC_None,0);
			
	CurrentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, CallMontage,1.f);
	CurrentMontageTask->OnCompleted.AddDynamic(this, &UGA_MountCall::OnMontageCompleted);
	CurrentMontageTask->ReadyForActivation();
}

void UGA_MountCall::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (CurrentMontageTask)
		CurrentMontageTask->EndTask();
}

void UGA_MountCall::OnMontageCompleted()
{
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(Player->GetController());
	if (PC)
	{
		PC->Client_SetIMCForDoNotMove();
	}
	
	// 말 생성만 실행
	if (URidingComponent* RC = Player->GetRidingComponent())
	{
		//RC->Server_Mount();
		// 말 생성하고 디졸브 시작
		if (HasAuthority(&CurrentActivationInfo))
		{
			RC->Server_SpawnMountAtFront();
			UAbilitySystemComponent* PlayerASC = GetAbilitySystemComponentFromActorInfo();
			// 탑승 애니메이션 어빌리티 실행
			FGameplayTag RideAnimTag = FGameplayTag::RequestGameplayTag("Ability.Mount.Ride");
			if (PlayerASC)
			{
				bool bSuccess = PlayerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(RideAnimTag));
				if (!bSuccess)
				{
					UE_LOG(LogTemp, Error, TEXT("Ride Ability Not Activated!"));
				}
				
			}

		}
	}

	if (CurrentMontageTask)
		CurrentMontageTask->EndTask();

	// 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}

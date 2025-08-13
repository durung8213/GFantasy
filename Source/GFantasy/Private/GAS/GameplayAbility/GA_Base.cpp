// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_Base.h"
#include "Character/GASPlayerCharacter.h"

UGA_Base::UGA_Base()
{
	// 현재 어빌리티가 실행될 때, 다른 어빌리티가 방해하지 않도록 하는 BlockedTag 추가
	
	// Block 태그에 해당되는 태그를 지님. ( 다른 어빌리티 차단 ) 
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat"));

	// 전투 시, 다른 어빌리티 Block하기 위한 Block 태그 추가
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat"));


	// 플레이어가 죽을 시, 어빌리티 모두 Block
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead.Player"));


	//ActivationRequiredTags : 어빌리티를 실행할 때 해당 Tag가 있어야함
}
//
// FGameplayTagContainer UGA_Base::GetActivationOwnedTags()
// {
// 	return ActivationOwnedTags;
// }

void UGA_Base::RotateToLockOnTaret()
{
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (Player && Player->TargetingState == ETargetingState::LockOn && Player->CurrentTarget)
	{
		FVector ToTarget = (Player->CurrentTarget->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal2D();
		FRotator Rot = ToTarget.Rotation();
		Player->SetActorRotation(Rot);

		if (Player->Controller)
		{
			// 카메라 기준 회전.
			//Player->Controller->SetControlRotation(Rot);
		}

	}
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_Base.h"
#include "Character/GASPlayerCharacter.h"

UGA_Base::UGA_Base()
{
	// ���� �����Ƽ�� ����� ��, �ٸ� �����Ƽ�� �������� �ʵ��� �ϴ� BlockedTag �߰�
	
	// Block �±׿� �ش�Ǵ� �±׸� ����. ( �ٸ� �����Ƽ ���� ) 
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat"));

	// ���� ��, �ٸ� �����Ƽ Block�ϱ� ���� Block �±� �߰�
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat"));


	// �÷��̾ ���� ��, �����Ƽ ��� Block
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead.Player"));


	//ActivationRequiredTags : �����Ƽ�� ������ �� �ش� Tag�� �־����
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
			// ī�޶� ���� ȸ��.
			//Player->Controller->SetControlRotation(Rot);
		}

	}
}

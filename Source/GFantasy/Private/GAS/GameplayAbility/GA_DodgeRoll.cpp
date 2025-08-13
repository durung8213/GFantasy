// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_DodgeRoll.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffect.h"
#include "Character/GASPlayerCharacter.h"


UGA_DodgeRoll::UGA_DodgeRoll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Dodge")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dodge.Invincible")));
}

void UGA_DodgeRoll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}


	// ���� �����Ƽ �������� ĳ���� ��������
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Player)
		return;

	//FVector InputDir = Player->CachedInputDirection;

	//if (!InputDir.IsNearlyZero())
	//{
	//	InputDir = InputDir.GetSafeNormal();

	//	FRotator DirRot = InputDir.Rotation();
	//	Player->SetActorRotation(DirRot);
	//}

	FVector InputDir = Cast<UPlayerCharacterGASC>(CurrentActorInfo->AbilitySystemComponent)->InputDirection;
	FRotator CharRot = CurrentActorInfo->AvatarActor->GetActorRotation();
	FVector LocalDir = CharRot.UnrotateVector(InputDir).GetSafeNormal();

	// Angle ���
	float AngleRad = FMath::Atan2(LocalDir.Y, LocalDir.X);
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);
	if (AngleDeg < 0)
	{
		AngleDeg += 360.f;
	}

	//8���� ������
	int32 DirIndex = FMath::RoundToInt(AngleDeg / 45.f) % 8;

	// ȸ�� �ִϸ��̼� ����

	FName SectionName = GetSectionNameByIndex(DirIndex);

	if (!DodgeMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	//���� ����Ʈ ����
	if (InvincibleEffect)
	{
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, InvincibleEffect.GetDefaultObject(), 1.0f);
	}

	// ��Ʈ ��� ��Ÿ�� ����
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		DodgeMontage,
		1.f,
		SectionName
	);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_DodgeRoll::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_DodgeRoll::OnMontageCancelled);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_DodgeRoll::OnMontageCancelled);
	MontageTask->ReadyForActivation();
}

void UGA_DodgeRoll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_DodgeRoll::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_DodgeRoll::OnMontageCancelled()
{
	if (MontageTask)
	{
		MontageTask->EndTask();  // EndTask�����δ� Montage�� Stop ���� ���� ���� ����
	}
	//
	// UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();
	// if (AnimInstance && AnimInstance->Montage_IsPlaying(DodgeMontage))
	// {
	// 	AnimInstance->Montage_Stop(0.1f);  // �߰� Stop ���
	// }
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FName UGA_DodgeRoll::GetSectionNameByIndex(const int32 InputIndex)
{
	static const TArray<FName> SectionNames = {
		"Dodge_Fwd",
		"Dodge_FwdRight",
		"Dodge_Right",
		"Dodge_BackRight",
		"Dodge_Back",
		"Dodge_BackLeft",
		"Dodge_Left",
		"Dodge_FwdLeft"
	};

	if (SectionNames.IsValidIndex(InputIndex))
	{
		return SectionNames[InputIndex];
	}

	return NAME_None;
}

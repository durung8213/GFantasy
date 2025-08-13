// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameplayAbility/GA_RangedCombo.h"
#include "Character/GASPlayerCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/AnimInstance/GASPlayerAnimInstance.h"
#include "Components/SphereComponent.h"


UGA_RangedCombo::UGA_RangedCombo()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Ranged")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Ranged")));
}

void UGA_RangedCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	RangedAttackCombo();

}

void UGA_RangedCombo::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_RangedCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}

	if (ShootEventTask)
	{
		ShootEventTask->EndTask();
		ShootEventTask = nullptr;
	}

	//AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	//if (Player && Player->HasAuthority())
	//{
	//	// 슬롯 blend 꺼주기
	//	Player->Multicast_PlayUpperBodyMontage(nullptr, /*bBlendOn=*/false);
	//}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_RangedCombo::RangedAttackCombo()
{
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());

	if (!RangedAttackMontage)
		return;

	// ─ 서버 역할 ───────────────────────────────────────
	if (Player->HasAuthority())
	{
		// true로 시작
		//Player->Multicast_PlayUpperBodyMontage(RangedAttackMontage, /*bBlendOn=*/true);
	}

	// 일반 몽타주 실행.
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, RangedAttackMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_RangedCombo::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_RangedCombo::OnMontageCanceled);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_RangedCombo::OnMontageCanceled);
	MontageTask->ReadyForActivation();


	// 투사체 발사 시점 감지
	ShootEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Attack.Ranged"),
		nullptr,
		true,
		true
	);
	ShootEventTask->EventReceived.AddDynamic(this, &UGA_RangedCombo::OnShootEventReceived);
	ShootEventTask->ReadyForActivation();

}

void UGA_RangedCombo::OnMontageCanceled() 
{
	//AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	//if (Player && Player->HasAuthority())
	//{
	//	// 슬롯 blend 꺼주기
	//	Player->Multicast_PlayUpperBodyMontage(nullptr, /*bBlendOn=*/false);
	//}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_RangedCombo::OnMontageCompleted()
{
	//AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	//if (Player && Player->HasAuthority())
	//{
	//	// 슬롯 blend 꺼주기
	//	Player->Multicast_PlayUpperBodyMontage(nullptr, /*bBlendOn=*/false);
	//}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_RangedCombo::OnShootEventReceived(FGameplayEventData Payload)
{
	// 서버에서만 투사체가 생성되도록 설정.
	if (!HasAuthority(&CurrentActivationInfo))
		return;

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());

	if (!ProjectileClass || !Player)
		return;

	FVector SpawnLocation = Player->GetMesh()->GetSocketLocation(MuzzleSocket);
	FRotator SpawnRotation;

	if (Player->CurrentTarget)
	{
		FVector ToTarget = (Player->CurrentTarget->GetActorLocation() - SpawnLocation).GetSafeNormal();
		SpawnRotation = ToTarget.Rotation();
	}
	else
	{
		FVector Forward = Player->GetActorForwardVector();
		SpawnRotation = Forward.Rotation();
	}

	FActorSpawnParameters Params;
	Params.Owner = Player;
	Params.Instigator = Player;

	ARangedProjectile* Projectile = GetWorld()->SpawnActor<ARangedProjectile>
		(
			ProjectileClass,
			SpawnLocation,
			SpawnRotation,
			Params
		);
	
	if (Projectile)
	{
		Projectile->Collision->IgnoreActorWhenMoving(Player, true);
		Projectile->SetActorRotation(SpawnRotation);

		if (Player->CurrentTarget)
		{
			Projectile->SetHomingTarget(Player->CurrentTarget);
		}
	}

}

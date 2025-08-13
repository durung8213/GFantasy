// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/NotifyState/EnemyAttackAnimNotifyState.h"

#include "Actor/Weapon/BaseWeapon.h"
#include "Character/GASCombatNPC.h"
#include "Kismet/KismetSystemLibrary.h"

void UEnemyAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (AGASCombatNPC* Owner = Cast<AGASCombatNPC>(MeshComp->GetOwner()))
	{
		// 매시 컴포넌트의 부모가 AGASCombatNPC면
		if (Owner->GetWeapon())
		{
			Owner->GetWeapon()->AttackEffectIndex = AttackEffectIndex;
			Owner->GetWeapon()->SetCollisionActive();	// 무기 콜리전 켜기
			Owner->GetWeapon()->ClearHitActorsArr();	// 기존 쳐 맞은 사람 배열 제거
			
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("무기 없음"));
		}
	}
}

void UEnemyAttackAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UEnemyAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	// if (MeshComp->GetOwner())
	// {
	// 	if (MeshComp->GetOwner()->HasAuthority())
	// 	{
	// 		UKismetSystemLibrary::PrintString(MeshComp->GetOwner(), TEXT("서버에서 무기 콜리전 꺼짐"));
	// 	}
	// 	else
	// 	{
	// 		UKismetSystemLibrary::PrintString(MeshComp->GetOwner(), TEXT("클라에서 무기 콜리전 꺼짐"));
	// 	}
	// }
	
	if (AGASCombatNPC* Owner = Cast<AGASCombatNPC>(MeshComp->GetOwner()))
	{
		// 매시 컴포넌트의 부모가 AGASCombatNPC면
		if (Owner->GetWeapon())
		{
			Owner->GetWeapon()->SetCollisionInactive();	// 더이상 때리지 못하게 함
			Owner->GetWeapon()->ClearHitActorsArr();	// 기존 쳐 맞은 사람 배열 제거
		}
	}
}

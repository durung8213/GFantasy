// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/NotifyState/ANS_ActiveWeaponByIndex.h"

#include "Actor/Weapon/BaseWeapon.h"
#include "Character/GASCombatNPC.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LOG_ANS_ActiveWeaponByIndex);

void UANS_ActiveWeaponByIndex::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AGASCombatNPC* Owner = Cast<AGASCombatNPC>(MeshComp->GetOwner()))
	{
		// 매시 컴포넌트의 부모가 AGASCombatNPC면
		if (Owner->WeaponArr.IsValidIndex(WeaponIndex) && Owner->WeaponArr[WeaponIndex])
		{
			Owner->WeaponArr[WeaponIndex]->AttackEffectIndex = AttackEffectIndex;
			Owner->WeaponArr[WeaponIndex]->SetCollisionActive();	// 무기 콜리전 켜기
			Owner->WeaponArr[WeaponIndex]->ClearHitActorsArr();	// 기존 쳐 맞은 사람 배열 제거
			
		}
		else
		{
			UE_LOG(LOG_ANS_ActiveWeaponByIndex, Warning, TEXT("No Weapon In Arr Index %d"), WeaponIndex);
			//UKismetSystemLibrary::PrintString(this, TEXT("무기 없음"));
		}
	}
}

void UANS_ActiveWeaponByIndex::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UANS_ActiveWeaponByIndex::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AGASCombatNPC* Owner = Cast<AGASCombatNPC>(MeshComp->GetOwner()))
	{
		// 매시 컴포넌트의 부모가 AGASCombatNPC면
		if (Owner->WeaponArr.IsValidIndex(WeaponIndex) && Owner->WeaponArr[WeaponIndex])
		{
			Owner->WeaponArr[WeaponIndex]->SetCollisionInactive();	// 더이상 때리지 못하게 함
			Owner->WeaponArr[WeaponIndex]->ClearHitActorsArr();	// 기존 쳐 맞은 사람 배열 제거
		}
	}
}

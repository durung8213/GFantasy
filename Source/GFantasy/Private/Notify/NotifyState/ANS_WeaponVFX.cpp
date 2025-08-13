// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/NotifyState/ANS_WeaponVFX.h"

#include "NiagaraFunctionLibrary.h"
#include "Actor/Weapon/BaseWeapon.h"
#include "Character/GASCombatNPC.h"


void UANS_WeaponVFX::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (AGASCombatNPC* CurrentPawn = Cast<AGASCombatNPC>(MeshComp->GetOwner()))
	{
		if (CurrentPawn->WeaponArr.IsValidIndex(WeaponIndex) && CurrentPawn->WeaponArr[WeaponIndex])
		{
			SelectWeapon = CurrentPawn->WeaponArr[WeaponIndex];
			CurrentPawn->WeaponArr[WeaponIndex]->ActivateTrail();
		}
	}
}

void UANS_WeaponVFX::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	
}

void UANS_WeaponVFX::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (SelectWeapon)
	{
		SelectWeapon->DeactivateTrail();
	}
}

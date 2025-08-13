// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Animation/ANS_SendLockOnState.h"
#include "Character/GASPlayerCharacter.h"

void UANS_SendLockOnState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (auto* PC = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		//PC->bIsDodging = true;
	}
}

void UANS_SendLockOnState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (auto* PC = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		//PC->bIsDodging = false;
	}
}

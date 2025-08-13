// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/ANS_LimitBreakVFX.h"

#include "Character/GASPlayerCharacter.h"
#include "Character/Weapon/SwordWeapon.h"


void UANS_LimitBreakVFX::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
		return;

	AGASPlayerCharacter* Owner = Cast<AGASPlayerCharacter>(MeshComp->GetOwner());
	if (!Owner)	return;

	ASwordWeapon* EquippedWeapon = Cast<ASwordWeapon>(Owner->CurrentWeapon);
	
	if (EquippedWeapon)
	{
		EquippedWeapon->ActivateLimitVFX();
		UE_LOG(LogTemp, Error, TEXT("Activate EquippedWeapon"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EquippedWeapon is NULL"));
	}
}

void UANS_LimitBreakVFX::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;

	AGASPlayerCharacter* Owner = Cast<AGASPlayerCharacter>(MeshComp->GetOwner());
	if (!Owner)	return;
	
	ASwordWeapon* EquippedWeapon = Cast<ASwordWeapon>(Owner->CurrentWeapon);
	
	if (EquippedWeapon)
	{
		EquippedWeapon->DeactivateLimitVFX();
		UE_LOG(LogTemp, Error, TEXT("Activate EquippedWeapon"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EquippedWeapon is NULL"));
	}
}

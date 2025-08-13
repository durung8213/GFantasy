// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Animation/ANS_TrailEffect.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "Character/Weapon/SwordWeapon.h"


void UANS_TrailEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
		return;

	AGASPlayerCharacter* Owner = Cast<AGASPlayerCharacter>(MeshComp->GetOwner());
	if (!Owner)	return;

	AWeaponBase* EquippedWeapon = Owner->CurrentWeapon;
	
	if (EquippedWeapon)
	{
		EquippedWeapon->ActivateTrail();
		//UE_LOG(LogTemp, Error, TEXT("Activate EquippedWeapon"));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("EquippedWeapon is NULL"));
	}

}

void UANS_TrailEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
		return;

	AGASPlayerCharacter* Owner = Cast<AGASPlayerCharacter>(MeshComp->GetOwner());
	if (!Owner)	return;

	AWeaponBase* EquippedWeapon = Owner->CurrentWeapon;

	if (EquippedWeapon)
	{
		EquippedWeapon->DeactivateTrail();
		//UE_LOG(LogTemp, Error, TEXT("DeactivateTrail EquippedWeapon"));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("EquippedWeapon is NULL"));
	}
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Animation/ANS_EquipWeapon.h"
#include "Character/GASPlayerCharacter.h"
#include "Character/Weapon/WeaponBase.h"

void UANS_EquipWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		Player->AttachWeaponToSocket(SocketName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player In Notify : WeaponAttack"));
	}
	
}

void UANS_EquipWeapon::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void UANS_EquipWeapon::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	if (AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		if(Player->CurrentWeapon)
		{
			DrawDebugSphere(GetWorld(), Player->CurrentWeapon->GetActorLocation(), 5.f, 12, FColor::Red);
		}
		else
		{
			UE_LOG(LogTemp,Warning, TEXT("No Current Weapon : In Notify"));
		}
	}
}

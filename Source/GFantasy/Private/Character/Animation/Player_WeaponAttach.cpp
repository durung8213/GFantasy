// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Animation/Player_WeaponAttach.h"
#include "Character/GASPlayerCharacter.h"


void UPlayer_WeaponAttach::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		Player->AttachWeaponToSocket(SocketName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player In Notify : WeaponAttack"));
	}
	
}

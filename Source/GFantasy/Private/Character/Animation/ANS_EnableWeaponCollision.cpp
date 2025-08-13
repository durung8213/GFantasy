// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Animation/ANS_EnableWeaponCollision.h"
#include "Character/Weapon/WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

void UANS_EnableWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
		return;
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
		return;

	// ���� ���͸� ã�� EnableHitCollision ȣ��
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		if (AWeaponBase* Weapon = Cast<AWeaponBase>(Actor))
		{
			Weapon->EnableHitCollision();
			break;
		}
	}

}

void UANS_EnableWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
		return;


	// ���� ���͸� ã�� EnableHitCollision ȣ��
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		if (AWeaponBase* Weapon = Cast<AWeaponBase>(Actor))
		{
			Weapon->DisableHitCollision();
			break;
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/Enemy/RandomIdleEndAnimNotify.h"

#include "Character/Enemy/NormalEnemy/NormalEnemyBase.h"


void URandomIdleEndAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner()) {
		ANormalEnemyBase* CurrentEnemy = Cast<ANormalEnemyBase>(MeshComp->GetOwner());
		if (CurrentEnemy) {
			CurrentEnemy->RandomIdleEnd();
		}
	}
}

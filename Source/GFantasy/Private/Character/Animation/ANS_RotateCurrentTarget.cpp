// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/ANS_RotateCurrentTarget.h"

#include "Character/GASPlayerCharacter.h"


void UANS_RotateCurrentTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		// 캐릭터의 현재 캐싱된 입력 방향을 가져옴
		FVector Dir = Char->CachedInputDirection;

		// 현재 포커싱된 적이 있을 때,
		if (Char->CurrentTarget)
		{
			// 적을 바라보도록 캐릭터를 회전시킨다.
			FVector ToTarget = (Char->CurrentTarget->GetActorLocation() - Char->GetActorLocation()).GetSafeNormal2D();
			FRotator Rot = ToTarget.Rotation(); 
			Char->GetRootComponent()->SetWorldRotation(Rot);
			return;
		}
		// 적이 없을 경우
		else
		{
			// 현재 캐싱된 방향이 유효하다면, 해당 방향으로 캐릭터를 회전.
			if (!Dir.IsNearlyZero())
			{
				// 한 번만 즉시 회전
				Char->GetRootComponent()->SetWorldRotation(Dir.Rotation());
				//SetActorRotation(Dir.Rotation());
			}
		}

	}
}

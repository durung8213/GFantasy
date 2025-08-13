// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Animation/ANS_UpdatePlayerDirection.h"
#include "Character/GASPlayerCharacter.h"


void UANS_UpdatePlayerDirection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}

void UANS_UpdatePlayerDirection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		// 입력된 방향을 가져옴.
		FVector Dir = Char->CachedInputDirection;

		// 현재 캐릭터가 락온 상태이고, 타겟이 유효할 때
		if (Char->TargetingState == ETargetingState::LockOn && Char->CurrentTarget)
		{
			FVector ToTarget = (Char->CurrentTarget->GetActorLocation() - Char->GetActorLocation()).GetSafeNormal2D();
			FRotator Rot = ToTarget.Rotation(); 

			// 입력된 방향과 무관하게 해당 타겟을 바라보도록 회전시킴.
			Char->GetRootComponent()->SetWorldRotation(Rot);
			return;
		}

		// 방향값이 유효할 때 
		if (!Dir.IsNearlyZero())
		{
			// 입력된 방향값으로 즉시 회전한 후, 공격 애니메이션 실행
			Char->GetRootComponent()->SetWorldRotation(Dir.Rotation());
		}
	}
}

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
		// �Էµ� ������ ������.
		FVector Dir = Char->CachedInputDirection;

		// ���� ĳ���Ͱ� ���� �����̰�, Ÿ���� ��ȿ�� ��
		if (Char->TargetingState == ETargetingState::LockOn && Char->CurrentTarget)
		{
			FVector ToTarget = (Char->CurrentTarget->GetActorLocation() - Char->GetActorLocation()).GetSafeNormal2D();
			FRotator Rot = ToTarget.Rotation(); 

			// �Էµ� ����� �����ϰ� �ش� Ÿ���� �ٶ󺸵��� ȸ����Ŵ.
			Char->GetRootComponent()->SetWorldRotation(Rot);
			return;
		}

		// ���Ⱚ�� ��ȿ�� �� 
		if (!Dir.IsNearlyZero())
		{
			// �Էµ� ���Ⱚ���� ��� ȸ���� ��, ���� �ִϸ��̼� ����
			Char->GetRootComponent()->SetWorldRotation(Dir.Rotation());
		}
	}
}

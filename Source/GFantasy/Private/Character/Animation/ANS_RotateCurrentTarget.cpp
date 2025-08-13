// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/ANS_RotateCurrentTarget.h"

#include "Character/GASPlayerCharacter.h"


void UANS_RotateCurrentTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(MeshComp->GetOwner()))
	{
		// ĳ������ ���� ĳ�̵� �Է� ������ ������
		FVector Dir = Char->CachedInputDirection;

		// ���� ��Ŀ�̵� ���� ���� ��,
		if (Char->CurrentTarget)
		{
			// ���� �ٶ󺸵��� ĳ���͸� ȸ����Ų��.
			FVector ToTarget = (Char->CurrentTarget->GetActorLocation() - Char->GetActorLocation()).GetSafeNormal2D();
			FRotator Rot = ToTarget.Rotation(); 
			Char->GetRootComponent()->SetWorldRotation(Rot);
			return;
		}
		// ���� ���� ���
		else
		{
			// ���� ĳ�̵� ������ ��ȿ�ϴٸ�, �ش� �������� ĳ���͸� ȸ��.
			if (!Dir.IsNearlyZero())
			{
				// �� ���� ��� ȸ��
				Char->GetRootComponent()->SetWorldRotation(Dir.Rotation());
				//SetActorRotation(Dir.Rotation());
			}
		}

	}
}

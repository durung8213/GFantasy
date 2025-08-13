// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Animation/ANS_RangedCombo.h"
#include "Character/AnimInstance/GASPlayerAnimInstance.h"
#include "GameFramework/Character.h"

void UANS_RangedCombo::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	UE_LOG(LogTemp, Warning, TEXT("In Ranged Notify"));
	ACharacter* Char = Cast<ACharacter>(MeshComp->GetOwner());

	if (Char->HasAuthority()) // 서버에서 실행되는 외부 코드 (어빌리티 등)
	{
		if (auto* Anim = Cast<UGASPlayerAnimInstance>(Char->GetMesh()->GetAnimInstance()))
		{
			Anim->bHasUpperBodyAction = true;
		}
	}
}	

void UANS_RangedCombo::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	UE_LOG(LogTemp, Warning, TEXT("Out Ranged Notify"));
	
	ACharacter* Char = Cast<ACharacter>(MeshComp->GetOwner());

	if (Char->HasAuthority()) // 서버에서 실행되는 외부 코드 (어빌리티 등)
	{
		if (auto* Anim = Cast<UGASPlayerAnimInstance>(Char->GetMesh()->GetAnimInstance()))
		{
			Anim->bHasUpperBodyAction = false;
		}
	}

}

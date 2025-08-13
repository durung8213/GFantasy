// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_MontageCancle.h"

#include "AIController.h"
#include "GameFramework/Character.h"

UBTT_MontageCancle::UBTT_MontageCancle()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_MontageCancle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetAIOwner() && OwnerComp.GetAIOwner()->GetPawn())
	{
		ACharacter* Character = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
		if (Character)
		{
			if (Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
			{
				if (Character->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
				{
					Character->GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
					
				}
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}

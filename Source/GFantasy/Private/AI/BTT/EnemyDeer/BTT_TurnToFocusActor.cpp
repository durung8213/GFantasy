// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/EnemyDeer/BTT_TurnToFocusActor.h"

#include "AIController.h"
#include "Character/Enemy/NormalEnemy/NormalEnemyBase.h"
#include "Controller/AIControllerBase.h"

UBTT_TurnToFocusActor::UBTT_TurnToFocusActor()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_TurnToFocusActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* CurrentController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (CurrentController)
	{
		CurrentController->RotateToPlayerStart();
		BehaviorTreeComponent = &OwnerComp;
		if (!CurrentController->OnRotateToPlayerEnd.IsAlreadyBound(this, &UBTT_TurnToFocusActor::TaskEnd))
		{
			// 중복 바인딩 방지
			CurrentController->OnRotateToPlayerEnd.AddDynamic(this, &UBTT_TurnToFocusActor::TaskEnd);
		}
		
	}

	return EBTNodeResult::InProgress;
}

void UBTT_TurnToFocusActor::TaskEnd()
{
	if (BehaviorTreeComponent)
	{
		FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
	}
}

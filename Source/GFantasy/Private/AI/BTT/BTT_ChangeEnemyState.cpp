// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_ChangeEnemyState.h"

#include "Controller/AIControllerBase.h"
#include "Enum/EnemyEnumLibrary.h"


UBTT_ChangeEnemyState::UBTT_ChangeEnemyState()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ChangeEnemyState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* CurrentAIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());

	if (CurrentAIController)
	{
		CurrentAIController->SetEnemyState(EEnemyState::Passive);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	
	
}

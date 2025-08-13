// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_ChangeUseSense.h"

#include "Controller/AIControllerBase.h"

UBTT_ChangeUseSense::UBTT_ChangeUseSense()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ChangeUseSense::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* CurrentController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (CurrentController)
	{
		CurrentController->ChangeUseSence(bUseSight, bUseHearing, bUseDamage);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}

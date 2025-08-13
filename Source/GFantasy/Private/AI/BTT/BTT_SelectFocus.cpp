// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_SelectFocus.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/AI/Boss/BossAIControllerBase.h"

UBTT_SelectFocus::UBTT_SelectFocus()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_SelectFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABossAIControllerBase* CurrentAIController = Cast<ABossAIControllerBase>(OwnerComp.GetAIOwner());

	if (CurrentAIController)
	{
		CurrentAIController->FocusTarget = CurrentAIController->FindFocusPlayerByCondition();
		CurrentAIController->GetBlackboardComponent()->SetValueAsObject("FocusActor", CurrentAIController->FocusTarget);
		CurrentAIController->SetFocus(CurrentAIController->FocusTarget);
	}
	
	return EBTNodeResult::Succeeded;
}

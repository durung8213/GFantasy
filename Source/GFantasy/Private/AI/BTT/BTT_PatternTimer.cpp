// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_PatternTimer.h"

#include "Controller/AIControllerBase.h"

UBTT_PatternTimer::UBTT_PatternTimer()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_PatternTimer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* CurrnetController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());

	TaskBehaviorTreeComponent = &OwnerComp;
	
	if (CurrnetController)
	{
		CurrnetController->CurrentPatternTimerTask = this;

		if (!CurrnetController->OnPatternTimerEnd.IsAlreadyBound(this, &UBTT_PatternTimer::EndTask))
		{
			// 중복 바인딩 방지
			CurrnetController->OnPatternTimerEnd.AddDynamic(this, &UBTT_PatternTimer::EndTask);
		}
	}
	
	OwnerComp.GetWorld()->GetTimerManager().ClearTimer(EndTimerHandle);
	OwnerComp.GetWorld()->GetTimerManager().SetTimer(
		EndTimerHandle,
		this,
		&UBTT_PatternTimer::EndTask,
		Time,
		false
	);
	
	return EBTNodeResult::InProgress;
}

void UBTT_PatternTimer::EndTask()
{
	GetWorld()->GetTimerManager().ClearTimer(EndTimerHandle);
	AAIControllerBase* CurrnetController = Cast<AAIControllerBase>(TaskBehaviorTreeComponent->GetAIOwner());
	if (CurrnetController)
	{
		CurrnetController->CurrentPatternTimerTask = nullptr;
	}
	FinishLatentTask(*TaskBehaviorTreeComponent, EBTNodeResult::Succeeded);
}


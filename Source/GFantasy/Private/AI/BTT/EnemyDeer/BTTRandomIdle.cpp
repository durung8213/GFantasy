// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/EnemyDeer/BTTRandomIdle.h"

#include "AIController.h"
#include "Character/Enemy/NormalEnemy/NormalEnemyBase.h"


UBTTRandomIdle::UBTTRandomIdle()
{
	NodeName = TEXT("RandomIdle");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTRandomIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BehaviorTreeComponent = &OwnerComp;
	
	if (ANormalEnemyBase* CurrentPawnCast = Cast<ANormalEnemyBase>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		CurrentPawnCast->RandomIdleStart();
		//CurrentPawnCast->OnRandomIdleEnd.RemoveDynamic(this, &UBTTRandomIdle::ReceiveRandomIdleEnd); // 해당 함수만 제거
		CurrentPawnCast->OnRandomIdleEnd.AddDynamic(this, &UBTTRandomIdle::ReceiveRandomIdleEnd);
		
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTRandomIdle::ReceiveRandomIdleEnd()
{
	if (BehaviorTreeComponent)
	{
		FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
	}
	
}


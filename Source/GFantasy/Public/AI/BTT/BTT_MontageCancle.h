// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MontageCancle.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_MontageCancle : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_MontageCancle();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

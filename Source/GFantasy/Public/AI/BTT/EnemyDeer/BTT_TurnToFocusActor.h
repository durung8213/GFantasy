// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_TurnToFocusActor.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_TurnToFocusActor : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_TurnToFocusActor();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void TaskEnd();

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent = nullptr;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTRandomIdle.generated.h"

/**
 * 
 */





UCLASS()
class GFANTASY_API UBTTRandomIdle : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTRandomIdle();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void ReceiveRandomIdleEnd();

protected:

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent;
};

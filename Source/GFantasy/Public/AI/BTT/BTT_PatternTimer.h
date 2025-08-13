// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PatternTimer.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_PatternTimer : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTT_PatternTimer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


public:
	UFUNCTION()
	void EndTask();
	
private:
	UPROPERTY()
	FTimerHandle EndTimerHandle;

	UPROPERTY()
	UBehaviorTreeComponent* TaskBehaviorTreeComponent;
	
public:
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTTask")
	float Time = 10.f;
};

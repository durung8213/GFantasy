// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChangeEnemyState.generated.h"

/**
 * 
 */
enum class EEnemyState : uint8;

UCLASS()
class GFANTASY_API UBTT_ChangeEnemyState : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_ChangeEnemyState();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	EEnemyState ChangeState;
};

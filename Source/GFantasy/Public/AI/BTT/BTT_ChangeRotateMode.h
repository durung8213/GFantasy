// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enum/EnemyEnumLibrary.h"
#include "BTT_ChangeRotateMode.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_ChangeRotateMode : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_ChangeRotateMode();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	EEnemyRotateMode FocusMode = EEnemyRotateMode::RoundMode;
};

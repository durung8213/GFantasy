// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChangeUseSense.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_ChangeUseSense : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_ChangeUseSense();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	bool bUseSight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	bool bUseHearing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	bool bUseDamage;
};

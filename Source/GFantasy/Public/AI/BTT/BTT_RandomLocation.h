// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RandomLocation.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(BTT_RandomLocation, Log, All);

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_RandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_RandomLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTTask")
	float Range;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTTask")
	float DistanceMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTTask")
	float DistanceMax = 0.0f;
};

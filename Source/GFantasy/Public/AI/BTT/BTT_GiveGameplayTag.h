// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_GiveGameplayTag.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(BTT_GiveGameplayTag, Log, All);

/**
 * 
 */

class UGameplayEffect;
UCLASS()
class GFANTASY_API UBTT_GiveGameplayTag : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_GiveGameplayTag();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UGameplayEffect> Effect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EffectLevel = 1.f;
};

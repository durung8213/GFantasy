// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTAbilityToOwner.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(BTT_AbilityToOwner, Log, All);

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTTAbilityToOwner : public UBTTaskNode
{
	GENERATED_BODY()

    UBTTAbilityToOwner();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void EndAbilityDelegateBindFunc();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityTask")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityTask")
	float ReleaseDeadlockTime = 10.f;
protected:
	UPROPERTY()
	UBehaviorTreeComponent* TaskBehaviorTreeComponent;

	UPROPERTY()
	FTimerHandle ReleaseDeadlock;

	
};

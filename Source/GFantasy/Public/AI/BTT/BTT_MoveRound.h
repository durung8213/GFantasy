// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveRound.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(BTT_MoveRound, Log, All);

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_MoveRound : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_MoveRound();
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	FVector GetVector();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	FName KeyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	float TurnRate = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	float ErrorLimits = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BTT")
	float TimeLimit = 20.f;

protected:
	UPROPERTY()
	UBlackboardComponent* BlackboardComp = nullptr;

	UPROPERTY()
	APawn* CurrentAIPawn = nullptr;

	UPROPERTY()
	int32 KeyID;

	UPROPERTY()
	FVector TargetVector;

	UPROPERTY()
	float ElapsedTime = 0.0f;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DetectFocusActorByTick.generated.h"

/**
 * 
 */
class AAIControllerBase;

UCLASS()
class GFANTASY_API UBTT_DetectFocusActorByTick : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	UBTT_DetectFocusActorByTick();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY()
	AAIControllerBase* CurrentController = nullptr;

	UPROPERTY()
	APawn* CurrentPawn = nullptr;

protected:
	virtual void DetectedFocusActor(AAIControllerBase* Owner, AActor* DetectedActor);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BoxEndSize = 50.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector BoxHalfSize = FVector(10,10,10);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Orientation = FRotator(0,0,0);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
};

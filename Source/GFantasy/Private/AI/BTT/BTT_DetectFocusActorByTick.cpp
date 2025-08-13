// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_DetectFocusActorByTick.h"

#include "Controller/AIControllerBase.h"
#include "Kismet/KismetSystemLibrary.h"

UBTT_DetectFocusActorByTick::UBTT_DetectFocusActorByTick()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_DetectFocusActorByTick::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CurrentController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	CurrentPawn = CurrentController->GetPawn();
	return EBTNodeResult::InProgress;
}

void UBTT_DetectFocusActorByTick::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (CurrentController && CurrentPawn)
	{
		/* 멀티 트레이스 */
		TArray<FHitResult> HitResults;
		FVector TraceStart = CurrentPawn->GetActorLocation();
		FVector TraceEnd = TraceStart + (CurrentPawn->GetActorForwardVector() * BoxEndSize);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CurrentPawn);
			
		TArray<AActor*> ActorsToIgnore;
			
		bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(
			CurrentPawn->GetWorld(),
			TraceStart,
			TraceEnd,
			BoxHalfSize,
			Orientation,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResults,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			DeltaSeconds
		);
			
		/* 멀티 트레이스 끝 */

		if (bHit)
		{
			for (FHitResult HitResult : HitResults)
			{
				if (HitResult.GetActor() == CurrentController->GetFocusActor())
				{
					DetectedFocusActor(CurrentController, HitResult.GetActor());
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					break;
				}
			}
		}
	}
	
}

void UBTT_DetectFocusActorByTick::DetectedFocusActor(AAIControllerBase* Owner, AActor* DetectedActor)
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Detected Actor : %s"), *DetectedActor->GetName()));
}


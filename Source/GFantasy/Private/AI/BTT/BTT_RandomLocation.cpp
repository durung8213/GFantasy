// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_RandomLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/AIControllerBase.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(BTT_RandomLocation);

UBTT_RandomLocation::UBTT_RandomLocation()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_RandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* CurrentController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());

	if (CurrentController)
	{
		FNavLocation RandomLocation;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		if (NavSys && NavSys->GetRandomReachablePointInRadius(CurrentController->SpawnerLocation, Range, RandomLocation))
		{
			FVector Dir = (RandomLocation.Location - CurrentController->GetPawn()->GetActorLocation()).GetSafeNormal();
			Dir *= FMath::FRandRange(DistanceMin, DistanceMax);

			Dir += CurrentController->GetPawn()->GetActorLocation();
			
			FNavLocation Projected;

			if (NavSys->ProjectPointToNavigation(Dir, Projected, FVector(100,100,500)))
			{
				// UKismetSystemLibrary::PrintString(
				// 	this,
				// 	FString::Printf(
				// 		TEXT("%s가 %s에서 -> %s로 이동 (%lf만큼 이동)"),
				// 		*CurrentController->GetPawn()->GetName(),
				// 		*CurrentController->GetPawn()->GetActorLocation().ToString(),
				// 		*Projected.Location.ToString(),
				// 		FVector::Distance(CurrentController->GetPawn()->GetActorLocation(), Projected.Location)
				// 	)
				// );
				CurrentController->GetBlackboardComponent()->SetValueAsVector(FName("PassiveLocation"), Projected.Location);
				CurrentController->SetCurrentTargetLocation(Projected.Location);
				return EBTNodeResult::Succeeded;
			}
			else
			{
				UE_LOG(BTT_RandomLocation, Error, TEXT("ProjectPointToNavigation fail"));
				//UKismetSystemLibrary::PrintString(this, TEXT("네비메시가 없거나 반경에 이동할 수 있는 위치가 없음2"));
				return EBTNodeResult::Failed;
			}
			
			
		}
		else
		{
			UE_LOG(BTT_RandomLocation, Error, TEXT("NavSys is not valid or NavSys->GetRandomReachablePointInRadius fail"));
			//UKismetSystemLibrary::PrintString(this, TEXT("네비메시가 없거나 반경에 이동할 수 있는 위치가 없음"));
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("컨트롤러가 AAIControllerBase가 아님"));
		return EBTNodeResult::Failed;
	}
}

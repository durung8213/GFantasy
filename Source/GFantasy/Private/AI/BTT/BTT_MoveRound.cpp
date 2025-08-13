// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_MoveRound.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Net/RPCDoSDetection.h"

DEFINE_LOG_CATEGORY(BTT_MoveRound);

UBTT_MoveRound::UBTT_MoveRound()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_MoveRound::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	CurrentAIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (BlackboardComp)
	{
		KeyID = BlackboardComp->GetKeyID(KeyName);

		const UBlackboardData* BBAsset = BlackboardComp->GetBlackboardAsset();
		if (BBAsset)
		{
			TargetVector = GetVector();
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("키아이디 구하기 실패"));
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("블랙보드 컴포넌트 없음"));
		return EBTNodeResult::Failed;
	}
	
	
	return EBTNodeResult::InProgress;
}

void UBTT_MoveRound::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;
	if (CurrentAIPawn && BlackboardComp)
	{
		TargetVector = GetVector();
		CurrentAIPawn->SetActorRotation(
			UKismetMathLibrary::RInterpTo(
			CurrentAIPawn->GetActorRotation(),
			FRotator(
				0,
				UKismetMathLibrary::FindLookAtRotation(CurrentAIPawn->GetActorLocation(), TargetVector).Yaw,
				0
			),
			DeltaSeconds,
			TurnRate
			)
		);
		
		CurrentAIPawn->AddMovementInput(CurrentAIPawn->GetActorForwardVector());

		if (CurrentAIPawn->GetActorLocation().Equals(TargetVector, ErrorLimits))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		UE_LOG(BTT_MoveRound, Error, TEXT("CurrentAIPawn or BlackboardComp is not valid"));
		//UKismetSystemLibrary::PrintString(this, TEXT("CurrentAIPawn && BlackboardComp 둘중 하나 없음"));
	}

	if (ElapsedTime > TimeLimit)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	
}

FVector UBTT_MoveRound::GetVector()
{
	TSubclassOf<UBlackboardKeyType> KeyType = BlackboardComp->GetKeyType(KeyID);

	if (KeyType->IsChildOf(UBlackboardKeyType_Vector::StaticClass()))
	{
		return BlackboardComp->GetValueAsVector(KeyName);
	}
	else if (KeyType->IsChildOf(UBlackboardKeyType_Object::StaticClass()))
	{
		return TargetVector = Cast<AActor>(BlackboardComp->GetValueAsObject(KeyName))->GetActorLocation();
	}
	else
	{
		UE_LOG(BTT_MoveRound, Error, TEXT("Key Value Error"));
		//UKismetSystemLibrary::PrintWarning(TEXT("키 값 잘 못 넣음"));
		return FVector::ZeroVector;
	}
	
}

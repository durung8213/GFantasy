// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/Enemy/DeerStagAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "Enum/EnemyEnumLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


ADeerStagAIController::ADeerStagAIController() : AAIControllerBase()
{
	SightConfig->SightRadius = 700.0f;
	SightConfig->LoseSightRadius = 1000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->SetMaxAge(7.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	if (SightConfig)
	{
		// Perception 컴포넌트에 시야 설정 추가
		AIPerception->ConfigureSense(*SightConfig);

		// 주 감지 센스를 시야로 설정
		//AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create SightConfig"));
	}
}

void ADeerStagAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeerStagAIController::OnEntityDetectedBySense_Implementation(AActor* SenseActor)
{
	Super::OnEntityDetectedBySense_Implementation(SenseActor);

	AGASPlayerCharacter* CurrentDetectedPlayer = Cast<AGASPlayerCharacter>(SenseActor);
	if (CurrentDetectedPlayer)
	{
		if (GetEnemyState() != EEnemyState::Attack)
		{
			//SetFocus(CurrentDetectedPlayer);
			SetEnemyState(EEnemyState::InvestigationActor);
			if (Blackboard)
			{
				Blackboard->SetValueAsObject(FName("InvestigationActor") , CurrentDetectedPlayer);
			}
		}
		
	}
	
	
}

void ADeerStagAIController::OnEntityDetectedByHearing_Implementation(AActor* Actor)
{
	Super::OnEntityDetectedByHearing_Implementation(Actor);

	AGASPlayerCharacter* CurrentDetectedPlayer = Cast<AGASPlayerCharacter>(Actor);
	if (CurrentDetectedPlayer)
	{
		FVector DetectedPlayerLocation = CurrentDetectedPlayer->GetActorLocation();
		SetEnemyState(EEnemyState::InvestigationLocation);
		if (Blackboard)
		{
			Blackboard->SetValueAsVector(FName("InvestigationLocation"), DetectedPlayerLocation);
		}
		
	}
	
}

void ADeerStagAIController::OnEntityDetectedByDamage_Implementation(AActor* Actor)
{
	Super::OnEntityDetectedByDamage_Implementation(Actor);
	if (SightConfig)
	{
		// UKismetSystemLibrary::PrintString(
		// this,
		// 	FString::Printf(
		// 		TEXT("공격 입힌 액터 : %s, AI와의 거리 : %lf, 시야 반경 : %lf"),
		// 		*Actor->GetName(),
		// 		FVector::Distance(GetPawn()->GetActorLocation(),
		// 		Actor->GetActorLocation()),
		// 		SightConfig->SightRadius
		// 	)
		// );
		if (FVector::Distance(GetPawn()->GetActorLocation(), Actor->GetActorLocation()) <= SightConfig->SightRadius)
		{
			SetEnemyState(EEnemyState::Attack);
			SetFocus(Actor);
			ClearFocus(EAIFocusPriority::Move);
			RotateToPlayerEnd();
		}
		else
		{
		
		}
	}
	else
	{
		// UKismetSystemLibrary::PrintString(
		// this,
		// 	FString::Printf(
		// 		TEXT("공격 입힌 액터 : %s, AI와의 거리 : %lf, 시야 반경 : %lf"),
		// 		*Actor->GetName(),
		// 		FVector::Distance(GetPawn()->GetActorLocation(),
		// 		Actor->GetActorLocation()),
		// 		700.f
		// 	)
		// );

		if (FVector::Distance(GetPawn()->GetActorLocation(), Actor->GetActorLocation()) <= 700)
		{
			SetEnemyState(EEnemyState::Attack);
			SetFocus(Actor);
			ClearFocus(EAIFocusPriority::Move);
			RotateToPlayerEnd();
		}
	}
	
}

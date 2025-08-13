// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/Enemy/WolfAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "Enum/EnemyEnumLibrary.h"
#include "Interface/EnemyInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AWolfAIController::AWolfAIController()
{
	SightConfig->SightRadius = 900.0f;
	SightConfig->LoseSightRadius = 1300.0f;
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

void AWolfAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWolfAIController::OnEntityDetectedBySense_Implementation(AActor* SenseActor)
{
	Super::OnEntityDetectedBySense_Implementation(SenseActor);
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(SenseActor);
	if (Player)
	{
		SetFocus(Player);
		IEnemyInterface::Execute_SetControllerFocusActor(GetPawn(), SenseActor);
		SetEnemyState(EEnemyState::Attack);
	}
	
}

void AWolfAIController::OnEntityDetectedByHearing_Implementation(AActor* Actor)
{
	Super::OnEntityDetectedByHearing_Implementation(Actor);
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(Actor);
	if (Player && GetEnemyState() != EEnemyState::Attack)
	{
		Blackboard->SetValueAsVector(FName("InvestigationLocation"), Player->GetActorLocation());
		SetEnemyState(EEnemyState::InvestigationLocation);
	}
}

void AWolfAIController::OnEntityDetectedByDamage_Implementation(AActor* Actor)
{
	Super::OnEntityDetectedByDamage_Implementation(Actor);
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(Actor);
	if (Player && GetEnemyState() != EEnemyState::Attack )
	{
		Blackboard->SetValueAsVector(FName("InvestigationLocation"), Player->GetActorLocation());
		SetEnemyState(EEnemyState::InvestigationLocation);
	}
	else if (GetEnemyState() == EEnemyState::InvestigationLocation)
	{
		SetEnemyState(EEnemyState::Passive);
		SetEnemyState(EEnemyState::InvestigationLocation);
	}
}

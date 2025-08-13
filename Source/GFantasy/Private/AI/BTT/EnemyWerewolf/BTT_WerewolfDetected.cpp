// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/EnemyWerewolf/BTT_WerewolfDetected.h"

#include "AI/BTT/BTT_PatternTimer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/AIControllerBase.h"


void UBTT_WerewolfDetected::DetectedFocusActor(AAIControllerBase* Owner, AActor* DetectedActor)
{
	if (Owner)
	{
		Owner->GetBlackboardComponent()->SetValueAsBool(FName("bChachKillPatternSuccess"), true);
	}
	
	if (Owner->CurrentPatternTimerTask
		&& Owner->CurrentPatternTimerTask->IsValidLowLevel()
		&& IsValid(Owner->CurrentPatternTimerTask)
		&& Owner->HasAuthority())
	{
		Owner->CurrentPatternTimerTask->EndTask();
		Owner->GetBlackboardComponent()->SetValueAsBool(FName("bChachKillPatternSuccess"), true);
	}
	
}

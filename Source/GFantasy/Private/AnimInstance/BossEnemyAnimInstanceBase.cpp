// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/BossEnemyAnimInstanceBase.h"

#include "Character/GASCombatBossNPC.h"

void UBossEnemyAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (CurrentPawn)
	{
		CurrentBossEnemyPawn = Cast<AGASCombatBossNPC>(CurrentPawn);	
	}
}

void UBossEnemyAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (CurrentBossEnemyPawn)
	{
		bGroggy = CurrentBossEnemyPawn->bIsGroggy;
	}
}

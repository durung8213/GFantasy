// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/Boss/Boss1.h"

#include "Kismet/GameplayStatics.h"
#include "BrainComponent.h"

ABoss1::ABoss1()
{
	BossLife = 1;
}


void ABoss1::BossOnDie_Implementation()
{
	// if (BossLife == 1)
	// {
	// 	BossLife--;
	// 	GetBrainComponent()->StopLogic(TEXT("Stopped by Code"));
	// }
	// else if (BossLife == 0)
	// {
	// 	
	// }
}

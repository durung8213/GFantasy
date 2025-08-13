// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCombatBossNPC.h"

#include "Actor/BossArea/BossAreaBase.h"
#include "Actor/BossArea/BossPositionPoint.h"
#include "Controller/AI/Boss/BossAIControllerBase.h"
#include "GAS/AttributeSet/GASBossGroggy.h"
#include "Net/UnrealNetwork.h"
#include "UI/Enemy/EnemyState/EnemyStateWidget.h"

AGASCombatBossNPC::AGASCombatBossNPC()
{
	GroggyAttributeSet = CreateDefaultSubobject<UGASBossGroggy>(TEXT("GroggyAttributeSet"));
}

void AGASCombatBossNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AGASCombatBossNPC, bIsGroggy, COND_None, REPNOTIFY_Always);
}

void AGASCombatBossNPC::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyStateWidget)
	{
		EnemyStateWidget->CollapsedMode();
	}
}

FVector AGASCombatBossNPC::GetBossPositionPointLocationByIndex_Implementation(int Index)
{
	return BossArea->GetBossPositionPointLocationByIndex(Index);
}

FVector AGASCombatBossNPC::GetBossAreaMiddlePositionPointLocation_Implementation()
{
	return BossArea->MiddlePositionPoint->GetActorLocation();
}


TArray<AGASPlayerCharacter*> AGASCombatBossNPC::GetInAreaPlayers_Implementation()
{
	TArray<AGASPlayerCharacter*> ReturnArr;
	
	ABossAIControllerBase* CurrentBossController =  Cast<ABossAIControllerBase>(GetController());
	if (CurrentBossController)
	{
		for (auto PlayerInfo : CurrentBossController->InAreaPlayerArr)
		{
			ReturnArr.Add(PlayerInfo.Player);
		}
		
	}
	return ReturnArr;
}

UGASBossGroggy* AGASCombatBossNPC::GetGroggyAttributeSet() const
{
	return GroggyAttributeSet;
}

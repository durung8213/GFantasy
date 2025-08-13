// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BossArea/BossAreaBase.h"

#include "Actor/BossArea/BossPositionPoint.h"
#include "Character/GASCombatBossNPC.h"
#include "Controller/KimmhTestWorldPlayerController.h"
#include "Controller/AI/Boss/BossAIControllerBase.h"
#include "HUD/KimmhTestWorldHUD.h"

// Sets default values
ABossAreaBase::ABossAreaBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void ABossAreaBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABossAreaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossAreaBase::OnPlayerBeginOverlap(AGASPlayerCharacter* Player)
{
	if (!BossCharacter)
	{
		return;
	}
	
	ABossAIControllerBase* CurrentBossAIController = Cast<ABossAIControllerBase>(BossCharacter->GetController());
	if (CurrentBossAIController && Player)
	{
		CurrentBossAIController->PlayerInArea(Player);

		if (AGASWorldPlayerController* CurrentController = Cast<AGASWorldPlayerController>( Player->GetController()))
		{
			CurrentController->ClientShowBossUI(BossCharacter);
			if (HasAuthority())
			{
				CurrentController->BindBossState(BossCharacter);


			}
			
		}
	}
}

void ABossAreaBase::OnPlayerEndOverlap(AGASPlayerCharacter* Player)
{

	if (!BossCharacter)
	{
		return;
	}
	
	ABossAIControllerBase* CurrentBossAIController = Cast<ABossAIControllerBase>(BossCharacter->GetController());
	if (CurrentBossAIController)
	{
		CurrentBossAIController->PlayerOutArea(Player);
		if (AGASWorldPlayerController* CurrentController = Cast<AGASWorldPlayerController>( Player->GetController()))
		{
			CurrentController->ClientDestoryBossUI();
		}
	}
}

FVector ABossAreaBase::GetBossPositionPointLocationByIndex(int Index)
{
	if (PositionPointArr.IsValidIndex(Index))
	{
		return PositionPointArr[Index]->GetActorLocation();
	}
	else
	{
		return FVector();
	}
}


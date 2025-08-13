// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Spawner/BossEnemySpawner.h"

#include "Actor/BossArea/BossAreaBase.h"
#include "Actor/BossArea/BossPositionPoint.h"
#include "Character/GASCombatBossNPC.h"
#include "Character/GASCombatNPC.h"
#include "Character/Enemy/Boss/BossWereWolfCharacter.h"
#include "Kismet/GameplayStatics.h"

void ABossEnemySpawner::Spawn()
{
	if (!SpawnEnemyClass->IsChildOf(AGASCombatBossNPC::StaticClass()) || !HasAuthority())
	{
		return;
	}

	if (!BossArea || !BossArea->MiddlePositionPoint)
	{
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	//
	FVector SpawnLocation = BossArea->MiddlePositionPoint->GetActorLocation();
	const FTransform Transform(FRotator(0, 0, 0), SpawnLocation);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;	
	
	AGASCombatBossNPC* SpawnBossEnemy = GetWorld()->SpawnActorDeferred<AGASCombatBossNPC>(
			SpawnEnemyClass,
			Transform,
			SpawnParams.Owner,
			SpawnParams.Instigator,
			SpawnParams.SpawnCollisionHandlingOverride
		);
	
	if (SpawnBossEnemy)
	{
		SpawnBossEnemy->bIsSpawnBySpawner = true;
		SpawnBossEnemy->BossArea = BossArea;
		BossArea->BossCharacter = SpawnBossEnemy;
		SpawnBossEnemy->OnCombatNPCDie.AddDynamic(this, &AEnemySpawner::OnSpawnedActorDied);
		SpawnBossEnemy->FinishSpawning(SpawnBossEnemy->GetTransform());
		SpawnBossEnemy->InitWeapon();
	}
	
}

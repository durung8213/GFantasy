// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Spawner/EnemySpawner.h"

#include "NavigationSystem.h"
#include "Character/GASCombatNPC.h"
#include "Character/Enemy/NormalEnemy/NormalEnemyBase.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i < SpawnCount; i ++)
	{
		Spawn();
	}

	//DebugArea();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::Spawn()
{
	if (!SpawnEnemyClass || !HasAuthority()) return;	// EnemyClass가 없거나 서버 로직이 아니면 스폰을 안함

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	FNavLocation RandomLocation;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSys && NavSys->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRange, RandomLocation))
	{
		// 스포너 기준 SpawnRange 반경 안에 랜덤으로 소환 가능한 위치가 있으면

		FVector SpawnLocation = FVector(RandomLocation.Location.X, RandomLocation.Location.Y, RandomLocation.Location.Z+90);
		
		const FTransform Transform(FRotator(0,FMath::RandRange(0.f, 360.f), 0), SpawnLocation);
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = Cast<APawn>(Params.Owner);
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AGASCombatNPC* SpawnEnemy = GetWorld()->SpawnActorDeferred<AGASCombatNPC>(
			SpawnEnemyClass,
			Transform,
			Params.Owner,
			Params.Instigator,
			Params.SpawnCollisionHandlingOverride
		);

		if (SpawnEnemy)
		{
			ANormalEnemyBase* NormalEnemy = Cast<ANormalEnemyBase>(SpawnEnemy);
			if (NormalEnemy)
			{
				NormalEnemy->SpawnerLocation = GetActorLocation();
				NormalEnemy->SpawnerArea = SpawnRange;
			}
			SpawnEnemy->OnCombatNPCDie.AddDynamic(this, &AEnemySpawner::OnSpawnedActorDied);
			SpawnEnemy->FinishSpawning(SpawnEnemy->GetTransform());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find NavMesh point."));
	}
	
	
}

void AEnemySpawner::OnSpawnedActorDied()
{
	UE_LOG(LogTemp, Log, TEXT("SpawnTimerStart"));
	//UKismetSystemLibrary::PrintString(this, TEXT("스포너로 스폰된 객체가 죽어 스폰 타이머 시작"));
	FTimerHandle SpawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AEnemySpawner::Spawn,
			FMath::RandRange(SpawnDelayMin, SpawnDelayMax),
			false
		);
	 
}


void AEnemySpawner::DebugArea()
{
#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),           // 위치
		SpawnRange,                     // 반지름
		12,                         // 세그먼트 수
		FColor::Green,             // 색상
		false,                     // 영구 여부
		100000.f                       // 지속 시간
	);

#endif
}


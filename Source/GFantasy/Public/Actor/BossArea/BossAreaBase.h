// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossAreaBase.generated.h"

class ABossPositionPoint;
class AGASCombatBossNPC;
class ABossAIControllerBase;
class AGASPlayerCharacter;

UCLASS()
class GFANTASY_API ABossAreaBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossAreaBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	/**
	 * 플레이어가 보스 영역에 들어올 경우 오버랩 처리
	 * - 매개변수 Player : 영역에 들어온 플레이어
	 */
	virtual void OnPlayerBeginOverlap(AGASPlayerCharacter* Player);
	
	virtual void OnPlayerEndOverlap(AGASPlayerCharacter* Player);

public:
	/**
	 * 
	 */
	virtual FVector GetBossPositionPointLocationByIndex(int Index);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BossArea")
	TObjectPtr<AGASCombatBossNPC> BossCharacter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BossArea")
	TObjectPtr<ABossPositionPoint> MiddlePositionPoint;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BossArea")
	TArray<TObjectPtr<ABossPositionPoint>> PositionPointArr;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LOG_AIControllerBase, Log, All);

class ANormalEnemyBase;
class AGASCombatNPC;
enum class EEnemyState : uint8;
class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;
class UBTT_PatternTimer;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRotateToPlayerEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPatternTimerEnd);

UCLASS()
class GFANTASY_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
protected:
	AAIControllerBase();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay) override;

	virtual void ClearFocus(EAIFocusPriority::Type InPriority) override;

	virtual void StopBT();
	
	virtual void SetEnemyState(EEnemyState NewState);

	virtual EEnemyState GetEnemyState();
protected:
	
	UFUNCTION()
	virtual void OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors);

	/**
	 * <summary>
	 * 개체를 시야로 감지하였을 때 실행될 함수
	 * </summary>
	 * <param name="Actor">감지한 액터</param>
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnEntityDetectedBySense(AActor* Actor);

	/**
	 * <summary>
	 * 개체를 시야로 감지하였을 때 실행될 함수
	 * </summary>
	 * <param name="Actor">감지한 액터</param>
	 */
	virtual void OnEntityDetectedBySense_Implementation(AActor* Actor);

	/**
	 * <summary>
	 * 개체를 소리로 감지하였을 때 실행될 함수
	 * </summary>
	 * <param name="Actor">감지한 액터</param>
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnEntityDetectedByHearing(AActor* Actor);
	virtual void OnEntityDetectedByHearing_Implementation(AActor* Actor);

	/**
	 * <summary>
	 * 개체를 데미지를 입었을 때 실행될 함수
	 * </summary>
	 * <param name="Actor">감지한 액터</param>
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnEntityDetectedByDamage(AActor* Actor);
	virtual void OnEntityDetectedByDamage_Implementation(AActor* Actor);

public:
	UFUNCTION()
	void SetCurrentTargetLocation(FVector NewCurrentTargetLocation);

	UFUNCTION()
	FVector GetCurrentTargetLocation();

	void SetControlledPawn();

	UFUNCTION(BlueprintCallable)
	void RotateToPlayerStart();

	UFUNCTION()
	void RotateToPlayer(float WorldDeltaTime);

	UFUNCTION()
	void RotateToPlayerEnd();

	UFUNCTION(BlueprintCallable)
	void ChangeUseSence(bool NewUseSight = true, bool NewUseHearing = true, bool NewUseDamage = true);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "AIController")
	TObjectPtr<UBehaviorTree>  BehaviorTree;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "AIController")
	bool bUseAIPerception = true;
	
	// 퍼셉션 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAIPerceptionComponent* AIPerception;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY()
	UAISenseConfig_Damage* DamageConfig;
	
	
protected:
	UPROPERTY()
	EEnemyState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIController")
	FName StateKeyName = FName("AIState");
	
public:
	UPROPERTY()
	FVector SpawnerLocation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIController", Replicated)
	FVector CurrentTargetLocation = FVector::ZeroVector;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIController", Replicated)
	bool bRotateToFocusActor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIController", Replicated)
	bool bUseSight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIController", Replicated)
	bool bUseHearing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIController", Replicated)
	bool bUseDamage = true;
	

protected:
	UPROPERTY()
	AGASCombatNPC* CurrentCombatPawn;

	UPROPERTY()
	ANormalEnemyBase* CurrentNormalEnemyPawn;

	FTimerHandle RotateToPlayerTimerHandle;
public:
	FOnRotateToPlayerEnd OnRotateToPlayerEnd;

public:
	UPROPERTY()
	UBTT_PatternTimer* CurrentPatternTimerTask;

	FOnPatternTimerEnd OnPatternTimerEnd;
};

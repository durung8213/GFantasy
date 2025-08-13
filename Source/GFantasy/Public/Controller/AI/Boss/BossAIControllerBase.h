// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/AIControllerBase.h"
#include "Character/GASPlayerCharacter.h"
#include "BossAIControllerBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LOG_BossAIControllerBase, Log, All);

class AGASPlayerCharacter;
class AGASWorldPlayerState;
class ULevelSequence;
class UAnimInstance;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FInArrayPlayer
{
	GENERATED_BODY()

public:
	FInArrayPlayer() {Player = nullptr, AggroValue = 0.f; NormalAttackCount = 0; StrongAttackCount = 0; Damage = 0;};
	
	FInArrayPlayer(AGASPlayerCharacter* NewPlayer) : Player(NewPlayer)
	{
		AggroValue = 0.0f;
		NormalAttackCount = 0;
		StrongAttackCount = 0;
		Damage = 0;
	};

	bool operator==(const FInArrayPlayer& Other) const
	{
		return Player == Other.Player;
	}
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// 영역 안에 있는 플레이어
	AGASPlayerCharacter* Player;

	// 어그로 수치
	float AggroValue;

	// 플레이어의 일반 공격 적중 횟수
	int NormalAttackCount;

	// 플레이어의 강공격 적중 횟수
	int StrongAttackCount;

	// 플레이어가 입힌 데미지
	float Damage;
};

UCLASS()
class GFANTASY_API ABossAIControllerBase : public AAIControllerBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override ;
protected:
	ABossAIControllerBase();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

public:
	void BindAttributes();
	
	/**
	 *  포커스할 플레이어을 찾는 함수 포커스에 조건이 없다
	 */
	virtual AGASPlayerCharacter* FindFocusPlayer();

	/**
	 * 특수 조건에 따라 포커스 할 플레이어를 찾는 것
	 * - 보스 거리순으로 플레이어명 수 만큼 점수 
	 * - 보스 강공격 타격 순으로
	 * - 보스 약공격 타격 순으로
	 * - hp낮은 순으로
	 * - 가장 우선도가 높은 사람 2명 중 랜덤으로 함
	 */
	virtual AGASPlayerCharacter* FindFocusPlayerByCondition();

	/**
	 * <summary>
	 * 플레이어가 보스전을 할 때 공격할 플레이어 목록을 추가한다
	 * </summary>
	 * <param name="NewPlayer">보스전을 할 플레이어</param>
	 */
	virtual void PlayerInArea(AGASPlayerCharacter* NewPlayer);

	/**
	 * <summary>
	 * 플레이어가 보스전을 할 때 공격할 플레이어 목록을 삭제한다
	 * </summary>
	 * <param name="OutPlayer">보스전에서 나온 플레이어(틩기기, 탈출 등)</param>
	 */
	virtual void PlayerOutArea(AGASPlayerCharacter* OutPlayer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BossOnDie();
	virtual void BossOnDie_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlaySequence(AGASPlayerCharacter* Player, ULevelSequence* LevelSequence);
	
	virtual void MeshChangeByIndex(int Index);

	UFUNCTION(BlueprintCallable)
	void SetGroggy();
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void BossHitNormalAttack(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);
	
	virtual void BossHitNormalAttack_Implementation(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);
	
	UFUNCTION(BlueprintNativeEvent)
	void BossHitStrongAttack(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);
	
	virtual void BossHitStrongAttack_Implementation(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);	

	UFUNCTION(BlueprintNativeEvent)
	void OnGroggyMax();
	virtual void OnGroggyMax_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void OnGroggyZero();
	virtual void OnGroggyZero_Implementation();
	
public:
	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay) override;

	UFUNCTION(BlueprintCallable)
	virtual void TpPlayerByTime(FVector TPLocation, AGASPlayerCharacter* Player, float Time);
	
protected:
	void LerpToFocusTarget();

public:
	UFUNCTION(BlueprintCallable)
	virtual void SendToPlayerIfDie(AGASWorldPlayerState* CurrentPlayerState);
public:


	// 포커스 중인 타겟
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* FocusTarget = nullptr;

	// 영역 안에 있는 플레이어 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FInArrayPlayer> InAreaPlayerArr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ULevelSequence>> LevelSequenceArr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Phase2AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Phase3AnimInstance;

protected:
	FTimerHandle RotationToTargetTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	int BossLife = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FocusActor = FName("Player");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UGameplayEffect> GroggyResetEffect;
};

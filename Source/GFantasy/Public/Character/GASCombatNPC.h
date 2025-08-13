// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASBaseCharacter.h"
#include "Interface/EnemyInterface.h"
#include "GASCombatNPC.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LOG_GASCombatNPC, Log, All);

class AGASWorldPlayerState;
class UGASResourceAS;
class UGASCharacterAS;
class UEnemyStateWidget;
class UWidgetComponent;
class ABaseWeapon;
class UGASCBase;
class UGASCombatAS;
class UGameplayEffect;
class UGameplayAbility;
class UCombatNonPlayerGASC;
struct FOnAttributeChangeData;
class UCurveFloat;
class UTimelineComponent;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatNPCDie);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatNPCGetDamage, AActor*, HitActor);

UCLASS()
class GFANTASY_API AGASCombatNPC : public AGASBaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override ;
	
protected:
	AGASCombatNPC();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
public:
	virtual void PossessedBy(AController* NewController) override;

	/// <summary>
	/// 캐릭터 자신에게 특정 GameplayEffect를 적용하는 함수
	/// </summary>
	/// <param name="Effect">Effect 클래스 타입, 어떤 효과를 적용할 것인지 정함</param>
	/// <param name="InEffectContext">효과의 컨텍스트 정보, 이 효과가 발생한 맥락(Context). 누가 시전했는지 등</param>
	/// <returns>적용 여부 True면 적용, false면 적용 못함</returns>
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	/// <summary>
	/// 기본 능력치 부여가 가능한지 확인하는 함수
	/// </summary>
	/// <returns></returns>
	bool CheckInitializeAttributes();

	/// <summary>
	/// 기본 능력치 부여
	/// </summary>
	virtual void InitAbilityActorInfo();

	/// <summary>
	/// 기본 어빌리티 부여
	/// </summary>
	void GiveAbilites();

	/// <summary>
	/// 게임 시작시 등장해야 하는 효과 부어
	/// </summary>
	void ApplyStartupEffects();
	
	/// <summary>
	/// AbilitySystemComponent를 반환하는 함수
	/// </summary>
	/// <returns>AbilitySystemComponent</returns>
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/// <summary>
	/// CharacterAttributeSet을 반환하는 함수
	/// </summary>
	/// <returns>AttributeSet</returns>
	virtual UGASCharacterAS* GetCharacterAttributeSet() const;

	/// <summary>
	/// CombatAttributeSet을 반환하는 함수
	/// </summary>
	/// <returns>AttributeSet</returns>
	virtual UGASCombatAS* GetCombatAttributeSet() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "CombatNPC")
	void InitWeapon();
	
	UFUNCTION(BlueprintCallable)
	void SetWeapon(UChildActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable)
	void AddWeaponArr(UChildActorComponent* ActorComponent);
	
	UFUNCTION(BlueprintCallable)
	ABaseWeapon* GetWeapon();

	// 포커싱 중인지 설정
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnemyInterface")
	void SetbIsFocus(bool bNewIsFocus);
	virtual void SetbIsFocus_Implementation(bool bNewIsFocus) override;  

	// 포커싱 중인지 가져오기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnemyInterface")
	bool GetbIsFocus();
	virtual bool GetbIsFocus_Implementation() override;

	// 삑 죽엇구
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnemyInterface")
	void OnDie();
	virtual void OnDie_Implementation() override;

	virtual void SetEnemyMovementSpeed_Implementation(float NewMovementSpeed) override;

	void ParryReceived();

	void CalcInstigatorAngle(AActor* InstigatorActor, float& AngleDegrees, FVector& CrossProduct);
	
	UFUNCTION()
	virtual void OnNormalDamage(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);

	UFUNCTION()
	virtual void OnStrongDamage(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);

	UFUNCTION()
	virtual void OnNormalAttackDie(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);

	UFUNCTION()
	virtual void OnStrongAttackDie(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target);
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UFUNCTION(BlueprintCallable)
	void Ragdoll();
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetRagdollVector(FVector NewRagdollVector);

	UFUNCTION(BlueprintCallable)
	virtual void SendToPlayerIfDie(AGASWorldPlayerState* CurrentPlayerState);

	void ClearTimer();

	void ClearDeadTimerHandle();
	
	// 아무것도 안보이게 함
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void CollapsedMode();
	
	// 적 HpBar만 보임
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void UnFocusMode();

	/**
	 * <summary>
	 * 적 HpBar이 Time 만큼 보임
	 * </summary>
	 * <param name="Time">HpBar가 보일 시간</param>
	 */
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void UnFocusModeByTime(float Time);

	// 적 HpBar, 퀘스트 마크가 보임
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void UnFocusModeWithQuest();

	/**
	 * <summary>
	 * 적 HpBar, 퀘스트 마크가 Time 만큼 보임
	 * </summary>
	 * <param name="Time">Hp Bar가 보일 시간</param>
	 */
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void UnFocusModeWithQuestByTime(float Time);
	
	// 적 HpBar, 이름이 보임
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void FocusMode();

	/**
	 * <summary>
	 * 적 HpBar, 이름이 Time만큼 보임
	 * </summary>
	 * <param name="Time">보일 시간</param>
	 */
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void FocusModeByTime(float Time);

	
	// 적 HpBar, 이름, 퀘스트 마크가 보임
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void FocusModeWithQuest();

	/**
	 * <summary>
	 * 적 HpBar, 이름, 퀘스트 마크 Time 만큼 보임
	 * </summary>
	 * <param name="Time">보일 시간</param>
	 */
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void FocusModeWithQuestByTime(float Time);

	void ChangeMeshByIndex(int Index);
	
	
	
protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRagdoll();
	
	void BindHealth(const FOnAttributeChangeData& Data);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBindHealth(float NewHealth);
	
	void BindMaxHealth(const FOnAttributeChangeData& Data);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastMaxBindHealth(float NewMaxHealth);

	/**
	 * <summary>
	 * 서버에서 채력바 초기화(현재값이랑 맞추기)를 진행하라는 함수
	 * </summary>
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerInitHealths();

	/**
	 * <summary>
	 * 
	 * </summary>
	 * <param name="Health"></param>
	 * <param name="MaxHealth"></param>
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MultiInitHealths(float Health, float MaxHealth);

	UFUNCTION(NetMulticast, Reliable)
	void DissolveEnemyStateWidget();
	
	/**
	 * <summary>
	 * 디졸브 타임라인을 실행하는 함수
	 * </summary>
	 */
	UFUNCTION(NetMulticast, Reliable)
	void DeadDissolveTimelineStart();
	
	/**
	 * <summary>
	 * 디졸브 타임라인의 값이 업데이트 될 때 호출되는 함수
	 * </summary>
	 * <param name="Value">타임라인 값</param>
	 */
	UFUNCTION()
	void DeadDissolveTimelineProgress(float Value);
	
	/**
	 * <summary>
	 * 디졸브 타임라인이 완료될 때 호출될 함수
	 * </summary>
	 */
	UFUNCTION()
	void DeadDissolveTimelineFinished();

protected:
	UFUNCTION()
	void StateWidgetDissolveLoop();
	
public:
	/// <summary>
	/// 기본 스탯을 초기화할 때 적용할 GameplayEffect
	/// </summary>
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DefaultStateSet;

	/// <summary>
	/// 처음부터 플레이어가 가지고 있어야 할 능력들(Ability)
	/// </summary>
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	/// <summary>
	/// 게임 시작 시 자동으로 발동되어야 하는 효과들
	/// </summary>
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	UCombatNonPlayerGASC* AbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	UGASCharacterAS* CharacterAttributeSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	UGASCombatAS* CombatAttributeSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatNPC")
	ABaseWeapon* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ABaseWeapon>> WeaponArr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatNPC")
	TObjectPtr<UWidgetComponent> EnemyStateWidgetComponent;

	UPROPERTY()
	TObjectPtr<UEnemyStateWidget> EnemyStateWidget;

	//맞앗을는지
	UPROPERTY(BlueprintReadOnly, Category= "Hit")
	bool bHitReacting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name")
	FGameplayTag MonsterTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name")
	FText MonsterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatNPC")
	TArray<FGameplayTag> ItemDropTags;
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "CombatNPC")
	bool bIsRagdoll;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CombatNPC", Replicated)
	bool bIsFocus = false;
	
	UPROPERTY(Replicated)
	FVector RagdollVelocity;

	UPROPERTY(Replicated)
	bool bIsDead = false;

	UPROPERTY()
	FTimerHandle EnemyStateTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	TArray<USkeletalMesh*> BossSkeletalMeshArr;

	UPROPERTY()
	UTimelineComponent* DeadDissolveTimeline; 

	// 커브 자산
	UPROPERTY(EditAnywhere, Category = "CombatNPC")
	UCurveFloat* DeadDissolveFloatCurve;

	UPROPERTY()
	FTimerHandle InitHealthTimerHandle;
	
	UPROPERTY()
	FTimerHandle DeadTimerHandle;

	int HitCount = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	int SuperArmorActivateCount = 4;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> SuperArmor;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "CombatNPC")
	FOnCombatNPCDie OnCombatNPCDie;

protected:
	UPROPERTY()
	FTimerHandle WidgetDissolveTimerHandle;

	UPROPERTY()
	float WidgetDissolveElapsedTime = 0;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatNPC")
	bool bIsSpawnBySpawner = false;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GAS/AbilitySystemComponent/GASCBase.h"
#include "GameplayAbilities/Public/AttributeSet.h"
#include "PlayerCharacterGASC.generated.h"


struct FGFEquipmentEntry;

// Limit Break UI 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FLBTagChangedDelegate,
	FGameplayTag, Tag,
	bool, bTagAdded);

// Charge Attack UI 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FChargeReadyTagDelegate,
	FGameplayTag, Tag,
	bool, bTagAdded);

// Q Skill UI 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FQSkillCooldownTagDelegate,
	FGameplayTag, Tag,
	bool, bTagAdded);

// Limit Gague 델리게이트
DECLARE_DELEGATE_OneParam(
	FLimitGagueChangedSignature,
	float);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnCooldownDelegate,
	FGameplayTag, CooldownTag,
	FActiveGameplayEffectHandle, Handle,
	float, StartTime,
	float, Duration
 );
UCLASS()
class GFANTASY_API UPlayerCharacterGASC : public UGASCBase
{
	GENERATED_BODY()


public:

	/*캐릭터 AS 설정 함수*/
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect);

	// AttributeSet 등록을 위한 public 함수 추가
	void RegisterAttributeSet(UAttributeSet* AttributeSet)
	{
		if (AttributeSet) AddAttributeSetSubobject(AttributeSet);
	}

	// InputDirection Getter / Setter	
	// 클라 -> 서버 : 입력 키 전송

	UFUNCTION(Server, Reliable)
	void Server_SetInputDirection(const FVector& Direction);
	
	// ASC 에서 Tag 기반 어빌리티 지연 실행
	UFUNCTION()
	void DeferredActivateAbilityByTag(FGameplayTag AbilityTag);

	// 장비아이템 효과 부여 / 제거
	void AddEquipmentEffects(FGFEquipmentEntry* EquipmentEntry);
	void RemoveEquipmentEffects(FGFEquipmentEntry* EquipmentEntry);

	// 리미트 게이지 증가 
	UFUNCTION()
	void HandleLimitGageEvent(AActor* InstigatorActor, bool bIsAttacker);

	FVector GetInputDirection() const { return InputDirection;  }	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* UI 쿨타임 */
	
	// 초기화 함수 오버라이드
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
	
	virtual void InitializeComponent() override;

	void HandleLimitGageChanged(float NewValue);
public:

	// Dodge에서 사용할 입력 키
	UPROPERTY(Replicated)
	FVector InputDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintAssignable)
	FOnCooldownDelegate OnCooldownStarted;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "LimitGE")
	TSubclassOf<UGameplayEffect> LimitHitGEClass;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "LimitGE")
	TSubclassOf<UGameplayEffect> LimitAttackGEClass;

	UPROPERTY(BlueprintAssignable,BlueprintReadWrite)
	FLBTagChangedDelegate OnLimitBreakTagChanged;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FChargeReadyTagDelegate OnChargingTagChanged;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FQSkillCooldownTagDelegate OnQSkillCooldownChanged;

	// 리미트 브레이크 사용 가능 Effect
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "LimitGE")
	TSubclassOf<UGameplayEffect> GE_LimitBreakReady;

	// 리미트 브레이크 사용가능 Effect 핸들
	FLimitGagueChangedSignature OnLimitGageChanged;

private:
	FActiveGameplayEffectHandle LimitBreakEffectHandle;
	
};

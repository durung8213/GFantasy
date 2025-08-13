// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GAS/AbilitySystemComponent/GASCBase.h"
#include "GameplayAbilities/Public/AttributeSet.h"
#include "PlayerCharacterGASC.generated.h"


struct FGFEquipmentEntry;

// Limit Break UI ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FLBTagChangedDelegate,
	FGameplayTag, Tag,
	bool, bTagAdded);

// Charge Attack UI ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FChargeReadyTagDelegate,
	FGameplayTag, Tag,
	bool, bTagAdded);

// Q Skill UI ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FQSkillCooldownTagDelegate,
	FGameplayTag, Tag,
	bool, bTagAdded);

// Limit Gague ��������Ʈ
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

	/*ĳ���� AS ���� �Լ�*/
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect);

	// AttributeSet ����� ���� public �Լ� �߰�
	void RegisterAttributeSet(UAttributeSet* AttributeSet)
	{
		if (AttributeSet) AddAttributeSetSubobject(AttributeSet);
	}

	// InputDirection Getter / Setter	
	// Ŭ�� -> ���� : �Է� Ű ����

	UFUNCTION(Server, Reliable)
	void Server_SetInputDirection(const FVector& Direction);
	
	// ASC ���� Tag ��� �����Ƽ ���� ����
	UFUNCTION()
	void DeferredActivateAbilityByTag(FGameplayTag AbilityTag);

	// �������� ȿ�� �ο� / ����
	void AddEquipmentEffects(FGFEquipmentEntry* EquipmentEntry);
	void RemoveEquipmentEffects(FGFEquipmentEntry* EquipmentEntry);

	// ����Ʈ ������ ���� 
	UFUNCTION()
	void HandleLimitGageEvent(AActor* InstigatorActor, bool bIsAttacker);

	FVector GetInputDirection() const { return InputDirection;  }	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* UI ��Ÿ�� */
	
	// �ʱ�ȭ �Լ� �������̵�
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
	
	virtual void InitializeComponent() override;

	void HandleLimitGageChanged(float NewValue);
public:

	// Dodge���� ����� �Է� Ű
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

	// ����Ʈ �극��ũ ��� ���� Effect
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "LimitGE")
	TSubclassOf<UGameplayEffect> GE_LimitBreakReady;

	// ����Ʈ �극��ũ ��밡�� Effect �ڵ�
	FLimitGagueChangedSignature OnLimitGageChanged;

private:
	FActiveGameplayEffectHandle LimitBreakEffectHandle;
	
};

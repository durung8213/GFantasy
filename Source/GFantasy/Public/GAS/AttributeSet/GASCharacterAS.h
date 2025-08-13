// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "GameplayAbilities/Public/AttributeSet.h"
#include "GASCharacterAS.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNormalHitDelegate, const FGameplayEffectSpec&, EffectSpec, const FGameplayModifierEvaluatedData&, EvaluatedData, const UAbilitySystemComponent*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FStrongHitDelegate, const FGameplayEffectSpec&, EffectSpec, const FGameplayModifierEvaluatedData&, EvaluatedData, const UAbilitySystemComponent*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnDamageDelegate,
	AActor*, Instigator,
	float, DamageAmount, 
	bool, bIsStrong
	);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnLimitGageDelegate,
	AActor*, Instigator,
	bool, bIsAttacker	// 플레이어가 공격한건지, 맞은건지 구분.
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEnemyDieToNormalHitDelegate, const FGameplayEffectSpec&, EffectSpec, const FGameplayModifierEvaluatedData&, EvaluatedData, const UAbilitySystemComponent*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEnemyDieToStrongHitDelegate, const FGameplayEffectSpec&, EffectSpec, const FGameplayModifierEvaluatedData&, EvaluatedData, const UAbilitySystemComponent*, Target);



#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GFANTASY_API UGASCharacterAS : public UAttributeSet
{
	GENERATED_BODY()
          
protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldSpeed);
public:

	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, Health)

	// 최대 체력 치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, MaxHealth)

	// 스테미나
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, Stamina)

	// 최대 스테미나
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, MaxStamina)

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, MoveSpeed);

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	FGameplayAttributeData NormalAttackToEnemy;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, NormalAttackToEnemy);

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	FGameplayAttributeData StrongAttackToEnemy;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, StrongAttackToEnemy);
	
	UPROPERTY(BlueprintReadWrite, Category = "Character")
	FGameplayAttributeData NormalAttackToPlayer;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, NormalAttackToPlayer);

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	FGameplayAttributeData StrongAttackToPlayer;
	ATTRIBUTE_ACCESSORS(UGASCharacterAS, StrongAttackToPlayer);

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FNormalHitDelegate NormalHitDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FEnemyDieToNormalHitDelegate StrongHitDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FEnemyDieToNormalHitDelegate EnemyDieToNormalHitDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FEnemyDieToStrongHitDelegate EnemyDieToStrongHitDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnLimitGageDelegate IncreaseLimitGage;

	// 캐릭터 데미지 적용 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnDamageDelegate OnDamageDelegate;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	// 현재 수치가 최대 수치를 넘지 못하도록 Clamp 하는 함수
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	void ApplyNormalDamage(const FGameplayEffectModCallbackData& Data);
	void ApplyStrongDamage(const FGameplayEffectModCallbackData& Data);

private:
	void HandleDamage(const FGameplayEffectModCallbackData& Data, bool bStrong);
};

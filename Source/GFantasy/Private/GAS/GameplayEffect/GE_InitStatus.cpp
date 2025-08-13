// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameplayEffect/GE_InitStatus.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GAS/AttributeSet/GASCombatAS.h"
#include "GAS/AttributeSet/GASResourceAS.h"

UGE_InitStatus::UGE_InitStatus()
{
	/* 기본 체력 추가 */
	// 게임 이펙트 모드 설정
	//Instant -> 효과가 즉시 적용. 지속시간 X 단발성 효과.
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// 속성을 구조체 단위로 하나 생성
	FGameplayModifierInfo HealthMod;
	// AS 에 있는 체력 가져오기.
	HealthMod.Attribute = UGASCharacterAS::GetHealthAttribute();
	
	//어떤 방식으로 해당 수치를 변경할 지, 지정
	// Additive -> 기존 값에 해당 값을 더하는 방식.
	// 덮어 씌우는 Override, 곱하는 Multiplicative 도 있음.
	HealthMod.ModifierOp = EGameplayModOp::Additive;
	HealthMod.ModifierMagnitude = FScalableFloat(550.f);

	// GameplayEffect 는 여러개의 Modifier를 가짐.
	// 해당 배열에 추가해서 속성값을 얻게 한다.
	this->Modifiers.Add(HealthMod);


	// 최대 체력
	FGameplayModifierInfo MaxHealthMod;
	MaxHealthMod.Attribute = UGASCharacterAS::GetMaxHealthAttribute();
	MaxHealthMod.ModifierOp = EGameplayModOp::Additive;
	MaxHealthMod.ModifierMagnitude = FScalableFloat(600.f);
	this->Modifiers.Add(MaxHealthMod);

	// 이동 속도
	FGameplayModifierInfo MoveSpeedMod;
	MoveSpeedMod.Attribute = UGASCharacterAS::GetMoveSpeedAttribute();
	MoveSpeedMod.ModifierOp = EGameplayModOp::Additive;
	MoveSpeedMod.ModifierMagnitude = FScalableFloat(600.f);
	this->Modifiers.Add(MoveSpeedMod);

	// 공격력
	FGameplayModifierInfo AtkMod;
	AtkMod.Attribute = UGASCombatAS::GetAttackPowerAttribute();
	AtkMod.ModifierOp = EGameplayModOp::Additive;
	AtkMod.ModifierMagnitude = FScalableFloat(50.f);
	this->Modifiers.Add(AtkMod);

	// 공격 속도
	//FGameplayModifierInfo AtkSpeedMod;
	//AtkSpeedMod.Attribute = UGASCombatAS::GetAttackSpeedAttribute();
	//AtkSpeedMod.ModifierOp = EGameplayModOp::Additive;
	//AtkSpeedMod.ModifierMagnitude = FScalableFloat(10.f);
	//this->Modifiers.Add(AtkSpeedMod);

	// 치명타 확률
	FGameplayModifierInfo CRTChanceMod;
	CRTChanceMod.Attribute = UGASCombatAS::GetCriticalChanceAttribute();
	CRTChanceMod.ModifierOp = EGameplayModOp::Additive;
	CRTChanceMod.ModifierMagnitude = FScalableFloat(0.2f);
	this->Modifiers.Add(CRTChanceMod);
	
	// 치명타 데미지
	FGameplayModifierInfo CRTDamageMod;
	CRTDamageMod.Attribute = UGASCombatAS::GetCriticalDamageAttribute();
	CRTDamageMod.ModifierOp = EGameplayModOp::Additive;
	CRTDamageMod.ModifierMagnitude = FScalableFloat(1.3f);
	this->Modifiers.Add(CRTDamageMod);
	
}
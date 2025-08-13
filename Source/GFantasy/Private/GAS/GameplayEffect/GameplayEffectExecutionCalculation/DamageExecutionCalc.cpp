// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffect/GameplayEffectExecutionCalculation/DamageExecutionCalc.h"

#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GAS/AttributeSet/GASCombatAS.h"


UDamageExecutionCalc::UDamageExecutionCalc()
{
	// 소스(Instigator) 쪽 attribute 캡처 / 소스 -> 어빌리티 시전자의 속성
	AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(
		UGASCombatAS::GetAttackPowerAttribute(),
		EGameplayEffectAttributeCaptureSource::Source, true);

	CritChanceDef = FGameplayEffectAttributeCaptureDefinition(
		UGASCombatAS::GetCriticalChanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Source, true);

	CritDamageDef = FGameplayEffectAttributeCaptureDefinition(
		UGASCombatAS::GetCriticalDamageAttribute(),
		EGameplayEffectAttributeCaptureSource::Source, true);

	// 계산에서 참조할 속성 리스트에 추가
	RelevantAttributesToCapture.Add(AttackPowerDef);
	RelevantAttributesToCapture.Add(CritChanceDef);
	RelevantAttributesToCapture.Add(CritDamageDef);
}

void UDamageExecutionCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.SourceTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	// 속성 읽기 : 미리 캡처한 속성 가져오기
	float AttackPower = 0.f;
	float CritChance = 0.f;
	float CritDamage = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AttackPowerDef, EvaluateParams, AttackPower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritChanceDef, EvaluateParams, CritChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritDamageDef, EvaluateParams, CritDamage);

	// 어빌리티에서 넘겨준 SkillRatio 가져오기. ( 기본값 1.0 )
	float SkillRatio = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.SkillRatio"),true,1.0f);

	// 데미지 계산 스킬 계수 곱하기
	float RawDamage = AttackPower * SkillRatio;

	// 크리티컬 확률 적용
	const bool bIsCriticalHit = FMath::FRand() <= CritChance;
	if (bIsCriticalHit)
	{
		// 치명타 태그 추가
		RawDamage *= CritDamage;
	}

	// 타겟의 Health Attribute에 적용 (Additive 방식)
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UGASCharacterAS::GetHealthAttribute(),
		EGameplayModOp::Additive,
		-RawDamage));	// 피해량이므로 음수
	
}

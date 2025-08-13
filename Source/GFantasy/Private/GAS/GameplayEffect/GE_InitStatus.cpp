// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameplayEffect/GE_InitStatus.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GAS/AttributeSet/GASCombatAS.h"
#include "GAS/AttributeSet/GASResourceAS.h"

UGE_InitStatus::UGE_InitStatus()
{
	/* �⺻ ü�� �߰� */
	// ���� ����Ʈ ��� ����
	//Instant -> ȿ���� ��� ����. ���ӽð� X �ܹ߼� ȿ��.
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// �Ӽ��� ����ü ������ �ϳ� ����
	FGameplayModifierInfo HealthMod;
	// AS �� �ִ� ü�� ��������.
	HealthMod.Attribute = UGASCharacterAS::GetHealthAttribute();
	
	//� ������� �ش� ��ġ�� ������ ��, ����
	// Additive -> ���� ���� �ش� ���� ���ϴ� ���.
	// ���� ����� Override, ���ϴ� Multiplicative �� ����.
	HealthMod.ModifierOp = EGameplayModOp::Additive;
	HealthMod.ModifierMagnitude = FScalableFloat(550.f);

	// GameplayEffect �� �������� Modifier�� ����.
	// �ش� �迭�� �߰��ؼ� �Ӽ����� ��� �Ѵ�.
	this->Modifiers.Add(HealthMod);


	// �ִ� ü��
	FGameplayModifierInfo MaxHealthMod;
	MaxHealthMod.Attribute = UGASCharacterAS::GetMaxHealthAttribute();
	MaxHealthMod.ModifierOp = EGameplayModOp::Additive;
	MaxHealthMod.ModifierMagnitude = FScalableFloat(600.f);
	this->Modifiers.Add(MaxHealthMod);

	// �̵� �ӵ�
	FGameplayModifierInfo MoveSpeedMod;
	MoveSpeedMod.Attribute = UGASCharacterAS::GetMoveSpeedAttribute();
	MoveSpeedMod.ModifierOp = EGameplayModOp::Additive;
	MoveSpeedMod.ModifierMagnitude = FScalableFloat(600.f);
	this->Modifiers.Add(MoveSpeedMod);

	// ���ݷ�
	FGameplayModifierInfo AtkMod;
	AtkMod.Attribute = UGASCombatAS::GetAttackPowerAttribute();
	AtkMod.ModifierOp = EGameplayModOp::Additive;
	AtkMod.ModifierMagnitude = FScalableFloat(50.f);
	this->Modifiers.Add(AtkMod);

	// ���� �ӵ�
	//FGameplayModifierInfo AtkSpeedMod;
	//AtkSpeedMod.Attribute = UGASCombatAS::GetAttackSpeedAttribute();
	//AtkSpeedMod.ModifierOp = EGameplayModOp::Additive;
	//AtkSpeedMod.ModifierMagnitude = FScalableFloat(10.f);
	//this->Modifiers.Add(AtkSpeedMod);

	// ġ��Ÿ Ȯ��
	FGameplayModifierInfo CRTChanceMod;
	CRTChanceMod.Attribute = UGASCombatAS::GetCriticalChanceAttribute();
	CRTChanceMod.ModifierOp = EGameplayModOp::Additive;
	CRTChanceMod.ModifierMagnitude = FScalableFloat(0.2f);
	this->Modifiers.Add(CRTChanceMod);
	
	// ġ��Ÿ ������
	FGameplayModifierInfo CRTDamageMod;
	CRTDamageMod.Attribute = UGASCombatAS::GetCriticalDamageAttribute();
	CRTDamageMod.ModifierOp = EGameplayModOp::Additive;
	CRTDamageMod.ModifierMagnitude = FScalableFloat(1.3f);
	this->Modifiers.Add(CRTDamageMod);
	
}
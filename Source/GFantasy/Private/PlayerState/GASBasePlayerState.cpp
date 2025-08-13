// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerState/GASBasePlayerState.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GAS/AttributeSet/GASCombatAS.h"
#include "GAS/AttributeSet/GASResourceAS.h"
#include "GameplayEffect.h"



AGASBasePlayerState::AGASBasePlayerState()
{
	// AbilitySystemComponent �ʱ�ȭ
	ASC = CreateDefaultSubobject<UPlayerCharacterGASC>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet �ʱ�ȭ
    CharacterAS = CreateDefaultSubobject<UGASCharacterAS>(TEXT("CharacterAS"));
    CombatAS = CreateDefaultSubobject<UGASCombatAS>(TEXT("CombatAS"));
    ResourceAS = CreateDefaultSubobject<UGASResourceAS>(TEXT("ResourceAS"));

    // 3. ASC�� ����� ��� (�߰��� RegisterAttributeSet ���)
    if (ASC)
    {
        ASC->RegisterAttributeSet(CharacterAS);
        ASC->RegisterAttributeSet(CombatAS);
        ASC->RegisterAttributeSet(ResourceAS);
    }

	// ��Ʈ��ũ ������Ʈ �� �� ����
	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 66.f;
}

UAbilitySystemComponent* AGASBasePlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

UPlayerCharacterGASC* AGASBasePlayerState::GetPlayerCharacterGASC() const
{
	return ASC;
}

UGASCharacterAS* AGASBasePlayerState::GetCharacterAttributeSet() const
{
	return CharacterAS;
}

UGASCombatAS* AGASBasePlayerState::GetCombatAttributeSet() const
{
	return CombatAS;
}

UGASResourceAS* AGASBasePlayerState::GetResourceAttributeSet() const
{
	return ResourceAS;
}

void AGASBasePlayerState::InitDefaultAttributes()
{
	if (!GetAbilitySystemComponent())	return;
		
	// �ʱ� �ɷ�ġ �ο�

    UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
    if (!AbilitySystem)
    {
        UE_LOG(LogTemp, Error, TEXT("InitDefaultAttributes: AbilitySystemComponent is null!"));
        return;
    }

    // �ʱ� �ɷ�ġ �ο�
    if (!GE_InitStatus)
    {
        UE_LOG(LogTemp, Error, TEXT("InitDefaultAttributes: GE_InitStatus is null!"));
        return;
    }

    // ������ �ʱ�ȭ �õ�
    FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
    if (!EffectContext.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("InitDefaultAttributes: Failed to create EffectContext!"));
        return;
    }

    FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(GE_InitStatus, 1.0f, EffectContext);
    if (!SpecHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("InitDefaultAttributes: Failed to create SpecHandle!"));
        return;
    }

    FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    if (!ActiveGEHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("InitDefaultAttributes: Failed to apply GameplayEffect!"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("InitDefaultAttributes: Successfully applied GameplayEffect!"));
    }
}

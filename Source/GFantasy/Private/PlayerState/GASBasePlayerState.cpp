// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerState/GASBasePlayerState.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GAS/AttributeSet/GASCombatAS.h"
#include "GAS/AttributeSet/GASResourceAS.h"
#include "GameplayEffect.h"



AGASBasePlayerState::AGASBasePlayerState()
{
	// AbilitySystemComponent 초기화
	ASC = CreateDefaultSubobject<UPlayerCharacterGASC>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet 초기화
    CharacterAS = CreateDefaultSubobject<UGASCharacterAS>(TEXT("CharacterAS"));
    CombatAS = CreateDefaultSubobject<UGASCombatAS>(TEXT("CombatAS"));
    ResourceAS = CreateDefaultSubobject<UGASResourceAS>(TEXT("ResourceAS"));

    // 3. ASC에 명시적 등록 (추가된 RegisterAttributeSet 사용)
    if (ASC)
    {
        ASC->RegisterAttributeSet(CharacterAS);
        ASC->RegisterAttributeSet(CombatAS);
        ASC->RegisterAttributeSet(ResourceAS);
    }

	// 네트워크 업데이트 빈도 수 증가
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
		
	// 초기 능력치 부여

    UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
    if (!AbilitySystem)
    {
        UE_LOG(LogTemp, Error, TEXT("InitDefaultAttributes: AbilitySystemComponent is null!"));
        return;
    }

    // 초기 능력치 부여
    if (!GE_InitStatus)
    {
        UE_LOG(LogTemp, Error, TEXT("InitDefaultAttributes: GE_InitStatus is null!"));
        return;
    }

    // 안전한 초기화 시도
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

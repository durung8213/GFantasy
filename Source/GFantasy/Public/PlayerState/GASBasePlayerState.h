// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/AbilitySystemInterface.h"
#include "GASBasePlayerState.generated.h"


class UGASCharacterAS;
class UGASCombatAS;
class UGASResourceAS;
class UPlayerCharacterGASC;
class UGameplayEffect;


/**
 * 
 */
UCLASS()
class GFANTASY_API AGASBasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AGASBasePlayerState();

	// 어빌리티 시스템 인터페이스 구현
	// ASC 구현
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	UPlayerCharacterGASC* GetPlayerCharacterGASC() const;

	UFUNCTION(BlueprintPure)
	UGASCharacterAS* GetCharacterAttributeSet() const;

	UFUNCTION(BlueprintPure)
	UGASCombatAS* GetCombatAttributeSet() const;

	UFUNCTION(BlueprintPure)
	UGASResourceAS* GetResourceAttributeSet() const;

	UFUNCTION()
	virtual void InitDefaultAttributes();

public:
	// ASC
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UPlayerCharacterGASC> ASC;

	//AS ( Character AS )
	UPROPERTY()
	TObjectPtr<UGASCharacterAS> CharacterAS;

	UPROPERTY()
	TObjectPtr<UGASCombatAS> CombatAS;

	UPROPERTY()
	TObjectPtr<UGASResourceAS> ResourceAS;

	UPROPERTY(EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> GE_InitStatus;

	bool bIsInitialize = false;

};

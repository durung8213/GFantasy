// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASNonCombatNPC.h"
#include "GASQuestBaseNPC.generated.h"

/**
 * 
 */

UCLASS()
class GFANTASY_API AGASQuestBaseNPC : public AGASNonCombatNPC
{
	GENERATED_BODY()
	
public:
	AGASQuestBaseNPC();
	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void ShowInteractionWidget(AActor* Interactor) override;
	FGameplayTag GetNPCTag() const;

private:

	// �±� ���ڿ����� ������ '.' ���� �κи� ��ȯ
	FString GetNameFromTag(const FString& FullTag) const;
};

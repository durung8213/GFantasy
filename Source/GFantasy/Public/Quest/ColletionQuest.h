// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestInstance.h"
#include "ColletionQuest.generated.h"

/**
 * 
 */


UCLASS()
class GFANTASY_API UColletionQuest : public UQuestInstance
{
	GENERATED_BODY()
	
public:
	// TargetName을 인벤토리에서 가져오도록 오버라이드
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow) override;

	// Quest 수락시 호출될 함수
	virtual void OnQuestAccepted_Implementation() override;

	// CurrentCount변경후 진행도 변경
	virtual void SetCount(int32 Amount) override;
	
	// 아이템 수량 변경 델리게이트 핸들러
	UFUNCTION()
	void HandleItemCountChanged(const FGameplayTag& ItemTag, int32 NewCount);

private:
};

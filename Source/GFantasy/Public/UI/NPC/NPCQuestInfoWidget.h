// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NPC/BaseNPCQuestWidget.h"
#include "NPCQuestInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UNPCQuestInfoWidget : public UBaseNPCQuestWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitializeWithQuest(UQuestInstance* InQuest, UQuestManagerComponent* InManager, UNPCQuestListWidget* InParentList, FGameplayTag InTag);


protected:
	// 위젯 끄는 버튼
	UFUNCTION()
	void OnExitButtonClicked();

	// 퀘스트 수락/완료 버튼
	UFUNCTION()
	void OnQuestEnterButtonClicked();

	UFUNCTION()
	void OnAcceptClicked();

	UFUNCTION()
	void OnCompleteClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestDescription;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TargetName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RequiredCount;

	UPROPERTY(meta = (BindWidget))
	class UImage* RewardImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RewardName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RewardCount;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuestEnterButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestButtonText;

protected:
	class UQuestManagerComponent* ManagerComponent;

	// 기본 색상
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance|Quest State Colors")
	FLinearColor NormalColor = FLinearColor::White;

	// 진행 중(InProgress)이지만 완료 불가능할 때 텍스트 색상 (에디터에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance|Quest State Colors")
	FLinearColor InProgressColor = FLinearColor::Red;

	// 진행 중이면서 완료 가능할 때 텍스트 색상 (에디터에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance|Quest State Colors")
	FLinearColor CanCompleteColor = FLinearColor::Blue;

private:
	UPROPERTY()
	UQuestInstance* Quest;

	UPROPERTY()
	class UNPCQuestListWidget* ParentListWidget;

	FGameplayTag AvailableTag;
	FGameplayTag InProgressTag;

	// 상호작용 중인 NPC의 태그를 저장
	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FGameplayTag InteractNPCTag;
};

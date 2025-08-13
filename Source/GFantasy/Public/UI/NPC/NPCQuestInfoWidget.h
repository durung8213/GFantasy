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
	// ���� ���� ��ư
	UFUNCTION()
	void OnExitButtonClicked();

	// ����Ʈ ����/�Ϸ� ��ư
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

	// �⺻ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance|Quest State Colors")
	FLinearColor NormalColor = FLinearColor::White;

	// ���� ��(InProgress)������ �Ϸ� �Ұ����� �� �ؽ�Ʈ ���� (�����Ϳ��� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance|Quest State Colors")
	FLinearColor InProgressColor = FLinearColor::Red;

	// ���� ���̸鼭 �Ϸ� ������ �� �ؽ�Ʈ ���� (�����Ϳ��� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance|Quest State Colors")
	FLinearColor CanCompleteColor = FLinearColor::Blue;

private:
	UPROPERTY()
	UQuestInstance* Quest;

	UPROPERTY()
	class UNPCQuestListWidget* ParentListWidget;

	FGameplayTag AvailableTag;
	FGameplayTag InProgressTag;

	// ��ȣ�ۿ� ���� NPC�� �±׸� ����
	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FGameplayTag InteractNPCTag;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NPC/BaseNPCQuestWidget.h"
#include "NPCQuestWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UNPCQuestWidget : public UBaseNPCQuestWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitializeWithQuest(UQuestInstance* InQuest, UQuestManagerComponent* InManager, UNPCQuestListWidget* InListWidget, FGameplayTag InTag);

	void CloseNPCQuestWidget();

protected:
	UFUNCTION()
	void OnQuestButtonClicked();

public:
	// ���� �� �� ������ ����Ʈ ���� ���� �ν��Ͻ�. ��� NPCQuestWidget�� �̰� �����Ѵ�.
	UPROPERTY()
	class UNPCQuestInfoWidget* NPCQuestInfoWidget;

	static UNPCQuestInfoWidget* SharedInfoWidgetInstance;

	UPROPERTY()
	class UNPCQuestListWidget* ParentListWidget;



protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* QuestButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RewardItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	TSubclassOf<class UNPCQuestInfoWidget> NPCQuestInfoWidgetClass;

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
	/** InitializeWithQuest() ȣ�� �� ���õǴ� ����Ʈ �ν��Ͻ� */
	UPROPERTY()
	UQuestInstance* Quest;

	/** InitializeWithQuest() ȣ�� �� ���õǴ� �Ŵ��� ������Ʈ */
	UPROPERTY()
	UQuestManagerComponent* Manager;

	// ��ȣ�ۿ� ���� NPC�� �±׸� ����
	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FGameplayTag InteractNPCTag;
};

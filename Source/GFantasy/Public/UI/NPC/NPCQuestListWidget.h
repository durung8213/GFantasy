// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NPC/BaseNPCQuestWidget.h"
#include "NPCQuestListWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UNPCQuestListWidget : public UBaseNPCQuestWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void RefreshQuestList();

	UFUNCTION(BlueprintCallable, Category = "NPC")
	void InitializeWithNPCTag(FGameplayTag InNPCTag);

protected:
	UFUNCTION()
	void OnExitButtonClicked();


private:
	void InitializeQuestList();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* NPCQuestScrollBox;

	class UQuestManagerComponent* QuestManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	TSubclassOf<class UNPCQuestWidget> NPCQuestWidgetClass;

	// ��ũ�ѹڽ��� �߰��� ��� ����Ʈ ��Ʈ�� ������ ĳ��
	UPROPERTY()
	TArray<UNPCQuestWidget*> CachedQuestWidgets;

public:
	// ��ȣ�ۿ� ���� NPC�� �±׸� ����
	UPROPERTY(BlueprintReadOnly, Category = "NPC")
	FGameplayTag InteractNPCTag;
};

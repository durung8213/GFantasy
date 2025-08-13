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
	// 최초 한 번 생성된 퀘스트 정보 위젯 인스턴스. 모든 NPCQuestWidget이 이걸 공유한다.
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
	/** InitializeWithQuest() 호출 시 세팅되는 퀘스트 인스턴스 */
	UPROPERTY()
	UQuestInstance* Quest;

	/** InitializeWithQuest() 호출 시 세팅되는 매니저 컴포넌트 */
	UPROPERTY()
	UQuestManagerComponent* Manager;

	// 상호작용 중인 NPC의 태그를 저장
	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FGameplayTag InteractNPCTag;
};

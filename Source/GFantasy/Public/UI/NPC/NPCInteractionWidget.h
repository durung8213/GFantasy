// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "NPCInteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UNPCInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 위젯 초기 설정
	virtual void NativeConstruct() override;
	
	// 게임 내에서 NPC 태그를 받아서 저장·사용
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void InitializeWithNPCTag(FGameplayTag InNPCTag, const FText& InNameText, ENPCType InType);

protected:
	// 위젯 끄는 버튼
	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

	// 각 NPC에 맞는 확인 버튼
	UFUNCTION()
	void OnInteractButtonClicked();

	// 퀘스트용 위젯 띄우기
	UFUNCTION()
	void ShowQuestWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowNormalNPCWidget();
	virtual void ShowNormalNPCWidget_Implementation();

	// 대장장이용 위젯 띄우기
	// 제작
	UFUNCTION(BlueprintImplementableEvent)
	void ShowCraftWidget();
	virtual void ShowCraftWidget_Implementation();

	// 강화
	UFUNCTION(BlueprintImplementableEvent)
	void ShowEnhanceWidget();
	virtual void ShowEnhanceWidget_Implementation();

	// 합성
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSynthesisWidget();
	virtual void ShowSynthesisWidget_Implementation();

	// 상점용 위젯 띄우기
	UFUNCTION(BlueprintImplementableEvent)
	void ShowMerchantWidget();
	virtual void ShowMerchantWidget_Implementation();

public:
	FGameplayTag GetNPCNameTag() const;
	void SetNPCNameText(/*FGameplayTag InNPCTag,*/ const FText& InNameText);

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NPCName;

	// 타입에 따라 다른 글씨 출력
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractText;

	// 타입에 따라 다른 위젯
	UPROPERTY(meta = (BindWidget))
	class UButton* InteractButton;


	// 퀘스트 목록 위젯을 생성할 클래스 (에디터에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	TSubclassOf<class UNPCQuestListWidget> NPCQuestListWidgetClass;

	// 상호작용 중인 NPC의 태그를 저장
	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FGameplayTag NPCInteractionTag;

	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FText NPCInteractionName;

	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	ENPCType NPCInteractionType;
};

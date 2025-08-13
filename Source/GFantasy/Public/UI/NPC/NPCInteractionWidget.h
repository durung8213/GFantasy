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
	// ���� �ʱ� ����
	virtual void NativeConstruct() override;
	
	// ���� ������ NPC �±׸� �޾Ƽ� ���塤���
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void InitializeWithNPCTag(FGameplayTag InNPCTag, const FText& InNameText, ENPCType InType);

protected:
	// ���� ���� ��ư
	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

	// �� NPC�� �´� Ȯ�� ��ư
	UFUNCTION()
	void OnInteractButtonClicked();

	// ����Ʈ�� ���� ����
	UFUNCTION()
	void ShowQuestWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowNormalNPCWidget();
	virtual void ShowNormalNPCWidget_Implementation();

	// �������̿� ���� ����
	// ����
	UFUNCTION(BlueprintImplementableEvent)
	void ShowCraftWidget();
	virtual void ShowCraftWidget_Implementation();

	// ��ȭ
	UFUNCTION(BlueprintImplementableEvent)
	void ShowEnhanceWidget();
	virtual void ShowEnhanceWidget_Implementation();

	// �ռ�
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSynthesisWidget();
	virtual void ShowSynthesisWidget_Implementation();

	// ������ ���� ����
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

	// Ÿ�Կ� ���� �ٸ� �۾� ���
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractText;

	// Ÿ�Կ� ���� �ٸ� ����
	UPROPERTY(meta = (BindWidget))
	class UButton* InteractButton;


	// ����Ʈ ��� ������ ������ Ŭ���� (�����Ϳ��� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	TSubclassOf<class UNPCQuestListWidget> NPCQuestListWidgetClass;

	// ��ȣ�ۿ� ���� NPC�� �±׸� ����
	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FGameplayTag NPCInteractionTag;

	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FText NPCInteractionName;

	UPROPERTY(BlueprintReadOnly, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	ENPCType NPCInteractionType;
};

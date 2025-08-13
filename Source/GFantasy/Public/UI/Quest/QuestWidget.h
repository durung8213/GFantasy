// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseUserQuestWidget.h"
#include "QuestWidget.generated.h"

 // ������ ���ε��� TextBlock ���� ����
class UTextBlock;
class UQuestInstance;

// üũ�ڽ� ���� �� ��ε�ĳ��Ʈ�� ��������Ʈ: üũ ����, ����Ʈ �ν��Ͻ� ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTraceCheckChanged, UQuestInstance*, QuestInstance, bool, bChecked);

UCLASS()
class GFANTASY_API UQuestWidget : public UBaseUserQuestWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    // QuestInstance�� ��������Ʈ ���ε�
    UFUNCTION(BlueprintCallable, Category = "Quest|UI")
    void BindToQuest(UQuestInstance* QuestInstance);

    // QuestInstance  ǥ���� ����Ʈ �ν��Ͻ�
    UFUNCTION(BlueprintCallable, Category = "Quest|UI")
    void InitializeWidget(UQuestInstance* QuestInstance);

    void SetQuestName(UQuestInstance* QuestInstance);
    void SetQuestDescription(UQuestInstance* QuestInstance);
    void SetQuestState(UQuestInstance* QuestInstance);
    void SetCurrentCount(UQuestInstance* QuestInstance);
    void SetRequiredCount(UQuestInstance* QuestInstance);
    void SetProgressPercent(UQuestInstance* QuestInstance);
    void SetCheckBox(UQuestInstance* QuestInstance);

    // ��������Ʈ: üũ�ڽ� ��� �� ȣ��
    UPROPERTY(BlueprintAssignable, Category = "Quest|Indicator")
    FOnTraceCheckChanged OnTraceCheckChanged;

    // üũ�ڽ� ���º��� �ڵ鷯
    UFUNCTION()
    void HandleTraceCheckChanged(bool bIsChecked);


private:
    // �±��� ������ .���ǰ��� �����ִ� �Լ�
    FString GetStateNameFromTag(const FString& FullTag) const;

private:
    UPROPERTY()
    UQuestInstance* CurrentQuestInstance = nullptr;


public:
    // ����Ʈ �̸��� ǥ���ϴ� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestName;

    // ����Ʈ ������ ǥ���ϴ� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestDescription;

    // ����Ʈ ����(�±�)�� ǥ���ϴ� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestState;

    // ���� ����/óġ ������ ǥ���ϴ� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentCount;

    // �䱸 ������ ǥ���ϴ� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* RequiredCount;

    // �����(%)�� ǥ���ϴ� �ؽ�Ʈ */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ProgressPercent;


    UPROPERTY(meta = (BindWidget))
    class UCheckBox* TraceOnOffCheck;
	
   };

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "BaseQuestWidget.generated.h"

/**
 * 
 */
class UQuestInstance;

UCLASS()
class GFANTASY_API UBaseQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // ����Ʈ ���� �������� ����� ������ �ʱ�ȭ�ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void InitializeQuestData(
        const TArray<UQuestInstance*>& InAllQuests,
        const TArray<UQuestInstance*>& InAvailable,
        const TArray<UQuestInstance*>& InInProgress,
        const TArray<UQuestInstance*>& InCompleted
    );

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetAvailableArr(TArray<UQuestInstance*>& InArr);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetInProgressArr(TArray<UQuestInstance*>& InArr);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetCompletedArr(TArray<UQuestInstance*>& InArr);
	
protected:
    // �������� �����ؼ������� �ֱ�, �̰ɷ� UI �����ϵ���
    // ��� ����Ʈ �ν��Ͻ� �迭
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> AllQuestArr;

    // ���� ���� ����Ʈ �迭
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> AvailableArr;

    // ���� �� ����Ʈ �迭
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> InProgressArr;

    // �Ϸ�� ����Ʈ �迭
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> CompletedArr;



    // ����Ʈ ���� ���п� �±�: Locked
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagLocked;

    // ����Ʈ ���� ���п� �±�: Available
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagAvailable;

    // ����Ʈ ���� ���п� �±�: InProgress
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagInProgress;

    // ����Ʈ ���� ���п� �±�: Completed
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagCompleted;
};

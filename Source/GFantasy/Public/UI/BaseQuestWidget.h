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
    // 퀘스트 관련 위젯에서 사용할 변수들 초기화하는 함수
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
    // 서버에서 복사해서가지고 있기, 이걸로 UI 갱신하도록
    // 모든 퀘스트 인스턴스 배열
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> AllQuestArr;

    // 시작 가능 퀘스트 배열
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> AvailableArr;

    // 진행 중 퀘스트 배열
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> InProgressArr;

    // 완료된 퀘스트 배열
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<UQuestInstance*> CompletedArr;



    // 퀘스트 상태 구분용 태그: Locked
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagLocked;

    // 퀘스트 상태 구분용 태그: Available
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagAvailable;

    // 퀘스트 상태 구분용 태그: InProgress
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagInProgress;

    // 퀘스트 상태 구분용 태그: Completed
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FGameplayTag TagCompleted;
};

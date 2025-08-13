// DataTable 행 데이터를 런타임에 복사해 진행상태를 관리하는 객체

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GFantasy/GFantasy.h" // EQuestType 정의 포함
#include "GFantasy/Public/GAS/GamePlayTag.h"
#include "GameplayTagContainer.h"         // FGameplayTagContainer

#include "QuestInstance.generated.h"

struct FQuestDataTableRow;
class UQuestManagerComponent;
class AGASWorldPlayerState;
class APlayerController;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCountChanged, UQuestInstance*, QuestInstance);
// 퀘스트 상태 변화시 UserQuestWidget을 갱신하는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChanged, UQuestInstance*, QuestInstance, FGameplayTag, OldState);
// 퀘스트 상태 변화시 NPCQuestWidget을 갱신하는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNPCQuestListUpdated);

/**
 * 런타임 퀘스트 인스턴스 : 진행도, 완료 여부 관리
 */
UCLASS()
class GFANTASY_API UQuestInstance : public UObject
{
	GENERATED_BODY()
	
public:
    UQuestInstance();

public:
    // 네트워킹 복제를 허용하도록 오버라이드
    virtual bool IsSupportedForNetworking() const override
    {
        return true;
    }

    // 복제 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // DataTable로부터 데이터 복사, 초기화
	UFUNCTION(BlueprintCallable, Category = "Quest")
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow);

    // NPC와 상호작용 후 퀘스트 수락시 실행될 함수
    // 여기서 오버라이드로 각 타입에 맞는 설정해주기
    UFUNCTION(BlueprintNativeEvent, Category = "Quest")
    void OnQuestAccepted();
    virtual void OnQuestAccepted_Implementation();

    // 퀘스트 완료시 호출되는 함수
    UFUNCTION(BlueprintNativeEvent, Category = "Quest")
    void OnQuestCompleted();
    virtual void OnQuestCompleted_Implementation();

    // 상태 태그 설정 함수(서버에서 호출/이 함수를 호출하는 함수가 서버에서 실행 중임)
    void SetQuestStateTag(const FGameplayTag& NewStateTag);

    // CurrentCount 변경시 브로드캐스트용 델리게이트 프로퍼티
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestCountChanged OnCountChanged;

    // 상태 변경 델리게이트
    // 인디케이터의 상태를 변경 시켜줄 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestStateChanged OnQuestStateChanged;

    // 퀘스트 수락/완료 시 호출되는 델리게이트 (UI에서 바인딩하여 해당 퀘스트만 제거)
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnNPCQuestListUpdated OnNPCQuestListUpdated;

    UFUNCTION()
    void OnRep_QuestStateTag();

    UFUNCTION()
    void OnRep_bCanComplete();

    UFUNCTION()
    void OnRep_CurrentCount();

    // CurrentCount 변화
    virtual void SetCount(int32 Amount);

    // 진행률 계산
    void UpdateProgress();

    // 진행 완료 여부 반환
    void SetbCanComplete(int32 Progress);


    // 진행률(0~100) 반환
    UFUNCTION(BlueprintPure, Category = "Quest")
    int32 GetProgressPercent() const;

    // 보상 아이템 획득 함수
    virtual void GetReward();

    // 보상 아이템 개수 반환
    int32 GetRewardCount();

    // UI 표시를 위해 태그를 일반 텍스트로 바꿔주는 함수
    UFUNCTION(BlueprintCallable, Category = "Quest")
    FText ChangeStateTagToText(const FGameplayTag& InTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    static FText ChangeTagToText(const FGameplayTag& InTag);

protected:
    UPROPERTY()
    class UInventoryComponent* CachedInvenComp = nullptr;

    // 인벤토리를 찾아서 캐싱하는 함수
    void CacheInventoryComponent();

public:
    // DataTableRow에서 고대로 가져온 변수들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    bool bCanAcceptQuest;

    // 퀘스트 고유 ID
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FName QuestID;

    // 퀘스트 이름/설명
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText QuestName;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText QuestDescription;

    // NPC 태그
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag QuestStartNPCTag;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag QuestEndNPCTag;

    // 퀘스트 분류 타입
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    EQuestType QuestType;

    // 이 태그로 상태를 관리(Locked, Available, InProgress, Completed)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_QuestStateTag, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag QuestStateTag;

    // 이전 상태 임시 보관 변수
    FGameplayTag PreviousStateTag;


    // 목표 대상 태그, 요구 수량, 현재 수량
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetTag;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 RequiredCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentCount, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 CurrentCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetLocationTag;

    // 진행률 (0~100)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 QuestProgress;

    // 보상 아이템 태그/수량
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag RewardItemTag;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 RewardCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FName NextQuestID;


    // 대상, 보상아이템 이름과 이미지 - 위젯노출을 위한 정보저장
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText TargetName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText RewardItemName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> TargetIcon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> RewardItemIcon = nullptr;


    // 완료 가능 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bCanComplete, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    bool bCanComplete;

    // “체크 On/Off” 상태 저장용 플래그
    UPROPERTY()
    bool bIsTraced = false;
	
protected:
    bool CheckInventory(UQuestManagerComponent* QM, AGASWorldPlayerState* PS, APlayerController* PC, UInventoryComponent* InvenComp);

};

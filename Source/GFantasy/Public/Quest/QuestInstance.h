// DataTable �� �����͸� ��Ÿ�ӿ� ������ ������¸� �����ϴ� ��ü

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GFantasy/GFantasy.h" // EQuestType ���� ����
#include "GFantasy/Public/GAS/GamePlayTag.h"
#include "GameplayTagContainer.h"         // FGameplayTagContainer

#include "QuestInstance.generated.h"

struct FQuestDataTableRow;
class UQuestManagerComponent;
class AGASWorldPlayerState;
class APlayerController;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCountChanged, UQuestInstance*, QuestInstance);
// ����Ʈ ���� ��ȭ�� UserQuestWidget�� �����ϴ� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChanged, UQuestInstance*, QuestInstance, FGameplayTag, OldState);
// ����Ʈ ���� ��ȭ�� NPCQuestWidget�� �����ϴ� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNPCQuestListUpdated);

/**
 * ��Ÿ�� ����Ʈ �ν��Ͻ� : ���൵, �Ϸ� ���� ����
 */
UCLASS()
class GFANTASY_API UQuestInstance : public UObject
{
	GENERATED_BODY()
	
public:
    UQuestInstance();

public:
    // ��Ʈ��ŷ ������ ����ϵ��� �������̵�
    virtual bool IsSupportedForNetworking() const override
    {
        return true;
    }

    // ���� ����
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // DataTable�κ��� ������ ����, �ʱ�ȭ
	UFUNCTION(BlueprintCallable, Category = "Quest")
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow);

    // NPC�� ��ȣ�ۿ� �� ����Ʈ ������ ����� �Լ�
    // ���⼭ �������̵�� �� Ÿ�Կ� �´� �������ֱ�
    UFUNCTION(BlueprintNativeEvent, Category = "Quest")
    void OnQuestAccepted();
    virtual void OnQuestAccepted_Implementation();

    // ����Ʈ �Ϸ�� ȣ��Ǵ� �Լ�
    UFUNCTION(BlueprintNativeEvent, Category = "Quest")
    void OnQuestCompleted();
    virtual void OnQuestCompleted_Implementation();

    // ���� �±� ���� �Լ�(�������� ȣ��/�� �Լ��� ȣ���ϴ� �Լ��� �������� ���� ����)
    void SetQuestStateTag(const FGameplayTag& NewStateTag);

    // CurrentCount ����� ��ε�ĳ��Ʈ�� ��������Ʈ ������Ƽ
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestCountChanged OnCountChanged;

    // ���� ���� ��������Ʈ
    // �ε��������� ���¸� ���� ������ ��������Ʈ
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestStateChanged OnQuestStateChanged;

    // ����Ʈ ����/�Ϸ� �� ȣ��Ǵ� ��������Ʈ (UI���� ���ε��Ͽ� �ش� ����Ʈ�� ����)
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnNPCQuestListUpdated OnNPCQuestListUpdated;

    UFUNCTION()
    void OnRep_QuestStateTag();

    UFUNCTION()
    void OnRep_bCanComplete();

    UFUNCTION()
    void OnRep_CurrentCount();

    // CurrentCount ��ȭ
    virtual void SetCount(int32 Amount);

    // ����� ���
    void UpdateProgress();

    // ���� �Ϸ� ���� ��ȯ
    void SetbCanComplete(int32 Progress);


    // �����(0~100) ��ȯ
    UFUNCTION(BlueprintPure, Category = "Quest")
    int32 GetProgressPercent() const;

    // ���� ������ ȹ�� �Լ�
    virtual void GetReward();

    // ���� ������ ���� ��ȯ
    int32 GetRewardCount();

    // UI ǥ�ø� ���� �±׸� �Ϲ� �ؽ�Ʈ�� �ٲ��ִ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Quest")
    FText ChangeStateTagToText(const FGameplayTag& InTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    static FText ChangeTagToText(const FGameplayTag& InTag);

protected:
    UPROPERTY()
    class UInventoryComponent* CachedInvenComp = nullptr;

    // �κ��丮�� ã�Ƽ� ĳ���ϴ� �Լ�
    void CacheInventoryComponent();

public:
    // DataTableRow���� ���� ������ ������
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    bool bCanAcceptQuest;

    // ����Ʈ ���� ID
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FName QuestID;

    // ����Ʈ �̸�/����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText QuestName;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText QuestDescription;

    // NPC �±�
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag QuestStartNPCTag;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag QuestEndNPCTag;

    // ����Ʈ �з� Ÿ��
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    EQuestType QuestType;

    // �� �±׷� ���¸� ����(Locked, Available, InProgress, Completed)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_QuestStateTag, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag QuestStateTag;

    // ���� ���� �ӽ� ���� ����
    FGameplayTag PreviousStateTag;


    // ��ǥ ��� �±�, �䱸 ����, ���� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetTag;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 RequiredCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentCount, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 CurrentCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetLocationTag;

    // ����� (0~100)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 QuestProgress;

    // ���� ������ �±�/����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag RewardItemTag;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    int32 RewardCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FName NextQuestID;


    // ���, ��������� �̸��� �̹��� - ���������� ���� ��������
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText TargetName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FText RewardItemName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> TargetIcon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> RewardItemIcon = nullptr;


    // �Ϸ� ���� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bCanComplete, Replicated, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    bool bCanComplete;

    // ��üũ On/Off�� ���� ����� �÷���
    UPROPERTY()
    bool bIsTraced = false;
	
protected:
    bool CheckInventory(UQuestManagerComponent* QM, AGASWorldPlayerState* PS, APlayerController* PC, UInventoryComponent* InvenComp);

};

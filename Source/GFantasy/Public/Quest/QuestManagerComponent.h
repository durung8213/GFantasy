// PS에 부착해서 현재 진행 중인 퀘스트를 등록, 진행, 완료, 처리한다.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "QuestManagerComponent.generated.h"

class UQuestInstance;
class UQuestListWidget;
class UAvailableQuestWidget;
class UInProgressQuestWidget;
class UCompletedQuestWidget;
struct FQuestDataTableRow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GFANTASY_API UQuestManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	UQuestManagerComponent();

public:
    // 게임 시작 시 PS, PC 캐싱 / DataTable 로드 ->UI 표시
    virtual void BeginPlay() override;

    // 배열 복제를 위한 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 서브오브젝트 복제 지원
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
    bool IsReady() const;

    // DataTable, SaveData에서 퀘스트 정보를 로드해 인스턴스 생성
    void LoadQuestsFromDataTable();

    // Save 함수
    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveQuestData(TArray<FQuestSaveData>& OutSaveQuests) const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    void LoadQuestSaveData(const TArray<FQuestSaveData>& SavedQuests);
    
    // 페이징 로드를 시작할 때 총 퀘스트 개수를 알려줍니다.
    void PrepareForLoad(int32 TotalQuests);

    // 페이지 단위로 넘어온 데이터를 받아 누적 로드합니다.
    void LoadQuestPage(int32 PageIndex, const TArray<FQuestSaveData>& QuestPage);

    // 서버·클라 공통: 준비 완료 델리게이트
    DECLARE_MULTICAST_DELEGATE(FOnQuestManagerReady);
    FOnQuestManagerReady OnQuestManagerReady;

private:
    // 모든 페이지 로드가 끝났을 때 실제 적용하는 내부 함수
    void FinalizeLoad();

private:
    // 서버에서 로드하는 중 누적할 임시 배열
    TArray<FQuestSaveData> PendingQuestData;

    // 전체 퀘스트 개수 (PrepareForLoad 호출 시 설정)
    int32 ExpectedQuestCount = 0;

private:
    UPROPERTY()
    class AGASWorldPlayerState* CachedPlayerState;

    UPROPERTY()
    class AGASWorldPlayerController* CachedPlayerController;

    void CachedOwnerReferences();

    // 서버가 초기화 완료 후 true; ReplicatedUsing 으로 클라에 알림
    UPROPERTY(ReplicatedUsing = OnRep_Ready)
    bool bInitialized = false;

    // bInitialized 값이 클라이언트에 복제되었을 때 실행
    UFUNCTION()
    void OnRep_Ready();

public:
    // 모든 퀘스트 인스턴스를 담는 배열
    // 이거 서버에 저장해서 세이브로드 시스템 진행
    UPROPERTY(Replicated, ReplicatedUsing = OnRep_AllQuestArr)
    TArray<UQuestInstance*> AllQuestArr;


    UPROPERTY(Replicated)
    TArray<UQuestInstance*> AvailableArr;

    UPROPERTY(Replicated)
    TArray<UQuestInstance*> InProgressArr;

    UPROPERTY(Replicated)
    TArray<UQuestInstance*> CompletedArr;

private:

    // QuestType에 맞는 서브 QuestInstance 생성
    UQuestInstance* CreateQuestInstanceFromRow(const FQuestDataTableRow& Row);

    // 복제 후 Quest가 가진 배열도 변경해주기
    UFUNCTION(BlueprintCallable)
    void OnRep_AllQuestArr();


public:
    // 태그로 퀘스트 골라오는 함수
    UFUNCTION(BlueprintCallable, Category = "Quest")
    TArray<UQuestInstance*> GetQuestsByState(const FGameplayTag& StateTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    TArray<UQuestInstance*> GetAvailableQuestsWithStartNPC(const FGameplayTag& StartNPCTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    TArray<UQuestInstance*> GetInProgressQuestsWithEndNPC(const FGameplayTag& EndNPCTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    TArray<UQuestInstance*> GetInProgressQuestsWithBothNPC(const FGameplayTag& InTag) const;


    // UI 수락버튼 클릭시 퀘스트 수락하는 함수
    // UI에서 퀘스트 정보를 가져와서 해당 퀘스트 정보 변경
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void AcceptQuest(UQuestInstance* Q);

    UFUNCTION(Server, Reliable)
    void ServerAcceptQuest(UQuestInstance* Q);
    

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void CompleteQuest(UQuestInstance* Q);

    UFUNCTION(Server, Reliable)
    void ServerCompleteQuest(UQuestInstance* Q);


   /* UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnNPCQuestListUpdated();
    void Multicast_OnNPCQuestListUpdated_Implementation();*/

private:
    // (에디터 할당) DataTable 에셋
    UPROPERTY(EditAnywhere, Category = "Quest")
    UDataTable* QuestDataTable;

    // (에디터 할당) 런타임에 NewObject로 생성할 클래스
    UPROPERTY(EditAnywhere, Category = "Quest")
    TSubclassOf<UQuestInstance> QuestInstanceClass;

    UPROPERTY(EditAnywhere, Category = "Quest")
    TSubclassOf<UQuestInstance> InteractionQuestClass;

    UPROPERTY(EditAnywhere, Category = "Quest")
    TSubclassOf<UQuestInstance> CollectionQuestClass;

    UPROPERTY(EditAnywhere, Category = "Quest")
    TSubclassOf<UQuestInstance> KillCountQuestClass;


    /** 상태 구분용 태그 */
    FGameplayTag TagLocked;
    FGameplayTag TagAvailable;
    FGameplayTag TagInProgress;
    FGameplayTag TagCompleted;



protected:
    // 만약 SetQuestListWidgetInstance()를 호출하지 않는다면 화면의 위젯을 찾아 자동으로 연결
    void SetQuestListWidgetInstance(UQuestListWidget* InWidget);

public:
    void SetupQuestUI();

    // 미리 HUD를 통해 생성해서 Hidden해 놓은 위젯 인스턴스를 찾아서 사용하기
    void FindExistingQuestListWidget(
        const TArray<UQuestInstance*>& Available,
        const TArray<UQuestInstance*>& InProgress,
        const TArray<UQuestInstance*>& Completed
    );

    void SplitQuestsByState(
        const TArray<UQuestInstance*>& In,
        TArray<UQuestInstance*>& OutAvailable,
        TArray<UQuestInstance*>& OutInProgress,
        TArray<UQuestInstance*>& OutCompleted
    ) const;

private:
    // 화면에 이미 띄워져 있는 위젯 참조
    UPROPERTY()
    UQuestListWidget* QuestListWidgetInstance;

    // 이미 생성된 위젯을 가져오기
    UPROPERTY()
    class UNPCQuestListWidget* NPCQuestListWidgetInstance;

    // WBP_AvailableQuest 블루프린트 래퍼 클래스
    UPROPERTY()
    UAvailableQuestWidget* AvailableQuestWidget;

    // 화면에 미리 배치된 InProgress 위젯
    UPROPERTY()
    UInProgressQuestWidget* InProgressQuestWidget;

    // 화면에 미리 배치된 InProgress 위젯
    UPROPERTY()
    UCompletedQuestWidget* CompletedQuestWidget;

    UPROPERTY()
    class UQuestMainWidget* QuestMainWidget;


    // 뷰포트에 추가된 AvailableQuestWidget 인스턴스
    UPROPERTY()
    UAvailableQuestWidget* AvailableQuestWidgetInstance;

private:
    bool CheckAcceptQuest(UQuestInstance* Q, FGameplayTag InNPCTag, FGameplayTag StartNPCTag);
    bool CheckCompleteQuest(UQuestInstance* Q, FGameplayTag InNPCTag, FGameplayTag EndNPCTag);
    void CheckQuestData();
    void CheckQuestProgress(UQuestInstance* Q, FGameplayTag TargetTag, int32 DeltaCount);

private:
    // 퀘스트 로드 후 UI 지연 출력을 위한 타이머
    FTimerHandle TH_QuestListWidget;

public:
    // 디버그: FQuestSaveData 하나 직렬화 크기 측정
    void LogSingleQuestSaveDataSize();
};
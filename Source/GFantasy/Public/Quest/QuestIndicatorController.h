// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "QuestIndicatorController.generated.h"

// 검색한 액터(NPC)와 그 NPC가 주는 퀘스트를 구별하기 위해 map의 키를 쌍으로 넣기 위한 구조체
// 처음에 만든 map은 키가 actor뿐이라서 NPC가 주는 퀘스트 여러개를 구별하지 않고 혼동시킴
// 그래서 두 퀘스트중 하나를 체크하면 다른 하나가 인식이 되지 않았음
USTRUCT()
struct FActorQuestKey
{
    GENERATED_BODY()

public:
    // 어느 액터(NPC)와 어느 퀘스트 인스턴스인지 구별하기 위한 쌍
    UPROPERTY()
    class AActor* Actor = nullptr;

    UPROPERTY()
    class UQuestInstance* QuestInstance = nullptr;

    bool operator==(FActorQuestKey const& Other) const
    {
        return Actor == Other.Actor && QuestInstance == Other.QuestInstance;
    }
};


FORCEINLINE uint32 GetTypeHash(FActorQuestKey const& Key)
{
    // Actor와 QuestInstance를 합친 해시
    return HashCombine(GetTypeHash(Key.Actor), GetTypeHash(Key.QuestInstance));
}


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GFANTASY_API UQuestIndicatorController : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestIndicatorController();

public:
    // BeginPlay에서 기존 액터 스캔 및 델리게이트 연결
    virtual void BeginPlay() override;

	// 매 프레임마다 등록된 액터들의 위치를 계산해 UI 업데이트
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // QuestWidget에서 체크 토글 시 호출될 함수
    UFUNCTION()
    void HandleTraceToggle(UQuestInstance* QuestInstance, bool bOn);

    bool DoesActorMatchQuest(AActor* Actor, UQuestInstance* QuestInstance, const FGameplayTag& StateTag) const;

    // 상태 토글 공통 로직 분리
    UFUNCTION()
    void UpdateIndicators(UQuestInstance* QuestInstance, bool bOn);

    // QuestInstance 상태가 바뀌면 호출
    UFUNCTION()
    void OnQuestStateChanged(UQuestInstance* QuestInstance, FGameplayTag OldState);


protected:
	// UI 위젯 생성 함수 (CheckOn시 퀘슽 정보를 받아서 월드의 액터를 스캔후 퀘스트 상태에 따라 생성) 
	void RegisterTargetActor(AActor* Actor, UQuestInstance* QuestInstance);

	// 인디케이터 UI 제거 함수(CheckOff시 특정 액터를 목록에서 제거, UI도 제거)
	void RemoveTargetActor(AActor* Actor, UQuestInstance* QuestInstance);

private:

    // 액터당 생성된 위젯 매핑
 /*   UPROPERTY()
    TMap<AActor*, class UQuestIndicatorWidget*> ActorWidgetMap;*/

    UPROPERTY()
    TMap<FActorQuestKey, class UQuestIndicatorWidget*> ActorQuestWidgetMap;

    // 위젯 클래스를 에디터에서 지정할 수 있도록
    UPROPERTY(EditAnywhere, Category = "Indicator")
    TSubclassOf<class UQuestIndicatorWidget> IndicatorWidgetClass;

    // 화면 밖 좌표를 Clamp시킬 때 사용할 오프셋
    UPROPERTY(EditAnywhere, Category = "Indicator")
    float EdgeOffset = 3.f;

    // Tick에서 화면 좌표 계산, 위젯 생성을 위해 필요한 로컬 PlayerController
    UPROPERTY()
    class AGASWorldPlayerController* GASControllerRef;

    // 체크 On 된 QuestInstance 들
    // 상태변경이 되어도 유지되도록 저장
    UPROPERTY()
    TSet<UQuestInstance*> TrackedQuests;

    bool bIsEndNPC;
    bool bIsStartNPC;
    bool bIsMoster;
};

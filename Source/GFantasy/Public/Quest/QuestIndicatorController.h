// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "QuestIndicatorController.generated.h"

// �˻��� ����(NPC)�� �� NPC�� �ִ� ����Ʈ�� �����ϱ� ���� map�� Ű�� ������ �ֱ� ���� ����ü
// ó���� ���� map�� Ű�� actor���̶� NPC�� �ִ� ����Ʈ �������� �������� �ʰ� ȥ����Ŵ
// �׷��� �� ����Ʈ�� �ϳ��� üũ�ϸ� �ٸ� �ϳ��� �ν��� ���� �ʾ���
USTRUCT()
struct FActorQuestKey
{
    GENERATED_BODY()

public:
    // ��� ����(NPC)�� ��� ����Ʈ �ν��Ͻ����� �����ϱ� ���� ��
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
    // Actor�� QuestInstance�� ��ģ �ؽ�
    return HashCombine(GetTypeHash(Key.Actor), GetTypeHash(Key.QuestInstance));
}


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GFANTASY_API UQuestIndicatorController : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestIndicatorController();

public:
    // BeginPlay���� ���� ���� ��ĵ �� ��������Ʈ ����
    virtual void BeginPlay() override;

	// �� �����Ӹ��� ��ϵ� ���͵��� ��ġ�� ����� UI ������Ʈ
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // QuestWidget���� üũ ��� �� ȣ��� �Լ�
    UFUNCTION()
    void HandleTraceToggle(UQuestInstance* QuestInstance, bool bOn);

    bool DoesActorMatchQuest(AActor* Actor, UQuestInstance* QuestInstance, const FGameplayTag& StateTag) const;

    // ���� ��� ���� ���� �и�
    UFUNCTION()
    void UpdateIndicators(UQuestInstance* QuestInstance, bool bOn);

    // QuestInstance ���°� �ٲ�� ȣ��
    UFUNCTION()
    void OnQuestStateChanged(UQuestInstance* QuestInstance, FGameplayTag OldState);


protected:
	// UI ���� ���� �Լ� (CheckOn�� ���� ������ �޾Ƽ� ������ ���͸� ��ĵ�� ����Ʈ ���¿� ���� ����) 
	void RegisterTargetActor(AActor* Actor, UQuestInstance* QuestInstance);

	// �ε������� UI ���� �Լ�(CheckOff�� Ư�� ���͸� ��Ͽ��� ����, UI�� ����)
	void RemoveTargetActor(AActor* Actor, UQuestInstance* QuestInstance);

private:

    // ���ʹ� ������ ���� ����
 /*   UPROPERTY()
    TMap<AActor*, class UQuestIndicatorWidget*> ActorWidgetMap;*/

    UPROPERTY()
    TMap<FActorQuestKey, class UQuestIndicatorWidget*> ActorQuestWidgetMap;

    // ���� Ŭ������ �����Ϳ��� ������ �� �ֵ���
    UPROPERTY(EditAnywhere, Category = "Indicator")
    TSubclassOf<class UQuestIndicatorWidget> IndicatorWidgetClass;

    // ȭ�� �� ��ǥ�� Clamp��ų �� ����� ������
    UPROPERTY(EditAnywhere, Category = "Indicator")
    float EdgeOffset = 3.f;

    // Tick���� ȭ�� ��ǥ ���, ���� ������ ���� �ʿ��� ���� PlayerController
    UPROPERTY()
    class AGASWorldPlayerController* GASControllerRef;

    // üũ On �� QuestInstance ��
    // ���º����� �Ǿ �����ǵ��� ����
    UPROPERTY()
    TSet<UQuestInstance*> TrackedQuests;

    bool bIsEndNPC;
    bool bIsStartNPC;
    bool bIsMoster;
};

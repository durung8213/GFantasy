
#include "Quest/QuestIndicatorController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Character/GASPlayerCharacter.h"
#include "Controller/GASWorldPlayerController.h"
#include "UI/Quest/QuestIndicatorWidget.h"
#include "UI/Quest/QuestWidget.h"
#include "Quest/QuestIndicatorTarget.h"
#include "Quest/QuestInstance.h"
#include "Quest/QuestManagerComponent.h"
#include "Character/GASQuestNPC.h"
#include "Actor/MonsterAreaIndicator/IndicatorTargetActor.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "GameFramework/Actor.h"


UQuestIndicatorController::UQuestIndicatorController()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UQuestIndicatorController::BeginPlay()
{
    Super::BeginPlay();
    
    // Manager가 붙은 액터가 캐릭터인지 확인
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar)
    {
        // 캐릭터가 아니면 작동 안 함 (혹은 다른 로직)
        SetComponentTickEnabled(false);
        return;
    }

    // 멀티플레이 환경에서 "내가 로컬컨트롤러인가?" 확인 (dedicated 서버나 다른클라에서는 UI 필요X)
    if (!OwnerChar->IsLocallyControlled())
    {
        // 로컬이 아니면 UI 필요 없으니 Tick 비활성화
        SetComponentTickEnabled(false);
        return;
    }

    // 로컬플레이어 컨트롤러 참조 획득
    APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());

    GASControllerRef = Cast<AGASWorldPlayerController>(PC);

    APlayerState* PS = GASControllerRef->PlayerState;
    if (PS)
    {
        UQuestManagerComponent* QMC = PS->FindComponentByClass<UQuestManagerComponent>();
        if (QMC)
        {
            // QuestInstance 들의 상태 변경 델리게이트 바인딩
            for (UQuestInstance* QI : QMC->AllQuestArr)
            {
                if (QI)
                {
                    QI->OnQuestStateChanged.AddDynamic(this, &UQuestIndicatorController::OnQuestStateChanged);
                }
            }
        }
    }
}

void UQuestIndicatorController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 로컬 플레이어 컨트롤러가 없으면 로직 수행 불가
    if (!GASControllerRef) return;

    // 화면 크기를 구함
    int32 ViewportX, ViewportY;
    GASControllerRef->GetViewportSize(ViewportX, ViewportY);

    // Manager의 소유자(로컬 캐릭터) 위치
    FVector OwnerLocation = GetOwner()->GetActorLocation();

    // 등록된 모든 액터를 순회
    for (auto& Pair : ActorQuestWidgetMap)
    {
        const FActorQuestKey& Key = Pair.Key;
        AActor* TargetActor = Key.Actor;                 // 추적 대상 액터
        UQuestIndicatorWidget* Widget = Pair.Value;    // 그 액터에 대응하는 위젯

        if (!TargetActor || !Widget) continue;

        // 액터 월드 위치 -> 스크린 좌표
        FVector WorldPos = TargetActor->GetActorLocation() + FVector(0.f, 0.f, 200.f);
        FVector2D ScreenPos;

        bool bProjected = GASControllerRef->ProjectWorldLocationToScreen(WorldPos, ScreenPos);

        bool bOffScreen = false;
        // 화면 범위를 벗어났는지 검사
        if (ScreenPos.X < 0.f || ScreenPos.X > ViewportX ||
            ScreenPos.Y < 0.f || ScreenPos.Y > ViewportY)
        {
            bOffScreen = true;
            // 화면 밖이면 가장자리로 Clamp (화면 가장자리 추적)
            ScreenPos.X = FMath::Clamp(ScreenPos.X, 0.f, (float)ViewportX);
            ScreenPos.Y = FMath::Clamp(ScreenPos.Y, 0.f, (float)ViewportY);
        }

        float Distance = FVector::Dist(OwnerLocation, WorldPos);
        float ScaleFactor = FMath::GetMappedRangeValueClamped(
            FVector2D(0.f, 30000.f),   // 거리 범위
            FVector2D(1.f, 0.4f),     // 스케일 범위 (가까우면 1.0, 멀면 0.1)
            Distance
        );
        Widget->SetRenderScale(FVector2D(ScaleFactor, ScaleFactor));

        // 위젯에 데이터 전달 (on/off screen 여부, 거리 등)
        Widget->UpdateIndicatorData(ScreenPos, Distance / 100, bOffScreen);
    }
}

void UQuestIndicatorController::HandleTraceToggle(UQuestInstance* QuestInstance, bool bOn)
{
    if (!QuestInstance)
    {
        return;
    }

    QuestInstance->bIsTraced = bOn;

    // 트래킹 세트에도 반영
    if (bOn)
    {
        TrackedQuests.Add(QuestInstance);
    }
    else
    {
        TrackedQuests.Remove(QuestInstance);
    }

    // 월드 순회
    UWorld* World = GetWorld();
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* TargetActor = *It;
        if (!TargetActor)
        {
            continue;
        }

        // 반드시 TargetComponent가 있어야 처리
        if (!TargetActor->FindComponentByClass<UQuestIndicatorTarget>())
        {
            continue;
        }

        bool bMatches = DoesActorMatchQuest(TargetActor, QuestInstance, QuestInstance->QuestStateTag);
        
        //UE_LOG(LogTemp, Log, TEXT("QuestIndicator : actor: %s"), *TargetActor->GetName());

        // 매칭된 액터에 대해서만 Register/Remove
        if (bMatches)
        {
            if (bOn)
            {
                RegisterTargetActor(TargetActor, QuestInstance);
                UE_LOG(LogTemp, Log, TEXT("QuestIndicator: Registered %s"), *TargetActor->GetName());
            }
            else
            {
                RemoveTargetActor(TargetActor, QuestInstance);
                UE_LOG(LogTemp, Log, TEXT("QuestIndicator: Removed %s"), *TargetActor->GetName());
            }
        }
    }

}

bool UQuestIndicatorController::DoesActorMatchQuest(AActor* Actor, UQuestInstance* QuestInstance, const FGameplayTag& StateTag) const
{
    //const FGameplayTag StateTag = QuestInstance->QuestStateTag;
    const FGameplayTag AvailableTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Available"));
    const FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"));
    
    if (StateTag.MatchesTagExact(AvailableTag))
    {
        // Available → QuestStartNPCTag 에 맞는 NPC
        AGASQuestBaseNPC* NPC = Cast<AGASQuestBaseNPC>(Actor);
        if (NPC)
        {
            return NPC->GetNPCNameTag().MatchesTagExact(QuestInstance->QuestStartNPCTag);
        }
    }
    else if (StateTag.MatchesTagExact(InProgressTag))
    {
        // InProgress
        if (QuestInstance->QuestType == EQuestType::KillCount)
        {
            // KillCountQuest
            if (!QuestInstance->bCanComplete)
            {
                // 진행 중 완료 불가 : TargetLocationTag 기준
                AIndicatorTargetActor* TgtActor = Cast<AIndicatorTargetActor>(Actor);
                if (TgtActor)
                {
                    return TgtActor->GetTargetLocationTag().MatchesTagExact(QuestInstance->TargetLocationTag);
                }
            }
            // 진행 중 완료 가능: QuestEndNPCTag 기준
            else
            {
                AGASQuestBaseNPC* NPC2 = Cast<AGASQuestBaseNPC>(Actor);
                if (NPC2)
                {
                    return NPC2->GetNPCNameTag().MatchesTagExact(QuestInstance->QuestEndNPCTag);
                }
            }
        }
        else
        {
            // 기타 퀘스트 타입: QuestEndNPCTag 기준
            AGASQuestBaseNPC* NPC3 = Cast<AGASQuestBaseNPC>(Actor);
            if (NPC3)
            {
                return NPC3->GetNPCNameTag().MatchesTagExact(QuestInstance->QuestEndNPCTag);
            }
        }
    }

    return false;
}

void UQuestIndicatorController::UpdateIndicators(UQuestInstance* QuestInstance, bool bOn)
{
    // 상태 태그 편의 변수
    const FGameplayTag State = QuestInstance->QuestStateTag;
    static const FGameplayTag AvailableTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Available"), false);
    static const FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);

    // 월드 순회
    UWorld* World = GetWorld();
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* TargetActor = *It;
        if (!TargetActor)
        {
            continue;
        }

        // 1) 반드시 TargetComponent가 있어야 처리
        if (!TargetActor->FindComponentByClass<UQuestIndicatorTarget>())
        {
            continue;
        }

        UE_LOG(LogTemp, Log, TEXT("QuestIndicator : actor: %s"), *TargetActor->GetName());

        bool bMatches = false;

        if (State.MatchesTagExact(AvailableTag))
        {
            // Available 상태일때 QuestStartNPCTag에게 인디케이터 부착
            if (AGASQuestBaseNPC* NPC = Cast<AGASQuestBaseNPC>(TargetActor))
            {
                bMatches = NPC->GetNPCNameTag().MatchesTagExact(QuestInstance->QuestStartNPCTag);
            }
        }
        else if (State.MatchesTagExact(InProgressTag))
        {
            // InProgress 상태일때
            if (QuestInstance->QuestType == EQuestType::KillCount)
            {
                // KillCountQuest 타입
                if (!QuestInstance->bCanComplete)
                {
                    // 진행 중 일때 : TargetLocationTag (TargetTag)
                    if (AIndicatorTargetActor* Target = Cast<AIndicatorTargetActor>(TargetActor))
                    {
                        bMatches = Target->GetTargetLocationTag().MatchesTagExact(QuestInstance->TargetLocationTag);
                    }
                }
                else
                {
                    // 완료 가능 : QuestEndNPCTag 기준
                    if (AGASQuestBaseNPC* NPC = Cast<AGASQuestBaseNPC>(TargetActor))
                    {
                        bMatches = NPC->GetNPCNameTag().MatchesTagExact(QuestInstance->QuestEndNPCTag);
                    }
                }
            }
            else
            {
                // 다른 타입들 : QuestEndNPCTag 기준
                if (AGASQuestBaseNPC* NPC = Cast<AGASQuestBaseNPC>(TargetActor))
                {
                    bMatches = NPC->GetNPCNameTag().MatchesTagExact(QuestInstance->QuestEndNPCTag);
                }
            }
        }

        if (bMatches)
        {
            // 최종 매칭 여부에 따라 등록/해제
            if (bOn)
            {
                RegisterTargetActor(TargetActor, QuestInstance);
                UE_LOG(LogTemp, Log, TEXT("QuestIndicator : Register Actor - %s"), *TargetActor->GetName());
            }
            else
            {
                RemoveTargetActor(TargetActor, QuestInstance);
                UE_LOG(LogTemp, Log, TEXT("QuestIndicator : Remove Actor"));
            }
        }
        else
        {
            // 체크 해제 시, 이전에 등록된 액터라면 반드시 제거
            if (!bOn)
            {
                RemoveTargetActor(TargetActor, QuestInstance);
                UE_LOG(LogTemp, Log, TEXT("QuestIndicator : Remove Actor"));
            }
        }
    }
}

void UQuestIndicatorController::OnQuestStateChanged(UQuestInstance* QuestInstance, FGameplayTag OldState)
{
    // 퀘스트가 Trace On 상태라면
    if (TrackedQuests.Contains(QuestInstance))
    {
        // 이전 상태 기준으로 등록됐던 키(Actor, QuestInstance) 만 골라 제거
        TArray<FActorQuestKey> KeysToRemove;

        for (auto& Pair : ActorQuestWidgetMap)
        {
            //// 이 액터가 “이전 상태”에 부합했으면
            //const FActorQuestKey& Key = Pair.Key;
            //if (Key.QuestInstance == QuestInstance
            //    && DoesActorMatchQuest(Key.Actor, QuestInstance, OldState))
            //{
            //    KeysToRemove.Add(Key);
            //}
            if (Pair.Key.QuestInstance == QuestInstance)
            {
                KeysToRemove.Add(Pair.Key);
            }
        }
        // 실제 제거
        for (const FActorQuestKey& Key : KeysToRemove)
        {
            RemoveTargetActor(Key.Actor, Key.QuestInstance);
        }


        // 새 상태 기준으로 등록
        UWorld* World = GetWorld();
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            AActor* Actor = *It;
            if (!Actor->FindComponentByClass<UQuestIndicatorTarget>())
            {
                continue;
            }

            if (DoesActorMatchQuest(Actor, QuestInstance, QuestInstance->QuestStateTag))
            {
                RegisterTargetActor(Actor, QuestInstance);
            }
        }
    }




}

void UQuestIndicatorController::RegisterTargetActor(AActor* Actor, UQuestInstance* QuestInstance)
{
    if (!Actor)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestIndicator : RegisterTargetActor: Invalid Actor"));
        return;
    }

    // 이미 등록됐는지, Actor+QuestInstance 쌍으로 확인
    FActorQuestKey Key{ Actor, QuestInstance };
    if (ActorQuestWidgetMap.Contains(Key))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestIndicator : RegisterTargetActor: Already registered: %s"), *Actor->GetName());
        return;
    }
    if (!IndicatorWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestIndicator : RegisterTargetActor: IndicatorWidgetClass is null"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("QuestIndicator : Creating widget for %s"), *Actor->GetName());

    // 인디케이터 위젯 생성
    UQuestIndicatorWidget* Widget = CreateWidget<UQuestIndicatorWidget>(GetWorld(), IndicatorWidgetClass);
    
    if (Widget)
    {
        Widget->OwningQuestInstance = QuestInstance;
        Widget->AddToViewport();
        Widget->InitQuestIndicatorWidget(Actor, QuestInstance);
        ActorQuestWidgetMap.Add(Key, Widget);
    }
}

void UQuestIndicatorController::RemoveTargetActor(AActor* Actor, UQuestInstance* QuestInstance)
{
    // Actor+QuestInstance 키 생성
    FActorQuestKey Key{ Actor, QuestInstance };

    if (UQuestIndicatorWidget** WidgetPtr = ActorQuestWidgetMap.Find(Key))
    {
        if (UQuestIndicatorWidget* Widget = *WidgetPtr)
        {
            if (Widget)
            {
                Widget->RemoveFromParent();
                ActorQuestWidgetMap.Remove(Key);
            }
        }
    }
}


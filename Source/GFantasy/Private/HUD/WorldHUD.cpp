// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WorldHUD.h"
#include "UI/Quest/QuestListWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NPC/NPCInteractionWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "UI/Player/PlayerHUD.h"
#include "UI/LogInWidget.h"
#include "Quest/QuestInstance.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Quest/QuestManagerComponent.h"
#include "UI/Enemy/Boss/BossStateUI.h"
#include "UI/Quest/QuestMainWidget.h"
#include "SaveGame/GFSaveGame.h"        // UGFSaveGame 정의 포함
#include "Kismet/GameplayStatics.h"    // SaveGame 함수들
#include "UI/Enemy/Boss/BossExtraGaugeUserWidget.h"
#include "UI/Enemy/Boss/WerewolfPattern1UI.h"

AWorldHUD::AWorldHUD()
{
}

void AWorldHUD::BeginPlay()
{
	Super::BeginPlay();

    APlayerController* OwningPC = GetOwningPlayerController();

    //APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

    // 저장 슬롯 이름과 유저 인덱스 지정
    const FString SlotName = TEXT("PlayerSaveSlot");
    const int32 UserIndex = 0;

    // 1) 저장 데이터가 있는지 검사
    const bool bHasSave = UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);


    // 2) 로그인 위젯은 저장이 없을 때만 띄우기
    if (!bHasSave && LogInWidgetClass)
    {
        if (OwningPC)
        {
            // UI 전용 입력 모드로 전환
            FInputModeUIOnly InputMode;
            OwningPC->SetInputMode(InputMode);
            OwningPC->bShowMouseCursor = true;

            // 로그인 위젯 생성 및 뷰포트에 추가
            LogInWidgetInstance = CreateWidget<ULogInWidget>(OwningPC, LogInWidgetClass);
            if (LogInWidgetInstance)
            {
                LogInWidgetInstance->AddToViewport();
            }
        }
    }
    else
    {
        // 저장이 있을 경우, 저장된 닉네임 불러와서 서버에 전달 (예시)
        USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
        if (UGFSaveGame* SG = Cast<UGFSaveGame>(Loaded))
        {
            APlayerController* PC = GetOwningPlayerController();
            if (PC)
            {
                // 서버 RPC로 닉네임 설정 (이 함수는 PC에 구현되어 있어야 합니다)
                // 예: Cast<AGASWorldPlayerController>(PC)->ServerSetNickname(SG->SavedNickName);
            }
        }
    }

    //if (LogInWidgetClass)
    //{
    //    // LogInWidgetInstance = CreateWidget<ULogInWidget>(PC, LogInWidgetClass);
    //    LogInWidgetInstance = CreateWidget<ULogInWidget>(OwningPC, LogInWidgetClass);
    //    LogInWidgetInstance->AddToViewport();
    //}

    if (PlayerHUDClass)
    {
        //PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
        PlayerHUD = CreateWidget<UPlayerHUD>(OwningPC, PlayerHUDClass);
        if (PlayerHUD)
        {
            PlayerHUD->AddToViewport();
            QuestListWidget = PlayerHUD->WBP_QuestList;
            QuestMainWidget = PlayerHUD->WBP_QuestMain;
        }
    }
}

void AWorldHUD::ShowNPCInteractionWidget(FGameplayTag InTag, const FText& InNameText, ENPCType InType)
{
    UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : In WorldHUD"));

    if (!NPCInteractionWidgetClass) return;
    UE_LOG(LogTemp, Log, TEXT("NonCombatNPC : NPCNameTag = %s, NPCName = %s"), *InTag.ToString(), *InNameText.ToString());


    // 아직 위젯을 만들지 않았다면
    if (!NPCInteractionWidget)
    {
        UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : WorldHUD - NPCInteractionWidget"));

        if (APlayerController* PC = Cast<APlayerController>(PlayerOwner))
        {
            // NPC 상호작용 위젯 생성
            NPCInteractionWidget = CreateWidget<UNPCInteractionWidget>(PC, NPCInteractionWidgetClass);
            if (NPCInteractionWidget)
            {
                UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : WorldHUD - Add NPCInteractionWidget"));
                // NPC Tag 정보 전달
                NPCInteractionWidget->InitializeWithNPCTag(InTag, InNameText, InType);
                NPCInteractionWidget->AddToViewport();

                // 마우스 보이기 + UI 전용 입력 모드
                PC->bShowMouseCursor = true;
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(NPCInteractionWidget->TakeWidget());
                PC->SetInputMode(InputMode);
            }
        }
    }
    // 이미 생성돼 있으면 그냥 보이기
    else
    {
        UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : WorldHUD - Already NPCInteractionWidget"));
        NPCInteractionWidget->InitializeWithNPCTag(InTag, InNameText, InType);
        NPCInteractionWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void AWorldHUD::ShowQuestListWidget(const TArray<UQuestInstance*>& Available, const TArray<UQuestInstance*>& InProgress, const TArray<UQuestInstance*>& Completed)
{
    if (QuestListWidget)
    {
        ShowAvailableQuestWidget(Available);
        ShowInProgressQuestWidget(InProgress);
        ShowCompletedQuestWidget(Completed);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Quest List : WorldHUD - No Quest List Widget"))
    }
}

void AWorldHUD::ShowAvailableQuestWidget(const TArray<UQuestInstance*>& Available)
{
    QuestListWidget->ShowAvailableTab(Available);
}

void AWorldHUD::ShowInProgressQuestWidget(const TArray<UQuestInstance*>& InProgress)
{
    QuestListWidget->ShowInProgressTab(InProgress);
    QuestMainWidget->ShowInProgressQuestList(InProgress);
}

void AWorldHUD::ShowCompletedQuestWidget(const TArray<UQuestInstance*>& Completed)
{
    QuestListWidget->ShowCompletedTab(Completed);
}

void AWorldHUD::HandleQuestListReady(UQuestListWidget* QuestList)
{
    if (QuestList)
    {
        APlayerController* PC = GetOwningPlayerController();
        if (PC)
        {
            if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
            {
                if (UQuestManagerComponent* QMC = PS->FindComponentByClass<UQuestManagerComponent>())
                {
                    QMC->FindExistingQuestListWidget(QMC->AvailableArr,QMC->InProgressArr,QMC->CompletedArr);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("WorldHUD : No QuestManagerComponent"));
                }
            }
        }
    }
}

bool AWorldHUD::IsNPCUIOpen() const
{
    return (NPCInteractionWidget && NPCInteractionWidget->IsInViewport() &&
            NPCInteractionWidget->GetVisibility() == ESlateVisibility::Visible);
}

/* 김민형 제작 부분 */

void AWorldHUD::ShowBossHpBar(AGASCombatBossNPC* BossCharacter)
{
    BossStateUI = CreateWidget<UBossStateUI>(GetOwningPlayerController(), BossStateUIClass);
    BossStateUI->AddToViewport();
    //BossStateUI->BindBossState(BossCharacter->GetAbilitySystemComponent(), BossCharacter->GetCharacterAttributeSet(), BossCharacter->GetGroggyAttributeSet(), BossCharacter->MonsterName);
}

void AWorldHUD::SetBossName(FText NewBossName)
{
    if (BossStateUI)
    {
        BossStateUI->SetBossName(NewBossName);
    }
    else
    {
        FTimerHandle BossNameTimer;
        GetWorld()->GetTimerManager().SetTimer(
        BossNameTimer,
        [this, NewBossName]
        {
            SetBossName(NewBossName);
        },
        0.3f,
        false
        );
    }
}

void AWorldHUD::SetBossUIHealth(float NewHealth)
{
    if (!BossStateUI) return;
    BossStateUI->SetBossHealth2(NewHealth);
}


void AWorldHUD::SetBossUIMaxHealth(float NewMaxHealth)
{
    if (!BossStateUI) return;
    BossStateUI->SetBossMaxHealth2(NewMaxHealth);
}

void AWorldHUD::SetBossUIGroggy(float NewGroggy)
{
    if (!BossStateUI) return;
    BossStateUI->SetBossGroggy2(NewGroggy);
}

void AWorldHUD::SetBossUIMaxGroggy(float NewMaxGroggy)
{
    if (!BossStateUI) return;
    BossStateUI->SetBossMaxGroggy2(NewMaxGroggy);
}

void AWorldHUD::HideBossHpBar()
{
    if (!BossStateUI) return;
	
    BossStateUI->RemoveFromParent();
    BossStateUI = nullptr;
}

void AWorldHUD::ShowBossExtraGaugeUI()
{
    if (BossExtraGaugeUIClass)
    {
        BossExtraGaugeUI = CreateWidget<UBossExtraGaugeUserWidget>(GetOwningPlayerController(), BossExtraGaugeUIClass);
        BossExtraGaugeUI->AddToViewport();
    }
   
}

void AWorldHUD::ExtraGaugeUISpacebarClick()
{
    if (!BossExtraGaugeUI) return;
    BossExtraGaugeUI->SpacebarClick();
}

void AWorldHUD::SetExtraGaugeUIExtraGauge(float NewExtraGauge)
{
    if (!BossExtraGaugeUI) return;
    BossExtraGaugeUI->SetExtraGaugeValue(NewExtraGauge);
}

void AWorldHUD::SetExtraGaugeUIMaxExtraGauge(float NewMaxExtraGauge)
{
    if (!BossExtraGaugeUI) return;
    BossExtraGaugeUI->SetMaxExtraGaugeValue(NewMaxExtraGauge);
}

void AWorldHUD::HideBossExtraGaugeUI()
{
    if (!BossExtraGaugeUI) return;
	
    BossExtraGaugeUI->RemoveFromParent();
    BossExtraGaugeUI = nullptr;
}

void AWorldHUD::ShowWerewolfPattern1Progressbar()
{
    if (WerewolfPattern1UIClass)
    {
        WerewolfPattern1UI = CreateWidget<UWerewolfPattern1UI>(GetOwningPlayerController(), WerewolfPattern1UIClass);
        WerewolfPattern1UI->AddToViewport();
    }
}

void AWorldHUD::WerewolfPattern1SetProgressbarPercent(float NewPercent)
{
    if (!WerewolfPattern1UI) return;
    WerewolfPattern1UI->SetFrontPercent(NewPercent);
}

void AWorldHUD::HideWerewolfPattern1Progressbar()
{
    if (!WerewolfPattern1UI) return;

    WerewolfPattern1UI->RemoveFromParent();
    WerewolfPattern1UI = nullptr;
}

/* 김민형 제작 부분 끝 */
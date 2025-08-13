// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPC/NPCInteractionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HUD/WorldHUD.h"
#include "Controller/GASWorldPlayerController.h"
#include "UI/NPC/NPCQuestListWidget.h"
#include "GFantasy/GFantasy.h"

void UNPCInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

    // 해당 버튼 클릭시 일어날 이벤트
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UNPCInteractionWidget::OnExitButtonClicked);
	}


    if (InteractButton)
    {
        InteractButton->OnClicked.AddDynamic(this, &UNPCInteractionWidget::OnInteractButtonClicked);
    }
}

void UNPCInteractionWidget::InitializeWithNPCTag(FGameplayTag InNPCTag, const FText& InNameText, ENPCType InType)
{
    NPCInteractionTag = InNPCTag;
    NPCInteractionName = InNameText;
    NPCInteractionType = InType;

    SetNPCNameText(NPCInteractionName);
    UE_LOG(LogTemp, Log, TEXT("NonCombatNPC : NPCNameTag = %s, NPCName = %s"), *InNPCTag.ToString(), *InNameText.ToString());

    // 위젯에 뜰 텍스트
    switch (NPCInteractionType)
    {
    case ENPCType::Crafter:
        if (InteractText)
        {
            InteractText->SetText(FText::FromString(TEXT("제작하기")));
        }
        break;
        
    case ENPCType::Enhancer:
        if (InteractText)
        {
            InteractText->SetText(FText::FromString(TEXT("강화하기")));
        }
        break;
        
    case ENPCType::Synthesizer:
        if (InteractText)
        {
            InteractText->SetText(FText::FromString(TEXT("합성하기")));
        }
        break;

    case ENPCType::Merchant:
        if (InteractText)
        {
            InteractText->SetText(FText::FromString(TEXT("거래하기")));
        }
        break;

    case ENPCType::Normal:
        if (InteractText)
        {
            InteractText->SetText(FText::FromString(TEXT("대화하기")));
        }
        break;

    case ENPCType::Quest:
        if (InteractText)
        {
            InteractText->SetText(FText::FromString(TEXT("퀘스트 받기")));
        }
        break;

    default:
        // 기타 타입(예비)인 경우
        if (InteractText)
        {
            InteractText->SetText(FText::FromString(TEXT("상호작용")));
        }
        break;
    }
}

void UNPCInteractionWidget::OnExitButtonClicked()
{
    // 1. 위젯 제거
    RemoveFromParent();
    // 2. HUD 인스턴스 포인터 초기화
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (AWorldHUD* HUD = Cast<AWorldHUD>(PC->GetHUD()))
        {
            HUD->NPCInteractionWidget = nullptr;
        }
    }
    // 3. 플레이어 컨트롤러 가져오기
    if (APlayerController* PC = GetOwningPlayer())
    {
        //// 4. 마우스 커서 숨기기
        //PC->bShowMouseCursor = false;
        //// 5. 입력 모드를 게임 전용으로 전환
        //FInputModeGameOnly GameInputMode;
        //PC->SetInputMode(GameInputMode);
        AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
        MyPC->SelectInputModeWhenUIOnly();
    }
}

void UNPCInteractionWidget::OnInteractButtonClicked()
{
    switch (NPCInteractionType)
    {
    // 대장장이
    case ENPCType::Crafter:
        ShowCraftWidget();
        break;

    case ENPCType::Enhancer:
        ShowEnhanceWidget();
        break;

    case ENPCType::Synthesizer:
        ShowSynthesisWidget();
        break;

    // 상점
    case ENPCType::Merchant:
        ShowMerchantWidget();
        break;

    // 일반
    case ENPCType::Normal:
        ShowNormalNPCWidget();
        break;

    // 퀘스트
    case ENPCType::Quest:
        ShowQuestWidget();
        break;

    default:
       break;
    }
}

// 퀘스트 NPC 위젯
void UNPCInteractionWidget::ShowQuestWidget()
{
    // 상호작용 UI 제거
    RemoveFromParent();
    UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : NPCInteractionWidget - QuestButtonClicked"));

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (AWorldHUD* HUD = Cast<AWorldHUD>(PC->GetHUD()))
        {
            HUD->NPCInteractionWidget = nullptr;
        }

        // 퀘스트 관련 UI 출력
        if (NPCQuestListWidgetClass)
        {
            UNPCQuestListWidget* QuestListWidget = CreateWidget<UNPCQuestListWidget>(PC, NPCQuestListWidgetClass);
            UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : NPCInteractionWidget - CreateQuestListWidget"));

            if (QuestListWidget)
            {
                // 4) InteractionWidget이 저장한 NPCTag을 자식 위젯에 전달
                UE_LOG(LogTemp, Log, TEXT("NPCInteractionWidget : NPCNameTag = %s"), *NPCInteractionTag.ToString());

                QuestListWidget->InitializeWithNPCTag(GetNPCNameTag());
                QuestListWidget->AddToViewport();
            }
        }
    }
}

// 일반 NPC 대화 위젯
void UNPCInteractionWidget::ShowNormalNPCWidget_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("NPCInteractionWidget : ShowNormalNPCWidget"));
}

// 대장장이 위젯
// 제작
void UNPCInteractionWidget::ShowCraftWidget_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("NPCInteractionWidget : ShowCraftWidget"));
}

// 강화
void UNPCInteractionWidget::ShowEnhanceWidget_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("NPCInteractionWidget : ShowEnhanceWidget"));
}

// 합성
void UNPCInteractionWidget::ShowSynthesisWidget_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("NPCInteractionWidget : ShowSynthesisWidget"));
}

// 상점 위젯
void UNPCInteractionWidget::ShowMerchantWidget_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("NPCInteractionWidget : ShowMerchantWidget"));
}


FGameplayTag UNPCInteractionWidget::GetNPCNameTag() const
{
    return NPCInteractionTag;
}

void UNPCInteractionWidget::SetNPCNameText(/*FGameplayTag InNPCTag,*/ const FText& InNameText)
{
    if (NPCName)
    {
        NPCName->SetText(InNameText);
    }
}

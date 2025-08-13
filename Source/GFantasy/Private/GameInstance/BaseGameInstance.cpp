// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/BaseGameInstance.h"
//#include "OnlineSubsystem.h"
//#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SaveGame.h"
#include "SaveGame/GFSaveGame.h"
#include "Controller/GASWorldPlayerController.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Inventory/Crafting/CraftManagerComponent.h"
#include "Quest/QuestManagerComponent.h"
#include "Character/Customization/CustomizerComponent.h"

void UBaseGameInstance::SavePlayerData()
{
    // 1) SaveGame 객체 생성
    UGFSaveGame* SaveObj = Cast<UGFSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UGFSaveGame::StaticClass())
    );
    if (!SaveObj)
    {
        UE_LOG(LogTemp, Warning, TEXT("SavePlayerData: No SaveGame "));
        return;
    }

    // 2) 현재 상태 복사
    // 로컬 플레이어 컨트롤러 → PlayerState 가져오기
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(PC->PlayerState))
        {
            // 닉네임 저장
            SaveObj->PlayerData.Nickname = PS->GetPlayerNickname();

            // 퀘스트 진행상태 저장
            if (UQuestManagerComponent* QuestComp = PS->FindComponentByClass<UQuestManagerComponent>())
            {
                QuestComp->SaveQuestData(SaveObj->PlayerData.QuestData);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Save : No Quest"));
            }

            // 커스터 마이징 저장
            if (UCustomizerComponent* CusComp = PS->FindComponentByClass<UCustomizerComponent>())
            {
                CusComp->SaveCustomizerData(SaveObj->PlayerData.CustomizerSaveData);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Save : No Custom"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Save : No PS for Quest, Custom"));
        }

        if (AGASPlayerController* MyPC = Cast<AGASPlayerController>(PC))
        {
            if (UInventoryComponent* InvenComp = MyPC->FindComponentByClass<UInventoryComponent>())
            {
                // 인벤토리 저장
                InvenComp->SaveInventoryData(SaveObj->PlayerData.InventorySaveData);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Save : No Inven"));
            }

            if (UCraftManagerComponent* CraftComp = MyPC->FindComponentByClass<UCraftManagerComponent>())
            {
                // 제작법 저장
                CraftComp->SaveCraftData(SaveObj->PlayerData.CraftSaveData);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Save : No Craft"));
            }
 
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Save : No PC for Inventory, Craft"));
        }
    }

    // 3) 슬롯에 저장
    if (UGameplayStatics::SaveGameToSlot(SaveObj, SlotName, UserIndex))
    {
        UE_LOG(LogTemp, Log, TEXT("SavePlayerData: Save Success"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SavePlayerData: Save Fail"));
    }
}

FString UBaseGameInstance::GetSavedNickname() const
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
    {
        if (UGFSaveGame* SG = Cast<UGFSaveGame>(
            UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)))
        {
            // 구조체 방식: SG->PlayerData.Nickname
            // 혹은 플랫 방식: SG->PlayerNickname
            return SG->PlayerData.Nickname;
        }
    }

    return TEXT("NoSave");
}

TArray<FQuestSaveData> UBaseGameInstance::GetSavedQuests() const
{
    if (UGFSaveGame* SG = LoadSaveGame())
    {
        // PlayerData.QuestData는 TArray<FQuestSaveData>
        return SG->PlayerData.QuestData;
    }
    // 로드 실패 시 빈 배열 반환
    return TArray<FQuestSaveData>();
}

TArray<FInventorySaveData> UBaseGameInstance::GetSavedInventory() const
{
    if (UGFSaveGame* SG = LoadSaveGame())
    {
        return SG->PlayerData.InventorySaveData;
    }
    // 저장된 데이터가 없으면 빈 배열 반환
    return TArray<FInventorySaveData>();
}

TArray<FCustomizerSaveData> UBaseGameInstance::GetSavedCustomizer() const
{
    if (UGFSaveGame* SG = LoadSaveGame())
    {
        return SG->PlayerData.CustomizerSaveData;
    }
    // 저장된 데이터가 없으면 빈 배열 반환
    return TArray<FCustomizerSaveData>();
}

UGFSaveGame* UBaseGameInstance::LoadSaveGame() const
{
    // 슬롯 존재 여부 확인
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadSaveGame: No Slot (%s, %d)"),
            *SlotName, UserIndex);
        return nullptr;
    }

    // 로드 시도
    USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
    UGFSaveGame* SG = Cast<UGFSaveGame>(Loaded);
    if (!SG)
    {
        UE_LOG(LogTemp, Error, TEXT("LoadSaveGame: LoadGameFromSlot Failed"));
    }
    return SG;
}


void UBaseGameInstance::Init()
{
    Super::Init();

    //if (bUseSteamOSS) {
    //    GetSteamOSS();
    //}
}
//
//void UBaseGameInstance::GetSteamOSS()
//{
//    // 개발자가 스팀 OSS를 사용한다고 하면
//    IOnlineSubsystem* OSS = IOnlineSubsystem::Get(); // 스팀 OSS를 받아옴 (기본 설정이 스팀이기에 가능한 것
//    if (OSS) {
//        // OSS가 있으면
//        UKismetSystemLibrary::PrintString(this,
//            *OSS->GetSubsystemName().ToString(),
//            true,
//            true,
//            FLinearColor(0.0f, 0.66f, 1.0f),
//            2.f); // 지금 서브시스템이 무엇인지 출력
//
//        //UKismetSystemLibrary::PrintString(this, OSS->GetOnlineEnvironmentName(), true, true, FLinearColor(0.0f, 0.66f, 1.0f), 2.f);
//
//        UserOSS = OSS;  // 저장
//
//        IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();  // OSS에서 유저의 정보를 가져옴
//
//        if (Identity.IsValid())
//        {
//            // 정보가 있으면
//            TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0); // 로컬 사용자 index
//
//            if (Id.IsValid())
//            {
//                //ID가 있으면 출력
//                FString SteamIdStr = Id->ToString();
//                UKismetSystemLibrary::PrintString(this, SteamIdStr, true, true, FLinearColor(0.0f, 0.66f, 1.0f), 2.f);  // ID출력
//
//                FString DisplayName = Identity->GetPlayerNickname(*Id);
//                UKismetSystemLibrary::PrintString(this, DisplayName, true, true, FLinearColor(0.0f, 0.66f, 1.0f), 2.f); // 닉네임 출력
//            }
//            else {
//                UKismetSystemLibrary::PrintString(this, TEXT("Id is NotValid"), true, true, FLinearColor(0.0f, 0.66f, 1.0f), 2.f);
//            }
//        }
//        else {
//            UKismetSystemLibrary::PrintString(this, TEXT("Identity is NotValid"), true, true, FLinearColor(0.0f, 0.66f, 1.0f), 2.f);
//        }
//    }
//}
//
///// <summary>
///// ID가져오기
///// </summary>
///// <returns>스팀 ID FString</returns>
//FString UBaseGameInstance::GetUserID()
//{
//    // INIT에 있는 코드와 같음
//    IOnlineIdentityPtr Identity = UserOSS->GetIdentityInterface();
//    TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0); // 로컬 사용자 index
//
//    if (Id.IsValid())
//    {
//        UKismetSystemLibrary::PrintString(this, Id->ToString());
//        return Id->ToString();
//    }
//    else {
//        return FString();
//    }
//}
//
///// <summary>
///// 스팀 닉네임 가져오기
///// </summary>
///// <returns>닉네임 FString</returns>
//FString UBaseGameInstance::GetUserNickName()
//{
//    // INIT에 있는 코드와 같음
//    IOnlineIdentityPtr Identity = UserOSS->GetIdentityInterface();
//    if (Identity.IsValid()) {
//        TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0); // 로컬 사용자 index
//        if (Id.IsValid()) {
//            UKismetSystemLibrary::PrintString(this, Identity->GetPlayerNickname(*Id));
//            return Identity->GetPlayerNickname(*Id);
//        }
//        else {
//            return FString();
//        }
//    }
//    else {
//        return FString();
//    }
//}
//
///// <summary>
///// OSS자체를 가져오기
///// </summary>
///// <returns>시작할 때 입력받은 OSS</returns>
//IOnlineSubsystem* UBaseGameInstance::GetUserOSS()
//{
//	return UserOSS;
//}

void UBaseGameInstance::TravelToServer(const FString& InputIP)
{
    UE_LOG(LogTemp, Log, TEXT("BaseGameInstance : TravelToServer"));

    if (UWorld* World = GetWorld())
    {
        // FName으로 변환해 절대 경로(서버 주소)로 이동
        UGameplayStatics::OpenLevel(World, FName(*InputIP), true);
    }
}

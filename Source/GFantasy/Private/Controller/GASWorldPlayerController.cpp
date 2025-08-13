// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GASWorldPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GFantasy/Public/Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/WidgetControllers/InventoryWidgetController.h"
#include "UI/InventoryWidget.h"
#include "UI/WidgetControllers/CraftWidgetController.h"
#include "UI/CraftWidget.h"
#include "UI/Player/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "HUD/WorldHUD.h"
#include "Inventory/Crafting/CraftManagerComponent.h"
#include "PlayerState/GASBasePlayerState.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Character/GASPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/QuestManagerComponent.h"
#include "InputMappingContext.h"
#include "Actor/Chest/ChestBase.h"
#include "Actor/Riding/RidingActorBase.h"
#include "Character/GASCombatBossNPC.h"
#include "GAS/AttributeSet/GASBossGroggy.h"
#include "Character/GASTestPlayerCharacter.h"
#include "Character/GASNonCombatNPC.h"
#include "Character/Customization/CustomizerComponent.h"
#include "Character/Weapon/WeaponBase.h"
#include "UI/NPC/NPCGuideWidget.h"
#include "Components/WidgetComponent.h"


AGASWorldPlayerController::AGASWorldPlayerController()
{
	bReplicates = true;

	/*인벤토리 컴포넌트 설정*/
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(true);

	EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentComponent");

	CraftingComponent = CreateDefaultSubobject<UCraftManagerComponent>("CraftingComponent");
	CraftingComponent->SetIsReplicated(true);

}

void AGASWorldPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BindCallbacksToDependencies();	

	if (HasAuthority())
	{
		if (AGASBasePlayerState* PS = GetMyPlayerState())
		{
			CachedQuestManager = PS->FindComponentByClass<UQuestManagerComponent>();
			if (CachedQuestManager)
			{
				CachedQuestManager->LoadQuestsFromDataTable();

				//BindQuestManagerReadyServer(CachedQuestManager);
			}

			CachedCustomManager = PS->FindComponentByClass<UCustomizerComponent>();
		}
	}
	else
	{
		if (IsLocalPlayerController())
		{
			// (1) SaveGame 슬롯 존재 확인
			if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
			{
				// (2) SaveGame 객체 로드
				USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex);
				UGFSaveGame* SG = Cast<UGFSaveGame>(Loaded);
				if (SG)
				{

					const FPlayerSaveData& Data = SG->PlayerData;

					// --- ① 기본 정보 전송 ---
					Server_LoadPlayerNickname(Data.Nickname);
	
					// 제작법(Craft) 세이브 데이터를 서버로 전송
					LoadCraftSaveData(Data);

					// 인벤토리 세이브 데이터를 서버로 전송
					LoadInventorySaveData(Data);

					// 퀘스트 정보 저장 캐시
					CachedSaveData = Data;       // 전체 구조체를 보관
					bHasCachedSave = true;       // “있다” 플래그

					// 커스터마이저 세이브 데이터를 서버로 전송
					LoadCustomizerSaveData(Data);

					/* 3 초 후 SendQuestSaveData_Delayed() 호출 */
					GetWorld()->GetTimerManager().SetTimer(
						QuestLoadTimerHandle,
						this,
						&AGASWorldPlayerController::SendQuestSaveData_Delayed,
						5.0f,          // Delay(초)
						false          // Loop=false
					);
					
					// 퀘스트 세이브 데이트럴 서버로 전송
					//LoadQuestSaveData(Data);

					UE_LOG(LogTemp, Log, TEXT("PC: SaveData Complete"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("PC: SaveGame CantComplete"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("PC: No SaveSlot (%s)"), *SaveSlotName);
			}
		}
	}
}

UInventoryComponent* AGASWorldPlayerController::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

UCraftManagerComponent* AGASWorldPlayerController::GetCraftComponent_Implementation()
{
	return CraftingComponent;
}

UAbilitySystemComponent* AGASWorldPlayerController::GetAbilitySystemComponent() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}

void AGASWorldPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGASWorldPlayerController, InventoryComponent);
	DOREPLIFETIME(AGASWorldPlayerController, CraftingComponent);
}

void AGASWorldPlayerController::ClientSetGuideWidgetVisibility_Implementation(AGASNonCombatNPC* NPCActor, bool bVisible)
{
	if (!NPCActor) return;

	// 클라이언트 로컬에서만 실행
	if (UWidgetComponent* WidgetComp = NPCActor->GetGuideWidgetComponent())
	{
		WidgetComp->SetHiddenInGame(!bVisible);
	}
}

void AGASWorldPlayerController::ClientSetBoxGuideWidgetVisibility_Implementation(AChestBase* ChestActor,bool bVisible)
{
	if (UWidgetComponent* WidgetComp = ChestActor->GetGuideWidgetComponent())
	{
		WidgetComp->SetHiddenInGame(!bVisible);
	}
}

void AGASWorldPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// ASC / AS가 확실히 준비된 뒤, 한번만 초기화
	if (!bHUDInitialized && IsLocalController())
	{
		bHUDInitialized = true;

		if (AGASBasePlayerState* PS = GetMyPlayerState())
		{
			UAbilitySystemComponent* ASC =
				PS->GetAbilitySystemComponent();
			UGASCharacterAS* CharacterAS = PS->GetCharacterAttributeSet();
			UGASResourceAS* ResourceAS = PS->GetResourceAttributeSet();
			if (ASC && CharacterAS && ResourceAS)
			{
				// 현재 내 컨트롤러에 달린 HUD를 찾는다,
				if (AWorldHUD* WorldHUD = Cast<AWorldHUD>(GetHUD()))
				{
					if (UPlayerHUD* HUDWidget = WorldHUD->GetPlayerHUDWidget())
					{
						HUDWidget->InitializeHUD(ASC, CharacterAS, ResourceAS);
						bHUDInitialized = false;
					}
				}
			}
		}
	}
}



void AGASWorldPlayerController::RefreshInputContextFromEquipment(const FGameplayTag WeaponTag)
{
	
	// Local Player 기반 접근
	if (!IsLocalController())
		return;

	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem
		<UEnhancedInputLocalPlayerSubsystem> (GetLocalPlayer());

	if (!SubSystem)		return;
	
	SubSystem->ClearAllMappings();

	// // 전달 받은 Tag와 맞는 IMC가 있다면 적용.
	// if (UInputMappingContext** FoundIMC = WeaponIMCMap.Find(WeaponTag))
	// {
	// 	SubSystem->AddMappingContext(*FoundIMC, 0);
	//
	// 	const FString IMCName = (*FoundIMC)->GetName(); // GetName()은 FName 리턴
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("FoundIMC : %s"), *IMCName);
	// }
	// //없다면 기본 IMC 적용
	// else if(DefaultUnArmedIMC)
	// {
	// 	SubSystem->AddMappingContext(DefaultUnArmedIMC, 0);
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("Default IMC Apply"));
	// }

	SubSystem->AddMappingContext(DefaultUnArmedIMC, 0);
}

void AGASWorldPlayerController::Client_RefreshInputContextFromEquipment_Implementation(const FGameplayTag WeaponTag)
{
	RefreshInputContextFromEquipment(WeaponTag);
}

// 장비칸 중 무기 칸이 변경될 때 마다 해당 함수 실행
void AGASWorldPlayerController::HandleEquipmentChanged(const FGameplayTag Tag, bool IsEquipped)
{

	//서버에서만 실행
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authrity In Equip Changed"));
		return;
	}

	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon");

	if (!Tag.MatchesTag(WeaponSlotTag))
	{
		// 현재 바뀐 장비가 무기가 아니라면 Return
		return;
	}

	const TArray<FGFEquipmentEntry> CurrentEquipments = EquipmentComponent->GetEquipmentEntries();
	FGameplayTag WeaponTag;


	for (const FGFEquipmentEntry& Equipped : CurrentEquipments)
	{
		// 만약 무기 칸에 장비가 있다면
		if (Equipped.SlotTag.MatchesTag(WeaponSlotTag))
		{
			if (!IsEquipped)
			{
				// 현재 장착이 아니라 해제라면 Tag를 비운다.
				WeaponTag = FGameplayTag::EmptyTag;
				break;
			}
			else
			{
				// 해당 태그를 반환
				WeaponTag = Equipped.EntryTag;
				break;
			}
		}
	}

	// 해당 태그를 클라이언트에게 전달.
	if (!HasAuthority())
	{
		RefreshInputContextFromEquipment(WeaponTag);
	}
	else
	{
		Client_RefreshInputContextFromEquipment(WeaponTag);
	}

}

void AGASWorldPlayerController::UpdateIMCFromCurrentWeaponSlot()
{
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon");

	const TArray<FGFEquipmentEntry>& CurrentEquipment = EquipmentComponent->GetEquipmentEntries();

	for (const FGFEquipmentEntry& Entry : CurrentEquipment)
	{
		if (Entry.SlotTag.MatchesTag(WeaponSlotTag))
		{	
			// 해당 태그를 클라이언트에게 전달.
			if (!HasAuthority())
			{
				RefreshInputContextFromEquipment(Entry.EntryTag);
			}
			else
			{
				Client_RefreshInputContextFromEquipment(Entry.EntryTag);
			}
			return;
		}
	}

	// 무기가 없을 경우 Default 적용
	if (!HasAuthority())
	{
		RefreshInputContextFromEquipment(FGameplayTag::EmptyTag);
	}
	else
	{
		Client_RefreshInputContextFromEquipment(FGameplayTag::EmptyTag);
	}
}

void AGASWorldPlayerController::RefreshInputContextFromCombatState(bool bInCombat)
{
	// Local Player 기반 접근
	if (!IsLocalController())
		return;

	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem
		<UEnhancedInputLocalPlayerSubsystem> (GetLocalPlayer());

	if (!SubSystem)		return;
	
	SubSystem->ClearAllMappings();

	if (bInCombat)
	{
		FGameplayTag WeaponTag = FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.Sword.BronzeSword");
		UInputMappingContext** FoundIMC = WeaponIMCMap.Find(WeaponTag);

		if (FoundIMC)
		{
			SubSystem->AddMappingContext(*FoundIMC, 0);
		}
	}
	else
	{
		SubSystem->AddMappingContext(DefaultUnArmedIMC,0);
	}
}

void AGASWorldPlayerController::OnNetCleanup(class UNetConnection* Connection)
{
	Super::OnNetCleanup(Connection);

	APawn* ControlledPawn = GetPawn();
	if (ARidingActorBase* RidingActor = Cast<ARidingActorBase>(ControlledPawn))
	{
		// 현재 빙의되어 있는 상태가 말이라면
		AGASPlayerCharacter* Rider = RidingActor->Rider;

		if (IsValid(Rider))
		{
			if (IsValid(Rider->GetCurrentWeapon()))
			{
				Rider->GetCurrentWeapon()->Destroy();
			}
			Rider->Destroy();
		}
		RidingActor->Destroy();
	}
	else if (AGASPlayerCharacter* PlayerChar = Cast<AGASPlayerCharacter>(ControlledPawn))
	{
		// 현재 빙의되어 있는 상태가 캐릭터라면

		if (IsValid(PlayerChar->GetCurrentWeapon()))
		{
			PlayerChar->GetCurrentWeapon()->Destroy();
		}

		PlayerChar->Destroy();
	}
}

void AGASWorldPlayerController::Client_ApplyIMCForCurrentPawn_Implementation(bool IsMount)
{
	if (!IsLocalController())
		return;

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)	return;

	UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!SubSystem)	return;

	SubSystem->ClearAllMappings();

	// 캐릭터 인지 말인지 확인해서 적용
	if (AGASPlayerCharacter* PlayerChar = Cast<AGASPlayerCharacter>(ControlledPawn))
	{
		// 만약 플레이어라면 전투 상태 여부들으로 결정해서 IMC 적용
		bool bInCombat = (PlayerChar->GetCombatState() == ECombatState::InCombat);
		RefreshInputContextFromCombatState(bInCombat);

	}
	else if (ARidingActorBase* RidingActor = Cast<ARidingActorBase>(ControlledPawn))
	{
		if (RidingIMC)
		{
			SubSystem->AddMappingContext(RidingIMC, 0);
		}
	}
}

void AGASWorldPlayerController::Client_SetIMCForDoNotMove_Implementation()
{
	if (!IsLocalController())
		return;

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::
		GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		SubSystem->ClearAllMappings();

		if (IMC_None)
		{
			SubSystem->AddMappingContext(IMC_None, 0);
		}
	}
}

void AGASWorldPlayerController::Client_SwitchInteractionIMC_Implementation(bool bIsInteraction)
{
	if (!IsLocalController())
	return;

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::
		GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		SubSystem->ClearAllMappings();


		if (bIsInteraction)
		{
			if (IMC_None)
			{
				SubSystem->AddMappingContext(IMC_None, 0);
			}
			
		}
		else
		{
			if (DefaultUnArmedIMC)
			{
				SubSystem->AddMappingContext(DefaultUnArmedIMC, 0);
			}
		}

	}
}

void AGASWorldPlayerController::Client_RefreshInputContextFromCombatState_Implementation(bool bInCombat)
{
	RefreshInputContextFromCombatState(bInCombat);
}

void AGASWorldPlayerController::BindCallbacksToDependencies()
{
	if (!IsValid(InventoryComponent) || !IsValid(EquipmentComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("No Inventory Component, EquipmentComponent"));
		return;
	}

	// 인벤토리 / 장비창 델리게이트 연결
	InventoryComponent->EquipmentItemDelegate.AddLambda(
		[this](const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffect, const int32 EnhancementLevel)
		{
			if (IsValid(EquipmentComponent))
			{
				EquipmentComponent->EquipItem(EquipmentDefinition, StatEffect, EnhancementLevel);
			}
		});
	InventoryComponent->UnEquipmentItemDelegate.AddLambda(
		[this](const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffect, const int32 EnhancementLevel)
		{
			if (IsValid(EquipmentComponent))
			{
				EquipmentComponent->InventoryUnEquipItem(EquipmentDefinition, StatEffect, EnhancementLevel);
			}
		});
	EquipmentComponent->EquipmentList.EquippedEntryDelegate.AddLambda(
		[this](const FGFEquipmentEntry& EquippedEntry)
		{
			if (IsValid(InventoryComponent))
			{
				InventoryComponent->AddEquippedItemEntry(EquippedEntry.EntryTag, EquippedEntry.StatEffects, EquippedEntry.EnhancementLevel);
				HandleEquipmentChanged(EquippedEntry.SlotTag, true);
			}
		});
	EquipmentComponent->EquipmentList.UnEquippedEntryDelegate.AddLambda(
		[this](const FGFEquipmentEntry& UnEquippedEntry)
		{
			if (IsValid(InventoryComponent))
			{
				InventoryComponent->AddUnEquippedItemEntry(UnEquippedEntry.EntryTag, UnEquippedEntry.StatEffects, UnEquippedEntry.EnhancementLevel);
				HandleEquipmentChanged(UnEquippedEntry.SlotTag, false);
			}
		});
	InventoryComponent->AddCraftRecipeDelegate.AddLambda(
		[this](const FGFInventoryEntry& Entry, const TArray<FCraftRecipeItemGroup>& RecipeItems)
		{
			if (IsValid(CraftingComponent))
			{
				CraftingComponent->AddRecipe(Entry.ItemTag, RecipeItems);
			}
		});

	if (!IsLocalController())
	{
		AGASWorldPlayerState* MyPlayerState = Cast<AGASWorldPlayerState>(PlayerState);
		if (MyPlayerState)
		{
			if (UCustomizerComponent* CustomizerComponent = ICustomizerInterface::Execute_GetCustomizerComponent(MyPlayerState))
			{
				CustomizerComponent->CustomizerList.CustomizerChangedDelegate.AddLambda(
					[this](const FGFCustomizerEntry& CustomizerEntry)
					{
						AGASTestPlayerCharacter* MyCharacter = Cast<AGASTestPlayerCharacter>(GetPawn());
						if (MyCharacter)
						{
							//MyCharacter->SetCustomizeMesh(CustomizerEntry);
							//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Yellow, TEXT("Server Customize Delegate"));
						}
					});
			}
		}
	}
}

void AGASWorldPlayerController::Client_RequestQuestSaveData_Implementation()
{
	if (!IsLocalPlayerController())
	{
		return;                 // 원격 프록시면 무시
	}

	// 세이브 캐싱 여부 확인
	if (bHasCachedSave)
	{
		// 이미 만들어 둔 함수 재활용
		LoadQuestSaveData(CachedSaveData);   // 이제서야 RPC 전송
		bHasCachedSave = false;              // 1회 전송 후 해제(선택)
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No cached save; cannot send Quest data"));
	}
}

void AGASWorldPlayerController::HandleQuestManagerReady_Server()
{
	// 서버 권한에서 클라이언트에게 “보내 달라” 요청
	Client_RequestQuestSaveData();
}

void AGASWorldPlayerController::BindQuestManagerReadyServer(UQuestManagerComponent* QuestComp)
{
	if (!QuestComp) return;

	// 이미 초기화돼 있으면 즉시 실행
	if (QuestComp->IsReady())
	{
		HandleQuestManagerReady_Server();
		return;
	}

	// 아직 준비 전 → 델리게이트에 서버-측 콜백 등록
	QuestComp->OnQuestManagerReady.AddUObject(this, &AGASWorldPlayerController::HandleQuestManagerReady_Server);
}



void AGASWorldPlayerController::LoadCraftSaveData(const FPlayerSaveData& Data)
{	
	// --- ③ 제작법 페이징 전송 ---
	const int32 TotalC = Data.CraftSaveData.Num();
	const int32 CraftPageSize = 50;
	Server_InitCraftLoad(TotalC);
	for (int32 i = 0; i < FMath::DivideAndRoundUp(TotalC, CraftPageSize); ++i)
	{
		int32 Start = i * CraftPageSize;
		int32 Count = FMath::Min(CraftPageSize, TotalC - Start);
		TArray<FCraftSaveData> Page;
		Page.Append(&Data.CraftSaveData[Start], Count);
		Server_LoadCraftPage(i, Page);
	}
}

void AGASWorldPlayerController::LoadInventorySaveData(const FPlayerSaveData& Data)
{	
	// --- ③ 인벤토리 페이징 전송 ---
	const int32 TotalI = Data.InventorySaveData.Num();
	const int32 InvPageSize = 50;
	Server_InitInventoryLoad(TotalI);
	for (int32 i = 0; i < FMath::DivideAndRoundUp(TotalI, InvPageSize); ++i)
	{
		int32 Start = i * InvPageSize;
		int32 Count = FMath::Min(InvPageSize, TotalI - Start);
		TArray<FInventorySaveData> Page;
		Page.Append(&Data.InventorySaveData[Start], Count);
		Server_LoadInventoryPage(i, Page);
	}
}

void AGASWorldPlayerController::LoadQuestSaveData(const FPlayerSaveData& Data)
{	
	// --- ② 퀘스트 페이징 전송 ---
	UE_LOG(LogTemp, Log, TEXT("Save : PC - QuestLoad"));
	const int32 TotalQ = Data.QuestData.Num();
	const int32 QuestPageSize = 50;
	Server_InitQuestLoad(TotalQ);
	for (int32 i = 0; i < FMath::DivideAndRoundUp(TotalQ, QuestPageSize); ++i)
	{
		int32 Start = i * QuestPageSize;
		int32 Count = FMath::Min(QuestPageSize, TotalQ - Start);
		TArray<FQuestSaveData> Page;
		Page.Append(&Data.QuestData[Start], Count);
		Server_LoadQuestPage(i, Page);
	}
}

void AGASWorldPlayerController::LoadCustomizerSaveData(const FPlayerSaveData& Data)
{	
	// --- ③ 커마 페이징 전송 ---
	const int32 TotalM = Data.CustomizerSaveData.Num();
	const int32 CustomPageSize = 50;
	Server_InitCustomizerLoad(TotalM);
	for (int32 i = 0; i < FMath::DivideAndRoundUp(TotalM, CustomPageSize); ++i)
	{
		int32 Start = i * CustomPageSize;
		int32 Count = FMath::Min(CustomPageSize, TotalM - Start);
		TArray<FCustomizerSaveData> Page;
		Page.Append(&Data.CustomizerSaveData[Start], Count);
		Server_LoadCustomizerPage(i, Page);
	}
}



void AGASWorldPlayerController::Server_LoadPlayerNickname_Implementation(const FString& Nickname)
{
	// 클라PC에서 Beginplay로 SaveData를 가져온 후 서버RPC로 Save 정보 넘겨주기

	// 서버 권한을 가진 PlayerState에 데이터 적용
	if (AGASWorldPlayerState* PS = GetPlayerState<AGASWorldPlayerState>())
	{
		// --- 닉네임 적용 ---
		if (!Nickname.IsEmpty())
		{
			PS->SetPlayerNickName(Nickname);
		}
	}
}


void AGASWorldPlayerController::Server_InitCraftLoad_Implementation(int32 TotalCrafts)
{
	if (CraftingComponent)
	{
		//CraftingComponent->PrepareForLoad(TotalCrafts);
	}

	if (InventoryComponent)
	{
		InventoryComponent->PrepareCraftForLoad(TotalCrafts);
	}
}

void AGASWorldPlayerController::Server_LoadCraftPage_Implementation(int32 PageIndex, const TArray<FCraftSaveData>& CraftPage)
{
	if (CraftingComponent)
	{
		//CraftingComponent->LoadCraftPage(PageIndex, CraftPage);
	}

	if (InventoryComponent)
	{
		InventoryComponent->LoadCraftPage(PageIndex, CraftPage);
	}
}


void AGASWorldPlayerController::Server_InitInventoryLoad_Implementation(int32 TotalItems)
{
	if (InventoryComponent)
	{
		InventoryComponent->PrepareForLoad(TotalItems);
	}
}

void AGASWorldPlayerController::Server_LoadInventoryPage_Implementation(int32 PageIndex, const TArray<FInventorySaveData>& InvPage)
{
	if (InventoryComponent)
	{
		InventoryComponent->LoadInventoryPage(PageIndex, InvPage);
	}
}

void AGASWorldPlayerController::Server_InitQuestLoad_Implementation(int32 TotalQuests)
{
	if (CachedQuestManager)
	{
		CachedQuestManager->PrepareForLoad(TotalQuests);
	}
	else
	{
		AGASWorldPlayerState* PS = GetPlayerState<AGASWorldPlayerState>();
		if (UQuestManagerComponent* QuestComp = PS->FindComponentByClass<UQuestManagerComponent>())
		{
			QuestComp->PrepareForLoad(TotalQuests);
		}
	}
}

void AGASWorldPlayerController::Server_LoadQuestPage_Implementation(int32 PageIndex, const TArray<FQuestSaveData>& QuestPage)
{
	if (CachedQuestManager)
	{
		CachedQuestManager->LoadQuestPage(PageIndex, QuestPage);
	}
	else
	{
		AGASWorldPlayerState* PS = GetPlayerState<AGASWorldPlayerState>();
		if (UQuestManagerComponent* QuestComp = PS->FindComponentByClass<UQuestManagerComponent>())
		{
			QuestComp->LoadQuestPage(PageIndex, QuestPage);
		}
	}
}

void AGASWorldPlayerController::Server_InitCustomizerLoad_Implementation(int32 TotalCustoms)
{
	if (CachedCustomManager)
	{
		CachedCustomManager->PrepareForLoad(TotalCustoms);
	}
	else
	{
		AGASWorldPlayerState* PS = GetPlayerState<AGASWorldPlayerState>();
		if (UCustomizerComponent* CustomComp = PS->FindComponentByClass<UCustomizerComponent>())
		{
			CustomComp->PrepareForLoad(TotalCustoms);
		}
	}
}

void AGASWorldPlayerController::Server_LoadCustomizerPage_Implementation(int32 PageIndex, const TArray<FCustomizerSaveData>& CustomPage)
{
	if (CachedCustomManager)
	{
		CachedCustomManager->LoadCustomizerPage(PageIndex, CustomPage);
	}
	else
	{
		AGASWorldPlayerState* PS = GetPlayerState<AGASWorldPlayerState>();
		if (UCustomizerComponent* CustomComp = PS->FindComponentByClass<UCustomizerComponent>())
		{
			CustomComp->LoadCustomizerPage(PageIndex, CustomPage);
		}
	}
}

void AGASWorldPlayerController::SendQuestSaveData_Delayed()
{
	if (!IsLocalPlayerController()) return;

	if (!bHasCachedSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("Delayed quest load: no cached data"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Delayed quest load: sending to server"));
	LoadQuestSaveData(CachedSaveData);   // 기존 페이지-전송 함수 재사용
	bHasCachedSave = false;              // 1회 사용 후 해제
}

void AGASWorldPlayerController::SetPCMouseSensitivity(float NewSensitivity)
{
}

float AGASWorldPlayerController::GetPCMouseSensitivity() const
{
	return 0.0f;
}



UInventoryWidgetController* AGASWorldPlayerController::GetInventoryWidgetController()
{
	if (!IsValid(InventoryWidgetController))
	{
		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetOwningActor(this);
		InventoryWidgetController->BindCallbacksToDependencies();
	}

	return InventoryWidgetController;
}

UCraftWidgetController* AGASWorldPlayerController::GetCraftWidgetController()
{
	if (!IsValid(CraftWidgetController))
	{
		CraftWidgetController = NewObject<UCraftWidgetController>(this, CraftWidgetControllerClass);
		CraftWidgetController->SetOwningActor(this);
		CraftWidgetController->BindCallbacksToDependencies();
	}

	return CraftWidgetController;
}

UInventoryWidgetController* AGASWorldPlayerController::GetEnhancedWidgetController()
{
	if (!IsValid(EnhancedWidgetController))
	{
		EnhancedWidgetController = NewObject<UInventoryWidgetController>(this, EnhancedWidgetControllerClass);
		EnhancedWidgetController->SetOwningActor(this);
		EnhancedWidgetController->BindCallbacksToDependencies();
	}

	return EnhancedWidgetController;
}

void AGASWorldPlayerController::CreateInventoryWidget()
{
	if (UUserWidget* Widget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass))
	{
		InventoryWidget = Cast<UInventoryWidget>(Widget);
		InventoryWidget->SetWidgetController(GetInventoryWidgetController());
		InventoryWidgetController->BroadcastInitialValues();
		InventoryWidget->AddToViewport();
	}
}

void AGASWorldPlayerController::CreateCraftWidget()
{
	if (UUserWidget* Widget = CreateWidget<UCraftWidget>(this, CraftWidgetClass))
	{
		CraftWidget = Cast<UCraftWidget>(Widget);
		CraftWidget->SetWidgetController(GetCraftWidgetController());
		CraftWidgetController->BroadcastInitialValues();
		CraftWidget->AddToViewport();		
	}
}

void AGASWorldPlayerController::CreateEnhancedWidget()
{
	if (UUserWidget* Widget = CreateWidget<UInventoryWidget>(this, EnhancedWidgetClass))
	{
		EnhancedWidget = Cast<UInventoryWidget>(Widget);
		EnhancedWidget->SetWidgetController(GetEnhancedWidgetController());
		EnhancedWidgetController->BroadcastInitialValues();
		EnhancedWidget->AddToViewport();
	}
}

void AGASWorldPlayerController::ServerSetNickname_Implementation(const FString& InName)
{
	AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(PlayerState);
	if (PS)
	{
		PS->SetPlayerNickName(InName);
	}
}

void AGASWorldPlayerController::ClientSetNickname_Implementation(const FString& InName)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		AGASPlayerCharacter* MyCharacter = Cast<AGASPlayerCharacter>(GetPawn());
		if (MyCharacter)
		{
			UE_LOG(LogTemp, Log, TEXT("Set NickName : %s"), *InName);
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Set Nickname : %s"), *InName));
			MyCharacter->ServerSetNickname(InName);
		}
	}
}

void AGASWorldPlayerController::MultiSetNickname_Implementation(const FString& InName)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		AGASPlayerCharacter* MyCharacter = Cast<AGASPlayerCharacter>(GetPawn());
		if (MyCharacter)
		{
			UE_LOG(LogTemp, Log, TEXT("Set NickName : %s"), *InName);
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Set Nickname : %s"), *InName));
			MyCharacter->SetNicknameInWidget(InName);
			//MyCharacter->SetPlayerNameInWidget(InName);
		}
	}
}

void AGASWorldPlayerController::ClientShowInteractionWidget_Implementation(FGameplayTag InTag, const FText& InNameText, ENPCType InType)
{
	UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : In GASWorldPC"));

	if (AWorldHUD* WorldHUD = Cast<AWorldHUD>(GetHUD()))
	{
		UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : WorldHUD"));

		WorldHUD->ShowNPCInteractionWidget(InTag, InNameText, InType);
		UE_LOG(LogTemp, Log, TEXT("WorldPlayerController : NPCNameTag = %s, NPCName = %s"), *InTag.ToString(), *InNameText.ToString());

	}
}

void AGASWorldPlayerController::ClientShowQuestListWidget_Implementation(const TArray<UQuestInstance*>& Available, const TArray<UQuestInstance*>& InProgress, const TArray<UQuestInstance*>& Completed)
{
	if (!IsLocalController()) return;

	UE_LOG(LogTemp, Log, TEXT("Quest List : In GASWorldPC"));

	if (AWorldHUD* WorldHUD = Cast<AWorldHUD>(GetHUD()))
	{
		UE_LOG(LogTemp, Log, TEXT("Quest List : WorldHUD"));

		WorldHUD->ShowQuestListWidget(Available, InProgress, Completed);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Quest List : No WorldHUD"));
	}
}

void AGASWorldPlayerController::ServerShowQuestListWidget_Implementation()
{
	// PS의 QuestManager에게 정보를 받아서 UI 갱신
	AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(PlayerState);
	if (PS)
	{
		UQuestManagerComponent* QM = PS->FindComponentByClass<UQuestManagerComponent>();
		if (QM)
		{
			ClientShowQuestListWidget(QM->AvailableArr, QM->InProgressArr, QM->CompletedArr);
		}
	}
}

/**
 * <summary>
 * 보스 UI를 보여주는 클라 함수
 * </summary>
 * <param name="CurrentBoss"></param>
 */
void AGASWorldPlayerController::ClientShowBossUI_Implementation(AGASCombatBossNPC* CurrentBoss)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());
	CurrentHUD->ShowBossHpBar(CurrentBoss);
	
}

void AGASWorldPlayerController::ClientDestoryBossUI_Implementation()
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());
	CurrentHUD->HideBossHpBar();
}


void AGASWorldPlayerController::Client_SwitchIMCFromSequence_Implementation(bool bShouldMove)
{
	if (!IsLocalController())
		return;

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)	return;

	UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!SubSystem)	return;

	AGASPlayerCharacter* PlayerChar = Cast<AGASPlayerCharacter>(ControlledPawn);
	if (!PlayerChar)	return;
	SubSystem->ClearAllMappings();

	// 캐릭터가 움직일 수 있을 때,
	if (bShouldMove)
	{
		// 전투 상태 여부들으로 결정해서 IMC 적용
		bool bInCombat = (PlayerChar->GetCombatState() == ECombatState::InCombat);
		RefreshInputContextFromCombatState(bInCombat);

	}
	else
	{
		if (IMC_None)
		{
			SubSystem->AddMappingContext(IMC_None, 0);
		}
	}
}

void AGASWorldPlayerController::BindBossState(AGASCombatBossNPC* CurrentBoss)
{
	CurrentBoss->GetAbilitySystemComponent()
	->GetGameplayAttributeValueChangeDelegate(
			CurrentBoss->GetCharacterAttributeSet()->GetHealthAttribute()
		).AddUObject(this, &AGASWorldPlayerController::BindBossHealth);
	
	CurrentBoss->GetAbilitySystemComponent()
	->GetGameplayAttributeValueChangeDelegate(
			CurrentBoss->GetCharacterAttributeSet()->GetMaxHealthAttribute()
		).AddUObject(this, &AGASWorldPlayerController::BindBossMaxHealth);

	CurrentBoss->GetAbilitySystemComponent()
	->GetGameplayAttributeValueChangeDelegate(
		CurrentBoss->GetGroggyAttributeSet()->GetGroggyAttribute()
	).AddUObject(this, &AGASWorldPlayerController::BindBossGroggy);

	CurrentBoss->GetAbilitySystemComponent()
	->GetGameplayAttributeValueChangeDelegate(
		CurrentBoss->GetGroggyAttributeSet()->GetMaxGroggyAttribute()
	).AddUObject(this, &AGASWorldPlayerController::BindBossMaxGroggy);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this, CurrentBoss]()
		{
			InitBossState(
				CurrentBoss->GetCharacterAttributeSet()->GetHealth(),
				CurrentBoss->GetCharacterAttributeSet()->GetMaxHealth(),
				CurrentBoss->GetGroggyAttributeSet()->GetGroggy(),
				CurrentBoss->GetGroggyAttributeSet()->GetMaxGroggy(),
				CurrentBoss->MonsterName.ToString()
				);
		}),
		0.01f,
		false
	);
}

void AGASWorldPlayerController::BindBossHealth(const FOnAttributeChangeData& Data)
{
	SetBossHealth(Data.NewValue);
}

void AGASWorldPlayerController::InitBossState_Implementation(float Health, float MaxHealth, float Groggy, float MaxGroggy, const FString& NewBossName)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;
	CurrentHUD->SetBossUIHealth(Health);
	CurrentHUD->SetBossUIMaxHealth(MaxHealth);
	CurrentHUD->SetBossUIGroggy(Groggy);
	CurrentHUD->SetBossUIMaxGroggy(MaxGroggy);
	CurrentHUD->SetBossName(FText::FromString(NewBossName));
}


void AGASWorldPlayerController::SetBossHealth_Implementation(float NewHealth)
{	
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;
	
	CurrentHUD->SetBossUIHealth(NewHealth);
}

void AGASWorldPlayerController::BindBossMaxHealth(const FOnAttributeChangeData& Data)
{
	SetBossMaxHealth(Data.NewValue);
}

void AGASWorldPlayerController::SetBossMaxHealth_Implementation(float NewMaxHealth)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;
	CurrentHUD->SetBossUIMaxHealth(NewMaxHealth);
}

void AGASWorldPlayerController::BindBossGroggy(const FOnAttributeChangeData& Data)
{
	SetBossGroggy(Data.NewValue);
}

void AGASWorldPlayerController::SetBossGroggy_Implementation(float NewGroggy)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;
	CurrentHUD->SetBossUIGroggy(NewGroggy);
}

void AGASWorldPlayerController::BindBossMaxGroggy(const FOnAttributeChangeData& Data)
{
	SetBossMaxGroggy(Data.NewValue);
}

void AGASWorldPlayerController::SetBossMaxGroggy_Implementation(float NewMaxGroggy)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;
	CurrentHUD->SetBossUIMaxGroggy(NewMaxGroggy);
}

void AGASWorldPlayerController::ClientShowBossExtraGaugeUI_Implementation()
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());
	CurrentHUD->ShowBossExtraGaugeUI();
}

void AGASWorldPlayerController::ExtraGaugeUISpacebarClick_Implementation()
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());
	CurrentHUD->ExtraGaugeUISpacebarClick();
}

void AGASWorldPlayerController::BindBossExtraGauge(AGASCombatBossNPC* CurrentBoss)
{
	CurrentBoss->GetAbilitySystemComponent()
	->GetGameplayAttributeValueChangeDelegate(
		CurrentBoss->GetGroggyAttributeSet()->GetExtraGaugeAttribute()
	).AddUObject(this, &AGASWorldPlayerController::BindBossExtraGaugeFunc);

	CurrentBoss->GetAbilitySystemComponent()
	->GetGameplayAttributeValueChangeDelegate(
		CurrentBoss->GetGroggyAttributeSet()->GetMaxExtraGaugeAttribute()
	).AddUObject(this, &AGASWorldPlayerController::BindBossMaxExtraGaugeFunc);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this, CurrentBoss]()
		{
			SetBossExtraGauge(CurrentBoss->GetGroggyAttributeSet()->GetExtraGauge());
			SetBossMaxExtraGauge(CurrentBoss->GetGroggyAttributeSet()->GetMaxExtraGauge());
		}),
		0.01f,
		false
	);
}

void AGASWorldPlayerController::BindBossExtraGaugeFunc(const FOnAttributeChangeData& Data)
{
	SetBossExtraGauge(Data.NewValue);
}

void AGASWorldPlayerController::SetBossExtraGauge_Implementation(float NewExtraGauge)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;
	CurrentHUD->SetExtraGaugeUIExtraGauge(NewExtraGauge);
}

void AGASWorldPlayerController::BindBossMaxExtraGaugeFunc(const FOnAttributeChangeData& Data)
{
	SetBossMaxExtraGauge(Data.NewValue);
}

void AGASWorldPlayerController::SetBossMaxExtraGauge_Implementation(float NewMaxExtraGauge)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;
	CurrentHUD->SetExtraGaugeUIMaxExtraGauge(NewMaxExtraGauge);
}

void AGASWorldPlayerController::ClientDestroyBossExtraGaugeUI_Implementation()
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());
	CurrentHUD->HideBossExtraGaugeUI();
}

void AGASWorldPlayerController::ClientShowWerewolfPattern1Progressbar_Implementation()
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;

	CurrentHUD->ShowWerewolfPattern1Progressbar();
}

void AGASWorldPlayerController::ClientSetWerewolfPattern1ProgressbarPercent_Implementation(float NewPercent)
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;

	CurrentHUD->WerewolfPattern1SetProgressbarPercent(NewPercent);
}

void AGASWorldPlayerController::ClientHideWerewolfPattern1Progressbar_Implementation()
{
	AWorldHUD* CurrentHUD = Cast<AWorldHUD>(GetHUD());

	if (!CurrentHUD) return;

	CurrentHUD->HideWerewolfPattern1Progressbar();
}



/**
 * 김민형 제작 부분 끝
 */


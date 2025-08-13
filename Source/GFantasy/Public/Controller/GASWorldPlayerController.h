// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Controller/GASPlayerController.h"
#include "Interface/InventoryInterface.h"
#include "Inventory/InventoryComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Customization/CustomizerComponent.h"
#include "GFantasy/GFantasy.h"
#include "SaveGame/GFSaveGame.h"
#include "Interface/CustomizerInterface.h"
#include "GASWorldPlayerController.generated.h"


class AGASCombatBossNPC;
class UCraftManagerComponent;
class UEquipmentManagerComponent;
class UInventoryWidgetController;
class UInventoryWidget;
class UCraftWidgetController;
class UCraftWidget;
class UPlayerHUD;
class UInputMappingContext;
struct FOnAttributeChangeData;
class AWeaponBase;

/**
 * 
 */
UCLASS()
class GFANTASY_API AGASWorldPlayerController : public AGASPlayerController, public IAbilitySystemInterface, public IInventoryInterface, public ICustomizerInterface
{
	GENERATED_BODY()


public:

	AGASWorldPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	/*인벤토리 인터페이스 구현*/
	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;

	virtual UCraftManagerComponent* GetCraftComponent_Implementation() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UInventoryWidgetController* GetInventoryWidgetController();

	UCraftWidgetController* GetCraftWidgetController();

	UInventoryWidgetController* GetEnhancedWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void CreateCraftWidget();

	UFUNCTION(BlueprintCallable)
	void CreateEnhancedWidget();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// NPC 상호작용 UI를 보여주기 위한 클라이언트 RPC
	UFUNCTION(Client, Reliable)
	void ClientShowInteractionWidget(FGameplayTag InTag, const FText& InNameText, ENPCType InType);

	// NPC Guide위젯을 해당 클라에게만 보여주는 함수
	UFUNCTION(Client, Reliable)
	void ClientSetGuideWidgetVisibility(class AGASNonCombatNPC* NPCActor, bool bVisible);

	// 상자 위젯을 해당 클라에게만 보여주는 함수
	UFUNCTION(Client, Reliable)
	void ClientSetBoxGuideWidgetVisibility(class AChestBase* ChestActor, bool bVisible);

	virtual void OnRep_PlayerState() override;


	// Quest 상황을 보여주기 위한 클라이언트 RPC
	UFUNCTION(Client, Reliable)
	void ClientShowQuestListWidget(const TArray<UQuestInstance*>& Available, const TArray<UQuestInstance*>& InProgress, const TArray<UQuestInstance*>& Completed);

	// Quest 갱신을 위한 서버 요청 RPC
	UFUNCTION(Server, Reliable)
	void ServerShowQuestListWidget();


	// 닉네임 세팅을 위해 서버 RPC
	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& InName);

	UFUNCTION(Client, Reliable)
	void ClientSetNickname(const FString& InName);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSetNickname(const FString& InName);


	// 현재 장비에 따른 IMC 갱신
	UFUNCTION(BlueprintCallable)
	void RefreshInputContextFromEquipment(const FGameplayTag WeaponTag);

	UFUNCTION(Client, Reliable)
	void Client_RefreshInputContextFromEquipment(const FGameplayTag WeaponTag);

	// 서버 : 장비에 따른 IMC 갱신
	UFUNCTION(BlueprintCallable)
	void HandleEquipmentChanged(const FGameplayTag Tag, bool IsEquipped);

	// 현재 장착되어 있는 장비를 확인해서 해당 IMC로 등록
	UFUNCTION(BlueprintCallable)
	void UpdateIMCFromCurrentWeaponSlot();


	// 끌때 UI Only Mode -> Game Only/ Game & UI Mode 선택 함수
	UFUNCTION(BlueprintImplementableEvent)
	void SelectInputModeWhenUIOnly();


	/* IMC 변경 */
	
	// 전투 상태에 따른 IMC 변화
	UFUNCTION()
	void RefreshInputContextFromCombatState(bool bInCombat);

	// 전투 상태에 따른 IMC 변화 ( 클라이언트 실행 )
	UFUNCTION(Client, Reliable)
	void Client_RefreshInputContextFromCombatState(bool bInCombat);

	// 게임 종료할 때 실행되는 함수
	virtual void OnNetCleanup(class UNetConnection* Connection) override;

	// 말 / 캐릭터 간의 변화 IMC 적용
	UFUNCTION(Client, Reliable)
	void Client_ApplyIMCForCurrentPawn(bool IsMount);

	// 아무런 행동도 할 수 없는 IMC 적용
	UFUNCTION(Client, Reliable)
	void Client_SetIMCForDoNotMove();

	// 인터랙션 여부에 따라 함수 적용
	UFUNCTION(Client,Reliable,BlueprintCallable)
	void Client_SwitchInteractionIMC(bool bIsInteraction);

	// 시퀀스 여부에 따라 IMC 적용
	UFUNCTION(Client,Reliable,BlueprintCallable)
	void Client_SwitchIMCFromSequence(bool bShouldMove);
	
	/* 김민형 제작 부분(함수) */

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientShowBossUI(AGASCombatBossNPC* CurrentBoss);

	UFUNCTION(BlueprintCallable)
	void BindBossState(AGASCombatBossNPC* CurrentBoss);

	void BindBossHealth(const FOnAttributeChangeData& Data);

	UFUNCTION(NetMulticast, Reliable)
	void InitBossState(float Health, float MaxHealth, float Groggy, float MaxGroggy, const FString& NewBossName);

	UFUNCTION(NetMulticast, Reliable)
	void SetBossHealth(float NewHealth);

	void BindBossMaxHealth(const FOnAttributeChangeData& Data);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void SetBossMaxHealth(float NewMaxHealth);

	void BindBossGroggy(const FOnAttributeChangeData& Data);

	UFUNCTION(NetMulticast, Reliable)
	void SetBossGroggy(float NewGroggy);

	void BindBossMaxGroggy(const FOnAttributeChangeData& Data);

	UFUNCTION(NetMulticast, Reliable)
	void SetBossMaxGroggy(float NewMaxGroggy);

	UFUNCTION(Client, Reliable)
	void ClientDestoryBossUI();

	//------- ExtraGauge

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientShowBossExtraGaugeUI();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ExtraGaugeUISpacebarClick();

	UFUNCTION(BlueprintCallable)
	void BindBossExtraGauge(AGASCombatBossNPC* CurrentBoss);

	void BindBossExtraGaugeFunc(const FOnAttributeChangeData& Data);

	UFUNCTION(Client, Reliable)
	void SetBossExtraGauge(float NewExtraGauge);

	void BindBossMaxExtraGaugeFunc(const FOnAttributeChangeData& Data);

	UFUNCTION(Client, Reliable)
	void SetBossMaxExtraGauge(float NewMaxExtraGauge);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientDestroyBossExtraGaugeUI();

	//-------- Werewolf Round Progress bar
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientShowWerewolfPattern1Progressbar();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientSetWerewolfPattern1ProgressbarPercent(float NewPercent);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientHideWerewolfPattern1Progressbar();

	/* 김민형 제작 부분(함수) 끝 */
	

	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEquipmentManagerComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UCraftManagerComponent> CraftingComponent;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Widget")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UCraftWidgetController> CraftWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Widget")
	TSubclassOf<UCraftWidgetController> CraftWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> EnhancedWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Widget")
	TSubclassOf<UInventoryWidgetController> EnhancedWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryWidget> InventoryWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Widget")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCraftWidget> CraftWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Widget")
	TSubclassOf<UCraftWidget> CraftWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryWidget> EnhancedWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Widget")
	TSubclassOf<UInventoryWidget> EnhancedWidgetClass;
	
	// 무기에 따른 IMC Map
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<FGameplayTag, UInputMappingContext*> WeaponIMCMap;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultUnArmedIMC;

	
	// 무기 Tag에 따른 무기 액터
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TMap<FGameplayTag, AWeaponBase*> WeaponActorMap;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* RidingIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_None;

	void BindCallbacksToDependencies();
	
	bool bHUDInitialized = false;


	// ───── 서버 → 클라이언트: 세이브 전송 요청 ─────
	// 서버가 QuestManagerComponent 준비 완료 시 클라에 호출
	UFUNCTION(Client, Reliable)
	void Client_RequestQuestSaveData();

	// ───── 서버 전용 콜백 ─────
	// QuestManagerComponent 가 bInitialized == true 가 되었을 때 호출 */
	UFUNCTION()
	void HandleQuestManagerReady_Server();

	// 서버에서 QuestManagerReady 델리게이트에 바인딩
	void BindQuestManagerReadyServer(class UQuestManagerComponent* QuestComp);

	/* 캐싱된 세이브 */
	FPlayerSaveData CachedSaveData;
	bool bHasCachedSave = false;

protected:
	// SaveGame 슬롯 이름 (에디터에서 설정 가능)
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	FString SaveSlotName = TEXT("PlayerSaveSlot");

	// 사용자 인덱스 (보통 0)
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	uint32 UserIndex = 0;


	// 캐시해둘 QuestManagerComponent 포인터
	UPROPERTY()
	class UQuestManagerComponent* CachedQuestManager = nullptr;

	// 캐시해둘 CustomizerComponent 포인터
	UPROPERTY()
	class UCustomizerComponent* CachedCustomManager = nullptr;

	// 제작법(Craft) 세이브 데이터를 서버로 전송
	void LoadCraftSaveData(const FPlayerSaveData& Data);

	// 인벤토리 세이브 데이터를 서버로 전송
	void LoadInventorySaveData(const FPlayerSaveData& Data);

	// 퀘스트 세이브 데이터를 서버로 전송
	void LoadQuestSaveData(const FPlayerSaveData& Data);

	// 커스터마이저 세이브 데이터를 서버로 전송
	void LoadCustomizerSaveData(const FPlayerSaveData& Data);


	// --- ① 기본 정보 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_LoadPlayerNickname(const FString& Nickname);


	// --- ③ 제작법 페이징 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitCraftLoad(int32 TotalCrafts);

	UFUNCTION(Server, Reliable)
	void Server_LoadCraftPage(int32 PageIndex, const TArray<FCraftSaveData>& CraftPage);
	

	// --- ③ 인벤토리 페이징 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitInventoryLoad(int32 TotalItems);

	UFUNCTION(Server, Reliable)
	void Server_LoadInventoryPage(int32 PageIndex, const TArray<FInventorySaveData>& InvPage);


	// --- ② 퀘스트 페이징 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitQuestLoad(int32 TotalQuests);

	UFUNCTION(Server, Reliable)
	void Server_LoadQuestPage(int32 PageIndex, const TArray<FQuestSaveData>& QuestPage);
	

	// --- ② 커마 페이징 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitCustomizerLoad(int32 TotalCustoms);

	UFUNCTION(Server, Reliable)
	void Server_LoadCustomizerPage(int32 PageIndex, const TArray<FCustomizerSaveData>& CustomPage);

public:
	UFUNCTION()
	void SendQuestSaveData_Delayed();

private:
	FTimerHandle QuestLoadTimerHandle;

/////////////////////////// 마우스 감도 설정 ///////////////////////////
public:

	// 감도 설정
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetPCMouseSensitivity(float NewSensitivity);

	// 감도 조회
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetPCMouseSensitivity() const;

private:
	float MouseSensitivity = 1.0f;
};

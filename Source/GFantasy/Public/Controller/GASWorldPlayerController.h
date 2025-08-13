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
	/*�κ��丮 �������̽� ����*/
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

	// NPC ��ȣ�ۿ� UI�� �����ֱ� ���� Ŭ���̾�Ʈ RPC
	UFUNCTION(Client, Reliable)
	void ClientShowInteractionWidget(FGameplayTag InTag, const FText& InNameText, ENPCType InType);

	// NPC Guide������ �ش� Ŭ�󿡰Ը� �����ִ� �Լ�
	UFUNCTION(Client, Reliable)
	void ClientSetGuideWidgetVisibility(class AGASNonCombatNPC* NPCActor, bool bVisible);

	// ���� ������ �ش� Ŭ�󿡰Ը� �����ִ� �Լ�
	UFUNCTION(Client, Reliable)
	void ClientSetBoxGuideWidgetVisibility(class AChestBase* ChestActor, bool bVisible);

	virtual void OnRep_PlayerState() override;


	// Quest ��Ȳ�� �����ֱ� ���� Ŭ���̾�Ʈ RPC
	UFUNCTION(Client, Reliable)
	void ClientShowQuestListWidget(const TArray<UQuestInstance*>& Available, const TArray<UQuestInstance*>& InProgress, const TArray<UQuestInstance*>& Completed);

	// Quest ������ ���� ���� ��û RPC
	UFUNCTION(Server, Reliable)
	void ServerShowQuestListWidget();


	// �г��� ������ ���� ���� RPC
	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& InName);

	UFUNCTION(Client, Reliable)
	void ClientSetNickname(const FString& InName);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSetNickname(const FString& InName);


	// ���� ��� ���� IMC ����
	UFUNCTION(BlueprintCallable)
	void RefreshInputContextFromEquipment(const FGameplayTag WeaponTag);

	UFUNCTION(Client, Reliable)
	void Client_RefreshInputContextFromEquipment(const FGameplayTag WeaponTag);

	// ���� : ��� ���� IMC ����
	UFUNCTION(BlueprintCallable)
	void HandleEquipmentChanged(const FGameplayTag Tag, bool IsEquipped);

	// ���� �����Ǿ� �ִ� ��� Ȯ���ؼ� �ش� IMC�� ���
	UFUNCTION(BlueprintCallable)
	void UpdateIMCFromCurrentWeaponSlot();


	// ���� UI Only Mode -> Game Only/ Game & UI Mode ���� �Լ�
	UFUNCTION(BlueprintImplementableEvent)
	void SelectInputModeWhenUIOnly();


	/* IMC ���� */
	
	// ���� ���¿� ���� IMC ��ȭ
	UFUNCTION()
	void RefreshInputContextFromCombatState(bool bInCombat);

	// ���� ���¿� ���� IMC ��ȭ ( Ŭ���̾�Ʈ ���� )
	UFUNCTION(Client, Reliable)
	void Client_RefreshInputContextFromCombatState(bool bInCombat);

	// ���� ������ �� ����Ǵ� �Լ�
	virtual void OnNetCleanup(class UNetConnection* Connection) override;

	// �� / ĳ���� ���� ��ȭ IMC ����
	UFUNCTION(Client, Reliable)
	void Client_ApplyIMCForCurrentPawn(bool IsMount);

	// �ƹ��� �ൿ�� �� �� ���� IMC ����
	UFUNCTION(Client, Reliable)
	void Client_SetIMCForDoNotMove();

	// ���ͷ��� ���ο� ���� �Լ� ����
	UFUNCTION(Client,Reliable,BlueprintCallable)
	void Client_SwitchInteractionIMC(bool bIsInteraction);

	// ������ ���ο� ���� IMC ����
	UFUNCTION(Client,Reliable,BlueprintCallable)
	void Client_SwitchIMCFromSequence(bool bShouldMove);
	
	/* ����� ���� �κ�(�Լ�) */

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

	/* ����� ���� �κ�(�Լ�) �� */
	

	
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
	
	// ���⿡ ���� IMC Map
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<FGameplayTag, UInputMappingContext*> WeaponIMCMap;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultUnArmedIMC;

	
	// ���� Tag�� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TMap<FGameplayTag, AWeaponBase*> WeaponActorMap;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* RidingIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_None;

	void BindCallbacksToDependencies();
	
	bool bHUDInitialized = false;


	// ���������� ���� �� Ŭ���̾�Ʈ: ���̺� ���� ��û ����������
	// ������ QuestManagerComponent �غ� �Ϸ� �� Ŭ�� ȣ��
	UFUNCTION(Client, Reliable)
	void Client_RequestQuestSaveData();

	// ���������� ���� ���� �ݹ� ����������
	// QuestManagerComponent �� bInitialized == true �� �Ǿ��� �� ȣ�� */
	UFUNCTION()
	void HandleQuestManagerReady_Server();

	// �������� QuestManagerReady ��������Ʈ�� ���ε�
	void BindQuestManagerReadyServer(class UQuestManagerComponent* QuestComp);

	/* ĳ�̵� ���̺� */
	FPlayerSaveData CachedSaveData;
	bool bHasCachedSave = false;

protected:
	// SaveGame ���� �̸� (�����Ϳ��� ���� ����)
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	FString SaveSlotName = TEXT("PlayerSaveSlot");

	// ����� �ε��� (���� 0)
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	uint32 UserIndex = 0;


	// ĳ���ص� QuestManagerComponent ������
	UPROPERTY()
	class UQuestManagerComponent* CachedQuestManager = nullptr;

	// ĳ���ص� CustomizerComponent ������
	UPROPERTY()
	class UCustomizerComponent* CachedCustomManager = nullptr;

	// ���۹�(Craft) ���̺� �����͸� ������ ����
	void LoadCraftSaveData(const FPlayerSaveData& Data);

	// �κ��丮 ���̺� �����͸� ������ ����
	void LoadInventorySaveData(const FPlayerSaveData& Data);

	// ����Ʈ ���̺� �����͸� ������ ����
	void LoadQuestSaveData(const FPlayerSaveData& Data);

	// Ŀ���͸����� ���̺� �����͸� ������ ����
	void LoadCustomizerSaveData(const FPlayerSaveData& Data);


	// --- �� �⺻ ���� RPC ---
	UFUNCTION(Server, Reliable)
	void Server_LoadPlayerNickname(const FString& Nickname);


	// --- �� ���۹� ����¡ RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitCraftLoad(int32 TotalCrafts);

	UFUNCTION(Server, Reliable)
	void Server_LoadCraftPage(int32 PageIndex, const TArray<FCraftSaveData>& CraftPage);
	

	// --- �� �κ��丮 ����¡ RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitInventoryLoad(int32 TotalItems);

	UFUNCTION(Server, Reliable)
	void Server_LoadInventoryPage(int32 PageIndex, const TArray<FInventorySaveData>& InvPage);


	// --- �� ����Ʈ ����¡ RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitQuestLoad(int32 TotalQuests);

	UFUNCTION(Server, Reliable)
	void Server_LoadQuestPage(int32 PageIndex, const TArray<FQuestSaveData>& QuestPage);
	

	// --- �� Ŀ�� ����¡ RPC ---
	UFUNCTION(Server, Reliable)
	void Server_InitCustomizerLoad(int32 TotalCustoms);

	UFUNCTION(Server, Reliable)
	void Server_LoadCustomizerPage(int32 PageIndex, const TArray<FCustomizerSaveData>& CustomPage);

public:
	UFUNCTION()
	void SendQuestSaveData_Delayed();

private:
	FTimerHandle QuestLoadTimerHandle;

/////////////////////////// ���콺 ���� ���� ///////////////////////////
public:

	// ���� ����
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetPCMouseSensitivity(float NewSensitivity);

	// ���� ��ȸ
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetPCMouseSensitivity() const;

private:
	float MouseSensitivity = 1.0f;
};

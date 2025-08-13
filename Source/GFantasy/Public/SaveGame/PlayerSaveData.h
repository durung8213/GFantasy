// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Equipment/EquipmentTypes.h"
#include "PlayerSaveData.generated.h"

/**
 *
 */

// ����Ʈ �����Ȳ ����� ����ü
USTRUCT(BlueprintType)
struct GFANTASY_API FQuestSaveData
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    FName S_QuestID;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    FGameplayTag S_QuestStateTag;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    int32 S_CurrentCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    bool S_bCanComplete = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    bool S_bCanAcceptQuest = true;

};

// �κ��丮 ����� ����ü
USTRUCT(BlueprintType)
struct GFANTASY_API FInventorySaveData
{
    GENERATED_BODY()

    // �⺻ ������: ��� ����� �ʱⰪ���� ���� */
    FInventorySaveData()
        : ItemTag()
        , CategoryTag()
        , EnhancementLevel(0)
        , ItemName(FText::GetEmpty())
        , Quantity(0)
        , ItemID(0)
        //, StatEffects()
        , Equipped(0)
    {}

    // ������ �±�
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FGameplayTag ItemTag;

    // ī�װ� �±�
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FGameplayTag CategoryTag;

    // ��ȭ ����
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 EnhancementLevel;

    // ������ �̸�
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FText ItemName;

    // ����
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 Quantity;

    // ���� ID
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 ItemID;

    // ���� ����Ʈ �׷�
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<struct FEquipmentStatEffectGroup> StatEffects;

    // ���� ���� (0: ������, 1: ����) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 Equipped;

    // ��ȿ�� �˻� */
    bool IsValid() const
    {
        return ItemID != 0;
    };
};


// ���۹� ����� ����ü
USTRUCT(BlueprintType)
struct GFANTASY_API FCraftSaveData
{
    GENERATED_BODY()

    FCraftSaveData()
        : EntryTag()
    {}

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FGameplayTag EntryTag;
};


// Ŀ�� ����� ����ü
USTRUCT(BlueprintType)
struct GFANTASY_API FCustomizerSaveData
{
    GENERATED_BODY()

    FCustomizerSaveData()
        : CustomizeTag()                                  // �⺻ FGameplayTag() �� �ʱ�ȭ
        , SkeletalMeshName(NAME_None)                     // NAME_None �� �ʱ�ȭ
        , CustomizeName(NAME_None)                        // NAME_None �� �ʱ�ȭ
        , CustomMesh(nullptr)                             // �޽� nullptr �� �ʱ�ȭ
        , SubCustomMesh(nullptr)                          // ���� �޽� nullptr �� �ʱ�ȭ
        , CustomMaterial(nullptr)                         // ��Ƽ���� ����Ʈ ���� �ʱ�ȭ
        , CustomMaterialColor(FLinearColor::Black)        // ���������� �ʱⰪ ����
    {}

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FGameplayTag CustomizeTag;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FName SkeletalMeshName;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FName CustomizeName;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TObjectPtr<USkeletalMesh> CustomMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TObjectPtr<USkeletalMesh> SubCustomMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TSoftObjectPtr<UMaterialInterface> CustomMaterial;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FLinearColor CustomMaterialColor;
};


USTRUCT(BlueprintType)
struct GFANTASY_API FPlayerSaveData
{
	GENERATED_BODY()

public:
    FPlayerSaveData();

public:
    // 1. ���� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    FString Nickname;

    // 2. ����Ʈ ���� ��Ȳ
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FQuestSaveData> QuestData;

    // 3. �κ��丮 ����
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FInventorySaveData> InventorySaveData;

    // 4. ����� ���۹� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FCraftSaveData> CraftSaveData;

    // 5. Ŀ���͸���¡ ����
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FCustomizerSaveData> CustomizerSaveData;


};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Equipment/EquipmentTypes.h"
#include "PlayerSaveData.generated.h"

/**
 *
 */

// 퀘스트 진행상황 저장용 구조체
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

// 인벤토리 저장용 구조체
USTRUCT(BlueprintType)
struct GFANTASY_API FInventorySaveData
{
    GENERATED_BODY()

    // 기본 생성자: 모든 멤버를 초기값으로 세팅 */
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

    // 아이템 태그
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FGameplayTag ItemTag;

    // 카테고리 태그
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FGameplayTag CategoryTag;

    // 강화 레벨
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 EnhancementLevel;

    // 아이템 이름
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    FText ItemName;

    // 수량
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 Quantity;

    // 고유 ID
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 ItemID;

    // 스탯 이펙트 그룹
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<struct FEquipmentStatEffectGroup> StatEffects;

    // 장착 여부 (0: 미장착, 1: 장착) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 Equipped;

    // 유효성 검사 */
    bool IsValid() const
    {
        return ItemID != 0;
    };
};


// 제작법 저장용 구조체
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


// 커마 저장용 구조체
USTRUCT(BlueprintType)
struct GFANTASY_API FCustomizerSaveData
{
    GENERATED_BODY()

    FCustomizerSaveData()
        : CustomizeTag()                                  // 기본 FGameplayTag() 로 초기화
        , SkeletalMeshName(NAME_None)                     // NAME_None 로 초기화
        , CustomizeName(NAME_None)                        // NAME_None 로 초기화
        , CustomMesh(nullptr)                             // 메시 nullptr 로 초기화
        , SubCustomMesh(nullptr)                          // 서브 메쉬 nullptr 로 초기화
        , CustomMaterial(nullptr)                         // 머티리얼 소프트 참조 초기화
        , CustomMaterialColor(FLinearColor::Black)        // 검정색으로 초기값 설정
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
    // 1. 계정 정보
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    FString Nickname;

    // 2. 퀘스트 진행 상황
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FQuestSaveData> QuestData;

    // 3. 인벤토리 정보
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FInventorySaveData> InventorySaveData;

    // 4. 사용한 제작법 정보
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FCraftSaveData> CraftSaveData;

    // 5. 커스터마이징 정보
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
    TArray<FCustomizerSaveData> CustomizerSaveData;


};
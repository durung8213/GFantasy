// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DataAsset/SkillBarConfig.h"
#include "DataAsset/PlayerSkillLibrary.h"
#include "AbilitySystemComponent.h"    
#include "SkillBarComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GFANTASY_API USkillBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillBarComponent();

	// 서버에서 최초 한 번 호출
	UFUNCTION(BlueprintCallable, Category = "ActionBar")
	void InitializeSkillBar();

	// UI / 클라이언트애서 슬롯 교체 요청
	UFUNCTION(BlueprintCallable, Category = "ActionBar")
	void RequestSwapSlotSkill(EAbilitySlot Slot, FGameplayTag SkillTag);

	// Q, E 등 입력 시, 실행할 로컬 액티베이트
	// 슬롯에 있는 스킬 실행
	UFUNCTION(BlueprintCallable, Category = "ActionBar")
	void ActivateSlot(EAbilitySlot Slot);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_SkillLibrary();

	UFUNCTION()
	void OnRep_SkillBarConfig();

protected:

	//서버 RPC
	UFUNCTION(Server, Reliable)
	void Server_SwapSkillSlot(EAbilitySlot Slot, FGameplayTag SkillTag);

	// 실제 서버 로직, ( 스킬 제거 -> Config 갱신 -> 스킬 부여 )
	void SwapSlotSkill_Internal(EAbilitySlot Slot, FGameplayTag SkillTag);


	virtual void BeginPlay() override;

	void TryInitialize();

public:	

	// DataAsset 참조 ( 에디터 할당 ) 
	// 전체 스킬 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillBar", ReplicatedUsing = OnRep_SkillLibrary)
	UPlayerSkillLibrary* SkillLibrary;

	// 스킬 태그 & 게임 어빌리티 Map 관리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_SkillBarConfig)
	USkillBarConfig* SkillBarConfig;

	// 슬롯별로 ASC에 부여된 SpecHandle 저장
	TMap<EAbilitySlot, FGameplayAbilitySpecHandle> ActiveSpec;

	// 초기화 여부 체크 Bool
	bool bIsInitialized = false;
	
};

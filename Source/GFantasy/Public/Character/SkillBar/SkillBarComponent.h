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

	// �������� ���� �� �� ȣ��
	UFUNCTION(BlueprintCallable, Category = "ActionBar")
	void InitializeSkillBar();

	// UI / Ŭ���̾�Ʈ�ּ� ���� ��ü ��û
	UFUNCTION(BlueprintCallable, Category = "ActionBar")
	void RequestSwapSlotSkill(EAbilitySlot Slot, FGameplayTag SkillTag);

	// Q, E �� �Է� ��, ������ ���� ��Ƽ����Ʈ
	// ���Կ� �ִ� ��ų ����
	UFUNCTION(BlueprintCallable, Category = "ActionBar")
	void ActivateSlot(EAbilitySlot Slot);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_SkillLibrary();

	UFUNCTION()
	void OnRep_SkillBarConfig();

protected:

	//���� RPC
	UFUNCTION(Server, Reliable)
	void Server_SwapSkillSlot(EAbilitySlot Slot, FGameplayTag SkillTag);

	// ���� ���� ����, ( ��ų ���� -> Config ���� -> ��ų �ο� )
	void SwapSlotSkill_Internal(EAbilitySlot Slot, FGameplayTag SkillTag);


	virtual void BeginPlay() override;

	void TryInitialize();

public:	

	// DataAsset ���� ( ������ �Ҵ� ) 
	// ��ü ��ų ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillBar", ReplicatedUsing = OnRep_SkillLibrary)
	UPlayerSkillLibrary* SkillLibrary;

	// ��ų �±� & ���� �����Ƽ Map ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_SkillBarConfig)
	USkillBarConfig* SkillBarConfig;

	// ���Ժ��� ASC�� �ο��� SpecHandle ����
	TMap<EAbilitySlot, FGameplayAbilitySpecHandle> ActiveSpec;

	// �ʱ�ȭ ���� üũ Bool
	bool bIsInitialized = false;
	
};

// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/SkillBar/SkillBarComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USkillBarComponent::USkillBarComponent()
{
	SetIsReplicatedByDefault(true);
}

// 첫 생성 때 한번만 호출 -> 초기화
void USkillBarComponent::InitializeSkillBar()
{
	UE_LOG(LogTemp, Warning, TEXT("=== InitializeSkillBar START ==="));

	if (!GetOwner()->HasAuthority())
		UE_LOG(LogTemp, Error, TEXT("SkillBar Init Fail: No Authority"));

	if (!SkillLibrary)
		UE_LOG(LogTemp, Error, TEXT("SkillBar Init Fail: SkillLibrary is NULL"));

	if (!SkillBarConfig)
		UE_LOG(LogTemp, Error, TEXT("SkillBar Init Fail: SkillBarConfig is NULL"));

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
	if (!ASI)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillBar Init Failed: Owner is not ASC interface"));
		return;
	}
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC || !ASC->AbilityActorInfo.IsValid() || !ASC->AbilityActorInfo->OwnerActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("SkillBar Init Failed: ASC not properly initialized"));
		return;
	}


	//// 스킬바에 지정된 모든 태그들을 기준으로 어빌리티 등록
	//for (auto& Pair : SkillBarConfig->SlotSkillTags)
	//{
	//	FGameplayTag Tag = Pair.Value;

	//	if (auto Class = SkillLibrary->GetAbilityClass(Tag))
	//	{
	//		// Tag 로 찾은 어빌리티들을 추가해준다.
	//		FGameplayAbilitySpec Spec(Class, 1, (int32)Pair.Key, this);
	//		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
	//		ActiveSpec.Add(Pair.Key, Handle);
	//	}
	//}

	// 안전하게 어빌리티 부여
	for (auto& Pair : SkillBarConfig->SlotSkillTags)
	{

		FGameplayTag Tag = Pair.Value;

		if (!Tag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid tag in SkillBarConfig for slot: %d"), static_cast<int32>(Pair.Key));
			return;
		}

		TSubclassOf<UGameplayAbility> Class = SkillLibrary->GetAbilityClass(Tag);

		if (!Class)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability class not found for tag: %s"), *Tag.ToString());
			continue;
		}

		FGameplayAbilitySpec Spec(Class, 1, (int32)Pair.Key, this);
		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		ActiveSpec.Add(Pair.Key, Handle);
	}
}

void USkillBarComponent::RequestSwapSlotSkill(EAbilitySlot Slot, FGameplayTag SkillTag)
{
	if (GetOwner()->HasAuthority())
	{
		SwapSlotSkill_Internal(Slot, SkillTag);
	}
	else
	{
		Server_SwapSkillSlot(Slot, SkillTag);
	}
}

void USkillBarComponent::ActivateSlot(EAbilitySlot Slot)
{
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
	if (!ASI)	return;
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	//ASC->AbilityLocalInputPressed((int32)Slot);

	if (SkillBarConfig->SlotSkillTags.Contains(Slot))
	{
		FGameplayTag Tag = SkillBarConfig->SlotSkillTags[Slot];
		if (Tag.IsValid())
		{
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(Tag));
		}
	}
}

// Called when the game starts
void USkillBarComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USkillBarComponent::TryInitialize()
{
	if (GetOwner()->HasAuthority() && SkillLibrary && SkillBarConfig && !bIsInitialized)
	{
		InitializeSkillBar();
		bIsInitialized = true;
		UE_LOG(LogTemp, Warning, TEXT("SkillBar Initialized (via OnRep)"));
	}
}


void USkillBarComponent::Server_SwapSkillSlot_Implementation(EAbilitySlot Slot, FGameplayTag SkillTag)
{
	SwapSlotSkill_Internal(Slot, SkillTag);
}

void USkillBarComponent::SwapSlotSkill_Internal(EAbilitySlot Slot, FGameplayTag SkillTag)
{
	if (!SkillLibrary || !SkillBarConfig)
		return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
	if (!ASI)	return;
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();

	// 1. 기존 스킬 제거
	if (ActiveSpec.Contains(Slot))
	{
		ASC->ClearAbility(ActiveSpec[Slot]);
		ActiveSpec.Remove(Slot);
	}

	// 2. Config 태그 갱신
	SkillBarConfig->SlotSkillTags[Slot] = SkillTag;

	// 3. 새로운 스킬 부여
	if (auto Class = SkillLibrary->GetAbilityClass(SkillTag))
	{
		FGameplayAbilitySpec Spec(Class, 1, (int32)Slot, this);
		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		ActiveSpec.Add(Slot, Handle);

	}
}


void USkillBarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USkillBarComponent, SkillLibrary, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USkillBarComponent, SkillBarConfig, COND_None, REPNOTIFY_OnChanged);
}

void USkillBarComponent::OnRep_SkillLibrary()
{
}

void USkillBarComponent::OnRep_SkillBarConfig()
{
}

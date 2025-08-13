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

// ù ���� �� �ѹ��� ȣ�� -> �ʱ�ȭ
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


	//// ��ų�ٿ� ������ ��� �±׵��� �������� �����Ƽ ���
	//for (auto& Pair : SkillBarConfig->SlotSkillTags)
	//{
	//	FGameplayTag Tag = Pair.Value;

	//	if (auto Class = SkillLibrary->GetAbilityClass(Tag))
	//	{
	//		// Tag �� ã�� �����Ƽ���� �߰����ش�.
	//		FGameplayAbilitySpec Spec(Class, 1, (int32)Pair.Key, this);
	//		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
	//		ActiveSpec.Add(Pair.Key, Handle);
	//	}
	//}

	// �����ϰ� �����Ƽ �ο�
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

	// 1. ���� ��ų ����
	if (ActiveSpec.Contains(Slot))
	{
		ASC->ClearAbility(ActiveSpec[Slot]);
		ActiveSpec.Remove(Slot);
	}

	// 2. Config �±� ����
	SkillBarConfig->SlotSkillTags[Slot] = SkillTag;

	// 3. ���ο� ��ų �ο�
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

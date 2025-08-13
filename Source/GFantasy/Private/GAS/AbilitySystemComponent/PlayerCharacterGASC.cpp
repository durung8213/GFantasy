// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Net/UnrealNetwork.h"


void UPlayerCharacterGASC::Server_SetInputDirection_Implementation(const FVector& Direction)
{
	InputDirection = Direction;
}

void UPlayerCharacterGASC::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);
		GiveAbility(AbilitySpec);
	}
}

void UPlayerCharacterGASC::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : PassivesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UPlayerCharacterGASC::DeferredActivateAbilityByTag(FGameplayTag AbilityTag)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(AbilityTag);

	bool bSuccess = TryActivateAbilitiesByTag(Tags);    
	
	if (AbilityActorInfo->AbilitySystemComponent->GetActivatableAbilities().Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Activatable Abilities"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Blocked? Maybe by other active ability"));
	}
}

// 장비 효과 적용
void UPlayerCharacterGASC::AddEquipmentEffects(FGFEquipmentEntry* EquipmentEntry)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	TWeakObjectPtr<UPlayerCharacterGASC> WeakThis(this);
	
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	
	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->StatEffects)
	{
		if (IsValid(StatEffect.EffectClass.Get()))
		{
			const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
			const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
		}
		else
		{
			Manager.RequestAsyncLoad(StatEffect.EffectClass.ToSoftObjectPath(),
				[WeakThis, StatEffect, ContextHandle, EquipmentEntry]
				{
					const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
					const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

					EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
				});
		}
	}
}

// 장비 효과 적용 해제
void UPlayerCharacterGASC::RemoveEquipmentEffects(FGFEquipmentEntry* EquipmentEntry)
{
	for (auto HandleIt = EquipmentEntry->GrantedHandles.ActiveEffects.CreateIterator(); HandleIt; ++HandleIt)
	{		
		RemoveActiveGameplayEffect(*HandleIt);
		HandleIt.RemoveCurrent();		
	}
}

void UPlayerCharacterGASC::HandleLimitGageEvent(AActor* InstigatorActor, bool bIsAttacker)
{
	if (!LimitHitGEClass || !LimitAttackGEClass)
		return;
	
	FGameplayEffectContextHandle Context = MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle;

	if (bIsAttacker)
	{
		// 공격 했을 때, 처리
		SpecHandle = MakeOutgoingSpec(LimitAttackGEClass, 1.f, Context);
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			UE_LOG(LogTemp, Warning, TEXT("Limit Gage Up From Attack"));
		}
	}
	else
	{
		// 피격 당했을 때 처리
		SpecHandle = MakeOutgoingSpec(LimitHitGEClass, 1.f, Context);
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			UE_LOG(LogTemp, Warning, TEXT("Limit Gage Up From Hit"));
		}
	}
}

// 기본 속성 초기화.
void UPlayerCharacterGASC::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect)
{
	checkf(AttributeEffect, TEXT("No valid default attributes for this character %s"), *GetAvatarActor()->GetName());

	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.f, ContextHandle);
	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UPlayerCharacterGASC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerCharacterGASC, InputDirection);
}


// ActorInfo 초기화.
void UPlayerCharacterGASC::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// AttributeSet 유효성 검사 (옵션)
	if (GetOwnerActor() == nullptr || GetAvatarActor() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InitAbilityActorInfo: Owner/Avatar Actor is null!"));
	}

	FGameplayTag LimitBreakReadyTag = FGameplayTag::RequestGameplayTag("State.Limit.Ready");

	RegisterGameplayTagEvent(LimitBreakReadyTag, EGameplayTagEventType::NewOrRemoved).AddLambda([this]
		(const FGameplayTag Tag, int32 NewCount)
	{
		const bool bAdded = NewCount > 0;
		OnLimitBreakTagChanged.Broadcast(Tag,bAdded);
	});

	FGameplayTag ChargeReadyTag = FGameplayTag::RequestGameplayTag("State.Ability.Charged");
	
	RegisterGameplayTagEvent(ChargeReadyTag, EGameplayTagEventType::NewOrRemoved).AddLambda([this]
	(const FGameplayTag Tag, int32 NewCount)
	{
		const bool bAdded = NewCount > 0;
		OnChargingTagChanged.Broadcast(Tag,bAdded);
	});

	FGameplayTag CooldownQTag = FGameplayTag::RequestGameplayTag("CoolDown.Skill.Sword.RainAttack");
	
	RegisterGameplayTagEvent(CooldownQTag, EGameplayTagEventType::NewOrRemoved).AddLambda([this]
	(const FGameplayTag Tag, int32 NewCount)
	{
		const bool bAdded = NewCount > 0;
		OnQSkillCooldownChanged.Broadcast(Tag,bAdded);
	});

	// 리미트 게이즈 증가할 때마다 해당 함수 실행.
	OnLimitGageChanged.BindUObject(this, &UPlayerCharacterGASC::HandleLimitGageChanged);
}

void UPlayerCharacterGASC::InitializeComponent()
{
	Super::InitializeComponent();

}

void UPlayerCharacterGASC::HandleLimitGageChanged(float NewValue)
{
	FGameplayTag ReadyTag = FGameplayTag::RequestGameplayTag("State.Limit.Ready");

	if (NewValue >= 100.0f)
	{
		if (!HasMatchingGameplayTag(ReadyTag))
		{
			if (GE_LimitBreakReady)
			{
				FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(GE_LimitBreakReady,1.f,MakeEffectContext());
				if (Spec.IsValid()) 
				{
					LimitBreakEffectHandle = ApplyGameplayEffectSpecToSelf(*Spec.Data);
				}
			}
		}
	}
	else
	{
		if (LimitBreakEffectHandle.IsValid())
		{
			RemoveActiveGameplayEffect(LimitBreakEffectHandle);
		}
	}
}


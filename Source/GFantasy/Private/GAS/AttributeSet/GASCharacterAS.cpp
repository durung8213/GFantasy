// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/AttributeSet/GASCharacterAS.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Character/GASCombatNPC.h"
#include "GAS/GamePlayTag.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GAS/AttributeSet/GASResourceAS.h"
#include "Character/GASPlayerCharacter.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"


void UGASCharacterAS::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCharacterAS, Health, OldHealth);
}

void UGASCharacterAS::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCharacterAS, MaxHealth, OldMaxHealth);
}

void UGASCharacterAS::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCharacterAS, Stamina, OldStamina);
}

void UGASCharacterAS::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCharacterAS, MaxStamina, OldMaxStamina);
}

void UGASCharacterAS::OnRep_MoveSpeed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASCharacterAS, MoveSpeed, OldSpeed);
}


void UGASCharacterAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASCharacterAS, Health, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCharacterAS, MaxHealth, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCharacterAS, Stamina, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCharacterAS, MaxStamina, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASCharacterAS, MoveSpeed, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UGASCharacterAS::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute()) {	// 바뀐 값이 Health면
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());	// 이 값을 NewValue로 해라
	}

	if (Attribute == GetStaminaAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
}

void UGASCharacterAS::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 회복 포션 마시면 피 안까이는 문제 해결책
	if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));	// 다시 Clamp했다
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute()) {
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));	// 다시 Clamp했다
	}
	
	/* Player-> Enemy 공격 : Enemy 의 피격 상태 */

	if (Data.EvaluatedData.Attribute == GetNormalAttackToEnemyAttribute())
	{
		ApplyNormalDamage(Data);
	}

	if (Data.EvaluatedData.Attribute == GetStrongAttackToEnemyAttribute())
	{
		ApplyStrongDamage(Data);
	}

	/* Enemy -> Player 공격 : Player 의 피격 상태 */

	if (Data.EvaluatedData.Attribute == GetNormalAttackToPlayerAttribute())
	{
		HandleDamage(Data, false);
	}

	if (Data.EvaluatedData.Attribute == GetStrongAttackToPlayerAttribute())
	{
		HandleDamage(Data, true);
	}
}

void UGASCharacterAS::ApplyNormalDamage(const FGameplayEffectModCallbackData& Data)
{
	float OldNormalDamage = GetNormalAttackToEnemy();
	SetNormalAttackToEnemy(0.f);
	if (OldNormalDamage > 0.f)
	{
		const float NewHealth = GetHealth() - OldNormalDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bIsDead = NewHealth <= 0.f;

		
		if (!GetOwningAbilitySystemComponent())	// 이 어트리뷰트 셋의 오너가 되는 AbilitySystemComponent가 없으면
			return;	// 종료

		if (!GetOwningAbilitySystemComponent()->GetAvatarActor())	// AbilitySystemComponent의 아바타가 없으면
			return;	// 종료
		
		if (bIsDead)
		{
			EnemyDieToNormalHitDelegate.Broadcast(Data.EffectSpec, Data.EvaluatedData, &Data.Target);

		}
		else
		{
			// 피격 당했는데 죽은게 아니라면
			
			// 리미트 게이지 증가 델리게이트 
			AActor* Attacker = Data.EffectSpec.GetContext().GetInstigator();
			if (Attacker)
			{
				if (AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(Attacker))
				{
					if (UPlayerCharacterGASC* ASC = Cast<UPlayerCharacterGASC>(Player->GetAbilitySystemComponent()))
					{
						ASC->HandleLimitGageEvent(Player, true);
					}
				}

			}
			NormalHitDelegate.Broadcast(Data.EffectSpec, Data.EvaluatedData, &Data.Target);
		}
	}
}

void UGASCharacterAS::ApplyStrongDamage(const FGameplayEffectModCallbackData& Data)
{
	// if (GetOwningAbilitySystemComponent()->HasAnyMatchingGameplayTags(FGameplayTags::Get().패링가능태그))
	// {
	// 	UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Data.Target.AbilityActorInfo->AvatarActor.Get())->TryActivateAbilitiesByTag(패링 애니메이션 실행 태그가 들어간 TagContainer);
	// 	UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Data.EffectSpec.GetContext().GetInstigator())->TryActivateAbilitiesByTag(패링 당함 태그가 들어있는 어빌리티 실행);
	//	return;
	// }
	
	float OldStrongDamage = GetStrongAttackToEnemy();
	SetStrongAttackToEnemy(0.f);
	if (OldStrongDamage > 0.f)
	{
		const float NewHealth = GetHealth() - OldStrongDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bIsDead = NewHealth <= 0.f;
		if (bIsDead)
		{
			EnemyDieToStrongHitDelegate.Broadcast(Data.EffectSpec, Data.EvaluatedData, &Data.Target);
		}
		else
		{
			// 리미트 게이지 증가 델리게이트 
			AActor* Attacker = Data.EffectSpec.GetContext().GetInstigator();
			if (Attacker)
			{
				if (AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(Attacker))
				{
					if (UPlayerCharacterGASC* ASC = Cast<UPlayerCharacterGASC>(Player->GetAbilitySystemComponent()))
					{
						ASC->HandleLimitGageEvent(Player, true);
					}
				}

			}

			StrongHitDelegate.Broadcast(Data.EffectSpec, Data.EvaluatedData, &Data.Target);
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, "SetDamage Over 0");
	}
}

// NormalDamage, StrongDamage를 받을 때 실행될 함수.
void UGASCharacterAS::HandleDamage(const FGameplayEffectModCallbackData& Data, bool bStrong)
{

	// 캐릭터가 없을 시, 종료
	if (!GetOwningAbilitySystemComponent()->GetAvatarActor())
	{
		return;
	}

	// 데미지 처리 전 무적 상태인지 체크
	if (GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag("State.Dodge.Invincible")
	))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invincible Damage From Dodge"));
		return;
	}

	// 데미지 처리 전 패링 태그 여부 체크하기.
	FGameplayTagContainer  ParryTag;
	ParryTag.AddTag(FGameplayTag::RequestGameplayTag("Ability.Parry.Window"));

	if (!ParryTag.IsValid()) return;

	if (!GetOwningAbilitySystemComponent())	return;

	UE_LOG(LogTemp, Warning, TEXT("Parrying Check"));
	

	/* 패링 기능 처리 */

	if (GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Ability.Parry.Window")))
	{
		UE_LOG(LogTemp, Warning, TEXT("ParryingStart"));

		/* 패링 태그로 패링 어빌리티를 활성화. */

		// 플레이어에게 패링 어빌리티 실행
		FGameplayTagContainer	ParryActiveTag;
		ParryActiveTag.AddTag(FGameplayTag::RequestGameplayTag("Ability.Parry.Active"));

		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			Data.Target.AbilityActorInfo->AvatarActor.Get())->
			TryActivateAbilitiesByTag(ParryActiveTag);

		// 공격자에게 패링 상태의 어빌리티를 실행
		FGameplayTagContainer ParryReceive;
		ParryReceive.AddTag(FGameplayTags::Get().State_Parry_Received);

		AActor* Debug = Data.EffectSpec.GetContext().GetInstigator();
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Debug Character: %s"), *Debug->GetName()));
		UAbilitySystemComponent* InstigatorAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Debug);
		if (InstigatorAbilitySystemComponent)
		{
			// 패링 당한 몬스터에게 패링 상태의 어빌리티 실행 ( 밀리는 모션 / VFX 실행 )
			bool bAbilitySuccess = InstigatorAbilitySystemComponent->TryActivateAbilitiesByTag(ParryReceive);
			if (!bAbilitySuccess)
			{
				UKismetSystemLibrary::PrintString(this, TEXT("Parried Ability Fail"));
			}
		}

		// 이후 데미지 처리를 진행하지 않고 바로 종료
		return;
	}

	// 데미지 처리 시작.
	float DamageApplied = Data.EvaluatedData.Magnitude;

	if (!bStrong)
	{
		SetNormalAttackToPlayer(0.f);
	}
	else
	{
		SetStrongAttackToPlayer(0.f);
	}
	
	// Health 에 반영
	const float NewHealth = GetHealth() - DamageApplied;
	SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

	const bool bIsDead = NewHealth <= 0.f;

	// 데미지 발생 시점의 Instigator 배우 찾기
	AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator();
	if (!Instigator)
		return;

	// 데미지 처리 완료된 후, 캐릭터가 죽었는지 체크
	if (bIsDead)
	{
		if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
		{
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.Dead")));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BroadCastStart"));

		// 리미트 게이지 증가 델리게이트 
		IncreaseLimitGage.Broadcast(Data.Target.AbilityActorInfo->AvatarActor.Get(), false);

		// 브로드 캐스트 : 캐릭터가 비즈니스 로직을 처리하도록 넘긴다.
		OnDamageDelegate.Broadcast(Instigator, DamageApplied, bStrong);
	}
}

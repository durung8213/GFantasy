// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystemComponent/CombatNonPlayerGASC.h"
#include "Kismet/KismetSystemLibrary.h"


void UCombatNonPlayerGASC::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UCombatNonPlayerGASC::EffectApplied);
}

void UCombatNonPlayerGASC::ActiveAbilityByTag(const FGameplayTag Tag)
{
	if (!Tag.IsValid())
	{
		return;
	}
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//UKismetSystemLibrary::PrintString(this, *AbilitySpec.Ability.GetName());
		if (AbilitySpec.Ability->AbilityTags.HasTag(Tag))
		{
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}

		// 다이나믹으로 부여한 어빌리티의 태그일 경우 가져오기인데 AI의 경우 안쓸거임 ㅎㅎ..
		// if (AbilitySpec.DynamicAbilityTags.HasTagExact(Tag))
		// {
		// 	AbilitySpecInputPressed(AbilitySpec);
		// 	if (!AbilitySpec.IsActive())
		// 	{
		// 		TryActivateAbility(AbilitySpec.Handle);
		// 	}
		// }
	}
}

bool UCombatNonPlayerGASC::GetAbilityIsActiveByTag(const FGameplayTag Tag)
{
	
	return false;
}

// 이팩트가 정상적으로 적용되면 적용되었다고 델리게이트를 Broadcast 하는 함수
void UCombatNonPlayerGASC::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                         const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;

	GameplayEffectSpec.GetAllAssetTags(TagContainer);	// 적용된 Effect가 가지고 있는 모든 태그를 가져옴

	EffectAssetsTag.Broadcast(TagContainer);
}

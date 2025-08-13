// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/Boss/BossSuccubusAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/BossArea/BossAreaBase.h"
#include "Actor/BossArea/BossPositionPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/GASCombatBossNPC.h"
#include "Character/Enemy/Boss/BossSuccubusCharacter.h"
#include "Controller/GASWorldPlayerController.h"
#include "Enum/EnemyEnumLibrary.h"
#include "GAS/GamePlayTag.h"
#include "Libraries/GASAbilitySystemLibrary.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Camera/CameraComponent.h"

class AGASCombatBossNPC;

void ABossSuccubusAIController::PlayerInArea(AGASPlayerCharacter* NewPlayer)
{
	Super::PlayerInArea(NewPlayer);
}

void ABossSuccubusAIController::PlayerOutArea(AGASPlayerCharacter* OutPlayer)
{
	Super::PlayerOutArea(OutPlayer);

	if (InAreaPlayerArr.IsEmpty())
	{
		SetEnemyState(EEnemyState::Passive);
	}
}

void ABossSuccubusAIController::BossOnDie_Implementation()
{
	if (BossLife == 1)
	{
		GetBlackboardComponent()->SetValueAsBool(FName("bStopTree"), true);
		GetBlackboardComponent()->SetValueAsInt(FName("PhaseIndex"), 1);
		ClearFocus(EAIFocusPriority::Gameplay);
		
		//UKismetSystemLibrary::PrintString(this, TEXT("1페이즈 종료"));

		UAbilitySystemComponent* PawnAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());

		FGameplayTagContainer AbilityCancle;
		AbilityCancle.AddTag(FGameplayTags::Get().Combat_Ability_Cancel);
		PawnAbilitySystemComponent->TryActivateAbilitiesByTag(AbilityCancle);
		
		//UKismetSystemLibrary::PrintString(this, TEXT("1페이즈 종료"));

		AGASCombatBossNPC* CurrentPawn = Cast<AGASCombatBossNPC>(GetPawn());
		if (CurrentPawn)
		{
			for (int PlayerNum = 0; PlayerNum < InAreaPlayerArr.Num(); PlayerNum++)
			{
				if (!CurrentPawn->BossArea) continue;
				if (!CurrentPawn->BossArea->PositionPointArr.IsValidIndex(PlayerNum)) continue;
				if (!CurrentPawn->BossArea->PositionPointArr[PlayerNum]) continue;
				if (AGASWorldPlayerController* CurrentPlayerController = Cast<AGASWorldPlayerController>( InAreaPlayerArr[PlayerNum].Player->GetController())) {
					CurrentPlayerController->Client_SwitchIMCFromSequence(false);
				}
				InAreaPlayerArr[PlayerNum].Player->SetActorLocation(CurrentPawn->BossArea->PositionPointArr[PlayerNum]->GetActorLocation());
			}
		}
		
		for (FInArrayPlayer InAreaPlayer : InAreaPlayerArr)
		{
			PlaySequence(InAreaPlayer.Player, LevelSequenceArr[0]);
		}
		
		FTimerHandle NextPhaseTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			NextPhaseTimerHandle,
			this,
			&ABossSuccubusAIController::StartPhase2,
			18.f,
			false
		);
		
		BossLife--;
	}
	else
	{
		const FGameplayTag PhaseTag = FGameplayTag::RequestGameplayTag("Combat.State.Phase2");
		UAbilitySystemComponent* CurrentPawnAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
		
		if (!CurrentPawnAbilitySystemComponent->HasMatchingGameplayTag(PhaseTag))
		{
			return;
		}
		const FGameplayTag PatternTag = FGameplayTag::RequestGameplayTag("Combat.Pattern1");
		if (!CurrentPawnAbilitySystemComponent->HasMatchingGameplayTag(PatternTag))
		{
			
			for (FInArrayPlayer Player :  InAreaPlayerArr)
			{
				if (AGASWorldPlayerController* CurrentPlayerController = Cast<AGASWorldPlayerController>( Player.Player->GetController()))
				{
					CurrentPlayerController->ClientDestoryBossUI();
					
					if (AGASWorldPlayerState* CurrentKillerPlayerState = CurrentPlayerController->GetPlayerState<AGASWorldPlayerState>())
					{
						SendToPlayerIfDie(CurrentKillerPlayerState);
					}
				}
			}
			
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FGameplayTags::Get().Combat_Dead_Dead);
			CurrentPawnAbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
			
		}
		else
		{
			UGASAbilitySystemLibrary::ApplyGASEffectToSelf(this, JustOneTickHeal);
		}
	}
}

void ABossSuccubusAIController::OnGroggyMax_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(GroggyActiveTimerHandle);
	
	AGASCombatBossNPC* CurrentBossCharacter = Cast<AGASCombatBossNPC>(GetPawn());
	if (CurrentBossCharacter)
	{
		UAbilitySystemComponent* CurrentAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
		if (CurrentAbilitySystemComponent)
		{
			if (!CurrentAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTags::Get().Combat_Pattern1_Loop) && !CurrentAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTags::Get().Combat_State_Groggy))
			{
				FGameplayTagContainer GroggyTagContainer;
				GroggyTagContainer.AddTag(FGameplayTags::Get().Combat_State_Groggy);
				CurrentAbilitySystemComponent->TryActivateAbilitiesByTag(GroggyTagContainer);
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimer(
					GroggyActiveTimerHandle,
					this,
					&ABossSuccubusAIController::OnGroggyMax_Implementation,
					0.5f,
					false
					);
			}
		}
	}

	
}

void ABossSuccubusAIController::StartPhase2()
{
	GetBlackboardComponent()->SetValueAsBool(FName("bStopTree"), false);
	GetBlackboardComponent()->SetValueAsBool(FName("bAttackDoing"), false);
	
	ABossSuccubusCharacter* CurrentCharacter = Cast<ABossSuccubusCharacter>(GetPawn());

	if (Phase2Heal)
	{
		UGASAbilitySystemLibrary::ApplyGASEffectToSelf(GetPawn(), Phase2Heal);
	}
	
	if (CurrentCharacter)
	{
		CurrentCharacter->ChangeCharacterAnimPhase(1);

		if (Phase2Effect)
		{
			UGASAbilitySystemLibrary::ApplyGASEffectToSelf(GetPawn(), Phase2Effect);
			CurrentCharacter->MultiChangeMeshTwoPhase();
		}
	}
	
	for (FInArrayPlayer InAreaPlayer : InAreaPlayerArr)
	{
		if (AGASWorldPlayerController* CurrentPlayerController = Cast<AGASWorldPlayerController>( InAreaPlayer.Player->GetController())) {
			CurrentPlayerController->Client_SwitchIMCFromSequence(true);
		}
	}
	
	ViewTargetPlayer();
	
}

void ABossSuccubusAIController::ViewTargetPlayer()
{
	for (FInArrayPlayer Player :  InAreaPlayerArr)
	{
		if (AGASWorldPlayerController* CurrentPlayerController = Cast<AGASWorldPlayerController>( Player.Player->GetController()))
		{
			CurrentPlayerController->SetViewTargetWithBlend(Player.Player);
			// TArray<UActorComponent*> Components;
			// Player.Player->GetComponents(Components);
			//
			// for (UActorComponent* Comp : Components)
			// {
			// 	if (UCameraComponent* Camera = Cast<UCameraComponent>(Comp))
			// 	{
			// 		Camera->Deactivate();
			// 	}
			// }
			//
			// Player.Player->FollowCamera->Activate();
			Player.Player->CustomizeCameraOff();
		}
	}
}

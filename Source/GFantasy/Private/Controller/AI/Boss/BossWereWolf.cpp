// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/Boss/BossWereWolf.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BrainComponent.h"
#include "Actor/BossArea/BossAreaBase.h"
#include "Actor/BossArea/BossPositionPoint.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/Enemy/Boss/BossWereWolfCharacter.h"
#include "Controller/GASWorldPlayerController.h"
#include "Enum/EnemyEnumLibrary.h"
#include "GAS/GamePlayTag.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/GASAbilitySystemLibrary.h"
#include "PlayerState/GASWorldPlayerState.h"

void ABossWereWolf::PlayerInArea(AGASPlayerCharacter* NewPlayer)
{
	Super::PlayerInArea(NewPlayer);
}

void ABossWereWolf::PlayerOutArea(AGASPlayerCharacter* OutPlayer)
{
	Super::PlayerOutArea(OutPlayer);

	if (InAreaPlayerArr.IsEmpty())
	{
		SetEnemyState(EEnemyState::Passive);
	}
}

void ABossWereWolf::BossOnDie_Implementation()
{
	if (BossLife == 1)
	{
		GetBlackboardComponent()->SetValueAsBool(FName("bStopTree"), true);
		GetBlackboardComponent()->SetValueAsInt(FName("PhaseIndex"), 1);
		ClearFocus(EAIFocusPriority::Gameplay);

		UAbilitySystemComponent* PawnAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());

		FGameplayTagContainer AbilityCancle;
		AbilityCancle.AddTag(FGameplayTags::Get().Combat_Ability_Cancel);
		PawnAbilitySystemComponent->TryActivateAbilitiesByTag(AbilityCancle);
		
		//UKismetSystemLibrary::PrintString(this, TEXT("1페이즈 종료"));

		AGASCombatBossNPC* CurrentPawn = Cast<AGASCombatBossNPC>(GetPawn());
		if (CurrentPawn)
		{
			for (FInArrayPlayer InAreaPlayer : InAreaPlayerArr)
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
					InAreaPlayerArr[PlayerNum].Player->SetActorRotation(FRotator(0, UKismetMathLibrary::MakeRotFromX(CurrentPawn->GetActorLocation()-InAreaPlayerArr[PlayerNum].Player->GetActorLocation()).Yaw, 0));  
				}
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
			&ABossWereWolf::StartPhase2,
			6.8f,
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
			
			for (FInArrayPlayer player :  InAreaPlayerArr)
			{
				 if (AGASWorldPlayerController* CurrentPlayerController = Cast<AGASWorldPlayerController>( player.Player->GetController()))
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

void ABossWereWolf::StartPhase2()
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
	
	GetBlackboardComponent()->SetValueAsBool(FName("bStopTree"), false);
	GetBlackboardComponent()->SetValueAsBool(FName("bAttackDoing"), false);
	
	ABossWereWolfCharacter* CurrentCharacter = Cast<ABossWereWolfCharacter>(GetPawn());
	

	if (Phase2Heal)
	{
		UGASAbilitySystemLibrary::ApplyGASEffectToSelf(GetPawn(), Phase2Heal);
	}
	
	if (CurrentCharacter)
	{
		CurrentCharacter->ChangeCharacterAnimPhase(1);
		CurrentCharacter->MultiChangeMeshTwoPhase();
		//CurrentCharacter->ChangeCharacterAnimInstance(Phase2AnimInstance);

		if (Phase2Effect)
		{
			UGASAbilitySystemLibrary::ApplyGASEffectToSelf(GetPawn(), Phase2Effect);
			
		}
	}
	
	for (FInArrayPlayer InAreaPlayer : InAreaPlayerArr)
	{
		if (AGASWorldPlayerController* CurrentPlayerController = Cast<AGASWorldPlayerController>( InAreaPlayer.Player->GetController())) {
			CurrentPlayerController->Client_SwitchIMCFromSequence(true);
		}
	}

	
}

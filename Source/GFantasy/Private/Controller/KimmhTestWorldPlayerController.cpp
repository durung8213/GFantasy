// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/KimmhTestWorldPlayerController.h"

#include "GameplayEffectTypes.h"
#include "Character/GASCombatBossNPC.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "HUD/KimmhTestWorldHUD.h"

// void AKimmhTestWorldPlayerController::ClientShowBossUI_Implementation(AGASCombatBossNPC* CurrentBoss)
// {
// 	AKimmhTestWorldHUD* CurrentHUD = Cast<AKimmhTestWorldHUD>(GetHUD());
// 	CurrentHUD->ShowBossHpBar(CurrentBoss);
// 	
// }
//
// void AKimmhTestWorldPlayerController::ClientDestoryBossUI_Implementation()
// {
// 	AKimmhTestWorldHUD* CurrentHUD = Cast<AKimmhTestWorldHUD>(GetHUD());
// 	CurrentHUD->HideBossHpBar();
// }
//
// void AKimmhTestWorldPlayerController::BindBossState(AGASCombatBossNPC* CurrentBoss)
// {
// 	CurrentBoss->GetAbilitySystemComponent()
// 	->GetGameplayAttributeValueChangeDelegate(
// 			CurrentBoss->GetCharacterAttributeSet()->GetHealthAttribute()
// 		).AddUObject(this, &AKimmhTestWorldPlayerController::BindBossHealth);
// 	
// 	CurrentBoss->GetAbilitySystemComponent()
// 	->GetGameplayAttributeValueChangeDelegate(
// 			CurrentBoss->GetCharacterAttributeSet()->GetMaxHealthAttribute()
// 		).AddUObject(this, &AKimmhTestWorldPlayerController::BindBossMaxHealth);
//
// 	FTimerHandle TimerHandle;
// 	GetWorld()->GetTimerManager().SetTimer(
// 		TimerHandle,
// 		FTimerDelegate::CreateLambda([this, CurrentBoss]()
// 		{
// 			
// 			InitBossState(CurrentBoss->GetCharacterAttributeSet()->GetHealth(), CurrentBoss->GetCharacterAttributeSet()->GetMaxHealth());
// 		}),
// 		2.f,
// 		false
// 	);
// }
//
// void AKimmhTestWorldPlayerController::BindBossHealth(const FOnAttributeChangeData& Data)
// {
// 	SetBossHealth(Data.NewValue);
// }
//
// void AKimmhTestWorldPlayerController::InitBossState_Implementation(float Health, float MaxHealth)
// {
// 	AKimmhTestWorldHUD* CurrentHUD = Cast<AKimmhTestWorldHUD>(GetHUD());
//
// 	if (!CurrentHUD) return;
// 	CurrentHUD->SetBossUIHealth(Health);
// 	CurrentHUD->SEtBossUIMaxHealth(MaxHealth);
// 	
// }
//
//
// void AKimmhTestWorldPlayerController::SetBossHealth_Implementation(float NewHealth)
// {	
// 	AKimmhTestWorldHUD* CurrentHUD = Cast<AKimmhTestWorldHUD>(GetHUD());
//
// 	if (!CurrentHUD) return;
// 	
// 	CurrentHUD->SetBossUIHealth(NewHealth);
// }
//
// void AKimmhTestWorldPlayerController::BindBossMaxHealth(const FOnAttributeChangeData& Data)
// {
// 	SetBossMaxHealth(Data.NewValue);
// }
//
// void AKimmhTestWorldPlayerController::SetBossMaxHealth_Implementation(float NewMaxHealth)
// {
// 	AKimmhTestWorldHUD* CurrentHUD = Cast<AKimmhTestWorldHUD>(GetHUD());
//
// 	if (!CurrentHUD) return;
// 	CurrentHUD->SEtBossUIMaxHealth(NewMaxHealth);
// }
//

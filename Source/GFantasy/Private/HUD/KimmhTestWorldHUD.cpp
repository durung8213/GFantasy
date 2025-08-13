// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/KimmhTestWorldHUD.h"

#include "Blueprint/UserWidget.h"
#include "Character/GASCombatBossNPC.h"
#include "UI/Enemy/Boss/BossStateUI.h"

//
// void AKimmhTestWorldHUD::ShowBossHpBar(AGASCombatBossNPC* BossCharacter)
// {
// 	BossStateUI = CreateWidget<UBossStateUI>(GetOwningPlayerController(), BossStateUIClass);
// 	BossStateUI->AddToViewport();
// 	//BossStateUI->BindBossState(BossCharacter->GetAbilitySystemComponent(), BossCharacter->GetCharacterAttributeSet(), BossCharacter->GetGroggyAttributeSet(), BossCharacter->MonsterName);
// }
//
// void AKimmhTestWorldHUD::SetBossUIHealth(float NewHealth)
// {
// 	if (!BossStateUI) return;
// 	BossStateUI->SetBossHealth2(NewHealth);
// }
//
// void AKimmhTestWorldHUD::SEtBossUIMaxHealth(float NewMaxHealth)
// {
// 	if (!BossStateUI) return;
// 	BossStateUI->SetBossMaxHealth2(NewMaxHealth);
// }
//
// void AKimmhTestWorldHUD::HideBossHpBar()
// {
// 	if (!BossStateUI) return;
// 	
// 	BossStateUI->RemoveFromParent();
// 	BossStateUI = nullptr;
// }

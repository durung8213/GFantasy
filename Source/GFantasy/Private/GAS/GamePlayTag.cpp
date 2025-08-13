// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GamePlayTag.h"
#include "GameplayTagsManager.h"

FGameplayTags FGameplayTags::GameplayTags;

void FGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Input_Mouse_LMBOnClick = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Mouse.LMBOnClick"), FString("좌클릭"));
	GameplayTags.Input_Mouse_RMBOnClick = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Mouse.RMBOnClick"), FString("우클릭"));
	GameplayTags.PlayerAttack_GreatSword_Combo1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("PlayerAttack.GreatSword.Combo1"), FString("콤보 1"));
	GameplayTags.PlayerAttack_GreatSword_Combo2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("PlayerAttack.GreatSword.Combo2"), FString("콤보 2"));
	GameplayTags.PlayerAttack_GreatSword_Combo3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("PlayerAttack.GreatSword.Combo3"), FString("콤보 2"));
	GameplayTags.PlayerAttack_Request_NextComboAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("PlayerAttack.Request.NextComboAttack"), FString("다음 콤보를 행동하는 것을 요청할 때 사용하는 태그"));
	GameplayTags.PlayerAttack_Combo_CanNextComboTrue = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("PlayerAttack.Combo.CanNextComboTrue"), FString("콤보가 가능하다고 알리는 태그"));
	GameplayTags.PlayerAttack_Combo_CanNextComboFalse = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("PlayerAttack.Combo.CanNextComboFalse"), FString("콤보가 불가능하다고 알리는 태그"));
	GameplayTags.Enemy_Attack_Combo1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Enemy.Attack.Combo1"), FString("적 콤보용"));
	GameplayTags.Combat_State_Groggy = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.State.Groggy"), FString("그로기 상태"));
	GameplayTags.Combat_State_Phase1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.State.Phase1"), FString("페이즈 1"));
	GameplayTags.Combat_State_Phase2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.State.Phase2"), FString("페이즈 2"));
	GameplayTags.Combat_State_Phase3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.State.Phase3"), FString("페이즈 3"));
	GameplayTags.Combat_Pattern1_Start = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern1.Start"), FString("패턴 1 시작"));
	GameplayTags.Combat_Pattern1_Loop = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern1.Loop"), FString("패턴 1 하는 중"));
	GameplayTags.Combat_Pattern1_End = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern1.End"), FString("패턴 1 끝"));
	GameplayTags.Combat_Pattern1_End_TargetDie = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern1.End.TargetDie"), FString("패턴 1 끝 타겟 죽음"));
	GameplayTags.Combat_Pattern1_End_TargetAlive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern1.End.TargetAlive"), FString("패턴 2 끝 타겟 생존"));
	GameplayTags.Combat_Pattern2_Start = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern2.Start"), FString("패턴 2 시작"));
	GameplayTags.Combat_Pattern2_Loop = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern2.Loop"), FString("패턴 2 하는 중"));
	GameplayTags.Combat_Pattern2_End = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Pattern2.End"), FString("패턴 2 끝"));

	GameplayTags.Enemy_Boss_ExtraGaugeFull = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Enemy.Boss.ExtraGaugeFull"), FString("액스트라 게이지 다 참"));

	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack1_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Attack1_1"), FString("웨어 울프 1페이즈 공격 1_1 사운드"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack1_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Attack1_2"), FString("웨어 울프 1페이즈 공격 1_2 사운드"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack2_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Attack2_1"), FString("웨어 울프 1페이즈 공격 2_1 사운드"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack2_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Attack2_2"), FString("웨어 울프 1페이즈 공격 2_2 사운드"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_PlayerHit1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.PlayerHit1"), FString("웨어 울프 1페이즈 공격에 대한 플레이어 피격 사운드 1"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_PlayerHit2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.PlayerHit2"), FString("웨어 울프 1페이즈 공격에 대한 플레이어 피격 사운드 2"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_PlayerHit3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.PlayerHit3"), FString("웨어 울프 1페이즈 공격에 대한 플레이어 피격 사운드 3"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1Start = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Pattern1Start"), FString("웨어 울프 1페이즈 패턴 1 시작"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1WalkLoop = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Pattern1WalkLoop"), FString("웨어 울프 1페이즈 패턴 1 시작 발구름 소리"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1Hit = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Pattern1Hit"), FString("웨어 울프 1페이즈 패턴 1 공격"));
	GameplayTags.GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1PlayerHit = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Sound.Enemy.Werewolf.Phase1.Pattern1PlayerHit"), FString("웨어 울프 1페이즈 패턴 1 플레이어 맞음"));

	GameplayTags.Combat_Ability_Cancel = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Ability.Cancel"), FString("웨어 울프 1페이즈 패턴 1 플레이어 맞음"));

	/*
	// 퀘스트 진행방식
	// 일단 이렇게 했는데 퀘스트큰 진행방식은 Enum을 이용하고, 세부사항(어떤 몬스터인지 등)을 Tag로 해줄거라서 수정 필요
	GameplayTags.Quest_Type_Interaction = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.Type.Interacion"), FString("말 걸기 퀘스트"));
	GameplayTags.Quest_Type_Collection = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.Type.Collection"), FString("수집 퀘스트"));
	GameplayTags.Quest_Type_KillCount = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.Type.KillCount"), FString("사냥 퀘스트"));
	GameplayTags.Quest_Type_Delivery = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.Type.Delivery"), FString("전달 퀘스트"));
	
	// 퀘스트 진행 상황
	// 시작가능/진행중/완료
	GameplayTags.Quest_State_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.State.Locked"), FString("시작불가능한 퀘스트"));
	GameplayTags.Quest_State_Available = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.State.Available"), FString("시작가능한 퀘스트"));
	GameplayTags.Quest_State_InProgress = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.State.InProgress"), FString("진행중인 퀘스트"));
	GameplayTags.Quest_State_Completed = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Quest.State.Completed"), FString("완료된 퀘스트"));

	GameplayTags.Combat_Damage_NormalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Damage.NormalDamage"), FString("평타"));
	GameplayTags.Combat_Damage_StrongDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Damage.StrongDamage"), FString("강공격"));
	*/
	GameplayTags.Combat_Hit_HitReactFront = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Hit.HitReactFront"), FString("피격"));
	GameplayTags.Combat_Hit_HitReactBack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Hit.HitReactBack"), FString("피격"));
	GameplayTags.Combat_Hit_HitReactRight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Hit.HitReactRight"), FString("피격"));
	GameplayTags.Combat_Hit_HitReactLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Hit.HitReactLeft"), FString("피격"));
	GameplayTags.Combat_Hit_StrongHitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Hit.StrongHitReact"), FString("강피격"));
	GameplayTags.Combat_Hit_StrongHitReactFront = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Hit.StrongHitReactFront"), FString("강피격 앞"));
	GameplayTags.Combat_Hit_StrongHitReactBack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Hit.StrongHitReactBack"), FString("강피격 뒤"));
	GameplayTags.Combat_Dead_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Combat.Dead.Dead"), FString("강피격 뒤"));
	GameplayTags.State_Parry_Received = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.Parry.Received"), FString("패링 당함"));
}




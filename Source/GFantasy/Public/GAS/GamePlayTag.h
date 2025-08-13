// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**#include "GameplayTagContainer.h"
/**
 *	It Must To Singleton / 반드시 절대적으로 싱글톤으로 유지해야 한다
 *	You can create the tags to be used in the code here / 코드에서 사용할 태그는 전부 이곳에서 생성해야만 합니다 (태그 불러오는 함수를 씀으로써 발생하는 비용 방지)
 */
struct FGameplayTags
{
public:
	static const FGameplayTags& Get()	// 위의 GameplayTags를 참조로 반환하는 정적 메서드 (접근용)
	{
		return GameplayTags;
	}

	static void InitializeNativeGameplayTags();	// Native Gameplay Tag들을 등록하는 초기화 함수

	FGameplayTag Attribute_Secondary_Armor;	// 실제 사용할 태그의 Armor변수

	//			Input Tags				//
	FGameplayTag Input_Mouse_LMBOnClick;
	FGameplayTag Input_Mouse_RMBOnClick;

	//			Input Tags End			//

	//			Normal Attack Tags		//
	FGameplayTag PlayerAttack_Request_NextComboAttack;
	FGameplayTag PlayerAttack_Combo_CanNextComboTrue;
	FGameplayTag PlayerAttack_Combo_CanNextComboFalse;
	FGameplayTag PlayerAttack_GreatSword_Combo1;
	FGameplayTag PlayerAttack_GreatSword_Combo2;
	FGameplayTag PlayerAttack_GreatSword_Combo3;
	//			Normal Attack Tags End	//

	//			Enemy Attack Tags		//
	FGameplayTag Enemy_Attack_Combo1;
	//FGameplayTag PlayerAttack_GreatSword_Combo3;
	//FGameplayTag PlayerAttack_GreatSword_Combo3;
	
	//			Enemy Attack Tags End	//

	//			Combat					//
	FGameplayTag Combat_Damage_NormalDamage;
	FGameplayTag Combat_Damage_StrongDamage;
	FGameplayTag Combat_Hit_HitReactFront;
	FGameplayTag Combat_Hit_HitReactBack;
	FGameplayTag Combat_Hit_HitReactRight;
	FGameplayTag Combat_Hit_HitReactLeft;
	FGameplayTag Combat_Hit_StrongHitReact;
	FGameplayTag Combat_Hit_StrongHitReactFront;
	FGameplayTag Combat_Hit_StrongHitReactBack;
	
	FGameplayTag Combat_Dead_Dead;

	FGameplayTag State_Parry_Received;

	FGameplayTag Combat_State_Groggy;

	FGameplayTag Combat_State_Phase1;
	FGameplayTag Combat_State_Phase2;
	FGameplayTag Combat_State_Phase3;

	FGameplayTag Combat_Pattern1_Start;
	FGameplayTag Combat_Pattern1_Loop;
	FGameplayTag Combat_Pattern1_End;
	FGameplayTag Combat_Pattern1_End_TargetDie;
	FGameplayTag Combat_Pattern1_End_TargetAlive;

	FGameplayTag Combat_Pattern2_Start;
	FGameplayTag Combat_Pattern2_Loop;
	FGameplayTag Combat_Pattern2_End;
	FGameplayTag Combat_Ability_Cancel;

	FGameplayTag Enemy_Boss_ExtraGaugeFull;

	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack1_1;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack1_2;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack2_1;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Attack2_2;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1Start;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1Hit;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1PlayerHit;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_Pattern1WalkLoop;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_PlayerHit1;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_PlayerHit2;
	FGameplayTag GameplayCue_Sound_Enemy_Werewolf_Phase1_PlayerHit3;
	
	//// ================ Quest ===================
	//FGameplayTag Quest_Type_Interaction;
	//FGameplayTag Quest_Type_Collection;
	//FGameplayTag Quest_Type_KillCount;
	//FGameplayTag Quest_Type_Delivery;

	//FGameplayTag Quest_State_Locked;
	//FGameplayTag Quest_State_Available;
	//FGameplayTag Quest_State_InProgress;
	//FGameplayTag Quest_State_Completed;

	//FGameplayTag Quest_Action_Accept;
	//FGameplayTag Quest_Action_Complete;

protected:
	static FGameplayTags GameplayTags;	// 정적 인스턴스, 싱글턴처럼 쓰기 위해 사용
};
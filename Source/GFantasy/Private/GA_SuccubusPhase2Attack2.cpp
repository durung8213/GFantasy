// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SuccubusPhase2Attack2.h"

FVector UGA_SuccubusPhase2Attack2::GetSickleAuraSpawnLocation(float Distance)
{
	FVector BossLocation =  GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector BossForwardLocation = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	return BossLocation + BossForwardLocation * Distance;
		
}

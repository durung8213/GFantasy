// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayAbility/Enemy/GA_SuccubusPhase2Attack3.h"


FVector UGA_SuccubusPhase2Attack3::GetAttackSpawnLocation(int Count)
{
	AActor* BossActor = GetAvatarActorFromActorInfo();
	if (!BossActor) return FVector::ZeroVector;
	
	FVector BossForwardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	float Noise;

	if (Count % 2 == 0)
	{
		Noise = -200.f;
	}
	else
	{
		Noise = 200.f;
	}
	
	// 소환 위치(현재 위치 + (앞방향 * 거리 * 소환 개수) + 
	return BossActor->GetActorLocation()
	+ (Count * AttackDecalDistance * BossForwardVector)
	+ BossActor->GetActorRightVector() * Noise;
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/PlayerSkill/GA_BaseAOE.h"
#include "Abilities/Tasks/AbilityTask_SpawnActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Weapon/PlayerAOEBase.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_BaseAOE::UGA_BaseAOE()
{
	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UGA_BaseAOE::SpawnAOEActorAtLocation(const FHitResult& Hit)
{
	
	UKismetSystemLibrary::PrintString(this, TEXT("SpawnAOEActor"));

	if (HasAuthority(&CurrentActivationInfo))
	{
		const FTransform Transform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint);
		FActorSpawnParameters Params;
		Params.Owner = GetAvatarActorFromActorInfo();
		Params.Instigator = Cast<APawn>(Params.Owner);
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APlayerAOEBase* AOE = GetWorld()->SpawnActorDeferred<APlayerAOEBase>(
			AOEActorClass,
			Transform,
			Params.Owner,
			Params.Instigator,
			Params.SpawnCollisionHandlingOverride
		);

		if (AOE)
		{
			OnAOESpawnedSuccess(AOE);
		}
	}
}

void UGA_BaseAOE::SpawnAOEActorAtLocation(const FVector& Location)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		const FTransform Transform(FRotator::ZeroRotator, Location);

		FActorSpawnParameters Params;
		Params.Owner = GetAvatarActorFromActorInfo();
		Params.Instigator = Cast<APawn>(Params.Owner);
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 생성하지 않고 미리 겉만 생성해둔다. 
		// 구성만 완료한 상태로 일시적으로 생성하고.
		APlayerAOEBase* AOE = GetWorld()->SpawnActorDeferred<APlayerAOEBase>(
			AOEActorClass,
			Transform,
			Params.Owner,
			Params.Instigator,
			Params.SpawnCollisionHandlingOverride
		);
		
		if (AOE)
		{
			//
			OnAOESpawnedSuccess(AOE);
		}
	}
}

// AOE 생성이 성공했을 때 호출되는 함수
void UGA_BaseAOE::OnAOESpawnedSuccess(AActor* SpawnedActor)
{
	UKismetSystemLibrary::PrintString(this, TEXT("In Spawn Ability"));
	APlayerAOEBase* AOE = Cast<APlayerAOEBase>(SpawnedActor);
	if (!AOE)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("No AOE Spawn"));
		return;
	}

	// 기존 Initialize로 AOE 세팅
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("AOE Spawn Setting"));
		AOE->Initialize(ASC, 
			AOE->DamageGE, 
			AOE->DamageRadius, 
			AOE->NiagaraFX, 
			AOE->TotalDamageCount, 
			AOE->DamageInterval);
	}

	UKismetSystemLibrary::PrintString(this, TEXT("SpawnOver"));

	// AOE의 기본값을 초기화 한 다음에 생성한다.
	// 스폰된 액터를 마무리 ( beginplay 호출 )
	AOE->FinishSpawning(AOE->GetTransform());

}

void UGA_BaseAOE::OnAOESpawnedFailed(AActor* SpawnedActor)
{
	UE_LOG(LogTemp, Warning, TEXT("GA_BaseAOE : AOE Spawn Fail"));
}

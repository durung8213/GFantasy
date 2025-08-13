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

		// �������� �ʰ� �̸� �Ѹ� �����صд�. 
		// ������ �Ϸ��� ���·� �Ͻ������� �����ϰ�.
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

// AOE ������ �������� �� ȣ��Ǵ� �Լ�
void UGA_BaseAOE::OnAOESpawnedSuccess(AActor* SpawnedActor)
{
	UKismetSystemLibrary::PrintString(this, TEXT("In Spawn Ability"));
	APlayerAOEBase* AOE = Cast<APlayerAOEBase>(SpawnedActor);
	if (!AOE)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("No AOE Spawn"));
		return;
	}

	// ���� Initialize�� AOE ����
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

	// AOE�� �⺻���� �ʱ�ȭ �� ������ �����Ѵ�.
	// ������ ���͸� ������ ( beginplay ȣ�� )
	AOE->FinishSpawning(AOE->GetTransform());

}

void UGA_BaseAOE::OnAOESpawnedFailed(AActor* SpawnedActor)
{
	UE_LOG(LogTemp, Warning, TEXT("GA_BaseAOE : AOE Spawn Fail"));
}

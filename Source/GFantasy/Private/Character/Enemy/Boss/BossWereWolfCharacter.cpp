// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Boss/BossWereWolfCharacter.h"

#include "AnimInstance/BossEnemyAnimInstanceBase.h"
#include "Controller/AI/Boss/BossWereWolf.h"
#include "Kismet/KismetSystemLibrary.h"

void ABossWereWolfCharacter::MultiRotateActor_Implementation(AActor* Target, FRotator TargetRotator)
{
	if (Target)
	{
		Target->SetActorRotation(TargetRotator);
	}
	
}

void ABossWereWolfCharacter::MultiSetLocationActor_Implementation(AActor* Target, FVector TargetVector)
{
	if (Target)
	{
		Target->SetActorLocation(TargetVector);
	}
	
}

void ABossWereWolfCharacter::OnNormalAttackDie(const FGameplayEffectSpec& EffectSpec,
                                               const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	
	ABossWereWolf* CurrentController = Cast<ABossWereWolf>(GetController());
	if (CurrentController)
	{
		CurrentController->BossOnDie();
	}
}

void ABossWereWolfCharacter::OnStrongAttackDie(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	ABossWereWolf* CurrentController = Cast<ABossWereWolf>(GetController());
	if (CurrentController)
	{
		CurrentController->BossOnDie();
	}
}

void ABossWereWolfCharacter::SetIsDead(bool NewbIsDead)
{
	bIsDead = NewbIsDead;
}

bool ABossWereWolfCharacter::GetIsDead()
{
	return bIsDead;
}

void ABossWereWolfCharacter::MultiChangeMeshTwoPhase_Implementation()
{
	ChangeMeshTwoPhase();
}

void ABossWereWolfCharacter::ChangeCharacterAnimPhase_Implementation(int Phase)
{
	//GetRootComponent()->SetWorldScale3D(FVector(1.3f, 1.3f, 1.3f));
	if (UBossEnemyAnimInstanceBase* CurrentAnimInstance = Cast<UBossEnemyAnimInstanceBase>(GetMesh()->GetAnimInstance()))
	{
		CurrentAnimInstance->Phase = Phase;
	}
}

void ABossWereWolfCharacter::ChangeCharacterAnimInstance_Implementation(TSubclassOf<UAnimInstance> NewAnimInstance)
{
	if (Phase2AnimInstance)
	{
		//GetMesh()->HandleExistingParallelEvaluationTask(true, true);
		if (HasAuthority()) return;
		GetMesh()->SetAnimInstanceClass(Phase2AnimInstance);
		
		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("애님 인스턴스 %s 바뀌어야 함 그리고 현재 애님 인스턴스 : %s"), *NewAnimInstance->GetName(), *GetMesh()->GetAnimInstance()->GetClass()->GetName()));
	}
}


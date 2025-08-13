// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "NiagaraSystem.h"
#include "AbilitySystemInterface.h"
#include "PlayerAOEBase.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UAbilitySystemComponent;


UCLASS(Abstract)
class GFANTASY_API APlayerAOEBase : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerAOEBase();
	
	// AOE �ʱ�ȭ
	void Initialize(
		UAbilitySystemComponent* InInstigatorASC,
		TSubclassOf<UGameplayEffect> InDamageGE,
		float InRadius,
		UNiagaraSystem* InNiagaraFX,
		int32 InTotalCount,
		float InInterval);

	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;

	// ������ �߻� ��, ������ ����
	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_ActivateVFX();
	
	
	// ���ݰ� Ƚ���� ���� overlap Timer���� �ݺ� ����� �Լ�
	void ApplyAOEDamage();

public:	
	// instigator ASC ( ��ų �������� ASC )
	UPROPERTY()
	UAbilitySystemComponent* InstigatorASC;

	// Sphere ���� �ݸ���
	UPROPERTY(VisibleAnywhere, Category = "AOE")
	USphereComponent* SphereComp;

	// �ٴ� ����Ʈ ����� Niagara Comp
	UPROPERTY(VisibleAnywhere, Category = "AOE")
	UNiagaraComponent* FXSystem;

	// �������� ����� Effect
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	TSubclassOf<UGameplayEffect> DamageGE;

	// AOE �ݰ�
	UPROPERTY(EditDefaultsOnly, Category =  "AOE")
	float DamageRadius = 100.f;

	// �ٴڿ� �Ѹ� Niagara ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	UNiagaraSystem* NiagaraFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	int32 TotalDamageCount = 1;	// �� ������ Ƚ��

	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	float DamageInterval = 0.3f;	// �ݺ� ����

	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	float SkillRatio = 1.0f;	// ��ų ��� ��
private:
	FTimerHandle DamageTH;

	TSet<AActor*> DamagedActors;
	
	// ���� �������� �� Ƚ��
	int32 CurrentDamageCount = 0;

	// ������ �ִ� Ÿ�̸� �ڵ�
	FTimerHandle MultiDamageTimerHandle;

	
};

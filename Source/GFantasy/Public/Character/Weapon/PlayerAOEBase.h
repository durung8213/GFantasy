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
	
	// AOE 초기화
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

	// 오버랩 발생 시, 데미지 로직
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
	
	
	// 간격과 횟수에 따라 overlap Timer에서 반복 실행될 함수
	void ApplyAOEDamage();

public:	
	// instigator ASC ( 스킬 소유자의 ASC )
	UPROPERTY()
	UAbilitySystemComponent* InstigatorASC;

	// Sphere 범위 콜리전
	UPROPERTY(VisibleAnywhere, Category = "AOE")
	USphereComponent* SphereComp;

	// 바닥 이펙트 재생용 Niagara Comp
	UPROPERTY(VisibleAnywhere, Category = "AOE")
	UNiagaraComponent* FXSystem;

	// 데미지로 사용할 Effect
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	TSubclassOf<UGameplayEffect> DamageGE;

	// AOE 반경
	UPROPERTY(EditDefaultsOnly, Category =  "AOE")
	float DamageRadius = 100.f;

	// 바닥에 뿌릴 Niagara 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	UNiagaraSystem* NiagaraFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	int32 TotalDamageCount = 1;	// 총 데미지 횟수

	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	float DamageInterval = 0.3f;	// 반복 간격

	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	float SkillRatio = 1.0f;	// 스킬 계수 값
private:
	FTimerHandle DamageTH;

	TSet<AActor*> DamagedActors;
	
	// 현재 데미지를 준 횟수
	int32 CurrentDamageCount = 0;

	// 데미지 주는 타이머 핸들
	FTimerHandle MultiDamageTimerHandle;

	
};

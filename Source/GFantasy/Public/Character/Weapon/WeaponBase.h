// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentActor.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "WeaponBase.generated.h"

class UGameplayEffect;
class UNiagaraComponent;

UCLASS()
class AWeaponBase : public AEquipmentActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	// 콜리전 오버랩 시 실행될 함수
	UFUNCTION()
	void OnHitCollisionOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetWeaponSocketTransform(FName SocketName, FTransform& OutTransform) const;

	virtual void BeginPlay() override;

	virtual void OnRep_Owner() override; 
	
	virtual void Tick(float DeltaTime) override;

	// 충돌 트리거
	void EnableHitCollision();
	void DisableHitCollision();

	// 나이아가라 VFX 켜기
	UFUNCTION(BlueprintCallable)
	void ActivateTrail();

	UFUNCTION(BlueprintCallable)
	void DeactivateTrail();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachWeapon(USkeletalMeshComponent* Mesh,
		FName SocketName);
	
protected:

	// 무기 소유
	virtual void SetWeaponOwner(AActor* NewOwner);
	FORCENOINLINE AActor* GetWeaponOwner() const { return WeaponOwner; }

	// 데미지 적용
	virtual void ApplyDamageTo(AActor* TargetActor,  const FHitResult& HitResult);


public:	
	// 무기 Tag
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tags")
	//FGameplayTag WeaponTag;

	// 무기 Static Mesh 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="VFX")
	UNiagaraComponent* TrailVFX;
	
protected:

	// 공통 변수
	UPROPERTY()
	AActor* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// 충돌 체크할 콜리전
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* HitCollision;
	
	// overlap 중복 체크 방지
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* HitEffectPoint;
	
};

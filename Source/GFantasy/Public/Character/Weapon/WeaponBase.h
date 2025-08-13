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

	// �ݸ��� ������ �� ����� �Լ�
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

	// �浹 Ʈ����
	void EnableHitCollision();
	void DisableHitCollision();

	// ���̾ư��� VFX �ѱ�
	UFUNCTION(BlueprintCallable)
	void ActivateTrail();

	UFUNCTION(BlueprintCallable)
	void DeactivateTrail();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachWeapon(USkeletalMeshComponent* Mesh,
		FName SocketName);
	
protected:

	// ���� ����
	virtual void SetWeaponOwner(AActor* NewOwner);
	FORCENOINLINE AActor* GetWeaponOwner() const { return WeaponOwner; }

	// ������ ����
	virtual void ApplyDamageTo(AActor* TargetActor,  const FHitResult& HitResult);


public:	
	// ���� Tag
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tags")
	//FGameplayTag WeaponTag;

	// ���� Static Mesh 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="VFX")
	UNiagaraComponent* TrailVFX;
	
protected:

	// ���� ����
	UPROPERTY()
	AActor* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// �浹 üũ�� �ݸ���
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* HitCollision;
	
	// overlap �ߺ� üũ ����
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* HitEffectPoint;
	
};

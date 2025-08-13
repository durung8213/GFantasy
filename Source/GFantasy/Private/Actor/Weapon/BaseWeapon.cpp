// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/BaseWeapon.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "Character/GASCombatNPC.h"
#include "Components/PrimitiveComponent.h"
#include "GAS/AbilitySystemComponent/GASCBase.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*
	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(FName("WeaponVFX"));
	TrailVFX->SetupAttachment(RootComponent);
	*/
}

void ABaseWeapon::GetOwnerActor()
{
	AActor* ParentActor = GetParentActor();
	if (ParentActor)
	{
		// 부모 액터가 있고
		IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(GetParentActor());
		if (AbilityInterface)
		{
			// 부모 액터에 IAbilitySystemInterface 인터페이스가 있다면
			OwnerActor = ParentActor;
			OwnerActorASC = Cast<UGASCBase>(AbilityInterface->GetAbilitySystemComponent());
		}
	}
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	GetOwnerActor();
}

void ABaseWeapon::OnBeginOverlap(AActor* OtherActor)
{
	if (OtherActor == OwnerActor || Cast<AGASCombatNPC>(OtherActor))
	{
		return;
	}
	
	if (!HitActors.Contains(OtherActor))
	{
		EnemyHit(OtherActor);
	}
}

void ABaseWeapon::EnemyHit(AActor* OtherActor)
{
	IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OtherActor);

	if (AbilityInterface)
	{
		if (UAbilitySystemComponent* OtherActorASC = AbilityInterface->GetAbilitySystemComponent())
		{
			// 피격 Effect를 적용시키기
			//UKismetSystemLibrary::PrintString(this, OtherActor->GetName());
			if (AttackEffect.IsValidIndex(AttackEffectIndex) && AttackEffect[AttackEffectIndex])
			{
				// 1. 효과 컨텍스트 생성
				FGameplayEffectContextHandle EffectContext = OwnerActorASC->MakeEffectContext();
				EffectContext.AddInstigator(OwnerActorASC->GetOwner(), OwnerActorASC->GetOwner());
				EffectContext.AddSourceObject(OwnerActorASC->GetOwner());

				// 2. 효과 스펙 생성
				FGameplayEffectSpecHandle SpecHandle = OwnerActorASC->MakeOutgoingSpec(
					AttackEffect[AttackEffectIndex],
					1.0f, // 레벨
					EffectContext
				);

				if (SpecHandle.IsValid())
				{
					FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
					if (Spec)
					{
						
						FActiveGameplayEffectHandle EffectHandle = OwnerActorASC->ApplyGameplayEffectSpecToTarget(*Spec, OtherActorASC);
						if (EffectHandle.IsValid())
						{
							//UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
						}
						else
						{
							//UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true, FLinearColor::Red);
						}
					}
					else
					{
						//UKismetSystemLibrary::PrintString(this, TEXT("Spec 없음"), true, true, FLinearColor::Red);
					}
				}
				else
				{
					//UKismetSystemLibrary::PrintString(this, TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
				}
			}
			else
			{
				//UKismetSystemLibrary::PrintString(this, TEXT("AttackEffect 없음"), true, true, FLinearColor::Red);
			}
		}
		else
		{
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("피격 액터 %s가 ASC 없음"), *OtherActor->GetName()), true, true, FLinearColor::Red);
		}
	}
	else
	{
		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("피격 액터 %s가 ASC 인터페이스 없음"), *OtherActor->GetName()), true, true, FLinearColor::Red);
	}
}

FTransform ABaseWeapon::GetWeaponSocketWorldTransform(FName Socket)
{
	return FTransform();
}

void ABaseWeapon::ClearHitActorsArr()
{
	HitActors.Empty();
}

void ABaseWeapon::SetCollisionActive()
{
	for (UPrimitiveComponent* Collision : HitComponents)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ABaseWeapon::SetCollisionInactive()
{
	for (UPrimitiveComponent* Collision : HitComponents)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::ActivateTrail()
{
	if (TrailVfx)
	{
		TrailVfx->Activate();
	}
}

void ABaseWeapon::DeactivateTrail()
{
	if (TrailVfx)
	{
		TrailVfx->Deactivate();
	}
}
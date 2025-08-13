// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Weapon/WeaponBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "GameplayEffectTypes.h"           // FGameplayEffectContextHandle
#include "Character/GASPlayerCharacter.h"
// Sets default values
AWeaponBase::AWeaponBase()
{

	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = SceneRoot;

	HitCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitCollision"));
	HitCollision->SetupAttachment(RootComponent);
	HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(HitCollision);

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(FName("TrailVFX"));
	TrailVFX->SetupAttachment(RootComponent);

	HitEffectPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HitEffectPoint"));
	HitEffectPoint->SetupAttachment(WeaponMesh);	// Į �� ���Ͽ� ���̱�.
	
	bReplicates = true;
}

//  �ݸ��� ������ ���� �� ����Ǵ� �Լ�
void AWeaponBase::OnHitCollisionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}
	UKismetSystemLibrary::PrintString(this, TEXT("Overlap Start"));
	if (!OtherActor || OtherActor == GetOwner()|| OtherActor == this)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("No OtherActor"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("HIT ACTOR : %s"), *OtherActor->GetName());

	AActor* TrueTarget = OtherComp->GetOwner();

	if (!TrueTarget || AlreadyHitActors.Contains(TrueTarget))
	{
		return;
	}
	
	AlreadyHitActors.AddUnique(TrueTarget);

	FHitResult Hit;
	FVector Start = HitEffectPoint->GetComponentLocation();
	FVector End = OtherActor->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(Hit, Start,End, ECC_Pawn, Params);

	
	// �ش� ���Ϳ��� Effect �ֱ�.
	ApplyDamageTo(OtherActor, Hit);
}

bool AWeaponBase::GetWeaponSocketTransform(FName SocketName, FTransform& OutTransform) const
{
	if (WeaponMesh && WeaponMesh->DoesSocketExist(SocketName))
	{
		OutTransform = WeaponMesh->GetSocketTransform(SocketName, RTS_World);
		return true;
	}

	return false;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnHitCollisionOverlap);
}

void AWeaponBase::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetOwner()))
	{
		Player->CurrentWeapon = this;
		UE_LOG(LogTemp, Warning, TEXT("WeaponBase :: OnREP_CurrentWeapon"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponBase :: Owner is Not AGASPlayer"));
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AWeaponBase::SetWeaponOwner(AActor* NewOwner)
{
	WeaponOwner = NewOwner;
	SetOwner(WeaponOwner);
}

void AWeaponBase::EnableHitCollision()
{
	if (HasAuthority())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("CollisionOn"));
		HitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AWeaponBase::DisableHitCollision()
{
	if (HasAuthority())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Disable"));
		HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// �迭 �ʱ�ȭ
		AlreadyHitActors.Empty();
	}
}

void AWeaponBase::ActivateTrail()
{
	if (TrailVFX)
	{
		TrailVFX->Activate();
		UKismetSystemLibrary::PrintString(this, TEXT("Trail On"));
	}
}

void AWeaponBase::DeactivateTrail()
{
	if (TrailVFX)
	{
		TrailVFX->Deactivate();
		UKismetSystemLibrary::PrintString(this, TEXT("Trail Off"));
	}
}

void AWeaponBase::Multicast_AttachWeapon_Implementation(USkeletalMeshComponent* Mesh,
	FName SocketName)
{
	if (Mesh)
	{
		this->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}


// ������ ������ �ֱ�
void AWeaponBase::ApplyDamageTo(AActor* TargetActor,  const FHitResult& HitResult)
{
	UKismetSystemLibrary::PrintString(this, TEXT("Damage Start"));
	// ������ �ƴ϶�� ����
	if (!TargetActor)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("1"));
		return;
	}
	else if (!DamageEffect)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("2"));
		return;
	}
	else if (!HasAuthority())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("3"));
		return;
	}

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (DamageEffect)
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				Context.AddSourceObject(this);

				UAbilitySystemComponent* PlayerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
				Context.AddInstigator(PlayerASC->GetAvatarActor(), PlayerASC->GetAvatarActor());
				
				Context.AddHitResult(HitResult, true);
				FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(
					DamageEffect, 1.f, Context);

				if (ASC && Spec.IsValid() && Spec.Data.IsValid())
				{
					Spec.Data->SetSetByCallerMagnitude(
					FGameplayTag::RequestGameplayTag("Data.SkillRatio"),
					1.0f);
					PlayerASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
				}

			}
		}
	}

}

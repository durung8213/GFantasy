// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Weapon/RangedProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/GASPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
ARangedProjectile::ARangedProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	// �ݸ��� �浹 ����
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
	// �浹�� ���ε��� �Լ�
	Collision->OnComponentHit.AddDynamic(this, &ARangedProjectile::OnHit);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 5.0f;
}

void ARangedProjectile::SetHomingTarget(AActor* Target)
{
	if (ProjectileMovement && Target)
	{
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingAccelerationMagnitude = 8000.f;
		ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();

		// �̰� �߰������ ���� Homing�� ����-Ŭ�� ��� �ݿ���
		ProjectileMovement->SetUpdatedComponent(Collision);
		ProjectileMovement->Activate();
	}
}

// ����ü�� Collision�� Hit ���� ��,
void ARangedProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *OtherActor->GetName());

	if (!HasAuthority())
		return;

	if (OtherActor->IsA(ARangedProjectile::StaticClass()))
	{

		UKismetSystemLibrary::PrintString(this, TEXT("1"));
		return;
	}

	if (OtherActor == GetOwner() || OtherActor == this)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("2"));
		Destroy();
		return;
	}

	
	UAbilitySystemComponent* PlayerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());


	FGameplayCueParameters Params;
	Params.Location = Hit.ImpactPoint;

	//GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
	//	FGameplayTag::RequestGameplayTag("GameplayCue.Parry.HitEffect"), Params
	//);

	// PlayerASC->AddGameplayCue(
	// 	FGameplayTag::RequestGameplayTag("GameplayCue.Projectile.Hit"),
	// 	Params
	// );

	PlayerASC->ExecuteGameplayCue(
	FGameplayTag::RequestGameplayTag("GameplayCue.Projectile.Hit"),
	Params
	);
	
	//ASC�� �ִ� ���Ϳ��Ը� ����
	if (IAbilitySystemInterface* ASI = Cast< IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (DamageEffect)
			{
				// �� ( �������� �ο��� ����� Context)
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				Context.AddSourceObject(this);

				// ����ü�� ���� �÷��̾�
				Context.AddInstigator(PlayerASC->GetAvatarActor(), PlayerASC->GetAvatarActor());

				FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(
					DamageEffect, 1.f, Context);

				if (ASC && Spec.IsValid() && Spec.Data.IsValid())
				{
					Spec.Data->SetSetByCallerMagnitude( FGameplayTag::RequestGameplayTag("Data.SkillRatio"),
					0.95f);
					// �÷��̾ ��󿡰� Effect �ο�.
					PlayerASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
				}

			}
		}
	}


	//���̵� �ƴϵ� �ϴ� �ı�.
	//UE_LOG(LogTemp, Warning, TEXT("Destroyed"));
	UKismetSystemLibrary::PrintString(this, TEXT("3"));
	Destroy();
}

// Called when the game starts or when spawned
void ARangedProjectile::BeginPlay()
{
	Super::BeginPlay();

	SpawendLocation = GetActorLocation();

	SetActorTickEnabled(true);

	if (ProjectileMovement)
	{
		ProjectileMovement->SetUpdatedComponent(Collision); // �̰� �����ϸ� ������ ����
		ProjectileMovement->Activate(); // ���� Tick �۵� ����
	}

	if (ProjectileMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Projectile %s] %s | Velocity: %s | Authority: %s"),
			*GetName(),
			*GetActorLocation().ToString(),
			*ProjectileMovement->Velocity.ToString(),
			HasAuthority() ? TEXT("Server") : TEXT("Client"));
	}

}

void ARangedProjectile::Destroyed()
{
	Super::Destroyed();

	//��ƼŬ ȿ�� �߰��ϱ�.

}

void ARangedProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// �̵� �� �浹 ����
		Collision->IgnoreActorWhenMoving(GetOwner(), true);

		// ProjectileMovement�� Sweep���ε� ���õǵ��� MoveIgnoreActors�� �߰�
		Collision->MoveIgnoreActors.Add(GetOwner());
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called every frame
void ARangedProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Traveled = FVector::Dist(SpawendLocation, GetActorLocation());
	if (Traveled >= MaxDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroyed by MaxDistance"));
		Destroy();
	}
}


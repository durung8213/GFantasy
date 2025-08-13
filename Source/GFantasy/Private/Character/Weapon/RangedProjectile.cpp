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

	// 콜리전 충돌 설정
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
	// 충돌시 바인딩될 함수
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

		// 이걸 추가해줘야 실제 Homing이 서버-클라 모두 반영돼
		ProjectileMovement->SetUpdatedComponent(Collision);
		ProjectileMovement->Activate();
	}
}

// 투사체의 Collision이 Hit 됐을 시,
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
	
	//ASC가 있는 액터에게만 적용
	if (IAbilitySystemInterface* ASI = Cast< IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (DamageEffect)
			{
				// 적 ( 데미지를 부여할 대상의 Context)
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				Context.AddSourceObject(this);

				// 투사체를 날린 플레이어
				Context.AddInstigator(PlayerASC->GetAvatarActor(), PlayerASC->GetAvatarActor());

				FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(
					DamageEffect, 1.f, Context);

				if (ASC && Spec.IsValid() && Spec.Data.IsValid())
				{
					Spec.Data->SetSetByCallerMagnitude( FGameplayTag::RequestGameplayTag("Data.SkillRatio"),
					0.95f);
					// 플레이어가 대상에게 Effect 부여.
					PlayerASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
				}

			}
		}
	}


	//적이든 아니듣 일단 파괴.
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
		ProjectileMovement->SetUpdatedComponent(Collision); // 이걸 누락하면 무조건 멈춤
		ProjectileMovement->Activate(); // 내부 Tick 작동 시작
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

	//파티클 효과 추가하기.

}

void ARangedProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// 이동 중 충돌 무시
		Collision->IgnoreActorWhenMoving(GetOwner(), true);

		// ProjectileMovement의 Sweep으로도 무시되도록 MoveIgnoreActors에 추가
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


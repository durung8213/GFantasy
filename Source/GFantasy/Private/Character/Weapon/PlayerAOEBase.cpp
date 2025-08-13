// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Weapon/PlayerAOEBase.h"
#include "Abilities/Tasks/AbilityTask_SpawnActor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
APlayerAOEBase::APlayerAOEBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// 복제 설정
	bReplicates = true;
	SetReplicateMovement(true);

	// Sphere 콜리전
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->InitSphereRadius(DamageRadius);
	SphereComp->SetCollisionProfileName("OverlapAllDynamic");
	SphereComp->SetGenerateOverlapEvents(true);
	RootComponent = SphereComp;

	// Niagara 컴포넌트
	FXSystem = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp");
	FXSystem->SetupAttachment(RootComponent);
	FXSystem->bAutoActivate = false;

	//InitialLifeSpan = 1.0f;

}

// Called when the game starts or when spawned
void APlayerAOEBase::BeginPlay()
{
	Super::BeginPlay();

	//SphereComp->OnComponentBeginOverlap.AddDynamic(this, &APlayerAOEBase::OnOverlapBegin);
	
	if (HasAuthority())
	{
		if (TotalDamageCount <= 1)
		{
			// 한번만 실행
			ApplyAOEDamage();
		}
		else
		{
			// 타이머를 활용하여 다수의 AOE Overlap을 처리
			GetWorld()->GetTimerManager().SetTimer(
				MultiDamageTimerHandle,
				this, 
				&APlayerAOEBase::ApplyAOEDamage,
				DamageInterval,
				true);
		}

	}
}

void APlayerAOEBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void APlayerAOEBase::MultiCast_ActivateVFX_Implementation()
{
	if (FXSystem && NiagaraFX)
	{
		FXSystem->SetAsset(NiagaraFX);
		FXSystem->Activate(true);
	}
}

void APlayerAOEBase::ApplyAOEDamage()
{

	if (!DamageGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Skill Overlap : 1"));
		return;
	}

	if (!InstigatorASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Skill Overlap : 2"));
		return;
	}

	TArray<AActor*> HitActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(InstigatorASC->GetOwner());

	// ShpereOverlapActors를 사용하여 지정된 반경 내의 액터들을 가져옵니다.
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		DamageRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_Pawn) },
		nullptr,
		IgnoreActors,
		HitActors
	);

	for (AActor* Actor : HitActors)
	{
		// 데미지 적용 중복 방지
		if (!Actor || DamagedActors.Contains(Actor))
			return;

		DamagedActors.Add(Actor);

		UE_LOG(LogTemp, Warning, TEXT("Current Skill Overlap : %s"), *Actor->GetName());


		// 해당 액터가 IAbilitySystemInterface를 구현하고 있는지 확인
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
		{
			UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();

			// GameplayEffect로 전달할 SpecHandle 생성
			FGameplayEffectContextHandle Ctx = InstigatorASC->MakeEffectContext();
			Ctx.AddSourceObject(this);
			Ctx.AddInstigator(InstigatorASC->GetAvatarActor(), InstigatorASC->GetAvatarActor());
			FGameplayEffectSpecHandle Spec = InstigatorASC
				->MakeOutgoingSpec(DamageGE, 1.0f, Ctx);

			if (Spec.IsValid())
			{
				// AOEBase에 있는 SkillRatio를 Spec에 설정 
				Spec.Data->SetSetByCallerMagnitude(
				FGameplayTag::RequestGameplayTag("Data.SkillRatio"),
				SkillRatio); 

				// 이후 Spec의 Data를 TargetASC ( AOE를 맞은 적 ) 에 적용 -> 데미지 처리 진행
				InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
				UE_LOG(LogTemp, Warning, TEXT("Apply Skill Overlap Effect: %s"), *Actor->GetName());
			}
		}
		
	}
	
	// 데미지 카운트 증가 
	CurrentDamageCount++;

	// 오버랩된 액터들 초기화
	DamagedActors.Empty();

	// 총 데미지 횟수에 도달했으면 타이머를 중지
	if (CurrentDamageCount >= TotalDamageCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(MultiDamageTimerHandle);
		//Destroy();
	}
	
}


void APlayerAOEBase::Initialize(UAbilitySystemComponent* InInstigatorASC, TSubclassOf<UGameplayEffect> InDamageGE, float InRadius, UNiagaraSystem* InNiagaraFX,
	int32 InTotalCount,float InInterval)
{
	// AOE 초기화
	InstigatorASC = InInstigatorASC;
	DamageGE = InDamageGE;
	DamageRadius = InRadius;
	TotalDamageCount = InTotalCount;
	DamageInterval = InInterval;
	NiagaraFX = InNiagaraFX;
	
	SphereComp->SetSphereRadius(DamageRadius);
	// FXSystem->SetAsset(InNiagaraFX);
	// FXSystem->Activate(true);

	if (HasAuthority())
	{
		MultiCast_ActivateVFX();
	}

	float DamageDuration = (DamageInterval * TotalDamageCount);
	float ExtralLifeTime = 0.9f;	// 이펙트가 자연스럽게 사라질 시간

	SetLifeSpan(DamageDuration + ExtralLifeTime);
}

// Called every frame
void APlayerAOEBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




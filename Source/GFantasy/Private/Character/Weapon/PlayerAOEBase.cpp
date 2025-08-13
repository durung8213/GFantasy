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

	// ���� ����
	bReplicates = true;
	SetReplicateMovement(true);

	// Sphere �ݸ���
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->InitSphereRadius(DamageRadius);
	SphereComp->SetCollisionProfileName("OverlapAllDynamic");
	SphereComp->SetGenerateOverlapEvents(true);
	RootComponent = SphereComp;

	// Niagara ������Ʈ
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
			// �ѹ��� ����
			ApplyAOEDamage();
		}
		else
		{
			// Ÿ�̸Ӹ� Ȱ���Ͽ� �ټ��� AOE Overlap�� ó��
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

	// ShpereOverlapActors�� ����Ͽ� ������ �ݰ� ���� ���͵��� �����ɴϴ�.
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
		// ������ ���� �ߺ� ����
		if (!Actor || DamagedActors.Contains(Actor))
			return;

		DamagedActors.Add(Actor);

		UE_LOG(LogTemp, Warning, TEXT("Current Skill Overlap : %s"), *Actor->GetName());


		// �ش� ���Ͱ� IAbilitySystemInterface�� �����ϰ� �ִ��� Ȯ��
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
		{
			UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();

			// GameplayEffect�� ������ SpecHandle ����
			FGameplayEffectContextHandle Ctx = InstigatorASC->MakeEffectContext();
			Ctx.AddSourceObject(this);
			Ctx.AddInstigator(InstigatorASC->GetAvatarActor(), InstigatorASC->GetAvatarActor());
			FGameplayEffectSpecHandle Spec = InstigatorASC
				->MakeOutgoingSpec(DamageGE, 1.0f, Ctx);

			if (Spec.IsValid())
			{
				// AOEBase�� �ִ� SkillRatio�� Spec�� ���� 
				Spec.Data->SetSetByCallerMagnitude(
				FGameplayTag::RequestGameplayTag("Data.SkillRatio"),
				SkillRatio); 

				// ���� Spec�� Data�� TargetASC ( AOE�� ���� �� ) �� ���� -> ������ ó�� ����
				InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
				UE_LOG(LogTemp, Warning, TEXT("Apply Skill Overlap Effect: %s"), *Actor->GetName());
			}
		}
		
	}
	
	// ������ ī��Ʈ ���� 
	CurrentDamageCount++;

	// �������� ���͵� �ʱ�ȭ
	DamagedActors.Empty();

	// �� ������ Ƚ���� ���������� Ÿ�̸Ӹ� ����
	if (CurrentDamageCount >= TotalDamageCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(MultiDamageTimerHandle);
		//Destroy();
	}
	
}


void APlayerAOEBase::Initialize(UAbilitySystemComponent* InInstigatorASC, TSubclassOf<UGameplayEffect> InDamageGE, float InRadius, UNiagaraSystem* InNiagaraFX,
	int32 InTotalCount,float InInterval)
{
	// AOE �ʱ�ȭ
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
	float ExtralLifeTime = 0.9f;	// ����Ʈ�� �ڿ������� ����� �ð�

	SetLifeSpan(DamageDuration + ExtralLifeTime);
}

// Called every frame
void APlayerAOEBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




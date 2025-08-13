#include "GAS/GameplayAbility/GA_SwordFlameAttack.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_SpawnActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/GASPlayerCharacter.h"
#include "Character/Weapon/WeaponBase.h"
#include "GameplayEffectTypes.h"
#include "Character/Weapon/FlameAttackAOE.h"
#include "DrawDebugHelpers.h" 
#include "Character/Weapon/PlayerAOEBase.h"

UGA_SwordFlameAttack::UGA_SwordFlameAttack()
{
	// �±� ����
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Sword.FlameAttack")));
	 
	// �±� �ʱ�ȭ
	TrailEventTag = FGameplayTag::RequestGameplayTag("Event.Attack.Sword.FlameTrail");
	GroundEventTag = FGameplayTag::RequestGameplayTag("Event.Attack.Sword.FlameGround");
	//CueTag_Trail = FGameplayTag::RequestGameplayTag("GameplayCue.Sword.FlameTrail");
	CueTag_Ground = FGameplayTag::RequestGameplayTag("GameplayCue.Sword.FlameGround");

	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_SwordFlameAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	
	
	//��Ÿ�� ����
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, SmashMontage
	);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_SwordFlameAttack::OnMontageCompleted);
	MontageTask->ReadyForActivation();

	// Trail ��� �̺�Ʈ
	TrailTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, TrailEventTag, nullptr, true
	);
	TrailTask->EventReceived.AddDynamic(this, &UGA_SwordFlameAttack::OnTrailEvent);
	TrailTask->ReadyForActivation();

	// Ground �̺�Ʈ ���
	GroundTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, GroundEventTag, nullptr, true
	);
	GroundTask->EventReceived.AddDynamic(this, &UGA_SwordFlameAttack::OnGroundEvent);
	GroundTask->ReadyForActivation();


}

void UGA_SwordFlameAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_SwordFlameAttack::OnTrailEvent(const FGameplayEventData Payload)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!ASC || !Char)	return;

	// ���� ���
	//ASC->AddGameplayCue(CueTag_Trail);

}

void UGA_SwordFlameAttack::OnGroundEvent(const FGameplayEventData Payload)
{

	if (!HasAuthority(&CurrentActivationInfo))
		return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	// ĳ������ Weapon�� �����ͼ� Static Mesh�� ���� ��ġ�� �����ͼ� ���� Ʈ���̽�
	AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)	return;

	//ASC->RemoveGameplayCue(CueTag_Trail);

	const FVector Forward = Char->GetActorForwardVector();
	const FVector StartPos = Char->GetActorLocation()
		+ Forward * 150.0f
		+ FVector(0, -60.f, 50.f);

	const FVector EndPos = StartPos - FVector(0, 0, 500.f);


	// 4) ���� Ʈ���̽�
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Char);

	bool bHit = Char->GetWorld()->LineTraceSingleByChannel(
		Hit, StartPos, EndPos, ECC_Visibility, Params);

	DrawDebugLine(Char->GetWorld(), StartPos, EndPos, FColor::Blue, false, 1.0f, 0, 2.0f);

	const FVector SpawnLocation = bHit
		? Hit.Location
		: (Char->GetActorLocation() + Forward * 50.0f);

	const FRotator SpawnRotation = FRotator::ZeroRotator;

	if (bHit && AOEActorClass)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("bHit True"));

		// AOE ����
		SpawnAOEActorAtLocation(Hit);

	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Flame : No Hit"));
	}
}

void UGA_SwordFlameAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_SwordFlameAttack::OnAOEActorSpawned(AFlameAttackAOE* SpawnedActor)
{
	//AOE ���� �ʱ�ȭ ���� ( �����ؼ� Effect �ֱ� )
	if (SpawnedActor)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		SpawnedActor->Initialize(ASC, SpawnedActor->DamageGE, SpawnedActor->DamageRadius, 
			SpawnedActor->NiagaraFX, SpawnedActor->TotalDamageCount, SpawnedActor->DamageInterval);
	}
	
}

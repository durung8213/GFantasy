// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCombatNPC.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/Weapon/BaseStaticMeshBoxWeapon.h"
#include "Actor/Weapon/BaseWeapon.h"
#include "Components/WidgetComponent.h"
#include "GAS/AbilitySystemComponent/CombatNonPlayerGASC.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GAS/AttributeSet/GASCombatAS.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Enemy/EnemyState/EnemyStateWidget.h"
#include "GAS/GamePlayTag.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Quest/KillCountQuest.h"
#include "Quest/QuestManagerComponent.h"
#include "Quest/QuestInstance.h"
#include "UI/Enemy/EnemyState/EnemyHealthBarWidget.h"
#include "UI/Enemy/EnemyState/EnemyInfoWidget.h"
#include "Components/TimelineComponent.h"
#include "Controller/AIControllerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/GASAbilitySystemLibrary.h"

DEFINE_LOG_CATEGORY(LOG_GASCombatNPC);

void AGASCombatNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AGASCombatNPC, RagdollVelocity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGASCombatNPC, bIsFocus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGASCombatNPC, bIsDead, COND_None, REPNOTIFY_Always);
}

AGASCombatNPC::AGASCombatNPC()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCombatNonPlayerGASC>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CharacterAttributeSet = CreateDefaultSubobject<UGASCharacterAS>(TEXT("CharacterAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UGASCombatAS>(TEXT("CombatAttributeSet"));

	EnemyStateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyStateWidgetComponent"));
	EnemyStateWidgetComponent->SetupAttachment(RootComponent);
	EnemyStateWidgetComponent->SetRelativeLocation(FVector(0, 0, 95));
	EnemyStateWidgetComponent->SetDrawSize(FVector2D(150.f,50.f));
	EnemyStateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> EnemyStateWidgetClassFInder(TEXT("/Game/UI/Enemy/WBP_EnemyState.WBP_EnemyState_C"));
	if (EnemyStateWidgetClassFInder.Succeeded())
	{
		EnemyStateWidgetComponent->SetWidgetClass(EnemyStateWidgetClassFInder.Class);
	}

	DeadDissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DeadDissolveTimeline"));

	//EnemyStateWidgetComponent->SetAutoActivate(false);
}

void AGASCombatNPC::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		// 능력치 초기화
		InitAbilityActorInfo();
		GiveAbilites();
		ApplyStartupEffects();
		check(AbilitySystemComponent);

		CharacterAttributeSet->NormalHitDelegate.AddDynamic(this, &AGASCombatNPC::OnNormalDamage);
		CharacterAttributeSet->StrongHitDelegate.AddDynamic(this, &AGASCombatNPC::OnStrongDamage);
		CharacterAttributeSet->EnemyDieToNormalHitDelegate.AddDynamic(this, &AGASCombatNPC::OnNormalAttackDie);
		CharacterAttributeSet->EnemyDieToStrongHitDelegate.AddDynamic(this, &AGASCombatNPC::OnStrongAttackDie);
	}
	
	EnemyStateWidget = Cast<UEnemyStateWidget>(EnemyStateWidgetComponent->GetWidget());
	if (EnemyStateWidget)
	{
		//UKismetSystemLibrary::PrintString(this, TEXT("바인드"));
		EnemyStateWidget->BindCallbackToASC(GetAbilitySystemComponent(), GetCharacterAttributeSet());
		EnemyStateWidget->FocusMode();
	}
	else
	{
		UE_LOG(LOG_GASCombatNPC, Error, TEXT("EnemyStateWidget Cast Fail"));
		//UKismetSystemLibrary::PrintString(this, TEXT("EnemyStateWidget Cast Fail"), true, true, FLinearColor::Red);
	}

	// Combat_Hit_HitReact 태그가 생겼을 때, 삭제되었을 때 내가 만든 함수를 실행하게 하도록 바인딩 함
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTags::Get().Combat_Hit_HitReactFront, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&AGASCombatNPC::HitReactTagChanged
	);

	// Combat_Hit_HitReact 태그가 생겼을 때, 삭제되었을 때 내가 만든 함수를 실행하게 하도록 바인딩 함
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTags::Get().Combat_Hit_HitReactBack, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&AGASCombatNPC::HitReactTagChanged
	);

	// Combat_Hit_HitReact 태그가 생겼을 때, 삭제되었을 때 내가 만든 함수를 실행하게 하도록 바인딩 함
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTags::Get().Combat_Hit_HitReactRight, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&AGASCombatNPC::HitReactTagChanged
	);

	// Combat_Hit_HitReact 태그가 생겼을 때, 삭제되었을 때 내가 만든 함수를 실행하게 하도록 바인딩 함
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTags::Get().Combat_Hit_HitReactLeft, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&AGASCombatNPC::HitReactTagChanged
	);

	if (HasAuthority())
	{
		AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(
			CharacterAttributeSet->GetHealthAttribute()
		).AddUObject(this, &AGASCombatNPC::BindHealth);

		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributeSet->GetMaxHealthAttribute()
			).AddUObject(this, &AGASCombatNPC::BindMaxHealth);
		
		GetWorld()->GetTimerManager().SetTimer(
			InitHealthTimerHandle,
			this,
			&AGASCombatNPC::ServerInitHealths,
			2.f,
			false
		);
		
	}
	if (EnemyStateWidget) {
		if (EnemyStateWidget->EnemyInfo) {
			EnemyStateWidget->EnemyInfo->SetName(MonsterName);
		}
		
	}
	
	
}

void AGASCombatNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGASCombatNPC::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

bool AGASCombatNPC::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
	FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) {
		// 받아온 이팩트가 없으면 이팩트 적용에 실패했다고 하고 종료
		return false;
	}

	// FGameplayEffectSpecHandle : 어떤 GameplayEffect를 어떻게 적용할지를 담은 사양 객체 핸들이야
	// MakeOutgoingSpec : 새로운 효과 사양(Spec)을 만드는 함수. MakeOutgoingSpec(어떤 효과(클래스), 레벨, 컨텍스트)
	FGameplayEffectSpecHandle SpecHandle
		= AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);

	if (SpecHandle.IsValid()) {
		// SpecHandle이 만들어 졌으면
		FActiveGameplayEffectHandle ActiveGEHandle	// FActiveGameplayEffectHandle : 실제로 적용된 효과를 추적하는 핸들. 나중에 제거하거나 갱신할 때 사용
			= AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());	// ApplyGameplayEffectSpecToSelf : 만든 Spec을 자기 자신에게 적용하는 함수, *SpecHandle.Data.Get() : 	SpecHandle 안의 진짜 데이터(Spec)를 꺼내서 포인터를 역참조하고 전달
		return ActiveGEHandle.WasSuccessfullyApplied();	// 효과가 제대로 적용됐는지 확인하고 그 값을 리턴함
	}

	return false;
}

bool AGASCombatNPC::CheckInitializeAttributes()
{
	if (!(GetLocalRole() == ROLE_Authority)) {
		// 서버 권한을 가진 인스턴스가 아니면 
		return false;
	}

	if (!DefaultStateSet) {
		// DefaultStateSet가 nullptr이면
		UE_LOG(LOG_GASCombatNPC, Error, TEXT("Require to set DefaultStateSet in AINPC's Detail Panel"));
		//UKismetSystemLibrary::PrintString(this, TEXT("Require to set DefaultStateSet in AI NPC's Detail Panel"));
		return false;
	}

	if (!CharacterAttributeSet || !CombatAttributeSet) {
		// AttributeSet가 nullptr이면 false
		// AttributeSet은 생성자에서 생성하기에 이것이 false가 나면 안된다
		UE_LOG(LOG_GASCombatNPC, Error, TEXT("Require to set AttributeSet in AI NPC's Detail Panel"));
		//UKismetSystemLibrary::PrintString(this, TEXT("Require to set AttributeSet in AI NPC's Detail Panel"));
		return false;
	}

	return true;
}

void AGASCombatNPC::InitAbilityActorInfo()
{
	if (CheckInitializeAttributes()) {
		// 기본으로 적용해야할 Effect가 있으면 적용
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();	// 효과 컨텍스트 생성
		EffectContext.AddSourceObject(this);							// 현재 캐릭터를 소스로 설정
		ApplyGameplayEffectToSelf(DefaultStateSet, EffectContext);		// DefaultStateSet을 자신에게 적용
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	//AbilitySystemComponent->AbilityActorInfoSet();
}

void AGASCombatNPC::GiveAbilites()
{
	if (HasAuthority() && AbilitySystemComponent) {
		// 서버에서만 실행
		// AbilitySystemComponent가 존재할 경우 만 실행
		for (auto DefaultAbility : DefaultAbilities) {
			AbilitySystemComponent
				->GiveAbility(FGameplayAbilitySpec(DefaultAbility));	// DefaultAbilities 배열에 있는 UGameplayAbility를 FGameplayAbilitySpec로 래핑해 ASC에 등록
		}
	}
}

void AGASCombatNPC::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority) {
		// 서버에서만 수행
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();	// 효과 컨텍스트 생성
		EffectContext.AddSourceObject(this);							// 현재 캐릭터를 소스로 설정

		for (auto CharacterEffect : DefaultEffects) {
			ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);	// 추가될 능력치를 자신에게 적용
		}
	}
}

UAbilitySystemComponent* AGASCombatNPC::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGASCharacterAS* AGASCombatNPC::GetCharacterAttributeSet() const
{
	return CharacterAttributeSet;
}

UGASCombatAS* AGASCombatNPC::GetCombatAttributeSet() const
{
	return CombatAttributeSet;
}


void AGASCombatNPC::SetWeapon(UChildActorComponent* ActorComponent)
{
	if (!ActorComponent) return;
	if (ABaseWeapon* ComponentChildActor = Cast<ABaseWeapon>(ActorComponent->GetChildActor()))
	{
		Weapon = ComponentChildActor;
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Set Weapon(%s) Success"), *GetName(), *ComponentChildActor->GetName());
		//UKismetSystemLibrary::PrintString(this,TEXT("Set Weapon Success"));
	}
	else
	{
		UE_LOG(LOG_GASCombatNPC, Error, TEXT("%s Set Weapon(%s) Fail(Cast Fail)"), *GetName(), *ActorComponent->GetName());
		//UKismetSystemLibrary::PrintString(this,TEXT("Set Weapon Fail"), true, true, FLinearColor::Red);
	}
	
}

void AGASCombatNPC::AddWeaponArr(UChildActorComponent* ActorComponent)
{
	if (ABaseWeapon* ComponentChildActor = Cast<ABaseWeapon>(ActorComponent->GetChildActor()))
	{
		WeaponArr.Add(ComponentChildActor);
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Set Weapon(%s) to Arr Success"), *GetName(), *ComponentChildActor->GetName());
		//UKismetSystemLibrary::PrintString(this,TEXT("Set Weapon Success"));
	}
	else
	{
		UE_LOG(LOG_GASCombatNPC, Error, TEXT("%s Set Weapon(%s) to Arr Fail(Cast Fail)"), *GetName(), *ActorComponent->GetName());
		//UKismetSystemLibrary::PrintString(this,TEXT("Set Weapon Fail"), true, true, FLinearColor::Red);
	}
}

ABaseWeapon* AGASCombatNPC::GetWeapon()
{
	return Weapon;
}

void AGASCombatNPC::SetbIsFocus_Implementation(bool bNewIsFocus)
{
	bIsFocus = bNewIsFocus;
}

bool AGASCombatNPC::GetbIsFocus_Implementation()
{
	return bIsFocus;
}

void AGASCombatNPC::OnDie_Implementation()
{
	Tags.Empty();
	Ragdoll();
	//bIsDead = true;

	if (AAIControllerBase* CurrentController = Cast<AAIControllerBase>(GetController()))
	{
		CurrentController->StopBT();
	}
	
	if (HasAuthority())
	{
		FTimerHandle DissolveTimerHandle;
		GetWorld()
			->GetTimerManager().SetTimer(
				DissolveTimerHandle,
				this,
				&AGASCombatNPC::DeadDissolveTimelineStart,
				5.f,
				false
			);

		FTimerHandle HpWidgetRemoveTimerHandle;
		GetWorld()
			->GetTimerManager().SetTimer(
				HpWidgetRemoveTimerHandle,
				this,
				&AGASCombatNPC::DissolveEnemyStateWidget,
				2.f,
				false
			);
	}

	

	if (InitHealthTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(InitHealthTimerHandle);
				
	}

	if (EnemyStateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(EnemyStateTimerHandle);
	}
	
	GetWorld()
	->GetTimerManager().SetTimer(
		DeadTimerHandle,
		this,
		&AGASCombatNPC::ClearDeadTimerHandle,
		10.f,
		false
	);
}

void AGASCombatNPC::SetEnemyMovementSpeed_Implementation(float NewMovementSpeed)
{
	IEnemyInterface::SetEnemyMovementSpeed_Implementation(NewMovementSpeed);
	GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
}

void AGASCombatNPC::ParryReceived()
{
	FGameplayTagContainer ParryReceive;
	ParryReceive.AddTag(FGameplayTags::Get().State_Parry_Received);
	//GetAbilitySystemComponent()->TryActivateAbilitiesByTag()
}

void AGASCombatNPC::CalcInstigatorAngle(AActor* InstigatorActor, float& AngleDegrees, FVector& CrossProduct)
{
	FVector Direction = InstigatorActor->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();  // 단위 벡터로 정규화
	
	// 내 forward vector
	FVector MyForward = GetActorForwardVector();
	MyForward.Z = 0.0f;
	MyForward.Normalize();
			
	// 두 벡터 간의 내적을 사용해서 각도 구하기
	float Dot = FVector::DotProduct(MyForward, Direction);
	
	AngleDegrees = FMath::Acos(Dot) * (180.0f / PI); // 라디안 -> 도
	CrossProduct = FVector::CrossProduct(MyForward, Direction);
}


void AGASCombatNPC::OnNormalDamage(const FGameplayEffectSpec& EffectSpec,
                                   const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	
	//UKismetSystemLibrary::PrintString(this,TEXT("노멀 공격 델리게이트 받음"));

	float AngleDegrees;
	FVector CrossProduct;
	AActor* InstigatorActor = EffectSpec.GetContext().GetInstigator();
	CalcInstigatorAngle(InstigatorActor, AngleDegrees, CrossProduct);
	
	//			방향에 맞는 태그로 어빌리티 실행		//
	FGameplayTagContainer TagContainer;
			
	if (AngleDegrees <= 45.0f)
	{
		TagContainer.AddTag(FGameplayTags::Get().Combat_Hit_HitReactFront);
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Hit Normal Attack %s by Front"), *InstigatorActor->GetName(), *GetName());
		//UKismetSystemLibrary::PrintString(this, TEXT("Front"));
	}
	else if (AngleDegrees > 135.0f)
	{
		TagContainer.AddTag(FGameplayTags::Get().Combat_Hit_HitReactBack);
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Hit Normal Attack %s by Back"), *InstigatorActor->GetName(), *GetName());
		//UKismetSystemLibrary::PrintString(this, TEXT("Back"));
	}
	else if (CrossProduct.Z > 0)
	{
		TagContainer.AddTag(FGameplayTags::Get().Combat_Hit_HitReactRight);
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Hit Normal Attack %s by Right"), *InstigatorActor->GetName(), *GetName());
		//UKismetSystemLibrary::PrintString(this, TEXT("Right"));
	}
	else
	{
		TagContainer.AddTag(FGameplayTags::Get().Combat_Hit_HitReactLeft);
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Hit Normal Attack %s by Left"), *InstigatorActor->GetName(), *GetName());
		//UKismetSystemLibrary::PrintString(this, TEXT("Left"));
	}

	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);
	//			방향에 맞는 태그로 어빌리티 실행 end	//

	// OnCombatNPCGetDamage.Broadcast(EffectSpec.GetContext().GetInstigator());

	if (HitCount >= SuperArmorActivateCount && !GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Combat.State.SuperArmor")))
	{
		UGASAbilitySystemLibrary::ApplyGASEffectToSelfToASC(GetAbilitySystemComponent(), SuperArmor);
		HitCount = 0;
	}
	else
	{
		HitCount++;
	}
}

void AGASCombatNPC::OnStrongDamage(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	//UKismetSystemLibrary::PrintString(this,TEXT("강 공격 델리게이트 받음"));
	// 피격 당했는데 죽은게 아니라면

	float AngleDegrees;
	FVector CrossProduct;
	CalcInstigatorAngle(EffectSpec.GetContext().GetInstigator(), AngleDegrees, CrossProduct);
	
	AActor* OwnerActor = this;	// 자기 자신
	AActor* InstigatorActor = EffectSpec.GetContext().GetInstigator();			// 타격한 상대

	
	//			상대와 나 와의 각도 구하기			//
	FVector Direction = InstigatorActor->GetActorLocation() - OwnerActor->GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();  // 단위 벡터로 정규화

	// 기준 회전: Instigator 쪽을 바라보는 회전
	FRotator LookAtRotation = Direction.Rotation();
	//			상대와 나 와의 각도 구하기 end		//
	

	FRotator NewRotation;
	FGameplayTagContainer TagContainer;
	if (AngleDegrees <= 90.f)
	{
		// Instigator가 OwnerActor의 앞에 있음
		NewRotation = LookAtRotation;
		TagContainer.AddTag(FGameplayTags::Get().Combat_Hit_StrongHitReactFront);
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Hit Strong Attack %s by Front"), *InstigatorActor->GetName(), *GetName());
	}
	else
	{
		// Instigator가 OwnerActor의 뒤에 있음
		NewRotation = (-Direction).Rotation(); // 반대 방향
		TagContainer.AddTag(FGameplayTags::Get().Combat_Hit_StrongHitReactBack);
		UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Hit Strong Attack %s by Back"), *InstigatorActor->GetName(), *GetName());
	}
	OwnerActor->SetActorRotation(NewRotation);

	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);

	// OnCombatNPCGetDamage.Broadcast(EffectSpec.GetContext().GetInstigator());

	if (HitCount >= SuperArmorActivateCount && !GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Combat.State.SuperArmor")))
	{
		UGASAbilitySystemLibrary::ApplyGASEffectToSelfToASC(GetAbilitySystemComponent(), SuperArmor);
		HitCount = 0;
	}
	else
	{
		HitCount++;
	}
}

void AGASCombatNPC::OnNormalAttackDie(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	if (bIsDead) {
		return;
	}
	//UKismetSystemLibrary::PrintString(this,TEXT("노멀 공격으로 죽음"));
	AActor* InstigatorActor = EffectSpec.GetContext().GetInstigator();
	
	UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Kill(Normal Attack) %s"), *InstigatorActor->GetName(), *GetName());
	
	FVector RagdollVector = GetActorLocation() - InstigatorActor->GetActorLocation();
	RagdollVector.Z = 0.f;
	RagdollVector.Normalize();				
	SetRagdollVector(RagdollVector);	// AI가 날아가야 하는 방향을 Normalize해서 알려줌
			
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTags::Get().Combat_Dead_Dead);
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);

	UE_LOG(LogTemp, Log, TEXT("KillCountQuest : MonsterNormalAttackDie"));


	if (AGASWorldPlayerState* CurrentKillerPlayerState = InstigatorActor->GetInstigatorController()->GetPlayerState<AGASWorldPlayerState>())
	{
		UE_LOG(LogTemp, Log, TEXT("KillCountQuest : Go MonsterNormalAttackDie"));
		SendToPlayerIfDie(CurrentKillerPlayerState);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("KillCountQuest : No PlayerState"));
	}

	bIsDead = true;
}

void AGASCombatNPC::OnStrongAttackDie(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	AActor* InstigatorActor = EffectSpec.GetContext().GetInstigator();

	UE_LOG(LOG_GASCombatNPC, Log, TEXT("%s Kill(Strong Attack) %s"), *InstigatorActor->GetName(), *GetName());
	
	FVector RagdollVector = GetActorLocation() - InstigatorActor->GetActorLocation();
	RagdollVector.Z = 0.f;
	RagdollVector.Normalize();				
	SetRagdollVector(RagdollVector);	// AI가 날아가야 하는 방향을 Normalize해서 알려줌
			
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTags::Get().Combat_Dead_Dead);
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);

	UE_LOG(LogTemp, Log, TEXT("KillCountQuest : MonsterStrongAttackDie"));


	if (InstigatorActor)
	{
		if (InstigatorActor->GetInstigatorController())
		{
			if (AGASWorldPlayerState* CurrentKillerPlayerState = InstigatorActor->GetInstigatorController()->GetPlayerState<AGASWorldPlayerState>())
			{
				UE_LOG(LogTemp, Log, TEXT("KillCountQuest : Go MonsterStrongAttackDie"));
				SendToPlayerIfDie(CurrentKillerPlayerState);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("KillCountQuest : No PlayerState"));
			}
	
		}
	}
	
	
	bIsDead = true;
}

void AGASCombatNPC::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	//const bool bHitReact = NewCount > 0;	// 히트 리액트 태그가 0보다 커야만 (없어질 때도 호출되니) 맞앚다고 처줌
	// UKismetSystemLibrary::PrintString(this,TEXT("???"));
}

void AGASCombatNPC::Ragdoll()
{
	//UKismetSystemLibrary::PrintString(this,TEXT("Ragdoll"));
	if (HasAuthority() && !bIsRagdoll)
	{
		MulticastRagdoll();;
	}
	
}

/*
 * 향희님이 추가해달라 했던 죽었을 때 보내는 함수
 */
void AGASCombatNPC::SendToPlayerIfDie(AGASWorldPlayerState* CurrentPlayerState)
{
	UE_LOG(LogTemp, Log, TEXT("KillCountQuest : GASCombatNPC SendToPlayerIfDie"));

	if (HasAuthority() && !bIsDead) {
		if (UQuestManagerComponent* QuestComp = CurrentPlayerState->FindComponentByClass<UQuestManagerComponent>())
		{
			// InProgress 상태 태그 요청
			static const FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);

			// 진행 중인 퀘스트만 필터링
			TArray<UQuestInstance*> InProgressQuests = QuestComp->GetQuestsByState(InProgressTag);

			// KillCountQuest에 태그 전달
			for (UQuestInstance* Quest : InProgressQuests)
			{
				if (UKillCountQuest* KillQuest = Cast<UKillCountQuest>(Quest))
				{
					KillQuest->HandleMonsterKilled(MonsterTag);
					//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s 태그를 가지고 있는 몬스터를 죽임"), *MonsterTag.ToString()));
				}
			}
		}
	}

	
}

void AGASCombatNPC::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(EnemyStateTimerHandle);
}

void AGASCombatNPC::ClearDeadTimerHandle()
{
	if (DeadTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DeadTimerHandle);
	}

	if (HasAuthority())
	{
		OnCombatNPCDie.Broadcast();
	}
	Destroy();
}


// 적 상태 안보이게 함
void AGASCombatNPC::CollapsedMode_Implementation()
{
	if (EnemyStateWidget)
	{
		EnemyStateWidget->CollapsedMode();
	}
}

/**
 * <summary>
 * 적 HpBar만 보임
 * </summary>
 */
void AGASCombatNPC::UnFocusMode_Implementation()
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->UnFocusMode();
	}
	

}

/**
 * <summary>
 * 적 HpBar이 Time 만큼 보임
 * </summary>
 * <param name="Time">보일 시간</param>
 */
void AGASCombatNPC::UnFocusModeByTime_Implementation(float Time)
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->UnFocusMode();
	}
	GetWorld()->GetTimerManager().SetTimer(EnemyStateTimerHandle, this, &AGASCombatNPC::CollapsedMode, Time, false);
}

/**
 * <summary>
 * 적 HpBar, 퀘스트 마크가 보임
 * </summary>
 */
void AGASCombatNPC::UnFocusModeWithQuest_Implementation()
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->UnFocusModeWithQuest();
	}
}

/**
 * <summary>
 * 적 HpBar, 퀘스트 마크가 Time 만큼 보임
 * </summary>
 * <param name="Time">보일 시간</param>
 */
void AGASCombatNPC::UnFocusModeWithQuestByTime_Implementation(float Time)
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->UnFocusModeWithQuest();
	}
	GetWorld()->GetTimerManager().SetTimer(EnemyStateTimerHandle, this, &AGASCombatNPC::CollapsedMode, Time, false);
}

/**
 * <summary>
 * 적 HpBar, 이름이 보임
 * </summary>
 */
void AGASCombatNPC::FocusMode_Implementation()
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->FocusMode();
	}
}

/**
 * <summary>
 * 적 HpBar, 이름이 Time 만큼 보임
 * </summary>
 * <param name="Time">보일 시간</param>
 */
void AGASCombatNPC::FocusModeByTime_Implementation(float Time)
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->FocusMode();
	}
	GetWorld()->GetTimerManager().SetTimer(EnemyStateTimerHandle, this, &AGASCombatNPC::CollapsedMode, Time, false);
}

/**
 * <summary>
 * 적 HpBar, 이름, 퀘스트 마크가 보임
 * </summary>
 */
void AGASCombatNPC::FocusModeWithQuest_Implementation()
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->FocusModeWithQuest();
	}
}

/**
 * <summary>
 * 적 HpBar, 이름, 퀘스트 마크가 Time만큼 보임
 * </summary>
 * <param name="Time">보일 시간</param>
 */
void AGASCombatNPC::FocusModeWithQuestByTime_Implementation(float Time)
{
	ClearTimer();
	if (EnemyStateWidget)
	{
		EnemyStateWidget->FocusModeWithQuest();
	}
	GetWorld()->GetTimerManager().SetTimer(EnemyStateTimerHandle, this, &AGASCombatNPC::CollapsedMode, Time, false);
}

void AGASCombatNPC::ChangeMeshByIndex(int Index)
{
	//check(BossSkeletalMeshArr.Contains(Index));
	//check(BossSkeletalMeshArr[Index])

	// if (BossSkeletalMeshArr.Contains(Index))
	// {
	// 	if (BossSkeletalMeshArr[Index])
	// 	{
	// 		GetMesh()->SetSkeletalMesh(BossSkeletalMeshArr[Index]);
	// 	}
	// }
	
	
}

void AGASCombatNPC::BindHealth(const FOnAttributeChangeData& Data)
{
	MulticastBindHealth(Data.NewValue);
}

void AGASCombatNPC::MulticastBindHealth_Implementation(float NewHealth)
{
	if (EnemyStateWidget)
	{
		EnemyStateWidget->EnemyHealthBar->SetHealth(NewHealth);
	}
}

void AGASCombatNPC::BindMaxHealth(const FOnAttributeChangeData& Data)
{
	MulticastMaxBindHealth(Data.NewValue);
}

void AGASCombatNPC::DeadDissolveTimelineStart_Implementation()
{
	if (DeadDissolveFloatCurve)
	{
		// 델리게이트 바인딩
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("DeadDissolveTimelineProgress"));

		FOnTimelineEvent TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("DeadDissolveTimelineFinished"));
		
		DeadDissolveTimeline->AddInterpFloat(DeadDissolveFloatCurve, ProgressFunction);
		DeadDissolveTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
		DeadDissolveTimeline->SetLooping(false); // 루프 여부
		DeadDissolveTimeline->SetPlayRate(1.0f); // 속도

		DeadDissolveTimeline->PlayFromStart(); // 재생
	}
}

void AGASCombatNPC::DissolveEnemyStateWidget_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(WidgetDissolveTimerHandle);
	WidgetDissolveElapsedTime = 0;
	if (EnemyStateWidget)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WidgetDissolveTimerHandle,
			this,
			&AGASCombatNPC::StateWidgetDissolveLoop,
			0.03f,
			true
		);
		//EnemyStateWidget->CollapsedMode();
	}
}

void AGASCombatNPC::DeadDissolveTimelineProgress(float Value)
{
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
	{
		UMaterialInterface* BaseMat = GetMesh()->GetMaterial(i);
		if (!BaseMat) continue;

		// 이미 Dynamic Material Instance가 적용되어 있는지 확인
		UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(BaseMat);
		if (!DynMat)
		{
			DynMat = UMaterialInstanceDynamic::Create(BaseMat, this);
			GetMesh()->SetMaterial(i, DynMat);
		}

		DynMat->SetScalarParameterValue(TEXT("DissolveAmount"), Value);
	}
	//GetMesh()->SetScalarParameterValueOnMaterials(TEXT("DissolveAmount"), Value);
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("DissolveAmount %lf%"), Value));
}

void AGASCombatNPC::DeadDissolveTimelineFinished()
{
	Destroy();
	UE_LOG(LOG_GASCombatNPC, Log, TEXT("DeadDissolveTimelineFinished"));
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Dissolve Complete")));
	ClearDeadTimerHandle();
}

void AGASCombatNPC::StateWidgetDissolveLoop()
{
	WidgetDissolveElapsedTime += 0.03f;

	EnemyStateWidgetComponent
		->GetWidget()
			->SetRenderOpacity(
				FMath::Lerp(
					EnemyStateWidgetComponent->GetWidget()->GetRenderOpacity()
					, 0
					, WidgetDissolveElapsedTime / 1.5f
					)
				);

	if (WidgetDissolveElapsedTime >= 1.5f)
	{
		GetWorld()->GetTimerManager().ClearTimer(WidgetDissolveTimerHandle);
		EnemyStateWidgetComponent->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
		EnemyStateWidgetComponent->SetActive(false);
	}
}

void AGASCombatNPC::MulticastMaxBindHealth_Implementation(float NewMaxHealth)
{
	if (EnemyStateWidget)
	{
		EnemyStateWidget->EnemyHealthBar->SetMaxHealth(NewMaxHealth);
	}
}


void AGASCombatNPC::MultiInitHealths_Implementation(float Health, float MaxHealth)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%lf, %lf"), Health, MaxHealth));
	if (EnemyStateWidget && !HasAuthority())
	{
		EnemyStateWidget->InitHealthBarPercent(Health, MaxHealth);
	}
}

void AGASCombatNPC::ServerInitHealths_Implementation()
{
	MultiInitHealths(GetCharacterAttributeSet()->GetHealth(), GetCharacterAttributeSet()->GetMaxHealth());
}

void AGASCombatNPC::SetRagdollVector_Implementation(FVector NewRagdollVector)
{
	RagdollVelocity = NewRagdollVector;
	
}

void AGASCombatNPC::MulticastRagdoll_Implementation()
{
	bIsRagdoll = true;
	// 레그돌 처리
	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetAnimInstanceClass(nullptr);
	
	// 1. 캡슐과 매시의 모든 채널 무시
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	// 2. 바닥이 될 스테틱과 다이나믹만 무시 취소
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	// 공격자의 앞 방향으로 날라가게 하기
	FVector DeadVector = RagdollVelocity * 150.f;
	//UKismetSystemLibrary::PrintString(this,RagdollVelocity.ToString());
	DeadVector.Z = 10.f;
	
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetMesh()->AddImpulseToAllBodiesBelow(DeadVector, NAME_None, true);
	
	if (Weapon)
	{
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		Weapon->Destroy();
	}
	
	
	
}





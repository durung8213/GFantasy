// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI/Boss/BossAIControllerBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GASCombatBossNPC.h"
#include "Character/GASPlayerCharacter.h"
#include "Enum/EnemyEnumLibrary.h"
#include "GAS/AttributeSet/GASBossGroggy.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "LevelSequence.h"
#include "Libraries/GASAbilitySystemLibrary.h"

DEFINE_LOG_CATEGORY(LOG_BossAIControllerBase);

void ABossAIControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ABossAIControllerBase, InAreaPlayerArr, COND_None, REPNOTIFY_Always);
}

ABossAIControllerBase::ABossAIControllerBase()
{
	
	
}

void ABossAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	BindAttributes();
}

void ABossAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ABossAIControllerBase::BindAttributes()
{
	AGASCombatBossNPC* CurrentBossCharacter = Cast<AGASCombatBossNPC>(GetPawn());
	if (CurrentBossCharacter)
	{
		UGASCharacterAS* CurrentCharacterAttributeSet = CurrentBossCharacter->GetCharacterAttributeSet();
		if (CurrentCharacterAttributeSet)
		{
			CurrentCharacterAttributeSet->NormalHitDelegate.AddDynamic(this, &ABossAIControllerBase::BossHitNormalAttack);
			CurrentCharacterAttributeSet->StrongHitDelegate.AddDynamic(this, &ABossAIControllerBase::BossHitStrongAttack);
		}

		UGASBossGroggy* CurrentBossGroggyAttributeSet = CurrentBossCharacter->GetGroggyAttributeSet();
		if (CurrentBossGroggyAttributeSet)
		{
			CurrentBossGroggyAttributeSet->OnGroggyChargeMax.AddDynamic(this, &ABossAIControllerBase::OnGroggyMax);
			CurrentBossGroggyAttributeSet->OnGroggyUseUp.AddDynamic(this, &ABossAIControllerBase::OnGroggyZero);
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("폰이 없거나 캐스트 실패"));
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ABossAIControllerBase::BindAttributes,
		0.5f,
		false
		);
	}
}

AGASPlayerCharacter* ABossAIControllerBase::FindFocusPlayer()
{
	if (InAreaPlayerArr.IsEmpty())
	{
		return nullptr;
	}
	
	if (InAreaPlayerArr.Num() == 1)
	{
		return InAreaPlayerArr[0].Player;
	}
	
	int Index = FMath::RandRange(0, InAreaPlayerArr.Num() -1);
	return InAreaPlayerArr[Index].Player;
}

AGASPlayerCharacter* ABossAIControllerBase::FindFocusPlayerByCondition()
{
	AActor* PossessActor = GetPawn();

	if (PossessActor && !InAreaPlayerArr.IsEmpty())
	{
		int ArraySize =  InAreaPlayerArr.Num();
		
		// 가까운 순으로 정렬
		InAreaPlayerArr.Sort([&PossessActor](const FInArrayPlayer& A, const FInArrayPlayer& B){
			float ADistance = UKismetMathLibrary::Vector_DistanceSquared(PossessActor->GetActorLocation(), A.Player->GetActorLocation());
			float BDistance = UKismetMathLibrary::Vector_DistanceSquared(PossessActor->GetActorLocation(), B.Player->GetActorLocation());
			
			return ADistance > BDistance;
		});
		
		for (int i = 0; i < ArraySize; i++)
		{
			InAreaPlayerArr[i].AggroValue = ArraySize - i + 2;
		}

		// 강공격 타격 순으로 정렬
		InAreaPlayerArr.Sort([&PossessActor](const FInArrayPlayer& A, const FInArrayPlayer& B){
			return A.StrongAttackCount > B.StrongAttackCount;
		});
		
		for (int i = 0; i < ArraySize; i++)
		{
			InAreaPlayerArr[i].AggroValue += FMath::Clamp(ArraySize - i + 1, 0, ArraySize) ;
		}

		// 약공격 타격 순으로 정렬
		InAreaPlayerArr.Sort([&PossessActor](const FInArrayPlayer& A, const FInArrayPlayer& B){
			return A.NormalAttackCount > B.NormalAttackCount;
		});
		
		for (int i = 0; i < ArraySize; i++)
		{
			InAreaPlayerArr[i].AggroValue += FMath::Clamp(ArraySize - i - 1, 0, ArraySize) ;
		}
		
		// Hp 적은 순으로 정렬
		InAreaPlayerArr.Sort([&PossessActor](const FInArrayPlayer& A, const FInArrayPlayer& B){
			FGameplayAttribute HealthAttr = UGASCharacterAS::GetHealthAttribute();
			return A.Player->GetAbilitySystemComponent()->GetNumericAttribute(HealthAttr) > B.Player->GetAbilitySystemComponent()->GetNumericAttribute(HealthAttr);
		});
		
		for (int i = 0; i < ArraySize; i++)
		{
			InAreaPlayerArr[i].AggroValue += FMath::Clamp(ArraySize - i + 3, 0, ArraySize) ;
		}

		// 어그로 수치 순으로 정렬
		InAreaPlayerArr.Sort([&PossessActor](const FInArrayPlayer& A, const FInArrayPlayer& B){
			FGameplayAttribute HealthAttr = UGASCharacterAS::GetHealthAttribute();
			return A.AggroValue < B.AggroValue;
		});
		
		return InAreaPlayerArr.Num() > 1 ? InAreaPlayerArr[FMath::RandRange(0, 1)].Player : InAreaPlayerArr[0].Player;
	}

	return nullptr;

	
}

void ABossAIControllerBase::PlayerInArea(AGASPlayerCharacter* NewPlayer)
{
	
	FInArrayPlayer NewPlayerInfo(NewPlayer);
	for (FInArrayPlayer CurrentArrPlayer : InAreaPlayerArr)
	{
		if (CurrentArrPlayer.Player == NewPlayer)
		{
			// 이미 들어와 있는 플레이어
			return;
		}
	}
	
	InAreaPlayerArr.AddUnique(NewPlayerInfo);

	if (InAreaPlayerArr.Num() == 1)
	{
		FocusTarget = FindFocusPlayer();
		SetFocus(FocusTarget);
		if (GetBlackboardComponent() && FocusTarget)
		{
			GetBlackboardComponent()->SetValueAsObject(FocusActor, FocusTarget);	// 포커스 해줌
			SetEnemyState(EEnemyState::Attack);
		}
		
	}

	if (GetPawn())
	{
		UE_LOG(LOG_AIControllerBase, Log, TEXT("Player %s go in %s's Ares"), *NewPlayer->GetName(), *GetPawn()->GetName());
	}
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("플레이어 %s가 보스 영역에 들어옴"), *NewPlayer->GetName()));
	
	// 어느정도 진행이 되면 플레이어가 귀환, 탈출, 텔레포트, 게임 종료 등의 방법으로 게임에서 나갔을 때 PlayerOutArea를 실행하도록 델리게이트를 바인딩 해야한다
	
}

void ABossAIControllerBase::PlayerOutArea(AGASPlayerCharacter* OutPlayer)
{
	if (!OutPlayer) return;
	 
	for (FInArrayPlayer CurrentArrPlayer : InAreaPlayerArr)
	{
		if (CurrentArrPlayer.Player == OutPlayer)
		{
			// 플레이어를 배열에서 삭제
			InAreaPlayerArr.Remove(CurrentArrPlayer);

			if (InAreaPlayerArr.IsEmpty())
			{
				ClearFocus(EAIFocusPriority::Gameplay);
				FocusTarget = nullptr;
				GetBlackboardComponent()->SetValueAsObject(FocusActor, nullptr);	// 포커스 해줌
				break;
			}
			
			if(FocusTarget && OutPlayer == FocusTarget) {
				// 만약 나간 플레이어가 타겟이라면
				FocusTarget = FindFocusPlayer();	// 새 타겟 플레이어 설정
				SetFocus(FocusTarget);
				GetBlackboardComponent()->SetValueAsObject(FocusActor, FocusTarget);	// 포커스 해줌
			}

			if (GetPawn())
			{
				UE_LOG(LOG_AIControllerBase, Log, TEXT("Player %s go out %s's Ares"), *OutPlayer->GetName(), *GetPawn()->GetName());
			}
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("플레이어 %s가 보스 영역에서 나감"), *OutPlayer->GetName()));
			break;
		}
	}
}


void ABossAIControllerBase::BossOnDie_Implementation()
{
	UE_LOG(LOG_AIControllerBase, Log, TEXT("Boss %s Die"), *GetPawn()->GetName())
	//UKismetSystemLibrary::PrintString(this, TEXT("보스 주금"));
}

void ABossAIControllerBase::MeshChangeByIndex(int Index)
{
	AGASCombatNPC* CurrentPawn = GetPawn<AGASCombatNPC>();
	if (CurrentPawn)
	{
		CurrentPawn->ChangeMeshByIndex(Index);
	}
}

void ABossAIControllerBase::SetGroggy()
{
	AGASCombatBossNPC* CurrentBossCharacter = Cast<AGASCombatBossNPC>(GetPawn());
	UAbilitySystemComponent* CurrentAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (CurrentBossCharacter && GroggyResetEffect && CurrentAbilitySystemComponent)
	{
		UGASAbilitySystemLibrary::ApplyGASEffectToSelfToASC(CurrentAbilitySystemComponent, GroggyResetEffect);
		GetBlackboardComponent()->SetValueAsBool(FName("Groggy"), true);
		CurrentBossCharacter->bIsGroggy = true;
	}
}


void ABossAIControllerBase::BossHitNormalAttack_Implementation(const FGameplayEffectSpec& EffectSpec,
                                                               const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	UKismetSystemLibrary::PrintString(this, TEXT("이거가 실행되면 안됨"));
}

void ABossAIControllerBase::BossHitStrongAttack_Implementation(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	UKismetSystemLibrary::PrintString(this, TEXT("이거가 실행되면 안됨"));
}

void ABossAIControllerBase::OnGroggyMax_Implementation()
{
	GetBlackboardComponent()->SetValueAsBool(FName("Groggy"), true);
	AGASCombatBossNPC* CurrentBossCharacter = Cast<AGASCombatBossNPC>(GetPawn());
	if (CurrentBossCharacter)
	{
		CurrentBossCharacter->bIsGroggy = true;
		UAbilitySystemComponent* CurrentAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
		if (CurrentAbilitySystemComponent)
		{
			// 피격 Effect를 적용시키기
			if (GroggyResetEffect)
			{
				// 1. 효과 컨텍스트 생성
				FGameplayEffectContextHandle EffectContext = CurrentAbilitySystemComponent->MakeEffectContext();
				EffectContext.AddInstigator(CurrentAbilitySystemComponent->GetOwner(), CurrentAbilitySystemComponent->GetOwner());
				EffectContext.AddSourceObject(CurrentAbilitySystemComponent->GetOwner());

				// 2. 효과 스펙 생성
				FGameplayEffectSpecHandle SpecHandle = CurrentAbilitySystemComponent->MakeOutgoingSpec(
					GroggyResetEffect,
					1.0f, // 레벨
					EffectContext
				);

				if (SpecHandle.IsValid())
				{
					FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
					if (Spec)
					{
						FActiveGameplayEffectHandle EffectHandle = CurrentAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
					}
					else
					{
						UE_LOG(LOG_BossAIControllerBase, Error, TEXT("Spec is not valid"));
						//UKismetSystemLibrary::PrintString(this, TEXT("Spec 없음"), true, true, FLinearColor::Red);
					}
				}
				else
				{
					UE_LOG(LOG_BossAIControllerBase, Error, TEXT("SpecHandle is not valid"));
					//UKismetSystemLibrary::PrintString(this, TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
				}
			}
			else
			{
				UE_LOG(LOG_BossAIControllerBase, Error, TEXT("AttackEffect is not valid"));
				//UKismetSystemLibrary::PrintString(this, TEXT("AttackEffect 없음"), true, true, FLinearColor::Red);
			}
		}
	}
}

void ABossAIControllerBase::OnGroggyZero_Implementation()
{
	GetBlackboardComponent()->SetValueAsBool(FName("Groggy"), false);
	AGASCombatBossNPC* CurrentBossCharacter = Cast<AGASCombatBossNPC>(GetPawn());
	if (CurrentBossCharacter)
	{
		CurrentBossCharacter->bIsGroggy = false;
	}
}

void ABossAIControllerBase::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	Super::SetFocus(NewFocus, InPriority);

	if (NewFocus == FocusTarget)
	{
		GetPawn()->bUseControllerRotationYaw = true;
	}
	else
	{
		GetPawn()->bUseControllerRotationYaw = false;
		GetWorldTimerManager().SetTimer(RotationToTargetTimerHandle, this, &ABossAIControllerBase::LerpToFocusTarget, 0.033f, true);
	}
}

void ABossAIControllerBase::TpPlayerByTime(FVector TPLocation, AGASPlayerCharacter* Player, float Time)
{
	FTimerHandle TPTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
			TPTimerHandle,
			FTimerDelegate::CreateLambda([this, TPLocation, Player]()
			{
				Player->SetActorLocation(TPLocation);
			}),
			Time,
			false
		);
	
}

void ABossAIControllerBase::LerpToFocusTarget()
{
	if (GetFocusActor())
	{
		APawn* PossessActor = GetPawn();
		FRotator CurrentRotation = PossessActor->GetActorRotation();
		FRotator TargetRotation = (GetFocusActor()->GetActorLocation() - PossessActor->GetActorLocation()).Rotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, 0.03f, 0.5f);

		PossessActor->SetActorRotation(NewRotation);

		if (TargetRotation.EqualsOrientation(PossessActor->GetActorRotation(), 1.f))
		{
			// 대충 각도 비슷 하면
			PossessActor->SetActorRotation(TargetRotation);
			PossessActor->bUseControllerRotationYaw = false;
			GetWorldTimerManager().ClearTimer(RotationToTargetTimerHandle);
		}
	}
}

void ABossAIControllerBase::SendToPlayerIfDie(AGASWorldPlayerState* CurrentPlayerState)
{
	if (AGASCombatNPC* CurrentPawn = Cast<AGASCombatNPC>( GetPawn()))
	{
		CurrentPawn->SendToPlayerIfDie(CurrentPlayerState);
	}
}

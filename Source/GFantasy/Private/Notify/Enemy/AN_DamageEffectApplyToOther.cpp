// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/Enemy/AN_DamageEffectApplyToOther.h"

#include "AbilitySystemInterface.h"
#include "GAS/AbilitySystemComponent/GASCBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UAN_DamageEffectApplyToOther::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp->GetOwner() && MeshComp->GetOwner()->HasAuthority())
	{
		// 매시의 오너인 액터가 존재하면
		IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
		if (AbilityInterface)
		{
			// 어빌리티 인터페이스가 있으면
			UGASCBase* OwnerActorASC = Cast<UGASCBase>(AbilityInterface->GetAbilitySystemComponent());

			/* 멀티 트레이스 */
			TArray<FHitResult> HitResults;
			FVector TraceStart = MeshComp->GetBoneLocation(TraceStartBoneName);
			FVector TraceEnd = TraceStart + (MeshComp->GetOwner()->GetActorForwardVector() * TracePlus);
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(MeshComp->GetOwner());
			
			
			TArray<AActor*> ActorsToIgnore;

			
			
			bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
				MeshComp->GetWorld(),
				TraceStart,
				TraceEnd,
				TraceRadius,
				ObjectTypes,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResults,
				true,
				FLinearColor::Red,
				FLinearColor::Green,
				5.f 
			);
			
			/* 멀티 트레이스 끝 */
			FString BoolAsText = bHit ? TEXT("true") : TEXT("false");
			UKismetSystemLibrary::PrintString(
				this,
				FString::Printf(
					TEXT("성공여부 : %s, 시작지점 : %s, 끝지점 : %s"),
					*BoolAsText,
					*TraceStart.ToString(),
					*TraceEnd.ToString()
					)
				);
			
			for (auto HitResult : HitResults)
			{
				IAbilitySystemInterface* OtherAbilityInterface = Cast<IAbilitySystemInterface>(HitResult.GetActor());
				if (AbilityInterface)
				{
					if (UAbilitySystemComponent* OtherActorASC = OtherAbilityInterface->GetAbilitySystemComponent())
					{
						// 피격 Effect를 적용시키기
						if (AttackEffect)
						{
							// 1. 효과 컨텍스트 생성
							FGameplayEffectContextHandle EffectContext = OwnerActorASC->MakeEffectContext();
							EffectContext.AddInstigator(OwnerActorASC->GetOwner(), OwnerActorASC->GetOwner());
							EffectContext.AddSourceObject(OwnerActorASC->GetOwner());

							// 2. 효과 스펙 생성
							FGameplayEffectSpecHandle SpecHandle = OwnerActorASC->MakeOutgoingSpec(
								AttackEffect,
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
										UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
									}
									else
									{
										UKismetSystemLibrary::PrintString(this, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true, FLinearColor::Red);
									}
								}
								else
								{
									UKismetSystemLibrary::PrintString(this, TEXT("Spec 없음"), true, true, FLinearColor::Red);
								}
							}
							else
							{
								UKismetSystemLibrary::PrintString(this, TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
							}
						}
						else
						{
							UKismetSystemLibrary::PrintString(this, TEXT("AttackEffect 없음"), true, true, FLinearColor::Red);
						}
					}
					else
					{
						UKismetSystemLibrary::PrintString(this, TEXT("피격 액터가 ASC 없음"), true, true, FLinearColor::Red);
					}
				}
				else
				{
					UKismetSystemLibrary::PrintString(this, TEXT("피격 액터가 ASC 인터페이스 없음"), true, true, FLinearColor::Red);
				}
			}
		}
	}

	
	
	

	
}

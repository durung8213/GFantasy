// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/GASAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GFantasy/Public/GameMode/BaseGameMode.h"

#include "Kismet/GameplayStatics.h"

UCharacterClassInfo* UGASAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject* WorldContextObject)
{
	if (const ABaseGameMode* GFGameMode = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GFGameMode->GetCharacterClassDefaultInfo();
	}

	return nullptr;
}

bool UGASAbilitySystemLibrary::ApplyGASEffectToSelf(AActor* SelfActor, TSubclassOf<UGameplayEffect> Effect, float Level)
{
	if (!SelfActor)
	{
		return false;
	}
	
	UAbilitySystemComponent* SelfASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SelfActor);
	
	if (SelfASC && Effect)
	{
		// 1. 효과 컨텍스트 생성
		FGameplayEffectContextHandle EffectContext = SelfASC->MakeEffectContext();
		EffectContext.AddInstigator(SelfASC->GetOwner(), SelfASC->GetOwner());
		EffectContext.AddSourceObject(SelfASC->GetOwner());

		// 2. 효과 스펙 생성
		FGameplayEffectSpecHandle SpecHandle = SelfASC->MakeOutgoingSpec(
			Effect,
			Level, // 레벨
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec)
			{
				FActiveGameplayEffectHandle EffectHandle = SelfASC->ApplyGameplayEffectSpecToSelf(*Spec);
				if (EffectHandle.IsValid())
				{
					// UKismetSystemLibrary::PrintString(
					// 	SelfActor, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
					return true;
				}
				else
				{
					// UKismetSystemLibrary::PrintString(
					// 	SelfActor, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true,
					// 	FLinearColor::Red);
				}
			}
			else
			{
				//UKismetSystemLibrary::PrintString(SelfActor, TEXT("Spec 없음"), true, true, FLinearColor::Red);
			}
		}
		else
		{
			//UKismetSystemLibrary::PrintString(SelfActor, TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
		}
	}
	else
	{
		//UKismetSystemLibrary::PrintString(SelfActor, TEXT("피격 액터가 ASC 인터페이스 없음"), true, true, FLinearColor::Red);
	}
	return false;
			
}

bool UGASAbilitySystemLibrary::ApplyGASEffectToSelfToASC(UAbilitySystemComponent* SelfAbilitySystemComponent,
	TSubclassOf<UGameplayEffect> Effect, float Level)
{
	if (SelfAbilitySystemComponent && Effect)
	{
		// 1. 효과 컨텍스트 생성
		FGameplayEffectContextHandle EffectContext = SelfAbilitySystemComponent->MakeEffectContext();
		EffectContext.AddInstigator(SelfAbilitySystemComponent->GetOwner(), SelfAbilitySystemComponent->GetOwner());
		EffectContext.AddSourceObject(SelfAbilitySystemComponent->GetOwner());

		// 2. 효과 스펙 생성
		FGameplayEffectSpecHandle SpecHandle = SelfAbilitySystemComponent->MakeOutgoingSpec(
			Effect,
			Level, // 레벨
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec)
			{
				FActiveGameplayEffectHandle EffectHandle = SelfAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
				if (EffectHandle.IsValid())
				{
					// UKismetSystemLibrary::PrintString(
					// 	SelfAbilitySystemComponent->GetOwnerActor(), TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
					return true;
				}
				else
				{
					// UKismetSystemLibrary::PrintString(
					// 	SelfAbilitySystemComponent->GetOwnerActor(), TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true,
					// 	FLinearColor::Red);
				}
			}
			else
			{
				//UKismetSystemLibrary::PrintString(SelfAbilitySystemComponent->GetOwnerActor(), TEXT("Spec 없음"), true, true, FLinearColor::Red);
			}
		}
	}

	return false;
}

bool UGASAbilitySystemLibrary::ApplyGASEffectToTarget(AActor* SourceActor, AActor* TargetActor,
	TSubclassOf<UGameplayEffect> Effect, float Level)
{
	if (!SourceActor || !TargetActor || !Effect)
	{
		return false;
	}
	
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (SourceASC && TargetASC)
	{
		// 1. 효과 컨텍스트 생성
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		EffectContext.AddInstigator(SourceASC->GetOwner(), SourceASC->GetOwner());
		EffectContext.AddSourceObject(SourceASC->GetOwner());

		// 2. 효과 스펙 생성
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
			Effect,
			Level, // 레벨
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec)
			{
				FActiveGameplayEffectHandle EffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*Spec ,TargetASC);
				if (EffectHandle.IsValid())
				{
					// UKismetSystemLibrary::PrintString(
					// 	SourceActor, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
					return true;
				}
				else
				{
					// UKismetSystemLibrary::PrintString(
					// 	SourceActor, TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true,
					// 	FLinearColor::Red);
				}
			}
			else
			{
				//UKismetSystemLibrary::PrintString(SourceActor, TEXT("Spec 없음"), true, true, FLinearColor::Red);
			}
		}
		else
		{
			//UKismetSystemLibrary::PrintString(SourceActor, TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
		}
	}
	else
	{
		//UKismetSystemLibrary::PrintString(SourceActor, TEXT("피격 액터가 ASC 인터페이스 없음"), true, true, FLinearColor::Red);
	}
	return false;
}

bool UGASAbilitySystemLibrary::ApplyGASEffectToTargetToASC(UAbilitySystemComponent* SourceAbilitySystemComponent,
	UAbilitySystemComponent* TargetAbilitySystemComponent, TSubclassOf<UGameplayEffect> Effect, float Level)
{	
	if (SourceAbilitySystemComponent && TargetAbilitySystemComponent && Effect)
	{
		// 1. 효과 컨텍스트 생성
		FGameplayEffectContextHandle EffectContext = SourceAbilitySystemComponent->MakeEffectContext();
		EffectContext.AddInstigator(SourceAbilitySystemComponent->GetOwner(), SourceAbilitySystemComponent->GetOwner());
		EffectContext.AddSourceObject(SourceAbilitySystemComponent->GetOwner());

		// 2. 효과 스펙 생성
		FGameplayEffectSpecHandle SpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(
			Effect,
			Level, // 레벨
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec)
			{
				FActiveGameplayEffectHandle EffectHandle = SourceAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec ,TargetAbilitySystemComponent);
				if (EffectHandle.IsValid())
				{
					// UKismetSystemLibrary::PrintString(
					// 	SourceAbilitySystemComponent->GetOwnerActor(), TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 성공"));
					return true;
				}
				else
				{
					// UKismetSystemLibrary::PrintString(
					// 	SourceAbilitySystemComponent->GetOwnerActor(), TEXT("ApplyGameplayEffectSpecToTarget로 Attack Effect 적용 실패"), true, true,
					// 	FLinearColor::Red);
				}
			}
			else
			{
				//UKismetSystemLibrary::PrintString(SourceAbilitySystemComponent->GetOwnerActor(), TEXT("Spec 없음"), true, true, FLinearColor::Red);
			}
		}
		else
		{
			//UKismetSystemLibrary::PrintString(SourceAbilitySystemComponent->GetOwnerActor(), TEXT("SpecHandle 없음"), true, true, FLinearColor::Red);
		}
	}
	else
	{
		//UKismetSystemLibrary::PrintString(SourceAbilitySystemComponent->GetOwnerActor(), TEXT("피격 액터가 ASC 인터페이스 없음"), true, true, FLinearColor::Red);
	}
	return false;
}


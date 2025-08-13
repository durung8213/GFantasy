// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GASAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
class UCharacterClassInfo;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class GFANTASY_API UGASAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/*캐릭터 Info 설정을 GM을 통해서 하도록 만든 라이브러리 함수*/
	UFUNCTION(BlueprintPure)
	static UCharacterClassInfo* GetCharacterClassDefaultInfo(const UObject* WorldContextObject);
	
	/*인벤토리에서 아이템 찾기 위한 템플릿 함수*/
	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag);

	/**
	 * <summary>
	 * 자기 자신에게 Effect를 적용한다
	 * </summary>
	 * <param name="SelfActor">이팩트를 적용할 액터</param>
	 * <param name="Effect">적용할 Effect</param>
	 * <param name="Level">Effect Level</param>
	 * <returns>적용 여부(이팩트가 Instant일 경우 적용에 성공했어도 false가 반환됨)</returns>
	 */
	UFUNCTION(BlueprintCallable, Category="GAS Library")
	static bool ApplyGASEffectToSelf(AActor* SelfActor, TSubclassOf<UGameplayEffect> Effect, float Level = 1.f);

	/**
	 * <summary>
	 * 자기 자신에게 Effect를 적용한다
	 * </summary>
	 * <param name="SelfAbilitySystemComponent">이팩트를 적용할 어빌리티 시스템 컴포넌트</param>
	 * <param name="Effect">적용할 Effect</param>
	 * <param name="Level">Effect Level</param>
	 * <returns>적용 여부(이팩트가 Instant일 경우 적용에 성공했어도 false가 반환됨)</returns>
	 */
	UFUNCTION(BlueprintCallable, Category="GAS Library")
	static bool ApplyGASEffectToSelfToASC(UAbilitySystemComponent* SelfAbilitySystemComponent, TSubclassOf<UGameplayEffect> Effect, float Level = 1.f);

	/**
	 * <summary>
	 * 타인에게 이팩트를 적용한다
	 * </summary>
	 * <param name="SourceActor">이팩트를 적동시킨 액터</param>
	 * <param name="TargetActor">이팩트를 적용할 액터</param>
	 * <param name="Effect">적용할 Effect</param>
	 * <param name="Level">Effect Level</param>
	 * <returns>적용 여부(이팩트가 Instant일 경우 적용에 성공했어도 false가 반환됨)</returns>
	 */
	UFUNCTION(BlueprintCallable, Category="GAS Library")
	static bool ApplyGASEffectToTarget(AActor* SourceActor, AActor* TargetActor, TSubclassOf<UGameplayEffect> Effect, float Level = 1.f);

	/**
	 * <summary>
	 * 타인에게 이팩트를 적용한다
	 * </summary>
	 * <param name="SourceAbilitySystemComponent">이팩트를 적동시킨 액터</param>
	 * <param name="TargetAbilitySystemComponent">이팩트를 적용할 어빌리티 시스템 컴포넌트</param>
	 * <param name="Effect">적용할 Effect</param>
	 * <param name="Level">Effect Level</param>
	 * <returns>적용 여부(이팩트가 Instant일 경우 적용에 성공했어도 false가 반환됨)</returns>
	 */
	UFUNCTION(BlueprintCallable, Category="GAS Library")
	static bool ApplyGASEffectToTargetToASC(UAbilitySystemComponent* SourceAbilitySystemComponent, UAbilitySystemComponent* TargetAbilitySystemComponent, TSubclassOf<UGameplayEffect> Effect, float Level = 1.f);
	
	
};

template<typename T>
T* UGASAbilitySystemLibrary::GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag)
{
	/*Tag를 기반으로 아이템을 찾음*/
	return DataTable->FindRow<T>(Tag.GetTagName(), FString(""));
}

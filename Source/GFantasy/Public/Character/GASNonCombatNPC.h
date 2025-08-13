// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GFantasy/GFantasy.h"
#include "Character/GASBaseCharacter.h"
#include "Interface/InteractionInterface.h"
#include "GASNonCombatNPC.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API AGASNonCombatNPC : public AGASBaseCharacter, public IInteractionInterface
{
	GENERATED_BODY()
	
public:
	AGASNonCombatNPC();

protected:

	// ���Ͱ� �����ǰų� ������Ƽ ���� �� ȣ��
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;



public:
	// FŰ �������� NPC�� ��ȣ�ۿ��ϴ� �������̽� �Լ�
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	// FŰ �������� �ߴ� ����
	// NPC����(����Ʈ, ����, ���)�� ���� �ٸ� ������ ��Ÿ������ �� �ִ�.
	// �ٵ� ���߿� �׳� �� �������� �̸��� �ߴ� ������ �ٸ��� �ؾ߰ڴ�.
	virtual void ShowInteractionWidget(AActor* Interactor);

	FGameplayTag GetNPCNameTag() const;

	class UWidgetComponent* GetGuideWidgetComponent() const;

protected:
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual void ShowGuideWidget();
	virtual void HideGuideWidget();

	static FText GetNameFromTag(const FGameplayTag& InTag);
	//FText GetKoreanName();

	UFUNCTION(BlueprintCallable, Category = "NPC")
	virtual FText ChangeTagToName();

	void SetNameInWidget();

	UFUNCTION(BlueprintCallable, Category = "NPC")
	virtual FText ChangeTypeToText();

	void SetTypeInWidget();


	// ī�޶�� �̸� ���� ���̿� ��ֹ��� ������ ����
	void CheckNameVisibility();

	// ī�޶�� �̸� ���� ���̿� ��ֹ��� ������ ����
	void CheckTypeVisibility();



protected:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* AreaSphere;

	// �̸� �ٿ��� ��ȣ�ۿ� ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI")
	class UWidgetComponent* GuideWidgetComponent;

	// �⺻���� ����� ���� Ŭ���� (��������Ʈ �ڽĿ��� ���� ����)
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|UI")
	TSubclassOf<class UNPCGuideWidget> DefaultGuideWidgetClass;

	// �̸� ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name|UI")
	class UWidgetComponent* NameWidgetComponent;

	// �⺻���� ����� ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Name|UI")
	TSubclassOf<class UNPCNameWidget> DefaultNameWidgetClass;

	// Ÿ�� ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name|UI")
	class UWidgetComponent* TypeWidgetComponent;

	// �⺻���� ����� ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Name|UI")
	TSubclassOf<class UNPCNameWidget> DefaultTypeWidgetClass;

	// �±׸� �̿��� NPC�� �ѱ��̸��� ������ ������ ���̺�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Name")
	UDataTable* NPCNameDataTable;
	
	// �±׸� �̿��� NPC�� Ÿ���� ������ ������ ���̺�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Name")
	UDataTable* NPCTypeDataTable;


private:
	// enum���� Ÿ�� ����
	UPROPERTY(EditAnywhere, Category = "Type")
	ENPCType NPCType;

	UPROPERTY(EditAnywhere, Category = "Name|Tag")
	FGameplayTag NPCNameTag = FGameplayTag();

	UPROPERTY(EditAnywhere)
	FText NPCName;

	UPROPERTY(EditAnywhere)
	FText NPCKoreanType;
};

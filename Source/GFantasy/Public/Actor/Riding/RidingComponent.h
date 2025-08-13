
#pragma once

#include "CoreMinimal.h"
#include <UObject/CoreNet.h>
#include "Components/ActorComponent.h"
#include "Actor/Riding/RidingActorBase.h"
#include "GameplayEffect.h"
#include "RidingComponent.generated.h"

class AGASPlayerCharacter;
class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GFANTASY_API URidingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URidingComponent();

	UFUNCTION(Server,Reliable)
	void Server_Mount();

	UFUNCTION(Server,Reliable)
	void Server_UnMount();


	UFUNCTION(BlueprintCallable)
	void Mount();

	UFUNCTION()
	void UnMount();
	
	UFUNCTION()
	bool IsMounted() const { return SpawnedRidingActor != nullptr; }

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MountVFX(FVector Location);

	UFUNCTION(Server, Reliable)
	void Server_SpawnMountAtFront();

	// ���� �θ��� �����Ƽ ����
	UFUNCTION()
	void MountCall();
public:

	UPROPERTY(EditAnywhere, Category = "Riding")
	TSubclassOf<ARidingActorBase> RidingActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	ARidingActorBase* SpawnedRidingActor;
	// ���� Ÿ�� ���� ĳ������ IMC ���¸� �����ߴٰ� ���.
	UPROPERTY()
	UInputMappingContext* CachedIMC;

	// ���� VFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Riding")
	UNiagaraSystem* PuffVFX;

	// ��ٿ� GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Riding")
	TSubclassOf<UGameplayEffect> MountCooldownEffect;
	
protected:
	UPROPERTY()
	AGASPlayerCharacter* CachedPlayer;


protected:
	// Called when the game startsw
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};

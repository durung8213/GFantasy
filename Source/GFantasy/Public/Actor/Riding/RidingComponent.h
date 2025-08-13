
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

	// 말을 부르는 어빌리티 실행
	UFUNCTION()
	void MountCall();
public:

	UPROPERTY(EditAnywhere, Category = "Riding")
	TSubclassOf<ARidingActorBase> RidingActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	ARidingActorBase* SpawnedRidingActor;
	// 말을 타기 전의 캐릭터의 IMC 상태를 저장했다가 사용.
	UPROPERTY()
	UInputMappingContext* CachedIMC;

	// 등장 VFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Riding")
	UNiagaraSystem* PuffVFX;

	// 쿨다운 GE
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

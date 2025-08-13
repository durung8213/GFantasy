// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Chest/ChestBase.h"

#include "Actor/Item/ItemSpawn.h"
#include "Character/GASPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "Controller/GASWorldPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(ChestBase);

// Sets default values
AChestBase::AChestBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	ItemSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSpawnPoint"));
	ItemSpawnPoint->SetupAttachment(Root);
	
	ChestBodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestBodyMeshComponent"));
	ChestBodyMeshComponent->SetupAttachment(RootComponent);

	ChestTopMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestTopMeshComponent"));
	ChestTopMeshComponent->SetupAttachment(RootComponent);

	ChestOpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ChestOpenTimeline"));


	// SphereComponent 생성 및 초기 설정
	AreaSphere = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBox"));
	AreaSphere->SetupAttachment(RootComponent);
	// 커스텀 생성한 오브젝트 타입은 컴퓨터별로 다르니까 그냥 블루프린트에서 설정할 수 있도록
	//AreaSphere->SetCollisionObjectType(/*ECC_WorldDynamic*/ ECC_GameTraceChannel5);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	// 쿼리만, 물리적으로 충돌하지 않음
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// 모든 채널 무시(Overlap/Block 안함)

	// 상호작용용 WidgetComponent
	GuideWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	GuideWidgetComponent->SetupAttachment(RootComponent);
	GuideWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	GuideWidgetComponent->SetDrawAtDesiredSize(true);
	GuideWidgetComponent->SetRelativeLocation(FVector(0, 0, 30.f));
	GuideWidgetComponent->SetHiddenInGame(true);	// 기본적으로 숨김 처리
}

void AChestBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AChestBase, ItemTag, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AChestBase, ItemCountRangeMin, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AChestBase, ItemCountRangeMax, COND_None, REPNOTIFY_Always);
}


// Called when the game starts or when spawned
void AChestBase::BeginPlay()
{
	Super::BeginPlay();
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
}

// Called every frame
void AChestBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChestBase::Interact_Implementation(AActor* Interactor)
{
	IInteractionInterface::Interact_Implementation(Interactor);
	UE_LOG(ChestBase, Error, TEXT("ChestOpen by %s"), *Interactor->GetName());
	//UKismetSystemLibrary::PrintString(this, TEXT("상자 열림 시작"));

	if (HasAuthority())
	{
		AGASPlayerCharacter* CurrentInteractPlayer =  Cast<AGASPlayerCharacter>(Interactor);
		
		if (CurrentInteractPlayer)
		{
			SetOwner(CurrentInteractPlayer->GetController());
		}
		
		ClientOpenChest();
	}
	
	
}

void AChestBase::ClientOpenChest_Implementation()
{
	if (bIsOpened) return;
	
	//UKismetSystemLibrary::PrintString(this, TEXT("상자 열림 타임라인 가동"));
	//UE_LOG(ChestBase, Error, TEXT("ChestOpen by %s"), *Interactor->GetName());
	if (ChestOpenCurveFloat && ChestOpenTimeline)
	{
		// 델리게이트 바인딩
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("ChestOpenTimelineProgress"));

		FOnTimelineEvent TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("ChestOpenTimelineFinished"));
		
		ChestOpenTimeline->AddInterpFloat(ChestOpenCurveFloat, ProgressFunction);
		ChestOpenTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
		ChestOpenTimeline->SetLooping(false); // 루프 여부
		ChestOpenTimeline->SetPlayRate(1.0f); // 속도

		ChestOpenTimeline->PlayFromStart(); // 재생
	}

	const FTransform Transform(FRotator(0,FMath::RandRange(0.f, 360.f), 0), ItemSpawnPoint->GetComponentLocation());
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = Cast<APawn>(Params.Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int i = 0; i < FMath::RandRange(ItemCountRangeMin, ItemCountRangeMax); i++)
	{
		AItemSpawn* ItemSpawn = GetWorld()->SpawnActorDeferred<AItemSpawn>(
		SpawnItem,
		Transform,
		Params.Owner,
		Params.Instigator,
		Params.SpawnCollisionHandlingOverride
	);
		if (ItemSpawn)
		{
			ItemSpawn->bRandomSpawn = false;
			// 액터의 전방 벡터
			FVector ForwardVector = GetActorForwardVector();

			// 회전 기준: 액터의 현재 회전
			FRotator BaseRotation = ForwardVector.Rotation();

			// Yaw를 -30도 ~ 30도 랜덤으로 변경
			float RandomYawOffset = FMath::RandRange(-30.0f, 30.0f);
			FRotator NewRotation = BaseRotation;
			NewRotation.Yaw += RandomYawOffset;

			// 방향 벡터로 변환
			FVector ResultDirection = NewRotation.Vector();
		
			ItemSpawn->RandomSpawnVector = ResultDirection * 300.f + FVector(0,0,FMath::RandRange(300.f, 600.0f));
			ItemSpawn->ItemTag = ItemTag;
			ItemSpawn->FinishSpawning(ItemSpawn->GetTransform());
		}

	}
	
	bIsOpened = true;
}

void AChestBase::ChestOpenTimelineProgress(float Value)
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("상자 Roll 값 : %lf"), Value), true, true, FLinearColor(0.0f, 0.66f, 1.0f), 0.03f);
	ChestTopMeshComponent->SetRelativeRotation(FRotator(0, -90, Value));
}

void AChestBase::ChestOpenTimelineFinished()
{
	
}

void AChestBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGASPlayerCharacter* PlayerCharacter = Cast<AGASPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		// 해당 플레이어의 PC 얻기
		if (AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(PlayerCharacter->GetController()))
		{
			// 클라이언트 RPC 호출: 이 NPC의 GuideWidget을 보이도록
			PC->ClientSetBoxGuideWidgetVisibility(this, true);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GASNonCombatNPC : No AGASPlayerCharacter."));
	}
}

void AChestBase::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGASPlayerCharacter* PlayerCharacter = Cast<AGASPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		
		if (AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(PlayerCharacter->GetController()))
		{
			// 클라이언트 RPC 호출: 이 NPC의 GuideWidget을 숨기도록
			PC->ClientSetBoxGuideWidgetVisibility(this, false);
		}
	}
}

class UWidgetComponent* AChestBase::GetGuideWidgetComponent() const
{
	if (GuideWidgetComponent)
	{
		return GuideWidgetComponent;
	}
	else
	{
		return nullptr;
	}
}



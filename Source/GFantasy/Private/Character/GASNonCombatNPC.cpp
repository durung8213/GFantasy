// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASNonCombatNPC.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Controller/GASWorldPlayerController.h"
#include "HUD/WorldHUD.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "UI/NPC/NPCGuideWidget.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GFantasy/GFantasy.h"
#include "Data/NPCNameRow.h"
#include "Engine/DataTable.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/NPC/NPCNameWidget.h"
#include "UI/NPC/NPCTypeWidget.h"
#include "Data/NPCTypeRow.h"

AGASNonCombatNPC::AGASNonCombatNPC()
{
	PrimaryActorTick.bCanEverTick = true;

	// NPC의 CapsuleComponent 설정
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		// ObjectType을 Pawn채널로 지정
		Capsule->SetCollisionObjectType(ECC_Pawn/*ECC_GameTraceChannel4*/);
	}

	// SphereComponent 생성 및 초기 설정
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	// 커스텀 생성한 오브젝트 타입은 컴퓨터별로 다르니까 그냥 블루프린트에서 설정할 수 있도록
	//AreaSphere->SetCollisionObjectType(/*ECC_WorldDynamic*/ ECC_GameTraceChannel5);
	AreaSphere->SetSphereRadius(300.f);	// 범위 반경 설정
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	// 쿼리만, 물리적으로 충돌하지 않음
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// 모든 채널 무시(Overlap/Block 안함)

	// 이름을 표시할 WidgetComponent
	NameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NPCNameWidget"));
	NameWidgetComponent->SetupAttachment(RootComponent);
	NameWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameWidgetComponent->SetDrawAtDesiredSize(true);
	NameWidgetComponent->SetRelativeLocation(FVector(0, 0, 120.f));

	// 타입을 표시할 WidgetComponent
	TypeWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NPCTypeWidget"));
	TypeWidgetComponent->SetupAttachment(RootComponent);
	TypeWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TypeWidgetComponent->SetDrawAtDesiredSize(true);
	TypeWidgetComponent->SetRelativeLocation(FVector(0, 0, 150.f));

	// 상호작용용 WidgetComponent
	GuideWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	GuideWidgetComponent->SetupAttachment(RootComponent);
	GuideWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	GuideWidgetComponent->SetDrawAtDesiredSize(true);
	GuideWidgetComponent->SetRelativeLocation(FVector(0, 0, 30.f));
	GuideWidgetComponent->SetHiddenInGame(true);	// 기본적으로 숨김 처리

	// C++ 디폴트로 WBP_NPCGuide 로드 (에디터에서 바꾸지 않으면 이 값 사용)
	static ConstructorHelpers::FClassFinder<UNPCGuideWidget> WidgetBP(TEXT("/Game/UI/NPC/WBP_NPCGuide"));
	if (WidgetBP.Succeeded())
	{
		DefaultGuideWidgetClass = WidgetBP.Class;
		GuideWidgetComponent->SetWidgetClass(DefaultGuideWidgetClass);
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_NPCName(TEXT("/Game/BluePrints/Data/NPC/DT_NPCName"));
	if (DT_NPCName.Succeeded())
	{
		NPCNameDataTable = DT_NPCName.Object;
	}
	else
	{
		// 경로가 틀렸거나 에셋이 없으면 경고 로그 출력
		UE_LOG(LogTemp, Warning, TEXT("GASNonCombatNPC : Cant Find NPCNameDataTable"));
		NPCNameDataTable = nullptr;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_NPCType(TEXT("/Game/BluePrints/Data/NPC/DT_NPCType"));
	if (DT_NPCType.Succeeded())
	{
		NPCTypeDataTable = DT_NPCType.Object;
	}
	else
	{
		// 경로가 틀렸거나 에셋이 없으면 경고 로그 출력
		UE_LOG(LogTemp, Warning, TEXT("GASNonCombatNPC : Cant Find NPCTypeDataTable"));
		NPCTypeDataTable = nullptr;
	}
}

void AGASNonCombatNPC::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGASNonCombatNPC::BeginPlay()
{
	Super::BeginPlay();

	if (NPCNameTag.IsValid())
	{
		if (NPCNameDataTable)
		{
			// 태그를 한글 이름으로 변환 후 이름 텍스트 렌더에 대입
			ChangeTagToName();
			SetNameInWidget();
		}
		else
		{
			// 예외 처리: 태그나 데이터테이블이 없으면, 텍스트를 숨기거나 기본 이름으로 셋팅 가능
			GetNameFromTag(NPCNameTag);
			SetNameInWidget();
		}
	}
	else
	{
		// 예외 처리: 태그나 데이터테이블이 없으면, 텍스트를 숨기거나 기본 이름으로 셋팅 가능
		GetNameFromTag(NPCNameTag);
		SetNameInWidget();
	}

	if (NPCTypeDataTable)
	{
		ChangeTypeToText();
	}
	else
	{
		// 데이터테이블이 없으면 enum 이름으로 대체
		NPCKoreanType = FText::FromString(UEnum::GetValueAsString(TEXT("ENPCType"), NPCType));
	}
	SetTypeInWidget();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
}

void AGASNonCombatNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 프레임마다 가시선 검사
	CheckNameVisibility();
	CheckTypeVisibility();
}

void AGASNonCombatNPC::Interact_Implementation(AActor* Interactor)
{
    if (!Interactor)
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("NonCombat Interact : %s"), *GetName());

	ShowInteractionWidget(Interactor);
}

void AGASNonCombatNPC::ShowInteractionWidget(AActor* Interactor)
{    
	// 1) 인터랙트한 게 Pawn 이라면
	if (APawn* Pawn = Cast<APawn>(Interactor))
	{
		// 2) Pawn 의 소유 컨트롤러 가져오기
		if (AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(Pawn->GetController()))
		{
			UE_LOG(LogTemp, Log, TEXT("NonCombat Interact: GASWorldPC"));

			// 3) 서버 → 클라이언트 RPC 호출 / NPCTag전달
			MyPC->ClientShowInteractionWidget(NPCNameTag, NPCName, NPCType);
			UE_LOG(LogTemp, Log, TEXT("NonCombatNPC : NPCNameTag = %s, NPCName = %s"), *NPCNameTag.ToString(), *NPCName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("NonCombat Interact: not GASWorldPC"));
		}
	}
}

void AGASNonCombatNPC::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString Msg = FString::Printf(
		TEXT("GASNonCombatNPC: OtherActor = %s | Class = %s | Component = %s"),
		*OtherActor->GetName(),
		*OtherActor->GetClass()->GetName(),
		*OtherComp->GetName()
	);
	// 화면 상단에 초록색으로 2초간 표시
	UKismetSystemLibrary::PrintString(GetWorld(), Msg, true, true, FLinearColor::Green, 2.f);

	AGASPlayerCharacter* PlayerCharacter = Cast<AGASPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		// 해당 플레이어의 PC 얻기
		if (AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(PlayerCharacter->GetController()))
		{
			// 클라이언트 RPC 호출: 이 NPC의 GuideWidget을 보이도록
			PC->ClientSetGuideWidgetVisibility(this, true);
		}
		//ShowGuideWidget();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GASNonCombatNPC : No AGASPlayerCharacter."));
	}
}

void AGASNonCombatNPC::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGASPlayerCharacter* PlayerCharacter = Cast<AGASPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("GASNonCombatNPC : CHaracter Out Sphere"));
		//HideGuideWidget();
		
		if (AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(PlayerCharacter->GetController()))
		{
			// 클라이언트 RPC 호출: 이 NPC의 GuideWidget을 숨기도록
			PC->ClientSetGuideWidgetVisibility(this, false);
		}
	}
}

void AGASNonCombatNPC::ShowGuideWidget()
{
	if (GuideWidgetComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("GASNonCombatNPC : Show Widget"))

		GuideWidgetComponent->SetHiddenInGame(false);
	}
}

void AGASNonCombatNPC::HideGuideWidget()
{
	if (GuideWidgetComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("GASNonCombatNPC : Hide Widget"))

		GuideWidgetComponent->SetHiddenInGame(true);
	}
}

FText AGASNonCombatNPC::GetNameFromTag(const FGameplayTag& InTag)
{
	UE_LOG(LogTemp, Log, TEXT("GASNonCombatNPC : GetNameFromTag"))

	// 인자가 유효하지 않을 경우 빈 텍스트 반환
	if (!InTag.IsValid())
	{
		return FText::GetEmpty();
	}

	FString FullString = InTag.GetTagName().ToString();
	int32 LastDotIndex = INDEX_NONE;

	if (FullString.FindLastChar('.', LastDotIndex))
	{
		FString SubString = FullString.Mid(LastDotIndex + 1);
		return FText::FromString(SubString);
	}

	return FText::FromString(FullString);
}

FText AGASNonCombatNPC::ChangeTagToName()
{
	// FindRow를 사용하려면 “RowName”이 필요하지만 FNPCNameRow 안의 D_NPCNameTag 필드를 비교해야 하기 때문에
	// 전체 로우를 순회하면서 일치하는 태그를 찾는 방식을 사용

	static const FString ContextString(TEXT("NPC Name Lookup"));
	// ContextString은 에디터에서 이름 검색 중 오류가 발생했을 때 로그에 Context로 출력됩니다.

	// 모든 로우를 가져와 순회
	TArray<FNPCNameRow*> AllRows;
	NPCNameDataTable->GetAllRows(ContextString, /*out*/ AllRows);

	// 매칭되는 행을 찾았는지 여부를 기록할 변수
	const FNPCNameRow* MatchedRow = nullptr;

	for (const FNPCNameRow* Row : AllRows)
	{
		if (!Row)
		{
			continue;
		}

		// 태그가 일치하는지 비교
		if (Row->D_NPCNameTag.MatchesTagExact(NPCNameTag))
		{
			MatchedRow = Row;
			break;
		}
	}

	// 매칭된 로우가 있는지 확인
	if (MatchedRow)
	{
		// D_NPCKoreanName 값을 멤버 변수에 저장
		NPCName = MatchedRow->D_NPCKoreanName;
	}
	else
	{
		// 일치하는 태그를 찾지 못했다면, 로그를 띄우고
		UE_LOG(LogTemp, Warning, TEXT("GASNonCombatNPC : Cant Find Name Match Tag(%s)"),
			*NPCNameTag.GetTagName().ToString());

		// 태그를 잘라서 가져오기(영어)
		NPCName = GetNameFromTag(NPCNameTag);
	}

	// TagTextRender에 최종 NPCName을 반영

	UE_LOG(LogTemp, Log, TEXT("NPCName = %s"), *NPCName.ToString());
	return NPCName;
}

void AGASNonCombatNPC::SetNameInWidget()
{
	if (!NameWidgetComponent)
	{
		return;
	}

	// 1) NameWidgetComponent가 UserWidget 인스턴스를 가지고 있는지 확인
	if (UUserWidget* UserWidget = NameWidgetComponent->GetUserWidgetObject())
	{
		// 2) UserWidget을 우리가 만든 UNPCNameWidget으로 캐스트
		if (UNPCNameWidget* NameWidget = Cast<UNPCNameWidget>(UserWidget))
		{
			// 3) 내부 TextBlock에 이름을 설정
			NameWidget->SetNPCName(NPCName);
		}
	}
}

FText AGASNonCombatNPC::ChangeTypeToText()
{
	// ContextString: 에디터 에러 시 로그에 찍힐 식별자
	static const FString ContextString(TEXT("NPC Type Lookup"));

	// 데이터테이블에서 모든 행을 가져와 순회
	TArray<FNPCTypeRow*> AllRows;
	NPCTypeDataTable->GetAllRows(ContextString, /*out*/ AllRows);

	const FNPCTypeRow* MatchedRow = nullptr;
	for (const FNPCTypeRow* Row : AllRows)
	{
		if (!Row) continue;

		// ENPCType 열 값이 현재 NPCType과 일치하면 그 행을 선택
		if (Row->D_NPCType == NPCType)
		{
			MatchedRow = Row;
			break;
		}
	}

	if (MatchedRow)
	{
		// 찾아낸 한글명을 멤버 변수에 저장
		NPCKoreanType = MatchedRow->D_NPCTypeName;
	}
	else
	{
		// 일치하는 행이 없으면 로그 경고 후, enum 이름(String)으로 대체
		UE_LOG(LogTemp, Warning,
			TEXT("GASNonCombatNPC: Can't find type match for %d"), (int32)NPCType);

		// ENPCType enum을 문자열로 변환
		FString EnumName = UEnum::GetValueAsString(TEXT("ENPCType"), NPCType);
		NPCKoreanType = FText::FromString(EnumName);
	}

	UE_LOG(LogTemp, Log, TEXT("GASNonCombatNPC: NPCKoreanType = %s"), *NPCKoreanType.ToString());
	return NPCKoreanType;
}

void AGASNonCombatNPC::SetTypeInWidget()
{
	if (!TypeWidgetComponent)
	{
		return;
	}

	// 1) NameWidgetComponent가 UserWidget 인스턴스를 가지고 있는지 확인
	if (UUserWidget* UserWidget = TypeWidgetComponent->GetUserWidgetObject())
	{
		// 2) UserWidget을 우리가 만든 UNPCTypeWidget으로 캐스트
		if (UNPCTypeWidget* NPCTypeWidget = Cast<UNPCTypeWidget>(UserWidget))
		{
			// 3) 내부 TextBlock에 이름을 설정
			NPCTypeWidget->SetNPCTypeName(NPCKoreanType);
		}
	}
}

void AGASNonCombatNPC::CheckNameVisibility()
{
	if (!NameWidgetComponent || !NameWidgetComponent->GetWidget())
		return;

	// 카메라 위치 가져오기
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	// 위젯 월드 위치
	FVector WidgetLoc = NameWidgetComponent->GetComponentLocation();

	// 라인 트레이스 설정
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);  // 자기 자신 무시

	bool bBlocked = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CamLoc,
		WidgetLoc,
		ECC_Visibility,
		Params
	);

	// 디버그용
	// DrawDebugLine(GetWorld(), CamLoc, WidgetLoc, bBlocked ? FColor::Red : FColor::Green, false, 0.1f);

	// 가려졌으면 숨기고, 아니면 보이기
	NameWidgetComponent->SetVisibility(!bBlocked);
}

void AGASNonCombatNPC::CheckTypeVisibility()
{
	if (!TypeWidgetComponent || !TypeWidgetComponent->GetWidget())
		return;

	// 카메라 위치 가져오기
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	// 위젯 월드 위치
	FVector WidgetLoc = TypeWidgetComponent->GetComponentLocation();

	// 라인 트레이스 설정
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);  // 자기 자신 무시

	bool bBlocked = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CamLoc,
		WidgetLoc,
		ECC_Visibility,
		Params
	);

	// 가려졌으면 숨기고, 아니면 보이기
	TypeWidgetComponent->SetVisibility(!bBlocked);
}

FGameplayTag AGASNonCombatNPC::GetNPCNameTag() const
{
	return NPCNameTag;
}

UWidgetComponent* AGASNonCombatNPC::GetGuideWidgetComponent() const
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

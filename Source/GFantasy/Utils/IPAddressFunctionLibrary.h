//=============================================================================
// IPAddressFunctionLibrary.h
//  로컬 머신의 IP 주소를 블루프린트에서 가져올 수 있도록 유틸리티 함수 집합을 정의합니다.
//=============================================================================

#pragma once

#include "CoreMinimal.h"                       // 언리얼 엔진 핵심 타입들
#include "Kismet/BlueprintFunctionLibrary.h"   // 블루프린트용 함수 라이브러리 상속용
#include "IPAddressFunctionLibrary.generated.h"

/**
 * UIPAddressFunctionLibrary
 *  - 블루프린트에서 호출 가능한 정적 함수로 로컬 IPv4 주소를 반환합니다.
 */
UCLASS()
class GFANTASY_API UIPAddressFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * GetLocalIPAddress
     *  - 로컬 머신(현재 플레이어 PC)의 IPv4 주소를 문자열로 반환합니다.
     *  - 실패 혹은 루프백 주소일 경우 "127.0.0.1" 을 반환합니다.
     *
     * @return FString  예: "192.168.0.10" 또는 실패 시 "127.0.0.1"
     */
    UFUNCTION(BlueprintCallable, Category = "Network|Utility")
    static FString GetLocalIPAddress();
};

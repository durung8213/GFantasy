//=============================================================================
// IPAddressFunctionLibrary.h
//  ���� �ӽ��� IP �ּҸ� �������Ʈ���� ������ �� �ֵ��� ��ƿ��Ƽ �Լ� ������ �����մϴ�.
//=============================================================================

#pragma once

#include "CoreMinimal.h"                       // �𸮾� ���� �ٽ� Ÿ�Ե�
#include "Kismet/BlueprintFunctionLibrary.h"   // �������Ʈ�� �Լ� ���̺귯�� ��ӿ�
#include "IPAddressFunctionLibrary.generated.h"

/**
 * UIPAddressFunctionLibrary
 *  - �������Ʈ���� ȣ�� ������ ���� �Լ��� ���� IPv4 �ּҸ� ��ȯ�մϴ�.
 */
UCLASS()
class GFANTASY_API UIPAddressFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * GetLocalIPAddress
     *  - ���� �ӽ�(���� �÷��̾� PC)�� IPv4 �ּҸ� ���ڿ��� ��ȯ�մϴ�.
     *  - ���� Ȥ�� ������ �ּ��� ��� "127.0.0.1" �� ��ȯ�մϴ�.
     *
     * @return FString  ��: "192.168.0.10" �Ǵ� ���� �� "127.0.0.1"
     */
    UFUNCTION(BlueprintCallable, Category = "Network|Utility")
    static FString GetLocalIPAddress();
};

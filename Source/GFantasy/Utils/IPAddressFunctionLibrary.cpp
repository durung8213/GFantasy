// IPAddressFunctionLibrary.cpp

#include "IPAddressFunctionLibrary.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Misc/OutputDevice.h"  // GLog 사용

FString UIPAddressFunctionLibrary::GetLocalIPAddress()
{
    // 소켓 서브시스템 가져오기
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem)
    {
        return TEXT("127.0.0.1");
    }

    // bool 레퍼런스로 넘겨줄 로컬 변수 선언
    bool bForceIPv4 = true;

    // FOutputDevice& 로는 *GLog 를, bool& 로는 위 로컬 변수를 넘깁니다.
    TSharedRef<FInternetAddr> LocalAddrRef =
        SocketSubsystem->GetLocalHostAddr(*GLog, bForceIPv4);

    // 주소가 유효하면 포트 없이 IP 문자열만 반환
    if (LocalAddrRef->IsValid())
    {
        return LocalAddrRef->ToString(/*bAppendPort=*/ false);
    }

    return TEXT("127.0.0.1");
}

// IPAddressFunctionLibrary.cpp

#include "IPAddressFunctionLibrary.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Misc/OutputDevice.h"  // GLog ���

FString UIPAddressFunctionLibrary::GetLocalIPAddress()
{
    // ���� ����ý��� ��������
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem)
    {
        return TEXT("127.0.0.1");
    }

    // bool ���۷����� �Ѱ��� ���� ���� ����
    bool bForceIPv4 = true;

    // FOutputDevice& �δ� *GLog ��, bool& �δ� �� ���� ������ �ѱ�ϴ�.
    TSharedRef<FInternetAddr> LocalAddrRef =
        SocketSubsystem->GetLocalHostAddr(*GLog, bForceIPv4);

    // �ּҰ� ��ȿ�ϸ� ��Ʈ ���� IP ���ڿ��� ��ȯ
    if (LocalAddrRef->IsValid())
    {
        return LocalAddrRef->ToString(/*bAppendPort=*/ false);
    }

    return TEXT("127.0.0.1");
}

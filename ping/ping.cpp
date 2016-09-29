// ping.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include <WinSock.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Iphlpapi.lib")

#pragma warning(disable: 4995)
#pragma warning(disable: 4996)
#include "AdkPrecomp.h"


void test_function(char *buf, int len)
{
	char *newbuf = NULL;

	if (!buf && !len)
	{
		assert(buf);
		assert(len);
		return;
	}

	newbuf = (char *)malloc((len + 1) * sizeof(char));
	if (newbuf)
	{
		memcpy(newbuf, buf, len);
	}

	return ;
}

#define  TIME_OUT  (1 * 1000)                // send timeout

BOOL WINAPI Ping(LPCSTR lpszAddress)
{
	BOOL  bRet = FALSE;
	HANDLE hIcmpFile = INVALID_HANDLE_VALUE;

	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[] = "Data Buffer";
	DWORD dwSendSize = sizeof(SendData);
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;
	LPHOSTENT lpHostent = NULL;

	if(!lpszAddress)
	{
		return bRet;
	}

	do
	{
		ipaddr = inet_addr(lpszAddress);
		if (ipaddr == INADDR_NONE)
		{
			lpHostent = gethostbyname(lpszAddress);
			if(!lpHostent)
			{
				AdkTRACEA(( "[prober] gethostbyname failed, error %d \r\n", WSAGetLastError() ));
				break;
			}

			ipaddr = *(unsigned long*)lpHostent->h_addr_list[0];

#ifdef _DEBUG
			struct in_addr inaddr;
			inaddr.S_un.S_addr = ipaddr;
			AdkTRACEA(( "[prober] gethostbyname: url = %s, ip = %s \r\n", lpszAddress, inet_ntoa(inaddr)));
#endif // _DEBUG
		}

		hIcmpFile = IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE)
		{
			break;
		}

		ReplySize = sizeof(ICMP_ECHO_REPLY) + dwSendSize + 1;
		ReplyBuffer = (VOID*)AdkMalloc(ReplySize);
		if (ReplyBuffer == NULL)
		{
			break;
		}        

		dwRetVal = IcmpSendEcho(
			hIcmpFile,
			ipaddr,
			SendData,
			(WORD)dwSendSize,
			NULL,
			ReplyBuffer,
			ReplySize,
			TIME_OUT);
		if (dwRetVal != 0)
		{
#ifdef _DEBUG
			PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
			struct in_addr ReplyAddr;
			ReplyAddr.S_un.S_addr = pEchoReply->Address;

			if (dwRetVal > 1)
			{
				AdkTRACEA(("[prober] Received %ld icmp message responses\n", dwRetVal));
				AdkTRACEA(("[prober] Information from the first response:\n"));
			}
			else
			{
				AdkTRACEA(("[prober] Received %ld icmp message response\n", dwRetVal));
				AdkTRACEA(("[prober] Information from this response:\n"));
			}

			AdkTRACEA(("[prober] Received from %s\n", inet_ntoa( ReplyAddr ) ));
			AdkTRACEA(("[prober] Status = %ld\n", pEchoReply->Status));
			AdkTRACEA(("[prober] Roundtrip time = %ld milliseconds\r\n\r\n", pEchoReply->RoundTripTime));
#endif // _Debug
		}
		else
		{
			AdkTRACEA(("[prober] ping %s failed, error code: %d \r\n", lpszAddress, GetLastError() ));
			break;
		}

		bRet = TRUE;

	}while(false);

	if(ReplyBuffer)
	{
		AdkFree(ReplyBuffer);
	}

	if(hIcmpFile != INVALID_HANDLE_VALUE)
	{
		IcmpCloseHandle(hIcmpFile);
	}

	return bRet;
}

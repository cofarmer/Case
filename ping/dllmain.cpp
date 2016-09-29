// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <WinSock.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	WSADATA wsData = {0};

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			WSAStartup(MAKEWORD(2, 2), &wsData);
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			WSACleanup();
		}
		break;
	}
	return TRUE;
}


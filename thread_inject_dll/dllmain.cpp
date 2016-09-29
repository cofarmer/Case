#include "stdafx.h"
#include <stdlib.h>
#include <Windows.h>

DWORD WINAPI proc(void *ctx)
{
	wchar_t module_name[MAX_PATH] = {0};

	for (;;)
	{
		GetModuleFileNameW(NULL, module_name, _countof(module_name));
		OutputDebugStringW(module_name);

		Sleep(1000);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugStringW(L"thread_inject_dll ===>>");
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)proc, NULL, 0, NULL);
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugStringW(L"thread_inject_dll <<===");
		break;
	}
	return TRUE;
}


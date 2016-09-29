
//#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#pragma warning(disable: 4995)
#pragma warning(disable: 4996)
#include "adkprecomp.h"

#pragma comment(lib, "../debug/hookdll")

extern "C" __declspec(dllimport) int set_hook();
extern "C" __declspec(dllimport) int stop_hook();

int _tmain(int argc, _TCHAR *argv[])
{
	char yesno;

	set_hook();

	Sleep(30 * 1000);

	stop_hook();

	return 0;

	while (1)
	{
		printf("stop hook: (yes or no)");
		yesno = getchar();
		if (yesno == 'Y' || yesno == 'y')
		{
			stop_hook();
			break;
		}
	}

	return 0;
}


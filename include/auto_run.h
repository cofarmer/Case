
#ifndef _AUTO_RUN_H_
#define _AUTO_RUN_H_

//
// Author: Yanfei Zhang
// Date: 2015-07-09
//

// check bits of current system, if x86 return 32, alse if x64 return 64
//
int x86_x64()
{
	SYSTEM_INFO sysinfo = {0};
	GetNativeSystemInfo(&sysinfo);

	if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 
		|| sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		return 64;
	}

	return 32;
}

// set the program start with system that specified
// if set succeed, the return value is 0, alse if failed, the return value is -1
// the failed error code will output by error_code
// this function need runas administrator with your application
//
int register_start_with_system(const wchar_t *command_line, const wchar_t *name, long *error_code = NULL)
{
	// different bits of system use different subkey path in HKEY_LOCAL_MACHINE key
	//
	static wchar_t *g_win64_subkey = L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run";
	static wchar_t *g_win32_subkey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

	int ret = -1;
	HKEY root_key = NULL;
	LSTATUS status = ERROR_SUCCESS;
	wchar_t subkey[MAX_PATH] = {0};

	if (command_line == NULL || name == NULL)
	{
		return -1;
	}

	do 
	{
		if (x86_x64() == 32)
		{
			wcscpy_s(subkey, _countof(subkey), g_win32_subkey);
		}
		else
		{
			wcscpy_s(subkey, _countof(subkey), g_win64_subkey);
		}

		status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, subkey, 0, KEY_ALL_ACCESS, &root_key);
		if (status != ERROR_SUCCESS)
		{
			break;
		}

		status = RegSetValueExW(root_key, name, 0, REG_SZ, (BYTE *)command_line, wcslen(command_line)*sizeof(wchar_t));
		if (status != ERROR_SUCCESS)
		{
			break;
		}

		ret = 0;

	} while (0);

	if (error_code)
	{
		*error_code = (long)status;
	}

	if (root_key)
	{
		RegCloseKey(root_key);
	}

	return ret;
}
#endif
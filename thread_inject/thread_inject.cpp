

//
// 
// 本程序利用远程线程注入，将准备好的代码注入目标进程并运行起来
// 也可在注入的代码中调用LoadLibrary加载目的DLL
// 编译为x64可注入64位进程
//
//

#include "stdafx.h"
#include <time.h>
#include <assert.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "adkprecomp.h"


int adjust_process_token_privilege()
{
	int ret = -1;
	LUID uid;
	HANDLE token;
	TOKEN_PRIVILEGES t_privilege ;

	do 
	{
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
		{
			ret = GetLastError();
			break;
		}

		if (!LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &uid))
		{
			ret = GetLastError();
			break;
		}

		t_privilege.PrivilegeCount = 1;
		t_privilege.Privileges[0].Luid = uid;
		t_privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(token, FALSE, &t_privilege, sizeof(t_privilege), NULL, NULL))
		{
			ret = GetLastError();
			break;
		}

		ret = 0;

	} while (0);

	return ret;
}

int find_process(const wchar_t *proc_name, unsigned long *pid)
{
	int ret = -1;
	HANDLE proc_snap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 proc_32 = {0};
	BOOL first_result = FALSE;

	do 
	{
		proc_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (proc_snap == INVALID_HANDLE_VALUE)
		{
			ret = GetLastError();
			break;
		}

		proc_32.dwSize = sizeof(proc_32);

		first_result = Process32FirstW(proc_snap, &proc_32);
		if(!first_result) break;
		do 
		{
			if (wcsicmp(proc_32.szExeFile, proc_name) == 0)
			{
				// found it

				*pid = proc_32.th32ProcessID;

				break;
			}

		} while (Process32NextW(proc_snap, &proc_32));

		ret = 0;

	} while (0);

	return ret;
}

struct param_info 
{
	wchar_t msg[MAX_PATH];
	wchar_t title[MAX_PATH];
	wchar_t dll[MAX_PATH];
	ULONG_PTR msgaddr;
	ULONG_PTR sleepaddr;
	ULONG_PTR outputaddr;
	ULONG_PTR loadlibrary;
};
void get_param_info(struct param_info *pinfo)
{
	HMODULE user32dll = LoadLibraryW(L"User32.dll");
	HMODULE kernel32dll = LoadLibraryW(L"Kernel32.dll");
	if (user32dll)
	{
		pinfo->msgaddr = (ULONG_PTR)GetProcAddress(user32dll, "MessageBoxW");
		if (pinfo->msgaddr)
		{
			wcscpy_s(pinfo->msg, _countof(pinfo->msg), L"Haha, inject succeed!");
			wcscpy_s(pinfo->title, _countof(pinfo->title), L"title");
		}

		FreeLibrary(user32dll);
	}

	if (kernel32dll)
	{
		pinfo->sleepaddr = (ULONG_PTR)GetProcAddress(kernel32dll, "Sleep");
		pinfo->outputaddr = (ULONG_PTR)GetProcAddress(kernel32dll, "OutputDebugStringW");
		pinfo->loadlibrary = (ULONG_PTR)GetProcAddress(kernel32dll, "LoadLibraryW");

		FreeLibrary(kernel32dll);
	}

	wcscpy_s(pinfo->dll, _countof(pinfo->dll), L"E:\\CodeJob\\Case\\x64\\Debug\\thread_inject_dll.dll");

	return ;
}

DWORD WINAPI inject_proc(LPVOID param)
{
	unsigned long index = 0;
	struct param_info *pinfo = (struct param_info *)param;

	typedef int (WINAPI *MSGBOX)(HWND, LPCWSTR, LPCWSTR, UINT);
	typedef void (WINAPI *SLEEP)(DWORD);
	typedef void (WINAPI *OUTPUT)(LPCWSTR);
	typedef HMODULE (WINAPI *LOADLIBRARY)(LPCTSTR);

	MSGBOX msgbox = NULL;
	SLEEP sleep = NULL;
	OUTPUT print = NULL;
	LOADLIBRARY loadlibrary = NULL;

	msgbox = (MSGBOX)pinfo->msgaddr;
	sleep = (SLEEP)pinfo->sleepaddr;
	print = (OUTPUT)pinfo->outputaddr;
	loadlibrary = (LOADLIBRARY)pinfo->loadlibrary;

	msgbox(NULL, pinfo->msg, pinfo->title, MB_OK|MB_ICONINFORMATION);

	loadlibrary(pinfo->dll);

	return 0;
}

ULONG_PTR get_func_real_address(void *ctx)
{
	assert(ctx);

	ULONG_PTR real_addr = (ULONG_PTR)ctx;
	unsigned char *funcaddr = (unsigned char *)ctx;

	// 
	// 这里Debug版本存在ILT（增量链接表），需要根据ILT解析出函数的真实地址
	// Release版本没有ILT
	// refer to http://www.cnblogs.com/fanzhidongyzby/archive/2012/08/30/2664287.html
	//

	if (funcaddr[0] == 0xE9)  // 0xE9 为jmp指令的机器码
	{
		int disp = *(int *)(funcaddr + 1); // 取JMP之后的参数，该参数为函数真实地址的偏移量 
		real_addr += 5;			// 指向下一条指令地址 
		real_addr += disp;		// 用下一条指令地址 + 偏移量 = 函数真实地址
	}

	return real_addr;
}

int main()
{
	//
	// NOTE: 编译选项中需要去掉运行时检查，否则在注入目标程序后退出时导致目标程序崩溃
	//		 编译为x86可以注入32bits进程
	//		 编译为x64可以注入64bits进程
	//

	wchar_t *target_procname = L"notepad.exe";

	int ret = 0;
	unsigned long target_pid = 0;
	HANDLE proc_t = NULL;
	struct param_info pinfo = {0};
	BOOL res = FALSE;
	unsigned long error_code = NO_ERROR;
	unsigned long real_func_size = 0;
	ULONG_PTR real_func_addr = 0;
	char *real_func_data = NULL;

	ret = adjust_process_token_privilege();
	assert(ret == 0);
	printf("adjust process token privileges result code %d \r\n", ret);

	ret = find_process(target_procname, &target_pid);
	printf("find process result code %d \r\n", ret);
	printf("pid %d \r\n", target_pid);
	if (target_pid == 0)
	{
		printf("not found target process. \r\n");
		system("pause");
		return 1;
	}

	get_param_info(&pinfo);

	// 定位函数真实地址
	//
	real_func_addr = get_func_real_address(inject_proc);
	assert(real_func_addr);

	// 计算函数字节大小
	//
	real_func_data = (char *)real_func_addr;
	for (char *p = real_func_data; real_func_size<2048; real_func_size++, p++ )
	{
		if ((unsigned long long )*(unsigned long long *)p == 0xcccccccccccccccc)
		{
			break;
		}
	}

	proc_t = OpenProcess(PROCESS_ALL_ACCESS, FALSE, target_pid);
	assert(proc_t);

	void *vmem1 = VirtualAllocEx(proc_t, NULL, real_func_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	assert(vmem1);

	void *vmem2 = VirtualAllocEx(proc_t, NULL, sizeof(pinfo), MEM_COMMIT, PAGE_READWRITE);
	assert(vmem2);

	SIZE_T writen1, writen2;
	res = WriteProcessMemory(proc_t, vmem1, real_func_data, real_func_size, &writen1);
	assert(res);
	res = WriteProcessMemory(proc_t, vmem2, &pinfo, sizeof(pinfo), &writen2);
	assert(res);

	HANDLE thread_t = CreateRemoteThread(proc_t, NULL, 0, (DWORD (WINAPI *)(LPVOID))vmem1, vmem2, 0, NULL);
	if (thread_t == NULL)
	{
		error_code = GetLastError();
		printf("createremotethread error code %d \r\n", error_code);
	}

	WaitForSingleObject(thread_t, INFINITE);
	CloseHandle(thread_t);

	VirtualFreeEx(proc_t, vmem1, writen1, MEM_FREE);
	VirtualFreeEx(proc_t, vmem2, writen2, MEM_FREE);

	return 0;
}


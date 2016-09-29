// proofer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <Windows.h>
#include <locale>
#include "adkprecomp.h"
#include "adkmd5.h"
#include "adksha1.h"

int proof(const wchar_t *file, char *md5_str, int str_len, char *sha1_str, int sha1_len)
{
#define MAX_BLOCK_SIZE		1024 * 1024

	int ret = -1;
	HANDLE file_t = INVALID_HANDLE_VALUE;
	unsigned long filesize;
	unsigned long readsize;
	char *filebuf = NULL;
	unsigned char md5_digest[32] = {0};
	MD5_CTX mc = {0};
	unsigned char sha1_digest[20] = {0};
	SHA_CTX sc = {0};

	if(!file) return -1;

	do 
	{
		file_t = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(file_t == INVALID_HANDLE_VALUE) 
		{
			wprintf_s(L"open file failed, file %ws, error code %d \n", file, GetLastError());
			break;
		}

		filesize = GetFileSize(file_t, NULL);
		if(!filesize || filesize == INVALID_FILE_SIZE) break;

		filebuf = (char *)malloc(MAX_BLOCK_SIZE + 1);
		if( !filebuf ) break;

		AdkMD5Init(&mc);
		AdkSHA1Init(&sc);

		do 
		{
			memset(filebuf, '\0', MAX_BLOCK_SIZE + 1);
			ReadFile(file_t, filebuf, MAX_BLOCK_SIZE, &readsize, NULL);
			if (readsize)
			{
				AdkMD5Update(&mc, (unsigned char *)filebuf, (unsigned int)readsize);
				AdkSHA1Update(&sc, (unsigned char *)filebuf, (unsigned int)readsize);
			}
		} while (readsize);

		AdkMD5Final(&mc, md5_digest);
		AdkSHA1Final(&sc, sha1_digest);

		for (int i=0; i<16; ++i)
		{
			sprintf_s(md5_str + i*2, str_len-i*2, "%02X", md5_digest[i]);
		}
		for (int i=0; i<20; ++i)
		{
			sprintf_s(sha1_str + i*2, sha1_len-i*2, "%02X", sha1_digest[i]);
		}

		ret = 0;

	} while (0);

	if(file_t != INVALID_HANDLE_VALUE) CloseHandle(file_t);

	return ret;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int ret = -1;
	wchar_t file[MAX_PATH] = {0};
	char md5_str[64] = {0};
	char sha1_str[64] = {0};

	setlocale(LC_ALL, "chs");
	system("color a");

	if (argc < 2)
	{
		printf("not specified file. \n");
	}
	else
	{
		for (int index=1; index<argc; ++index)
		{
			wcscpy_s(file, _countof(file), argv[index]);
			wprintf_s(L"checking %ws ... \n", file);
			ret = proof(file, md5_str, _countof(md5_str), sha1_str, _countof(sha1_str));
			if (ret == 0)
			{
				printf("  MD5: %s \n", md5_str);
				printf(" SHA1: %s \n", sha1_str);
			}
		}
	}

	printf("checking finished. \n");

	system("pause");
	return 0;
}


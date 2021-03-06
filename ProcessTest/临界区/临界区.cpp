// 临界区.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

DWORD WINAPI ThreadProcA(LPVOID lpParameter);
DWORD WINAPI ThreadProcB(LPVOID lpParameter);

HANDLE hThreadArr[2] = { 0 };
INT nResource = 10;

CRITICAL_SECTION cs;

int main()
{
	DWORD dwTid = 0;
	DWORD dwPraA = 10;
	DWORD dwPraB = 10;

	InitializeCriticalSection(&cs);
	hThreadArr[0] = CreateThread(NULL, 0, ThreadProcA, &dwPraA, 0, &dwTid);
	hThreadArr[1] = CreateThread(NULL, 0, ThreadProcB, &dwPraB, 0, &dwTid);
	while (1)
	{
		Sleep(500);
	}

	return 0;
}


DWORD WINAPI ThreadProcA(LPVOID lpParameter)
{
	DWORD parament = *(LPDWORD)lpParameter;


	printf("线程A开始执行 \n");
	while (1)
	{
		EnterCriticalSection(&cs);
		if (nResource > 0)
		{
			Sleep(200);
			nResource--;
			printf("A线程获取资源 剩余 %d \n", nResource);
		}
		else
		{
			break;
		}
		LeaveCriticalSection(&cs);
	}
	printf("A线程执行结束 \n");
	return 0;
}

DWORD WINAPI ThreadProcB(LPVOID lpParameter)
{
	DWORD parament = *(LPDWORD)lpParameter;


	printf("线程B开始执行 \n");
	while (1)
	{
		EnterCriticalSection(&cs);
		if (nResource > 0)
		{
			Sleep(200);
			nResource--;
			printf("B线程获取资源 剩余 %d \n", nResource);
		}
		else
		{
			break;
		}
		LeaveCriticalSection(&cs);
	}
	printf("B线程执行结束 \n");
	return 0;
}
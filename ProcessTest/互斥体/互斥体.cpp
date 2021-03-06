// 互斥体.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

DWORD WINAPI ThreadProcA(LPVOID lpParameter);
DWORD WINAPI ThreadProcB(LPVOID lpParameter);

DWORD g_resource = 100;

int main()
{
	HANDLE hThreadArr[2] = { 0 };
	DWORD dwPraA = 0;
	DWORD dwTid = 0;
	
	hThreadArr[0] = CreateThread(NULL, 0, ThreadProcA, &dwPraA, 0, &dwTid);
	hThreadArr[1] = CreateThread(NULL, 0, ThreadProcB, &dwPraA, 0, &dwTid);

	WaitForMultipleObjects(2, hThreadArr, TRUE, INFINITE);
	for (DWORD i = 0; i < 2; i++)
	{
		CloseHandle(hThreadArr[i]);
	}
	getchar();
    return 0;
}


DWORD WINAPI ThreadProcA(LPVOID lpParameter)
{
	DWORD parament = *(LPDWORD)lpParameter;
	HANDLE mutex = CreateMutex(NULL, TRUE, L"g_resource");
	while (1)
	{
		
		WaitForSingleObject(mutex, INFINITE);
		if (g_resource > 0)
		{
			g_resource--;
			printf("线程A 处理变量 => %d \n", g_resource);
			ReleaseMutex(mutex);
			CloseHandle(mutex);
		}
		else
		{
			ReleaseMutex(mutex);
			
			break;
		}
	}

	CloseHandle(mutex);
	return 0;
}

DWORD WINAPI ThreadProcB(LPVOID lpParameter)
{
	DWORD parament = *(LPDWORD)lpParameter;
	HANDLE mutex = CreateMutex(NULL, TRUE, L"g_resource");
	while (1)
	{
		
		WaitForSingleObject(mutex, INFINITE);
		if (g_resource > 0)
		{
			g_resource--;
			printf("线程B 处理变量 => %d \n", g_resource);
			ReleaseMutex(mutex);
		}
		else
		{
			ReleaseMutex(mutex);
			break;
		}
	}
	CloseHandle(mutex);
	return 0;
}
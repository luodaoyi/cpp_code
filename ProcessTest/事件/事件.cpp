
// 事件.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

DWORD WINAPI ThreadProcA(LPVOID lpParameter);
DWORD WINAPI ThreadProcB(LPVOID lpParameter);

DWORD g_resource = 100;

int main()
{
	HANDLE hThreadArr[2] = { 0 };
	DWORD dwPraA = 5;
	DWORD dwPraB = 5;
	DWORD dwTid = 0;

	hThreadArr[0] = CreateThread(NULL, 0, ThreadProcA, &dwPraA, 0, &dwTid);
	hThreadArr[1] = CreateThread(NULL, 0, ThreadProcB, &dwPraB, 0, &dwTid);

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
	HANDLE mutex = CreateEvent(NULL, FALSE, TRUE, L"g_event");
	DWORD all_res = 0;
	while (1)
	{
		WaitForSingleObject(mutex, INFINITE);
		ResetEvent(mutex);
		if (g_resource > 0)
		{
			all_res += parament;
			g_resource -= parament;
			printf("线程A 获得资源 => %d 还剩资源 %d \n", all_res, g_resource);
			SetEvent(mutex);
		}
		else
		{
			SetEvent(mutex);
			break;
		}
	}
	CloseHandle(mutex);
	printf("线程A 获得资源 => %d \n", all_res);
	return 0;
}

DWORD WINAPI ThreadProcB(LPVOID lpParameter)
{
	DWORD parament = *(LPDWORD)lpParameter;
	HANDLE mutex = CreateEvent(NULL, FALSE, TRUE, L"g_event");
	DWORD all_res = 0;
	while (1)
	{
		WaitForSingleObject(mutex, INFINITE);
		ResetEvent(mutex);
		if (g_resource > 0)
		{
			all_res += parament;
			g_resource -= parament;
			printf("线程B 获得资源 => %d 还剩资源 %d \n", all_res, g_resource);
			SetEvent(mutex);
		}
		else
		{
			SetEvent(mutex);
			break;
		}
	}
	CloseHandle(mutex);
	printf("线程B 获得资源 => %d \n", all_res);
	return 0;
}
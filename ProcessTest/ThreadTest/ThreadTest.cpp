// ThreadTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>


DWORD WINAPI ThreadProcA(LPVOID lpParameter);
DWORD WINAPI ThreadProcB(LPVOID lpParameter);
DWORD WINAPI ThreadProcC(LPVOID lpParameter);

DWORD g_nRes = 0;

int main()
{
	HANDLE hThreadArr[2] = { 0 };
	DWORD dwPraA = 5;
	DWORD dwPraB = 5;
	DWORD dwPraC = 5;
	DWORD dwTid = 0;

	hThreadArr[0] = CreateThread(NULL, 0, ThreadProcA, &dwPraA, 0, &dwTid);
	hThreadArr[1] = CreateThread(NULL, 0, ThreadProcB, &dwPraB, 0, &dwTid);
	hThreadArr[2] = CreateThread(NULL, 0, ThreadProcC, &dwPraC, 0, &dwTid);

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
	
	HANDLE mutex_a = CreateEvent(NULL, FALSE, TRUE, L"g_event_a");
	HANDLE mutex_b = CreateEvent(NULL, FALSE, FALSE, L"g_event_b");
	HANDLE mutex_c = CreateEvent(NULL, FALSE, FALSE, L"g_event_c");

	DWORD all_res = 0;
	while (all_res < 100)
	{
		WaitForSingleObject(mutex_a, INFINITE);

		g_nRes += 1;
		all_res += 1;
		
		printf("线程A +1 累计+ %d  目前 %d \n", all_res, g_nRes);
		if (all_res % 2 == 0) 
		{
			SetEvent(mutex_b);
		}
		else
		{
			SetEvent(mutex_c);
		}
	}
	CloseHandle(mutex_a);
	SetEvent(mutex_b); //打印内容
	SetEvent(mutex_c); //打印内容
	CloseHandle(mutex_b);
	CloseHandle(mutex_c);
	printf("线程A 累计增加资源  %d \n", all_res);
	return 0;
}

DWORD WINAPI ThreadProcB(LPVOID lpParameter)
{
	DWORD parament = *(LPDWORD)lpParameter;
	HANDLE mutex_a = CreateEvent(NULL, FALSE, TRUE, L"g_event_a");
	HANDLE mutex_b = CreateEvent(NULL, FALSE, FALSE, L"g_event_b");


	DWORD all_res = 0;
	while (1)
	{
		WaitForSingleObject(mutex_b, INFINITE);

		if (g_nRes > 0)
		{
			g_nRes -= 1;
			all_res += 1;
			printf("线程B -1 累计- %d  目前 %d \n", all_res, g_nRes);			
			SetEvent(mutex_a);
		}
		else
		{
			SetEvent(mutex_a);
			break;
		}
	}
	CloseHandle(mutex_a);
	CloseHandle(mutex_b);
	printf("线程B 累计获得资源  %d \n", all_res);
	return 0;
}

DWORD WINAPI ThreadProcC(LPVOID lpParameter)
{
	DWORD parament = *(LPDWORD)lpParameter;
	HANDLE mutex_a = CreateEvent(NULL, FALSE, TRUE, L"g_event_a");
	HANDLE mutex_c = CreateEvent(NULL, FALSE, FALSE, L"g_event_c");


	DWORD all_res = 0;
	while (1)
	{

		WaitForSingleObject(mutex_c, INFINITE);

		if (g_nRes > 0)
		{
			g_nRes -= 1;
			all_res += 1;
			printf("线程C -1 累计- %d  目前 %d \n", all_res, g_nRes);
			SetEvent(mutex_a);
			
		}
		else
		{
			SetEvent(mutex_a);
			break;
		}
	}
	CloseHandle(mutex_a);
	CloseHandle(mutex_c);
	printf("线程C 累计获得资源  %d \n", all_res);
	return 0;
}
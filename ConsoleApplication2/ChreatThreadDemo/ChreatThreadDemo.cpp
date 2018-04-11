// ChreatThreadDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

DWORD WINAPI ThreadProcA(LPVOID lpParameter);
DWORD WINAPI ThreadProcB(LPVOID lpParameter);

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwTid = 0;
	DWORD dwPraA = 1;
	DWORD dwPraB = 2;
	DWORD dwErr = 0;
	HANDLE hThreadArr[2] = { 0 };

	DWORD dwRetA = 0;
	hThreadArr[0] = CreateThread(NULL, 0, ThreadProcA, &dwPraA, 0, &dwTid);
	//hThreadArr[1] = CreateThread(NULL, 0, ThreadProcB, &dwPraB, 0, &dwTid);

	//等待单个
	DWORD dwRetFlag = WaitForSingleObject(hThreadArr[0], INFINITE);
	//等待句柄数组
	//DWORD dwRetFlag = WaitForMultipleObjects(2, hThreadArr, TRUE, INFINITE);
	if (dwRetFlag == WAIT_FAILED){
		printf("等待失败!!!\n");
	}

	GetExitCodeThread(hThreadArr[0], &dwRetA);
	printf("线程A的返回值 %d \n",dwRetA);

	while (1)
	{
		printf("主线程正在执行 \n");
		Sleep(500);
	}
	return 0;
}


DWORD WINAPI ThreadProcA(LPVOID lpParameter)
{
	for (int i = 0; i < 10;i++)
	{
		printf("A线程正在执行 => %d\n",*(LPDWORD)lpParameter);
		Sleep(500);
	}
	return 666;
}

DWORD WINAPI ThreadProcB(LPVOID lpParameter)
{
	for (int i = 0; i < 15; i++)
	{
		printf("B线程正在执行 => %d \n", *(LPDWORD)lpParameter);
		Sleep(500);
	}
	return 0;
}
// ThreadControl3.cpp : 定义控制台应用程序的入口点。
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
	hThreadArr[0] = CreateThread(NULL, NULL, ThreadProcA, &dwPraA, 0, &dwTid);
	hThreadArr[1] = CreateThread(NULL, NULL, ThreadProcB, &dwPraB, CREATE_SUSPENDED, &dwTid);

	//等待单个
	DWORD dwRetFlag = WaitForSingleObject(hThreadArr[0], INFINITE);
	//等待句柄数组
	//DWORD dwRetFlag = WaitForMultipleObjects(2, hThreadArr, TRUE, INFINITE);
	if (dwRetFlag == WAIT_FAILED){
		OutputDebugString(L"等待失败!!!\n");
	}
	ResumeThread(hThreadArr[1]);

	GetExitCodeThread(hThreadArr[0], &dwRetA);
	printf("线程A的返回值 %d \n", dwRetA);

	while (1)
	{
		printf("主线程正在执行 \n");
		Sleep(500);
	}
	return 0;
}


DWORD WINAPI ThreadProcA(LPVOID lpParameter)
{
	OutputDebugString(L"A线程开始执行 \n");
	for (int i = 0; i < 10; i++)
	{
		//printf("A线程正在执行 \n");//不可重入类型
		OutputDebugString(L"A线程正在执行 \n");
		Sleep(500);
	}
	OutputDebugString(L"A线程结束执行 \n");
	return 666;
}

DWORD WINAPI ThreadProcB(LPVOID lpParameter)
{
	OutputDebugString(L"B线程开始执行 \n");
	for (int i = 0; i < 15; i++)
	{
		OutputDebugString(L"B线程正在执行 \n");
		Sleep(500);
	}
	OutputDebugString(L"B线程结束执行 \n");
	return 0;
}

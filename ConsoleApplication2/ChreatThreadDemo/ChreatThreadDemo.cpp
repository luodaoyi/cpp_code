// ChreatThreadDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//�ȴ�����
	DWORD dwRetFlag = WaitForSingleObject(hThreadArr[0], INFINITE);
	//�ȴ��������
	//DWORD dwRetFlag = WaitForMultipleObjects(2, hThreadArr, TRUE, INFINITE);
	if (dwRetFlag == WAIT_FAILED){
		printf("�ȴ�ʧ��!!!\n");
	}

	GetExitCodeThread(hThreadArr[0], &dwRetA);
	printf("�߳�A�ķ���ֵ %d \n",dwRetA);

	while (1)
	{
		printf("���߳�����ִ�� \n");
		Sleep(500);
	}
	return 0;
}


DWORD WINAPI ThreadProcA(LPVOID lpParameter)
{
	for (int i = 0; i < 10;i++)
	{
		printf("A�߳�����ִ�� => %d\n",*(LPDWORD)lpParameter);
		Sleep(500);
	}
	return 666;
}

DWORD WINAPI ThreadProcB(LPVOID lpParameter)
{
	for (int i = 0; i < 15; i++)
	{
		printf("B�߳�����ִ�� => %d \n", *(LPDWORD)lpParameter);
		Sleep(500);
	}
	return 0;
}
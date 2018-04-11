// ThreadControl2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>

DWORD WINAPI ThreadProc(LPVOID lpParameter);

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hThradAB[2] = {0};
	HANDLE hThradCD[2] = { 0 };

	DWORD dLoopCountA = 10;
	DWORD dLoopCountB = 15;
	DWORD dLoopCountC = 20;
	DWORD dLoopCountD = 25;
	DWORD dTid[4] = { 0 };
	
	hThradAB[0] = CreateThread(NULL, NULL, ThreadProc, &dLoopCountA, NULL, &(dTid[0]));
	hThradAB[1] = CreateThread(NULL, NULL, ThreadProc, &dLoopCountB, NULL, &(dTid[1]));
	WaitForMultipleObjects(2, hThradAB, FALSE, INFINITE);
	

	hThradCD[0] = CreateThread(NULL, NULL, ThreadProc, &dLoopCountC, NULL, &(dTid[2]));
	hThradCD[1] = CreateThread(NULL, NULL, ThreadProc, &dLoopCountD, NULL, &(dTid[3]));
	WaitForMultipleObjects(2, hThradCD, TRUE, INFINITE);
	
	printf("��̴���!! \n");
	getchar();
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	DWORD length = *(LPDWORD)lpParameter;
	int iIndex = (length / 5)-1;
	for (int i = 0; i < length; i++)
	{
		printf("�� %d ���߳� ѭ���� %d �� \n",iIndex, i);
		Sleep(100);
	}
	return 0;
}
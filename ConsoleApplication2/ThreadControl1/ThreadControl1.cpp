// <1>创建一个控制台窗口程序，定义一个值为100的变量，创建三个线程，同时对这个变量进行循环 - 1的操作，并输出结果，直到这个变量的值为0为止，观察输出的值并思考为什么。
//


#include "stdafx.h"
#include <Windows.h>

DWORD WINAPI ThreadProc(LPVOID lpParameter);
int _tmain(int argc, _TCHAR* argv[])
{
	DWORD value = 100;
	HANDLE hThreadArr[3] = { 0 };
	DWORD dwTid[3] = {3};
	for (int i = 0; i < 3; i++)
	{
		hThreadArr[i] = CreateThread(NULL, NULL, ThreadProc, &value, NULL, &(dwTid[i]));
	}

	DWORD dwRetFlag = WaitForMultipleObjects(3, hThreadArr, TRUE, INFINITE);
	if (dwRetFlag == WAIT_FAILED){
		printf("等待失败!!!\n");
	}
	
	printf("执行完毕!!!\n");
	getchar();
	return 0;
}


DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	while (*(LPDWORD)lpParameter > 0)
	{
		(*(LPDWORD)lpParameter)--;
		printf("current value: => %d \n", *(LPDWORD)lpParameter);
	}
	
	return 0;
}

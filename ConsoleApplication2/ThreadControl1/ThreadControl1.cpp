// <1>����һ������̨���ڳ��򣬶���һ��ֵΪ100�ı��������������̣߳�ͬʱ�������������ѭ�� - 1�Ĳ���������������ֱ�����������ֵΪ0Ϊֹ���۲������ֵ��˼��Ϊʲô��
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
		printf("�ȴ�ʧ��!!!\n");
	}
	
	printf("ִ�����!!!\n");
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

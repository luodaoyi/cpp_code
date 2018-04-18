// 防止躲开.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int main()
{
	HANDLE mutex = CreateMutex(NULL, TRUE, L"mutl");
	DWORD mutl = WaitForSingleObject(mutex, 10);
	if (mutl == WAIT_TIMEOUT)
	{
		printf("不要多开！！");
		getchar();
		return 0;
	}
	printf("运行中！！");
	getchar();
	ReleaseMutex(mutex);
	CloseHandle(mutex);
	return 0;
}


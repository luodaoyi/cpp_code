// 远程线程注入.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "windows.h"


int main()
{
	DWORD dwPID = 0;
	DWORD dwAddress = 0;
	printf("输入目标进程PID \r\n");
	scanf_s("%d",&dwPID);
	printf("输入目标进程地址 \r\n");
	scanf_s("%d", &dwAddress);
	//打开进程

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
	{
		printf("打开错误 错误代码: %d", GetLastError());
		getchar();
		return 0;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)dwAddress, NULL, NULL,NULL);
	if (hThread == NULL)
	{
		printf("创建错误 错误代码: %d", GetLastError());
		getchar();
		return 0;
	}
	printf("创建成功 %d", (DWORD)hThread);

	getchar();
    return 0;
}


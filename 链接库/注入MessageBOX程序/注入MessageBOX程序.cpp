// 注入MessageBOX程序.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>          
#include <stdio.h>
#include <Windows.h>

int main()
{



	DWORD dwPID = 0;
	DWORD dwAddress = 0;
	printf("输入目标进程PID \r\n");
	scanf_s("%d", &dwPID);

	WCHAR  wzDllFullPath[MAX_PATH] = { 0 }; //注入的dll 路径

	printf("输入将要注入的dll路径 \r\n");

	wcsncat_s(wzDllFullPath, L"C:\\Users\\luoda\\Documents\\GitHub\\cpp_code\\链接库\\Debug\\MessageBoxDLL.dll", MAX_PATH);

	printf("dll路径 %s \r\n", wzDllFullPath);
	HANDLE  TargetProcessHandle = NULL;

	TargetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (NULL == TargetProcessHandle)
	{
		printf("failed to open process!!\n");
		return FALSE;
	}

	WCHAR* VirtualAddress = NULL;
	ULONG32 ulDllLength = (ULONG32)_tcslen(wzDllFullPath) + 1;
	//ALLOC Address for Dllpath
	VirtualAddress = (WCHAR*)VirtualAllocEx(TargetProcessHandle, NULL, ulDllLength * sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);

	if (NULL == VirtualAddress)
	{
		printf("failed to Alloc!!\n");
		CloseHandle(TargetProcessHandle);
		return FALSE;
	}

	// write
	if (FALSE == WriteProcessMemory(TargetProcessHandle, VirtualAddress, (LPVOID)wzDllFullPath, ulDllLength * sizeof(WCHAR), NULL))
	{
		printf("failed to write!!\n");
		VirtualFreeEx(TargetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		CloseHandle(TargetProcessHandle);
		return FALSE;
	}

	LPTHREAD_START_ROUTINE FunctionAddress = NULL;
	FunctionAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
	HANDLE ThreadHandle = INVALID_HANDLE_VALUE;
	//start
	ThreadHandle = CreateRemoteThread(TargetProcessHandle, NULL, 0, FunctionAddress, VirtualAddress, 0, NULL);
	if (NULL == ThreadHandle)
	{
		VirtualFreeEx(TargetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		CloseHandle(TargetProcessHandle);
		return FALSE;
	}
	// WaitForSingleObject
	WaitForSingleObject(ThreadHandle, INFINITE);
	VirtualFreeEx(TargetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);			// 清理
	CloseHandle(ThreadHandle);
	CloseHandle(TargetProcessHandle);

    return 0;
}


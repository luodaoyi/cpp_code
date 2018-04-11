// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{

	LPWSTR commd = GetCommandLine();
	printf("in subprocess，calc_handler_code=> %d notepad_handler_code => %d", (DWORD)commd[0], (DWORD)commd[1]);

	if (!TerminateProcess((HANDLE)commd[0], 0))
	{
		printf("calc error_code = %d \n", GetLastError());
	}
	if (!TerminateProcess((HANDLE)commd[1], 0))
	{
		printf("notepad error_code = %d \n", GetLastError());
	}
	getchar();

	return 0;
}


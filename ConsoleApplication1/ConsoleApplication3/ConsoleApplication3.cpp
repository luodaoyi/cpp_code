// ConsoleApplication3.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include "MyFunction.h"

int main()
{
	STARTUPINFO start_info = { 0 };
	start_info.cb = sizeof STARTUPINFO;

	
	/*PROCESS_INFORMATION process_info = { 0 };
	CreateProcess(
		L"C:\\Windows\\System32\\notepad.exe",
		NULL,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&start_info,
		&process_info
	);*/
	MyFindProcessW(L"任务管理器.exe");
	return 0;
}


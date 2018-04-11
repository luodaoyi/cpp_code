// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	WCHAR calc_name[] = L"C:\\Windows\\System32\\calc.exe";
	WCHAR notepad_name[] = L"C:\\Windows\\System32\\notepad.exe";
	
	SECURITY_ATTRIBUTES security_struct_calc = { 0 };
	security_struct_calc.nLength = sizeof SECURITY_ATTRIBUTES;
	security_struct_calc.bInheritHandle = TRUE;  //可继承

	SECURITY_ATTRIBUTES security_struct_notepad = { 0 };
	security_struct_notepad.nLength = sizeof SECURITY_ATTRIBUTES;
	security_struct_notepad.bInheritHandle = FALSE;  //不可继承

	STARTUPINFO startup_info_calc = { 0 };
	startup_info_calc.cb = sizeof STARTUPINFO;
	STARTUPINFO startup_info_notepad = { 0 };
	startup_info_notepad.cb = sizeof STARTUPINFO;

	PROCESS_INFORMATION process_information_calc = { 0 };
	PROCESS_INFORMATION process_information_notepad = { 0 };

	//创建计算器进程 句柄可以继承
	BOOL result = CreateProcess(calc_name, NULL, &security_struct_calc, NULL, TRUE, NULL, NULL, NULL, &startup_info_calc, &process_information_calc);
	//创建记事本进程 不可继承
	BOOL result2 = CreateProcess(notepad_name, NULL, &security_struct_notepad, NULL, TRUE, NULL, NULL, NULL, &startup_info_notepad, &process_information_notepad);

	if (!result || !result2)
	{
		printf(" error code => %d \n ", GetLastError());
		getchar();
		return 0;
	}

	printf("calc_handler_code => %d \n", process_information_calc.hProcess);
	printf("notepad_handler_code => %d \n", process_information_notepad.hProcess);
	//Z:\projects\cpp\ConsoleApplication2\Debug\ConsoleApplication1.exe
	WCHAR sub_process[] = L"Z:\\projects\\cpp\\ConsoleApplication2\\Debug\\ConsoleApplication1.exe";

	WCHAR command[20] = { 0 };
	command[0] = (WCHAR)process_information_calc.hProcess;
	command[1] = (WCHAR)process_information_notepad.hProcess;

	STARTUPINFO startup_info_sub = { 0 };
	startup_info_sub.cb = sizeof STARTUPINFO;

	PROCESS_INFORMATION process_information_sub = { 0 };

	SECURITY_ATTRIBUTES security_struct_sub = { 0 };
	security_struct_sub.nLength = sizeof SECURITY_ATTRIBUTES;
	security_struct_sub.bInheritHandle = FALSE;  //可继承

	CreateProcess(sub_process, command, &security_struct_sub, NULL, TRUE, NULL, NULL, NULL, &startup_info_sub, &process_information_sub);

	getchar();

	return 0;
}


// Main.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

//显示的导入

//1 定义函数指针
typedef DWORD(_cdecl * PFindProcessW)(PWCHAR);
//2 定义变量
PFindProcessW pMyFindProcessW = NULL;


int main()
{
	//3 装载DLL
	HMODULE hModule = LoadLibrary(L"动态链接库.dll");

	//4 获得函数

	pMyFindProcessW = (PFindProcessW)GetProcAddress(hModule, "find_processid_by_name");
	DWORD err = GetLastError();

	WCHAR name[] = L"notepad.exe";
	printf_s("PID=%d \r\n", pMyFindProcessW(name));
	getchar();
    return 0;
}


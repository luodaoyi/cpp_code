#include "stdafx.h"
#include "MyFunction.h"
#include <TlHelp32.h>



// 找到指定进程 返回指定进程id,0=>没找到 -1=>查找出错
// 参数 process_name  要找的目标进程名称
DWORD MyFindProcessW(PWCHAR process_name)
{

	DWORD err = 0;
	//创建进程快照
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ((DWORD)hand == -1) {

		return  -1;
	}

	PROCESSENTRY32 process_info = { 0 };
	process_info.dwSize = sizeof PROCESSENTRY32;

	if (Process32First(hand, &process_info))
	{
		err = GetLastError();
		return -1;
	}

	do
	{
		if (!wcscmp(process_info.szExeFile, process_name)) 
		{

			return process_info.th32ProcessID;
		}

	} while (Process32Next(hand, &process_info));

	return 0;

}
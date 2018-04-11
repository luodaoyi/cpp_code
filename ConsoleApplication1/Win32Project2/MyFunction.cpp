#include "stdafx.h"
#include <Windows.h>
#include <TlHelp32.h>

DWORD find_processid_by_name(PWCHAR process_name)
{

	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if ((int)hand == -1)
	{
		return GetLastError();
	}
	PROCESSENTRY32 process_info = { 0 };
	process_info.dwSize = sizeof PROCESSENTRY32;

	if (!Process32First(hand, &process_info))
	{
		return GetLastError();
	}

	do
	{
		if (!wcscmp(process_info.szExeFile, process_name))
		{
			return process_info.th32ProcessID;
		}

	} while (Process32Next(hand,&process_info));
	return 0;
}



#include "stdafx.h"
#include "MyFunction.h"
#include <TlHelp32.h>



// �ҵ�ָ������ ����ָ������id,0=>û�ҵ� -1=>���ҳ���
// ���� process_name  Ҫ�ҵ�Ŀ���������
DWORD MyFindProcessW(PWCHAR process_name)
{

	DWORD err = 0;
	//�������̿���
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
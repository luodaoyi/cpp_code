#include "stdafx.h"
#include <Windows.h>

//�������ֲ��ҽ���id
//extern "C" _declspec(dllexport) �������� ����Լ�� ������ (�����б�)
extern "C" _declspec(dllexport) DWORD _stdcall find_processid_by_name(PWCHAR process_name);
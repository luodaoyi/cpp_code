#include "stdafx.h"
#include <Windows.h>

//根据名字查找进程id
//extern "C" _declspec(dllexport) 返回类型 调用约定 函数名 (参数列表)
extern "C" _declspec(dllexport) DWORD _stdcall find_processid_by_name(PWCHAR process_name);
// MessageBoxDLL.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


void Alert()
{
	MessageBox(NULL, L"我日哦 咋回事", NULL, NULL);
}
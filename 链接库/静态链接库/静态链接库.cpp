// 静态链接库.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MyFunction.h"

int main()
{
	WCHAR name[] = L"notepad.exe";
	DWORD dwPid = find_processid_by_name(name);
	printf("%d\n", dwPid);
	getchar();
    return 0;
}


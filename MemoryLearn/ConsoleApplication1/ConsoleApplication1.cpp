// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

LPWORD g_pMapData = NULL;

int main()
{
	TCHAR MapName[] = TEXT("共享内存");
	//创建一个文件映射对象
	HANDLE hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, NULL, 0X1000, MapName);
	if (hMapping == NULL) {
		printf("创建文件映射对象失败！");
		getchar();
		return 1;
	}
	//映射虚拟内存
	g_pMapData = (LPWORD)MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, 0X1000);
	if (g_pMapData == NULL) {
		printf("映射虚拟内存失败！");
		getchar();
		return 1;
	}

    return 0;
}


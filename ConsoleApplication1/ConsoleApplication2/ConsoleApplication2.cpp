// ConsoleApplication2.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int main()
{
	TCHAR msg[] = TEXT("小白兔白又白，两只耳朵竖起来。割腕静脉割动脉，一动不动真可爱。");

	printf("%d \n", sizeof(msg));

	_asm {
		jmp $
	}
	return 0;
	//typedef wchar_t     TCHAR;   63
	//typedef char            TCHAR;    63 
}
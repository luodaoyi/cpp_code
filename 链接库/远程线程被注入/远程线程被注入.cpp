// 远程线程被注入.cpp: 定义控制台应用程序的入口点。
//


#include "stdafx.h"
#include "stdio.h"
#include "windows.h"
void fun()
{
	printf_s("远程线程创建成功！！！\r\n");
}


int main()
{
	printf_s("START %d \r\n", (DWORD)fun);
	getchar();
	return 0;
}

// Win32Project3.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Project3.h"
#include <Tlhelp32.h>
#include <commctrl.h>
#include <stdio.h>


VOID OkPushDown(HWND hList)
{
	WCHAR wcBuffer[255] = { 0 };
	//DWORD dwIndex = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
	DWORD dwIndex = ListView_GetSelectionMark(hList);

	ListView_GetItemText(hList, dwIndex, 0, wcBuffer, 255);
	MessageBox(NULL, wcBuffer, NULL, NULL);
}



//ListView_DeleteAllItems(hList); //删除所有项
//ListView_SetBkColor(hList, RGB(255, 0, 0));//设置背景颜色
//ListView_SetTextColor(hList, RGB(255, 0, 0));//设置文本颜色
//ListView_SetTextBkColor(hList, RGB(255, 0, 0));//设置文本背景色
//ListView_GetItemText(hList, 1, 0, wcBuffer, 255);//获得制定行列的文本信息
//ListView_GetNextItem(hList, -1, LVNI_SELECTED);//筛选符合要求的选项
//ListView_GetSelectionMark(hList);//获得被选中行的索引
//ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);//设置控件的风格
VOID init_list_control(HWND hList)
{
	HANDLE pHand = 0;
	PWCHAR title_name[] = { L"进程名称", L"PID", L"线程数" };
	LVCOLUMN column = { 0 };
	LVITEM item = { 0 };
	item.mask = LVIF_TEXT;
	column.mask = LVCF_FMT | LVCF_TEXT|LVCF_WIDTH;
	column.fmt = LVCFMT_CENTER;
	column.cx = 150;
	WCHAR buffer[255] = { 0 };

	PROCESSENTRY32 process_info = { 0 };
	process_info.dwSize = sizeof PROCESSENTRY32;

	//设置表头
	for (int i = 0; i < 3; i++)
	{
		column.pszText = title_name[i];
		ListView_InsertColumn(hList, i, &column);
	}

	
	//获得进程快照
	pHand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	Process32First(pHand, &process_info);
	
	
	//设置进程属性
	do
	{
		swprintf_s(buffer, L"%s", process_info.szExeFile);
		item.pszText = buffer;
		item.iSubItem = 0;
		ListView_InsertItem(hList, &item);
		ZeroMemory(buffer,255);

		swprintf_s(buffer, L"%d", process_info.th32ProcessID);
		item.pszText = buffer;
		item.iSubItem = 1;
		ListView_SetItem(hList, &item);
		ZeroMemory(buffer, 255);

		swprintf_s(buffer, L"%d", process_info.cntThreads);
		item.pszText = buffer;
		item.iSubItem = 2;
		ListView_SetItem(hList, &item);

	} while (Process32Next(pHand,&process_info));
}


INT_PTR CALLBACK DialogProc(
	HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch (uMsg)
	{
	case WM_INITDIALOG: //初始化
		//MessageBox(NULL, L"初始化", L"启动", MB_OK);
		//初始化控件
		init_list_control(GetDlgItem(hwndDlg, IDC_LIST1));

		return TRUE;
	case WM_CLOSE:		//关闭窗口
		//MessageBox(NULL, L"关闭", L"退出", MB_OK);
		EndDialog(hwndDlg, 0);
		return TRUE;
	case WM_COMMAND:	//控件发送的消息
		switch (LOWORD(wParam))
		{
		case IDOK:
			//查找
			OkPushDown(GetDlgItem(hwndDlg, IDC_LIST1));
			return TRUE;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return FALSE;
}



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1),NULL, DialogProc);
	return 0;
}


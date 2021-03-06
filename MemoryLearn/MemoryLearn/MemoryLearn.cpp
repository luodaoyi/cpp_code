//MemoryLearn.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MemoryLearn.h"
#include <Windows.h>
#include <commctrl.h>
#include <Tlhelp32.h>

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//主窗口
VOID InitListControl(HWND hList); //初始化list 控件
VOID LoadProcessData(HWND hList, HWND hEdit);//载入进程信息到list空间中

INT_PTR CALLBACK MemInfoDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//内存详情窗口
VOID InitMemListControl(HWND hList); //c初始化list 控件
VOID LoadMemData(HWND hList);//载入内存信息到list空间中

DWORD OperatePID = 0;
CRITICAL_SECTION cs;            //创建令牌

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	InitializeCriticalSection(&cs); //初始化令牌
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

	return 0;
}


INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	INT selectIndex = -1;
	HWND hList = GetDlgItem(hWnd, IDC_LIST1);
	WCHAR buffer[255] = { 0 };
	ZeroMemory(&buffer, 0);

	switch (message)
	{
	case WM_INITDIALOG: //初始化
						//初始化控件
		InitListControl(hList);
		LoadProcessData(hList, hWnd);
		return TRUE;
	case WM_CLOSE:		//关闭窗口
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);//退出
			break;
		case IDC_BUTTON_FLUSH_LIST:
			LoadProcessData(hList, hWnd);
			break;
		case IDC_BUTTON_VIEW_PROCESS:

			//获得选中的pid
			selectIndex = ListView_GetSelectionMark(hList);
			if (selectIndex < 0) {
				break;
			}
			ListView_GetItemText(hList, selectIndex, 1, buffer, 255);

			selectIndex = -1;
			EnterCriticalSection(&cs);      //进入临界区
			OperatePID = _wtoi(buffer); //记录要查看内存的pid
			LeaveCriticalSection(&cs);      //离开临界区

			if (OperatePID)
			{
				DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG2), NULL, &MemInfoDialogProc);
			}
			else
			{
				MessageBox(NULL, L"请选择要查看的进程！", L"错误", MB_OK);
			}

			break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return 0;
}

//初始化list控件
VOID InitListControl(HWND hList)
{
	//LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
	//设置多选
	ListView_SetExtendedListViewStyle(hList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	PWCHAR title_name[] = { (PWCHAR)L"进程名称", (PWCHAR)L"PID", (PWCHAR)L"线程数" };
	LVCOLUMN column = { 0 };

	column.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt = LVCFMT_CENTER;

	//设置表头
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			column.cx = 150;
		}
		else {
			column.cx = 100;
		}
		column.pszText = title_name[i];
		ListView_InsertColumn(hList, i, &column);
	}
}

//载入进程信息到列表控件中 
VOID LoadProcessData(HWND hList, HWND hDialog)
{
	//先清理所有行
	ListView_DeleteAllItems(hList);

	//获取进程信息，并且加载
	HANDLE pHand = 0;
	WCHAR buffer[255] = { 0 };
	PROCESSENTRY32 process_info = { 0 };
	process_info.dwSize = sizeof PROCESSENTRY32;

	LVITEM item = { 0 };
	item.mask = LVIF_TEXT;

	//获得进程快照
	pHand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	Process32First(pHand, &process_info);

	INT process_count = 0;
	//设置进程属性
	do
	{
		if (process_info.th32ProcessID == 0) continue;
		//程序名
		swprintf_s(buffer, L"%s", process_info.szExeFile);
		item.pszText = buffer;
		item.iSubItem = 0;
		ListView_InsertItem(hList, &item);
		ZeroMemory(buffer, 255);

		//pid
		swprintf_s(buffer, L"%d", process_info.th32ProcessID);
		item.pszText = buffer;
		item.iSubItem = 1;
		ListView_SetItem(hList, &item);
		ZeroMemory(buffer, 255);

		//自线程数
		swprintf_s(buffer, L"%d", process_info.cntThreads);
		item.pszText = buffer;
		item.iSubItem = 2;
		ListView_SetItem(hList, &item);

		process_count += 1;
	} while (Process32Next(pHand, &process_info));

	ZeroMemory(buffer, 255);
	swprintf_s(buffer, L"%d", process_count);

	SetDlgItemText(hDialog, IDC_EDIT1, buffer);
	ZeroMemory(buffer, 255);
}




/*******************************/
/*内存详情窗口*/
INT_PTR CALLBACK MemInfoDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	WCHAR buffer[255] = { 0 };
	HWND hList = GetDlgItem(hWnd, IDC_LIST1);
	switch (message)
	{
	case WM_INITDIALOG: //初始化
						//初始化控件
		EnterCriticalSection(&cs);      //进入临界区
		swprintf_s(buffer, L"%d", OperatePID);//需要查看内存的pid
		LeaveCriticalSection(&cs);      //离开临界区
		SetDlgItemText(hWnd, IDC_EDIT1, buffer);

		ZeroMemory(buffer, 255);
		InitMemListControl(hList);
		LoadMemData(hList);
		return TRUE;
	case WM_CLOSE:		//关闭窗口
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);//退出
			break;
		case IDC_BUTTON_FLUSH_INFO:
			LoadMemData(hList);
			break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		EndDialog(hWnd, 0);
		break;
	default:
		break;
	}
	return 0;
}


VOID InitMemListControl(HWND hList)
{
	ListView_SetExtendedListViewStyle(hList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	PWCHAR title_name[] = { (PWCHAR)L"BaseAddress", (PWCHAR)L"区域基地址", (PWCHAR)L"标识区域大小" ,(PWCHAR)L"页面状态", (PWCHAR)L"页面类型" };
	LVCOLUMN column = { 0 };

	column.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt = LVCFMT_CENTER;
	column.cx = 100;

	//设置表头
	for (int i = 0; i < 5; i++)
	{
		column.pszText = title_name[i];
		ListView_InsertColumn(hList, i, &column);
	}
}

VOID LoadMemData(HWND hList)
{
	SYSTEM_INFO info = { 0 };
	WCHAR buffer[255] = { 0 };
	GetSystemInfo(&info);

	EnterCriticalSection(&cs);      //进入临界区
	HANDLE handProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, OperatePID);
	LeaveCriticalSection(&cs);      //离开临界区


	MEMORY_BASIC_INFORMATION memInfo = { 0 };
	ZeroMemory(&memInfo, sizeof(memInfo));

	DWORD pBeginPoint = (DWORD)info.lpMinimumApplicationAddress;
	DWORD PEndPoint = (DWORD)info.lpMaximumApplicationAddress;

	//先清理所有行
	ListView_DeleteAllItems(hList);

	
	for (DWORD i = pBeginPoint; i <= PEndPoint;)
	{
		VirtualQueryEx(handProcess, (LPCVOID)i, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
		i += memInfo.RegionSize;

		LVITEM lvitem = { 0 };
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = 0;

		lvitem.iSubItem = 0;
		ZeroMemory(buffer, 255);
		swprintf_s(buffer, L"0x%p", (DWORD)memInfo.BaseAddress);
		lvitem.pszText = buffer;
		ListView_InsertItem(hList, &lvitem);

		lvitem.iSubItem = 1;
		ZeroMemory(buffer, 255);
		swprintf_s(buffer, L"0x%p", (DWORD)memInfo.AllocationBase);
		lvitem.pszText = buffer;
		ListView_SetItem(hList, &lvitem);

		lvitem.iSubItem = 2;
		ZeroMemory(buffer, 255);
		swprintf_s(buffer, L"0x%x", memInfo.RegionSize);
		lvitem.pszText = buffer;
		ListView_SetItem(hList, &lvitem);

		lvitem.iSubItem = 3;
		ZeroMemory(buffer, 255);
		switch (memInfo.State)
		{
		case MEM_COMMIT:
			swprintf_s(buffer, L"%s", L"[COMMIT]");
			break;
		case MEM_FREE:
			swprintf_s(buffer, L"%s", L"[FREE]");
			break;
		case MEM_RESERVE:
			swprintf_s(buffer, L"%s", L"[RESERVE]");
			break;
		default:
			swprintf_s(buffer, L"%s", L"[----]");
			break;
		}
		lvitem.pszText = buffer;
		ListView_SetItem(hList, &lvitem);

		lvitem.iSubItem = 4;
		ZeroMemory(buffer, 255);
		switch (memInfo.Type)
		{
		case MEM_IMAGE:
			swprintf_s(buffer, L"%s", L"[IMAGE]");
			break;
		case MEM_MAPPED:
			swprintf_s(buffer, L"%s", L"[MAPPED]");
			break;
		case MEM_PRIVATE:
			swprintf_s(buffer, L"%s", L"[PRIVATE]");
			break;
		default:
			swprintf_s(buffer, L"%s", L"[----]");
			break;
		}
		lvitem.pszText = buffer;
		ListView_SetItem(hList, &lvitem);
	}

}
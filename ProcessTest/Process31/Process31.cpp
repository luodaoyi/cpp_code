// ProcessTest.cpp : 定义应用程序的入口点。
// 暂停任意进程下所有的线程


#include "stdafx.h"
#include "Process31.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <commctrl.h>
#include <stdio.h>
#include <Psapi.h>
#include <comdef.h>



INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//处理事件
VOID InitListControl(HWND hList);//初始化list控件
VOID LoadProcessData(HWND hList);//加载进程信息
VOID HandlerThread(BOOL suspend);//处理线程
VOID LoadSelectItems(HWND hList);//载入选择项


static DWORD selectedPidArr[255] = { 0 };//当前选择的进程id (pid)
static DWORD selectPidCount = 0;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	return 0;
}


INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	HWND hList = GetDlgItem(hWnd, IDC_LIST1);
	switch (message)
	{
	case WM_INITDIALOG: //初始化
		//初始化控件
		InitListControl(hList);
		LoadProcessData(hList);
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
		case IDC_BUTTON4:
			LoadProcessData(hList);//刷新 加载进程信息
			break;
		case IDC_BUTTON1: //
			if (selectPidCount == 0)
			{
				LoadSelectItems(hList);
				HandlerThread(TRUE);
			}
			else
			{
				HandlerThread(FALSE);
			}

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//初始化list控件
VOID InitListControl(HWND hList)
{
	//LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
	//设置多选
	ListView_SetExtendedListViewStyle(hList, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	PWCHAR title_name[] = { L"进程名称", L"PID", L"线程数" };
	LVCOLUMN column = { 0 };

	column.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt = LVCFMT_CENTER;
	column.cx = 150;

	//设置表头
	for (int i = 0; i < 3; i++)
	{
		column.pszText = title_name[i];
		ListView_InsertColumn(hList, i, &column);
	}
}

//载入进程信息到列表控件中
VOID LoadProcessData(HWND hList)
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

		//之前守护的进程 需要吧checkbox打勾！
		for (DWORD i = 0; i < selectPidCount; i++)
		{
			DWORD pidTmp = selectedPidArr[i];
			if (pidTmp == 0xcccccccc || pidTmp == 0) break;

			if (pidTmp == process_info.th32ProcessID)
			{
				ListView_SetCheckState(hList, 0, TRUE);
			}

		}

	} while (Process32Next(pHand, &process_info));
}


//载入已选择的选项
VOID LoadSelectItems(HWND hList)
{
	WCHAR wcBuffer[255] = { 0 };
	DWORD count = ListView_GetItemCount(hList);//获得总行数
	selectPidCount = 0;
	for (DWORD i = 0; i < count; i++)
	{
		//判断checkbox 状态
		if (ListView_GetCheckState(hList, i))
		{
			ListView_GetItemText(hList, i, 1, wcBuffer, 255); //获得内容
			selectedPidArr[selectPidCount] = _wtoi(wcBuffer);
			selectPidCount++;
		}
	}
}

//处理线程  true 暂停 false 释放
VOID HandlerThread(BOOL suspend)
{
	//获得所有线程
	HANDLE hSnapshotThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	THREADENTRY32 threadEntry = { sizeof(THREADENTRY32) };


	Thread32First(hSnapshotThread, &threadEntry);
	BOOL match = FALSE;
	do
	{
		for (DWORD i = 0; i < selectPidCount; i++)
		{
			
			if (threadEntry.th32OwnerProcessID == selectedPidArr[i])
			{
				match = TRUE;
				break;
			}
		}

		if (!match) continue;//没有匹配的就下一个

		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
		if (suspend)
		{
			SuspendThread(hThread);
		}
		else
		{
			ResumeThread(hThread);
		}
		CloseHandle(hThread);

		match = FALSE;//归位恢复，未匹配
	} while (Thread32Next(hSnapshotThread,&threadEntry));
	
	if (!suspend)//如果是恢复，那么暂停列表要清空
	{
		ZeroMemory(selectedPidArr, 255);
		selectPidCount = 0;
	}
}
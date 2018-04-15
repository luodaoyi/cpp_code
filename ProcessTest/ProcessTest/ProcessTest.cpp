// ProcessTest.cpp : 定义应用程序的入口点。
//


#include "stdafx.h"
#include "ProcessTest.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <commctrl.h>
#include <stdio.h>
#include <Psapi.h>
#include <comdef.h>


INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//处理事件
VOID InitListControl(HWND hList);//初始化list控件
VOID LoadProcessData(HWND hList);//加载进程信息
DWORD WINAPI DaemonThread(LPVOID lpParameter);//开始守护某进程
VOID LetUsDaemonProcess(HWND hList);//按下守护按钮


static HANDLE daeamArr[255] = { 0 };
static DWORD daemonProcessCount = 0;
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
		//创建时注册快捷键
		RegisterHotKey(hWnd, 1, MOD_CONTROL | MOD_NOREPEAT, 'H');// Ctrl + H 隐藏
		RegisterHotKey(hWnd, 2, MOD_CONTROL | MOD_NOREPEAT, 'M');// Ctrl + M 显示
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
			DestroyWindow(hWnd);
			break;
		case IDC_BUTTON1:
			LoadProcessData(hList);
			break;
		case IDOK:
			LetUsDaemonProcess(hList);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	// 快捷键消息
	case WM_HOTKEY:
		switch (wParam)
		{
		case 1:
			OutputDebugString(L"Ctrl + H 被按下！ \n");
			ShowWindow(hWnd, SW_HIDE);
			break;
		case 2:
			OutputDebugString(L"Ctrl + M 被按下！ \n");
			ShowWindow(hWnd, SW_SHOW);
			break;

		}
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

//载入进程信息
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



//开始守护进程 参数 list控件 句柄
VOID LetUsDaemonProcess(HWND hList)
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

	//结束之前所有的守护进程
	BOOL result = TRUE;
	for (DWORD i = 0; i < daemonProcessCount; i++)
	{
		TerminateThread(daeamArr[i], 0);
	}
	ZeroMemory(daeamArr, 255);
	daemonProcessCount = 0;

	//开始新的守护线程
	DWORD dwTid = 0;
	for (DWORD i = 0; i < selectPidCount; i++)
	{
		daeamArr[i] = CreateThread(NULL, 0, DaemonThread, &(selectedPidArr[i]), 0, &dwTid);
		daemonProcessCount++;
	}
}

//开始守护某进程 参数 pid
DWORD WINAPI DaemonThread(LPVOID lpParameter)
{
	WCHAR processFullPathData[MAX_PATH] = { 0 };
	DWORD processId = *(LPDWORD)lpParameter;

	//进程对象
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	si.cb = sizeof(si);

	//打开pid并且返回句柄
	HANDLE processHandle = OpenProcess(
		PROCESS_ALL_ACCESS,
		TRUE,
		processId
	);
	if (NULL == processHandle)
	{
		MessageBox(NULL, L"打开失败，权限不足！", NULL, MB_OK);
		OutputDebugString(L"打开失败  mmp \n");
		return 1;
	}

	//获得进程下的第一个模块
	HMODULE ModuleHandle = NULL;
	DWORD ReturnLength = GetModuleFileNameEx(processHandle, ModuleHandle, processFullPathData, sizeof(processFullPathData));

	if (ReturnLength == 0)
	{
		//清空
		RtlZeroMemory(processFullPathData, MAX_PATH);
		//再次查询
		QueryFullProcessImageName(processHandle, 0, processFullPathData, &ReturnLength);
		if (ReturnLength == 0)
		{
			OutputDebugString(L"枚举信息失败  mmp\n");
			return 1;
		}
	}

	OutputDebugString(processFullPathData);

	wchar_t buffer[256];


	//守护这些进程，如果结束就特么重启
	while (true)
	{

		//等待单个
		DWORD dwRetFlag = WaitForSingleObject(processHandle, INFINITE);
		if (dwRetFlag == WAIT_FAILED){
			DWORD errorCode = GetLastError();
			wsprintfW(buffer, L"等待失败!!error code : %d \n", errorCode);
			OutputDebugString(buffer);
			return -1;
		}
		CloseHandle(processHandle);

		//创建子进程，判断是否执行成功
		if (!CreateProcess(processFullPathData, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			DWORD errorCode = GetLastError();
			wsprintfW(buffer, L"守护进程启动失败，程序即将退出!!error code : %d \n", errorCode);
			OutputDebugString(buffer);
			return -1;
		}
		*(LPDWORD)lpParameter = pi.dwProcessId;
		processHandle = pi.hProcess;//更新已有守护的进程
		Sleep(100);
	}
}

//ReadFile.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ReadFile.h"
#include <Windows.h>
#include <Commdlg.h>



INT_PTR CALLBACK    DialogProc(HWND, UINT, WPARAM, LPARAM);
VOID LoadFile(HWND hwndDialog);
VOID ShowText(HWND hwndDialog, DWORD pageNum);

HANDLE g_hFile;						//文件句柄
HANDLE g_hFileMapping;				//文件映射的内存

DWORD g_dwAllocationGranularity;    //分配粒度
DWORD g_fileSize;                   //文件总大小
DWORD g_currentPage;				//当前页码
DWORD g_totalPage;					//总页数
BOOL  g_is_ascii = TRUE;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// 获取系统分配内存粒度
	SYSTEM_INFO sysInfo = { 0 };
	GetSystemInfo(&sysInfo);
	g_dwAllocationGranularity = sysInfo.dwAllocationGranularity;

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	return 0;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDC_BUTTON_ASCII:
			g_is_ascii = TRUE;
			ShowText(hWnd, g_currentPage);
			break;
		case IDC_BUTTON_UNICODE:
			g_is_ascii = FALSE;
			ShowText(hWnd, g_currentPage);
			break;
		case IDOK:
			LoadFile(hWnd);
			break;
		case IDC_BUTTON_PRE:
			//上一页
			g_currentPage--;
			ShowText(hWnd, g_currentPage);
			break;
		case IDC_BUTTON_NEXT:
			//下一页
			g_currentPage++;
			ShowText(hWnd, g_currentPage);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		CloseHandle(g_hFile);
		CloseHandle(g_hFileMapping);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//读取文件 映射内存
VOID LoadFile(HWND hwndDialog)
{
	TCHAR msgBoxTextBuffer[256] = { 0 };
	
	TCHAR fileNameBuffer[MAX_PATH] = { 0 };

	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	ofn.hwndOwner = hwndDialog;
	ofn.lpstrFile = fileNameBuffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXTENSIONDIFFERENT;

	if (!GetOpenFileName(&ofn)) {
		return;
	}
	SetDlgItemText(hwndDialog, IDC_EDIT_FILENAME, fileNameBuffer);

	//打开文件,获取文件句柄
	//g_hFile = CreateFile(fileNameBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	g_hFile = CreateFile(fileNameBuffer, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (g_hFile == INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		_stprintf_s(msgBoxTextBuffer, L"打开文件失败 => %d", error);
		MessageBox(NULL, msgBoxTextBuffer, L"error", MB_OK);
		CloseHandle(g_hFile);
		return;
	}
	//计算页码
	g_fileSize = GetFileSize(g_hFile, NULL);
	g_totalPage = g_fileSize % g_dwAllocationGranularity == 0 ? g_fileSize / g_dwAllocationGranularity : g_fileSize / g_dwAllocationGranularity + 1;

	//映射到内存
	g_hFileMapping = CreateFileMapping(g_hFile, NULL, PAGE_READWRITE | SEC_COMMIT, NULL, NULL, L"_open_file_edit");

	if (g_hFileMapping == NULL) {
		
		DWORD error = GetLastError();
		_stprintf_s(msgBoxTextBuffer, L"创建映射文件失败 => %d", error);
		MessageBox(NULL, msgBoxTextBuffer, L"error", MB_OK);
		CloseHandle(g_hFileMapping);
		return;
	}
	//显示第一页
	ShowText(hwndDialog, 1);
}

VOID ShowText(HWND hwndDialog, DWORD pageNum)
{
	TCHAR msgBoxTextBuffer[256] = { 0 };

	if (pageNum <= 0)
	{
		pageNum = 1;
	}
	if (pageNum >= g_totalPage)
	{
		pageNum = g_totalPage;
	}

	g_currentPage = pageNum;

	DWORD offset = g_dwAllocationGranularity * (pageNum - 1);
	DWORD limit = g_dwAllocationGranularity + 10;

	//获得上一页/下一页按钮的句柄
	HWND btn_pre = GetDlgItem(hwndDialog, IDC_BUTTON_PRE);
	HWND btn_next = GetDlgItem(hwndDialog, IDC_BUTTON_NEXT);
	//如果是第一页 就关闭 上一页的那个按钮
	if (pageNum == 1) {
		EnableWindow(btn_pre, FALSE);
	}
	//如果是最后一页 就关闭下一个按钮
	if (pageNum == g_totalPage)
	{
		EnableWindow(btn_next, FALSE);
		limit = g_fileSize;
	}

	//显示页码/总页数
	TCHAR szBuff[10] = { 0 };
	_stprintf_s(szBuff, L"%d/%d", pageNum, g_totalPage);
	SetDlgItemText(hwndDialog, IDC_EDIT_TOTAL_PAGE, szBuff);

	

	//创建映射
	PBYTE pAddress = (PBYTE)MapViewOfFile(g_hFileMapping, FILE_MAP_READ, 0, offset, limit);
	if (pAddress == NULL) {
		DWORD error = GetLastError();
		_stprintf_s(msgBoxTextBuffer, L"创建映射内存 => %d", error);
		MessageBox(NULL, msgBoxTextBuffer, L"error", MB_OK);
		return;
	}

	//读取文本
	BYTE* buftext = new BYTE[limit + 1];
	ZeroMemory(buftext, limit + 1);
	CopyMemory(buftext, pAddress, limit);

	//ASCI码
	if (g_is_ascii)
	{
		SetDlgItemTextA(hwndDialog, IDC_EDIT1, (LPCSTR)buftext);

	}
	else
	{
		SetDlgItemTextW(hwndDialog, IDC_EDIT1, (LPCWSTR)buftext);
	}

	UnmapViewOfFile(pAddress);
	delete buftext;

}
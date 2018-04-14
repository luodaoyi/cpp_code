// Win32Project2.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Project2.h"
#include "MyFunction.h"
#include <wchar.h>

//窗口过程函数
INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	DWORD pid = 0;
	WCHAR buff[255] = { 0 };
	WCHAR process_name[255] = { 0 };
	HANDLE  hProcess = NULL;
	//IDC_EDIT1
	switch (uMsg)
	{
	case WM_INITDIALOG: //初始化
		MessageBox(NULL, L"初始化", L"启动", MB_OK);
		return TRUE;
	case WM_CLOSE:		//关闭窗口
		MessageBox(NULL, L"关闭", L"退出", MB_OK);
		EndDialog(hwndDlg, 0);
		return TRUE;
	case WM_COMMAND:	//控件发送的消息
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:

			/*GetDlgItemText(hwndDlg, IDC_EDIT1, wcBuffer, 255);
			MessageBox(NULL, wcBuffer, L"Button1", MB_OK);*/

			GetDlgItemText(hwndDlg, IDC_EDIT2, process_name, 255);
		
			MessageBox(NULL, process_name, L"进程名称", MB_OK);
			pid = find_processid_by_name(process_name);
			if (pid > 0)
			{
				WCHAR t[20];
				_itow_s(pid, t, 10);
				wcscpy_s(buff, TEXT("pid=>"));
				wcscat_s(buff, t);
				MessageBox(NULL, buff, L"找到pid了", MB_OK);
			}
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (hProcess)
			{
				MessageBox(NULL, process_name, L"打开进程了！", MB_OK);
				TerminateProcess(hProcess, 0);
			}
			MessageBox(NULL, process_name, L"关闭进程了！", MB_OK);

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
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
}

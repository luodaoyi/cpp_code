//Win32Project1.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Project1.h"

INT_PTR CALLBACK DialogProc(
	HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MYDIALOG), NULL, DialogProc);
	return 0;
}

//窗口过程函数
INT_PTR CALLBACK DialogProc(
	HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	WCHAR wcBuffer[255] = { 0 };
	//IDC_EDIT1
	switch (uMsg)
	{
	case WM_INITDIALOG: //初始化
		MessageBox(NULL, L"初始化", L"启动", MB_OK);
		SetDlgItemText(hwndDlg, IDC_EDIT1, L"hello world");
		return TRUE;
	case WM_CLOSE:		//关闭窗口
		MessageBox(NULL, L"关闭", L"退出", MB_OK);
		EndDialog(hwndDlg, 0);
		return TRUE;
	case WM_COMMAND:	//控件发送的消息
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:

			GetDlgItemText(hwndDlg, IDC_EDIT1, wcBuffer,255);
			MessageBox(NULL, wcBuffer, L"Button1", MB_OK);
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
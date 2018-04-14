// Win32Project2.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Win32Project2.h"
#include "MyFunction.h"
#include <wchar.h>

//���ڹ��̺���
INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	DWORD pid = 0;
	WCHAR buff[255] = { 0 };
	WCHAR process_name[255] = { 0 };
	HANDLE  hProcess = NULL;
	//IDC_EDIT1
	switch (uMsg)
	{
	case WM_INITDIALOG: //��ʼ��
		MessageBox(NULL, L"��ʼ��", L"����", MB_OK);
		return TRUE;
	case WM_CLOSE:		//�رմ���
		MessageBox(NULL, L"�ر�", L"�˳�", MB_OK);
		EndDialog(hwndDlg, 0);
		return TRUE;
	case WM_COMMAND:	//�ؼ����͵���Ϣ
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:

			/*GetDlgItemText(hwndDlg, IDC_EDIT1, wcBuffer, 255);
			MessageBox(NULL, wcBuffer, L"Button1", MB_OK);*/

			GetDlgItemText(hwndDlg, IDC_EDIT2, process_name, 255);
		
			MessageBox(NULL, process_name, L"��������", MB_OK);
			pid = find_processid_by_name(process_name);
			if (pid > 0)
			{
				WCHAR t[20];
				_itow_s(pid, t, 10);
				wcscpy_s(buff, TEXT("pid=>"));
				wcscat_s(buff, t);
				MessageBox(NULL, buff, L"�ҵ�pid��", MB_OK);
			}
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (hProcess)
			{
				MessageBox(NULL, process_name, L"�򿪽����ˣ�", MB_OK);
				TerminateProcess(hProcess, 0);
			}
			MessageBox(NULL, process_name, L"�رս����ˣ�", MB_OK);

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

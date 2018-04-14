// Win32Project3.cpp : ����Ӧ�ó������ڵ㡣
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



//ListView_DeleteAllItems(hList); //ɾ��������
//ListView_SetBkColor(hList, RGB(255, 0, 0));//���ñ�����ɫ
//ListView_SetTextColor(hList, RGB(255, 0, 0));//�����ı���ɫ
//ListView_SetTextBkColor(hList, RGB(255, 0, 0));//�����ı�����ɫ
//ListView_GetItemText(hList, 1, 0, wcBuffer, 255);//����ƶ����е��ı���Ϣ
//ListView_GetNextItem(hList, -1, LVNI_SELECTED);//ɸѡ����Ҫ���ѡ��
//ListView_GetSelectionMark(hList);//��ñ�ѡ���е�����
//ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);//���ÿؼ��ķ��
VOID init_list_control(HWND hList)
{
	HANDLE pHand = 0;
	PWCHAR title_name[] = { L"��������", L"PID", L"�߳���" };
	LVCOLUMN column = { 0 };
	LVITEM item = { 0 };
	item.mask = LVIF_TEXT;
	column.mask = LVCF_FMT | LVCF_TEXT|LVCF_WIDTH;
	column.fmt = LVCFMT_CENTER;
	column.cx = 150;
	WCHAR buffer[255] = { 0 };

	PROCESSENTRY32 process_info = { 0 };
	process_info.dwSize = sizeof PROCESSENTRY32;

	//���ñ�ͷ
	for (int i = 0; i < 3; i++)
	{
		column.pszText = title_name[i];
		ListView_InsertColumn(hList, i, &column);
	}

	
	//��ý��̿���
	pHand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	Process32First(pHand, &process_info);
	
	
	//���ý�������
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
	case WM_INITDIALOG: //��ʼ��
		//MessageBox(NULL, L"��ʼ��", L"����", MB_OK);
		//��ʼ���ؼ�
		init_list_control(GetDlgItem(hwndDlg, IDC_LIST1));

		return TRUE;
	case WM_CLOSE:		//�رմ���
		//MessageBox(NULL, L"�ر�", L"�˳�", MB_OK);
		EndDialog(hwndDlg, 0);
		return TRUE;
	case WM_COMMAND:	//�ؼ����͵���Ϣ
		switch (LOWORD(wParam))
		{
		case IDOK:
			//����
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


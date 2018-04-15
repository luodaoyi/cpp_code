// ProcessTest.cpp : ����Ӧ�ó������ڵ㡣
// ��ͣ������������е��߳�


#include "stdafx.h"
#include "Process31.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <commctrl.h>
#include <stdio.h>
#include <Psapi.h>
#include <comdef.h>



INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//�����¼�
VOID InitListControl(HWND hList);//��ʼ��list�ؼ�
VOID LoadProcessData(HWND hList);//���ؽ�����Ϣ
VOID HandlerThread(BOOL suspend);//�����߳�
VOID LoadSelectItems(HWND hList);//����ѡ����


static DWORD selectedPidArr[255] = { 0 };//��ǰѡ��Ľ���id (pid)
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
	case WM_INITDIALOG: //��ʼ��
		//��ʼ���ؼ�
		InitListControl(hList);
		LoadProcessData(hList);
		return TRUE;
	case WM_CLOSE:		//�رմ���
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);//�˳�
			break;
		case IDC_BUTTON4:
			LoadProcessData(hList);//ˢ�� ���ؽ�����Ϣ
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


//��ʼ��list�ؼ�
VOID InitListControl(HWND hList)
{
	//LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
	//���ö�ѡ
	ListView_SetExtendedListViewStyle(hList, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	PWCHAR title_name[] = { L"��������", L"PID", L"�߳���" };
	LVCOLUMN column = { 0 };

	column.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt = LVCFMT_CENTER;
	column.cx = 150;

	//���ñ�ͷ
	for (int i = 0; i < 3; i++)
	{
		column.pszText = title_name[i];
		ListView_InsertColumn(hList, i, &column);
	}
}

//���������Ϣ���б�ؼ���
VOID LoadProcessData(HWND hList)
{
	//������������
	ListView_DeleteAllItems(hList);

	//��ȡ������Ϣ�����Ҽ���
	HANDLE pHand = 0;
	WCHAR buffer[255] = { 0 };
	PROCESSENTRY32 process_info = { 0 };
	process_info.dwSize = sizeof PROCESSENTRY32;


	LVITEM item = { 0 };
	item.mask = LVIF_TEXT;

	//��ý��̿���
	pHand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	Process32First(pHand, &process_info);

	//���ý�������
	do
	{
		if (process_info.th32ProcessID == 0) continue;


		//������
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
		//���߳���
		swprintf_s(buffer, L"%d", process_info.cntThreads);
		item.pszText = buffer;
		item.iSubItem = 2;
		ListView_SetItem(hList, &item);

		//֮ǰ�ػ��Ľ��� ��Ҫ��checkbox�򹴣�
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


//������ѡ���ѡ��
VOID LoadSelectItems(HWND hList)
{
	WCHAR wcBuffer[255] = { 0 };
	DWORD count = ListView_GetItemCount(hList);//���������
	selectPidCount = 0;
	for (DWORD i = 0; i < count; i++)
	{
		//�ж�checkbox ״̬
		if (ListView_GetCheckState(hList, i))
		{
			ListView_GetItemText(hList, i, 1, wcBuffer, 255); //�������
			selectedPidArr[selectPidCount] = _wtoi(wcBuffer);
			selectPidCount++;
		}
	}
}

//�����߳�  true ��ͣ false �ͷ�
VOID HandlerThread(BOOL suspend)
{
	//��������߳�
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

		if (!match) continue;//û��ƥ��ľ���һ��

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

		match = FALSE;//��λ�ָ���δƥ��
	} while (Thread32Next(hSnapshotThread,&threadEntry));
	
	if (!suspend)//����ǻָ�����ô��ͣ�б�Ҫ���
	{
		ZeroMemory(selectedPidArr, 255);
		selectPidCount = 0;
	}
}
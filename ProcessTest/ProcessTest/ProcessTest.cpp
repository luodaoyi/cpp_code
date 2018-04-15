// ProcessTest.cpp : ����Ӧ�ó������ڵ㡣
//


#include "stdafx.h"
#include "ProcessTest.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <commctrl.h>
#include <stdio.h>
#include <Psapi.h>
#include <comdef.h>


INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//�����¼�
VOID InitListControl(HWND hList);//��ʼ��list�ؼ�
VOID LoadProcessData(HWND hList);//���ؽ�����Ϣ
DWORD WINAPI DaemonThread(LPVOID lpParameter);//��ʼ�ػ�ĳ����
VOID LetUsDaemonProcess(HWND hList);//�����ػ���ť


static HANDLE daeamArr[255] = { 0 };
static DWORD daemonProcessCount = 0;
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
		//����ʱע���ݼ�
		RegisterHotKey(hWnd, 1, MOD_CONTROL | MOD_NOREPEAT, 'H');// Ctrl + H ����
		RegisterHotKey(hWnd, 2, MOD_CONTROL | MOD_NOREPEAT, 'M');// Ctrl + M ��ʾ
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
	// ��ݼ���Ϣ
	case WM_HOTKEY:
		switch (wParam)
		{
		case 1:
			OutputDebugString(L"Ctrl + H �����£� \n");
			ShowWindow(hWnd, SW_HIDE);
			break;
		case 2:
			OutputDebugString(L"Ctrl + M �����£� \n");
			ShowWindow(hWnd, SW_SHOW);
			break;

		}
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

//���������Ϣ
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



//��ʼ�ػ����� ���� list�ؼ� ���
VOID LetUsDaemonProcess(HWND hList)
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

	//����֮ǰ���е��ػ�����
	BOOL result = TRUE;
	for (DWORD i = 0; i < daemonProcessCount; i++)
	{
		TerminateThread(daeamArr[i], 0);
	}
	ZeroMemory(daeamArr, 255);
	daemonProcessCount = 0;

	//��ʼ�µ��ػ��߳�
	DWORD dwTid = 0;
	for (DWORD i = 0; i < selectPidCount; i++)
	{
		daeamArr[i] = CreateThread(NULL, 0, DaemonThread, &(selectedPidArr[i]), 0, &dwTid);
		daemonProcessCount++;
	}
}

//��ʼ�ػ�ĳ���� ���� pid
DWORD WINAPI DaemonThread(LPVOID lpParameter)
{
	WCHAR processFullPathData[MAX_PATH] = { 0 };
	DWORD processId = *(LPDWORD)lpParameter;

	//���̶���
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	si.cb = sizeof(si);

	//��pid���ҷ��ؾ��
	HANDLE processHandle = OpenProcess(
		PROCESS_ALL_ACCESS,
		TRUE,
		processId
	);
	if (NULL == processHandle)
	{
		MessageBox(NULL, L"��ʧ�ܣ�Ȩ�޲��㣡", NULL, MB_OK);
		OutputDebugString(L"��ʧ��  mmp \n");
		return 1;
	}

	//��ý����µĵ�һ��ģ��
	HMODULE ModuleHandle = NULL;
	DWORD ReturnLength = GetModuleFileNameEx(processHandle, ModuleHandle, processFullPathData, sizeof(processFullPathData));

	if (ReturnLength == 0)
	{
		//���
		RtlZeroMemory(processFullPathData, MAX_PATH);
		//�ٴβ�ѯ
		QueryFullProcessImageName(processHandle, 0, processFullPathData, &ReturnLength);
		if (ReturnLength == 0)
		{
			OutputDebugString(L"ö����Ϣʧ��  mmp\n");
			return 1;
		}
	}

	OutputDebugString(processFullPathData);

	wchar_t buffer[256];


	//�ػ���Щ���̣������������ô����
	while (true)
	{

		//�ȴ�����
		DWORD dwRetFlag = WaitForSingleObject(processHandle, INFINITE);
		if (dwRetFlag == WAIT_FAILED){
			DWORD errorCode = GetLastError();
			wsprintfW(buffer, L"�ȴ�ʧ��!!error code : %d \n", errorCode);
			OutputDebugString(buffer);
			return -1;
		}
		CloseHandle(processHandle);

		//�����ӽ��̣��ж��Ƿ�ִ�гɹ�
		if (!CreateProcess(processFullPathData, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			DWORD errorCode = GetLastError();
			wsprintfW(buffer, L"�ػ���������ʧ�ܣ����򼴽��˳�!!error code : %d \n", errorCode);
			OutputDebugString(buffer);
			return -1;
		}
		*(LPDWORD)lpParameter = pi.dwProcessId;
		processHandle = pi.hProcess;//���������ػ��Ľ���
		Sleep(100);
	}
}

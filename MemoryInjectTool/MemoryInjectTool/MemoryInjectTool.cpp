//MemoryInjectTool.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MemoryInjectTool.h"
#include <Windows.h>
#include <commctrl.h>
#include <Tlhelp32.h>
#include <Commdlg.h>
#include <Winternl.h>

typedef NTSTATUS(NTAPI *pfnNtQueryInformationProcess)(
	IN  HANDLE ProcessHandle,
	IN  PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN  ULONG ProcessInformationLength,
	OUT PULONG ReturnLength    OPTIONAL
	);

pfnNtQueryInformationProcess gNtQueryInformationProcess;



INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//主窗口
VOID InitListControl(HWND hList); //初始化list 控件
VOID LoadProcessData(HWND hList, HWND hEdit);//载入进程信息到list空间中

INT_PTR CALLBACK MemInfoDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//内存详情窗口
VOID InitMemListControl(HWND hList); //c初始化list 控件
VOID LoadMemData(HWND hList);//载入内存信息到list空间中

VOID InitModuleListControl(HWND hList);//初始化模块list
VOID LoadModuleData(HWND hList);//载入内存信息


VOID InjectModule(HWND hwndDialog, DWORD dwPID);//注入模块									
VOID Yin(PWCHAR yin_text);//隐藏指定名字的模块

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

//主窗口
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
	INT selectIndex = -1;
	WCHAR buffer[255] = { 0 };
	HWND hList = GetDlgItem(hWnd, IDC_LIST1);
	HWND hModuleList = GetDlgItem(hWnd, IDC_LIST_MODULE);
	switch (message)
	{
	case WM_INITDIALOG: //初始化
						//初始化控件
		EnterCriticalSection(&cs);      //进入临界区
		swprintf_s(buffer, L"%d", OperatePID);//需要查看内存的pid
		LeaveCriticalSection(&cs);      //离开临界区
		SetDlgItemText(hWnd, IDC_EDIT1, buffer);

		ZeroMemory(buffer, 255);
		InitMemListControl(hList); //初始化内存控件
		InitModuleListControl(hModuleList);//初始化模块控件

		LoadMemData(hList);//加载内存
		LoadModuleData(hModuleList);//加载模块
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
		case IDC_BUTTON_FLUSH_INFO: //加载内存框架
			LoadMemData(hList);//加载内存
			LoadModuleData(hModuleList);//加载模块
			break;
		case ID_INJECT_DLL: //注入DLL
			// TODO
			EnterCriticalSection(&cs);      //进入临界区
			InjectModule(hWnd, OperatePID);//需要注入的pid
			LeaveCriticalSection(&cs);      //离开临界区

			LoadMemData(hList);//刷新内存
			LoadModuleData(hModuleList);//刷新模块

			break;
		case IDC_INJECT_YIN:
			//获得选中的pid
			selectIndex = ListView_GetSelectionMark(hModuleList);
			if (selectIndex < 0) {
				break;
			}
			ZeroMemory(buffer, 255);
			//ListView_GetItemText(hModuleList, selectIndex, 5, buffer, 255);
			ListView_GetItemText(hModuleList, selectIndex, 1, buffer, 255);

			Yin(buffer); //隐藏模块
			LoadMemData(hList);//刷新内存
			LoadModuleData(hModuleList);//刷新模块

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


//初始化模块list
VOID InitModuleListControl(HWND hList)
{
	ListView_SetExtendedListViewStyle(hList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	PWCHAR title_name[] = { (PWCHAR)L"BaseAddress", (PWCHAR)L"模块名", (PWCHAR)L"模块大小" ,(PWCHAR)L"模块句柄",(PWCHAR)L"引用次数", (PWCHAR)L"模块路径" };
	LVCOLUMN column = { 0 };

	column.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt = LVCFMT_LEFT;
	column.cx = 100;

	//设置表头
	for (int i = 0; i < 6; i++)
	{
		column.pszText = title_name[i];
		if (i == 5)
		{
			column.cx = 400;
		}
		ListView_InsertColumn(hList, i, &column);
	}
}


// 载入模块信息
VOID LoadModuleData(HWND hList)
{
	//先清理所有行
	ListView_DeleteAllItems(hList);

	//获取进程模块信息，并且加载
	HANDLE pHand = 0;
	WCHAR buffer[255] = { 0 };
	MODULEENTRY32  module_info = { 0 };
	module_info.dwSize = sizeof MODULEENTRY32;

	LVITEM item = { 0 };
	item.mask = LVIF_TEXT;

	//获得进程模块快照
	EnterCriticalSection(&cs);      //进入临界区
	pHand = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, OperatePID);
	LeaveCriticalSection(&cs);      //离开临界区

	if (pHand == (HANDLE)-1)
	{
		DWORD errCode = GetLastError();
		swprintf_s(buffer, L"%d", errCode);
	}
	Module32First(pHand, &module_info);

	//设置进程属性
	do
	{
		//"BaseAddress"
		ZeroMemory(buffer, 255);
		swprintf_s(buffer, L"0x%p", (DWORD)module_info.modBaseAddr);
		item.pszText = buffer;
		item.iSubItem = 0;
		ListView_InsertItem(hList, &item);
		ZeroMemory(buffer, 255);

		//"模块名" 
		swprintf_s(buffer, L"%s", module_info.szModule);
		item.pszText = buffer;
		item.iSubItem = 1;
		ListView_SetItem(hList, &item);
		ZeroMemory(buffer, 255);

		// "模块大小" 
		swprintf_s(buffer, L"0x%x", module_info.modBaseSize);
		item.pszText = buffer;
		item.iSubItem = 2;
		ListView_SetItem(hList, &item);

		// "模块句柄"
		swprintf_s(buffer, L"%d", (DWORD)module_info.hModule);
		item.pszText = buffer;
		item.iSubItem = 3;
		ListView_SetItem(hList, &item);

		//"引用次数"
		swprintf_s(buffer, L"%d", module_info.ProccntUsage);
		item.pszText = buffer;
		item.iSubItem = 4;
		ListView_SetItem(hList, &item);

		//"模块路径"
		swprintf_s(buffer, L"%s", module_info.szExePath);
		item.pszText = buffer;
		item.iSubItem = 5;
		item.cchTextMax = 200;
		ListView_SetItem(hList, &item);

	} while (Module32Next(pHand, &module_info));


}

//初始化内存list
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


//载入内存信息
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

		//swprintf_s(buffer, L"%d", i);
		//MessageBox(NULL, buffer, NULL, NULL);

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

/*
注入模块
hwndDialog 主进程句柄
dwPID 将要注入的pid
*/
VOID InjectModule(HWND hwndDialog, DWORD dwPID)
{

	TCHAR wzDllFullPath[MAX_PATH] = { 0 };

	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	ofn.hwndOwner = hwndDialog;
	ofn.lpstrFile = wzDllFullPath;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXTENSIONDIFFERENT;
	ofn.lpstrFilter = TEXT("DLL (*.dll)\0*.dll\0\0");

	if (!GetOpenFileName(&ofn))
	{
		MessageBox(NULL, wzDllFullPath, L"请正确选择DLL文件", MB_OK);
		return;
	}
	//显示选择的文件。  
	MessageBox(NULL, wzDllFullPath, L"即将要注入的DLL路径", MB_OK);

	//打开目标进程
	HANDLE targetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (NULL == targetProcessHandle)
	{
		MessageBox(NULL, L"打开进程失败", L"打开选择的进程失败", MB_OK);
		return;
	}

	WCHAR* VirtualAddress = NULL; //在目标进程中申请的内存地址
	ULONG32 ulDllLength = (ULONG32)_tcslen(wzDllFullPath) + 1; //选择的dll文件大小
	//在目标进程中申请内存 放入需要注入的dll
	VirtualAddress = (WCHAR*)VirtualAllocEx(targetProcessHandle, NULL, ulDllLength * sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);

	if (NULL == VirtualAddress)
	{
		MessageBox(NULL, L"申请内存失败", L"在选择的进程中申请内存空间失败", MB_OK);
		CloseHandle(targetProcessHandle);
		return;
	}

	// 写入dll
	if (FALSE == WriteProcessMemory(targetProcessHandle, VirtualAddress, (LPVOID)wzDllFullPath, ulDllLength * sizeof(WCHAR), NULL))
	{
		MessageBox(NULL, L"写入dll失败", L"在选择的进程中写入dll失败", MB_OK);
		VirtualFreeEx(targetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		CloseHandle(targetProcessHandle);
		return;
	}

	//获得LoadLibraryW 的地址
	LPTHREAD_START_ROUTINE FunctionAddress = NULL;
	FunctionAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
	HANDLE ThreadHandle = INVALID_HANDLE_VALUE;

	//在目标进程中创建进程注入dll
	ThreadHandle = CreateRemoteThread(targetProcessHandle, NULL, 0, FunctionAddress, VirtualAddress, 0, NULL);
	if (NULL == ThreadHandle)
	{
		MessageBox(NULL, L"注入失败", L"在目标进程中创建注入线程失败", MB_OK);
		VirtualFreeEx(targetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		CloseHandle(targetProcessHandle);
		return;
	}

	// 等待注入完成
	WaitForSingleObject(ThreadHandle, INFINITE);


	// 清理
	VirtualFreeEx(targetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);
	CloseHandle(ThreadHandle);
	CloseHandle(targetProcessHandle);
}



//隐藏指定名字的模块
VOID Yin(PWCHAR yin_text)
{



	HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
	if (hNtDll == NULL)
	{
		MessageBox(NULL, L"加载ntdll失败", L"错误", MB_OK);
		return;
	};

	gNtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if (gNtQueryInformationProcess == NULL) {
		MessageBox(NULL, L"查找NtQueryInformation函数地址失败", L"目标进程地址查找失败", MB_OK);
		FreeLibrary(hNtDll);
		return;
	}


	// 获得PEB
	EnterCriticalSection(&cs);      //进入临界区
	//打开目标进程
	//HANDLE targetProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_ALL_ACCESS, FALSE, OperatePID);
	HANDLE targetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, OperatePID);
	LeaveCriticalSection(&cs);      //离开临界区
	if (NULL == targetProcessHandle)
	{
		MessageBox(NULL, L"打开进程失败", L"打开选择的进程失败", MB_OK);
		return;
	}
	PROCESS_BASIC_INFORMATION pbi = { 0 };
	NTSTATUS dwStatus = gNtQueryInformationProcess(targetProcessHandle, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);

	if (dwStatus < 0)
	{
		MessageBox(NULL, L"读取进程信息失败", L"错误", MB_OK);
		return;
	}

	DWORD dwNumberOfBytesRead;
	PEB peb = { 0 };
	ReadProcessMemory(targetProcessHandle, pbi.PebBaseAddress, &peb, sizeof(peb), &dwNumberOfBytesRead);

	PPEB_LDR_DATA pldr = peb.Ldr;

	PLIST_ENTRY pBeginEntry = (PLIST_ENTRY)((DWORD)pldr + 0x14);
	LIST_ENTRY beginEntry;
	ReadProcessMemory(targetProcessHandle, pBeginEntry, &beginEntry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);

	PLIST_ENTRY pNextEntry = beginEntry.Flink;
	LIST_ENTRY nextEntry;

	LDR_DATA_TABLE_ENTRY p;
	WCHAR nameBuffer[MAX_PATH] = { 0 };
	
	while (pBeginEntry != pNextEntry)
	{
		ReadProcessMemory(targetProcessHandle, pNextEntry, &nextEntry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead); //读取整个LIST_ENTRY 结构体

		ZeroMemory(nameBuffer, MAX_PATH);
		ReadProcessMemory(targetProcessHandle, pNextEntry, &p, sizeof(p), &dwNumberOfBytesRead); //读取整个LDR_DATA_TABLE_ENTRY结构体
		
		ReadProcessMemory(targetProcessHandle, p.FullDllName.Buffer, nameBuffer, p.FullDllName.Length, &dwNumberOfBytesRead); //读取名字buffer
		
		//读取指针
		if (_tcscmp(nameBuffer, yin_text) == 0) //相同说明就是要隐藏这个模块
		{
			BOOL result = FALSE;
			//断链
			LIST_ENTRY tmp_list_entry = { 0 };
			ReadProcessMemory(targetProcessHandle, nextEntry.Blink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);//读节点
			tmp_list_entry.Flink = nextEntry.Flink;//改节点
			result = WriteProcessMemory(targetProcessHandle, nextEntry.Blink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);//写回去

			tmp_list_entry = {0};
			ReadProcessMemory(targetProcessHandle, nextEntry.Flink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);
			tmp_list_entry.Blink = nextEntry.Blink;//该节点
			result = WriteProcessMemory(targetProcessHandle, nextEntry.Flink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);//写回去

			/*next->Blink->Flink = next->Flink;
			next->Flink->Blink = next->Blink;*/

			PLIST_ENTRY p_order = (PLIST_ENTRY)((DWORD)pNextEntry - 0x8);
			LIST_ENTRY order = { 0 };
			ReadProcessMemory(targetProcessHandle, p_order, &order, sizeof(LIST_ENTRY), &dwNumberOfBytesRead); //另外一个链表
			
			tmp_list_entry = { 0 };
			ReadProcessMemory(targetProcessHandle, order.Blink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);//读节点
			tmp_list_entry.Flink = order.Flink;//改节点
			result = WriteProcessMemory(targetProcessHandle, order.Blink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);//写回去

			tmp_list_entry = { 0 };
			ReadProcessMemory(targetProcessHandle, order.Flink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);
			tmp_list_entry.Blink = order.Blink;//该节点
			result = WriteProcessMemory(targetProcessHandle, order.Flink, &tmp_list_entry, sizeof(LIST_ENTRY), &dwNumberOfBytesRead);//写回去

			MessageBox(NULL, yin_text, L"已隐藏模块", 0);
			break;
		}
		
		
		pNextEntry = nextEntry.Flink;
	}

	//DWORD dwLoadDataAddr = 0;

	//_asm
	//{
	//	mov eax, fs:[0x30]   //找到 PEB地址
	//	mov ecx, [eax + 0xC] //找到 PEB_LDR_DATA地址
	//	mov dwLoadDataAddr, ecx
	//}
	//PPEB_LDR_DATA pLdr = (PPEB_LDR_DATA)dwLoadDataAddr; //保存链表开始的结构体  
	//PLIST_ENTRY ldr_InMemoryOrderModuleList = (PLIST_ENTRY)(dwLoadDataAddr + 0x14);


	//PLIST_ENTRY ple = ldr_InMemoryOrderModuleList; //按内存排序的模块链表 
	//PLIST_ENTRY next = ple->Flink;
	//while (ple != next)
	//{
	//	PLDR_DATA_TABLE_ENTRY p = (PLDR_DATA_TABLE_ENTRY)next;
	//	MessageBox(NULL, p->FullDllName.Buffer, L"找到模块", 0);

	//	if (_tcscmp(p->FullDllName.Buffer, yin_text) == 0) //相同说明就是要隐藏这个模块
	//	{
	//		//断链
	//		next->Blink->Flink = next->Flink;
	//		next->Flink->Blink = next->Blink;

	//		PLIST_ENTRY order_1 = (PLIST_ENTRY)((DWORD)next - 0x8);
	//		order_1->Blink->Flink = order_1->Flink;

	//		/*PLIST_ENTRY order_2 = (PLIST_ENTRY)((DWORD)next - 0x8 + 0x10);
	//		order_2->Blink->Flink = order_2->Flink;*/

	//		MessageBox(NULL, yin_text, L"已隐藏模块", 0);
	//		break;
	//	}
	//	next = next->Flink;//下一个节点
	//}

}
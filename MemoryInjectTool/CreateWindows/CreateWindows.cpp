//CreateWindows.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "CreateWindows.h"


#define IDC_STATIC1 0x1
#define IDC_STATIC2 0x2
#define IDC_EDIT1 0x3
#define IDC_EDIT2 0x4
#define IDC_BUTTM1 0x5
#define IDC_BUTTM2 0x6

// 此代码模块中包含的函数的前向声明: 

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID Login(HWND hwnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	TCHAR className[] = TEXT("MyFirstWnd");
	//1 创建窗口类型模板
	WNDCLASS wndclass = { 0 };
	wndclass.lpszClassName = className; //名字 
	wndclass.hbrBackground = (HBRUSH)COLOR_BACKGROUND; //背景画笔句柄
	wndclass.hInstance = hInstance; //窗口过程的实例句柄
	wndclass.lpfnWndProc = WndProc; //窗口过程函数

	//2 注册窗口
	RegisterClass(&wndclass);

	//3 创建窗口
	HWND hwindows = CreateWindow(className, TEXT("MYWINDOWS"), WS_OVERLAPPEDWINDOW, 500, 300, 300, 250, NULL, NULL, hInstance,NULL);
   

	//创建窗口

	HWND hsitc1 = CreateWindow(L"STATIC", TEXT("账号："), WS_CHILD | WS_VISIBLE, 10, 30, 50, 20, hwindows, HMENU(IDC_STATIC1), hInstance, NULL);
	HWND hstic2 = CreateWindow(L"STATIC", TEXT("密码"), WS_CHILD | WS_VISIBLE, 10, 80, 50, 20, hwindows, HMENU(IDC_STATIC2), hInstance, NULL);
	HWND hedit1 = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE, 80, 30, 100, 20, hwindows, HMENU(IDC_EDIT1), hInstance, NULL);
	HWND hedit2 = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE, 80, 80, 100, 20, hwindows, HMENU(IDC_EDIT2), hInstance, NULL);
	HWND hbutt1 = CreateWindow(L"BUTTON", TEXT("登录"), WS_CHILD | WS_VISIBLE, 50, 120, 50, 25, hwindows, HMENU(IDC_BUTTM1), hInstance, NULL);
	HWND hbutt2 = CreateWindow(L"BUTTON", TEXT("取消"), WS_CHILD | WS_VISIBLE, 110, 120, 50, 25, hwindows, HMENU(IDC_BUTTM2), hInstance, NULL);

	//4 显示窗口
	ShowWindow(hwindows,SW_SHOW);

	//5 构建消息循环
	MSG msg = { NULL };
	while (GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDC_BUTTM1:
				//MessageBox(NULL, L"登录", NULL, NULL);
				Login(hWnd);
				break;
			case IDC_BUTTM2:
				CloseWindow(hWnd);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
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


VOID Login(HWND hwnd)
{
	WCHAR strName[100] = { 0 };
	WCHAR PassWord[100] = { 0 };
	GetDlgItemText(hwnd, IDC_EDIT1, strName, 100);
	GetDlgItemText(hwnd, IDC_EDIT2, PassWord, 100);

	if (!wcscmp(strName, L"123") && !wcscmp(PassWord, L"qwe"))
	{
		MessageBox(NULL, L"登录成功！", strName, MB_OK);
	}
	else {
		MessageBox(NULL, L"登录失败！", L"提示", MB_OK);
	}
}
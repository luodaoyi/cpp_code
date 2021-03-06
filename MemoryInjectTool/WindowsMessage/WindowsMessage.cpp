//WindowsMessage.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WindowsMessage.h"


LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

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
	HWND hwindows = CreateWindow(className, TEXT("MYWINDOWS"), WS_OVERLAPPEDWINDOW, 500, 300, 300, 250, NULL, NULL, hInstance, NULL);

	//4 显示窗口
	ShowWindow(hwindows, SW_SHOW);

	//5 构建消息循环
	MSG msg = { NULL };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		//调用消息转换 否则有的消息无法捕捉
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
//  WM_RBUTTONDOWN - 按下右键
//  WM_LBUTTONDOWN - 按下左键
//  WM_RBUTTONUP - 弹起右键
//  WM_LBUTTONUP - 弹起左键
//
//  WM_KEYDOWN - 按键消息
//  WM_CHAR - 按键消息 直接判断anscii码
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
            case IDM_ABOUT:
               
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
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

	case WM_KEYDOWN:
		//要想捕捉到此消息，必须在创建窗口时的第5步构建消息循环的时候 调用TranslateMessage
		//WM_KEYDOWN 的msdn文档中有强调
		if (wParam == 0x42)
		{
			MessageBox(hWnd, TEXT("b"), NULL, MB_OK);
		}
		break;
	case WM_CHAR:
		//此消息也需要 TranslateMessage
		if (wParam == 'a')
		{
			MessageBox(hWnd, TEXT("a"), NULL, MB_OK);
		}
		break;

	case WM_RBUTTONDOWN :
		//MessageBox(hWnd, TEXT("按下右键"), NULL, MB_OK);
		break;
	case WM_LBUTTONDOWN:
		//MessageBox(hWnd, TEXT("按下左键"), NULL, MB_OK);
		SendMessage(hWnd, WM_RBUTTONUP, 0, 0);
		break;
	case WM_RBUTTONUP:
		MessageBox(hWnd, TEXT("弹起右键"), NULL, MB_OK);
		break;

	case WM_LBUTTONUP:
		//MessageBox(hWnd, TEXT("弹起左键"), NULL, MB_OK);
		break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

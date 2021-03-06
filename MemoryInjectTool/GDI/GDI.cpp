//GDI.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GDI.h"
#include <Windowsx.h>

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

#define IDC_BUTTM1 0x1
#define IDC_BUTTM2 0x2
#define IDC_BUTTM3 0x3
#define IDC_BUTTM4 0x4
#define IDC_STATIC1 0x5



int left = 0;
int top = 0;
int right = 0;
int bottom = 0;

int type = 1;
HWND static_text;
HWND hwindows;

VOID Draw();

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
	hwindows = CreateWindow(className, TEXT("MYWINDOWS"), WS_OVERLAPPEDWINDOW, 10, 10, 1024, 768, NULL, NULL, hInstance, NULL);

	static_text = CreateWindow(L"STATIC", TEXT("矩形"), WS_CHILD | WS_VISIBLE, 0, 0, 100, 20, hwindows, HMENU(IDC_STATIC1), hInstance, NULL);
	CreateWindow(L"BUTTON", TEXT("矩形"), WS_CHILD | WS_VISIBLE, 110, 0, 100, 25, hwindows, HMENU(IDC_BUTTM1), hInstance, NULL);
	CreateWindow(L"BUTTON", TEXT("圆形"), WS_CHILD | WS_VISIBLE, 210, 0, 100, 25, hwindows, HMENU(IDC_BUTTM2), hInstance, NULL);
	CreateWindow(L"BUTTON", TEXT("圆角矩形"), WS_CHILD | WS_VISIBLE, 310, 0, 100, 25, hwindows, HMENU(IDC_BUTTM3), hInstance, NULL);
	CreateWindow(L"BUTTON", TEXT("一条线"), WS_CHILD | WS_VISIBLE, 410, 0, 100, 25, hwindows, HMENU(IDC_BUTTM4), hInstance, NULL);


	//4 显示窗口
	ShowWindow(hwindows, SW_SHOW);

	/*
	//hwindows 就是设备对象

	//获得设备上下文
	HDC hdc = GetDC(hwindows); //如果举办参数为NULL，那么会得到整个桌面的DC

	//创建画笔
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB(0, 255, 0));
	//将图形对象和上下文关联
	SelectObject(hdc, hPen);

	//创建画刷
	HBRUSH hBrush =  CreateSolidBrush(RGB(255, 0, 0));
	SelectObject(hdc, hBrush);

	//画一条线
	//MoveToEx(hdc, 10, 0, NULL);
	//LineTo(hdc, 30, 40);

	//画一个矩形
	Rectangle(hdc, 10, 20, 50, 80);
	Ellipse(hdc, 10, 20, 50, 80);
	RoundRect(hdc, 10, 20, 50, 80,10,10);

	// 清理
	//清理图形对象
	DeleteObject(hPen);
	DeleteObject(hBrush);

	//释放上下文
	ReleaseDC(hwindows, hdc);
	*/


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
		case IDC_BUTTM1:
			SetWindowText(static_text, L"矩形");
			type = 1;
			break;
		case IDC_BUTTM2:
			SetWindowText(static_text, L"圆形");
			type = 2;
			break;
		case IDC_BUTTM3:
			SetWindowText(static_text, L"圆角矩形");
			type = 3;
			break;
		case IDC_BUTTM4:
			SetWindowText(static_text, L"一条线");
			type = 4;
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
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		//记录按下坐标
		left = GET_X_LPARAM(lParam);
		top = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONUP:
		//记录放开鼠标的坐标
		right = GET_X_LPARAM(lParam);
		bottom = GET_Y_LPARAM(lParam);
		Draw();
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


VOID Draw()
{
	//获得设备上下文
	HDC hdc = GetDC(hwindows); //如果举办参数为NULL，那么会得到整个桌面的DC

							   //创建画笔
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB(0, 255, 0));
	//将图形对象和上下文关联
	SelectObject(hdc, hPen);

	//创建画刷
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	SelectObject(hdc, hBrush);

	switch (type)
	{
	case 1: //矩形
		Rectangle(hdc, left, top, right, bottom);
		break;
	case 2: //圆形
		Ellipse(hdc, left, top, right, bottom);
		break;
	case 3: //圆角矩形
		RoundRect(hdc, left, top, right, bottom,10,10);
		break;
	case 4: //一条线
		MoveToEx(hdc, left, top, NULL);
		LineTo(hdc, right, bottom);
		break;
	default:
		break;
	}

	//清理图形对象
	DeleteObject(hPen);
	DeleteObject(hBrush);

	//释放上下文
	ReleaseDC(hwindows, hdc);
}

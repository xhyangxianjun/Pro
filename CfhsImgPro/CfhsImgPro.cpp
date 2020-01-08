// testTuopan.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include <windows.h>
#include <iostream>
#include "resource.h"
#include "Server.h"
#include "thread"
using namespace std;


#define WMU_NOTIFY WM_USER+1

TCHAR szAppName[] = TEXT("CFHS");

HWND consoleWindowHandle;

const int mainWindowWidth = 640;
const int mainWindowHeight = 270;

void AutoPowerOn();

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	NOTIFYICONDATA  nid;
	static HMENU    hMenu;
	POINT           point;
	HINSTANCE       hInstance;
	consoleWindowHandle = GetConsoleWindow();

	switch (message)
	{
	case WM_CREATE:
		////用这个函数能够获得指定窗口的信息,得到应用程序运行实例的句柄
		hInstance = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
		hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
		hMenu = GetSubMenu(hMenu, 0);
		nid.cbSize = sizeof(nid);
		nid.hWnd = hwnd;
		nid.uID = IDI_ICON1;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WMU_NOTIFY;
		//nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);  
		nid.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1));
		lstrcpy(nid.szTip, szAppName);
		Shell_NotifyIcon(NIM_ADD, &nid);
		return 0;

	case	WM_LBUTTONDOWN:
		ShowWindow(consoleWindowHandle, SW_SHOW);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TUOPAN_EXIT:
			nid.uID = IDI_ICON1;
			nid.hWnd = hwnd;
			Shell_NotifyIcon(NIM_DELETE, &nid);
			PostQuitMessage(0);
			return 0;
		case ID_TUOPAN_HIDE:
			ShowWindow(consoleWindowHandle, SW_HIDE);
			return 0;
		}
		return 0;
	case WMU_NOTIFY:
		if (lParam == WM_RBUTTONDOWN)//托盘消息中lParam接受鼠标的行为
		{
			GetCursorPos(&point);
			SetForegroundWindow(hwnd);
			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
		}
		else if (lParam == WM_LBUTTONDOWN)
		{
			ShowWindow(consoleWindowHandle, SW_SHOW);
		}
		return 0;
	case WM_DESTROY:
		nid.uID = IDI_ICON1;
		nid.hWnd = hwnd;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void mythread()
{
	Server m_Server;
	m_Server.Start();
	return;
}

int main()
{
	HINSTANCE hInstance;
	int iCmdShow;
	LPTSTR szCmdLine;
	hInstance = GetModuleHandle(NULL);  //获取程序本身的实例句柄    
	iCmdShow = SW_SHOWNORMAL;//定义窗口显示模式    
							 //szCmdLine = GetCommandLine();//获取命令行字符串  

	HWND	hwnd;
	MSG		msg;
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MainWindow";

	RegisterClass(&wc);

	hwnd = CreateWindow(wc.lpszClassName, "GoodbyeDPI",
		WS_DISABLED	,
		100, 100, mainWindowWidth, mainWindowHeight, 0, 0, GetModuleHandle(0), 0);


	//HWND hwnd1 = GetConsoleWindow();
	//HMENU hmenu1 = GetSystemMenu(hwnd1, false);
	//RemoveMenu(hmenu1, SC_CLOSE, MF_BYCOMMAND);
	//LONG style = GetWindowLong(hwnd1, GWL_STYLE);
	//style &= ~(WS_MINIMIZEBOX);
	//SetWindowLong(hwnd1, GWL_STYLE, style);
	//SetWindowPos(hwnd1, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//ShowWindow(hwnd1, SW_SHOWNORMAL);
	//DestroyMenu(hmenu1);
	//ReleaseDC(hwnd1, NULL);

	AutoPowerOn();

	std::thread mytobj(mythread);	
	mytobj.detach();
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}


//设置当前程序开机启动
void AutoPowerOn()
{
	HKEY hKey;

	//1、找到系统的启动项  
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项  
	{
		//2、得到本程序自身的全路径
		TCHAR strExeFullDir[MAX_PATH];
		GetModuleFileName(NULL, strExeFullDir, MAX_PATH);

		//3、判断注册表项是否已经存在
		TCHAR strDir[MAX_PATH] = {};
		DWORD nLength = MAX_PATH;
		long result = RegGetValue(hKey, nullptr, _T("CfhsImgPro"), RRF_RT_REG_SZ, 0, strDir, &nLength);

		//4、已经存在
		if (result != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0)
		{
			//5、添加一个子Key,并设置值，"GISRestart"是应用程序名字（不加后缀.exe） 
			RegSetValueEx(hKey, _T("CfhsImgPro"), 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));

			//6、关闭注册表
			RegCloseKey(hKey);
		}
	}
}
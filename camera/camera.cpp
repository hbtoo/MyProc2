// camera.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "stdio.h"

#define MAX_LOADSTRING 1000

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

HWND hWnd;
HWND target = NULL;
TCHAR str[MAX_LOADSTRING] = "initial";

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	MSLLHOOKSTRUCT* mouseInfo = (MSLLHOOKSTRUCT*) lParam;
	RECT rt;
	GetClientRect(hWnd, &rt);

	strcpy(szHello, "haha");

	switch (message) 
	{
		case WM_LBUTTONDOWN:
			int m, n;
			m = LOWORD(lParam);
			n = HIWORD(lParam);

			RECT targetRt;
			GetClientRect(target, &targetRt);

			if (target == NULL) {
				strcpy(str, "Yi Home app not found");
			}
			sprintf(szHello, "  me: %d %d %d %d  \n  click: %d %d    \n   target: %d %d %d %d  \n %s",
				rt.left, rt.right, rt.top, rt.bottom, 
				LOWORD(lParam), HIWORD(lParam),
				targetRt.left, targetRt.right, targetRt.top, targetRt.bottom, str);

			hdc = ::GetDC(hWnd);
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			ReleaseDC(hWnd, hdc);

			// Yi Home app minimum size = 1038, 714
			m = targetRt.right - 140;
			n = targetRt.bottom - 30;
			PostMessage ( target, WM_LBUTTONDOWN, 0, MAKELPARAM(m, n) );
			PostMessage ( target, WM_LBUTTONUP, 0, MAKELPARAM(m, n) );

			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam) {
	if( GetWindowTextLength( hWnd ) == 0 )
		return TRUE;

	TCHAR buffer[MAX_LOADSTRING];
	GetWindowText( hWnd, buffer, MAX_LOADSTRING);
	if (strcmp(buffer, "YI Home Camera") == 0) {
		target = hWnd;
	}
	if (strlen(str) < MAX_LOADSTRING - 100) {
	  strcat(str, buffer);
	  strcat(str, "\n");
	}
	return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CAMERA, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CAMERA);

	/*
	// Botong: ctrl + T to make the front-most window the targetted window
	while (true) {
		if ( KeyIsPressed ( VK_CONTROL ) && KeyIsPressed ( 'T' ) ) {
			target = GetForegroundWindow ();
			break;
		}
		Sleep(50);
	}
	DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
	*/

	// Botong: search for the YI home target window
	EnumWindows(enumWindowsProc, 0);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



// Not used for now
bool KeyIsPressed ( unsigned char k ) {
	USHORT status = GetAsyncKeyState ( k );
	return (( ( status & 0x8000 ) >> 15 ) == 1) || (( status & 1 ) == 1);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CAMERA);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_CAMERA;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

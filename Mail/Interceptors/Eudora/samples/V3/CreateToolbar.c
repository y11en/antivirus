/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: CreateToolbar.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include <commctrl.h>

// Project Headers
#include "CreateToolbar.h"
#include "transBMP.h"
#include "ReadMailToolbarWndProc.h"
#include "SendMailToolbarWndProc.h"
#include "PGPDefinedMessages.h"
#include "resource.h"


#define TRANSPARENT_COLOR RGB(255, 0, 0)

extern HINSTANCE g_hinst;

HWND CreateToolbarRead(HWND hwndParent)
{
	TBBUTTON tbread[]= {
				{0,IDC_DECRYPT,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
				{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
				{2,IDC_KEYMGR,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
				{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0}};

	static HBITMAP hTransparentBMP	= NULL;
	HBITMAP	hReadToolbar	= NULL; 
	HBITMAP	hOldBitmap		= NULL;
	HDC		hdc				= NULL;
	HDC		hdcMem			= NULL;
	HBRUSH	hBrush			= NULL;
	HBRUSH	hOldBrush		= NULL;
	SIZE	size			= {0,0};
	HWND	hwndToolbar		= NULL;

	// all this work just to load a bitmap and make it transparent...
	hReadToolbar = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_READTOOLBAR));

	hdc = CreateIC("DISPLAY",NULL,NULL,NULL);
	hdcMem = CreateCompatibleDC(hdc);
	hBrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	hTransparentBMP =  CreateCompatibleBitmap(hdc,54,18);
	hOldBitmap = SelectObject(hdcMem,hTransparentBMP);
	hOldBrush = SelectObject(hdcMem,hBrush);
	PatBlt(hdcMem,0,0,54,18,PATCOPY);
	TransparentBitmap(hReadToolbar, 54, 18, TRANSPARENT_COLOR, hdcMem, 0,0);
	SelectObject(hdcMem,hOldBitmap);
	SelectObject(hdcMem,hOldBrush);
	DeleteObject(hBrush);
	DeleteObject(hReadToolbar);
	DeleteDC(hdcMem);
	DeleteDC(hdc);

	// now we can create the actual toolbar
	hwndToolbar = CreateToolbarEx(
				hwndParent,
				TBSTYLE_TOOLTIPS|WS_VISIBLE|WS_CHILD|
				WS_CLIPSIBLINGS|CCS_TOP|CCS_NODIVIDER|CCS_NORESIZE,
				IDC_READTOOLBAR,
				3,
				NULL,
				(UINT)hTransparentBMP,
				tbread,
				sizeof(tbread)/sizeof(TBBUTTON),
				18,
				18,
				18,
				18,
				sizeof(TBBUTTON));

	
	// Set the proc address as a property 
	// of the window so it can get it
	SetProp(hwndParent, 
			"oldproc",
			(HANDLE)GetWindowLong( hwndParent, GWL_WNDPROC ) ); 

	// Subclass the window
	SetWindowLong(	hwndParent, 
					GWL_WNDPROC, 
					(DWORD)ReadMailToolbarWndProc );  

	return hwndToolbar;
}

HWND CreateToolbarSend(HWND hwndParent)
{
	TBBUTTON tbread[]= {
				{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
				{4,IDC_KEYMGR,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
				{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
				{3,IDC_MIME,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,0},						
				{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
				{1,IDC_ENCRYPT,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,0},
				{2,IDC_SIGN,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,0},
				{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0}};

	static HBITMAP hTransparentBMP	= NULL;

	HBITMAP	hSendToolbar			= NULL; 
	HBITMAP	hOldBitmap				= NULL;
	HDC		hdc						= NULL;
	HDC		hdcMem					= NULL;
	HBRUSH	hBrush					= NULL;
	HBRUSH	hOldBrush				= NULL;
	HWND	hwndToolbar				= NULL;

	// all this work just to load a bitmap and make it transparent...
	hSendToolbar = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_SENDTOOLBAR));

	hdc = CreateIC("DISPLAY",NULL,NULL,NULL);
	hdcMem = CreateCompatibleDC(hdc);
	hBrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	hTransparentBMP =  CreateCompatibleBitmap(hdc,90,18);
	hOldBitmap = SelectObject(hdcMem,hTransparentBMP);
	hOldBrush = SelectObject(hdcMem,hBrush);
	PatBlt(hdcMem,0,0,90,18,PATCOPY);
	TransparentBitmap(hSendToolbar, 90, 18, TRANSPARENT_COLOR, hdcMem, 0,0);
	SelectObject(hdcMem,hOldBitmap);
	SelectObject(hdcMem,hOldBrush);
	DeleteObject(hBrush);
	DeleteObject(hSendToolbar);
	DeleteDC(hdcMem);
	DeleteDC(hdc);

	// now we can create the actual toolbar
	hwndToolbar = CreateToolbarEx(
				hwndParent,
				TBSTYLE_TOOLTIPS|WS_VISIBLE|WS_CHILD|
				WS_CLIPSIBLINGS|CCS_TOP|CCS_NODIVIDER|CCS_NORESIZE,
				IDC_SENDTOOLBAR,
				5,
				NULL,
				(UINT)hTransparentBMP,
				tbread,
				sizeof(tbread)/sizeof(TBBUTTON),
				18,
				18,
				18,
				18,
				sizeof(TBBUTTON));

	
	// Set the proc address as a property 
	// of the window so it can get it
	SetProp(hwndParent, 
			"oldproc",
			(HANDLE)GetWindowLong( hwndParent, GWL_WNDPROC ) ); 
	
	// Subclass the window
	SetWindowLong(	hwndParent, 
					GWL_WNDPROC, 
					(DWORD)SendMailToolbarWndProc );  

	return hwndToolbar;
}

void CalculateToolbarRect(HWND hwndToolbar, RECT* rect)
{
	int width		= 0;
	int buttoncount = 0;
	RECT tempRect;

	// initialize rect
	rect->left		= 0;
	rect->right		= 0;
	rect->top		= 0;
	rect->bottom	= 0;

	buttoncount = SendMessage(hwndToolbar,TB_BUTTONCOUNT,0,0); 

	// get right most button
	SendMessage(hwndToolbar, 
				TB_GETITEMRECT,
				buttoncount-1,
				(LPARAM) (LPRECT) &tempRect); 

	rect->right = tempRect.right;
	rect->bottom = tempRect.bottom;

	// get left most button
	SendMessage(hwndToolbar, TB_GETITEMRECT,0,(LPARAM) (LPRECT) &tempRect); 

	rect->left = tempRect.left;
	rect->top = tempRect.top;
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

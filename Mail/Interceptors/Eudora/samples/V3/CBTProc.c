/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: CBTProc.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

// Project Headers
#include "strstri.h"
#include "EudoraSendMailWndProc.h"
#include "EudoraReadMailWndProc.h"
#include "EudoraMainWndProc.h"
#include "PGPDefinedMessages.h"
#include "resource.h"

// Global Variables
extern HHOOK		g_hCBTHook;
extern HINSTANCE	g_hinst;
extern HWND			g_hwndEudoraStatusbar;
extern HWND			g_hwndEudoraMainWindow;

// Project Defines
#define EUDORA_SEND_EMAIL 1
#define EUDORA_READ_EMAIL 2

void HANDLE_HCBT_CREATEWND(int nCode, WPARAM wParam, LPARAM lParam);


//=================== CBTProc: Finds Windows ====================

LRESULT WINAPI CBTProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		switch (nCode)
        {
			case HCBT_CREATEWND:
			{
				HANDLE_HCBT_CREATEWND(	nCode,
										wParam,
										lParam);
				break;
			}	
		}			
	}

	return CallNextHookEx (g_hCBTHook, nCode, wParam, lParam);
}

void HANDLE_HCBT_CREATEWND(int nCode, WPARAM wParam, LPARAM lParam)
{
	static	DWORD ThreadId			= 0;
	int		WindowCode				= 0;
	HWND	hwndDetect				= NULL;
	HWND	hwnd					= NULL ;
	HWND	hwndParent				= NULL; 
	int		result					= 0;
	char	szWndClass[1024]		= {0x00};
	char	szTitleText[256]		= {0x00};
	char	szModuleName[MAX_PATH]	= {0x00};

	hwnd	= (HWND) wParam ;
	hwndParent	= (HWND)((LPCREATESTRUCT)
					((CBT_CREATEWND*)lParam)->lpcs)->hwndParent; 

	// Get the class of our window
	result = GetClassName( hwnd, szWndClass, sizeof(szWndClass) );	

	// did we get the classname ??
	if(result)
	{
		// Eudora_Pro and Eudora_Light Child Windows
		if( strstri(szWndClass, "RICHEDIT") ) 
		{
			HWND hwndGrandParent = GetParent(hwndParent);
			HWND hwndGreat_GrandParent = GetParent(hwndGrandParent);
			HWND hwndGreat_Great_GrandParent = 
									GetParent(hwndGreat_GrandParent);

			if(hwndGreat_GrandParent)
			{
				// Get the text in the TitleBar
				result = GetWindowText(	hwndGreat_GrandParent, 
										szTitleText, 
										sizeof(szTitleText) );

				if( result && 
					( strstr(szTitleText, "Eudora Pro") || 
					strstr(szTitleText, "Eudora Light") ) )
				{
					WindowCode = EUDORA_READ_EMAIL;
				}
			} // if hwndGreat_GrandParent

			if(hwndGreat_Great_GrandParent)
			{
				// Get the text in the TitleBar
				result = GetWindowText(	hwndGreat_Great_GrandParent, 
										szTitleText, 
										sizeof(szTitleText) );

				if( result && 
					( strstr(szTitleText, "Eudora Pro") || 
					strstr(szTitleText, "Eudora Light") ) )
				{
					WindowCode = EUDORA_SEND_EMAIL;
				}
			} //if hwndGreat_Great_GrandParent
 

			if( WindowCode == EUDORA_SEND_EMAIL )
			{
				//MessageBox(NULL, "EUDORA_SEND_EMAIL", "Detected", MB_OK);

				// Set the proc address as a property 
				// of the window so it can get it
				SetProp(hwnd, 
						"oldproc",
						(HANDLE)GetWindowLong(hwnd, GWL_WNDPROC));
				
				// Subclass the window
				SetWindowLong(	hwnd, 
								GWL_WNDPROC, 
								(DWORD)EudoraSendMailWndProc );  
			}
			else if( WindowCode == EUDORA_READ_EMAIL )
			{
				//MessageBox(NULL, "EUDORA_READ_EMAIL", "Detected", MB_OK);

				// Set the proc address as a property 
				// of the window so it can get it
				SetProp(hwnd, 
						"oldproc",
						(HANDLE)GetWindowLong(hwnd, GWL_WNDPROC));
				
				// Subclass the window
				SetWindowLong(	hwnd, 
								GWL_WNDPROC, 
								(DWORD)EudoraReadMailWndProc);  
			}

			// Send Main Window ID
			PostMessage(hwnd, WM_PGP_CREATE_INTERFACE, 0,(LPARAM)hwnd); 
			
		} // if RICHEDIT
		// Check to see if Parent Window Menu Created yet
		else if( strstri(szWndClass, "ToolbarWindow32")) 
		{
			static BOOL bParentFound = FALSE;

			//MessageBox(NULL, "Toolbar Window", "Detected", MB_OK);

			if(!bParentFound)
			{
				HMENU hmenu = GetMenu(hwndParent);

				if(hmenu)
				{
					HMENU submenu = NULL;
					HMENU AboutMenu = NULL;

					bParentFound = TRUE;

					AboutMenu = LoadMenu(g_hinst, 
										MAKEINTRESOURCE(IDM_PGPABOUTMENU));

					InsertMenu(	hmenu, 
								7, 
								MF_ENABLED|MF_BYPOSITION|MF_STRING|MF_POPUP, 
								(UINT)AboutMenu, 
								"&PGP");

					// Set the proc address as a property 
					// of the window so it can get it
					SetProp(hwndParent, 
							"oldproc",
							(HANDLE)GetWindowLong(hwndParent, GWL_WNDPROC)); 

					// Subclass the window
					SetWindowLong(	hwndParent, 
									GWL_WNDPROC, 
									(DWORD)EudoraMainWndProc );  

					g_hwndEudoraMainWindow = hwndParent;
				}
			}
		}
		// Check to see if this is the statusbar
		else if(strstri(szWndClass, "msctls_statusbar32")) 
		{
			g_hwndEudoraStatusbar = hwnd;
		}

	} // if result
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

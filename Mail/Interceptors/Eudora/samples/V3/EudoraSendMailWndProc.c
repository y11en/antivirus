/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: EudoraSendMailWndProc.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include <commctrl.h>

// Shared Headers
#include "pgpClientLib.h"
#include "Prefs.h"

// Project Headers
#include "PGPDefinedMessages.h"
#include "CreateToolbar.h"

// Project Defines
#define SENDBUTTON_SHRINKAGE	31
#define SENDTOOLBAR_SEPARATORS	16
#define SENDTOOLBAR_SPACING		8

// Global Variables
extern BOOL g_bEncrypt;
extern BOOL g_bSign;
extern BOOL g_bSendingMail;
extern HWND g_hwndSendToolbar;

// Global Variables
extern PGPContextRef	g_pgpContext;

LRESULT WINAPI EudoraSendMailWndProc(HWND hwnd, 
									 UINT msg, 
									 WPARAM wParam, 
									 LPARAM lParam)
{
	WNDPROC lpOldProc;

	lpOldProc = (WNDPROC)GetProp( hwnd, "oldproc" );
   
	switch(msg)
	{
		case WM_PGP_CREATE_INTERFACE:
		{
			HWND hwndParent		= NULL;
			HWND hwndToolbar	= NULL;
			HWND hwndSendToolbar = NULL;
			HWND hwndSendButton	= NULL;
			int	buttoncount = 0;
			RECT rectEudoraToolbarButton;
			LONG style;

			// Find the windows we are interested in
			hwndParent = GetParent(hwnd);
			hwndParent = GetParent(hwndParent);
			hwndToolbar =  FindWindowEx(hwndParent, 
										NULL, 
										"ToolbarWindow32", 
										NULL);
			hwndSendButton = FindWindowEx(hwndToolbar, NULL, "Button", NULL);

			hwndSendToolbar = CreateToolbarSend(hwndToolbar);

			style = GetWindowLong(hwndToolbar, GWL_STYLE);
			style |= WS_CLIPCHILDREN;
			SetWindowLong(hwndToolbar, GWL_STYLE,style);
			buttoncount = SendMessage(hwndToolbar,TB_BUTTONCOUNT,0,0); 

			SendMessage(hwndToolbar, 
				TB_GETITEMRECT,
				buttoncount-1,
				(LPARAM) &rectEudoraToolbarButton); 

			if( hwndSendToolbar )
			{
				PGPMemoryMgrRef memoryMgr;
				RECT rectSendToolbar;
				BOOL bCheckByDefault = FALSE;
				BOOL bPGPMIME = FALSE;
				int SendToolbarWidth = 0;

				memoryMgr = PGPPeekContextMemoryMgr(g_pgpContext);
				CalculateToolbarRect(hwndSendToolbar, &rectSendToolbar);

				SendToolbarWidth = 
					(rectSendToolbar.right - rectSendToolbar.left);

				MoveWindow(	hwndSendToolbar,
							rectEudoraToolbarButton.right,
							rectEudoraToolbarButton.top - 2,
							SendToolbarWidth,
							rectSendToolbar.bottom - rectSendToolbar.top + 5,
							TRUE);

				// Preset the buttons according to the User Preferences 
				// or globals indicating return from cancel

				if( UsePGPMimeForEncryption(memoryMgr)  || g_bEncrypt 
					|| g_bSign)
				{
					SendMessage(hwndSendToolbar,
						TB_CHECKBUTTON,
						(WPARAM)IDC_MIME, 
						MAKELPARAM(TRUE, 0));
					bPGPMIME = TRUE;
				}

				if( ByDefaultEncrypt(memoryMgr) || g_bEncrypt)
				{
					g_bEncrypt = FALSE;
					SendMessage(hwndSendToolbar,
						TB_CHECKBUTTON,
						(WPARAM)IDC_ENCRYPT, 
						MAKELPARAM(TRUE, 0));
					bCheckByDefault = TRUE;
				}

				if( ByDefaultSign(memoryMgr) || g_bSign)
				{
					g_bSign = FALSE;
					SendMessage(hwndSendToolbar,
						TB_CHECKBUTTON,
						(WPARAM)IDC_SIGN,
						MAKELPARAM(TRUE, 0));
					bCheckByDefault = TRUE;
				}

				if( !bCheckByDefault || bPGPMIME)
				{
					SendMessage(hwndSendToolbar,
						TB_ENABLEBUTTON,
						(WPARAM)IDC_JUSTDOIT,
						(LPARAM) MAKELONG(FALSE, 0));
				}

				// Find New Position of ReadToolbar
				GetWindowRect(hwndSendToolbar, &rectSendToolbar);
				MapWindowPoints(NULL, 
					hwndToolbar, 
					(POINT*)&rectSendToolbar, 
					2);

				if( hwndSendButton )
				{
					RECT rectButton;

					// Resize the Send Button 
					GetWindowRect(hwndSendButton, &rectButton);
					MapWindowPoints(NULL, 
						hwndToolbar, 
						(POINT*)&rectButton,
						2);

					MoveWindow(	hwndSendButton, 
								rectSendToolbar.right, 
								rectButton.top, 
								(rectButton.right - rectButton.left) 
								- SENDBUTTON_SHRINKAGE,
								(rectButton.bottom - rectButton.top),
								TRUE);
				}
			}

			break;
		}

		case WM_DESTROY:   
		{
			//  Put back old window proc and
			SetWindowLong( hwnd, GWL_WNDPROC, (DWORD)lpOldProc );

			//  remove window property
			RemoveProp( hwnd, "oldproc" ); 
			break;
		}
	} 
	
	//  Pass all non-custom messages to old window proc
	return CallWindowProc(lpOldProc, hwnd, msg, wParam, lParam ) ;
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

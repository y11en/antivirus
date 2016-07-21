/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: EudoraReadMailWndProc.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

// Shared Headers
#include "pgpClientLib.h"
#include "Prefs.h"

// Project Headers
#include "PGPDefinedMessages.h"
#include "CreateToolbar.h"


// Project Defines
#define READTOOLBAR_SPACING		8

// Global Variables
extern PGPContextRef	g_pgpContext;

LRESULT WINAPI EudoraReadMailWndProc(HWND hwnd, 
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
			HWND hwndReadToolbar = NULL;

			// Find the windows we are interested in
			hwndParent = GetParent(hwnd);
			hwndToolbar =  FindWindowEx(hwndParent, 
										NULL, 
										"ToolbarWindow32", 
										NULL);

			if( hwndToolbar )
			{
				HWND hwndSubjectText = NULL;
				HWND hwndSubjectEdit = NULL;
				HWND hwndComboBox	 = NULL;
				RECT SubjectTextRect;
				RECT SubjectEditRect;
				RECT ComboBoxRect;
				RECT ReadToolbarRect;
				int ReadToolbarWidth = 0;

				// find the windows we are interested in

				// The first will be the little graphic, 
				// we want the 'Subject:' Text
				hwndSubjectText = FindWindowEx(	hwndToolbar, 
												NULL, 
												"Static", 
												NULL);

				hwndSubjectText = FindWindowEx(	hwndToolbar, 
												hwndSubjectText, 
												"Static", 
												NULL);

				hwndSubjectEdit = FindWindowEx(	hwndToolbar, 
												NULL, 
												"Edit", 
												NULL);

				hwndComboBox	= FindWindowEx(	hwndToolbar, 
												NULL, 
												"ComboBox", 
												NULL);

				if( hwndSubjectText && hwndSubjectEdit && hwndComboBox )
				{
					// create our Decrypting Toolbar for the Read window
					hwndReadToolbar = CreateToolbarRead(hwndToolbar);

					// did we create it correctly
					if( hwndReadToolbar )
					{
						// Find Position of ReadToolbar
						CalculateToolbarRect(hwndReadToolbar, 
											&ReadToolbarRect);

						ReadToolbarWidth = (ReadToolbarRect.right - 
												ReadToolbarRect.left);

						// Find Position of ComboBox
						GetWindowRect(hwndComboBox, &ComboBoxRect);
						MapWindowPoints(NULL, 
										hwndToolbar, 
										(POINT*)&ComboBoxRect, 
										2);

						// Find Position of the TextField 'Subject:'
						GetWindowRect(hwndSubjectText, &SubjectTextRect);
						MapWindowPoints(NULL, 
										hwndToolbar, 
										(POINT*)&SubjectTextRect, 
										2);

						// move the toolbar into the proper position
						// we want it right where the old subject used to be.
						MoveWindow(	hwndReadToolbar, 
									SubjectTextRect.left,  
									ComboBoxRect.top - 2, 
									ReadToolbarWidth,
									(ComboBoxRect.bottom - 
										ComboBoxRect.top) + 5,
									TRUE);

						// Find New Position of ReadToolbar
						GetWindowRect(hwndReadToolbar, &ReadToolbarRect);
						MapWindowPoints(NULL, 
										hwndToolbar, 
										(POINT*)&ReadToolbarRect, 
										2);


						MoveWindow(	hwndSubjectText, 
								ReadToolbarRect.right, 
								SubjectTextRect.top, 
								(SubjectTextRect.right - 
									SubjectTextRect.left),
								(SubjectTextRect.bottom - 
									SubjectTextRect.top),
								TRUE);

						// Find the New Position of the TextField 'Subject:'
						GetWindowRect(hwndSubjectText, &SubjectTextRect);
						MapWindowPoints(NULL, 
										hwndToolbar, 
										(POINT*)&SubjectTextRect, 
										2);

						// Move the Edit Box containing the subject
						GetWindowRect(hwndSubjectEdit, &SubjectEditRect);
						MapWindowPoints(NULL, 
										hwndToolbar, 
										(POINT*)&SubjectEditRect, 
										2);

						MoveWindow(	hwndSubjectEdit, 
									SubjectTextRect.right, 
									SubjectEditRect.top, 
									(SubjectEditRect.right - 
										SubjectEditRect.left) - 
										ReadToolbarWidth,
									(SubjectEditRect.bottom - 
										SubjectEditRect.top),
									TRUE);

						if(AutoDecrypt(PGPPeekContextMemoryMgr(g_pgpContext)))
						{
							SendMessage(hwndToolbar, 
										WM_COMMAND,
										(WPARAM)IDC_DECRYPT,
										0);	
						}
					}
				}

				//MessageBox(NULL, "found hwndToolbar", "ReadMail", MB_OK);
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

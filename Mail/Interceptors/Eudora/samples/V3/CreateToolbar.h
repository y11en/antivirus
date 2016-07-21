/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: CreateToolbar.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_CreateToolbar_h	/* [ */
#define Included_CreateToolbar_h

#define IDC_READTOOLBAR	WM_USER + 1020
#define IDC_SENDTOOLBAR	WM_USER + 1021


HWND CreateToolbarRead(HWND hwndParent);
HWND CreateToolbarSend(HWND hwndParent);
void CalculateToolbarRect(HWND hwndToolbar, RECT* rect);

#endif /* ] Included_CreateToolbar_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: EudoraMainWndProc.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_EudoraMainWndProc_h	/* [ */
#define Included_EudoraMainWndProc_h

LRESULT 
WINAPI 
EudoraMainWndProc(	HWND hwnd, 
					UINT msg, 
					WPARAM wParam, 
					LPARAM lParam);

LRESULT 
WINAPI 
EudoraStatusbarWndProc(	HWND hwnd, 
						UINT msg, 
						WPARAM wParam, 
						LPARAM lParam);

#endif /* ] Included_EudoraMainWndProc_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

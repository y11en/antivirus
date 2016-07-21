/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: DisplayMessage.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

int DisplayMessage(HWND hwnd, long StringId, long TitleId, UINT buttons)
{
	char szMessageBuffer[1024];
	char szTitleBuffer[256];
	HINSTANCE hInst = NULL;

	hInst = GetModuleHandle("PGPplugin.dll");

	LoadString(hInst, StringId, szMessageBuffer, sizeof(szMessageBuffer));
	LoadString(hInst, TitleId, szTitleBuffer, sizeof(szTitleBuffer));

	return MessageBox(hwnd, szMessageBuffer, szTitleBuffer, buttons);
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: DisplayMessage.c 20209 2004-01-25 22:24:43Z wjb $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include "pgpRc.h"

// Project Headers
#include "resource.h"

int DisplayMessage(HWND hwnd, long StringId, long TitleId, UINT buttons)
{
	PGPChar szMessageBuffer[1024];
	PGPChar szTitleBuffer[256];
	HINSTANCE hInst = NULL;

	hInst = GetModuleHandle(PGPTXT_MACHINE("PGPeudora.dll"));

	PGPLoadString(hInst, StringId, szMessageBuffer, sizeof(szMessageBuffer)/sizeof(PGPChar));
	PGPLoadString(hInst, TitleId, szTitleBuffer, sizeof(szTitleBuffer)/sizeof(PGPChar));

	return MessageBox(hwnd, szMessageBuffer, szTitleBuffer, buttons);
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/


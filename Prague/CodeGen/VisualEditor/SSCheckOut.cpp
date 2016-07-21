////////////////////////////////////////////////////////////////////
//
//  PragueServerSaveDB.cpp
//  SourceSafe CheckOut support
//  Prague Interfaces Server
//  Mike Pavlyushchik [mike@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"

BOOL CheckOutFile(LPCTSTR pContainerName)
{

	if (pContainerName && pContainerName[0] && pContainerName[1] == ':') // only full name supported
	{
		DWORD dwAttr = ::GetFileAttributes(pContainerName);
		if ((dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_READONLY))
		{
			TCHAR rcMessage[MAX_PATH*2];
			::wsprintf(rcMessage, _T("The file '%s' is read-only and possible under SourceSafe control, whold you like to checkout this file?"), pContainerName);
			UINT nRes = ::MessageBox( 0, rcMessage, "Visual Editor", MB_YESNOCANCEL );
			if (nRes == IDCANCEL)
				return FALSE;
			if (nRes == IDNO)
				return TRUE;

			TCHAR szParams[MAX_PATH*2];
			TCHAR szSuggestedSSName[MAX_PATH*2];
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			BOOL bStarted = FALSE;
			
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			ZeroMemory( &pi, sizeof(pi) );
			
			::ExpandEnvironmentStrings(_T("%MSDevDir%\\..\\..\\VSS\\win32\\ss.exe"), szSuggestedSSName, sizeof(szSuggestedSSName) / sizeof(szSuggestedSSName[0]));
			_tcscpy(szParams, _T("ss.exe checkout $"));
			_tcscat(szParams, pContainerName+2);
			bStarted = ::CreateProcess(szSuggestedSSName, szParams, 0, 0, FALSE, 0, 0, 0, &si, &pi);
			if (!bStarted)
				bStarted = ::CreateProcess(0, szParams, 0, 0, FALSE, 0, 0, 0, &si, &pi);
			if (bStarted)
			{
				::WaitForSingleObject( pi.hProcess, INFINITE );
				::CloseHandle( pi.hProcess );
				::CloseHandle( pi.hThread );
			}
		}
	}
	return TRUE;
}
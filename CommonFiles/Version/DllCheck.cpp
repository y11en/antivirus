////////////////////////////////////////////////////////////////////
//
//  DLLCHECK.CPP
//  Check DLL version utility function implementation
//  AVP	general application software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "DllCheck.h"
#include "VersInfo.h"


// ---
static void ParseVersionString( const TCHAR *pStr, WORD *pArray ) {
	while ( pStr && _tcsnextc(pStr) != L'\0' ) {
		if ( _tcsnextc(pStr) == L'.' ) 
			pArray++;

		if ( isdigit(_tcsnextc(pStr)) ) 
			*pArray = *pArray * 10 + (_tcsnextc(pStr) - L'0');

		pStr++;
	}
}

// Check version of given DLLs
// pDescr					- pointer to two-dimension string array 
//									Each its line must have format "<Dll name>","<version>"
//									Exmpl : { {"Comctrl32.dll", "4.72.3110.9"}, {"Mfc42.dll","6.00.8447.0"}	}
// nDescrCount		- count of lines "pDescr" has.
// nCmpOperation  -	type of compare operation
//                  -1  - presented version must be less or equal than given
//                   0  - presented version must be equal to given
//                   1  - presented version must be more or equal than given
//
// Returned result -   -1 in case of success
//                     zero based index of "pDescr" line didn't fulfil condition
// ---
int CheckDllVersion( TCHAR *pDescr[][2], int nDescrCount, int nCmpOperation ) {
	for ( int i=0; i<nDescrCount; i++ ) {
		HMODULE hInst = ::GetModuleHandle( pDescr[i][0] );
		if ( hInst ) {
			WORD pVersions[4] = {0, 0, 0, 0};
			ParseVersionString( pDescr[i][1], pVersions );

			CVersionInfo rcInfo( hInst );

			DWORD dwMSVersion = rcInfo.GetFileVersionMS();
			DWORD dwLSVersion = rcInfo.GetFileVersionLS();

			switch ( nCmpOperation ) {
				case -1 :
					if ( HIWORD(dwMSVersion) < pVersions[0] ) 
						break;
					else {
						if ( HIWORD(dwMSVersion) > pVersions[0] ) 
							return i;
					}
					if ( LOWORD(dwMSVersion) < pVersions[1] ) 
						break;
					else {
						if ( LOWORD(dwMSVersion) > pVersions[1] ) 
							return i;
					}
					if ( HIWORD(dwLSVersion) < pVersions[2] ) 
						break;
					else {
						if ( HIWORD(dwLSVersion) > pVersions[2] ) 
							return i;
					}
					if ( LOWORD(dwLSVersion) < pVersions[3] ) 
						break;
					else {
						if ( LOWORD(dwLSVersion) > pVersions[4] ) 
							return i;
					}
					/*
					if ( !(HIWORD(dwMSVersion) <= pVersions[0] &&
								 LOWORD(dwMSVersion) <= pVersions[1] &&	
								 HIWORD(dwLSVersion) <= pVersions[2] &&	
								 LOWORD(dwLSVersion) <= pVersions[3]) ) 
						return i;
					*/
					break;
				case 0 :
					if ( !(HIWORD(dwMSVersion) == pVersions[0] &&
								 LOWORD(dwMSVersion) == pVersions[1] &&	
								 HIWORD(dwLSVersion) == pVersions[2] &&	
								 LOWORD(dwLSVersion) == pVersions[3]) ) 
						return i;
					break;
				case 1 :
					if ( HIWORD(dwMSVersion) > pVersions[0] ) 
						break;
					else {
						if ( HIWORD(dwMSVersion) < pVersions[0] ) 
							return i;
					}
					if ( LOWORD(dwMSVersion) > pVersions[1] ) 
						break;
					else {
						if ( LOWORD(dwMSVersion) < pVersions[1] ) 
							return i;
					}
					if ( HIWORD(dwLSVersion) > pVersions[2] ) 
						break;
					else {
						if ( HIWORD(dwLSVersion) < pVersions[2] ) 
							return i;
					}
					if ( LOWORD(dwLSVersion) > pVersions[3] ) 
						break;
					else {
						if ( LOWORD(dwLSVersion) < pVersions[3] ) 
							return i;
					}
/*
					if ( !(HIWORD(dwMSVersion) >= pVersions[0] ||
								 LOWORD(dwMSVersion) >= pVersions[1] ||	
								 HIWORD(dwLSVersion) >= pVersions[2] ||	
								 LOWORD(dwLSVersion) >= pVersions[3]) ) 
						return i;
*/
					break;
			}
		}
	}
	return -1;
}

// ---
#ifdef _DEBUG
static TCHAR *gszUsedDlls[][2] = {
	{ _T("Mfc42d.dll")		, _T("6.0.8267.0") },
	{ _T("Msvcrtd.dll")		, _T("6.0.8267.0") },
	{ _T("Comctl32.dll")	, _T("4.70.0000.0") },
	{ _T("Mfco42d.dll")		, _T("6.0.8267.0") },
//{ _T("Atl.dll")				, _T("3.0.8449.0") },
	{ _T("Olepro32.dll")	,	_T("5.0.4275.0") },
//{ _T("Oleaut32.dll")	,	_T("2.40.4275.0") }
	{ _T("Oleaut32.dll")	,	_T("2.20.4118.0") }
};
#else
static TCHAR *gszUsedDlls[][2] = {
	{ _T("Mfc42.dll")			, _T("6.0.8267.0") },
	{ _T("Msvcrt.dll")		, _T("6.0.8337.0") },
	{ _T("Comctl32.dll")	, _T("4.70.0000.0") },
//{ _T("Atl.dll")				, _T("3.0.8449.0") },
	{ _T("Olepro32.dll")	,	_T("5.0.4275.0") },
//{ _T("Oleaut32.dll")	,	_T("2.40.4275.0") }
	{ _T("Oleaut32.dll")	,	_T("2.20.4118.0") }
};
#endif

static int    gnUsedDll = sizeof(gszUsedDlls) / sizeof(gszUsedDlls[0]);
static TCHAR *gpFormatLong  = _T("Incorrect version of module %s.\nThe program needs to have %s at least version %s to function properly.\nPresented version is %d.%d.%d.%d.");
static TCHAR *gpFormatShort = _T("Incorrect version of module %s.\nThe program needs to have %s at least version %s to function properly.");

// ---
BOOL CheckStandardAvpDLLWithMsg( BOOL bMessage ) {
	int nResult = ::CheckDllVersion( gszUsedDlls, gnUsedDll, 1 );
	if ( nResult >= 0 && bMessage ) {
		TCHAR szText[512];

		HMODULE hInst = ::GetModuleHandle( gszUsedDlls[nResult][0] );
		if ( hInst ) {
			CVersionInfo rcInfo( hInst );

			DWORD dwMSVersion = rcInfo.GetFileVersionMS();
			DWORD dwLSVersion = rcInfo.GetFileVersionLS();

			wsprintf( szText, gpFormatLong, gszUsedDlls[nResult][0], gszUsedDlls[nResult][0], gszUsedDlls[nResult][1], HIWORD(dwMSVersion), LOWORD(dwMSVersion), HIWORD(dwLSVersion), LOWORD(dwLSVersion) );
		}
		else
			wsprintf( szText, gpFormatShort, gszUsedDlls[nResult][0], gszUsedDlls[nResult][0], gszUsedDlls[nResult][1] );

		TCHAR szName[_MAX_PATH];
		::GetModuleFileName( NULL, szName, _MAX_PATH );

		LPTSTR lpszTemp = (LPTSTR)szName;
		for ( LPCTSTR lpsz = szName; _tcsnextc(lpsz) != L'\0'; lpsz = _tcsinc(lpsz) ) {
			// Remember last directory/drive separator
			int nNextC = _tcsnextc(lpsz);
			if (nNextC == L'\\' || nNextC == L'/' || nNextC == L':')
				lpszTemp = (LPTSTR)_tcsinc(lpsz);
		}

		::MessageBox( NULL, szText, lpszTemp, MB_OK | MB_ICONSTOP | MB_APPLMODAL );
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////
//
//  OCXReg.cpp
//  OCX registration checking utility
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include <AVPRegID.h>
#include <Stuff\OCXReg.h>
#include <Stuff\COMUtils.h>

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


/*
// ---
struct CRInfoStruct {
	HWND			 hWndOwner;
	TCHAR			 *pszCLSID;
	TCHAR			 *pszFileName;
	BOOL        bLeaveLoaded;
	TCHAR      *pszMessageTitle;
	TCHAR			 *pszNotRegisteredErrorTxt;
	TCHAR			 *pszRegisteringErrorTxt;
	TCHAR			 *pszOpenDlgTitle;
	TCHAR			 *pszOpenDlgFilter;
};
*/

#if !defined(_countof)
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

// ---
static BOOL GetRegStrValue( HKEY hKey, const TCHAR* pszKeyName, const TCHAR* pszValName, TCHAR* pszValue, DWORD dwSize ) {
  HKEY  hOpenedKey;
  DWORD dwType = REG_SZ;
  BOOL  bResult = FALSE;
	TCHAR pBuffer[_MAX_PATH];
  
  _tcscpy( pBuffer, AVP_REGKEY_ROOT );
	if ( _tcsnextc(_tcsninc(pBuffer, _tcsclen(pBuffer) - 1)) != L'\\' )
		_tcscat( pBuffer, _T("\\") );
  _tcscat( pBuffer, pszKeyName );
  if ( ERROR_SUCCESS == ::RegOpenKey(hKey, pBuffer, &hOpenedKey) ) {
    if ( ERROR_SUCCESS == ::RegQueryValueEx(hOpenedKey, pszValName, 0, &dwType, (LPBYTE)pszValue, &dwSize) && 
			   dwType == REG_SZ ) 
      bResult = TRUE;
    else
      *pszValue = 0;
    ::RegCloseKey( hOpenedKey );
  }
  return bResult;
}

//---
static void PathExtract( const TCHAR * pszFileName, TCHAR * pszDirName, size_t uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {
		
    TCHAR fDrive[_MAX_DRIVE];
    TCHAR fDir  [_MAX_DIR];
		TCHAR *pLast = 0;
		int  dLen;
		
		*pszDirName = 0;
		
		// выделить диск и каталог
    _tsplitpath( pszFileName, fDrive, fDir, NULL, NULL);
    dLen = (int)_tcsclen( fDir );
		
    if ( dLen > 1 && 
			(pLast = _tcsninc(fDir, dLen - 1)) && 
			(_tcsnextc(pLast) == L'\\') )
      *pLast = 0;
		
    if ( (size_t)(TCSCLEN(fDrive) + TCSCLEN(fDir)) < uicchBufferLen ) {
      _tcsncpy( pszDirName, fDrive, uicchBufferLen );
      int uichLen = (int)TCSCLEN(pszDirName);
      pszDirName = _tcsninc( pszDirName, _tcsclen(pszDirName)/*uichLen*/ );
      _tcsncpy( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
  }
}

// ---
static BOOL IsFileExists( const TCHAR *pFullPath ) {
  HANDLE          hFinder;
  WIN32_FIND_DATA w32fd;
	BOOL            bExists = FALSE;

  UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

  hFinder = ::FindFirstFile( pFullPath, &w32fd );
  if ( hFinder != INVALID_HANDLE_VALUE ) {
    ::FindClose( hFinder );
		bExists = TRUE;
	}

  ::SetErrorMode( uiOldMode );

	return bExists;
}



// ---
static BOOL RegisterOCX( const TCHAR *pFullPath ) {
	BOOL bResult = FALSE;

	if ( ::IsFileExists(pFullPath) ) {
		HINSTANCE hOCXLib = ::LoadLibraryEx( pFullPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

		if ( hOCXLib ) {
			HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);
			(FARPROC&)lpDllEntryPoint = ::GetProcAddress( hOCXLib, "DllRegisterServer" );

			if ( lpDllEntryPoint )
				bResult = SUCCEEDED(lpDllEntryPoint());
		}
	}
	return bResult;
}


// ---
static BOOL IsPath( const TCHAR *pszFileName ) {
	int n0 = _tcsnextc( _tcsninc(pszFileName, 0) );
	int n1 = _tcsnextc( _tcsninc(pszFileName, 1) );
	int n2 = _tcsnextc( _tcsninc(pszFileName, 2) );
  return n0 == L'\\' ||
          n1 == L':' ||
         (n0 == L'.' && n1 == '.' && n2 == L'\\' );
}

// ---
static UINT CALLBACK HookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) {
	if ( uiMsg == WM_NOTIFY ) {
		OFNOTIFY *pNotify = (OFNOTIFY *)lParam;
		if ( pNotify->hdr.code == CDN_INITDONE ) {
			HWND hWndDlg = ::GetParent( hdlg );
			HWND hWndCenter = ::GetWindow( hWndDlg, GW_OWNER );

			RECT rcDlg;
			::GetWindowRect( hWndDlg, &rcDlg );
			RECT rcArea;
			RECT rcCenter;

			rcArea.left = 0;
			rcArea.right = ::GetSystemMetrics( SM_CXSCREEN );
			rcArea.top = 0;
			rcArea.bottom = ::GetSystemMetrics( SM_CYSCREEN );

			if ( hWndCenter != NULL ) 
			  ::GetWindowRect( hWndCenter, &rcCenter );
			else
				memcpy( &rcCenter, &rcArea, sizeof(RECT) );

			// find dialog's upper left based on rcCenter
			int xLeft = (rcCenter.left + rcCenter.right) / 2 - (rcDlg.right - rcDlg.left) / 2;
			int yTop = (rcCenter.top + rcCenter.bottom) / 2 - (rcDlg.bottom - rcDlg.top) / 2;

			// if the dialog is outside the screen, move it inside
			if ( xLeft < rcArea.left )
				xLeft = rcArea.left;
			else 
				if ( xLeft + (rcDlg.right - rcDlg.left) > rcArea.right )
					xLeft = rcArea.right - (rcDlg.right - rcDlg.left);

			if ( yTop < rcArea.top )
				yTop = rcArea.top;
			else 
				if ( yTop + (rcDlg.bottom - rcDlg.top) > rcArea.bottom )
					yTop = rcArea.bottom - (rcDlg.bottom - rcDlg.top);

			// map screen coordinates to child coordinates
			::SetWindowPos( hWndDlg, NULL, xLeft, yTop, -1, -1,	SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
		}
	}
	return 0;
}


// ---
static BOOL PerformFileOpenDlg( CRInfoStruct *pInfo, TCHAR *pszFileName, int nFileNameSize ) {
	static TCHAR pStartPath[_MAX_PATH] = {0};
	if ( !*pStartPath )
		::GetRegStrValue( HKEY_LOCAL_MACHINE, AVP_REGKEY_SHAREDFILES, AVP_REGVALUE_FOLDER, pStartPath, _countof(pStartPath) );

	OPENFILENAME ofn;
	TCHAR pszTempFileName[_MAX_PATH];

	_tcscpy( pszTempFileName, pszFileName );

	::ZeroMemory( &ofn, sizeof(OPENFILENAME) );

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = pInfo->hWndOwner;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = pszFileName;
	ofn.nMaxFile          = nFileNameSize;
	ofn.lpstrTitle        = pInfo->pszOpenDlgTitle;

	ofn.lpstrDefExt       = _T("ocx");
	ofn.Flags 						= OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLEHOOK;
	ofn.lpfnHook					= (LPOFNHOOKPROC)HookProc;

	TCHAR pFullPath[_MAX_PATH];
	if ( !::IsPath(pszFileName) ) {
		_tcscpy( pFullPath, pStartPath );
		if ( _tcsnextc(_tcsninc(pFullPath, _tcsclen(pFullPath) - 1)) != L'\\' )
			_tcscat( pFullPath, _T("\\") );
		_tcscat( pFullPath, pszFileName );

		_tcsncpy( pszFileName, pFullPath, nFileNameSize );
	}

	TCHAR pFilter[_MAX_PATH];
	_tcsncpy( pFilter, pInfo->pszOpenDlgFilter, _countof(pFilter) );
	LPTSTR pch = pFilter; 

	while ((pch = _tcschr(pch, L'|')) != NULL) {
		*pch = L'\0';
		pch = _tcsinc( pch);
	}

	ofn.lpstrFilter = pFilter;

	// WINBUG: This is a special case for the file open/save dialog,
	//  which sometimes pumps while it is coming up but before it has
	//  disabled the main window.
	HWND hWndFocus = ::GetFocus();
	BOOL bEnableParent = FALSE;
	if (ofn.hwndOwner != NULL && ::IsWindowEnabled(ofn.hwndOwner) ) {
		bEnableParent = TRUE;
		::EnableWindow(ofn.hwndOwner, FALSE);
	}

	BOOL bResult = ::GetOpenFileName( &ofn );

	if ( bResult )
		::PathExtract( pszFileName, pStartPath, _countof(pStartPath) );

	// WINBUG: Second part of special case for file open/save dialog.
	if ( bEnableParent )
		::EnableWindow( ofn.hwndOwner, TRUE );
	if ( ::IsWindow(hWndFocus) )
		::SetFocus( hWndFocus );

	return bResult;
}

// ---
static BOOL FindAndRegisterCOMObject( const TCHAR *pszObjectName ) {
	static TCHAR pSharedPath[_MAX_PATH] = {0};
	static TCHAR pStartPath[_MAX_PATH] = {0};
	BOOL bResult = FALSE;

	if ( !*pSharedPath )
		::GetRegStrValue( HKEY_LOCAL_MACHINE, AVP_REGKEY_SHAREDFILES, AVP_REGVALUE_FOLDER, pSharedPath, _countof(pSharedPath) );

	if ( !*pStartPath ) {
		TCHAR szExeName[_MAX_PATH];
		::GetModuleFileName( NULL, szExeName, sizeof(szExeName) );
		::PathExtract( szExeName, pStartPath, sizeof(pStartPath) );
	}

	TCHAR pFullPath[_MAX_PATH];
	if ( !::IsPath(pszObjectName) ) {
		_tcscpy( pFullPath, pSharedPath );
		if ( _tcsnextc(_tcsninc(pFullPath, _tcsclen(pFullPath) - 1)) != L'\\' )
			_tcscat( pFullPath, _T("\\") );
		_tcscat( pFullPath, pszObjectName );
		bResult = ::RegisterOCX( pFullPath );
	}

	if ( !bResult ) {
		if ( !::IsPath(pszObjectName) ) {
			_tcscpy( pFullPath, pStartPath);
			if ( _tcsnextc(_tcsninc(pFullPath, _tcsclen(pFullPath) - 1)) != L'\\' )
				_tcscat( pFullPath, _T("\\") );
			_tcscat( pFullPath, pszObjectName );
			bResult = ::RegisterOCX( pFullPath );
		}
	}

	return bResult;
}


// ---
static BOOL CheckRegisteredIDispatch( CRInfoStruct *pInfo, GUID &rcGUID, IDispatch **pDispatch ) {
	BOOL bRegistered = FALSE;
	if ( pInfo->dwStructSize == (sizeof(CRInfoStruct) - sizeof(BOOL)) ||
			 pInfo->bParanoidCheck ) {
		bRegistered = !!(*pDispatch = (IDispatch *)::COMLoadCOMObjectID( rcGUID, IID_IDispatch ));
	}
	else {
		OLECHAR *pWClsidStr;
		bRegistered = ::ProgIDFromCLSID( rcGUID, &pWClsidStr ) == S_OK;
		::CoTaskMemFree( pWClsidStr );
	}
	return bRegistered;
}

// ---
BOOL CheckAndRegisterRequiredOCX( CRInfoStruct *pInfo ) {
	// The size of the first version of the struct CRInfoStruct is 40 bytes

	BOOL bResult = TRUE;

	if ( SUCCEEDED(OleInitialize(NULL)) ) {
		UINT nPreviousMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );

		IDispatch *pDispatch = NULL;
		GUID       rcGUID;

		if ( SUCCEEDED(::GUIDFromString(pInfo->pszCLSID, rcGUID)) ) {
			BOOL bRegistered = ::CheckRegisteredIDispatch( pInfo, rcGUID, &pDispatch );
			TCHAR pFileName[_MAX_PATH];
			_tcscpy( pFileName, pInfo->pszFileName );
			if ( !bRegistered ) {
				if ( ::FindAndRegisterCOMObject(pFileName) ) 
					bRegistered = ::CheckRegisteredIDispatch( pInfo, rcGUID, &pDispatch );
				while ( !bRegistered && bResult ) {
					const TCHAR *pErrorPattern = pInfo->pszNotRegisteredErrorTxt;
					const TCHAR *pErrorFile = pInfo->pszFileName;
					TCHAR  pErrorText[_MAX_PATH];
					while ( 1 ) {
						wsprintf( pErrorText, pErrorPattern, pErrorFile );
						if ( ::MessageBox( pInfo->hWndOwner, pErrorText, pInfo->pszMessageTitle, MB_YESNO | MB_ICONERROR) == IDYES ) {
							if ( !::PerformFileOpenDlg(pInfo, pFileName, _countof(pFileName)) ) {
								bResult = FALSE;
								break;
							}
							if ( !::RegisterOCX(pFileName) ) {
								pErrorPattern = pInfo->pszRegistrationErrorTxt;
								pErrorFile = pFileName;
								continue;
							}
							break;
						}
						else {
							bResult = FALSE;
							break;
						}
					}
					if ( bResult )
						bRegistered = ::CheckRegisteredIDispatch( pInfo, rcGUID, &pDispatch );
				}
			}

			if ( pDispatch ) {
				pDispatch->Release();
				if ( !pInfo->bLeaveLoaded )
					::CoFreeUnusedLibraries();
			}
		}

		::SetErrorMode( nPreviousMode );

		::OleUninitialize();
	}

	return bResult;
}


// ---
static BOOL CheckRegisteredIUnknown( CRInfoStruct *pInfo, GUID &rcGUID, IUnknown **pUnknown ) {
	BOOL bRegistered = FALSE;
	if ( pInfo->dwStructSize == (sizeof(CRInfoStruct) - sizeof(BOOL)) ||
			 pInfo->bParanoidCheck ) {
		bRegistered = !!(*pUnknown = (IUnknown *)::COMLoadCOMObjectID( rcGUID, IID_IUnknown ));
	}
	else {
		OLECHAR *pWClsidStr;
		bRegistered = ::ProgIDFromCLSID( rcGUID, &pWClsidStr ) == S_OK;
		::CoTaskMemFree( pWClsidStr );
	}
	return bRegistered;
}

// ---
BOOL CheckAndRegisterRequiredCOM( CRInfoStruct *pInfo ) {
	// The size of the first version of the struct CRInfoStruct is 40 bytes

	BOOL bResult = TRUE;

	if ( SUCCEEDED(CoInitialize(NULL)) ) {
		UINT nPreviousMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );

		IUnknown  *pUnknown = NULL;
		GUID       rcGUID;
		BOOL       bRegistered = FALSE;

		if ( SUCCEEDED(::GUIDFromString(pInfo->pszCLSID, rcGUID)) ) {
			BOOL bRegistered = ::CheckRegisteredIUnknown( pInfo, rcGUID, &pUnknown );

			TCHAR pFileName[_MAX_PATH];
			_tcscpy( pFileName, pInfo->pszFileName );
			if ( !bRegistered ) {
				if ( ::FindAndRegisterCOMObject(pFileName) ) 
					bRegistered = ::CheckRegisteredIUnknown( pInfo, rcGUID, &pUnknown );
				while ( !bRegistered && bResult ) {
					const TCHAR *pErrorPattern = pInfo->pszNotRegisteredErrorTxt;
					const TCHAR *pErrorFile = pInfo->pszFileName;
					TCHAR  pErrorText[_MAX_PATH];
					while ( 1 ) {
						wsprintf( pErrorText, pErrorPattern, pErrorFile );
						if ( ::MessageBox( pInfo->hWndOwner, pErrorText, pInfo->pszMessageTitle, MB_YESNO | MB_ICONERROR) == IDYES ) {
							if ( !::PerformFileOpenDlg(pInfo, pFileName, _countof(pFileName)) ) {
								bResult = FALSE;
								break;
							}
							if ( !::RegisterOCX(pFileName) ) {
								pErrorPattern = pInfo->pszRegistrationErrorTxt;
								pErrorFile = pFileName;
								continue;
							}
							break;
						}
						else {
							bResult = FALSE;
							break;
						}
					}
					if ( bResult )
						bRegistered = ::CheckRegisteredIUnknown( pInfo, rcGUID, &pUnknown );
				}
			}

			if ( pUnknown ) {
				pUnknown->Release();
				if ( !pInfo->bLeaveLoaded )
					::CoFreeUnusedLibraries();
			}
		}

		::SetErrorMode( nPreviousMode );

		::CoUninitialize();
	}

	return bResult;
}

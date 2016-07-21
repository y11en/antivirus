////////////////////////////////////////////////////////////////////
//
//  AutoReg.c
//  AutoRegisterOCX function implementation
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <AVPRegID.h>
#include <Stuff\PArray.h>
#include "AutoReg.h"

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


// ---
static BOOL GetRegStrValue( HKEY hKey, const TCHAR* pszKeyName, const TCHAR* pszValName, TCHAR* pszValue, DWORD dwSize ) {
  HKEY  hOpenedKey;
  DWORD dwType = REG_SZ;
  BOOL  bResult = FALSE;
	TCHAR pBuffer[_MAX_PATH];
  
  _tcscpy( pBuffer, AVP_REGKEY_ROOT );
	if ( *(pBuffer + TCSCLEN(pBuffer) - sizeof(TCHAR)) != L'\\' )
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


// ---
static BOOL RegisterOCX( const TCHAR *pFullPath, BOOL bLeaveLoaded ) {
	BOOL bResult = TRUE;
	HINSTANCE hOCXLib = ::GetModuleHandle( pFullPath );
	BOOL bSelfLoad = !hOCXLib;

	if ( bSelfLoad )
		hOCXLib = ::LoadLibraryEx( pFullPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

	if ( hOCXLib ) {
		HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);
		(FARPROC&)lpDllEntryPoint = ::GetProcAddress( hOCXLib, "DllRegisterServer" );

		if ( lpDllEntryPoint )
			bResult = SUCCEEDED(lpDllEntryPoint());

		if ( !bLeaveLoaded && bSelfLoad )
			::FreeLibrary( hOCXLib );
	}

	return bResult;
}


// ---
static TCHAR *MergePath( const TCHAR *pPath, const TCHAR *pFile ) {
	TCHAR *pFullPath = new TCHAR[_MAX_PATH];
	*pFullPath = 0;

	if ( !pPath )
		::GetRegStrValue( HKEY_LOCAL_MACHINE, AVP_REGKEY_SHAREDFILES, AVP_REGVALUE_FOLDER, pFullPath, _MAX_PATH );
	else 
		_tcscpy( pFullPath, pPath );

	if ( *(pFullPath + TCSCLEN(pFullPath) - sizeof(TCHAR)) != L'\\' )
		_tcscat( pFullPath, _T("\\") );

	if ( pFile )
		_tcscat( pFullPath, pFile );

	return pFullPath;
}

// ---
BOOL AutoRegisterOCX( const TCHAR *pBasePath, const TCHAR *pCLSIDStr, const TCHAR *pOCXFileName, BOOL bLeaveLoaded ) {
	BOOL bResult = FALSE;

	if ( SUCCEEDED(OleInitialize(NULL)) ) {
		UINT nPreviousMode = ::SetErrorMode( SEM_FAILCRITICALERRORS );

		CLSID clsID;
		OLECHAR pWStr[40] = {0};
#ifdef UNICODE
		_tcsncpy( pWStr, pCLSIDStr, sizeof(pWStr) );
#else
		::MultiByteToWideChar( CP_ACP, 0, pCLSIDStr, -1, pWStr, 40 );
#endif
		if ( SUCCEEDED(::CLSIDFromString(pWStr, &clsID)) ) {
			IDispatch *pDispatch = NULL;
			if ( ::CoCreateInstance(clsID, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void **)&pDispatch) == S_OK &&
					 pDispatch ) {
				pDispatch->Release();

				if ( !bLeaveLoaded )
					::CoFreeUnusedLibraries();
				bResult = TRUE;
			}
			else {
				TCHAR *pFullPath = ::MergePath( pBasePath, pOCXFileName );
				if ( pFullPath && *pFullPath ) 
					bResult = ::RegisterOCX( pFullPath, bLeaveLoaded );
				delete [] pFullPath;
			}
		}

		::SetErrorMode( nPreviousMode );

		::OleUninitialize();
	}

	return bResult;
}


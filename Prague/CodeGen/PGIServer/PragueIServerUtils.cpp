////////////////////////////////////////////////////////////////////
//
//  PragueIServerUtils.cpp
//  Some utility code implementation
//  Prague Interfaces Server
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <property/property.h>
#include <Stuff\COMUtils.h>
#include <Stuff\PathStr.h>
#include <StuffIO\Utils.h>
#include <StuffIO\IOUtil.h>

#include "..\AVPPveID.h"

#include "..\PGIServerInterface.h"
#include "PragueIServer.h"


//#define _countof(array) (sizeof(array)/sizeof(array[0]))

// ---
void PrepareFilterIndex( OPENFILENAME &pOfn, const char *pDefExt ) {
  CPathStr rcExt(_T("*."));
  rcExt += pDefExt;
  char *p;
  int   i;
  for ( p = (char *)pOfn.lpstrFilter, i = 0; *p; i++ ) {
    p += lstrlen( p ) + 1;
    if ( strstr( p, rcExt ) ) {
      pOfn.nFilterIndex = i + 1;
      break;
    }
    p += strlen( p ) + 1;
  }
}


// ---
BOOL IsLinkedToFile( HDATA hData ) {
	return !!::DATA_Find_Prop( hData, NULL, VE_PID_IF_IMPORTFILENAME );
}

// ---
BOOL IsLinkedByInterface( HDATA hData ) {
	return !!::DATA_Find_Prop( hData, NULL, VE_PID_IF_LINKEDBYINTERFACE );
}

// ---
BOOL IsImported( HDATA hData ) {
	AVP_bool bImported = FALSE;
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_IMPORTED );
	if ( hProp )
		::PROP_Get_Val( hProp, &bImported, sizeof(bImported) );
	return !!bImported;
}

// ---
BOOL IsAlias( HDATA hData ) {
	return !::DATA_Get_First( hData, NULL );
}

// ---
BOOL GetNodeUID( HDATA hData, CNodeUID &pvUID ) {
	if ( hData ) {
		HPROP hUIDProp = ::DATA_Find_Prop( hData, NULL, VE_PID_UNIQUEID );
		if ( hUIDProp ) {
			::PROP_Get_Val( hUIDProp, &pvUID, sizeof(CNodeUID) );
			return TRUE;
		}
	}
	return FALSE;
}

// ---
BOOL CompareNodesUID( CNodeUID &pvFUID, CNodeUID &pvSUID ) {
	GUID rcFGuid;
	GUID rcSGuid;
	if ( SUCCEEDED(::GUIDFromString(pvFUID, rcFGuid)) && SUCCEEDED(::GUIDFromString(pvSUID, rcSGuid)) )
		return ::IsEqualGUID( rcFGuid, rcSGuid );

	return FALSE;
}

// ---
ItType GetTreeSType( HDATA hData ) {
	AVP_dword dwCType;
	::DATA_Get_Val( hData, NULL, 0, &dwCType, sizeof(dwCType) );
	switch ( dwCType ) {
		case 0 :
			return itPrototype;
		case 1 :
			return itImplementation;
		case 2 :
			return itDataBase;
	}
	return itAny;
}

// ---
AVP_dword IType2WType( int iwType ) {
	switch ( iwType ) {
		case itPrototype :
			return 0;
		case itImplementation :
			return 1;
		case itDataBase :
			return 2;
	}
	return (AVP_dword)-1;
}

// ---
void *SerializeDTree( HDATA hData, DWORD &nSSize ) {
	nSSize = 0;
	if ( hData ) {

		AVP_Serialize_Data rcSData;
		memset( &rcSData, 0, sizeof(rcSData) );

		::DATA_Serialize( hData, NULL, &rcSData, NULL, 0, &nSSize );

		void *pBuffer = (BYTE *)::CoTaskMemAlloc( sizeof(BYTE) * nSSize );
		::DATA_Serialize( hData, NULL, &rcSData, pBuffer, nSSize, &nSSize );

/*
		nSSize = ::KLDT_SerializeToMemoryUsingSWM( NULL, 0, hData, "" );
		void *pBuffer = (BYTE *)::CoTaskMemAlloc( sizeof(BYTE) * nSSize );
		::KLDT_SerializeToMemoryUsingSWM( (char *)pBuffer, nSSize, hData, "" );
*/
		return pBuffer;
	}
	return NULL;
}

// ---
HDATA DeserializeDTree( void *pBuffer, DWORD nSize ) {
	if ( nSize ) {
		HDATA hData = NULL;
		if ( pBuffer ) {
			if ( *(DWORD *)pBuffer == AVP_SW_MAGIC ) 
				::KLDT_DeserializeFromMemoryUsingSWM( (AVP_char *)pBuffer, nSize, &hData );

			if ( !hData && *(WORD *)pBuffer == 0xadad ) {
				AVP_Serialize_Data rcSData;
				memset( &rcSData, 0, sizeof(rcSData) );
				hData = ::DATA_Deserialize( &rcSData, pBuffer, nSize, &nSize );
			}
		}
		return hData;
	}
	return NULL;
}


// ---
AVP_dword CreateDataId( HDATA hParentData ) {
	CSArray<UINT> rcIdArray;
  HDATA hData = ::DATA_Get_First( hParentData, NULL );
	while ( hData )	{
		AVP_dword nDataPID = ::DATA_Get_Id( hData, NULL ); 
		rcIdArray.Add( GET_AVP_PID_ID(nDataPID)	);
    hData = ::DATA_Get_Next( hData, NULL );
	}

	return ::GetMinUnusedValue( rcIdArray );
}

// ---
void SetDataPropWithCheck( HDATA hData, AVP_dword dwPID, LPCTSTR pStartingFolder, LPCTSTR pValue ) {
	CPathStr pBuffer( _MAX_PATH );
	pBuffer = pStartingFolder;
	pBuffer ^= _T("a");

	::IOSRelativePathFromFullPath( pBuffer, pValue, pBuffer, _MAX_PATH );

	HPROP hProp = ::DATA_Find_Prop( hData, NULL, dwPID );
	if ( !hProp ) 
		::DATA_Add_Prop( hData, NULL, dwPID, AVP_dword(LPCTSTR(pBuffer)), 0 );
	else 
		::PROP_Set_Val( hProp, AVP_dword(LPCTSTR(pBuffer)), 0 );
}


// ---
static BOOL IsPath( const char * pszFileName ) {
  return (*pszFileName == '\\' && *(pszFileName + 1) == '\\') ||
          *(pszFileName + 1) == ':' ||
         (*pszFileName == '.' && *(pszFileName + 1) == '.');
}

// ---
UINT CALLBACK HookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) {
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
BOOL PerformFileOpenDlg( HWND hWndOwner, LPCTSTR pszInitialDir, LPCTSTR pszFilter, LPCTSTR pszDefExt, TCHAR *pszFileName, int nFileNameSize ) {
	TCHAR rcFileName[_MAX_PATH] = {0};
	OPENFILENAME ofn;
	::ZeroMemory( &ofn, sizeof(OPENFILENAME) );

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hWndOwner;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = rcFileName;
	ofn.nMaxFile          = sizeof(rcFileName);
	ofn.lpstrDefExt       = pszDefExt;
	ofn.Flags 						= OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLEHOOK;
	ofn.lpfnHook					= HookProc;
	ofn.lpstrInitialDir		= pszInitialDir;

/*
	TCHAR pFullPath[_MAX_PATH];
	if ( !::IsPath(pszFileName) ) {
		lstrcpy( pFullPath, pszInitialDir );
		if ( *(pFullPath + lstrlen(pFullPath) - sizeof(TCHAR)) != L'\\' )
			lstrcat( pFullPath, _T("\\") );
		lstrcat( pFullPath, pszFileName );

//		lstrcpyn( pszFileName, pFullPath, nFileNameSize );
	}
*/

	TCHAR pFilter[_MAX_PATH];
	lstrcpyn( pFilter, pszFilter, _countof(pFilter) );
	LPTSTR pch = pFilter; 

	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

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

//	::PrepareFilterIndex( ofn, pszDefExt );

	BOOL bResult = ::GetOpenFileName( &ofn );
	if ( bResult ) {
		if ( pszFileName ) 
			lstrcpyn( pszFileName, rcFileName, nFileNameSize );
	}
//	DWORD dwResult = ::CommDlgExtendedError();

	// WINBUG: Second part of special case for file open/save dialog.
	if ( bEnableParent )
		::EnableWindow( ofn.hwndOwner, TRUE );
	if ( ::IsWindow(hWndFocus) )
		::SetFocus( hWndFocus );

	return bResult;
}


// ---
BOOL PerformFileSaveDlg( HWND hWndOwner, LPCTSTR pszInitialDir, LPCTSTR pszFilter, LPCTSTR pszDefExt, TCHAR *pszFileName, int nFileNameSize ) {
	OPENFILENAME ofn;
	::ZeroMemory( &ofn, sizeof(OPENFILENAME) );

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hWndOwner;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = pszFileName;
	ofn.nMaxFile          = nFileNameSize;
	ofn.lpstrInitialDir		= pszInitialDir;
	ofn.lpstrDefExt       = pszDefExt;
	ofn.Flags 						= OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_ENABLEHOOK;
	ofn.lpfnHook					= HookProc;
	ofn.lpstrInitialDir		= pszInitialDir;
	
/*
	TCHAR pFullPath[_MAX_PATH];
	if ( !::IsPath(pszFileName) ) {
		lstrcpy( pFullPath, pszInitialDir );
		if ( *(pFullPath + lstrlen(pFullPath) - sizeof(TCHAR)) != L'\\' )
			lstrcat( pFullPath, _T("\\") );
		lstrcat( pFullPath, pszFileName );

		lstrcpyn( pszFileName, pFullPath, nFileNameSize );
	}
*/

	TCHAR pFilter[_MAX_PATH];
	lstrcpyn( pFilter, pszFilter, _countof(pFilter) );
	LPTSTR pch = pFilter; 

	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

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

	::PrepareFilterIndex( ofn, pszDefExt );

	BOOL bResult = ::GetSaveFileName( &ofn );

//	DWORD dwResult = ::CommDlgExtendedError();

	// WINBUG: Second part of special case for file open/save dialog.
	if ( bEnableParent )
		::EnableWindow( ofn.hwndOwner, TRUE );
	if ( ::IsWindow(hWndFocus) )
		::SetFocus( hWndFocus );

	return bResult;
}



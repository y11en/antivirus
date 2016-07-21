#include <windows.h>
#include <tchar.h>
#include "../stuff/pathstr.h"
#include "../stuff/pathstra.h"
#include "../stuff/pathstrw.h"
#include "../stuff/csystem.h"
#include "xerrors.h"
#include "ioutil.h"

static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

IOS_ERROR_CODE IOSFinishCopyingW( const WCHAR * pszFileName, WCHAR * pszBuffer, uint uicchBufferLen );


// ---
DWORD IOSGetLogicalDriveStringsW( DWORD nBufferLength,  LPWSTR lpBuffer ) {
#if defined(_UNICODE)
	return ::GetLogicalDriveStrings( nBufferLength,  lpBuffer );
#else
	if ( g_bUnicodePlatform ) {
		return ::GetLogicalDriveStringsW( nBufferLength,  lpBuffer );
	}
	else {
		CHAR pBufferA[_MAX_PATH];
		DWORD dwResult = ::GetLogicalDriveStringsA( sizeof(pBufferA),  pBufferA );
		CMultiStrA pMultiA( pBufferA, sizeof(pBufferA) );
		CMultiStrW pMultiW;
		for ( int i=0,c=pMultiA.Count(); i<c; i++ ) {
			pMultiW += CPathStrW(pMultiA[i]);
		}
		memcpy( lpBuffer, (WCHAR *)pMultiW, min(nBufferLength, (DWORD)pMultiW.Size()) * sizeof(WCHAR) );
		return dwResult;
	}
#endif
}

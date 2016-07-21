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
DWORD IOSQueryDosDeviceW( LPCWSTR lpDeviceName, LPWSTR lpTargetPath, DWORD ucchMax ) {
#if defined(_UNICODE)
	return ::IOSQueryDosDevice( lpDeviceName, lpTargetPath, ucchMax );
#else
	if ( g_bUnicodePlatform ) {
		return ::QueryDosDeviceW( lpDeviceName, lpTargetPath, ucchMax );
	}
	else {
		CPathStrA lpDeviceNameA( lpDeviceName );
		CHAR pTargetPathA[_MAX_PATH];
		DWORD dwResult = ::QueryDosDeviceA( lpDeviceNameA, pTargetPathA, sizeof(pTargetPathA) );
		CPathStrW pTargetPathW( pTargetPathA );
		::IOSFinishCopyingW( pTargetPathW, lpTargetPath, ucchMax );
		return dwResult;
	}
#endif
}


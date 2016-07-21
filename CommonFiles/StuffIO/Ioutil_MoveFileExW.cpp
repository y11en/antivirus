#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


// ---
IOS_ERROR_CODE IOSMoveFileOnRebootW( LPCWSTR pszSrcName, LPCWSTR pszDestName ) {
#if defined(_UNICODE)
	return ::IOSMoveFileOnReboot( pszSrcName, pszDestName );
#else
	IOS_ERROR_CODE dwLastError = IOS_ERR_OK;
	
	OSVERSIONINFO	rcOS;
	rcOS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx( &rcOS );

	if( rcOS.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
		if ( !::MoveFileExW(pszSrcName, pszDestName, MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING) )
			dwLastError = ::Win32ErrToIOSErr( ::GetLastError() );
	}
	else 
		dwLastError = ::IOSMoveFileOnReboot9x( CPathStrA(pszSrcName), (pszDestName ? CPathStrA(pszDestName) : NULL) );

	return dwLastError;
#endif
}



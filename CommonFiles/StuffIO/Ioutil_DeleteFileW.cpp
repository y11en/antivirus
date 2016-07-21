#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSDeleteFileW( const WCHAR * pszFileName ) {
#if defined(_UNICODE)
	return ::IOSDeleteFile( pszFileName );
#else
  if ( pszFileName ) {
    ::IOSSetLastError( IOS_ERR_OK );
		
		if ( g_bUnicodePlatform ) {
			if ( !::DeleteFileW(pszFileName) ) 
				::Win32ErrToIOSErr( ::GetLastError() );
		}
		else {
			if ( !::DeleteFileA(CPathStrA(pszFileName)) ) 
				::Win32ErrToIOSErr( ::GetLastError() );
		}
		
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}

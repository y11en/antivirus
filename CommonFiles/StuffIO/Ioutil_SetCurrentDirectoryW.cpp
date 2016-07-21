#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSSetCurrentDirectoryW( const WCHAR * pszDirName ) {
#if defined(_UNICODE)
	return ::IOSSetCurrentDirectory( pszDirName );
#else
  if ( pszDirName ) {
    ::IOSSetLastError( IOS_ERR_OK );

    if ( g_bUnicodePlatform ) {
			if ( !::SetCurrentDirectoryW(pszDirName) ) 
				::Win32ErrToIOSErr( ::GetLastError() );
		}
 	  else {
			CPathStrA pConverted( pszDirName );
			if ( !::SetCurrentDirectory(pConverted) ) 
				::Win32ErrToIOSErr( ::GetLastError() );
		}
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}

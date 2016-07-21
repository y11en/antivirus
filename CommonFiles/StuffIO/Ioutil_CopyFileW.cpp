#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuff/csystem.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSCopyFileW( const WCHAR * pszFrom, const WCHAR * pszTo, bool fFailIfExists ) {
#if defined(_UNICODE)
	return ::IOSCopyFile( pszFrom, pszTo, fFailIfExists );
#else
  if ( pszFrom && pszTo ) {
    ::IOSSetLastError( IOS_ERR_OK );
		
		if ( g_bUnicodePlatform ) {
			if ( !::CopyFileW(pszFrom, pszTo, fFailIfExists) ) 
				::Win32ErrToIOSErr( ::GetLastError() );
		}
		else {
			if ( !::CopyFileA(CPathStrA(pszFrom), CPathStrA(pszTo), fFailIfExists) ) 
				::Win32ErrToIOSErr( ::GetLastError() );
		}
		
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}

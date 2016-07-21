#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE IOSCopyFile( const TCHAR * pszFrom, const TCHAR * pszTo, bool fFailIfExists ) {
  if ( pszFrom && pszTo ) {
    ::IOSSetLastError( IOS_ERR_OK );
		
    if ( !::CopyFile(pszFrom, pszTo, fFailIfExists) ) 
      ::Win32ErrToIOSErr( ::GetLastError() );
		
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



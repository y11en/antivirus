#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE IOSDeleteFile( const TCHAR * pszFileName ) {
  if ( pszFileName ) {
    ::IOSSetLastError( IOS_ERR_OK );
		
    if ( !::DeleteFile(pszFileName) ) 
      ::Win32ErrToIOSErr( ::GetLastError() );
		
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE IOSSetCurrentDirectory( const TCHAR * pszDirName ) {
  if ( pszDirName ) {
   ::IOSSetLastError( IOS_ERR_OK );
		
    if ( !::SetCurrentDirectory(pszDirName) ) 
      ::Win32ErrToIOSErr( ::GetLastError() );
		
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



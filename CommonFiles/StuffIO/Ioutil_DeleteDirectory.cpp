#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


//---
IOS_ERROR_CODE IOSDeleteDirectory( const TCHAR * pszDirName ) {
  if ( pszDirName ) {
    ::IOSSetLastError( IOS_ERR_OK );

    if ( !::RemoveDirectory(pszDirName) ) 
      ::Win32ErrToIOSErr( ::GetLastError() );

    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



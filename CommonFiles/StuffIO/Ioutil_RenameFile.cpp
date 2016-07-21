#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE IOSRenameFile( const TCHAR * pszOldFileName, const TCHAR * pszNewFileName ) {
  if ( pszOldFileName && pszNewFileName ) {
    ::IOSSetLastError( IOS_ERR_OK );

    if ( !::MoveFile(pszOldFileName, pszNewFileName) ) 
      ::Win32ErrToIOSErr( ::GetLastError() );

    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



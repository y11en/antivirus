#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE IOSGetCurrentDirectory( TCHAR * pszDirBuf, uint uicchBufSize, ulong *puicchBuffSizeNeeded/* = NULL*/ ) {
  if ( pszDirBuf ) {
    ::IOSSetLastError( IOS_ERR_OK );
		
		DWORD dwSize = 0;
    if ( !(dwSize = ::GetCurrentDirectory(uicchBufSize, pszDirBuf)) ) 
      ::Win32ErrToIOSErr( ::GetLastError() );
		
		if ( puicchBuffSizeNeeded )
			*puicchBuffSizeNeeded = dwSize;

    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_PARAMETERS;
}



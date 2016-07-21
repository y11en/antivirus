#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE IOSGetFileInfo( const TCHAR * pszFileName, PIOSFILEINFO pfi ) {
  if ( pszFileName ) {
    ::IOSSetLastError( IOS_ERR_OK );

    if ( pfi ) {

      HANDLE          hFinder;
      SYSTEMTIME      sysTime;
      WIN32_FIND_DATA w32fd;

      uint uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

      hFinder = ::FindFirstFile( pszFileName, &w32fd );
      if ( hFinder != INVALID_HANDLE_VALUE ) {
        ::FindClose( hFinder );

        pfi->dwcbSize = w32fd.nFileSizeLow;

        ::FileTimeToSystemTime(&(w32fd.ftLastWriteTime), &sysTime);

        pfi->fioTime.uiSecond    = (uint)sysTime.wSecond;
        pfi->fioTime.uiMinute    = (uint)sysTime.wMinute;
        pfi->fioTime.uiHour      = (uint)sysTime.wHour;
        pfi->fioTime.uiDay       = (uint)sysTime.wDay;
        pfi->fioTime.uiDayOfWeek = (uint)sysTime.wDayOfWeek;
        pfi->fioTime.uiMonth     = (uint)sysTime.wMonth;
        pfi->fioTime.uiYear      = (uint)sysTime.wYear;

        ::IOSSetLastError( IOS_ERR_OK );
      }
      else 
        ::Win32ErrToIOSErr( ::GetLastError() );

      ::SetErrorMode( uiOldMode );
    }
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

//---
IOS_ERROR_CODE IOSFileLengthName( const TCHAR * pszFileName, uint32 * pdwcbyFileLength ) {
  if ( pszFileName ) {
    ::IOSSetLastError( IOS_ERR_OK );

    IOSFILEINFO   fi;

    IOS_ERROR_CODE CSErrCode = ::IOSGetFileInfo( pszFileName, &fi );
    if ( CSErrCode != IOS_ERR_OK )
      return CSErrCode;

    if ( pdwcbyFileLength != NULL )
      *pdwcbyFileLength = fi.dwcbSize;

    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}


//---
IOS_ERROR_CODE IOSFileExists( const TCHAR * pszFileName ) {
  if ( pszFileName ) {
    IOSFILEINFO fi;
    return ::IOSGetFileInfo( pszFileName, &fi );
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



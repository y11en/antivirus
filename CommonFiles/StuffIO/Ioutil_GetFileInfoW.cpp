#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuffio/ioutil.h>

static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSGetFileInfoW( const WCHAR * pszFileName, PIOSFILEINFO pfi ) {
#if defined(_UNICODE)
	return IOSGetFileInfo( pszFileName, pfi );
#else
  if ( pszFileName ) {
    ::IOSSetLastError( IOS_ERR_OK );
		
    if ( pfi ) {
			
      HANDLE          hFinder;
      SYSTEMTIME      sysTime;
			
      uint uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );
			
			if ( g_bUnicodePlatform ) {
				WIN32_FIND_DATAW w32fd;
				hFinder = ::FindFirstFileW( pszFileName, &w32fd );
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
			}
			else {
				WIN32_FIND_DATAA w32fd;
				hFinder = ::FindFirstFileA( CPathStrA(pszFileName), &w32fd );
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
			}
			
      ::SetErrorMode( uiOldMode );
    }
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif	
}

//---
IOS_ERROR_CODE IOSFileLengthNameW( const WCHAR * pszFileName, uint32 * pdwcbyFileLength ) {
#if defined(_UNICODE)
	return ::IOSFileLengthName( pszFileName, pdwcbyFileLength );
#else
  if ( pszFileName ) {
    ::IOSSetLastError( IOS_ERR_OK );

    IOSFILEINFO   fi;

    IOS_ERROR_CODE CSErrCode = ::IOSGetFileInfoW( pszFileName, &fi );
    if ( CSErrCode != IOS_ERR_OK )
      return CSErrCode;

    if ( pdwcbyFileLength != NULL )
      *pdwcbyFileLength = fi.dwcbSize;

    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif	
}


//---
IOS_ERROR_CODE IOSFileExistsW( const WCHAR * pszFileName ) {
#if defined(_UNICODE)
	return ::IOSFileExists( pszFileName );
#else
  if ( pszFileName ) {
    IOSFILEINFO fi;
    return ::IOSGetFileInfoW( pszFileName, &fi );
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif	
}



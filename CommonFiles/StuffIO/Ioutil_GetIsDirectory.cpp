#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE IOSGetIsDirectory( const TCHAR * pszFileName ) {
  if ( pszFileName ) {
  
    ::IOSSetLastError( IOS_ERR_OK );
    HANDLE          hFinder;
    WIN32_FIND_DATA w32fd;

    uint uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

    hFinder = ::FindFirstFile( pszFileName, &w32fd );
    if ( hFinder != INVALID_HANDLE_VALUE ) {
      ::FindClose( hFinder );

      if ( !(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
        ::IOSSetLastError( IOS_ERR_FILE_NOT_FOUND );
    }
    else {
			int nLast = _tcsnextc( _tcsninc(pszFileName, _tcsclen(pszFileName) - 1) );
      // Если последний символ '\' или ':', то файл не находится, но является каталогом
      if ( nLast != L'\\' &&  nLast != L':' )
        ::Win32ErrToIOSErr( ::GetLastError() );
		}
    ::SetErrorMode( uiOldMode );
  
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



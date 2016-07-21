#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>

static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSGetIsDirectoryW( const WCHAR * pszFileName ) {
#if defined(_UNICODE)
	return ::IOSGetIsDirectory( pszFileName );
#endif
  if ( pszFileName ) {
		
    ::IOSSetLastError( IOS_ERR_OK );
    HANDLE          hFinder;
		
    uint uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );
		
		if ( g_bUnicodePlatform ) {
			WIN32_FIND_DATAW w32fd;
			hFinder = ::FindFirstFileW( pszFileName, &w32fd );
			if ( hFinder != INVALID_HANDLE_VALUE ) {
				::FindClose( hFinder );
				
				if ( !(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					::IOSSetLastError( IOS_ERR_FILE_NOT_FOUND );
			}
			else {
				int nLast = _wcsnextc( _wcsninc(pszFileName, wcslen(pszFileName) - 1) );
				// Если последний символ '\' или ':', то файл не находится, но является каталогом
				if ( nLast != L'\\' &&  nLast != L':' )
					::Win32ErrToIOSErr( ::GetLastError() );
			}
		}
		else {
			WIN32_FIND_DATAA w32fd;
			CPathStrA rcFileName( pszFileName );
			hFinder = ::FindFirstFileA( rcFileName, &w32fd );
			if ( hFinder != INVALID_HANDLE_VALUE ) {
				::FindClose( hFinder );
				
				if ( !(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					::IOSSetLastError( IOS_ERR_FILE_NOT_FOUND );
			}
			else {
				int nLast = _wcsnextc( _wcsninc(pszFileName, wcslen(pszFileName) - 1) );
				// Если последний символ '\' или ':', то файл не находится, но является каталогом
				if ( nLast != L'\\' &&  nLast != L':' )
					::Win32ErrToIOSErr( ::GetLastError() );
			}
		}

    ::SetErrorMode( uiOldMode );
		
    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

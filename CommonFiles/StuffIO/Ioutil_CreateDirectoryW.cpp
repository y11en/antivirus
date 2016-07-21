#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuff/pathstra.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

WCHAR* DuplicateStringW( const WCHAR* s );
WCHAR* DuplicateFileNameW( const WCHAR* s );

//---
IOS_ERROR_CODE IOSCreateDirectoryW( const WCHAR * pszDirName ) {
#if defined(_UNICODE)
		return ::IOSCreateDirectory( pszDirName );
#else
  if ( pszDirName ) {
    ::IOSSetLastError( IOS_ERR_OK );

    bool ret = true;
  
  // сохранить декущий диск/каталог
    CAPointer<WCHAR> currDir = new WCHAR[_MAX_PATH];
    ::IOSGetCurrentDirectoryW( currDir, _MAX_PATH );

    CAPointer<WCHAR> pszoemDirName = ::DuplicateFileNameW( pszDirName );
  
  /*  
    if ( !::CreateDirectory(pszDirName, NULL) ) 
      ::Win32ErrToIOSErr( ::GetLastError() );
  */
    if ( ::IOSSetCurrentDirectoryW(pszoemDirName) != IOS_ERR_OK ) {
  // если каталог не установился - его нужно создавать
  // пытаемся установить каталоги укорачивая путь
      CAPointer<WCHAR> path = ::DuplicateStringW( pszoemDirName );
      WCHAR *p;
      do {
        p = wcsrchr( path, L'\\' );
        if ( p ) 
          *p = L'\0';
      } while( p && *path != L'\0' && ::IOSSetCurrentDirectoryW(path) != IOS_ERR_OK ); // пока не устанавливается

  // создать цепочку каталогов
      while ( wcslen(path) != wcslen(pszoemDirName) ) {
        *((WCHAR *)path + wcslen(path)) = L'\\';
        
				if ( g_bUnicodePlatform ) {
					if ( !::CreateDirectoryW(path, NULL) ) {
						ret = false;
						break;
					}
				}
				else {
					if ( !::CreateDirectoryA(CPathStrA(path), NULL) ) {
						ret = false;
						break;
					}
				}
      }
    }

    if ( !ret )
      ::Win32ErrToIOSErr( ::GetLastError() );

  // восстановить текущий диск/каталог
    ::IOSSetCurrentDirectoryW( currDir );

    return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}

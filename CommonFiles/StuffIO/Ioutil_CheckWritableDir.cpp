#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


//---
IOS_ERROR_CODE  IOSCheckWritableDir( const TCHAR * pszDirName ) {
  if ( pszDirName ) {
    TCHAR szTestFileName[_MAX_PATH];

    // Here goes the most sweet task: check whether the whole directory can be accessed for write.
    // The most stupid but working method is: try to create file with unique name in this dir
    // Note: here we cannot use GetTempFileName() fn so far it seems to hang in Win3 for shares with readonly attr set

    while ( 1 ) {
      SYSTEMTIME            time;
      HANDLE                hFinder;
      WIN32_FIND_DATA       w32fd;
      TCHAR                  szUniqueFileName[ 20 ];
    
      _tcscpy( szTestFileName, pszDirName );
      if ( _tcsnextc(_tcsninc(szTestFileName, _tcsclen(szTestFileName) - 1)) != L'\\' )
        _tcscat( szTestFileName, _T("\\") );

      ::GetSystemTime( &time );
      wsprintf( szUniqueFileName, _T("%x%x.tmp"), time.wSecond, time.wMilliseconds );
      _tcscat( szTestFileName, szUniqueFileName );

      hFinder = ::FindFirstFile( szTestFileName, &w32fd );
      ::FindClose( hFinder );
      if ( hFinder == INVALID_HANDLE_VALUE )      // No such file => unique filename found
        break;
    }

    IOS_ERROR_CODE dwfError;

    CPointer<XFile> file = new XFile( szTestFileName, FIOAM_WRITE );

    dwfError = ::IOSGetLastError();

    file = 0;

    ::IOSDeleteFile( szTestFileName );

    return dwfError;

  }
  return IOS_ERR_INVALID_FILE_NAME;
}

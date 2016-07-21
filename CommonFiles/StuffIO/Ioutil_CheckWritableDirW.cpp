#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// ---
IOS_ERROR_CODE IOSCheckWritableDirW( const WCHAR * pszDirName ) {
#if defined(_UNICODE)
	return ::IOSCheckWritableDir( pszDirName );
#else
  if ( pszDirName ) {
    if ( g_bUnicodePlatform ) {
			WCHAR szTestFileName[_MAX_PATH];

			// Here goes the most sweet task: check whether the whole directory can be accessed for write.
			// The most stupid but working method is: try to create file with unique name in this dir
			// Note: here we cannot use GetTempFileName() fn so far it seems to hang in Win3 for shares with readonly attr set

			while ( 1 ) {
				SYSTEMTIME            time;
				HANDLE                hFinder;
				WIN32_FIND_DATAW      w32fd;
				WCHAR                 szUniqueFileName[ 20 ];
    
				wcscpy( szTestFileName, pszDirName );
				if ( *(szTestFileName + wcslen(szTestFileName) - 1) != L'\\' )
					wcscat( szTestFileName, L"\\" );

				::GetSystemTime( &time );
				wsprintfW( szUniqueFileName, L"%x%x.tmp", time.wSecond, time.wMilliseconds );
				wcscat( szTestFileName, szUniqueFileName );

				hFinder = ::FindFirstFileW( szTestFileName, &w32fd );
				::FindClose( hFinder );
				if ( hFinder == INVALID_HANDLE_VALUE )      // No such file => unique filename found
					break;
			}

			IOS_ERROR_CODE dwfError;

			CPointer<XFileW> file = new XFileW( szTestFileName, FIOAM_WRITE );

			dwfError = ::IOSGetLastError();

			file = 0;

			::IOSDeleteFileW( szTestFileName );

			return dwfError;
		}
 	  else {
			CPathStrA pConverted( pszDirName );
			return ::IOSCheckWritableDir( pConverted );
		}
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}

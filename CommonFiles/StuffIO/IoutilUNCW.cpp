#include <windows.h>
#include <tchar.h>
#include "../stuff/pathstr.h"
#include "../stuff/pathstra.h"
#include "../stuff/pathstrw.h"
#include "../stuff/csystem.h"
#include "xerrors.h"
#include "ioutil.h"


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);


IOS_ERROR_CODE IOSFinishCopyingW( const WCHAR * pszFileName, WCHAR * pszBuffer, uint uicchBufferLen );
IOS_ERROR_CODE IOSLocalPathToUNC95( const TCHAR *szLocalPath, TCHAR *szUNCPath, uint uicchUNCLen );

// ---
IOS_ERROR_CODE IOSLocalPathToUNCW( const WCHAR * szLocalPath, WCHAR * szUNCPath, uint uicchUNCLen ) {
#if defined(_UNICODE)
	return ::IOSLocalPathToUNC( szLocalPath, szUNCPath, uicchUNCLen );
#else
  if ( szLocalPath && szUNCPath ) {
    if ( _wcsnextc(_wcsninc(szLocalPath, 0)) != L'\\' || 
			   _wcsnextc(_wcsninc(szLocalPath, 1)) != L'\\' ) {
      // Если путь уже не UNC
      if ( !CSystem::IsWin95() ) {
        // Под Win95 эта функция дает ошибку
        uint32 dwSize;
        uint32 dwResult;
				if ( g_bUnicodePlatform ) {
					WCHAR buf[_MAX_PATH];
					dwSize = sizeof( buf );
					dwResult = ::WNetGetUniversalNameW( szLocalPath, UNIVERSAL_NAME_INFO_LEVEL, buf, &dwSize );
					if ( dwResult == ERROR_CONNECTION_UNAVAIL || dwResult == NO_ERROR ) 
						// Завершающее копирование с контролем размера буфера
						return ::IOSFinishCopyingW( ((UNIVERSAL_NAME_INFOW *)buf)->lpUniversalName, szUNCPath, uicchUNCLen );
				}
				else {
					WCHAR buf[_MAX_PATH];
					dwSize = sizeof( buf );
					CPathStrA szLocalPathA(szLocalPath);
					dwResult = ::WNetGetUniversalNameA( szLocalPathA, UNIVERSAL_NAME_INFO_LEVEL, buf, &dwSize );
					if ( dwResult == ERROR_CONNECTION_UNAVAIL || dwResult == NO_ERROR ) {
						// Завершающее копирование с контролем размера буфера
						CPathStrW pUniversalNameW( ((UNIVERSAL_NAME_INFO *)buf)->lpUniversalName );
						return ::IOSFinishCopyingW( pUniversalNameW, szUNCPath, uicchUNCLen );
					}
				}
      } 
      else {
				CPathStrA szLocalPathA(szLocalPath);
				CHAR szUNCPathA[_MAX_PATH];
        IOS_ERROR_CODE dwResult = ::IOSLocalPathToUNC95( szLocalPathA, szUNCPathA, sizeof(szUNCPathA) );
				CPathStrW pUNCNameW( szUNCPathA );
				return ::IOSFinishCopyingW( pUNCNameW, szUNCPath, uicchUNCLen );
			}
    }
    else 
      // Завершающее копирование с контролем размера буфера
      return ::IOSFinishCopyingW( szLocalPath, szUNCPath, uicchUNCLen );

    return IOS_ERR_NETWORK_ERROR;  
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}



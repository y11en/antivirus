#include <windows.h>
#include <tchar.h>
#include "../stuff/pathstr.h"
#include "../stuff/pathstra.h"
#include "../stuff/pathstrw.h"
#include "../stuff/csystem.h"
#include "xerrors.h"
#include "ioutil.h"

IOS_ERROR_CODE IOSFinishCopying( const TCHAR * pszFileName, TCHAR * pszBuffer, uint uicchBufferLen );

// ---
IOS_ERROR_CODE IOSLocalPathToUNC95( const TCHAR *szLocalPath, TCHAR *szUNCPath, uint uicchUNCLen ) {
  // get the local drive letter
  TCHAR chLocal = (TCHAR)toupper( _tcsnextc(szLocalPath) );

  // cursory validation
  if ( chLocal < L'A' || chLocal > L'Z' )
    return IOS_ERR_INVALID_FILE_NAME;

  if ( _tcsnextc(_tcsninc(szLocalPath, 1)) != L':' || _tcsnextc(_tcsninc(szLocalPath, 2)) != L'\\' )
    return IOS_ERR_INVALID_FILE_NAME;

  HANDLE hEnum;
  uint32 dwResult = ::WNetOpenEnum( RESOURCE_CONNECTED, RESOURCETYPE_DISK,
                                     0, NULL, &hEnum );

  if ( dwResult != NO_ERROR )
    return IOS_ERR_NETWORK_ERROR;

  // request all available entries
  const int    c_cEntries   = 0xFFFFFFFF;

  // start with a reasonable buffer size
  uint32                 cbBuffer     = 50 * sizeof( NETRESOURCE );
  CAPointer<NETRESOURCE> pNetResource = new NETRESOURCE[ cbBuffer ];

  IOS_ERROR_CODE fResult = IOS_ERR_NETWORK_ERROR;

  while ( true ) {
    uint32 dwSize   = cbBuffer;
    uint32 cEntries = c_cEntries;

    dwResult = ::WNetEnumResource( hEnum, &cEntries, pNetResource, &dwSize );

    if ( dwResult == ERROR_MORE_DATA ) {
      // the buffer was too small, enlarge
      cbBuffer = dwSize;
      pNetResource = new NETRESOURCE[ cbBuffer ];
      continue;
    }

    if ( dwResult != NO_ERROR )
      goto done;

    // search for the specified drive letter
    for ( int i=0; i<(int)cEntries; i++ )
      if ( pNetResource[i].lpLocalName &&
           chLocal == toupper(pNetResource[i].lpLocalName[0]) ) {
         // match - build a UNC name
        CPathStr pszFinishPath( _MAX_PATH );
        pszFinishPath += pNetResource[i].lpRemoteName;
        pszFinishPath ^= _tcsninc(szLocalPath, 2);
        // Завершающее копирование с контролем размера буфера
        fResult = ::IOSFinishCopying( pszFinishPath, szUNCPath, uicchUNCLen );
        goto done;
      }
   }

done:
  // cleanup
  ::WNetCloseEnum( hEnum );

  return fResult;
}


// ---
IOS_ERROR_CODE IOSLocalPathToUNC( const TCHAR * szLocalPath, TCHAR * szUNCPath, uint uicchUNCLen ) {
  if ( szLocalPath && szUNCPath ) {
    if ( _tcsnextc(_tcsninc(szLocalPath, 0)) != L'\\' || 
			   _tcsnextc(_tcsninc(szLocalPath, 1)) != L'\\' ) {
      // Если путь уже не UNC
      if ( !CSystem::IsWin95() ) {
        // Под Win95 эта функция дает ошибку
        uint32 dwSize;
        uint32 dwResult;
        TCHAR buf[_MAX_PATH];

        dwSize = sizeof( buf );
        dwResult = ::WNetGetUniversalName( szLocalPath, UNIVERSAL_NAME_INFO_LEVEL, buf, &dwSize );

        if ( dwResult == ERROR_CONNECTION_UNAVAIL || dwResult == NO_ERROR ) 
          // Завершающее копирование с контролем размера буфера
          return ::IOSFinishCopying( ((UNIVERSAL_NAME_INFO *)buf)->lpUniversalName, szUNCPath, uicchUNCLen );
      } 
      else
        return ::IOSLocalPathToUNC95( szLocalPath, szUNCPath, uicchUNCLen );
    }
    else 
      // Завершающее копирование с контролем размера буфера
      return ::IOSFinishCopying( szLocalPath, szUNCPath, uicchUNCLen );

    return IOS_ERR_NETWORK_ERROR;  
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



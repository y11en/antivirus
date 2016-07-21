#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuffio/ioutil.h>

IOS_ERROR_CODE IOSFinishCopying( const TCHAR * pszFileName, TCHAR * pszBuffer, uint uicchBufferLen );

// ---
IOS_ERROR_CODE IOSFullPathToBased( const TCHAR *szSourcePath, const TCHAR *szBasePath, const TCHAR *szAlias, TCHAR *szBuffer, uint uicchBufferLen ) {
  if ( szSourcePath && szBasePath && szAlias && szBuffer ) {
    const TCHAR *psBase = _tcsstr( szSourcePath, szBasePath );
    if ( psBase && psBase == szSourcePath ) {
      CPathStr pszFinishPath( _MAX_PATH );
      pszFinishPath += _T("<");
      pszFinishPath += szAlias;
      pszFinishPath += _T(">");
      pszFinishPath ^= _tcsninc(szSourcePath, _tcsclen(szBasePath));
      // Завершающее копирование с контролем размера буфера
      return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
    }
    // Завершающее копирование с контролем размера буфера
    return ::IOSFinishCopying( szSourcePath, szBuffer, uicchBufferLen );
  }
  return IOS_ERR_INVALID_FILE_NAME;
}


// ---
IOS_ERROR_CODE IOSBasedPathToFull( const TCHAR *szSourcePath, const TCHAR *szBasePath, const TCHAR *szAlias, TCHAR *szBuffer, uint uicchBufferLen ) {
  if ( szSourcePath && szBasePath && szAlias && szBuffer ) {
    if ( _tcsnextc(szSourcePath) == L'<' &&
          _tcsstr(szSourcePath, szAlias) == _tcsninc(szSourcePath, 1) &&
          _tcsnextc(_tcsninc(szSourcePath, _tcsclen(szAlias)) + 1) == L'>' ) {
      CPathStr pszFinishPath( _MAX_PATH );
      pszFinishPath += szBasePath;
      pszFinishPath ^= _tcsninc(szSourcePath, _tcsclen(szAlias)) + 2;
      // Завершающее копирование с контролем размера буфера
      return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
    }      
    // Завершающее копирование с контролем размера буфера
    return ::IOSFinishCopying( szSourcePath, szBuffer, uicchBufferLen );
  }
  return IOS_ERR_INVALID_FILE_NAME;
}


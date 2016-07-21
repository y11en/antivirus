#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuff/csystem.h>
#include <stuffio/ioutil.h>

IOS_ERROR_CODE IOSFinishCopying( const TCHAR * pszFileName, TCHAR * pszBuffer, uint uicchBufferLen );

// ---
static bool IsPath( const TCHAR * pszFileName ) {
	int n0 = _tcsnextc( _tcsninc(pszFileName, 0) );
	int n1 = _tcsnextc( _tcsninc(pszFileName, 1) );
	int n2 = _tcsnextc( _tcsninc(pszFileName, 2) );
  return (n0 == L'\\' && n1 == '\\') ||
          n1 == ':' ||
         (n0 == '.' && n1 == '.' && n2 == L'\\');
}

// —хема поиска
// 1 - каталог запуска
// 2 - текущий каталог
// 3 - каталог Windows
// 4 - каталог Windows\System
// 5 - каталог Windows\System32
// 6 - каталоги Path 

// ---
IOS_ERROR_CODE IOSFindFile( const TCHAR *pszFileName, TCHAR *szBuffer, uint uicchBufferLen, const TCHAR *pszFindPattern ) {
  if ( pszFileName && szBuffer ) {
    CPathStr pszFinishPath( _MAX_PATH );
    if ( ::IsPath(pszFileName) ) {
      if ( ::IOSMakeFullPath(pszFileName, pszFinishPath, _MAX_PATH) == IOS_ERR_OK ) {
        if ( ::IOSFileExists(pszFinishPath) == IOS_ERR_OK ) 
          return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
        else {
          if ( ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen ) != IOS_ERR_OK )
            return IOS_ERR_OUT_OF_MEMORY;
          return IOS_ERR_FILE_WITH_PATH_NOT_FOUND;  
        }  
      }
      else
        return IOS_ERR_INVALID_FILE_NAME;
    }  

    const TCHAR *pat = pszFindPattern;
    while ( _tcsnextc(pat) != L'\0' ) {
      switch ( _tcsnextc(pat) ) {
        case L'1' :
          if ( ::GetModuleFileName(NULL, pszFinishPath, _MAX_PATH) ) {
            if ( ::IOSDirExtract(pszFinishPath, pszFinishPath, _MAX_PATH) == IOS_ERR_OK ) {
              pszFinishPath ^= pszFileName;
              if ( ::IOSFileExists(pszFinishPath) == IOS_ERR_OK )
                return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
              else
                break;  
            }
          }
          return IOS_ERR_OUT_OF_MEMORY;  

        case L'2' :
          if ( ::IOSGetCurrentDirectory(pszFinishPath, _MAX_PATH) == IOS_ERR_OK ) {
            pszFinishPath ^= pszFileName;
            if ( ::IOSFileExists(pszFinishPath) == IOS_ERR_OK )
              return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
            else
              break;  
          } 
          return IOS_ERR_OUT_OF_MEMORY;  

        case L'3' :
          if ( ::GetWindowsDirectory(pszFinishPath, _MAX_PATH) ) {
            pszFinishPath ^= pszFileName;
            if ( ::IOSFileExists(pszFinishPath) == IOS_ERR_OK )
              return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
            else
              break;  
          }
          return IOS_ERR_OUT_OF_MEMORY;  

        case L'4' :
          if ( ::GetSystemDirectory(pszFinishPath, _MAX_PATH) ) {
            pszFinishPath ^= pszFileName;
            if ( ::IOSFileExists(pszFinishPath) == IOS_ERR_OK )
              return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
            else
              break;  
          }
          return IOS_ERR_OUT_OF_MEMORY;  

        case L'5' :
          if ( CSystem::IsNT() ) {
            if ( ::GetWindowsDirectory(pszFinishPath, _MAX_PATH) ) {
              pszFinishPath ^= _T("SYSTEM");
              pszFinishPath ^= pszFileName;
              if ( ::IOSFileExists(pszFinishPath) == IOS_ERR_OK )
                return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
              else
                break;  
            }
            return IOS_ERR_OUT_OF_MEMORY;  
          }
        case L'6' : {
					_tsearchenv( pszFileName, _T("PATH"), pszFinishPath );
          if ( *pszFinishPath ) 
            return ::IOSFinishCopying( pszFinishPath, szBuffer, uicchBufferLen );
          else
            break;  
        }
      }
      pat = _tcsinc( pat );
    }
    return IOS_ERR_FILE_NOT_FOUND;  
  }  
  return IOS_ERR_INVALID_FILE_NAME;
}

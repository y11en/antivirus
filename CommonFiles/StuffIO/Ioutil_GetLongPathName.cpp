#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuffio/ioutil.h>

IOS_ERROR_CODE IOSFinishCopying( const TCHAR * pszFileName, TCHAR * pszBuffer, uint uicchBufferLen );

// ---
IOS_ERROR_CODE IOSGetLongPathName( const TCHAR *pszFileName, TCHAR *szBuffer, uint uicchBufferLen ) {
  if ( pszFileName && szBuffer ) {
    CPathStr pszFinishPath( _MAX_PATH );
    if ( ::IOSMakeFullPath(pszFileName, pszFinishPath, _MAX_PATH) == IOS_ERR_OK ) {
      if ( ::IOSFileExists(pszFinishPath) == IOS_ERR_OK ) {
        CPathStr pszTmpFileName( _MAX_PATH );
        CPathStr pszFinishPathName( _MAX_PATH );
          
        WIN32_FIND_DATA w32fd;
        do {
          HANDLE hFinder = ::FindFirstFile( pszFinishPath, &w32fd );
          ::FindClose( hFinder );

          if ( hFinder == INVALID_HANDLE_VALUE ) 
            return IOS_ERR_FILE_WITH_PATH_NOT_FOUND;  

          if ( *w32fd.cAlternateFileName ) 
            // Значит имя конвертировано
            pszTmpFileName = w32fd.cFileName;
          else 
            ::IOSFileExtract( pszFinishPath, pszTmpFileName, _MAX_PATH );

          pszTmpFileName ^= pszFinishPathName;
          pszFinishPathName = pszTmpFileName;
              
          ::IOSDirExtract( pszFinishPath, pszFinishPath, _MAX_PATH );
        } while ( /*::IOSGetIsDirectory(pszFinishPath) == IOS_ERR_OK &&*/
                  ::IOSFileExtract(pszFinishPath, pszTmpFileName, _MAX_PATH) == IOS_ERR_OK &&
                  *pszTmpFileName );

        pszTmpFileName = pszFinishPath;
        pszTmpFileName ^= pszFinishPathName;
          
        return ::IOSFinishCopying( pszTmpFileName, szBuffer, uicchBufferLen );
      }
      return IOS_ERR_FILE_WITH_PATH_NOT_FOUND;  
    }
  }      
  return IOS_ERR_INVALID_FILE_NAME;
}

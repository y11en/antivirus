#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuffio/ioutil.h>


#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


//---
IOS_ERROR_CODE IOSDirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {
		
    TCHAR fDrive[_MAX_DRIVE];
    TCHAR fDir[_MAX_DIR];
		
		// выделить диск и каталог
    _tsplitpath( pszFileName, fDrive, fDir, NULL, NULL);
    int dLen = (int)_tcsclen( fDir );
		TCHAR *pLast;
		
    if ( dLen > 1 && 
			(pLast = _tcsninc(fDir, dLen - 1)) && 
			(_tcsnextc(pLast) == L'\\') )
      *pLast = L'\0';
		
    if ( (size_t)(TCSCLEN(fDrive) + TCSCLEN(fDir)) < uicchBufferLen ) {
      _tcsncpy( pszDirName, fDrive, uicchBufferLen );
      int uichLen = (int)TCSCLEN(pszDirName);
      pszDirName = _tcsninc( pszDirName, _tcsclen(pszDirName)/*uichLen*/ );
      _tcsncpy( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
    else
      return IOS_ERR_OUT_OF_MEMORY;  
		
    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

//---
IOS_ERROR_CODE IOSRootDirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen ) {

  IOS_ERROR_CODE fError = ::IOSDirExtract( pszFileName, pszDirName, uicchBufferLen );

  if ( fError == IOS_ERR_OK ) {
    if ( _tcsnextc(pszDirName) == L'\\' ) {
    // новая нотация \\netdrive\drive\dir\..
      TCHAR *psEndOfDrive = _tcschr( _tcsninc(pszDirName, 2), L'\\' );
      if ( psEndOfDrive ) {
        TCHAR *psEndOfFirstDir = _tcschr( _tcsninc(psEndOfDrive, 1), L'\\' );
        if ( psEndOfFirstDir )
        // файл находился в подкаталоге сетевого устройства
        // отрежем подкаталог
          *_tcsninc(psEndOfFirstDir, 1) = L'\0';
        else
        // файл находился в корневом каталоге сетевого устройства
        // нельзя отпускать каталог без концевого '\'
          _tcscat( pszDirName, _T("\\") );
      }
    }
    else {
    // старая нотация c:\dir\..
			int iLen = _tcslen( pszDirName );
			switch ( iLen ) {
				case 0 :
					*pszDirName = *pszFileName;
					*_tcsninc(pszDirName, 1) = L'\0';
				case 1 :
					_tcscat( pszDirName, _T(":\\") );
					break;
				case 2 :
					_tcscat( pszDirName, _T("\\") );
					break;
				default :
					*_tcsninc(pszDirName, 2) = L'\0';
					_tcscat( pszDirName, _T("\\") );
			}
    }
  }

  return fError;
}


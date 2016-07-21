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
IOS_ERROR_CODE IOSFileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen ) {
  if ( pszFullFileName && pszFileName ) {
		
    CAPointer<TCHAR> fFile = new TCHAR[_MAX_FNAME];
    CAPointer<TCHAR> fExt  = new TCHAR[_MAX_EXT];
		
		// выделить диск и каталог
    _tsplitpath( pszFullFileName, NULL, NULL, fFile, fExt);
		
    if ( uint(TCSCLEN(fFile) + TCSCLEN(fExt)) < uicchBufferLen ) {
      _tcsncpy( pszFileName, fFile, uicchBufferLen );
      uint uichLen = (uint)TCSCLEN( pszFileName );
      pszFileName = _tcsninc(pszFileName, _tcsclen( pszFileName )/*uichLen*/);
      _tcsncpy( pszFileName, fExt, uicchBufferLen - uichLen );
    }  
    else
      return IOS_ERR_OUT_OF_MEMORY;  
		
    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

//---
IOS_ERROR_CODE IOSPureFileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen ) {
  if ( pszFullFileName && pszFileName ) {
    CAPointer<TCHAR> pszFile = new TCHAR[_MAX_FNAME];
    if ( ::IOSFileExtract(pszFullFileName, pszFile, _MAX_FNAME) == IOS_ERR_OK ) {
      TCHAR *psExt = ::IOSFindFileExtention( pszFile, false );
      if ( psExt )
        *psExt = L'\0';
      if ( uint(TCSCLEN(pszFile)) < uicchBufferLen ) {
        _tcsncpy( pszFileName, pszFile, uicchBufferLen );
        return IOS_ERR_OK;
      }  
    }
    return IOS_ERR_OUT_OF_MEMORY;  
  }
  return IOS_ERR_INVALID_FILE_NAME;
}


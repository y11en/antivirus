#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


//---
IOS_ERROR_CODE IOSMakeFullPath( const TCHAR * pszRelativePath, TCHAR * pszBuffer, uint uicchBufferLen ) {
  if ( pszRelativePath && pszBuffer ) {
    if ( _tfullpath( pszBuffer, pszRelativePath, uicchBufferLen ) == NULL )
      return IOS_ERR_INVALID_FILE_NAME;   
		
    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

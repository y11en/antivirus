#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


//---
IOS_ERROR_CODE IOSFinishCopying( const TCHAR * pszFileName, TCHAR * pszBuffer, uint uicchBufferLen ) {
	if ( pszFileName ) {
		if ( uint(_tcsclen(pszFileName)) < uicchBufferLen ) {
			_tcsncpy( pszBuffer, pszFileName, uicchBufferLen );
			return IOS_ERR_OK;
		}  
		return IOS_ERR_OUT_OF_MEMORY;  
	}
  return IOS_ERR_INVALID_FILE_NAME;
}

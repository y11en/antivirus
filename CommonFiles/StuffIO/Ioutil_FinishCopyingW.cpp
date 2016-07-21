#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

IOS_ERROR_CODE IOSFinishCopying( const TCHAR * pszFileName, TCHAR * pszBuffer, uint uicchBufferLen );

//---
IOS_ERROR_CODE IOSFinishCopyingW( const WCHAR * pszFileName, WCHAR * pszBuffer, uint uicchBufferLen ) {
#if defined(_UNICODE)
	return ::IOSFinishCopying( pszFileName, pszBuffer, uicchBufferLen );
#else
	if ( pszFileName ) {
		if ( uint(wcslen(pszFileName)) < uicchBufferLen ) {
			wcsncpy( pszBuffer, pszFileName, uicchBufferLen );
			return IOS_ERR_OK;
		}  
		return IOS_ERR_OUT_OF_MEMORY;  
	}
  return IOS_ERR_INVALID_FILE_NAME;
#endif	
}

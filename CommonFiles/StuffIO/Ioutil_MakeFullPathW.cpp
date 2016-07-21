#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSMakeFullPathW( const WCHAR * pszRelativePath, WCHAR * pszBuffer, uint uicchBufferLen ) {
#if defined(_UNICODE)
	return ::IOSMakeFullPath( pszRelativePath, pszBuffer, uicchBufferLen );
#else
  if ( pszRelativePath && pszBuffer ) {
    if ( g_bUnicodePlatform ) {
			if ( _wfullpath( pszBuffer, pszRelativePath, uicchBufferLen ) == NULL )
				return IOS_ERR_INVALID_FILE_NAME;   
			return IOS_ERR_OK;
		}
		else {
			CPathStrA pConverted(pszRelativePath);
			CPathStrA pBuffer( (int)uicchBufferLen * 2 );
			IOS_ERROR_CODE nError = ::IOSMakeFullPath( pConverted, pBuffer, uicchBufferLen * 2  );
			wcsncpy( pszBuffer, CPathStrW(pBuffer), uicchBufferLen );
			//mbstowcs( pszBuffer, pBuffer, uicchBufferLen );
			return nError;
		}
    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}

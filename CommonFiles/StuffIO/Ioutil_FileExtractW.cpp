#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSFileExtractW( const WCHAR * pszFullFileName, WCHAR * pszFileName, uint uicchBufferLen ) {
#if defined(_UNICODE)
	return ::IOSFileExtract( pszFullFileName, pszFileName, uicchBufferLen );
#else
  if ( pszFullFileName && pszFileName ) {
		
    if ( g_bUnicodePlatform ) {
			CAPointer<WCHAR> fFile = new WCHAR[_MAX_FNAME];
			CAPointer<WCHAR> fExt  = new WCHAR[_MAX_EXT];
			
			// выделить диск и каталог
			_wsplitpath( pszFullFileName, NULL, NULL, fFile, fExt);
			
			if ( uint(wcslen(fFile) + wcslen(fExt)) < uicchBufferLen ) {
				wcscpy( pszFileName, fFile );
				pszFileName = _wcsninc(pszFileName, wcslen(pszFileName) );
				wcscpy( pszFileName, fExt );
			}  
			else
				return IOS_ERR_OUT_OF_MEMORY;  
			
			return IOS_ERR_OK;
		}
		else {
			CPathStrA pConverted(pszFullFileName);
			CPathStrA pBuffer( (int)uicchBufferLen * 2 );
			IOS_ERROR_CODE nError = ::IOSFileExtract( pConverted, pBuffer, uicchBufferLen * 2  );
			wcsncpy( pszFileName, CPathStrW(pBuffer), uicchBufferLen );
			return nError;
		}
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif	
}

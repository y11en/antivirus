#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSDirExtractW( const WCHAR * pszFileName, WCHAR * pszDirName, uint uicchBufferLen ) {
#if defined(_UNICODE)
	return ::IOSDirExtract( pszFileName, pszDirName, uicchBufferLen );
#else
  if ( pszFileName && pszDirName ) {
		
    if ( g_bUnicodePlatform ) {
			WCHAR fDrive[_MAX_DRIVE];
			WCHAR fDir[_MAX_DIR];
			
			// выделить диск и каталог
			_wsplitpath( pszFileName, fDrive, fDir, NULL, NULL);
			int dLen = (int)wcslen( fDir );
			WCHAR *pLast;
			
			if ( dLen > 1 && (pLast = fDir + (dLen - 1)) && (*pLast == L'\\') )
				*pLast = L'\0';
			
			if ( (size_t)(wcslen(fDrive) + wcslen(fDir)) < uicchBufferLen ) {
				wcsncpy( pszDirName, fDrive, uicchBufferLen );
				int uichLen = (int)wcslen(pszDirName);
				pszDirName += wcslen(pszDirName);
				wcsncpy( pszDirName, fDir, uicchBufferLen - uichLen );
			}  
			else
				return IOS_ERR_OUT_OF_MEMORY;  
			
			return IOS_ERR_OK;
		}
		else {
			CPathStrA pConverted(pszFileName);
			CPathStrA pBuffer( (int)uicchBufferLen * 2 );
			IOS_ERROR_CODE nError = ::IOSDirExtract( pConverted, pBuffer, uicchBufferLen * 2  );
			wcsncpy( pszDirName, CPathStrW(pBuffer), uicchBufferLen);
			//mbstowcs( pszDirName, pBuffer, uicchBufferLen );
			return nError;
		}
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif
}

// ---
static void NomalizeDriveName( WCHAR *pDriveName ) {
	int iLen = wcslen( pDriveName );
	switch ( iLen ) {
		case 1 :
			wcscat( pDriveName, L":\\" );
			break;
		case 2 :
			wcscat( pDriveName, L"\\" );
			break;
	}
}




//---
IOS_ERROR_CODE IOSRootDirExtractW( const WCHAR * pszFileName, WCHAR * pszDirName, uint uicchBufferLen ) {
#if defined(_UNICODE)
	return ::IOSRootDirExtract( pszFileName, pszDirName, uicchBufferLen );
#else
	
  IOS_ERROR_CODE fError = ::IOSDirExtractW( pszFileName, pszDirName, uicchBufferLen );

  if ( fError == IOS_ERR_OK ) {
    if ( _wcsnextc(pszDirName) == L'\\' ) {
    // новая нотация \\netdrive\drive\dir\..
      WCHAR *psEndOfDrive = wcschr( _wcsninc(pszDirName, 2), L'\\' );
      if ( psEndOfDrive ) {
        WCHAR *psEndOfFirstDir = wcschr( _wcsninc(psEndOfDrive, 1), L'\\' );
        if ( psEndOfFirstDir )
        // файл находился в подкаталоге сетевого устройства
        // отрежем подкаталог
          *_wcsninc(psEndOfFirstDir, 1) = L'\0';
        else
        // файл находился в корневом каталоге сетевого устройства
        // нельзя отпускать каталог без концевого '\'
          wcscat( pszDirName, L"\\" );
      }
    }
    else {
    // старая нотация c:\dir\..
			int iLen = wcslen( pszDirName );
			switch ( iLen ) {
				case 0 :
					*pszDirName = *pszFileName;
					*_wcsninc(pszDirName, 1) = L'\0';
				case 1 :
					wcscat( pszDirName, L":\\" );
					break;
				case 2 :
					wcscat( pszDirName, L"\\" );
					break;
				default :
					*_wcsninc(pszDirName, 2) = L'\0';
					wcscat( pszDirName, L"\\" );
					break;
			}
    }
  }

  return fError;
#endif
}


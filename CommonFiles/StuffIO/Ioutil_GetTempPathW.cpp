#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// Получить каталог временных файлов 
// 
DWORD IOSGetTempPathW( DWORD cchBuffer, WCHAR *pchBuffer ) {
#if defined(_UNICODE)
	return ::IOSGetTempPath( cchBuffer, pchBuffer ); 
#else
	if ( g_bUnicodePlatform ) {
		return ::GetTempPathW( cchBuffer, pchBuffer ); 
	}
	else {
		CPathStrA pAnsiName( (int)cchBuffer );
		DWORD dwResult = ::GetTempPathA( cchBuffer, pAnsiName ); 
		if ( cchBuffer > dwResult )	{
			wcsncpy( pchBuffer, CPathStrW(pAnsiName), cchBuffer );
			//mbstowcs( pchBuffer, pAnsiName, cchBuffer );
			return (DWORD)wcslen( pchBuffer );
		}
		return dwResult / sizeof(WCHAR)	+ 1;
	}
#endif
}


// Получить путь на временный файл
UINT IOSGetTempFileNameW( const WCHAR *lpPathName, const WCHAR *lpPrefixString, UINT uUnique, WCHAR *lpTempFileName ) {
#if defined(_UNICODE)
	return ::IOSGetTempFileName( lpPathName, lpPrefixString, uUnique, lpTempFileName );
#else
	if ( g_bUnicodePlatform ) {
		return ::GetTempFileNameW( lpPathName, lpPrefixString, uUnique, lpTempFileName );
	}
	else {
		CPathStrA pPathNameAnsi(lpPathName);
		CPathStrA pPrefixStringAnsi(lpPrefixString);
		CPathStrA pTempFileNameAnsi( _MAX_PATH );
		UINT uiResult = ::GetTempFileNameA( pPathNameAnsi, pPrefixStringAnsi, uUnique, pTempFileNameAnsi );
		wcsncpy( lpTempFileName, CPathStrW(pTempFileNameAnsi), _MAX_PATH );
		//mbstowcs( lpTempFileName, pTempFileNameAnsi, _MAX_PATH );
		return uiResult;
	}
#endif
}

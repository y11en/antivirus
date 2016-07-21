#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// Получить короткое имя файла 
//
DWORD IOSGetShortPathNameW( const WCHAR *lpszLongPath, WCHAR *lpszShortPath, DWORD cchBuffer ) {
#if defined(_UNICODE)
	return ::IOSGetShortPathName( lpszLongPath, lpszShortPath, cchBuffer );
#else
	if ( g_bUnicodePlatform ) {
		return ::GetShortPathNameW( lpszLongPath, lpszShortPath, cchBuffer );
	}
	else {
		CPathStrA pLongAnsiName(lpszLongPath);
		CPathStrA pShortAnsiName( (int)cchBuffer);
		::GetShortPathNameA( pLongAnsiName, pShortAnsiName, cchBuffer );
		wcsncpy( lpszShortPath, CPathStrW(pShortAnsiName), cchBuffer );
		//mbstowcs( lpszShortPath, pShortAnsiName, cchBuffer );
		return (DWORD)wcslen( lpszShortPath );
	}
#endif	
}

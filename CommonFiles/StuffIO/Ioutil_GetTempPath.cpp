#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


// Получить каталог временных файлов 
// 
DWORD IOSGetTempPath( DWORD cchBuffer, TCHAR *pchBuffer ) {
	return ::GetTempPath( cchBuffer, pchBuffer ); 
}


// Получить путь на временный файл
UINT IOSGetTempFileName( const TCHAR *lpPathName, const TCHAR *lpPrefixString, UINT uUnique, TCHAR *lpTempFileName ) {
	return ::GetTempFileName( lpPathName, lpPrefixString, uUnique, lpTempFileName );
}

#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

static CPathStrW GetPrefixedFileNameW(const WCHAR *pszFileName)
{
	if(!g_bUnicodePlatform)
		return pszFileName;

	CPathStrW FileName;
	size_t FileNameLen = pszFileName ? wcslen(pszFileName) : 0;
	if(FileNameLen)
	{
		FileName = pszFileName;
		if(FileNameLen >= 2 && (FileNameLen < 4 || wcsncmp(pszFileName, L"\\\\?\\", 4) != 0)) // skip "\\?\" and "\\?\UNC\"
		{
			if(pszFileName[1] == L':')
				FileName = CPathStrW(L"\\\\?\\") + pszFileName;
			else if(wcsncmp(pszFileName, L"\\\\", 2) == 0)
				FileName = CPathStrW(L"\\\\?\\UNC\\") + (pszFileName + 2);
		}
	}
	return FileName;
}

// Запросить атрибуты файла
//
DWORD IOSGetFileAttributesW( const WCHAR *pszFileName ) {
#if defined(_UNICODE)
	return ::IOSGetFileAttributes( pszFileName );
#else
	if ( g_bUnicodePlatform ) {
		DWORD dwResult = ::GetFileAttributesW( GetPrefixedFileNameW(pszFileName) );
		::Win32ErrToIOSErr( ::GetLastError() );
		return dwResult;
	}
	else {
		DWORD dwResult = ::GetFileAttributesA( CPathStrA(pszFileName) );
		::Win32ErrToIOSErr( ::GetLastError() );
		return dwResult;
	}
#endif
}


// ---
IOS_ERROR_CODE IOSGetFileAttributesExW( const WCHAR *pszFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation ) {
#if defined(_UNICODE)
	return ::IOSGetFileAttributesEx( pszFileName, fInfoLevelId, lpFileInformation );
#else
  ::IOSSetLastError( IOS_ERR_OK );
	if ( g_bUnicodePlatform ) {
		if ( !::GetFileAttributesExW(GetPrefixedFileNameW(pszFileName), fInfoLevelId, lpFileInformation) )
			::Win32ErrToIOSErr( ::GetLastError() );
	}
	else {
		if ( !::GetFileAttributesExA(CPathStrA(pszFileName), fInfoLevelId, lpFileInformation) )
			::Win32ErrToIOSErr( ::GetLastError() );
	}
	return ::IOSGetLastError();
#endif
}



// Установить атрибуты файла
// 
// ---
BOOL IOSSetFileAttributesW( const WCHAR *pszFileName, DWORD dwFileAttributes ) {
#if defined(_UNICODE)
	return ::IOSSetFileAttributes( pszFileName, dwFileAttributes );
#else
	if ( g_bUnicodePlatform ) {
		DWORD dwResult = ::SetFileAttributesW( GetPrefixedFileNameW(pszFileName), dwFileAttributes );
		::Win32ErrToIOSErr( ::GetLastError() );
		return dwResult;
	}
	else {
		DWORD dwResult = ::SetFileAttributesA( CPathStrA(pszFileName), dwFileAttributes );
		::Win32ErrToIOSErr( ::GetLastError() );
		return dwResult;
	}
#endif	
}

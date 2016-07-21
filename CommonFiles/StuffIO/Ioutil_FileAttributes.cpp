#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

static CPathStr GetPrefixedFileName(const TCHAR *pszFileName)
{
	if(!g_bUnicodePlatform)
		return pszFileName;

	CPathStr FileName;
	size_t FileNameLen = pszFileName ? _tcslen(pszFileName) : 0;
	if(FileNameLen)
	{
		FileName = pszFileName;
		if(FileNameLen >= 2 && (FileNameLen < 4 || _tcsncmp(pszFileName, _T("\\\\?\\"), 4) != 0)) // skip "\\?\" and "\\?\UNC\"
		{
			if(pszFileName[1] == _T(':'))
				FileName = CPathStr(_T("\\\\?\\")) + pszFileName;
			else if(_tcsncmp(pszFileName, _T("\\\\"), 2) == 0)
				FileName = CPathStr(_T("\\\\?\\UNC\\")) + (pszFileName + 2);
		}
	}
	return FileName;
}

// Запросить атрибуты файла
//
DWORD IOSGetFileAttributes( const TCHAR *pszFileName ) {
	DWORD dwResult = ::GetFileAttributes( GetPrefixedFileName(pszFileName) );
	::Win32ErrToIOSErr( ::GetLastError() );
	return dwResult;
}

// ---
IOS_ERROR_CODE IOSGetFileAttributesEx( const TCHAR *pszFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation ) {
  ::IOSSetLastError( IOS_ERR_OK );
	if ( !::GetFileAttributesEx(GetPrefixedFileName(pszFileName), fInfoLevelId, lpFileInformation) )
		::Win32ErrToIOSErr( ::GetLastError() );
	return ::IOSGetLastError();
}


// Установить атрибуты файла
// 
BOOL IOSSetFileAttributes( const TCHAR *pszFileName, DWORD dwFileAttributes ) {
	BOOL bResult = ::SetFileAttributes( GetPrefixedFileName(pszFileName), dwFileAttributes );
	::Win32ErrToIOSErr( ::GetLastError() );
	return bResult;
}


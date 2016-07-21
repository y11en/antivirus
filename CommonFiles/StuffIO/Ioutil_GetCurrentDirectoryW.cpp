#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

//---
IOS_ERROR_CODE IOSGetCurrentDirectoryW( WCHAR * pszDirBuf, uint uicchBufSize, ulong *puicchBuffSizeNeeded/* = NULL*/ ) {
#if defined(_UNICODE)
	return ::IOSGetCurrentDirectory( pszDirBuf, uicchBufSize, puicchBuffSizeNeeded );
#else
  if ( pszDirBuf ) {
    ::IOSSetLastError( IOS_ERR_OK );
		
		DWORD dwSize = 0;
    if ( g_bUnicodePlatform ) {
			if ( !(dwSize = ::GetCurrentDirectoryW(uicchBufSize, pszDirBuf)) ) 
				::Win32ErrToIOSErr( ::GetLastError() );
		}
		else {
			if ( !pszDirBuf || !uicchBufSize ) {
				if ( !(dwSize = ::GetCurrentDirectoryA(0, NULL)) ) 
					::Win32ErrToIOSErr( ::GetLastError() );
			}
			else {
				CAPointer<char> pConverted = new char[uicchBufSize << 1];
				if ( !(dwSize = ::GetCurrentDirectoryA((uicchBufSize << 1), pConverted)) ) 
					::Win32ErrToIOSErr( ::GetLastError() );
				else {
					if ( dwSize <= uicchBufSize ) {
						dwSize = ::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pConverted, -1, pszDirBuf, uicchBufSize );
					}
					else {
						dwSize = ::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pConverted, -1, NULL, 0 );
					}
				}
			}
		}
		
		if ( puicchBuffSizeNeeded )
			*puicchBuffSizeNeeded = dwSize;
		
		return ::IOSGetLastError();
  }
  return IOS_ERR_INVALID_PARAMETERS;
#endif
}

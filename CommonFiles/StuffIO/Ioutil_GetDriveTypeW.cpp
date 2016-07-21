#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// ---
UINT IOSGetDriveTypeW( const WCHAR *pszDriveName ) {
#if defined(_UNICODE)
	return ::IOSGetDriveType( pszDriveName );
#else
	if ( g_bUnicodePlatform )
		return ::GetDriveTypeW( pszDriveName );
	else
		return ::GetDriveTypeA( CPathStrA(pszDriveName) );
#endif	
}

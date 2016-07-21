#include <windows.h>
#include <tchar.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuffio/ioutil.h>


static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// Получить информацию об устройстве
//  LPCTSTR lpRootPathName,           // root directory
//  LPTSTR	lpVolumeNameBuffer,        // volume name buffer
//  DWORD		nVolumeNameSize,            // length of name buffer
//  LPDWORD lpVolumeSerialNumber,     // volume serial number
//  LPDWORD lpMaximumComponentLength, // maximum file name length
//  LPDWORD lpFileSystemFlags,        // file system options
//  LPTSTR	lpFileSystemNameBuffer,    // file system name buffer
//  DWORD		nFileSystemNameSize         // length of file system name buffer
BOOL IOSGetVolumeInformationW( LPCWSTR lpRootPathName, LPWSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize ) {
#if defined(_UNICODE)
	return ::IOSGetVolumeInformation( lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize );
#else
	if ( g_bUnicodePlatform ) {
		return ::GetVolumeInformationW( lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize );
	}
	else {
		CPathStrA pRootPathNameAnsi( lpRootPathName );
		CPathStrA pVolumeNameBufferAnsi( (int)nVolumeNameSize * 2 );
		CPathStrA pFileSystemNameBufferAnsi( (int)nFileSystemNameSize * 2 );
		BOOL bResult = ::GetVolumeInformationA( pRootPathNameAnsi, pVolumeNameBufferAnsi, nVolumeNameSize * 2,  lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, pFileSystemNameBufferAnsi, nFileSystemNameSize * 2 );
		wcsncpy( lpFileSystemNameBuffer, CPathStrW(pFileSystemNameBufferAnsi), nFileSystemNameSize );
		//mbstowcs( lpFileSystemNameBuffer, pFileSystemNameBufferAnsi, nFileSystemNameSize );
		wcsncpy( lpVolumeNameBuffer, CPathStrW(pVolumeNameBufferAnsi), nVolumeNameSize );
		//mbstowcs( lpVolumeNameBuffer, pVolumeNameBufferAnsi, nVolumeNameSize );
		return bResult;
	}
#endif
}

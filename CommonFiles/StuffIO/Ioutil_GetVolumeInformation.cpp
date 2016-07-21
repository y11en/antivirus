#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


// Получить информацию об устройстве
//  LPCTSTR lpRootPathName,           // root directory
//  LPTSTR	lpVolumeNameBuffer,        // volume name buffer
//  DWORD		nVolumeNameSize,            // length of name buffer
//  LPDWORD lpVolumeSerialNumber,     // volume serial number
//  LPDWORD lpMaximumComponentLength, // maximum file name length
//  LPDWORD lpFileSystemFlags,        // file system options
//  LPTSTR	lpFileSystemNameBuffer,    // file system name buffer
//  DWORD		nFileSystemNameSize         // length of file system name buffer
BOOL IOSGetVolumeInformation( LPCTSTR lpRootPathName, LPTSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPTSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize ) {
	return ::GetVolumeInformation( lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize );
}

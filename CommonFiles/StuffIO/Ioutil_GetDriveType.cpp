#include <windows.h>
#include <stuffio/ioutil.h>


// Запросить тип устройства
//
UINT IOSGetDriveType( const TCHAR *pszDriveName ) {
	return ::GetDriveType( pszDriveName );
}


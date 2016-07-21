#include <windows.h>
#include <tchar.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>

// ---
IOS_ERROR_CODE IOSIsSubstDriveW( WCHAR cDrive ) {
#if defined(_UNICODE)
	return ::IOSIsSubstDrive( cDrive );
#else
	IOS_ERROR_CODE dwLastError = IOS_ERR_INVALID_PARAMETERS;

	if ( cDrive ) {
		cDrive = towupper(cDrive);
		TCHAR caDrive;
		::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, &cDrive, -1, &caDrive, sizeof(caDrive), NULL, NULL );
		dwLastError = ::IOSIsSubstDrive( caDrive );
	}

	return dwLastError;
#endif
}

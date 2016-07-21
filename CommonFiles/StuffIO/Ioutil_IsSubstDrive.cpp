#include <windows.h>
#include <tchar.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>
#define VWIN32_DIOC_DOS_IOCTL 1

typedef struct DIOCRegs { 
    DWORD   reg_EBX; 
    DWORD   reg_EDX; 
    DWORD   reg_ECX; 
    DWORD   reg_EAX; 
    DWORD   reg_EDI; 
    DWORD   reg_ESI; 
    DWORD   reg_Flags; 
} DIOC_REGISTERS; 


#define DRIVE_PREFIX         _T("\\??\\")

// ---
IOS_ERROR_CODE IOSIsSubstDrive( TCHAR caDrive ) {
	IOS_ERROR_CODE dwLastError = IOS_ERR_INVALID_PARAMETERS;

	if ( caDrive ) {
		OSVERSIONINFO	rcOS;
		rcOS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx( &rcOS );

		if( rcOS.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
			TCHAR pTargetPath[MAX_PATH];
			TCHAR pDestPath[MAX_PATH];
			wsprintf( pDestPath, _T("%C:"), caDrive );
			if ( ::QueryDosDevice(pDestPath, pTargetPath, MAX_PATH) )	{
				if ( _tcsnicmp(pTargetPath, DRIVE_PREFIX, _tcslen(DRIVE_PREFIX)) == 0 )
					dwLastError = IOS_ERR_OK;
			}
			else
				dwLastError = ::Win32ErrToIOSErr( ::GetLastError() );
		}
		else {
			HANDLE hVWIN32Device = ::CreateFile( _T("\\\\.\\vwin32"), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
																					 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if ( hVWIN32Device != INVALID_HANDLE_VALUE ) {
				DIOC_REGISTERS rcReg;
				rcReg.reg_EAX = (LONG)0x4409;
				rcReg.reg_EBX = (LONG)caDrive - L'A' + 1;

				DWORD cb;
				if ( ::DeviceIoControl(hVWIN32Device, VWIN32_DIOC_DOS_IOCTL, &rcReg, sizeof(rcReg), &rcReg, sizeof(rcReg), &cb, 0) &&
						 rcReg.reg_EDX & 0x8000 )
					dwLastError = IOS_ERR_OK;

				::CloseHandle( hVWIN32Device );
			}
			else
				dwLastError = ::Win32ErrToIOSErr( ::GetLastError() );
		}
	}

	return dwLastError;
}



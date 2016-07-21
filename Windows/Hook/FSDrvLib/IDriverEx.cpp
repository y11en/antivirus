#include <windows.h>
#include <winioctl.h>

#include "..\hook\IDriver.h"
#include "..\hook\avpgcom.h"

HANDLE GetDriverHandle(char *pFileName)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD dwErr;

	if ((hDevice = CreateFile(pFileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	{
		dwErr = GetLastError();
		if (!dwErr || dwErr == ERROR_ACCESS_DENIED)
		{
			for (int idx = 0; (idx < 1500) && (hDevice == INVALID_HANDLE_VALUE); idx++)
			{
				hDevice = CreateFile(pFileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hDevice == INVALID_HANDLE_VALUE)
					Sleep(10);
			}
		}
	}
	
	return hDevice;
}

HANDLE __cdecl IDriver_GetDriverHandleQuick()
{
	OSVERSIONINFO OSVer;
	char DrvStr[] = "\\\\.\\klif";

	OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&OSVer))
		return INVALID_HANDLE_VALUE;

	if (VER_PLATFORM_WIN32_NT == OSVer.dwPlatformId)
		return GetDriverHandle(DrvStr);

	HANDLE hDrv = CreateFile(DrvStr, 0, 0, NULL, 0, 0, NULL);
	
	for (int cou = 0; cou < 3; cou++)
	{
		if ((INVALID_HANDLE_VALUE == hDrv) || (NULL == hDrv))
		{
			Sleep(200);
			hDrv = CreateFile(DrvStr, 0, 0, NULL, 0, 0, NULL);
		}
	}

	return hDrv;
}

BOOL IDriver_SpecFileRequest(PSPECFILEFUNC_REQUEST pRequest, ULONG RequestSize, PVOID pBuffer, PULONG pBufferSize)
{
	BOOL bRet = FALSE;

	HANDLE hDevice = IDriver_GetDriverHandleQuick();
	
	if (INVALID_HANDLE_VALUE == hDevice)
	{
		*pBufferSize = 0;
		return FALSE;
	}

	bRet = DeviceIoControl(hDevice, IOCTLHOOK_SPECFILEREQUEST, pRequest, RequestSize, pBuffer, *pBufferSize, pBufferSize, NULL);

	CloseHandle(hDevice);

	return bRet;
}
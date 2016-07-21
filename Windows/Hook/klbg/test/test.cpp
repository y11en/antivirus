// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winioctl.h>
#include "..\ctlcodes.h"

#define BUFFER_SIZE		0x1000

VOID PrintoutItem( PSERVICE_ENUM_ENTRY_OUT pEntry )
{
	wprintf( L" type: %d start: %d service_name: %s image_path: %s md5_computed: %s\n" ,
		pEntry->m_Type,
		pEntry->m_Start,
		pEntry->m_ServiceName,
		(PWCHAR) ((PCHAR) pEntry->m_ServiceName + pEntry->m_ImagePathOffset),
		pEntry->m_Md5Computed ? L"TRUE" : L"FALSE"
		);
}

VOID EnumServices()
{
	HANDLE hDevice = CreateFileW(
		DOS_DEVICE_NAME,
		SYNCHRONIZE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if ( INVALID_HANDLE_VALUE == hDevice )
	{
		_tprintf( _T("failed to open device.\n"));
		return;
	}

	PSERVICE_ENUM_ENTRY_OUT pEntry = (PSERVICE_ENUM_ENTRY_OUT) HeapAlloc( GetProcessHeap(), 0, BUFFER_SIZE );
	if ( !pEntry )
		_tprintf( _T("failed to allocate memory.\n"));
	else
	{
		BOOL bResult;
		DWORD Dummy;
		DWORD cbReturned = 0;

		bResult = DeviceIoControl(
			hDevice,
			IOCTL_SW_GETFIRST,
			&Dummy,
			0,
			pEntry,
			BUFFER_SIZE,
			&cbReturned,
			NULL
			);
		if ( !bResult )
			_tprintf( _T("failed to get first item.\n"));
		else
		{
			do 
			{
				PrintoutItem( pEntry );

				bResult = DeviceIoControl(
					hDevice,
					IOCTL_SW_GETNEXT,
					&Dummy,
					0,
					pEntry,
					BUFFER_SIZE,
					&cbReturned,
					NULL
					);
			} while( bResult );
		}

		HeapFree( GetProcessHeap(), 0, pEntry );
	}

	CloseHandle( hDevice );
}

int _tmain(int argc, _TCHAR* argv[])
{
	EnumServices();

	return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

#include "../mklapi/mklapi.h"

LONG gMemCounter = 0;

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );
	
	return ptr;
};

void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};

int _cdecl
main (
    __in int argc,
    __in_ecount(argc) char *argv[]
    )
{
    HRESULT hResult = S_OK;

	char* pClientContext;
	ULONG ApiVersion = 0;
	ULONG AppId = 0;

	ULONG FltId = 0;

	DWORD dwThreadID1, dwThreadID2;
	HANDLE ThHandle1, ThHandle2;

	hResult = MKL_ClientRegister (
		&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (IS_ERROR( hResult ))
	{
		printf( "register client failed. err 0x%x\n", hResult );
		return 1;
	}

	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if (SUCCEEDED( hResult ))
		printf( "Mklif API version: %d\n", ApiVersion );
	else
		printf( "Mklif get API version failed. err 0x%x\n", hResult );

	hResult = MKL_GetAppId( pClientContext, &AppId );
	if (SUCCEEDED( hResult ))
		printf( "Client registered with id: %d (0x%x)\n", AppId, AppId );
	else
		printf( "Mklif get API version failed. err 0x%x\n", hResult );

	{
		PVOID pDevices = NULL;
		ULONG NamesLen = 0;
		hResult = MKL_LLD_QueryNames( pClientContext, &pDevices, &NamesLen );
		
		printf( "get names result 0x%x. len %d\n", hResult, NamesLen );
		if (SUCCEEDED( hResult ))
		{
			PWCHAR pwchDevice;
			ULONG EnumContext = 0;
			while (SUCCEEDED( MKL_LLD_Enum( pDevices, NamesLen, &EnumContext, &pwchDevice ) ))
			{
				MKLIF_LLD_INFO Info;
				MKLIF_LLD_GEOMETRY Geometry;
				char Buffer[0x1000 * 4];

				hResult = MKL_LLD_GetInfo( pClientContext, pwchDevice, &Info );

				printf( "'%S'. Get info result 0x%x (flags 0x%x)\n", pwchDevice, hResult, Info.m_Resolved );
				if (SUCCEEDED( hResult ))
				{
					if (Info.m_Resolved & _LLD_RESOLVED_TYPE)
						printf( "\ttype 0x%x (%d)\n", Info.m_Type, Info.m_Type );

					if (Info.m_Resolved & _LLD_RESOLVED_SECTOR_SIZE)
						printf( "\tsector size 0x%x (%d)\n", Info.m_SectorSize, Info.m_SectorSize );

					if (Info.m_Resolved & _LLD_RESOLVED_PARTITION_TYPE)
						printf( "\tpartitional type 0x%x (%d)\n", Info.m_PartitionalType, Info.m_PartitionalType );

					if (Info.m_Resolved & _LLD_RESOLVED_LENGTH)
						printf( "\tlength 0x%x-%x\n", Info.m_Length.HighPart, Info.m_Length.LowPart );
					

					if (Info.m_Flags & (
						_LLD_FLAG_IS_PARTITIONAL
						| _LLD_FLAG_IS_LOCKED
						| _LLD_FLAG_IS_WRITE_PROTECTED
						| _LLD_FLAG_IS_BOOTABLE
						| _LLD_FLAG_IS_RECOGNIZED
						| _LLD_FLAG_IS_REMOVABLE)
						)
					{
						printf( "\tFlags:" );
						if (Info.m_Flags & _LLD_FLAG_IS_PARTITIONAL)
							printf( " partitional" );
						if (Info.m_Flags & _LLD_FLAG_IS_REMOVABLE)
							printf( " removable" );
						if (Info.m_Flags & _LLD_FLAG_IS_LOCKED)
							printf( " Locked" );
						if (Info.m_Flags & _LLD_FLAG_IS_WRITE_PROTECTED)
							printf( " WriteProtected" );
						if (Info.m_Flags & _LLD_FLAG_IS_BOOTABLE)
							printf( " Bootable" );
						if (Info.m_Flags & _LLD_FLAG_IS_RECOGNIZED)
							printf( " Recognized" );

						printf( "\n" );
					}
				}

				hResult = MKL_LLD_GetGeometry( pClientContext, pwchDevice, &Geometry );
				printf( "\tGet geometry result 0x%x (media type 0x%x)\n", hResult, Geometry.m_MediaType );

				//+ check read
				if (Info.m_Resolved & _LLD_RESOLVED_SECTOR_SIZE
					&& Info.m_SectorSize < sizeof(Buffer))
				{
					ULONG Bytes = 0;
					hResult = MKL_LLD_Read( pClientContext, pwchDevice, 0, Buffer, Info.m_SectorSize, &Bytes );

					/*if (!lstrcmp( pwchDevice, L"\\Device\\Floppy0"))
					{
						hResult = MKL_LLD_Write( pClientContext, pwchDevice, 0, Buffer, Info.m_SectorSize, &Bytes );
						printf( "\twrite on floppy result 0x%x (bytes 0x%x)\n", hResult, Bytes );
					}*/

					printf( "\tCheck read result 0x%x (bytes 0x%x)\n", hResult, Bytes );
				}
				//- check read
			}
			
			pfMemFree( NULL, &pDevices );
		}
	}

	MKL_ClientUnregister( &pClientContext );

	return 0;
}

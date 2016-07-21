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

void
FidBox2_Check (
	PVOID pClientContext
	)
{
	HANDLE hEnum = NULL;
	WIN32_FIND_DATA FindFileData;

	int nErrors = 0;
	int nProcessed = 0;

	hEnum = FindFirstFile( L"*", &FindFileData );

	if (INVALID_HANDLE_VALUE == hEnum)
	{
		printf("enum failed\n");
		return;
	}

	do
	{
		if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes )
		{
			// skip
			printf("skip folder '%S'\n", FindFileData.cFileName );
		}
		else
		{
			HANDLE hFile = CreateFile (
				FindFileData.cFileName,
				0,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
				);

			if(INVALID_HANDLE_VALUE != hFile)
			{
				HRESULT hResult = S_OK;
				ULONG FidBox2Data;

				nProcessed++;

				hResult = MKL_GetFidbox2(
					pClientContext,
					hFile,
					&FidBox2Data
					);

				if (SUCCEEDED( hResult ))
					printf("'%S' - ok, data 0x%x)\n", FindFileData.cFileName, FidBox2Data );
				else
				{
					nErrors++;
					printf("'%S' - failed result 0x%x\n", FindFileData.cFileName, hResult );
				}

				CloseHandle( hFile );
			}
		}
	} while(FindNextFile( hEnum, &FindFileData ));
	FindClose( hEnum );

	printf("\tProcessed: %d\n\tErrors: %d\n", nProcessed, nErrors );
}

int _cdecl
main (
    __in int argc,
    __in_ecount(argc) char *argv[]
    )
{
    HRESULT hResult = S_OK;

	PVOID pClientContext;
	ULONG ApiVersion = 0;
	ULONG AppId = 0;

	hResult = MKL_ClientRegister(
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

	if (IS_ERROR( hResult))
	{
		printf( "register client failed. err 0x%x\n", hResult );
		return 1;
	}

	FidBox2_Check( pClientContext );

	MKL_ClientUnregister( &pClientContext );

	return 0;
}

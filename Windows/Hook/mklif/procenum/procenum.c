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
EnumProcesses (
	PVOID pClientContext
	)
{
	PVOID pProcessInfo = NULL;
	ULONG ProcessInfoLen = 0;

	HRESULT hResult = MKL_QueryProcessesInfo( pClientContext, &pProcessInfo, &ProcessInfoLen );
	if (SUCCEEDED(hResult) )
	{
		ULONG EnumContext = 0;
		ULONG ProcessId = 0;
		ULONG ParentProcessId = 0;
		LARGE_INTEGER StartTime;
		PWCHAR pwchImagePath = NULL;
		PWCHAR pwchCurrDir = NULL;
		PWCHAR pwchCmdLine = NULL;

		DWORD hash[4];

		while (TRUE)
		{
			hResult = MKL_EnumProcessInfo(
				pProcessInfo,
				ProcessInfoLen,
				&EnumContext,
				&ProcessId,
				&ParentProcessId,
				&StartTime,
				&pwchImagePath,
				&pwchCurrDir,
				&pwchCmdLine
				);

			if (!SUCCEEDED(hResult) )
				break;

			printf( "\tPid 0x%04x (%4d) '%S'\n", ProcessId, ProcessId, pwchImagePath );

			hResult = MKL_QueryProcessHash( pClientContext, ProcessId, hash, sizeof(hash) );
			if (SUCCEEDED( hResult ))
			{
				printf( "\thash ok 0x%x%x%x%x'\n", hash[0], hash[1], hash[2], hash[3]);
			}
			else
			{
				printf( "\thash fail. Error 0x%x'\n", hResult );
			}
			hResult = S_OK;
		}

		pfMemFree( NULL, &pProcessInfo );
	}
}

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

	EnumProcesses( pClientContext );

	MKL_ClientUnregister( &pClientContext );

	return 0;
}

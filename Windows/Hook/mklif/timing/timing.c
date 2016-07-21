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

	PVOID pClientContext;
	ULONG BytesRet = 0;
	__int8 Buffer[1024];

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

	if (IS_ERROR( hResult ))
	{
		printf( "register client failed. err 0x%x\n", hResult );
		return 1;
	}

	hResult = MKL_Timing_Get( pClientContext, Buffer, sizeof(Buffer), &BytesRet );
	printf( "get timing result 0x%x (ret bytes %d)\n\n", hResult, BytesRet );
	if (SUCCEEDED( hResult ))
	{
		// name		hookid	mj		proctype	callcount	time
		// pwchar	ulong	ulong	ulong		large_int	large_int
		ULONG sec;
		__int8* ptr = Buffer;
		ULONG pos = 0;
		while (pos < BytesRet)
		{
			PWCHAR pwchName;
			ULONG HookId, Mj, ProcessingType;
			LARGE_INTEGER CallCount, Time;
			
			pwchName = (PWCHAR)(ptr + pos);
			pos += (lstrlen( pwchName ) + 1) * sizeof(WCHAR);
			
			HookId = *(ULONG*)(ptr + pos);
			pos += sizeof(ULONG);

			Mj = *(ULONG*)(ptr + pos);
			pos += sizeof(ULONG);

			ProcessingType = *(ULONG*)(ptr + pos);
			pos += sizeof(ULONG);

			CallCount = *(LARGE_INTEGER*)(ptr + pos);
			pos += sizeof(LARGE_INTEGER);

			Time = *(LARGE_INTEGER*)(ptr + pos);
			pos += sizeof(LARGE_INTEGER);

			sec = (ULONG)(Time.QuadPart / 10000000L);

			wprintf( L"%d:%s:0x%x (%s) count %I64d time %I64d - sec %d\n",
				HookId,
				pwchName,
				Mj,
				ProcessingType ? L"post" : L"pre",
				CallCount,
				Time,
				sec
				);
		}
	}

	MKL_ClientUnregister( &pClientContext );

	return 0;
}

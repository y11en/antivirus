// GetCountStat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\mklapi\mklapi.h"

char* pClientContext;
ULONG ApiVersion;
ULONG AppId;

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
}

void __cdecl pfMemFree (
						PVOID pOpaquePtr,
						void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
}

HRESULT
Init()
{
	HRESULT hResult = S_OK;

	pClientContext=NULL;
	ApiVersion = 0;
	AppId = 0;

	hResult = MKL_ClientRegister (
		(PVOID*)&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if ( IS_ERROR( hResult ) )
		return hResult;


	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if ( IS_ERROR( hResult ) )
		return hResult;;

	hResult = MKL_GetAppId( pClientContext, &AppId );

	return hResult;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hResult = Init();
	if ( !SUCCEEDED(hResult) )
	{
		printf("Error init \n");
		return 0;
	}

	printf("Statistic: \n");
	DWORD delay = 1000;
	ULONGLONG prev_time = 0;
	ULONGLONG curr_time = 0;
	ULONGLONG prev_stat = 0;
	ULONGLONG curr_stat = 0;
	ULONG prc = 0;

	MKL_GetStatCounter( pClientContext,  &prev_stat, &prev_time );

	DebugBreak();
	while (1)
	{
		Sleep(delay);

		MKL_GetStatCounter( pClientContext,  &curr_stat, &curr_time );

		if ( (curr_stat - prev_stat) > (curr_time - prev_time) )
		{
			DebugBreak();
		}

		prc = (ULONG)(((float)(curr_stat - prev_stat)) / (curr_time-prev_time) * 100);
		prev_stat = curr_stat;
		prev_time = curr_time;
		printf("\r");
		printf("      ");
		printf("\r");
		printf( "%d%",prc );
	}

	return 0;
}

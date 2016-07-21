#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>


#include "../mklapi/mklapi.h"

BOOL gbStop = FALSE;

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
	ULONG DrvFlags = 0;
	ULONG AppId = 0;
	DWORD AdditionSite;

	PVOID pFilter;

	ULONG FltId1 = 0, FltId2 = 0, FltId3 = 0, FltId4 = 0, FltId5 = 0;

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

	// -
	hResult = MKL_AddFilter( &FltId1, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,  FLT_A_INFO | FLT_A_DISABLED,
		FLTTYPE_REGS, Interceptor_QueryValueKey, 0, 0, PreProcessing, NULL, NULL );

	hResult = MKL_AddFilter( &FltId2, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,  FLT_A_INFO | FLT_A_DISABLED,
		FLTTYPE_REGS, Interceptor_QueryValueKey, 0, 0, PreProcessing, NULL, NULL );

	hResult = MKL_AddFilter( &FltId3, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,  FLT_A_INFO | FLT_A_DISABLED,
		FLTTYPE_REGS, Interceptor_QueryValueKey, 0, 0, PreProcessing, NULL, NULL );

	AdditionSite = 0;
	hResult = MKL_AddFilter( &FltId4, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,  FLT_A_INFO | FLT_A_DISABLED,
		FLTTYPE_REGS, Interceptor_QueryValueKey, 0, 0, PreProcessing, &AdditionSite, NULL );

	AdditionSite = FltId1;
	hResult = MKL_AddFilter( &FltId5, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,  FLT_A_INFO | FLT_A_DISABLED,
		FLTTYPE_REGS, Interceptor_QueryValueKey, 0, 0, PreProcessing, &AdditionSite, NULL );

	printf( "Filter's id: %d %d %d %d %d\n", FltId1, FltId2, FltId3, FltId4, FltId5 );
	printf( "must be: %d %d %d %d %d\n", FltId4, FltId1, FltId5, FltId2, FltId3 );

	hResult = MKL_GetFilterFirst( pClientContext, &pFilter );
	while (SUCCEEDED( hResult ))
	{
		ULONG FilterId = 0;
		MKL_GetFilterInfo( pFilter, &FilterId, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
		printf( "Next filter %d\n", FilterId );

		pfMemFree( 0, &pFilter );
		hResult = MKL_GetFilterNext( pClientContext, FilterId, &pFilter );
	}

	MKL_DelAllFilters( pClientContext );
	MKL_ClientUnregister( &pClientContext );

	printf( "\tmemusage %d\n", gMemCounter );

	return 0;
}

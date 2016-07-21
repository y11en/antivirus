#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>
#include "../../mklapi/mklapi.h"

LONG gMemCounter = 0;

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	)
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );

	return ptr;
};

void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr
	)
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
	ULONG ApiVersion;
	ULONG AppId;

	WCHAR usNativePath[1024];
	ULONG nativeFilePathSize;
	WCHAR pwchPath[MAX_PATH];

	if (argc < 2)
		return -1;

	if (!MultiByteToWideChar( CP_ACP, 0, argv[1], -1, pwchPath, sizeof(pwchPath) / sizeof(WCHAR) ))
	{
		printf( "invalid path\n" );
		return -1;
	}

	pClientContext = NULL;
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

	if (IS_ERROR( hResult ))
	{
		printf( "MKL_ClientRegister Error, hResult=%d\n",hResult );
		return -1;
	}

	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if (IS_ERROR( hResult ))
		printf( "MKL_GetDriverApiVersion Error, hResult=%d\n",hResult );

	hResult = MKL_GetAppId( pClientContext, &AppId );
	if (IS_ERROR( hResult ))
		printf( "MKL_GetAppId Error, hResult=%d\n",hResult );

	nativeFilePathSize = sizeof(WCHAR)*1024;

	memset( usNativePath, 0, nativeFilePathSize );

	hResult = MKL_QueryFileNativePath( pClientContext, pwchPath, usNativePath, &nativeFilePathSize );
	if ( !SUCCEEDED(hResult) )
		printf( "MKL_QueryFileNativePath Error, hResult=%d\n", hResult );
	else
	{
		memset( (char*)usNativePath + nativeFilePathSize,0, 2 );
		printf( "\nNativePath '%S'", usNativePath );
	}
	
	MKL_ClientUnregister((PVOID*)&pClientContext);

	return 0;
}




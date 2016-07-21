#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

#include "../mklapi/mklapi.h"

LONG gMemCounter = 0;

void* MKL_PROC pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );
	
	return ptr;
};

void MKL_PROC pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};


DWORD WINAPI WorkingThread(void* pParam)
{
	HRESULT hResult = S_OK;
	PVOID pClientContext = pParam;

	PVOID pMessage;
	PMKLIF_EVENT_HDR pEventHdr;
	ULONG MessageSize;
	MKLIF_REPLY_EVENT Verdict;

	memset( &Verdict, 0, sizeof(Verdict) );

	while(TRUE)
	{
		PWSTR pwchName;
		PWSTR pwchPath;
		hResult = MKL_GetMultipleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, INFINITE );

		if (SUCCEEDED (hResult) )
		{
			PSINGLE_PARAM pParamTmp = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );

			if (pParamTmp)
			{
				/*printf( "\t(0x%x memusage %d)KeyName '%S\n", 
					GetCurrentThreadId(),
					gMemCounter,
					pParamTmp->ParamValue );*/
				
				/*OutputDebugString( L"\t" );
				OutputDebugString( (PWCHAR) pParamTmp->ParamValue );
				OutputDebugString( L"\n" );*/

			}
			
			hResult = MKL_GetProcessName( pClientContext, pMessage, &pwchName );
			if (SUCCEEDED (hResult) )
			{
				OutputDebugString( L"\t" );
				OutputDebugString( pwchName );
				OutputDebugString( L"\n" );
			
				pfMemFree( NULL, &pwchName );
			}

			hResult = MKL_GetProcessPath( pClientContext, pMessage, &pwchPath );
			if (SUCCEEDED (hResult) )
			{
				OutputDebugString( L"\t" );
				OutputDebugString( pwchPath );
				OutputDebugString( L"\n" );

				pfMemFree( NULL, &pwchPath );
			}

			hResult = MKL_ReplyMessage( pClientContext, pMessage, &Verdict );

			pfMemFree( NULL, &pMessage );
		}
		else
		{
			printf( "thread get message failed\n" );
			break;
		}

		hResult = MKL_GetMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr );
		if (!SUCCEEDED (hResult) )
		{
			printf( "wait next failed\n" );
			break;
		}
	}

	return 0;
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

	ULONG FltId = 0;

	DWORD dwThreadID1, dwThreadID2;
	HANDLE ThHandle1, ThHandle2;

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

	// -
	hResult = MKL_AddFilter( &FltId, pClientContext, "*", DEADLOCKWDOG_TIMEOUT, 
		FLT_A_INFO, FLTTYPE_REGS, Interceptor_QueryValueKey, 0,
		0, PreProcessing, NULL, NULL );

	if (SUCCEEDED( hResult ))
		printf( "Filter %d added\n", FltId );
	else
		printf( "Adding filter failed. err 0x%x\n", hResult );

	// -
	hResult = MKL_DelFilter( pClientContext, FltId );

	if (SUCCEEDED( hResult ))
		printf( "Filter %d deleted\n", FltId );
	else
		printf( "Del filter failed. err 0x%x\n", hResult );

	// -
	hResult = MKL_AddFilter( &FltId, pClientContext, "*", DEADLOCKWDOG_TIMEOUT, 
		FLT_A_INFO, FLTTYPE_REGS, Interceptor_QueryValueKey, 0,
		0, PreProcessing, NULL, NULL );

	if (SUCCEEDED( hResult ))
		printf( "Filter %d readded\n", FltId );
	else
		printf( "Readding filter failed. err 0x%x\n", hResult );

	hResult = MKL_ChangeClientActiveStatus( pClientContext, TRUE );

	if (SUCCEEDED( hResult ))
	{
		PVOID pMessage;
		PMKLIF_EVENT_HDR pEventHdr;
		ULONG MessageSize;
		MKLIF_REPLY_EVENT Verdict;

		memset( &Verdict, 0, sizeof(Verdict) );

		while (TRUE)
		{
			hResult = MKL_GetMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr );
			if (SUCCEEDED(hResult) )
			{
				// processing
				
				hResult = MKL_ReplyMessage( pClientContext, pMessage, &Verdict );

				pfMemFree( NULL, &pMessage );
			}
		}
	}

	MKL_DelAllFilters( pClientContext );

	MKL_ChangeClientActiveStatus( pClientContext, FALSE );
	MKL_ClientUnregister( &pClientContext );

	return 0;
}

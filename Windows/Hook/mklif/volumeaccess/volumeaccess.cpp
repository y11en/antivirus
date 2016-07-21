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
	ULONG FltId;

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
		wprintf( L"register client failed. err 0x%x\n", hResult );
		return 1;
	}

	hResult = MKL_AddFilter( &FltId, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,  FLT_A_POPUP,
		FLTTYPE_SYSTEM, MJ_SYSTEM_VOLUME_ACCESS, 0, 0, PreProcessing, NULL, NULL );

	MKL_BuildMultipleWait( pClientContext, 1 );
	MKL_ChangeClientActiveStatus( pClientContext, TRUE );

	PVOID pMessage = NULL;
	PMKLIF_EVENT_HDR pEventHdr;
	ULONG MessageSize;

	BOOL bStop = FALSE;
	MKLIF_REPLY_EVENT ReplyEvent;
	memset( &ReplyEvent, 0, sizeof(ReplyEvent) );
	ReplyEvent.m_VerdictFlags = efVerdict_Allow;

	do
	{
		hResult = MKL_GetSingleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, INFINITE, &bStop );
		if (SUCCEEDED( hResult ))
		{
			PSINGLE_PARAM pParamFN = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
			hResult = MKL_ReplyMessage( pClientContext, pMessage, &ReplyEvent );
			
			wprintf( L"process %d (0x%x)volume access '%s' to '%S'\n",
				pEventHdr->m_ProcessId,
				pEventHdr->m_ProcessId,
				pParamFN->ParamValue
				);

			pfMemFree( NULL, &pMessage );
		}
	} while (SUCCEEDED( hResult ));

	MKL_DelAllFilters( pClientContext );
	MKL_ClientUnregister( &pClientContext );

	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

#include "../mklapi/mklapi.h"

void* MKL_PROC pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
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
};

int _cdecl
main (
    __in int argc,
    __in_ecount(argc) char *argv[]
    )
{
    HRESULT hResult = S_OK;
	PVOID pClientContext = NULL;
	ULONG FltId;

	hResult = MKL_ClientRegister (
		&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		NULL
		);

	if (IS_ERROR( hResult))
	{
		printf( "register client failed. err 0x%x\n", hResult );
		return 1;
	}

	hResult = MKL_AddInvisibleThread( pClientContext ); 

	hResult = MKL_AddFilter (
		&FltId,
		pClientContext,
		"*",
		DEADLOCKWDOG_TIMEOUT, 
		FLT_A_POPUP | FLT_A_USECACHE | FLT_A_SAVE2CACHE,
		FLTTYPE_NFIOR,
		IRP_MJ_SET_INFORMATION,
		FileLinkInformation,
		0,
		PreProcessing,
		NULL,
		NULL
		);

	hResult = MKL_BuildMultipleWait( pClientContext, 1 );
	hResult = MKL_ChangeClientActiveStatus( pClientContext, TRUE );

	if (SUCCEEDED( hResult ))
	{
		BOOL bStop = FALSE;
		PVOID pMessage = NULL;
		PMKLIF_EVENT_HDR pEventHdr;
		ULONG MessageSize;
	
		MKLIF_REPLY_EVENT ReplyEvent;
		MKLIF_REPLY_EVENT Verdict;

		memset( &ReplyEvent, 0, sizeof(ReplyEvent) );
		ReplyEvent.m_VerdictFlags = efVerdict_Pending;
		ReplyEvent.m_ExpTime = 3;

		memset( &Verdict, 0, sizeof(Verdict) );
		Verdict.m_VerdictFlags = efVerdict_Allow;

		while (TRUE)
		{
			hResult = MKL_GetSingleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, INFINITE, &bStop );
			if (SUCCEEDED( hResult ))
			{
				PSINGLE_PARAM pParamFN = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
				PSINGLE_PARAM pParamDest = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_DEST_W, FALSE );

				hResult = MKL_ReplyMessage( pClientContext, pMessage, &ReplyEvent );
				hResult = MKL_SetVerdict( pClientContext, pMessage, &Verdict );

				printf("create hardlink from '%S' to '%S'\n", pParamFN->ParamValue, pParamDest->ParamValue );

				pfMemFree( NULL, &pMessage );
			}
		}
	}

	MKL_ChangeClientActiveStatus( pClientContext, FALSE );
	MKL_ClientUnregister( &pClientContext );

	return 0;
}

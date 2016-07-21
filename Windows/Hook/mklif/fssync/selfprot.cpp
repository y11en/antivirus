#include "selfprot.h"
#include "../mklapi/mklapi.h"

void* __cdecl pfFSSYNC_MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );

	return ptr;
};

void __cdecl pfFSSYNC_MemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;
};

// -
typedef struct _sSelfProtContext
{
	ULONG				m_Slot;
	PVOID				m_pClientContext;
	_tpfProactivNotify	m_pfnCallback;
	PVOID				m_pUserContext;
	HANDLE				m_hThread;
}SelfProtContext, *PSelfProtContext;

DWORD
WINAPI
ThreadProc (
	LPVOID lpParam
	) 
{
	PSelfProtContext pSelf = (PSelfProtContext) lpParam;
	PVOID pClientContext = pSelf->m_pClientContext;

	PVOID pMessage = NULL;
	ULONG MessageSize = 0;
	PMKLIF_EVENT_HDR pEventHdr = NULL;

	MKLIF_REPLY_EVENT Reply;
	memset( &Reply, 0, sizeof(Reply) );

	SelfProtectionEvent SPEvent;

	BOOL bSkip;
	BOOL bSystemAccount;
	while (TRUE)
	{
		HRESULT hResult = MKL_GetSingleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, INFINITE, NULL );
		if (!SUCCEEDED( hResult ))
			break;

		memset( &SPEvent, 0, sizeof(SPEvent) );
		SPEvent.m_ProcessId = pEventHdr->m_ProcessId;
		SPEvent.m_SP_ProcessId = GetCurrentProcessId();

		bSkip = FALSE;
		bSystemAccount = FALSE;

		PSINGLE_PARAM pParamLuid = MKL_GetEventParam( pMessage, 0, _PARAM_OBJECT_LUID, FALSE );
		if (pParamLuid && sizeof(LUID) == pParamLuid->ParamSize)
		{
			PLUID pLuid = (PLUID) pParamLuid->ParamValue;
			LUID SystemLuid = SYSTEM_LUID;
			if (!memcmp( &SystemLuid, pLuid, sizeof(LUID) ))
				bSystemAccount = TRUE;
		}

		switch (pEventHdr->m_HookID)
		{
		case FLTTYPE_SYSTEM:
			switch (pEventHdr->m_FunctionMj)
			{
			case MJ_SYSTEM_OPEN_PROCESS:
				{
					SPEvent.m_ResourceType = eSPA_Process;
					
					PSINGLE_PARAM pAccessAttrib = MKL_GetEventParam( pMessage, 0, _PARAM_OBJECT_ACCESSATTR, FALSE );
					if (pAccessAttrib && sizeof(ULONG) == pAccessAttrib->ParamSize)
					{
						ULONG Access = *(ULONG*) pAccessAttrib->ParamValue;
						if (Access & PROCESS_TERMINATE)
						{
							bSkip = FALSE;
							SPEvent.m_Flags = _fSelfProtection_AccessFor | _fSelfProtection_Terminate;
						}
						else if (Access & (PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME))
						{
							bSkip = FALSE;
							SPEvent.m_Flags = _fSelfProtection_AccessFor | _fSelfProtection_Modify;
						}
						else if (Access & (PROCESS_SET_INFORMATION | PROCESS_SET_QUOTA |
							PROCESS_VM_OPERATION | PROCESS_DUP_HANDLE))
						{
							bSkip = FALSE;
							SPEvent.m_Flags = _fSelfProtection_AccessFor | _fSelfProtection_SetInfo;
						}
						/*else if (Access & PROCESS_QUERY_INFORMATION)
						{
							bSkip = FALSE;
							SPEvent.m_Flags = _fSelfProtection_AccessFor | _fSelfProtection_QueryInfo;
						}*/
					}
				}
				break;
			
			case MJ_SYSTEM_TERMINATE_PROCESS:
				bSkip = FALSE;
				SPEvent.m_ProcessId = pEventHdr->m_ProcessId;
				SPEvent.m_Flags = _fSelfProtection_Terminate;
				SPEvent.m_ResourceType = eSPA_Process;
				break;
			
			default:
				break;
			}
			break;

		default:
			break;
		}
		
		Reply.m_VerdictFlags = efVerdict_Allow;
		if (!bSkip)
		{
			if ( !pSelf->m_pfnCallback( pSelf->m_pUserContext, 0, &SPEvent ))
				Reply.m_VerdictFlags = efVerdict_Deny;
		}

		MKL_ReplyMessage( pClientContext, pMessage, &Reply );

		MKL_FreeSingleMessage( pClientContext, &pMessage );
	}

	return 0;
}
 
BOOL
SelfProt_StartThread (
	PVOID pClientContext,
	_tpfProactivNotify pfnCallback,
	PVOID pUserContext
	)
{
	PSelfProtContext pSelf = NULL;

	ULONG Slot;
	HRESULT hResult = MKL_Slot_Allocate( pClientContext, &Slot, sizeof(SelfProtContext), (PVOID*) &pSelf );
	if (!SUCCEEDED( hResult ))
		return FALSE;

	if (Slot)
	{
		MKL_Slot_Free( pClientContext, 0 );
		return FALSE;
	}

	DWORD ThreadId;
	pSelf->m_Slot = Slot;
	pSelf->m_pClientContext = pClientContext;
	pSelf->m_pfnCallback = pfnCallback;
	pSelf->m_pUserContext = pUserContext;
	pSelf->m_hThread = CreateThread( NULL, 0, ThreadProc, pSelf, 0, &ThreadId );
	if (!pSelf->m_hThread)
	{
		MKL_Slot_Free( pClientContext, 0 );
		return FALSE;
	}

	ULONG FilterId = 0;

	BYTE Buf[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM pParam = (PFILTER_PARAM) Buf;
	pParam->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	pParam->m_ParamFlt = FLT_PARAM_EUA;
	pParam->m_ParamID = _PARAM_OBJECT_DEST_ID;
	*(ULONG*)pParam->m_ParamValue = (DWORD) GetCurrentProcessId();
	pParam->m_ParamSize = sizeof(ULONG);

	MKL_AddFilter( &FilterId, pClientContext, NULL, 0, FLT_A_POPUP, 
		FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PreProcessing, NULL, pParam, NULL );

	MKL_AddFilter( &FilterId, pClientContext, NULL, 0, FLT_A_POPUP, 
		FLTTYPE_SYSTEM, MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, NULL, pParam, NULL );

	return TRUE;
}

void
SelfProt_StopThread (
	PVOID pClientContext
	)
{
	PSelfProtContext pSelf = NULL;
	HRESULT hResult = MKL_Slot_Get( pClientContext, 0, (PVOID*) &pSelf );

	MKL_ClientBreakConnection( pClientContext );

	if (SUCCEEDED( hResult ))
	{
		WaitForSingleObject( pSelf->m_hThread, INFINITE );
		CloseHandle( pSelf->m_hThread );
		MKL_Slot_Free( pClientContext, 0 );
	}
}

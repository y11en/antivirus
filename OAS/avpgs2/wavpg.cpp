#include "wavpg.h"
#include "mklavpg.h"
#include "klavpg.h"

#include <windows.h>
#include "../../windows/hook/hook/avpgcom.h"
#include "../../windows/hook/hook/fssync.h"

#pragma comment(lib, "fssync.lib")

tPROPID propid_EventDefinition = 0;

#define _DOS_NAME_LEN 4

//+ common functions
LONG gMemCounter = 0;

int __cdecl
Hash_Compare (
	void* pContext,
	void* pData1,
	void* pData2,
	int DataLen
	)
{
	return memcmp( (const char*) pData1, (const char*) pData2, DataLen );	
}

int __cdecl
Volume_Compare (
	void* pContext,
	void* pData1,
	void* pData2,
	int DataLen
	)
{
	WCHAR wch1 = ((PWCHAR)pData1)[DataLen / 2 - 2];
	WCHAR wch2 = ((PWCHAR)pData2)[DataLen / 2 - 2];
	if (wch1 != wch2)
	{
		if (wch1 > wch2)
			return 1;
		return -1;
	}
	
	return memcmp( (const char*) pData1, (const char*) pData2, DataLen );	
}


void* __cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	)
{	
	hOBJECT pPragueContext = (hOBJECT) pOpaquePtr;

	tPTR ptr = NULL;
	tERROR error = pPragueContext->heapAlloc( &ptr, size );
	if (PR_SUCC( error ))
	{
		InterlockedIncrement( &gMemCounter );
		return ptr;
	}

	return NULL;
};

void __cdecl
MemFree (
	PVOID pOpaquePtr,
	void** pptr
	)
{
	hOBJECT pPragueContext = (hOBJECT) pOpaquePtr;

	if (*pptr)
	{
		pPragueContext->heapFree( *pptr );
		*pptr = NULL;

		InterlockedDecrement( &gMemCounter );
	}
};

void
FillParamNOP (
	PFILTER_PARAM Param,
	ULONG ParamId,
	DWORD ParamFlags
	)
{
	Param->m_ParamID = ParamId;
	Param->m_ParamFlags = ParamFlags;
	Param->m_ParamFlt = FLT_PARAM_NOP;
	Param->m_ParamSize = 0;
}

void
AnswerMessage (
	hOBJECT pPragueContext,
	PVOID pDrvContext,
	PVOID *ppMessage,
	BOOL bAllow,
	BOOL bCacheable,
	ULONG ExpTime
	)
{
	HRESULT hResult = DRV_SetVerdict( pDrvContext, *ppMessage, bAllow, bCacheable, ExpTime );
	if (!SUCCEEDED( hResult ))
	{
		PR_TRACE(( g_root, prtERROR, TR "answer message error 0x%x", hResult ));
	}

	MemFree( pPragueContext, ppMessage );
}

void
CheckDelayed (
	cContext* pContext
	)
{
	while (true)
	{
		cDelayedContext* pDelayedContext = pContext->m_EvQueue.Next();
		if (!pDelayedContext)
			break;

		PR_TRACE(( g_root, prtNOTIFY, TR "next delayed context 0x%x", pDelayedContext ));

		cString* hCtx = NULL;
		tERROR error = pContext->m_pPragueContext->sysCreateObjectQuick((hOBJECT*) &hCtx, IID_STRING, PID_ANY, 0);
		if (PR_SUCC( error ))
		{
			hCtx->SetCP( cCP_UNICODE );
			hCtx->propSetDWord( propid_EventProcessID, pDelayedContext->m_ProcessId );

			if (OID_AVP3_BOOT == pDelayedContext->m_Origin)
			{
				PR_TRACE(( g_root, prtERROR, TR "delayed boot check" ));
				tDWORD dwPropSize;
				hCtx->propSet( &dwPropSize, propid_CustomOrigin, &pDelayedContext->m_Origin, sizeof(pDelayedContext->m_Origin) );
			}

			if (propid_Background)
				hCtx->propSetBool( propid_Background, cTRUE );

			error = hCtx->ImportFromBuff (
				NULL,
				pDelayedContext->m_ObjectName,
				pDelayedContext->m_NameLength,
				cCP_UNICODE,
				cSTRING_Z
				);

			if (PR_SUCC( error ))
			{
				SetThreadToken( NULL, pDelayedContext->m_Impersonate );

				tDWORD Verdict = cAVPG2_RC_ALLOW_ACCESS;
				tDWORD blen = sizeof(Verdict);

				error = pContext->m_pPragueContext->sysSendMsg (
					pmc_AVPG2,
					pm_AVPG2_PROCEED_OBJECT,
					(hOBJECT) *hCtx,
					&Verdict,
					&blen
					);

				PR_TRACE(( g_root, prtERROR, TR "proceed delayed 0x%x answer %terr, verdict 0x%x",
					pDelayedContext, error, Verdict ));

				SetThreadToken( NULL, NULL );
			}

			hCtx->sysCloseObject();
		}

		delete pDelayedContext;
	}
}

//+ Callbacks
tERROR
pr_call
ThreadCallback (
	tThreadCallType CallType,
	tPTR pCommonThreadContext,
	tPTR* ppThreadContext
	)
{
	tERROR err_ret = errOK;
	HRESULT hResult = E_FAIL;

	cContext* pContext = (cContext*) pCommonThreadContext;

	switch (CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		{
			PR_TRACE(( g_root, prtERROR, TR "init thread context" ));

			*ppThreadContext = pCommonThreadContext;
			DRV_RegisterInvisibleThread( pContext->m_pDrvContext );

			if (pContext->m_bMessageQueueInited)
				*ppThreadContext = pContext;
			else
			{
				PR_TRACE(( g_root, prtERROR, TR "initing queue..." ));
				hResult = DRV_PrepareMessageQueue( pContext->m_pDrvContext, 1 );
				if (SUCCEEDED( hResult ))
				{
					pContext->m_bMessageQueueInited = true;
					*ppThreadContext = pContext;
				}
				else
					err_ret = errUNEXPECTED;

				PR_TRACE(( g_root, prtERROR, TR "init queue result 0x%x", hResult ));
			}
		}
		if (PR_SUCC( err_ret ))
		{
			pContext->m_pPragueContext->sysSendMsg (
				pmc_AVPG2,
				pm_AVPG2_NOTIFY_THREAD_STARTED,
				NULL,
				NULL,
				NULL
				);
		}
		break;

	case TP_CBTYPE_THREAD_YIELD:
		PR_TRACE(( g_root, prtNOTIFY, TR "TP_CBTYPE_THREAD_YIELD" ));
		CheckDelayed( pContext );
		break;

	case TP_CBTYPE_THREAD_DONE_CONTEXT:
		PR_TRACE(( g_root, prtERROR, TR "done thread context" ));
		
		pContext->m_pPragueContext->sysSendMsg (
			pmc_AVPG2,
			pm_AVPG2_NOTIFY_THREAD_STOPPED,
			NULL,
			NULL,
			NULL
			);
		break;
	}

	return err_ret;
}

bool
IsTrusted (
	cContext* pContext,
	PVOID pMessage
	)
{
	ULONG ProcessId = 0;
	DRV_GetEventInfo( pMessage, NULL, NULL, NULL, NULL, NULL, &ProcessId );

	if (pContext->m_TrustedProcesses.IsTrusted( ProcessId ))
	{
		PR_TRACE(( g_root, prtIMPORTANT, TR "skip trusted process %d request", ProcessId ));
		return true;
	}

	return false;
}

bool
IsModifiedObject (
	PVOID pMessage
	)
{
	PSINGLE_PARAM pSingleContextFlags = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_CONTEXT_FLAGS, sizeof(ULONG) );
	if (!pSingleContextFlags)
		return false;

	ULONG ContextFlags = *(ULONG*) pSingleContextFlags->ParamValue;

	if ((_CONTEXT_OBJECT_FLAG_FORWRITE | _CONTEXT_OBJECT_FLAG_MODIFIED) & ContextFlags)
		return true;

	return false;
}

tERROR
pr_call
WorkCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
	HRESULT hResult = E_FAIL;

	cContext* pContext = (cContext*) pThreadContext;

	cThreadPool *pThPoolWork = pContext->m_pThPoolWork;
	hOBJECT pPragueContext = pContext->m_pPragueContext;
	PVOID m_pDrvContext = pContext->m_pDrvContext;

	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;
	case TP_CBTYPE_TASK_PROCESS:
		if (!TaskDataLen)
		{
			// основной выгребающий поток
			PR_TRACE(( g_root, prtERROR, TR "main thread" ));

			PVOID pMessage = NULL;

			while (true)
			{
				pMessage = NULL;
				hResult = DRV_PrepareMessage( m_pDrvContext, &pMessage );
				if (!SUCCEEDED( hResult ))
				{
					PR_TRACE(( g_root, prtERROR, TR "prepare error 0x%x", hResult ));
					break;
				}
				else
				{
					ULONG MessageSize = 0;
					PVOID pEventHdr = NULL;
					hResult = DRV_GetMessage( m_pDrvContext, &pMessage, &MessageSize, &pEventHdr );
					if (!SUCCEEDED( hResult ))
					{
						PR_TRACE(( g_root, prtERROR, TR "get error 0x%x", hResult ));
						MemFree( pContext->m_pPragueContext, &pMessage );
						break;
					}
					else
					{
						DRV_PendingMessage( m_pDrvContext, pMessage, 3 );

						if (IsTrusted( pContext, pMessage ))
							AnswerMessage( pPragueContext, m_pDrvContext, &pMessage, TRUE, FALSE, 0 );
						else
						{
							tTaskId TaskId;
							tERROR error = pThPoolWork->AddTask (
								&TaskId,
								WorkCallback,
								&pMessage,
								sizeof(pMessage),
								TP_THREADPRIORITY_NORMAL
								);

							if (!PR_SUCC( error ))
							{
								PR_TRACE(( g_root, prtERROR, TR "add message error 0x%x", error ));
								AnswerMessage( pPragueContext, m_pDrvContext, &pMessage, TRUE, FALSE, 0 );
							}
						}
					}
				}
			}

			PR_TRACE(( g_root, prtERROR, TR "main thread exit" ));
		}
		else
		{
			// обрабатывающий события поток
			PVOID pMessage = *(PVOID*) pTaskData;
			PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "proceed event 0x%x", pMessage ));
			pContext->m_pfProceed( pContext, pMessage );
		}
		break;
	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}

//- Callbacks
// --------------------------------------------------------

bool
CreateThreadPool (
	hOBJECT pPragueContext,
	const char *pcName,
	cThreadPool** ppThPool,
	tDWORD MaxLen,
	tThreadPriority Priority,
	tDWORD MinThreads,
	tDWORD MaxThreads,
	tDWORD IdleCount,
	tPTR pContext,
	tPTR pCallback
	)
{
	tERROR error = pPragueContext->sysCreateObject( (hOBJECT*)ppThPool, IID_THREADPOOL, PID_THPOOLIMP );

	if (PR_FAIL( error ))
	{
		*ppThPool = 0;
		return false;
	}

	(*ppThPool)->set_pgTP_TASK_QUEUE_MAX_LEN( MaxLen );

	(*ppThPool)->set_pgTP_DEFAULT_THREAD_PRIORITY( Priority );
	(*ppThPool)->set_pgTP_MIN_THREADS( MinThreads );
	(*ppThPool)->set_pgTP_MAX_THREADS( MaxThreads );	// +one for main thread

	(*ppThPool)->set_pgTP_ALLOW_TERMINATING( cTRUE );
	(*ppThPool)->set_pgTP_TERMINATE_TIMEOUT( 30000 );

	(*ppThPool)->set_pgTP_QUICK_EXECUTE( cTRUE );
	(*ppThPool)->set_pgTP_QUICK_DONE( cFALSE );

	(*ppThPool)->set_pgTP_MAX_IDLE_COUNT( IdleCount );
	(*ppThPool)->set_pgTP_INVISIBLE_FLAG( cTRUE );
	(*ppThPool)->set_pgTP_YIELD_TIMEOUT( 5000 );

	(*ppThPool)->set_pgTP_THREAD_CALLBACK( (tThreadCallback) pCallback );
	(*ppThPool)->set_pgTP_THREAD_CALLBACK_CONTEXT( pContext );
	
	(*ppThPool)->set_pgTP_NAME( (tPTR)pcName, lstrlenA( pcName ) + 1 );

	error = (*ppThPool)->sysCreateObjectDone();
	if (PR_FAIL(error))
	{
		(*ppThPool)->sysCloseObject();
		*ppThPool = 0;
		
		return false;
	}

	return true;
}

cContext::cContext (
	hOBJECT pPragueContext,
	tDWORD ThreadMaxCount
	) : m_pPragueContext( pPragueContext ),
		m_EvQueue( pPragueContext ),
		m_Users( pPragueContext ),
		m_TrustedProcesses( pPragueContext ),
		m_ProcessingList( pPragueContext )
{
	m_AvpgImp = NULL;
	m_pfProceed = NULL;
	m_pDrvContext = NULL;
	m_pThPoolWork = NULL;
	m_pThPoolDelayed = NULL;
	m_bMessageQueueInited = false;

	if (ThreadMaxCount < 3)
		ThreadMaxCount = 3;

	m_ThreadMaxCount = ThreadMaxCount;

	m_pVolumeNamesCache = _Hash_InitNew( pPragueContext, MemAlloc, MemFree, Volume_Compare, 8 );
	InitializeCriticalSection( &m_SyncVolume );
}
cContext::~cContext (
	)
{
	if (m_pThPoolWork) 
	{
		m_pThPoolWork->sysCloseObject();
		m_pThPoolWork = NULL;
	}
	if (m_pThPoolDelayed)
	{
		m_pThPoolDelayed->sysCloseObject();
		m_pThPoolDelayed = NULL;
	}

	if (m_pDrvContext)
		DRV_UnRegister( &m_pDrvContext );

	VolumeCache_Clean();
	DeleteCriticalSection( &m_SyncVolume );
	_Hash_Done( m_pVolumeNamesCache );
	m_pVolumeNamesCache = NULL;
}

bool
cContext::Init( )
{
	tERROR error = CreateThreadPool (
		m_pPragueContext,
		"ThPoolDelayed",
		&m_pThPoolDelayed,
		0,
		TP_THREADPRIORITY_TIME_CRITICAL,
		1,
		1,
		-1,
		this,
		ThreadCallback
		);

	if (PR_SUCC( error ))
	{
		error = CreateThreadPool (
			m_pPragueContext,
			"ThPoolWork",
			&m_pThPoolWork,
			0,
			TP_THREADPRIORITY_TIME_CRITICAL,
			3,
			m_ThreadMaxCount + 1,
			-1,
			this,
			ThreadCallback
			);
	
		if (!PR_SUCC( error ))
		{
			m_pThPoolDelayed->sysCloseObject();
			m_pThPoolDelayed = NULL;
			m_pThPoolWork = NULL;
			return false;
		}
	}
	

	tTaskId TaskId;

	PR_TRACE(( g_root, prtERROR, TR "initing first task!" ));
	if (m_pThPoolWork)
	{
		error = m_pThPoolWork->AddTask( &TaskId, WorkCallback, NULL, 0, TP_THREADPRIORITY_NORMAL );
		if (PR_SUCC( error ))
			return true;
	}

	return false;
}

void
replace_one (
	cString* hctx,
	PWCHAR pwchStr,
	ULONG StrLen,
	PWCHAR pwchSubstStr
	)
{
	tSTR_RANGE range;
	tERROR error = hctx->FindBuff (
		&range,
		cSTRING_WHOLE_LENGTH,
		pwchStr,
		StrLen,
		cCP_UNICODE,
		fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_FIND_FORWARD
		);

	if (PR_SUCC( error ))
		hctx->ReplaceBuff( range, pwchSubstStr, lstrlenW( pwchSubstStr ) + 1, cCP_UNICODE, NULL );
}

typedef struct _tsVolumeObject
{
	PWCHAR	m_pwchVolume;
	ULONG	m_VolumeLen;
	WCHAR	m_pwchDosName[_DOS_NAME_LEN];
}sVolumeObject, *PsVolumeObject;

HRESULT
cContext::VolumeCache_Get (
	PWCHAR pwchVolume,
	ULONG VolumeNameLen,
	PWCHAR pwchDosName
	)
{
	HRESULT hResult = E_FAIL;

	EnterCriticalSection( &m_SyncVolume );
	if (m_pVolumeNamesCache)
	{
		PHashTreeItem pHashItem = _Hash_Find (
			m_pVolumeNamesCache,
			pwchVolume,
			VolumeNameLen
			);

		if (pHashItem)
		{
			PsVolumeObject pVolume = (PsVolumeObject) pHashItem->m_pUserData;
			memcpy( pwchDosName, pVolume->m_pwchDosName, sizeof(pVolume->m_pwchDosName) );
			hResult = S_OK;
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, TR "volume '%S' not found in cache", pwchVolume ));
		}
	}

	LeaveCriticalSection( &m_SyncVolume );

	return hResult;
}

void
cContext::VolumeCache_Save (
	PWCHAR pwchVolume,
	ULONG VolumeNameLen,
	PWCHAR pwchDosName
	)
{
	EnterCriticalSection( &m_SyncVolume );
	if (m_pVolumeNamesCache)
	{
		PHashTreeItem pHashItem = _Hash_Find( m_pVolumeNamesCache, pwchVolume, VolumeNameLen );

		if (!pHashItem)
		{
			PsVolumeObject pVolume = (PsVolumeObject) MemAlloc( m_pPragueContext, sizeof(sVolumeObject), 0);
			if (pVolume)
			{
				pVolume->m_pwchVolume = (PWCHAR) MemAlloc( m_pPragueContext, VolumeNameLen, 0 );
				if (pVolume->m_pwchVolume)
				{
					memcpy( pVolume->m_pwchDosName, pwchDosName, sizeof(pVolume->m_pwchDosName) );
					memcpy( pVolume->m_pwchVolume, pwchVolume, VolumeNameLen );
					pVolume->m_VolumeLen = VolumeNameLen;

					pHashItem = _Hash_AddItem( m_pVolumeNamesCache, pVolume->m_pwchVolume, pVolume->m_VolumeLen, pVolume );
				}
				
				if (pHashItem)
				{
					PR_TRACE(( g_root, prtIMPORTANT, TR "cache volume name '%S' as '%S'", 
						pVolume->m_pwchVolume,
						pVolume->m_pwchDosName
						));
				}
				else
				{
					MemFree( m_pPragueContext, (void**) &pVolume );
				}
			}
		}
	}

	LeaveCriticalSection( &m_SyncVolume );
}

void
cContext::VolumeCache_Clean (
	)
{
	PR_TRACE(( g_root, prtERROR, TR "clean cache volumes" ));

	EnterCriticalSection( &m_SyncVolume );
	if (m_pVolumeNamesCache)
	{
		while (true)
		{
			PHashTreeItem pHashItem = _Hash_GetFirst( m_pVolumeNamesCache );
			if (!pHashItem)
				break;

			PsVolumeObject pVolume = (PsVolumeObject) pHashItem->m_pUserData;
			MemFree( m_pPragueContext, (void**)&pVolume->m_pwchVolume );
			MemFree( m_pPragueContext, (void**)&pVolume );

			_Hash_DelItem( m_pVolumeNamesCache, pHashItem );
		}
	}
	LeaveCriticalSection( &m_SyncVolume );
}

void
ReparseVolume (
	cContext* pContext,
	cString* hctx,
	PVOID pDrvContext,
	PVOID pMessage
	)
{
	PSINGLE_PARAM pSingleVolumeName = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_VOLUME_NAME_W, 0 );
	if (!pSingleVolumeName)
		return;

	PWCHAR pwchVolume = (PWCHAR) pSingleVolumeName->ParamValue;

	WCHAR dosname[_DOS_NAME_LEN];
	HRESULT hResult = pContext->VolumeCache_Get( pwchVolume, pSingleVolumeName->ParamSize, dosname );

	if (!SUCCEEDED( hResult ))
	{
		memset( dosname, 0, sizeof(dosname) );
		hResult = DRV_GetVolumeName( pDrvContext, pwchVolume, dosname, sizeof(dosname) / sizeof(WCHAR) );
	
		if (!SUCCEEDED( hResult ))
		{
			#define BUFSIZE 512
			WCHAR szTemp[BUFSIZE];
			szTemp[0] = '\0';

			if (GetLogicalDriveStringsW( BUFSIZE-1, szTemp )) 
			{
				WCHAR szName[MAX_PATH];
				WCHAR szDrive[3] = L" :";
				PWCHAR p = szTemp;
				do 
				{
					*szDrive = *p;
					dosname[0] = *p;

					if (QueryDosDeviceW( szDrive, szName, BUFSIZE ))
					{
						if (!lstrcmpW( pwchVolume, szName ))
						{
							dosname[1] = ':';
							dosname[2] = '\\';
							dosname[3] = 0;
							hResult = S_OK;

							PR_TRACE(( g_root, prtERROR, TR "altername name found '%S'", dosname ));
						}
					} while (!SUCCEEDED( hResult ) && *p++);
				} while (!SUCCEEDED( hResult ) && *p);
			}
		}

		if (SUCCEEDED( hResult ))
			pContext->VolumeCache_Save( pwchVolume, pSingleVolumeName->ParamSize, dosname );
	}

	if (SUCCEEDED( hResult ))
	{
		replace_one( hctx, (PWCHAR) pSingleVolumeName->ParamValue, fSTRING_COMPARE_CASE_INSENSITIVE, dosname);

		PR_TRACE(( g_root, prtNOT_IMPORTANT,  TR "reparse volume '%S' to '%S'", pSingleVolumeName->ParamValue, dosname ));
		
		CALL_SYS_PropertySet( hctx,
			0,
			propid_DosDeviceName,
			(LPWSTR) pSingleVolumeName->ParamValue,
			pSingleVolumeName->ParamSize
			);
	}
}

bool
FillObjectName (
	cContext* pContext,
	cString* hctx,
	PVOID pDrvContext,
	PVOID pMessage
	)
{
	PSINGLE_PARAM pSingleObjectName = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_URL_W, 0 );

	ULONG ContextFlags = 0;

	tERROR error;

	error = hctx->ImportFromBuff (
		NULL,
		pSingleObjectName->ParamValue,
		pSingleObjectName->ParamSize,
		cCP_UNICODE,
		cSTRING_Z
		);

	if (!PR_SUCC(error))
		return false;

	if (PR_SUCC(error))
		ReparseVolume( pContext, hctx, pDrvContext, pMessage );

	PSINGLE_PARAM pSingleContextFlags = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_CONTEXT_FLAGS, sizeof(ULONG) );
	if (pSingleContextFlags)
	{
		ContextFlags = *(ULONG*) pSingleContextFlags->ParamValue;
		if (_CONTEXT_OBJECT_FLAG_NETWORK_DEVICE & ContextFlags)
		{
			replace_one( hctx, L"\\Device\\LanmanRedirector", sizeof(L"\\Device\\LanmanRedirector") - sizeof(WCHAR), L"\\" );
			replace_one( hctx, L"\\Device\\Mup", sizeof(L"\\Device\\Mup") - sizeof(WCHAR), L"\\" );
		}
	}

	ULONG HookId, Mj, Mi, ProcessId;
	DRV_GetEventInfo( pMessage, &HookId, &Mj, &Mi, NULL, NULL, &ProcessId );

	PR_TRACE(( g_root, prtIMPORTANT, 
		TR "pid:%d ctx 0x%x op 0x%x:%x:%x(flags 0x%x) name ready: '%tstr'",
		ProcessId,
		(hOBJECT) *hctx, 
		HookId, Mj, Mi,
		ContextFlags, (hOBJECT) *hctx
		));

	return true;
}

cString*
GenerateContext (
	cContext* pContext,
	hOBJECT pPragueContext,
	PVOID pDrvContext,
	PVOID pMessage,
	ULONG ProcessId
	)
{
	PSINGLE_PARAM pSingleObjectName;
	pSingleObjectName = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_URL_W, 0 );
	if (!pSingleObjectName)
		return NULL;

	if (!pSingleObjectName->ParamSize)
		return NULL;

	PWCHAR pwchName = (PWCHAR) pSingleObjectName->ParamValue;
	if (pSingleObjectName->ParamSize > sizeof(WCHAR))
	{
		WCHAR symb = pwchName[pSingleObjectName->ParamSize / sizeof(WCHAR) - 2];
		if (L'\\' == symb)
			return NULL;
	}

	if (pSingleObjectName->ParamSize == sizeof(L"\\\\TSCLIENT\\SCARD"))
	{
		if (!memcmp(pSingleObjectName->ParamValue, L"\\\\TSCLIENT\\SCARD", sizeof(L"\\\\TSCLIENT\\SCARD") - sizeof(WCHAR)))
			return NULL;
	}

	cString* hCtx = NULL;
	tERROR error = pPragueContext->sysCreateObjectQuick((hOBJECT*) &hCtx, IID_STRING, PID_ANY, 0);
	if (PR_FAIL(error))
		return NULL;

	hCtx->SetCP( cCP_UNICODE );

	if (FillObjectName( pContext, hCtx, pDrvContext, pMessage ))
	{
		hCtx->propSetDWord( propid_EventProcessID, ProcessId );

		return hCtx;
	}

	hCtx->sysCloseObject();

	return NULL;
}

void
AttachUserInfo (
	cWUsers* pUsers,
	cString* hCtx,
	PVOID pMessage,
	cUserInfo* pUser
	)
{
	if (!propid_UserAccessed)
		return;

	PSINGLE_PARAM pParamLuid = DRV_GetEventParam (
		pMessage,
		0,
		_PARAM_OBJECT_LUID,
		sizeof(LUID)
		);

	if (!pParamLuid)
		return;

	PLUID pLuid = (PLUID) pParamLuid->ParamValue;
	
	if (pUsers->FillInfo( pLuid, pUser ))
		hCtx->propSetPtr( propid_UserAccessed, pUser );
}

void
SendVolumeObject (
	cContext* pContext,
	PVOID* ppMessage
	)
{
	pContext->VolumeCache_Clean();

	PSINGLE_PARAM pParamVolume = DRV_GetEventParam (
		*ppMessage,
		0,
		_PARAM_OBJECT_VOLUME_NAME_W,
		0
		);

	if (!pParamVolume || !pParamVolume->ParamSize)
	{
		PR_TRACE(( g_root, prtWARNING, TR "no special volume name. check generic" ));
		pParamVolume = DRV_GetEventParam (
			*ppMessage,
			0,
			_PARAM_OBJECT_URL_W,
			0
			);
	}

	if (!pParamVolume || !pParamVolume->ParamSize)
	{
		PR_TRACE(( g_root, prtWARNING, TR "no volume name. skip..." ));
		return;
	}

	PR_TRACE(( g_root, prtWARNING, TR "Mount volume '%S'", pParamVolume->ParamValue ));

	cString* hCtx = NULL;
	tERROR error = pContext->m_pPragueContext->sysCreateObjectQuick (
		(hOBJECT*) &hCtx,
		IID_STRING,
		PID_ANY,
		0
		);

	if (!PR_SUCC( error ))
		return;

	hCtx->SetCP( cCP_UNICODE );

	tORIG_ID Origin = OID_AVP3_BOOT;
	tDWORD dwPropSize = 0;

	error = hCtx->propSet( &dwPropSize, propid_CustomOrigin, &Origin, sizeof(Origin) );

	if (PR_SUCC( error ))
	{
		WCHAR dosname[_DOS_NAME_LEN];
		HRESULT hResult = DRV_GetVolumeName (
			pContext->m_pDrvContext,
			(PWCHAR) pParamVolume->ParamValue,
			dosname,
			sizeof(dosname) / sizeof(WCHAR)
			);
		if (SUCCEEDED( hResult ))
		{
			PR_TRACE(( g_root, prtWARNING, TR "resolved volume dos name'%S'", dosname ));
			error = hCtx->ImportFromBuff( &dwPropSize, dosname, sizeof(dosname), cCP_UNICODE, cSTRING_Z);
		}
		else
			error = hCtx->ImportFromBuff( &dwPropSize, pParamVolume->ParamValue, pParamVolume->ParamSize, cCP_UNICODE, cSTRING_Z);
	}

	if (PR_SUCC( error ))
	{
		// отложенное что бы не сбивать запись!
		AddToDelayed( pContext, hCtx, *ppMessage );
		AnswerMessage( pContext->m_pPragueContext, pContext->m_pDrvContext, ppMessage, TRUE, FALSE, 0 );

		tDWORD Verdict = cAVPG2_RC_ALLOW_ACCESS;
		tDWORD blen = sizeof(Verdict);

		pContext->m_pPragueContext->sysSendMsg( pmc_AVPG2, pm_AVPG2_NOTIFY_VOLUME_MOUNTED, (hOBJECT) *hCtx, &Verdict, &blen );
	}

	hCtx->sysCloseObject();
	hCtx = NULL;
}

void
SendObject (
	cContext* pContext,
	tAVPG2_WORKING_MODE WorkingMode,
	cString* hCtx,
	PVOID* ppMessage,
	ULONG ProcessId,
	bool bExecute
	)
{
	HRESULT hResult = S_OK;
	
	bool bSkip = false;
	bool bAllow = TRUE;
	bool bCachable = FALSE;
	ULONG ExpTime = 0;

	PR_TRACE(( g_root, prtIMPORTANT, TR "SendObject 0x%x in mode %d", (hOBJECT) *hCtx, WorkingMode ));

	if (!bExecute)
	{
		if (cAVPG2_WORKING_MODE_ON_SMART == WorkingMode
			&& 4 != ProcessId
			&& 8 != ProcessId
			)
		{
			if (!pContext->m_EvQueue.DeleteNotTheSame( hCtx ))
			{
				PR_TRACE(( g_root, prtNOTIFY, TR "smart mode - skip context 0x%x", (hOBJECT) *hCtx ));

				bSkip = true;
			}
		}
		else
			pContext->m_EvQueue.SkipItem( hCtx );
	}

	if (!bSkip)
	{
		tERROR error;
		tDWORD Verdict = cAVPG2_RC_ALLOW_ACCESS;
		tDWORD blen = sizeof(Verdict);

		hResult = DRV_ImpersonateThread( pContext->m_pDrvContext, *ppMessage );
		if (!SUCCEEDED( hResult ))
		{
			PR_TRACE(( g_root, prtERROR, TR "drv impersonate thread result 0x%x", hResult ));
		}

		cUserInfo User;
		AttachUserInfo( &pContext->m_Users, hCtx, *ppMessage, &User );

		pContext->m_ProcessingList.Enter( pContext->m_pDrvContext, *ppMessage, hCtx );
		error = pContext->m_pPragueContext->sysSendMsg( pmc_AVPG2, pm_AVPG2_PROCEED_OBJECT, (hOBJECT) *hCtx, &Verdict, &blen );
		pContext->m_ProcessingList.Leave( hCtx );

		SetThreadToken( NULL, NULL );

		PR_TRACE(( g_root, prtIMPORTANT, TR "proceed ctx 0x%x answer %terr, verdict 0x%x", 
			(hOBJECT) *hCtx, error, Verdict ));

		if (PR_SUCC(error))
		{
			if (cAVPG2_RC_CACHE & Verdict)
				bCachable = true;

			if (cAVPG2_RC_DENY_ACCESS & Verdict)
			{
				bAllow = false;
				bCachable = true;
				ExpTime = 3;
			}
		}
	}

	if (*ppMessage)
		AnswerMessage( pContext->m_pPragueContext, pContext->m_pDrvContext, ppMessage, bAllow, bCachable, ExpTime );
}

void
AddToDelayed (
	cContext* pContext,
	cString* hCtx,
	PVOID pMessage
	)
{
	PR_TRACE(( g_root, prtNOTIFY, TR "prepare delay event" ));

	if (pMessage)
	{
		HRESULT hResult = DRV_ImpersonateThread( pContext->m_pDrvContext, pMessage );
		if (!SUCCEEDED( hResult ))
		{
			PR_TRACE(( g_root, prtERROR, TR "drv impersonate thread (prepare delay) result 0x%x", hResult ));
		}
	}

	cDelayedContext* pDelayedContext = new cDelayedContext( pContext->m_pPragueContext, hCtx, 15 );
	if (pDelayedContext)
	{
		if (!pDelayedContext->IsValid())
		{
			delete pDelayedContext;
			pDelayedContext = NULL;
		}
	}

	if (pDelayedContext)
	{
		if (pContext->m_EvQueue.Add( pDelayedContext ))
		{
			PR_TRACE(( g_root, prtNOTIFY, TR "add delayed context 0x%x", pDelayedContext ));
		}
		else
		{
			PR_TRACE(( g_root, prtSPAM, TR "delayed already exist", pDelayedContext ));

			delete pDelayedContext;
			pDelayedContext = NULL;
		}
	}
}

//- common functions

cWinAvpgImpl::cWinAvpgImpl (
	hOBJECT pPragueContext,
	tAVPG2_WORKING_MODE WorkingMode,
	tDWORD ThreadMaxCount
	) : cAvpgImpl( pPragueContext, WorkingMode, ThreadMaxCount ), m_Context( pPragueContext, ThreadMaxCount )
{
	PR_TRACE(( g_root, prtERROR, TR "Registering..." ));
	m_pInstance = NULL;
	m_DriverFlags = _DRV_FLAG_NONE;

	if (PR_FAIL(CALL_Root_RegisterCustomPropId( g_root, &propid_EventDefinition, ((tSTRING)("mklif event definiton")), pTYPE_PTR | pCUSTOM_HERITABLE)))
	{
		PR_TRACE(( g_root, prtERROR, "cannot register custom property 'i_avpg event definiton'"));
		throw "cannot register custom property 'i_avpg event definiton'";
	}

	HRESULT hResult = DRV_Register (
		&m_Context.m_pDrvContext,
		AVPG_MPraguePlugin,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_PARALLEL | _CLIENT_FLAG_POPUP_TYPE |  _CLIENT_FLAG_USE_DRIVER_CACHE
		| _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_STRONG_PROCESSING | _CLIENT_FLAG_WANTIMPERSONATE,
		8000,
		DEADLOCKWDOG_TIMEOUT,
		(DRV_pfMemAlloc) MemAlloc,
		(DRV_pfMemFree) MemFree,
		pPragueContext
		);
	
	if (!SUCCEEDED( hResult ))
	{
		PR_TRACE(( g_root, prtERROR, TR "throw: DrvRegister failure!" ));
		throw "DrvRegister failure";
	}

	if (!m_Context.Init())
	{
		DRV_UnRegister( &m_Context.m_pDrvContext );
		throw "Context init failed";
	}

	hResult = DRV_QueryFlags( m_Context.m_pDrvContext, &m_DriverFlags );
	if (!SUCCEEDED( hResult ))
	{
		DRV_UnRegister( &m_Context.m_pDrvContext );
		throw "Unknown driver";
	}

	PR_TRACE(( g_root, prtERROR, TR "Driver flags 0x%x", m_DriverFlags ));

	try {
		if (_DRV_FLAG_MINIFILTER & m_DriverFlags)
			m_pInstance = new cMklAvpg( &m_Context, WorkingMode );
		else
			m_pInstance = new cKlAvpg( &m_Context, WorkingMode );
	} catch( char * pstrException )	{
		DRV_UnRegister( &m_Context.m_pDrvContext );
		throw pstrException;
	}
}

cWinAvpgImpl::~cWinAvpgImpl()
{
	PR_TRACE(( g_root, prtERROR, TR "~cWinAvpgImpl" ));
	if (m_pInstance)
	{
		DRV_BreakConnection( m_Context.m_pDrvContext );
		delete m_pInstance;
	}
}

tERROR cWinAvpgImpl::SetActivityMode (
	tAVPG2_CLIENT_ACTIVITY_MODE ActivityMode
	)
{
	HRESULT hResult = E_FAIL;

	switch (ActivityMode)
	{
	case cAVPG2_CLIENT_ACTIVITY_MODE_SLEEP:
		hResult = DRV_ChangeActiveStatus( m_Context.m_pDrvContext, FALSE );
		break;

	case cAVPG2_CLIENT_ACTIVITY_MODE_ACTIVE:
		hResult = DRV_ResetCache( m_Context.m_pDrvContext );
		if (SUCCEEDED( hResult ))
			hResult = DRV_ChangeActiveStatus( m_Context.m_pDrvContext, TRUE );
		break;
	}

	PR_TRACE(( g_root, prtERROR, TR "SetActivityMode %d result 0x%x", ActivityMode, hResult ));

	if (SUCCEEDED( hResult ))
		return m_pInstance->SetActivityMode( ActivityMode );

	return errUNEXPECTED;
}

tERROR
cWinAvpgImpl::ChangeWorkingMode (
	tAVPG2_WORKING_MODE WorkingMode )
{
	if (!m_pInstance)
		return errUNEXPECTED;

	PR_TRACE(( g_root, prtWARNING, TR "changing working mode to %d...", WorkingMode ));
	if (m_WorkingMode == WorkingMode)
	{
		PR_TRACE(( g_root, prtWARNING, TR "the same working mode. nothing todo" ));
		return errOK;
	}

	m_WorkingMode = WorkingMode;
	HRESULT hResult = DRV_FiltersClear( m_Context.m_pDrvContext );
	PR_TRACE(( g_root, prtWARNING, TR "FClear result 0x%x", hResult ));

	ResetDriverCache();

	bool bFiltersAdded = false;

	if (_DRV_FLAG_MINIFILTER & m_DriverFlags)
	{
		cMklAvpg* pmkla = (cMklAvpg*) m_pInstance;
		pmkla->m_WorkingMode = m_WorkingMode;
		bFiltersAdded = pmkla->AddFilters();
	}
	else
	{
		cKlAvpg* pkla = (cKlAvpg*) m_pInstance;
		pkla->m_WorkingMode = m_WorkingMode;
		bFiltersAdded = pkla->AddFilters();
	}

	if (bFiltersAdded)
	{
		PR_TRACE(( g_root, prtWARNING, TR "new working mode 0x%x", m_WorkingMode ));
		return errOK;
	}

	PR_TRACE(( g_root, prtWARNING, TR "changing working mode failed" ));

	return errUNEXPECTED;
};


tERROR
cWinAvpgImpl::ResetDriverCache (
	)
{
	HRESULT hResult = DRV_ResetCache( m_Context.m_pDrvContext );

	PR_TRACE(( g_root, prtERROR, TR "ResetDriverCache result 0x%x", hResult ));

	if (SUCCEEDED( hResult ))
		return errOK;

	return errUNEXPECTED;
}

tERROR
cWinAvpgImpl::CreateIO (
	hOBJECT* p_phObject,
	hSTRING p_hObjectName
	)
{
	if (m_pInstance)
		return m_pInstance->CreateIO( p_phObject, p_hObjectName );

	return errUNEXPECTED;
}

tERROR
cWinAvpgImpl::YieldEvent (
	hOBJECT EventObject,
	tDWORD YieldTimeout
	)
{
	PVOID pMessage = NULL;
	tDWORD dwPropSize = 0;

	tERROR error = EventObject->propGet( &dwPropSize, propid_EventDefinition, &pMessage, sizeof(pMessage) );
	if (PR_SUCC( error ) && sizeof(pMessage) == dwPropSize)
	{
		HRESULT hResult = DRV_Yeild( m_Context.m_pDrvContext, pMessage, YieldTimeout );

		PR_TRACE((g_root, prtSPAM, TR "yeild result %x", hResult));
	}
	
	return errOK;
}

tERROR
cWinAvpgImpl::ClearTrustedProcessesCache (
	)
{
	m_Context.m_TrustedProcesses.ClearCache();

	return errOK;
}

tERROR
cWinAvpgImpl::ResetBanList (
	)
{
	return m_Context.m_Users.ResetBanList();
}

tERROR
cWinAvpgImpl::AddBan (
	cUserInfo* pUserInfo
	)
{
	return m_Context.m_Users.AddBan( pUserInfo );
}

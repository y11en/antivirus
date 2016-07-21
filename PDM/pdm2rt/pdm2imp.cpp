#define PR_IMPEX_DEF

#include "pdm2imp.h"
#include <plugin/p_hash_md5.h>
#include "env/envhelper_win32w.h"

#include "pr_wtime.h"
#include <structs/procmonm.h>
//<<
#include <plugin/p_sfdb.h>
//>>
#include <strsafe.h>

#include "GetFileVer.h"

#include <Version/ver_product.h>

#pragma comment(lib, "Version.lib")

//////////////////////////////////////////////////////////////////////////

COwnSync::COwnSync()
{
	InitializeCriticalSection(&m_Sync);
}

COwnSync::~COwnSync()
{
	DeleteCriticalSection(&m_Sync);
}

void COwnSync::LockSync()
{
	EnterCriticalSection(&m_Sync);
}

void COwnSync::UnLockSync()
{
	LeaveCriticalSection(&m_Sync);
}

//////////////////////////////////////////////////////////////////////////

typedef struct _tsProcessInternalData
{
	bool	m_bUserTrusted;
	bool	m_bUserTrustedRequested;
}sProcessInternalData;

LONG gMemCounter = 0;

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );

	//PR_TRACE(( g_root, prtERROR, TR "allocated 0x%x, size %d", ptr, size ));
	
	return ptr;
};

void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	//PR_TRACE(( g_root, prtERROR, TR "free 0x%x", *pptr ));

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};

void
TraceEvent (
	PSingleEvent pse,
	PWCHAR pwchTrace
	)
{
	PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pse->m_pEventHdr;

	PR_TRACE(( g_root, prtERROR, TR "%S %d - 0x%x:0x%x",
		pwchTrace,
		pEventHdr->m_ProcessId,
		pEventHdr->m_HookID,
		pEventHdr->m_FunctionMj
		));
}

void CalcUnique(cObject* _this, tQWORD* pUnique, const cStrObj& name, tPdm2EventType eEventType)
{
	hHASH hHash;
	*pUnique = 0;
	if (PR_SUCC(_this->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5)))
	{
		hHash->Update((tBYTE*) (const wchar_t*)name.c_str(cCP_UNICODE), name.length() * 2);
		hHash->Update((tBYTE*) &eEventType, sizeof(eEventType));

		DWORD dwHash[4];
		if (PR_SUCC(hHash->GetHash((tBYTE*)&dwHash, sizeof(dwHash))))
		{
			dwHash[0] ^= dwHash[2];
			dwHash[1] ^= dwHash[3];
			memcpy(pUnique, &dwHash, sizeof(tQWORD));
		}

		hHash->sysCloseObject();
	}
}

Pdm2rtImp::Pdm2rtImp (
	PVOID pOpaquePtr,
	hSECUREFILEDB hSFDB_KL,
	cObject* pThis
	) :
		m_envhlpr(pOpaquePtr, pfMemAlloc, pfMemFree ),
		m_event_mgr( &m_envhlpr )
{
	//m_event_mgr.RegisterHandler( &m_heuristic, false );

	//<<
	pdm.m_hSFDB_KL = hSFDB_KL;
	pdm.m_pThis = pThis;
	//>>

	RegisterHeuristicHandlers(m_event_mgr);
	m_event_mgr.RegisterHandler( &pdm, false );

	m_SlotIdx = -1;

	if (m_event_mgr.m_pProcessList)
		m_event_mgr.m_pProcessList->AllocSlot( &m_SlotIdx );
};


Pdm2rtImp::~Pdm2rtImp()
{
}

void
Pdm2rtImp::RefreshTrusted (
	)
{
	if (-1 == m_SlotIdx)
		return;

	tPid* pPidArray = NULL;
	size_t nPidCount = 0;
	
	if (!m_event_mgr.m_pProcessList->GetSnapshot( &pPidArray, &nPidCount ))
		return;

	if (!pPidArray)
		return;

	for (size_t cou = 0; cou < nPidCount; cou++)
	{
		cProcessListItem* pProcess = m_event_mgr.m_pProcessList->FindProcess( pPidArray[cou] );
		if (pProcess)
		{
			cAutoProcessItemLock auLock( pProcess );

			sProcessInternalData* pData = (sProcessInternalData*) pProcess->GetSlotData (
				m_SlotIdx,
				sizeof(sProcessInternalData)
				);

			if (pData)
			{
				pData->m_bUserTrusted = false;
				pData->m_bUserTrustedRequested = false;
				PR_TRACE(( g_root, prtERROR, TR "reset trusted request for pid %d", (DWORD) pProcess->m_pid ));
			}
		}
	}

	m_event_mgr.m_pProcessList->ReleaseSnapshot( &pPidArray );
}

tERROR
pr_call
OnThreadInitCallback (
	tThreadCallType CallType,
	tPTR pCommonThreadContext,
	tPTR* ppThreadContext
	)
{
	tERROR err_ret = errOK;

	PsInternal pInternal = (PsInternal) pCommonThreadContext;

	switch (CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		if (pInternal && pInternal->m_pClientContext)
		{
			HRESULT hResult = MKL_AddInvisibleThread( pInternal->m_pClientContext );
			if (!SUCCEEDED( hResult ))
			{
				PR_TRACE(( g_root, prtERROR, TR "new thread - result 0x%x. pInternal 0x%x, client context 0x%x", 
					hResult, pInternal, pInternal->m_pClientContext ));
			}
		}
		break;
	
	case TP_CBTYPE_THREAD_DONE_CONTEXT:
		PR_TRACE(( g_root, prtERROR, TR "exit thread" ));
		break;
	
	case TP_CBTYPE_THREAD_YIELD:
		break;
	}

	return err_ret;
}

//////////////////////////////////////////////////////////////////////////
void
RegisterProcess (
	PsInternal pInternal,
	ULONG ProcessId,
	ULONG ParentProcessId,
	LARGE_INTEGER StartTime,
	PWCHAR pwchImagePath,
	PWCHAR pwchBaseFolder,
	PWCHAR pwchCmdLine
	)
{
	tPid pid = ProcessId;
	tPid parent_pid = ParentProcessId;
	cFile image_path( &pInternal->m_pPDM->m_envhlpr, pwchImagePath );
	cPath working_folder( &pInternal->m_pPDM->m_envhlpr, pwchBaseFolder );
	tcstring cmd_line = pwchCmdLine;
	uint64_t start_time = *(uint64_t*) &StartTime;

	EventContext evcon( pInternal, NULL );
	cEvent event( &evcon, parent_pid, 0 );

	pInternal->m_pPDM->m_event_mgr.OnProcessCreatePost (
		event, 
		pid,
		image_path,
		working_folder,
		cmd_line,
		0
		);
}

tVerdict
SingleEvent_System (
	PsInternal pInternal,
	PSingleEvent pse
	)
{
	tVerdict Verdict = evtVerdict_Default;

	PVOID pClientContext = pInternal->m_pClientContext;
	PVOID pMessage = pse->m_pMessage;
	PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pse->m_pEventHdr;
	ULONG MessageSize = pse->m_MessageSize;

	if (MJ_CREATE_PROCESS_NOTIFY == pEventHdr->m_FunctionMj)
	{
		// process start
		ULONG ProcessId = 0;
		PWCHAR pwchProcessFileName = NULL;
		PWCHAR pwchBaseFolder = NULL;
		PWCHAR pwchCmdLine = NULL;
		LARGE_INTEGER StartTime;
		
		StartTime.QuadPart = 0;

		PSINGLE_PARAM pParamProcName = MKL_GetEventParam (
			pMessage,
			MessageSize,
			_PARAM_OBJECT_URL_W,
			FALSE
			);

		if (pParamProcName && pParamProcName->ParamSize)
			pwchProcessFileName = (PWCHAR) pParamProcName->ParamValue;

		PSINGLE_PARAM pParamProcBaseFolder = MKL_GetEventParam (
			pMessage,
			MessageSize,
			_PARAM_OBJECT_BASEPATH,
			FALSE
			);

		if (pParamProcBaseFolder && pParamProcBaseFolder->ParamSize)
			pwchBaseFolder = (PWCHAR) pParamProcBaseFolder->ParamValue;

		PSINGLE_PARAM pParamProcCmdLine = MKL_GetEventParam (
			pMessage,
			MessageSize,
			_PARAM_OBJECT_STARTUP_STR,
			FALSE
			);

		if (pParamProcCmdLine && pParamProcCmdLine->ParamSize)
			pwchCmdLine = (PWCHAR) pParamProcCmdLine->ParamValue;

		PSINGLE_PARAM pParamTmp = MKL_GetEventParam (
			pMessage,
			MessageSize,
			_PARAM_OBJECT_SOURCE_ID,
			FALSE
			);

		if (pParamTmp && pParamTmp->ParamSize == sizeof(ULONG))
			ProcessId = *(ULONG*) pParamTmp->ParamValue;

		if (ProcessId && pwchProcessFileName)
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "Process created: '%S' pid %d (parent pid %d)", 
				pwchProcessFileName,
				(ULONG) ProcessId,
				(ULONG) pEventHdr->m_ProcessId
				));

			if (pwchBaseFolder)
				PR_TRACE(( g_root, prtIMPORTANT, TR "\t\tbase folder '%S'", pwchBaseFolder ));

			if (pwchCmdLine)
				PR_TRACE(( g_root, prtIMPORTANT, TR "\t\tcmd line '%S'", pwchCmdLine ));

			RegisterProcess (
				pInternal,
				ProcessId,
				pEventHdr->m_ProcessId,
				StartTime,
				pwchProcessFileName,
				pwchBaseFolder,
				pwchCmdLine
				);
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, TR "create process - no pid or path" ));
		}
	}

	if (MJ_EXIT_PROCESS == pEventHdr->m_FunctionMj)
	{
		// process stop
		tPid pid = 0;

		PSINGLE_PARAM pParamTmp = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_SOURCE_ID, FALSE );
		if (pParamTmp && pParamTmp->ParamSize == sizeof(ULONG))
			pid = *(ULONG*) pParamTmp->ParamValue;

		if (pid)
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "exit process - pid %d", (ULONG) pid ));
			
			EventContext evcon( pInternal, pse );
			cEvent event( &evcon, pid, 0 );
			//Verdict = pInternal->m_pPDM->m_event_mgr.OnProcessExitPost( event );
			pInternal->m_pPDM->m_event_mgr.OnProcessExitPost( event );
			Verdict = event.GetVerdict();
		}
	}

	if (MJ_SYSTEM_CREATE_THREAD == pEventHdr->m_FunctionMj)
	{
		ULONG CurrentPid = pEventHdr->m_ProcessId;

		ULONG DestPid = 0;
		PSINGLE_PARAM pDestProcId = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_CLIENTID1, FALSE );
		if (pDestProcId && pDestProcId->ParamSize == sizeof(ULONG))
			DestPid = *(ULONG*) pDestProcId->ParamValue;

		ULONG ThreadId = 0;
		PSINGLE_PARAM pThreadId = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_CLIENTID2, FALSE );
		if (pThreadId && pThreadId->ParamSize == sizeof(ULONG))
			ThreadId = *(ULONG*) pThreadId->ParamValue;

		tAddr eip = 0;
		tAddr thread_base = 0;
		
		if ((CurrentPid != DestPid) && ThreadId)
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "process %d create remote thread %d in process %d", 
				CurrentPid, ThreadId, DestPid));
			
			EventContext evcon( pInternal, pse );
			cEvent event( &evcon, CurrentPid, pEventHdr->m_ThreadId );

			pInternal->m_pPDM->m_event_mgr.OnThreadCreatePost (
				event,
				DestPid,
				ThreadId,
				eip,
				thread_base
				);
			Verdict = event.GetVerdict();
		}
	}

	if (MJ_SYSTEM_TERMINATE_THREAD_NOTIFY == pEventHdr->m_FunctionMj)
	{
		ULONG CurrentPid = pEventHdr->m_ProcessId;

		ULONG ThreadId = 0;
		PSINGLE_PARAM pThreadId = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_CLIENTID2, FALSE );
		if (pThreadId && pThreadId->ParamSize == sizeof(ULONG))
			ThreadId = *(ULONG*) pThreadId->ParamValue;

		if (ThreadId)
		{
			EventContext evcon( pInternal, pse );
			cEvent event( &evcon, CurrentPid, ThreadId );
			pInternal->m_pPDM->m_event_mgr.OnThreadTerminatePost( event, CurrentPid, ThreadId );
			Verdict = event.GetVerdict();
		}
	}

	return Verdict;
}

bool MakeFileName (
	PVOID pClientContext,
	PVOID pMessage,
	ULONG MessageSize,
	ULONG ContextFlags,
	PWCHAR* ppwchFileName,
	PULONG pFileNameLen,
	bool bDestination
	)
{
	PWCHAR pwchFileName = NULL;

	int flen = 0;
	int vlen = 0;

	ULONG ParamID = bDestination ? _PARAM_OBJECT_URL_DEST_W : _PARAM_OBJECT_URL_W;
	ULONG ParamVolumeID = bDestination ? _PARAM_OBJECT_VOLUME_NAME_DEST_W : _PARAM_OBJECT_VOLUME_NAME_W;

	WCHAR pwchDosName[MAX_PATH + 1];
	memset( pwchDosName, 0, sizeof(pwchDosName) );

	*ppwchFileName = NULL;
	*pFileNameLen = 0;

	PSINGLE_PARAM pParamFileName = MKL_GetEventParam( pMessage, MessageSize, ParamID, FALSE );
	if (pParamFileName && pParamFileName->ParamSize)
		pwchFileName = (PWCHAR) pParamFileName->ParamValue;

	if (!pwchFileName)
		return false;

	if (ContextFlags & _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE)
	{
		cStringObj strtmp = pwchFileName;
		strtmp.replace_one(L"\\Device\\LanmanRedirector", fSTRING_COMPARE_CASE_INSENSITIVE, L"\\");
		strtmp.replace_one(L"\\Device\\Mup", fSTRING_COMPARE_CASE_INSENSITIVE, L"\\");
		
		flen = (strtmp.length() + 1) * sizeof(WCHAR);
		*ppwchFileName = (PWCHAR) pfMemAlloc(NULL, flen, 0 );
		if (*ppwchFileName)
		{
			HRESULT hResult = StringCbCopy (
				*ppwchFileName,
				flen,
				strtmp.data()
				);

			if (S_OK == hResult)
			{
				*pFileNameLen = flen;
				return true;
			}

			PR_TRACE(( g_root, prtERROR, TR "Cannot build network name - string ops failed" ));

			pfMemFree( NULL, (void**) ppwchFileName );
		 
			return false;
		}
	}
	
	PWCHAR pwchVolumeName = NULL;
	
	PSINGLE_PARAM pParamVolumeName = MKL_GetEventParam( pMessage, MessageSize, ParamVolumeID, FALSE );
	if (pParamVolumeName && pParamVolumeName->ParamSize)
	{
		pwchVolumeName = (PWCHAR) pParamVolumeName->ParamValue;

		if (S_OK == MKL_GetVolumeName( pClientContext, pwchVolumeName, pwchDosName, sizeof(pwchDosName) / sizeof(WCHAR)))
		{
			//PR_TRACE(( g_root, prtERROR, TR "volume '%S' from '%S'", pwchDosName, pwchVolumeName ));
			vlen = lstrlen( pwchDosName );

			flen = vlen * sizeof(WCHAR) + pParamFileName->ParamSize - pParamVolumeName->ParamSize + sizeof(WCHAR);
			*ppwchFileName = (PWCHAR) pfMemAlloc(NULL, flen, 0 );

			if (*ppwchFileName)
			{
				PWCHAR pwchTmp = *ppwchFileName;

				memcpy( pwchTmp, pwchDosName, vlen * sizeof(WCHAR) );
				
				HRESULT hResult = StringCbCopy (
					pwchTmp + vlen,
					flen - vlen * sizeof(WCHAR),
					pwchFileName + pParamVolumeName->ParamSize / sizeof(WCHAR) - 1
					);

				if (S_OK == hResult)
				{
					*pFileNameLen = flen;
					return true;
				}

				PR_TRACE(( g_root, prtERROR, TR "Cannot build name - string ops failed" ));

				pfMemFree( NULL, (void**) ppwchFileName );

				return false;
			}
		}
	}

	*ppwchFileName = (PWCHAR) pfMemAlloc(NULL, pParamVolumeName->ParamSize, 0 );
	 if (*ppwchFileName)
	 {
		HRESULT hResult = StringCbCopy (
			*ppwchFileName,
			pParamVolumeName->ParamSize / sizeof(WCHAR),
			pwchFileName
			);

		if (S_OK == hResult)
		{
			*pFileNameLen = pParamVolumeName->ParamSize;
			return true;
		}

		PR_TRACE(( g_root, prtERROR, TR "Cannot build default name - string ops failed" ));

		pfMemFree( NULL, (void**) ppwchFileName );
	 }

	return false;
}

tVerdict
SingleEvent_File (
	PsInternal pInternal,
	PSingleEvent pse
	)
{
	tVerdict Verdict = evtVerdict_Default;

	PVOID pClientContext = pInternal->m_pClientContext;
	PVOID pMessage = pse->m_pMessage;
	PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pse->m_pEventHdr;
	ULONG MessageSize = pse->m_MessageSize;

	bool bPostEvent = false;
	if (pEventHdr->m_EventFlags & _EVENT_FLAG_POSTPROCESSING)
		bPostEvent = true;

	ULONG ContextFlags = 0;
	PSINGLE_PARAM pParamContextFlags = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_CONTEXT_FLAGS, FALSE );
	if (pParamContextFlags && (sizeof(ULONG) == pParamContextFlags->ParamSize))
		ContextFlags = *(PULONG) pParamContextFlags->ParamValue;

	PWCHAR pwchFileName;
	ULONG FileNameLen;

	uint64_t unique = 1;

	PSINGLE_PARAM pParamUnique = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_INTERNALID, FALSE );
	if (pParamUnique)
	{
		PR_TRACE(( g_root, prtSPAM, TR "pParamUnique size %d", pParamUnique->ParamSize ));
	}
	
	if (pParamUnique && (sizeof(uint64_t) == pParamUnique->ParamSize))
		unique = *(uint64_t*) pParamUnique->ParamValue;

	if (!MakeFileName( pClientContext, pMessage, MessageSize, ContextFlags, &pwchFileName, &FileNameLen, false ))
		return evtVerdict_Default;

//	PR_TRACE(( g_root, prtSPAM, TR "op 0x%x file name '%S'", pEventHdr->m_FunctionMj, pwchFileName ));

	EventContext evcon( pInternal, pse );
	cEvent event( &evcon, pEventHdr->m_ProcessId, pEventHdr->m_ThreadId );

	switch (pEventHdr->m_FunctionMj)
	{
	case IRP_MJ_SET_INFORMATION:
		switch (pEventHdr->m_FunctionMi)
		{
		case FileRenameInformation:
			{
				PWCHAR pwchFileNameDest = NULL;
				ULONG FileNameDestLen;

				if (MakeFileName (
					pClientContext,
					pMessage,
					MessageSize,
					ContextFlags,
					&pwchFileNameDest,
					&FileNameDestLen,
					true
					))
				{
					PR_TRACE(( g_root, prtNOTIFY, TR "%s rename '%S' -> '%S'",
						bPostEvent ? "(post)" : "(pre)",
						pwchFileName,
						pwchFileNameDest
						));

					HandleImp hFile;
					HandleImp hFileDst;

					if (bPostEvent)
						hFileDst.ConnectHandle( pwchFileNameDest );
					else
						hFile.ConnectHandle( pwchFileName );

					cFile image (
						&pInternal->m_pPDM->m_envhlpr,
						pwchFileName,
						NULL,
						FILE_ATTRIBUTE_UNDEFINED,
						(tHANDLE) &hFile,
						unique
						);

					cFile imagedst (
						&pInternal->m_pPDM->m_envhlpr,
						pwchFileNameDest,
						NULL,
						FILE_ATTRIBUTE_UNDEFINED,
						(tHANDLE) &hFile,
						unique
						);

					if (bPostEvent)
						pInternal->m_pPDM->m_event_mgr.OnFileRenamePost( event, image, imagedst );
					else
						pInternal->m_pPDM->m_event_mgr.OnFileRenamePre( event, image, imagedst );
				}

				pfMemFree( NULL, (PVOID*) &pwchFileNameDest );
			}
			break;
		}
		break;
	case IRP_MJ_CLEANUP:
		{
			if (ContextFlags & _CONTEXT_OBJECT_FLAG_DIRECTORY)
			{
				// skip folder
			}
			else if (ContextFlags & _CONTEXT_OBJECT_FLAG_MODIFIED)
			{
				// closing modified file
				HandleImp hFile;
				if (bPostEvent)
					hFile.ConnectHandle(pwchFileName);
				else
					hFile.ConnectHandle( pInternal->m_pClientContext, pMessage, MessageSize );

				cFile image (
					&pInternal->m_pPDM->m_envhlpr,
					pwchFileName,
					NULL,
					FILE_ATTRIBUTE_UNDEFINED,
					(tHANDLE) &hFile,
					unique
					);

				PR_TRACE(( g_root, prtERROR, TR "event 0x%x: close modified (%S) file name '%S' (unique 0x%I64x)",
					pEventHdr->m_DrvMark,
					pEventHdr->m_EventFlags & _EVENT_FLAG_POSTPROCESSING ? L"post" : L"pre",
					pwchFileName,
					unique
					));

				if (bPostEvent)
					pInternal->m_pPDM->m_event_mgr.OnFileCloseModifiedPost( event, image );
				else
					pInternal->m_pPDM->m_event_mgr.OnFileCloseModifiedPre( event, image );
			}
		}
	}

	pfMemFree( NULL, (PVOID*) &pwchFileName );

	Verdict = event.GetVerdict();

	return Verdict;
}

tVerdict
SingleEvent_Registry (
	PsInternal pInternal,
	PSingleEvent pse
	)
{
	tVerdict Verdict = evtVerdict_Default;

	PVOID pClientContext = pInternal->m_pClientContext;
	PVOID pMessage = pse->m_pMessage;
	PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pse->m_pEventHdr;
	ULONG MessageSize = pse->m_MessageSize;

	bool bPostEvent = false;
	if (pEventHdr->m_EventFlags & _EVENT_FLAG_POSTPROCESSING)
		bPostEvent = true;

	PWCHAR pwchKeyName = NULL;
	PSINGLE_PARAM pParamKeyName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
	if (pParamKeyName && pParamKeyName->ParamSize)
		pwchKeyName = (PWCHAR) pParamKeyName->ParamValue;

	PWCHAR pwchValueName = NULL;
	PSINGLE_PARAM pParamValueName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_PARAM_W, FALSE );
	if (pParamValueName && pParamValueName->ParamSize)
		pwchValueName = (PWCHAR) pParamValueName->ParamValue;
	
	if (!pwchKeyName)
		return evtVerdict_Default;

	PVOID pData = NULL;
	ULONG nDataSize = 0;
	PSINGLE_PARAM pParamVal = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_BINARYDATA, FALSE );
	if (pParamVal && pParamVal->ParamSize)
	{
		pData = pParamVal->ParamValue;
		nDataSize = pParamVal->ParamSize;
	}

	PULONG pDataType = NULL;
	PSINGLE_PARAM pParamType = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_SOURCE_ID, FALSE );
	if (pParamType && (sizeof(ULONG) == pParamType->ParamSize))
		pDataType = (PULONG) pParamType->ParamValue;

	switch (pEventHdr->m_FunctionMj)
	{
	case Interceptor_SetValueKey:
		if (pwchValueName && pData && pDataType)
		{ 
			PR_TRACE(( g_root, prtNOTIFY, TR "%s set value '%S'", bPostEvent ? "(post)" : "(pre)", pwchKeyName ));
			PR_TRACE(( g_root, prtNOTIFY, TR "\t'%S'", pwchValueName ));
			PR_TRACE(( g_root, prtNOTIFY, TR "\t\ttype %d, len %d", *pDataType, nDataSize ));

			uint32_t nType = *pDataType;

			cRegKey RegKey( 0, pwchKeyName );

			EventContext evcon( pInternal, pse );
			cEvent event( &evcon, pEventHdr->m_ProcessId, pEventHdr->m_ThreadId );
			
			if (bPostEvent)
			{
				pInternal->m_pPDM->m_event_mgr.OnRegValueSetPost (
					event,
					RegKey,
					pwchValueName,
					nType,
					pData,
					nDataSize,
					0, 0, 0
					);
			}
			else
			{
				pInternal->m_pPDM->m_event_mgr.OnRegValueSetPre (
					event,
					RegKey,
					pwchValueName,
					nType,
					pData,
					nDataSize,
					0, 0, 0
					);
			}
			
			Verdict = event.GetVerdict();

			if (REG_SZ == nType ||
				REG_EXPAND_SZ == nType ||
				REG_MULTI_SZ == nType)
			{
				if (bPostEvent)
				{
					pInternal->m_pPDM->m_event_mgr.OnRegValueSetStrPost (
						event,
						RegKey,
						pwchValueName,
						(PWCHAR) pData,
						NULL
						);
				}
				else
				{
					pInternal->m_pPDM->m_event_mgr.OnRegValueSetStrPre (
						event,
						RegKey,
						pwchValueName,
						(PWCHAR) pData,
						NULL
						);
				}
				Verdict |= event.GetVerdict();
			}
		}
		break;
	}

	return Verdict;
}

tVerdict
SingleEvent_Go (
	PsInternal pInternal,
	PSingleEvent pse,
	PMKLIF_EVENT_HDR pEventHdr
	)
{
	tVerdict Verdict = evtVerdict_Default;

	switch (pEventHdr->m_HookID)
	{
	case FLTTYPE_SYSTEM:
		Verdict = SingleEvent_System( pInternal, pse );
		break;

	case FLTTYPE_NFIOR:
		Verdict = SingleEvent_File( pInternal, pse );
		break;

	case FLTTYPE_REGS:
		Verdict = SingleEvent_Registry( pInternal, pse );
		break;
	}
	return Verdict;
}

void
ProcessSingleEvent (
	PsInternal pInternal,
	PSingleEvent pse
	)
{
	HRESULT hResult;

	PVOID pClientContext = pInternal->m_pClientContext;

	PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pse->m_pEventHdr;
	PVOID pMessage = pse->m_pMessage;

	tVerdict Verdict = evtVerdict_Default;

	MKLIF_REPLY_EVENT DrvVerdict;	// default answer
	memset( &DrvVerdict, 0, sizeof(DrvVerdict) );

	bool bWasImpersonated = false;

	bool bUserTrusted = false;
	cEvent event( NULL, pEventHdr->m_ProcessId, pEventHdr->m_ThreadId );
	event.m_pEventMgr = &pInternal->m_pPDM->m_event_mgr;

	if (pEventHdr->m_ProcessId && (4 != pEventHdr->m_ProcessId))
	{
		cProcessListItem* pProcess = event.GetProcess();
		if (!pProcess)
		{
			//PR_TRACE(( g_root, prtIMPORTANT, TR "check user trusted failed - process %d not found in list",
			//	pEventHdr->m_ProcessId ));
		}
		else
		{
			sProcessInternalData* pData = (sProcessInternalData*) pProcess->GetSlotData (
				pInternal->m_pPDM->m_SlotIdx,
				sizeof(sProcessInternalData)
				);

			if (!pData)
			{
				PR_TRACE(( g_root, prtERROR, TR "check trusted: alloc slot data failed. slot idx %d", pInternal->m_pPDM->m_SlotIdx ));
			}
			else
			{
				if (!pData->m_bUserTrustedRequested)
				{
					PR_TRACE(( g_root, prtIMPORTANT, TR "check user trusted '%S'", pProcess->m_image.getFull() ));

					cPRCMRequest req;
					tDWORD blen = sizeof(cPRCMRequest);
					
					req.m_Request = cPRCMRequest::_ePrcmn_ExclBehavior;
					req.m_ProcessId = (tDWORD) pProcess->m_pid;

					tERROR error = pInternal->m_pThis->sysSendMsg (
						pmc_PROCESS_MONITOR,
						pm_PROCESS_MONITOR_PROCESS_GETACTIONMASK,
						pInternal->m_pThis,
						&req,
						&blen
						);

					PR_TRACE(( g_root, prtIMPORTANT, TR "user trust request result %terr", error ));

					if (errOK_DECIDED == error)
					{
						pData->m_bUserTrusted = true;
						PR_TRACE(( g_root, prtIMPORTANT, TR "found user trusted '%S'", pProcess->m_image.getFull() ));
					}

					pData->m_bUserTrustedRequested = true;
				}

				bUserTrusted = pData->m_bUserTrusted;
			}
		}
	}

	if (!bUserTrusted)
	{
		/*PR_TRACE(( g_root, prtERROR, TR "event 0x%x, hook 0x%x, flags 0x%x",
			pEventHdr->m_DrvMark,
			pEventHdr->m_HookID,
			pEventHdr->m_EventFlags
			));*/

		hResult = MKL_ImpersonateThread( pClientContext, pMessage );
		if (SUCCEEDED( hResult ))
			bWasImpersonated = true;

		Verdict = SingleEvent_Go( pInternal, pse, pEventHdr );
	}

	//////////////////////////////////////////////////////////////////////////
	PR_TRACE(( g_root, prtSPAM, TR "verdict 0x%x",Verdict ));

	if (evtVerdict_Pending & Verdict)
	{
		DrvVerdict.m_VerdictFlags = efVerdict_Pending;
		PR_TRACE(( g_root, prtIMPORTANT, TR "pending event 0x%x", pEventHdr->m_DrvMark ));
	}
	else
	{
		if (evtVerdict_Deny & Verdict)
			DrvVerdict.m_VerdictFlags = efVerdict_Deny;
	}

	if (!(pEventHdr->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
	{
		hResult = MKL_ReplyMessage( pClientContext, pMessage, &DrvVerdict );
		
		// was direct answer
		pEventHdr->m_EventFlags |= _EVENT_FLAG_DELETED_ON_MARK;
	}

	if (evtVerdict_Pending & Verdict)
	{
		PR_TRACE(( g_root, prtERROR, TR "leave message ptr 0x%x (pending)", pse->m_pMessage ));
	}
	else
		pfMemFree( NULL, &pse->m_pMessage );

	if (bWasImpersonated)
		SetThreadToken( NULL, NULL );
}

typedef struct _sPdm2EventInfo
{
	enPdm2EventType		m_EventType;
	PWCHAR				m_pwchDescribe;
	enDetectDanger		m_Danger;
	
	tDWORD				m_Actions;
	tDWORD				m_ActionsMask;
}Pdm2EventInfo, *PPdm2EventInfo;

#define DEFAULT_ASK_ACTION		ACTION_QUARANTINE | ACTION_TERMINATE | ACTION_ALLOW
#define DEFAULT_ASK_ACTION_MASK	ACTION_QUARANTINE | ACTION_TERMINATE | ACTION_ALLOW

Pdm2EventInfo gEventInfo[] = {
	{PDM2_EVENT_UNKNOWN,		L"<unknown>",							DETDANGER_UNKNOWN,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_P2P_SC_RDL,		L"Worm.P2P.generic",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_SC_AR,			L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_SC_ARsrc,		L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_SCN,			L"Worm.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_P2P_SCN,		L"Worm.P2P.generic",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_SC2STARTUP,		L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_SC_MULTIPLE,	L"Worm.P2P.generic",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_RDR,			L"RootShell",							DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_HIDDEN_OBJ,		L"Hidden object",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_INVADER,		L"Invader",								DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_INVADER_LOADER,	L"Invader (loader)",					DETDANGER_LOW,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_STRANGEKEY,		L"Suspicious registry value",			DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_SYSCHANGE,		L"Strange behaviour",					DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_HIDDEN_INSTALL,	L"Hidden install",						DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_BUFFEROVERRUN,	L"Buffer overrun",						DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_DEP,			L"Data Execution",						DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_HOSTS,			L"Hosts file modification",				DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_DOWNLOADER,		L"Trojan Downloader",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_TROJAN_GEN,		L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_DRIVER_INS,		L"Suspicious driver installation",		DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	{PDM2_EVENT_HIDDEN_SEND,	L"Hidden data sending",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
};

void
VerQueryValueImp (
	const LPVOID pBlock,
	LPWSTR lpSubBlock,
	LPVOID * lplpBuffer,
	PUINT puLen
	)
{
	__try
	{
		VerQueryValue(pBlock, lpSubBlock, lplpBuffer, puLen);
	}
	__except(TRUE)
	{
		*puLen = 0;
		PR_TRACE(( g_root, prtERROR, TR "VerQueryValue - exception" ));
	}
}


void AssignProcessDataImp (
	cPdm2rtProcInfo* pProcessInfo,
	cProcessListItem* pProcess
	)
{
	if (true)
	{
		cAutoProcessItemLock auLock( pProcess );

		pProcessInfo->m_ProcessId = pProcess->m_pid;
		pProcessInfo->m_ImagePath = pProcess->m_image.getFull();
		pProcessInfo->m_CommandLine = pProcess->m_cmdline;

		
		cDateTime timeinfo;
		SYSTEMTIME SystemTime;

		PR_TRACE(( g_root, prtERROR, TR "start time %I64x, exit time %I64x",
				pProcess->m_ftStartTime,
				pProcess->m_ftExitTime
				));
		
		FileTimeToSystemTime( (PFILETIME) &pProcess->m_ftStartTime, &SystemTime );
		timeinfo.SetUTC (
			SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
			SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond
			);

		timeinfo.CopyTo( pProcessInfo->m_StartTime );
		
		FileTimeToSystemTime( (PFILETIME) &pProcess->m_ftExitTime, &SystemTime );
		timeinfo.SetUTC (
			SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
			SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond
			);

		timeinfo.CopyTo( pProcessInfo->m_StopTime );
		
		//pProcessInfo->m_bHidden;
	}

	#define _verinfo_size 0x4000
	PVOID pchVerInfoBuffer = pfMemAlloc( NULL, _verinfo_size, 0 );
	if (pchVerInfoBuffer)
	{
		BOOL bRet = _GetFileVersionInfoW( pProcessInfo->m_ImagePath.data(), _verinfo_size, pchVerInfoBuffer );
		if (!bRet)
		{
			PR_TRACE(( g_root, prtERROR, TR "GetFileVersionInfo for '%S' failed, winerr 0x%x", 
				pProcessInfo->m_ImagePath.data(),
				GetLastError() ));
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, TR "GetFileVersionInfo for '%S' ok", pProcessInfo->m_ImagePath.data() ));

			struct LANGANDCODEPAGE {
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;

			UINT cbTranslate;

			VerQueryValueImp( pchVerInfoBuffer, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate);

			for(UINT i = 0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
			{
				PWCHAR ptrinfo;
				UINT len;
				WCHAR SubBlock[0x100];

				StringCbPrintf( SubBlock, sizeof(SubBlock), L"\\StringFileInfo\\%04x%04x\\FileDescription",
					lpTranslate[i].wLanguage, lpTranslate[i].wCodePage );
				
				if (VerQueryValue( pchVerInfoBuffer, SubBlock, (PVOID*) &ptrinfo, &len ))
				{
					pProcessInfo->m_Description.assign( ptrinfo, cCP_UNICODE );
					PR_TRACE(( g_root, prtIMPORTANT, TR "\tDescription '%S'", pProcessInfo->m_Description.data() ));
				}

				StringCbPrintf(SubBlock, sizeof(SubBlock), L"\\StringFileInfo\\%04x%04x\\CompanyName",
					lpTranslate[i].wLanguage, lpTranslate[i].wCodePage );
				
				if (VerQueryValue(pchVerInfoBuffer, SubBlock, (PVOID*) &ptrinfo, &len))
				{
					pProcessInfo->m_Vendor.assign( ptrinfo, cCP_UNICODE );
					PR_TRACE(( g_root, prtIMPORTANT, TR "\tVendor '%S'", pProcessInfo->m_Vendor.data() ));
				}

				StringCbPrintf(SubBlock, sizeof(SubBlock), L"\\StringFileInfo\\%04x%04x\\FileVersion",
					lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
				
				if (VerQueryValue(pchVerInfoBuffer, SubBlock, (PVOID*) &ptrinfo, &len))
				{
					pProcessInfo->m_Version.assign( ptrinfo, cCP_UNICODE );
					PR_TRACE(( g_root, prtIMPORTANT, TR "\tVersion '%S'", pProcessInfo->m_Version.data() ));
				}

				break;
			}
		}

		pfMemFree( NULL, &pchVerInfoBuffer );
	}
}

void
AssignProcessData (
	PEventContext pEventContext,
	cAskObjectAction_PDM2* pAskAction
	)
{
	cProcessListItem* pProcess = pEventContext->m_pProcess;

	AssignProcessDataImp( &pAskAction->m_CurrentProcess, pProcess );
}

void
AssignParentProcessData (
	PEventContext pEventContext,
	cAskObjectAction_PDM2* pAskAction
	)
{
	cProcess ParentProccess = pEventContext->m_pProcess->get_parent();
	if (!ParentProccess)
		return;

	AssignProcessDataImp( &pAskAction->m_ParentProcess, ParentProccess );
}


void
AssignChildProcessesData (
	PEventContext pEventContext,
	cAskObjectAction_PDM2* pAskAction
	)
{
	tPid* pPidArray = NULL;
	size_t nPidCount = 0;
	
	if (!pEventContext->m_pInternal->m_pPDM->m_event_mgr.m_pProcessList->GetSnapshot( &pPidArray, &nPidCount ))
		return;

	if (!pPidArray)
		return;

	for (size_t cou = 0; cou < nPidCount; cou++)
	{
		cProcessListItem* pProcess = pEventContext->m_pInternal->m_pPDM->m_event_mgr.m_pProcessList->FindProcess( pPidArray[cou] );
		if (pProcess)
		{
			if (pEventContext->m_pProcess->m_uniq_pid == pProcess->m_parent_uniq_pid)
			{
				cPdm2rtProcInfo ProcessInfo;
				AssignProcessDataImp( &ProcessInfo, pProcess );

				pAskAction->m_ChildProcList.push_back( ProcessInfo );
			}
		}
	}

	pEventContext->m_pInternal->m_pPDM->m_event_mgr.m_pProcessList->ReleaseSnapshot( &pPidArray );
}

PDM2RTActions
ProceedRequest (
	PEventContext pEventContext,
	cAskObjectAction_PDM2& AskAction
	)
{
	PDM2RTActions Action = pEventContext->m_pInternal->m_Settings.m_Action;
	tBOOL bLog = pEventContext->m_pInternal->m_Settings.m_bLog;

	AskAction.m_EventType = pEventContext->m_EventType;
	AskAction.m_EventSubType = pEventContext->m_EventSubType;

	pEventContext->m_pInternal->m_Statistics.m_nTreats++;
	cDateTime current_time( time( NULL ) );
	current_time.CopyTo( pEventContext->m_pInternal->m_Statistics.m_timeLastTreat );

	AskAction.m_tmTimeStamp = cDateTime::now_utc();
	AskAction.m_nDetectStatus = DSTATUS_HEURISTIC;
	AskAction.m_nNonCuredReason = NCREASON_REPONLY;

	AskAction.m_nDefaultAction = ACTION_QUARANTINE;
	AskAction.m_nDetectType = DETYPE_RISKWARE;
	AskAction.m_nExcludeAction = ACTION_ALLOW;

	#pragma message("---------- fill m_strUserName ----------")
	//!todo AskAction.m_strUserName = L"";

	int ItemIdx = 0;

	for (int cou = 0; cou < _countof(gEventInfo); cou++)
	{
		if (gEventInfo[cou].m_EventType == pEventContext->m_EventType)
		{
			ItemIdx = cou;
			break;
		}
	}

	AskAction.m_strDetectName = gEventInfo[ItemIdx].m_pwchDescribe;
	AskAction.m_nDetectDanger = gEventInfo[ItemIdx].m_Danger;

	AskAction.m_nActionsAll = gEventInfo[ItemIdx].m_Actions;
	AskAction.m_nActionsMask = gEventInfo[ItemIdx].m_ActionsMask;
	
	AskAction.m_nPID = pEventContext->m_pProcess->m_pid;
	
	AskAction.m_nObjectStatus = OBJSTATUS_SUSPICION;
#if VER_PRODUCTVERSION_HIGH <= 7
	AskAction.m_nObjectType = OBJTYPE_MEMORYPROCESS;
	AskAction.m_strObjectName = AskAction.m_CurrentProcess.m_ImagePath;
	
	AssignProcessData( pEventContext, &AskAction );
	AssignParentProcessData( pEventContext, &AskAction );
	AssignChildProcessesData( pEventContext, &AskAction );
#endif

	PR_TRACE(( g_root, prtIMPORTANT, TR "processing: %S process='%d:%S'",
		gEventInfo[ItemIdx].m_pwchDescribe, 
		AskAction.m_CurrentProcess.m_ProcessId,
		AskAction.m_CurrentProcess.m_ImagePath.data()
		));

	cObject* _this = pEventContext->m_pInternal->m_pThis;

	tERROR error = _this->sysSendMsg(pmc_MATCH_EXCLUDES, 0, NULL, &AskAction, SER_SENDMSG_PSIZE);
	if (errOK_DECIDED == error)
	{
		PR_TRACE(( g_root, prtIMPORTANT, TR "ask skipped by Excludes!" ));

		Action = pdm2_allow;
		bLog = cFALSE;
	}
	else
	{
		CalcUnique(_this, &AskAction.m_qwUniqueId, AskAction.m_strObjectName, AskAction.m_EventType);

		cInfectedObjectInfo pInfectedInfo( AskAction );
		error = _this->sysSendMsg(pmc_UPDATE_THREATS_LIST, 0, _this, (cInfectedObjectInfo*)&pInfectedInfo, SER_SENDMSG_PSIZE);
		
		PR_TRACE(( g_root, prtIMPORTANT, TR "send to threats result %terr", error ));

		if (bLog)
		{
			AskAction.m_Action = cAskObjectAction_PDM2::DETECT;
			_this->sysSendMsg( pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE );
		}
	}

	switch (Action)
	{
	case pdm2_allow:
		AskAction.m_nResultAction = ACTION_ALLOW;
		break;

	case pdm2_ask:
		if (PR_SUCC( _this->sysSendMsg( pmcASK_ACTION, cAskObjectAction_PDM2::ASK, 0, &AskAction, SER_SENDMSG_PSIZE ) ))
		{
			switch (AskAction.m_nResultAction)
			{
			case ACTION_DENY:
				Action = pdm2_block;
				break;

			case ACTION_REPORTONLY:
			case ACTION_ALLOW:
				Action = pdm2_allow;
				break;

			case ACTION_TERMINATE:
				Action = pdm2_terminate;
				break;
				
			case ACTION_QUARANTINE:
				Action = pdm2_quarantine;
				break;
			}
		}
		break;

	case pdm2_block:
		AskAction.m_nResultAction = ACTION_DENY;
		break;

	case pdm2_terminate:
		AskAction.m_nResultAction = ACTION_TERMINATE;
		break;

	case pdm2_quarantine:
		AskAction.m_nResultAction = ACTION_QUARANTINE;
		break;
	}

	if (bLog)
	{
		AskAction.m_Action = cAskObjectAction_PDM2::ASK;
		_this->sysSendMsg( pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE );
	}

	pEventContext->m_pProcess->ReleaseRef();

	return Action;
}

void
TraceBeforeRequest (
	ULONG RefCount,
	PVOID pEventHdr
	)
{
	PMKLIF_EVENT_HDR pEventHdrTmp = (PMKLIF_EVENT_HDR) pEventHdr;
	
	if (!pEventHdrTmp)
		return;

	PR_TRACE(( g_root, prtERROR, TR "make request for event 0x%x (hdr 0x%x). event refcount %x", 
		pEventHdrTmp->m_DrvMark, pEventHdrTmp, RefCount ));
}

tERROR
pr_call
ThreadInfoCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;

	case TP_CBTYPE_TASK_PROCESS:
		{
			PEventContext pEventContext = (PEventContext) pTaskData;

			PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pEventContext->m_SE.m_pEventHdr;
			
			PR_TRACE(( g_root, prtIMPORTANT, TR "processing pending event 0x%x (hdr 0x%x) ptr 0x%x", 
				pEventHdr->m_DrvMark, pEventHdr, pEventContext->m_SE.m_pMessage ));

			while (true)
			{
				if (pEventHdr->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK)
					break;

				PR_TRACE(( g_root, prtIMPORTANT, TR "waiting direct answer" ));
				Sleep( 100 );
			}

			SetThreadToken( NULL, NULL );

			if (pEventContext->m_pRefData->m_hTokenImpersonate)
				SetThreadToken( NULL, pEventContext->m_pRefData->m_hTokenImpersonate );

			//+ send ask action!
			cAskObjectAction_PDM2 AskAction;
			{
				PDM2RTActions Action = ProceedRequest( pEventContext, AskAction );
				switch (Action)
				{
				case pdm2_allow:
					pEventContext->m_pInternal->m_Statistics.m_nTreatsAllowed++;
					break;

				case pdm2_block:
					pEventContext->m_pRefData->m_VerdictFlags |= efVerdict_Deny;
					pEventContext->m_pInternal->m_Statistics.m_nTreatsBlocked++;
					break;

				case pdm2_quarantine:
					pEventContext->m_pRefData->m_VerdictFlags |= efVerdict_Quarantine;
				case pdm2_terminate:
					pEventContext->m_pRefData->m_VerdictFlags |= efVerdict_Deny | efVerdict_TerminateProcess;
					pEventContext->m_pInternal->m_Statistics.m_nTreatsBlocked++;
					
					break;
				}
			}

			//- send ask action!
			bool bTermSucc = false;
			pEventContext->m_pRefData->m_RefCount--;

			PR_TRACE(( g_root, prtERROR, TR "end ask - event refcount %d", pEventContext->m_pRefData->m_RefCount ));
			
			if (!pEventContext->m_pRefData->m_RefCount)
			{
				PR_TRACE(( g_root, prtERROR, TR "result event flags 0x%x", pEventContext->m_pRefData->m_VerdictFlags ));

				char buff[sizeof(MKLIF_OBJECT_REQUEST) + sizeof(MKLIF_REPLY_EVENT)];
				memset( buff, 0, sizeof(buff) );

				PMKLIF_OBJECT_REQUEST pRequest = (PMKLIF_OBJECT_REQUEST) buff;
				PMKLIF_REPLY_EVENT pDrvVerdict = (PMKLIF_REPLY_EVENT) pRequest->m_Buffer;

				pRequest->m_RequestBufferSize = sizeof(MKLIF_REPLY_EVENT);
				pRequest->m_RequestType = _object_request_set_verdict;
				pDrvVerdict->m_VerdictFlags = pEventContext->m_pRefData->m_VerdictFlags;

				// reset quarantine flag for driver
				pDrvVerdict->m_VerdictFlags &= ~efVerdict_Quarantine;

				MKLIF_SETVERDICT_RESULT VerdictResult;
				ULONG VerdictResultSize = sizeof(VerdictResult);
				memset( &VerdictResult, 0, VerdictResultSize );

				HRESULT hResult = MKL_ObjectRequest( 
					pEventContext->m_pInternal->m_pClientContext, 
					pEventContext->m_SE.m_pMessage,
					pRequest,
					sizeof(buff),
					&VerdictResult,
					&VerdictResultSize
					);

				if (HRESULT_FROM_WIN32( ERROR_NOT_FOUND ) == hResult)
				{
					PR_TRACE(( g_root, prtERROR, TR "Event not found on verdict!" ));
					if (pDrvVerdict->m_VerdictFlags & efVerdict_TerminateProcess)
					{
						hResult = MKL_TerminateProcess (
							pEventContext->m_pInternal->m_pClientContext,
							pEventHdr->m_ProcessId
							);

						PR_TRACE(( g_root, prtIMPORTANT, TR "manual terminate process %d status 0x%x", 
							pEventHdr->m_ProcessId, hResult ));
						
						if (SUCCEEDED( hResult ))
						{
							pDrvVerdict->m_VerdictFlags &= efVerdict_TerminateProcess;
							bTermSucc = true;
						}
					}
				}

				// if terminate success - report event
				if (pDrvVerdict->m_VerdictFlags & efVerdict_TerminateProcess)
				{
					pEventContext->m_pInternal->m_Statistics.m_nTreatsTerminated++;

					// TODO: как тут определить, убили процесс или нет?
					if (VerdictResultSize >= sizeof(MKLIF_SETVERDICT_RESULT))
					{
						PR_TRACE(( g_root, prtERROR, TR "verdict result size %d, status 0x%x", VerdictResultSize, VerdictResult.m_Status ));

						if (0/*STATUS_SUCCESS*/ == VerdictResult.m_Status)
							bTermSucc = true;
					}
					
					AskAction.m_Action = bTermSucc ? cAskObjectAction_PDM2::TERMINATE_INFO : cAskObjectAction_PDM2::TERMINATE_FAILED;
					AskAction.m_nError = bTermSucc ? errOK : errACCESS_DENIED;				
					AskAction.m_nActionsAll = AskAction.m_nActionsMask = ACTION_OK;
					pEventContext->m_pInternal->m_pThis->sysSendMsg(pmcASK_ACTION, AskAction.m_Action, 0, &AskAction, SER_SENDMSG_PSIZE);
					if (pEventContext->m_pInternal->m_Settings.m_bLog)
					{
						pEventContext->m_pInternal->m_pThis->sysSendMsg(
							bTermSucc ? pmc_EVENTS_NOTIFY : pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE);
					}
				}

				PR_TRACE(( g_root, prtIMPORTANT, TR "free pending event 0x%x ptr 0x%x", 
					pEventHdr->m_DrvMark, pEventContext->m_SE.m_pMessage ));

				pfMemFree( NULL, &pEventContext->m_SE.m_pMessage );

				if (efVerdict_Quarantine & pEventContext->m_pRefData->m_VerdictFlags)
				{
					// TODO: quarantine if terminate successful
					tERROR errQuarantined = pEventContext->m_pInternal->m_pThis->sysSendMsg (
						pmcASK_ACTION,
						cAskObjectAction_PDM2::QUARANTINE,
						0,
						&AskAction,
						SER_SENDMSG_PSIZE
						);

					PR_TRACE(( g_root, prtERROR, TR "quarantine result %terr", errQuarantined ));

					if (PR_SUCC( errQuarantined ))
					{
						pEventContext->m_pInternal->m_Statistics.m_nTreatsQuarantined++;

						BOOL bDeleted = DeleteFile( AskAction.m_CurrentProcess.m_ImagePath.data() );
						if (!bDeleted)
						{
							Sleep( 100 );
							bDeleted = DeleteFile( AskAction.m_CurrentProcess.m_ImagePath.data() );
						}
						
						PR_TRACE(( g_root, prtERROR, TR "delete file result %s. error %d", 
							bDeleted ? "ok" : "failed", 
							bDeleted ? 0 : GetLastError() ));
					}
				}

				if (pEventContext->m_pRefData->m_hTokenImpersonate)
				{
					CloseHandle( pEventContext->m_pRefData->m_hTokenImpersonate );
					pEventContext->m_pRefData->m_hTokenImpersonate = NULL;
				}

				pfMemFree( NULL, (void**) &pEventContext->m_pRefData );
			}
		}
		break;

	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}

tERROR
pr_call
ThreadEvCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;

	case TP_CBTYPE_TASK_PROCESS:
		{
			PsInternal pInternal = *(PsInternal*) pTaskData;

			//PR_TRACE(( g_root, prtERROR, TR "incoming task..." ));

			PEventRoot pEvents;
			while (true) // main loop
			{
				//////////////////////////////////////////////////////////////////////////
				pEvents = NULL;

				pInternal->m_EventSync.LockSync();

				// search working
				if (!IsListEmpty( &pInternal->m_Events ))
				{
					PLIST_ENTRY Flink = pInternal->m_Events.Flink;

					PEventRoot pRoot = NULL;
					while (Flink != &pInternal->m_Events)
					{
						pRoot = CONTAINING_RECORD( Flink, EventRoot, m_List );
						Flink = Flink->Flink;

						if (pRoot->m_Thread)
						{
							//PR_TRACE(( g_root, prtERROR, TR "pid %d busy by thread %d", pRoot->m_pid, pRoot->m_Thread ));
						}
						else
						{
							if (!IsListEmpty( &pRoot->m_EventList ))
							{
								pEvents = pRoot;
								pRoot->m_Thread = GetCurrentThreadId();

								//PR_TRACE(( g_root, prtERROR, TR "processing pid %d", pRoot->m_pid ));

								break;
							}
						}
					}
				}

				pInternal->m_EventSync.UnLockSync();

				if (!pEvents)
					break;

				//////////////////////////////////////////////////////////////////////////
				// processing events
				PSingleEvent pse = NULL;

				while (true)
				{
					pInternal->m_EventSync.LockSync();

					if (IsListEmpty( &pEvents->m_EventList ))
					{
						pse = NULL;
						pEvents->m_Thread = 0; // release list for new thread
						//PR_TRACE(( g_root, prtERROR, TR "release pid %d", pEvents->m_pid ));
					}
					else
					{
						pse = CONTAINING_RECORD( pEvents->m_EventList.Flink, SingleEvent, m_List );
						RemoveEntryList( &pse->m_List );
					}

					pInternal->m_EventSync.UnLockSync();

					if (!pse)
						break;

					//////////////////////////////////////////////////////////////////////////
					// process one event
					//PR_TRACE(( g_root, prtERROR, TR "processing single event..." ));

					ProcessSingleEvent( pInternal, pse );

					pfMemFree( NULL, (void**) &pse );
				}

				//! todo: delete empty list
			}
		}
		break;

	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}


bool AssignEventToThread (
	PsInternal pInternal,
	PVOID pMessage,
	ULONG MessageSize,
	PMKLIF_EVENT_HDR pEventHdr
	)
{
	bool bRet = true;
	ULONG ProcessingPid = pEventHdr->m_ProcessId;

	if ((FLTTYPE_SYSTEM == pEventHdr->m_HookID)
		&& ((MJ_CREATE_PROCESS_NOTIFY == pEventHdr->m_FunctionMj)
			|| (MJ_EXIT_PROCESS == pEventHdr->m_FunctionMj)))
	{
		ProcessingPid = 0;
	}
	else
	{
		//+ watch system account
		if (!pInternal->m_Settings.m_bWatchSystemAccount)
		{
			PSINGLE_PARAM pParamLuid = MKL_GetEventParam (
				pMessage,
				MessageSize,
				_PARAM_OBJECT_LUID,
				FALSE
				);

			if (pParamLuid && (sizeof(LUID) == pParamLuid->ParamSize))
			{
				PLUID pLuid = (PLUID) pParamLuid->ParamValue;

				bool bSystemLuid = false;
				LUID SystemLuid = SYSTEM_LUID;

				if (!memcmp( &SystemLuid, pLuid, sizeof(LUID) ))
					bSystemLuid = true;

				if (bSystemLuid)
				{
					if (FLTTYPE_NFIOR == pEventHdr->m_HookID)
					{
						/*PR_TRACE(( g_root, prtIMPORTANT, TR "skip system file request %d:%d luid 0x%x:%x",
							pEventHdr->m_HookID,
							pEventHdr->m_FunctionMj,
							pLuid->HighPart,
							pLuid->LowPart ));*/

						return false;
					}

					PR_TRACE(( g_root, prtIMPORTANT, TR "process system request %d:%d luid 0x%x:%x",
						pEventHdr->m_HookID,
						pEventHdr->m_FunctionMj,
						pLuid->HighPart,
						pLuid->LowPart ));

				}
			}
		}
		//- watch system account
	}

	PSingleEvent pse = (PSingleEvent) pfMemAlloc( NULL, sizeof(SingleEvent), 0 );
	if (!pse)
		return false;

	pse->m_pMessage = pMessage;
	pse->m_MessageSize = MessageSize;
	pse->m_pEventHdr = pEventHdr;

	bool bRootFound = false;

	pInternal->m_EventSync.LockSync();

	pInternal->m_Statistics.m_nDriverEvents++;

	if (!IsListEmpty( &pInternal->m_Events ))
	{
		PLIST_ENTRY Flink = pInternal->m_Events.Flink;

		PEventRoot pRoot = NULL;
		while (Flink != &pInternal->m_Events)
		{
			pRoot = CONTAINING_RECORD( Flink, EventRoot, m_List );
			Flink = Flink->Flink;

			if (pRoot->m_pid == ProcessingPid)
			{
				bRootFound = true;

				InsertTailList( &pRoot->m_EventList, &pse->m_List );
				break;
			}
		}
	}

	if (!bRootFound)
	{
		PR_TRACE(( g_root, prtERROR, TR "new root for pid %d", ProcessingPid ));

		PEventRoot pNewRoot = (PEventRoot) pfMemAlloc( NULL, sizeof(EventRoot), 0 );
		if (pNewRoot)
		{
			pNewRoot->m_pid = ProcessingPid;
			pNewRoot->m_Thread = 0;

			InitializeListHead( &pNewRoot->m_EventList );
			InsertTailList( &pNewRoot->m_EventList, &pse->m_List );

			InsertTailList( &pInternal->m_Events, &pNewRoot->m_List );
		}
		else
		{
			// no memory and root not found
			bRet = false;
		}
	}

	pInternal->m_EventSync.UnLockSync();

	tTaskId task_drv;
	pInternal->m_ThPoolEvents->AddTask( &task_drv, ThreadEvCallback, &pInternal, sizeof(pInternal), 0 );

	return bRet;
}


void
QueryModInfo (
	PsInternal pInternal,
	ULONG ProcessId
	)
{
	PVOID pModuleInfo = NULL;
	ULONG ModuleInfoLen = 0;

	HRESULT hResult = MKL_QueryProcessModules (
		pInternal->m_pClientContext,
		ProcessId,
		&pModuleInfo,
		&ModuleInfoLen
		);

	if (SUCCEEDED(hResult) )
	{
		ULONG EnumContext = 0;
		PWCHAR pwchImagePath = NULL;
		LARGE_INTEGER ImageBase;
		ULONG SizeOfImage;
		LARGE_INTEGER EntryPoint;

		while( true)
		{
			hResult = MKL_EnumModuleInfo (
				pModuleInfo,
				ModuleInfoLen,
				&EnumContext,
				&pwchImagePath,
				&ImageBase,
				&SizeOfImage,
				&EntryPoint
				);

			if (!SUCCEEDED(hResult) )
				break;

			PR_TRACE(( g_root, prtERROR, TR "\t%S base: %I64x size: %d entry point: %I64x (%d)",
				pwchImagePath,
				ImageBase,
				SizeOfImage,
				EntryPoint,
				EnumContext
				));
		}

		pfMemFree( NULL, &pModuleInfo );
	}
}

void
RegisterProcessesOnStart (
	PsInternal pInternal
	)
{
	PVOID pProcessInfo = NULL;
	ULONG ProcessInfoLen = 0;

	HRESULT hResult = MKL_QueryProcessesInfo( pInternal->m_pClientContext, &pProcessInfo, &ProcessInfoLen );

	PR_TRACE(( g_root, prtERROR, TR "QueryProcessesInfo: 0x%x", hResult ));
	if (!SUCCEEDED(hResult) )
		return;

	ULONG EnumContext = 0;
	ULONG ProcessId = 0;
	ULONG ParentProcessId = 0;
	LARGE_INTEGER StartTime;
	PWCHAR pwchImagePath = NULL;
	PWCHAR pwchBaseFolder = NULL;
	PWCHAR pwchCmdLine = NULL;

	StartTime.QuadPart = 0;

	PR_TRACE(( g_root, prtERROR, TR "enum process (on start):", hResult ));

	while( true)
	{
		hResult = MKL_EnumProcessInfo (
			pProcessInfo,
			ProcessInfoLen,
			&EnumContext,
			&ProcessId,
			&ParentProcessId,
			&StartTime,
			&pwchImagePath,
			&pwchBaseFolder,
			&pwchCmdLine
			);

		if (!SUCCEEDED(hResult) )
			break;
		
		PR_TRACE(( g_root, prtERROR, TR "\tpid %d(parent %d) from '%S'", 
			ProcessId,
			ParentProcessId,
			pwchImagePath
			));

		PR_TRACE(( g_root, prtERROR, TR "\t\t'%S'", pwchBaseFolder ));
		PR_TRACE(( g_root, prtERROR, TR "\t\t'%S'", pwchCmdLine ));

		RegisterProcess (
			pInternal,
			ProcessId,
			ParentProcessId,
			StartTime,
			pwchImagePath,
			pwchBaseFolder,
			pwchCmdLine
			);


//		QueryModInfo( pInternal, ProcessId );
	}

	pfMemFree( NULL, &pProcessInfo );
}

tERROR
pr_call
ThreadDrvCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;

	case TP_CBTYPE_TASK_PROCESS:
		{
			HRESULT hResult = S_OK;
			PVOID pMessage;
			PMKLIF_EVENT_HDR pEventHdr;
			ULONG MessageSize;
			
			PsInternal pInternal = *(PsInternal*) pTaskData;
			PR_TRACE(( g_root, prtERROR, TR "internal ptr (in): 0x%x", pInternal ));

			MKL_AddInvisibleThread( pInternal->m_pClientContext );

			hResult = S_OK;
			while(true)
			{
				if (E_PENDING == hResult)
					hResult = S_OK; // already allocated
				else
					hResult = MKL_GetMessage( pInternal->m_pClientContext, &pMessage, &MessageSize, &pEventHdr );

				if (SUCCEEDED(hResult) )
				{
					hResult = MKL_GetMultipleMessage (
						pInternal->m_pClientContext,
						&pMessage,
						&MessageSize,
						&pEventHdr,
						1000
						);

					if (!SUCCEEDED(hResult) )
					{
						if (E_PENDING == hResult)
						{
							// timeout
							if (pInternal->m_bStop)
							{
								PR_TRACE(( g_root, prtERROR, TR "exit command" ));
								pfMemFree( NULL, &pMessage );
								break;
							}

							continue;
						}
						else
						{
							PR_TRACE(( g_root, prtERROR, TR "wrong state, hresult 0x%x (native err 0x%x). exit!", 
								hResult, MKL_GetLastNativeError(pInternal->m_pClientContext) ));
							break;
						}
					}
					else
					{
						/*PR_TRACE(( g_root, prtERROR, TR "income event 0x%x, process %d hook 0x%x:%x, flags 0x%x",
							pEventHdr->m_DrvMark,
							pEventHdr->m_ProcessId,
							pEventHdr->m_HookID,
							pEventHdr->m_FunctionMj,
							pEventHdr->m_EventFlags
							));*/

						if (pInternal->m_bStop || !AssignEventToThread( pInternal, pMessage, MessageSize, pEventHdr ))
						{
							MKLIF_REPLY_EVENT Verdict;	// default answer
							memset( &Verdict, 0, sizeof(Verdict) );

							hResult = MKL_ReplyMessage( pInternal->m_pClientContext, pMessage, &Verdict );
							
							pfMemFree( NULL, &pMessage );
						}
					}
				}
				else
				{
					PR_TRACE(( g_root, prtERROR, TR "get message failed, hresult 0x%x. exit!", hResult ));
				}
			}
			
			PR_TRACE(( g_root, prtERROR, TR "loop exit" ));
		}
		break;

	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}


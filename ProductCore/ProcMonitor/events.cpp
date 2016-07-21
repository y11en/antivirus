#include "events.h"
#include <strsafe.h>
#include <prague/pr_cpp.h>
#include <ProductCore/report.h>
#include <Prague/plugin/p_win32_nfio.h>

#ifndef _DEF_LIST_
#define _DEF_LIST_

/*
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;
*/

#ifndef FORCEINLINE
#if (MSC_VER >= 1200)
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE __inline
#endif
#endif


VOID
FORCEINLINE
InitializeListHead(
    IN PLIST_ENTRY ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))


BOOLEAN
FORCEINLINE
RemoveEntryList(
    IN PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOLEAN)(Flink == Blink);
}

PLIST_ENTRY
FORCEINLINE
RemoveHeadList(
    IN PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

PLIST_ENTRY
FORCEINLINE
RemoveTailList(
    IN PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}


VOID
FORCEINLINE
InsertTailList(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}


VOID
FORCEINLINE
InsertHeadList(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)



#endif //_DEF_LIST_

static LONG gMemCounter = 0;

static void* __cdecl pfMemAlloc (
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

static void __cdecl pfMemFree (
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

cEnvironmentHelperProcmon::cEnvironmentHelperProcmon(CProcMon* pProcmon, PVOID pOpaquePtr, w32_pfMemAlloc pfW32Alloc, w32_pfMemFree pfW32Free) :
	cEnvironmentHelperWin32W(pOpaquePtr, pfW32Alloc, pfW32Free),
	m_pProcmon(pProcmon)
{
}

cEnvironmentHelperProcmon::~cEnvironmentHelperProcmon()
{
}

const tProcessInfo* FindProcessInfo(tPid native_pid, uint64 ftStartTime, const tProcessInfo* pi, size_t nInfoCount)
{
	while (nInfoCount--)
	{
		const tProcessInfo* i = pi+nInfoCount;
		if (native_pid != i->m_NativePid)
			continue;
		if (0 != ftStartTime && UINT64_MAX != ftStartTime)
		{
			if (0 == i->m_ftStartTime)
				continue;
			if (UINT64_MAX == i->m_ftStartTime)
				continue;
			if (ftStartTime < i->m_ftStartTime - 1*(10^6))
				continue;
			if (ftStartTime > i->m_ftStartTime + 1*(10^6))
				continue;
		}
		return i;
	}
	return NULL;
}

bool cEnvironmentHelperProcmon::ProcessAssignUniqPid(cProcessListItem* pProcess)
{
	const int nMaxProcessLookup = 1000;
	tProcessInfo pi[nMaxProcessLookup];
	if (!pProcess)
		return false;
	hREPORTDB hReportDB = m_pProcmon->m_ReportDB;
	//if (0 == pProcess->m_ftStartTime || UINT64_MAX == pProcess->m_ftStartTime)
	//{
	//	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pProcess->m_pid);
	//	if (hProcess != INVALID_HANDLE_VALUE)
	//	{
	//		FILETIME dummy;
	//		BOOL bRes = GetProcessTimes(hProcess, (LPFILETIME)&pProcess->m_ftStartTime, (LPFILETIME)&pProcess->m_ftExitTime, &dummy, &dummy);
	//		CloseHandle(hProcess);
	//		if (!bRes)
	//			return false;
	//	}
	//}
	tDWORD pc = 0;
	uint64_t time_10sec = GetTimeLocal() - 10*10000000;
	if (pProcess->m_ftStartTime < time_10sec)
	{
		hReportDB->GetEvents(dbProcesses, -nMaxProcessLookup, &pi, sizeof(pi), nMaxProcessLookup, &pc);
		const tProcessInfo* fpi = FindProcessInfo(pProcess->m_pid, pProcess->m_ftStartTime, pi, pc);
		if (fpi)
		{
			pProcess->m_uniq_pid = fpi->m_UniqPid;
			pProcess->m_AppId = fpi->m_AppId;
			if (fpi->m_ParentUniqPid)
				pProcess->m_parent_uniq_pid = fpi->m_ParentUniqPid;
			return true;
		}
	}
	tProcessInfo npi = {0};
	hReportDB->FindObject(dbGlobalObjects, eFile, (const tPTR)pProcess->m_orig_image.get(), cSIZE_WSTR, cTRUE, 0, &npi.m_ImageFileID);
	if (pProcess->m_cmdline)
		hReportDB->FindObject(dbProcesses, eString, pProcess->m_cmdline, cSIZE_WSTR, cTRUE, RF_UNICODE, &npi.m_CmdLineID);
	npi.m_NativePid = (uint32)pProcess->m_pid;
	npi.m_ParentNativePid = (uint32)pProcess->m_parent_pid;
	npi.m_ParentUniqPid = pProcess->m_parent_uniq_pid;
	// TODO: make better AppId resolving method
	npi.m_AppId = (uint32)npi.m_ImageFileID;
	if (!npi.m_ParentUniqPid && pc)
	{
		if (!pc)
			hReportDB->GetEvents(dbProcesses, -nMaxProcessLookup, &pi, sizeof(pi), nMaxProcessLookup, &pc);
		const tProcessInfo* fpi = FindProcessInfo(pProcess->m_parent_pid, 0, pi, pc);
		if (fpi)
			npi.m_ParentUniqPid = fpi->m_UniqPid;
	}
	npi.m_ftStartTime = pProcess->m_ftStartTime;
	npi.m_ftExitTime = pProcess->m_ftExitTime;
	if (PR_FAIL(hReportDB->AddEvent(dbProcesses, &npi, sizeof(npi), &npi.m_UniqPid)))
		return false;
	pProcess->m_uniq_pid = npi.m_UniqPid;
	pProcess->m_AppId = npi.m_AppId;
	hReportDB->UpdateEvent(npi.m_UniqPid, &npi, sizeof(npi)); // store uniq pid in event
	return true;
}

cProcmonEventProvider::cProcmonEventProvider (
					  CProcMon* pProcMon
					  ) :
	m_envhlpr(pProcMon, NULL, pfMemAlloc, pfMemFree ),
	m_event_mgr( &m_envhlpr )
{
//	m_event_mgr.RegisterHandler( &m_heuristic, false );
//	m_event_mgr.RegisterHandler( &pdm, false );

	m_SlotIdx = -1;

	if (m_event_mgr.m_pProcessList)
		m_event_mgr.m_pProcessList->AllocSlot( &m_SlotIdx );

	m_pClientContext = NULL;
	m_bStop = false;
	InitializeListHead( &m_Events );
	m_pThPoolDrv = NULL;
	m_pThPoolEvents = NULL;
	m_pProcMon = pProcMon;

	m_bWatchSystemAccount = false;
	m_nDriverEvents = 0;
	m_bInited = false;
};


cProcmonEventProvider::~cProcmonEventProvider()
{
	if (m_bInited)
		Done();
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

	cProcmonEventProvider* pEventProvider = (cProcmonEventProvider*) pCommonThreadContext;

	switch (CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		if (pEventProvider && pEventProvider->m_pClientContext)
		{
			HRESULT hResult = MKL_AddInvisibleThread( pEventProvider->m_pClientContext );
			if (!SUCCEEDED( hResult ))
			{
				PR_TRACE(( g_root, prtERROR, TR "new thread - result 0x%x. pEventProvider 0x%x, client context 0x%x", 
					hResult, pEventProvider, pEventProvider->m_pClientContext ));
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
				 cProcmonEventProvider* pEventProvider,
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
	cFile image_path( &pEventProvider->m_envhlpr, pwchImagePath );
	cPath working_folder( &pEventProvider->m_envhlpr, pwchBaseFolder );
	tcstring cmd_line = pwchCmdLine;
	uint64_t start_time = *(uint64_t*) &StartTime;

	EventContext evcon( pEventProvider, NULL );
	cEvent event( &evcon, parent_pid, 0 );

	event.SetTime(start_time);
	pEventProvider->m_event_mgr.OnProcessCreatePost(
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
					cProcmonEventProvider* pEventProvider,
					PSingleEvent pse
					)
{
	tVerdict Verdict = evtVerdict_Default;

	PVOID pClientContext = pEventProvider->m_pClientContext;
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
				pEventProvider,
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

			EventContext evcon( pEventProvider, pse );
			cEvent event( &evcon, pid, 0 );
			//Verdict = pEventProvider->m_event_mgr.OnProcessExitPost( event );
			pEventProvider->m_event_mgr.OnProcessExitPost( event );
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

			EventContext evcon( pEventProvider, pse );
			cEvent event( &evcon, CurrentPid, pEventHdr->m_ThreadId );

			pEventProvider->m_event_mgr.OnThreadCreatePost (
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
			EventContext evcon( pEventProvider, pse );
			cEvent event( &evcon, CurrentPid, ThreadId );
			pEventProvider->m_event_mgr.OnThreadTerminatePost( event, CurrentPid, ThreadId );
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
				  cProcmonEventProvider* pEventProvider,
				  PSingleEvent pse
				  )
{
	tVerdict Verdict = evtVerdict_Default;

	PVOID pClientContext = pEventProvider->m_pClientContext;
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

#ifdef _DEBUG
	if (wcsstr(pwchFileName, L"Visual Assist"))
		return evtVerdict_Default;
#endif

	//	PR_TRACE(( g_root, prtSPAM, TR "op 0x%x file name '%S'", pEventHdr->m_FunctionMj, pwchFileName ));

	EventContext evcon( pEventProvider, pse );
	cEvent event( &evcon, pEventHdr->m_ProcessId, pEventHdr->m_ThreadId );

	HandleImp hDriverHandle;
	tHANDLE hFile = INVALID_HANDLE;
	if ((pEventHdr->m_EventFlags & _EVENT_FLAG_OBJECT_ATTENDED) && (ContextFlags & _CONTEXT_OBJECT_FLAG_FORREAD))
	{
		hDriverHandle.ConnectHandle(pEventProvider->m_pClientContext, pMessage, MessageSize);
		hFile = (tHANDLE) &hDriverHandle;
	}
	cFile image (
		&pEventProvider->m_envhlpr,
		pwchFileName,
		NULL,
		FILE_ATTRIBUTE_UNDEFINED,
		hFile,
		unique
		);

	switch (pEventHdr->m_FunctionMj)
	{
	case IRP_MJ_SET_INFORMATION:
		if (ContextFlags & _CONTEXT_OBJECT_FLAG_DIRECTORY)
			break;
		switch (pEventHdr->m_FunctionMi)
		{
		case FileRenameInformation:
			{
				PWCHAR pwchFileNameDest = NULL;
				ULONG FileNameDestLen;

				if (MakeFileName(pClientContext, pMessage, MessageSize, ContextFlags, &pwchFileNameDest, &FileNameDestLen, true))
				{
					PR_TRACE(( g_root, prtNOTIFY, TR "%s rename '%S' -> '%S'", 
						bPostEvent ? "(post)" : "(pre)",
						pwchFileName,
						pwchFileNameDest
						));

					cFile imagedst (
						&pEventProvider->m_envhlpr,
						pwchFileNameDest,
						NULL,
						FILE_ATTRIBUTE_UNDEFINED,
						INVALID_HANDLE,
						unique
						);

					if (bPostEvent)
						pEventProvider->m_event_mgr.OnFileRenamePost( event, image, imagedst );
					else
						pEventProvider->m_event_mgr.OnFileRenamePre( event, image, imagedst );
				}

				pfMemFree( NULL, (PVOID*) &pwchFileNameDest );
			}
			break;
		}
		break;
	case IRP_MJ_CLEANUP:
		{
			if (ContextFlags & _CONTEXT_OBJECT_FLAG_DIRECTORY)
				break;
			if (ContextFlags & (_CONTEXT_OBJECT_FLAG_MODIFIED | _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT))
			{
				// closing new or modified file
				PR_TRACE(( g_root, prtERROR, TR "event 0x%x: close modified (%S) file name '%S' (unique 0x%I64x)",
					pEventHdr->m_DrvMark,
					pEventHdr->m_EventFlags & _EVENT_FLAG_POSTPROCESSING ? L"post" : L"pre",
					pwchFileName,
					unique
					));

				if (bPostEvent)
					pEventProvider->m_event_mgr.OnFileCloseModifiedPost( event, image );
				else
					pEventProvider->m_event_mgr.OnFileCloseModifiedPre( event, image );
			}
			if (ContextFlags & _CONTEXT_OBJECT_FLAG_DELETEPENDING)
			{
				// delete file
				PR_TRACE(( g_root, prtERROR, TR "event 0x%x: delete (%S) file name '%S' (unique 0x%I64x)",
					pEventHdr->m_DrvMark,
					pEventHdr->m_EventFlags & _EVENT_FLAG_POSTPROCESSING ? L"post" : L"pre",
					pwchFileName,
					unique
					));

				if (bPostEvent)
					pEventProvider->m_event_mgr.OnFileDeletePost( event, image );
				else
					pEventProvider->m_event_mgr.OnFileDeletePre( event, image );
			}
		}
		break;
	case IRP_MJ_CREATE:
		{
			if (ContextFlags & _CONTEXT_OBJECT_FLAG_DIRECTORY)
				break;
			if (ContextFlags & _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT)
			{
				// opening file
				PR_TRACE(( g_root, prtERROR, TR "event 0x%x: pid %d create new file (%S) file name '%S' (unique 0x%I64x)",
					pEventHdr->m_DrvMark,
					pEventHdr->m_ProcessId,
					pEventHdr->m_EventFlags & _EVENT_FLAG_POSTPROCESSING ? L"post" : L"pre",
					pwchFileName,
					unique
					));

				if (bPostEvent)
					pEventProvider->m_event_mgr.OnFileCreateNewPost( event, image );
				else
					pEventProvider->m_event_mgr.OnFileCreateNewPre( event, image );
			}
		}
		break;
	}

	pfMemFree( NULL, (PVOID*) &pwchFileName );

	Verdict = event.GetVerdict();

	return Verdict;
}

#define _DEBUG_XP_HACK // REMOVE IT LATER

tVerdict
SingleEvent_Registry (
					  cProcmonEventProvider* pEventProvider,
					  PSingleEvent pse
					  )
{
	tVerdict Verdict = evtVerdict_Default;

	PVOID pClientContext = pEventProvider->m_pClientContext;
	PVOID pMessage = pse->m_pMessage;
	PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pse->m_pEventHdr;
	ULONG MessageSize = pse->m_MessageSize;

	PWCHAR pwchKeyName = NULL;
	PWCHAR pwchValueName = NULL;
	PVOID pData = NULL;
	ULONG nDataSize = 0;
	uint32_t nDataType = 0;
	bool bPostEvent = !!(pEventHdr->m_EventFlags & _EVENT_FLAG_POSTPROCESSING);

	PSINGLE_PARAM pParamKeyName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
	if (pParamKeyName && pParamKeyName->ParamSize)
		pwchKeyName = (PWCHAR) pParamKeyName->ParamValue;
	if (NULL == pwchKeyName)
		return evtVerdict_Default;

	PR_TRACE(( g_root, prtNOTIFY, TR "Registry %d %s '%S'", pEventHdr->m_FunctionMj, bPostEvent ? "(post)" : "(pre)", pwchKeyName ));

	if (pEventHdr->m_FunctionMj == Interceptor_SetValueKey || 
		pEventHdr->m_FunctionMj == Interceptor_QueryValueKey ||
		pEventHdr->m_FunctionMj == Interceptor_DeleteValueKey)
	{
		PSINGLE_PARAM pParamValueName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_PARAM_W, FALSE );
		if (pParamValueName && pParamValueName->ParamSize)
			pwchValueName = (PWCHAR) pParamValueName->ParamValue;
		if (NULL == pwchValueName)
			return evtVerdict_Default;

		PSINGLE_PARAM pParamVal = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_BINARYDATA, FALSE );
		if (pParamVal && pParamVal->ParamSize)
		{
			pData = pParamVal->ParamValue;
			nDataSize = pParamVal->ParamSize;
		}
		PULONG pDataType = NULL;
		PSINGLE_PARAM pParamType = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_SOURCE_ID, FALSE );
		if (pParamType && (sizeof(ULONG) == pParamType->ParamSize))
			nDataType = *(PULONG)pParamType->ParamValue;

		PR_TRACE(( g_root, prtNOTIFY, TR "\t'%S' type %d, len %d", pwchValueName, nDataType, nDataSize ));
	}

	cRegKey RegKey( 0, pwchKeyName );
	EventContext evcon( pEventProvider, pse );
	cEvent event( &evcon, pEventHdr->m_ProcessId, pEventHdr->m_ThreadId );
#ifdef _DEBUG_XP_HACK
	bPostEvent = !bPostEvent;
#endif

	switch (pEventHdr->m_FunctionMj)
	{
	case Interceptor_SetValueKey:
		{
			if (bPostEvent)
				pEventProvider->m_event_mgr.OnRegValueSetPost(event, RegKey, pwchValueName, nDataType, pData, nDataSize, 0, 0, 0);
			else
				pEventProvider->m_event_mgr.OnRegValueSetPre(event, RegKey, pwchValueName, nDataType, pData, nDataSize, 0, 0, 0);
			if (REG_SZ == nDataType || REG_EXPAND_SZ == nDataType || REG_MULTI_SZ == nDataType)
			{
				if (bPostEvent)
					pEventProvider->m_event_mgr.OnRegValueSetStrPost(event, RegKey, pwchValueName, (PWCHAR)pData, NULL);
				else
					pEventProvider->m_event_mgr.OnRegValueSetStrPre (event, RegKey, pwchValueName, (PWCHAR)pData, NULL);
			}
		}
		break;
	case Interceptor_DeleteValueKey:
		{
			if (bPostEvent)
				pEventProvider->m_event_mgr.OnRegValueDeletePost(event, RegKey, pwchValueName, nDataType, pData, nDataSize);
			else
				pEventProvider->m_event_mgr.OnRegValueDeletePre(event, RegKey, pwchValueName, nDataType, pData, nDataSize);
			if (REG_SZ == nDataType || REG_EXPAND_SZ == nDataType || REG_MULTI_SZ == nDataType)
			{
				if (bPostEvent)
					pEventProvider->m_event_mgr.OnRegValueDeleteStrPost(event, RegKey, pwchValueName, (PWCHAR)pData);
				else
					pEventProvider->m_event_mgr.OnRegValueDeleteStrPre (event, RegKey, pwchValueName, (PWCHAR)pData);
			}
		}
		break;
	case Interceptor_CreateKey:
		{
			if (bPostEvent)
				pEventProvider->m_event_mgr.OnRegKeyCreatePost(event, RegKey);
		}
		break;
	case Interceptor_RenameKey:
		{
			PWCHAR pwchDestKeyName = NULL;
			PSINGLE_PARAM pParamDestKeyName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_DEST_W, FALSE );
			if (pParamDestKeyName && pParamDestKeyName->ParamSize)
				pwchDestKeyName = (PWCHAR) pParamDestKeyName->ParamValue;
			if (NULL == pwchDestKeyName)
				return evtVerdict_Default;
			cRegKey DestRegKey(0, pwchDestKeyName);
			if (bPostEvent)
				pEventProvider->m_event_mgr.OnRegKeyRenamePost(event, RegKey, DestRegKey);
		}
		break;
	case Interceptor_DeleteKey:
		{
			if (bPostEvent)
				pEventProvider->m_event_mgr.OnRegKeyDeletePost(event, RegKey);
		}
		break;
	}

	return event.GetVerdict();
}

tVerdict
SingleEvent_Go (
				cProcmonEventProvider* pEventProvider,
				PSingleEvent pse,
				PMKLIF_EVENT_HDR pEventHdr
				)
{
	tVerdict Verdict = evtVerdict_Default;
	switch (pEventHdr->m_HookID)
	{
	case FLTTYPE_SYSTEM:
		Verdict = SingleEvent_System( pEventProvider, pse );
		break;
	case FLTTYPE_NFIOR:
		Verdict = SingleEvent_File( pEventProvider, pse );
		break;

	case FLTTYPE_REGS:
		Verdict = SingleEvent_Registry( pEventProvider, pse );
		break;
	}
	return Verdict;
}


void
ProcessSingleEvent (
					cProcmonEventProvider* pEventProvider,
					PSingleEvent pse
					)
{
	HRESULT hResult;

	PVOID pClientContext = pEventProvider->m_pClientContext;

	PMKLIF_EVENT_HDR pEventHdr = (PMKLIF_EVENT_HDR) pse->m_pEventHdr;
	PVOID pMessage = pse->m_pMessage;

	PR_TRACE(( g_root, prtNOTIFY, TR "start processing event 0x%x %x %x", pMessage, pEventHdr->m_HookID, pEventHdr->m_FunctionMj ));

	tVerdict Verdict = evtVerdict_Default;

	MKLIF_REPLY_EVENT DrvVerdict;	// default answer
	memset( &DrvVerdict, 0, sizeof(DrvVerdict) );

	bool bWasImpersonated = false;

	bool bUserTrusted = false;
	cEvent event( NULL, pEventHdr->m_ProcessId, pEventHdr->m_ThreadId );
	event.m_pEventMgr = &pEventProvider->m_event_mgr;

/*
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
				pEventProvider->m_SlotIdx,
				sizeof(sProcessInternalData)
				);

			if (!pData)
			{
				PR_TRACE(( g_root, prtERROR, TR "check trusted: alloc slot data failed. slot idx %d", pEventProvider->m_SlotIdx ));
			}
			else
			{
				if (!pData->m_bUserTrustedRequested)
				{
					PR_TRACE(( g_root, prtIMPORTANT, TR "check user trusted '%S'", pProcess->m_orig_image.getFull() ));

					cPRCMRequest req;
					tDWORD blen = sizeof(cPRCMRequest);

					req.m_Request = cPRCMRequest::_ePrcmn_ExclBehavior;
					req.m_ProcessId = (tDWORD) pProcess->m_pid;

					tERROR error = pEventProvider->m_pThis->sysSendMsg (
						pmc_PROCESS_MONITOR,
						pm_PROCESS_MONITOR_PROCESS_GETACTIONMASK,
						pEventProvider->m_pThis,
						&req,
						&blen
						);

					PR_TRACE(( g_root, prtIMPORTANT, TR "user trust request result %terr", error ));

					if (errOK_DECIDED == error)
					{
						pData->m_bUserTrusted = true;
						PR_TRACE(( g_root, prtIMPORTANT, TR "found user trusted '%S'", pProcess->m_orig_image.getFull() ));
					}

					pData->m_bUserTrustedRequested = true;
				}

				bUserTrusted = pData->m_bUserTrusted;
			}
		}
	}
*/

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

		Verdict = SingleEvent_Go( pEventProvider, pse, pEventHdr );
	}

	//////////////////////////////////////////////////////////////////////////
	PR_TRACE(( g_root, prtNOTIFY, TR "verdict 0x%x %08X",Verdict, pMessage ));

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
			cProcmonEventProvider* pEventProvider = *(cProcmonEventProvider**) pTaskData;

			//PR_TRACE(( g_root, prtERROR, TR "incoming task..." ));

			PEventRoot pEvents;
			while (true) // main loop
			{
				//////////////////////////////////////////////////////////////////////////
				pEvents = NULL;

				pEventProvider->m_EventSync.LockSync();

				// search working
				if (!IsListEmpty( &pEventProvider->m_Events ))
				{
					PLIST_ENTRY Flink = pEventProvider->m_Events.Flink;

					PEventRoot pRoot = NULL;
					while (Flink != &pEventProvider->m_Events)
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

				pEventProvider->m_EventSync.UnLockSync();

				if (!pEvents)
					break;

				//////////////////////////////////////////////////////////////////////////
				// processing events
				PSingleEvent pse = NULL;

				while (true)
				{
					pEventProvider->m_EventSync.LockSync();

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

					pEventProvider->m_EventSync.UnLockSync();

					if (!pse)
						break;

					//////////////////////////////////////////////////////////////////////////
					// process one event
					//PR_TRACE(( g_root, prtERROR, TR "processing single event..." ));

					ProcessSingleEvent( pEventProvider, pse );

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
						  cProcmonEventProvider* pEventProvider,
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
		if (!pEventProvider->m_bWatchSystemAccount)
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

	pEventProvider->m_EventSync.LockSync();

	pEventProvider->m_nDriverEvents++;

	if (!IsListEmpty( &pEventProvider->m_Events ))
	{
		PLIST_ENTRY Flink = pEventProvider->m_Events.Flink;

		PEventRoot pRoot = NULL;
		while (Flink != &pEventProvider->m_Events)
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

			InsertTailList( &pEventProvider->m_Events, &pNewRoot->m_List );
		}
		else
		{
			// no memory and root not found
			bRet = false;
		}
	}

	pEventProvider->m_EventSync.UnLockSync();

	tTaskId task_drv;
	pEventProvider->m_pThPoolEvents->AddTask( &task_drv, ThreadEvCallback, &pEventProvider, sizeof(pEventProvider), 0 );

	return bRet;
}

typedef LONG (__stdcall *ZWSETINFORMATIONTHREAD)(IN HANDLE ThreadHandle, IN LONG ThreadInformationClass, OUT PVOID ThreadInformation, IN ULONG ThreadInformationLength);

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

			cProcmonEventProvider* pEventProvider = *(cProcmonEventProvider**) pTaskData;
			PR_TRACE(( g_root, prtIMPORTANT, TR "internal ptr (in): 0x%x", pEventProvider ));

			MKL_AddInvisibleThread( pEventProvider->m_pClientContext );
			ZWSETINFORMATIONTHREAD pfZwSetInformationThread = (ZWSETINFORMATIONTHREAD)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "ZwSetInformationThread");
			if (pfZwSetInformationThread)
			{
				LONG ret = pfZwSetInformationThread( GetCurrentThread(), 17/*ThreadHideFromDebugger*/, NULL, 0 ); 
				PR_TRACE(( g_root, prtIMPORTANT, TR "HideFD %08X", ret));
			}

			hResult = S_OK;
			while(true)
			{
				if (E_PENDING == hResult)
					hResult = S_OK; // already allocated
				else
					hResult = MKL_GetMessage( pEventProvider->m_pClientContext, &pMessage, &MessageSize, &pEventHdr );

				if (SUCCEEDED(hResult) )
				{
					hResult = MKL_GetMultipleMessage (
						pEventProvider->m_pClientContext,
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
							if (pEventProvider->m_bStop)
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
								hResult, MKL_GetLastNativeError(pEventProvider->m_pClientContext) ));
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
						PR_TRACE(( g_root, prtNOTIFY, TR "received event 0x%x", pMessage ));

						if (pEventProvider->m_bStop || !AssignEventToThread( pEventProvider, pMessage, MessageSize, pEventHdr ))
						{
							MKLIF_REPLY_EVENT Verdict;	// default answer
							memset( &Verdict, 0, sizeof(Verdict) );

							hResult = MKL_ReplyMessage( pEventProvider->m_pClientContext, pMessage, &Verdict );

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

bool
cProcmonEventProvider::CreateThreadPool (
										cThreadPool** ppThPool,
										tDWORD MaxLen,
										tThreadPriority Priority,
										tDWORD MinThreads,
										tDWORD MaxThreads,
										tWCHAR* sThPoolName
										)
{
	tERROR error = m_pProcMon->sysCreateObject( (hOBJECT*)ppThPool, IID_THREADPOOL, PID_THPOOLIMP );

	if (PR_FAIL( error ))
	{
		*ppThPool = 0;
		return false;
	}

	(*ppThPool)->set_pgTP_TASK_QUEUE_MAX_LEN( MaxLen );

	(*ppThPool)->set_pgTP_DEFAULT_THREAD_PRIORITY( Priority );
	(*ppThPool)->set_pgTP_MIN_THREADS( MinThreads );
	(*ppThPool)->set_pgTP_MAX_THREADS( MaxThreads );

	(*ppThPool)->set_pgTP_QUICK_EXECUTE( cFALSE );
	(*ppThPool)->set_pgTP_QUICK_DONE( cFALSE );

	(*ppThPool)->set_pgTP_THREAD_CALLBACK( OnThreadInitCallback );
	(*ppThPool)->set_pgTP_THREAD_CALLBACK_CONTEXT( this );
	if (sThPoolName)
		(*ppThPool)->set_pgTP_NAME( sThPoolName, 0, cCP_UNICODE );


	error = (*ppThPool)->sysCreateObjectDone();

	if (PR_FAIL(error))
	{
		(*ppThPool)->sysCloseObject();
		*ppThPool = 0;

		return false;
	}

	return true;
}

void
cProcmonEventProvider::RegisterProcessesOnStart()
{
	PVOID pProcessInfo = NULL;
	ULONG ProcessInfoLen = 0;

	HRESULT hResult = MKL_QueryProcessesInfo( m_pClientContext, &pProcessInfo, &ProcessInfoLen );

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

	cStrObj sImagePath;
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
		
		if (pwchImagePath)
		{
			if (0 == wcsncmp(pwchImagePath, L"\\SystemRoot\\", 12))
			{
				sImagePath = pwchImagePath;
				sImagePath.replace_one(L"\\SystemRoot\\", 0, L"%SystemRoot%\\");
				m_pProcMon->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING, 
					(hOBJECT)cAutoString(sImagePath), 0, 0);
				pwchImagePath = (PWCHAR)sImagePath.data();
			}
		}

		PR_TRACE(( g_root, prtERROR, TR "\t\t'%S'", pwchBaseFolder ));
		PR_TRACE(( g_root, prtERROR, TR "\t\t'%S'", pwchCmdLine ));

		RegisterProcess (
			this,
			ProcessId,
			ParentProcessId,
			StartTime,
			pwchImagePath,
			pwchBaseFolder,
			pwchCmdLine
			);

		//		QueryModInfo( pEventProvider, ProcessId );
	}

	pfMemFree( NULL, &pProcessInfo );
}

tERROR cProcmonEventProvider::Init()
{
	ULONG ApiVersion = 0;
	ULONG AppId = 0;
	tERROR error = errOK;

#define _max_waiter_thread	8
	if (m_bInited)
		return errALREADY;
	m_bInited = true;
	error = errOK;	
	HRESULT hResult = MKL_ClientRegister(
		&m_pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WANTIMPERSONATE,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (IS_ERROR( hResult))
	{
		PR_TRACE((g_root, prtERROR, TR "client registration failed, hresult 0x%x", hResult ));
		return errUNEXPECTED;
	}

	hResult = MKL_BuildMultipleWait( m_pClientContext, _max_waiter_thread );
	if (IS_ERROR( hResult))
	{
		PR_TRACE((g_root, prtERROR, TR "MKL_BuildMultipleWait failed, hresult 0x%x", hResult ));
		return errUNEXPECTED;
	}

	PR_TRACE((g_root, prtERROR, TR "cProcmonEventProvider context 0x%x",	m_pClientContext ));

	//////////////////////////////////////////////////////////////////////////
	PEventRoot pSystemProcess = (PEventRoot) pfMemAlloc( NULL, sizeof(EventRoot), 0 );
	if (!pSystemProcess)
		return errNOT_ENOUGH_MEMORY;
	pSystemProcess->m_pid = 0;
	pSystemProcess->m_Thread = 0;
	InitializeListHead( &pSystemProcess->m_EventList );
	InsertHeadList( &m_Events, &pSystemProcess->m_List );
	
	//////////////////////////////////////////////////////////////////////////

	if (!CreateThreadPool( &m_pThPoolDrv, 1, TP_THREADPRIORITY_TIME_CRITICAL, 1, 1, L"ProcMonDrv")
		||
		!CreateThreadPool( &m_pThPoolEvents, 8, TP_THREADPRIORITY_NORMAL, 3, _max_waiter_thread, L"ProcMonEvents")
		)
	{
		if (m_pThPoolEvents)
		{
			m_pThPoolEvents->sysCloseObject();
			m_pThPoolEvents = 0;
		}

		if (m_pThPoolDrv)
		{
			m_pThPoolDrv->sysCloseObject();
			m_pThPoolDrv = 0;
		}

		PR_TRACE((g_root, prtERROR, TR "create thpools failed" ));

		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	//////////////////////////////////////////////////////////////////////////

	hResult = MKL_GetDriverApiVersion( m_pClientContext, &ApiVersion );
	if (SUCCEEDED( hResult ))
		PR_TRACE((g_root, prtERROR, TR "Mklif API version: %d", ApiVersion));

	hResult = MKL_GetAppId ( m_pClientContext, &AppId );
	if (SUCCEEDED( hResult ))
		PR_TRACE(( g_root, prtERROR, TR "Client registered with id: %d (0x%x)", AppId, AppId ));

	tTaskId task_drv;
	tPTR pData = this;

	error = m_pThPoolDrv->AddTask( &task_drv, ThreadDrvCallback, &pData, sizeof(pData), 0 );

	if (PR_SUCC(( error )))
	{
		ULONG FltId;

		ULONG ReqTimeout = 5; // 5 sec to set preverdict
#ifdef _DEBUG
		ReqTimeout = 1; // short timeout in debug mode
#endif

// Processes start/exit
#if 1
		hResult = MKL_AddFilter( &FltId, m_pClientContext, "*", ReqTimeout, 
			FLT_A_INFO, FLTTYPE_SYSTEM, MJ_EXIT_PROCESS, 0,
			0, PostProcessing, NULL, NULL );

		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter( &FltId, m_pClientContext, "*", ReqTimeout, 
			FLT_A_INFO, FLTTYPE_SYSTEM, MJ_CREATE_PROCESS_NOTIFY, 0,
			0, PostProcessing, NULL, NULL );
#endif

// Threads start/exit
#if 0
		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter( &FltId, m_pClientContext, "*", ReqTimeout, 
			FLT_A_INFO, FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_THREAD, 0,
			0, PostProcessing, NULL, NULL );

		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter( &FltId, m_pClientContext, "*", ReqTimeout, 
			FLT_A_INFO, FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_THREAD_NOTIFY, 0,
			0, PostProcessing, NULL, NULL );
#endif

// File close modified/delete
#if 1
		if (SUCCEEDED( hResult ))
		{
			char param_buf[sizeof(FILTER_PARAM) + sizeof(ULONG)];
			PFILTER_PARAM pContextFlags = (PFILTER_PARAM) param_buf;

			pContextFlags->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
			pContextFlags->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			pContextFlags->m_ParamFlt = FLT_PARAM_AND;
			pContextFlags->m_ParamSize = sizeof(ULONG);
			*(ULONG*)pContextFlags->m_ParamValue = _CONTEXT_OBJECT_FLAG_MODIFIED | _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT | _CONTEXT_OBJECT_FLAG_DELETEPENDING;

			hResult = MKL_AddFilter ( &FltId, m_pClientContext, "*", ReqTimeout, 
				FLT_A_POPUP, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0,
				0, AnyProcessing, NULL, pContextFlags, NULL
				);
		}
#endif

// File create new
#if 1
		if (SUCCEEDED( hResult ))
		{
			char param_buf[sizeof(FILTER_PARAM) + sizeof(ULONG)];
			PFILTER_PARAM pContextFlags = (PFILTER_PARAM) param_buf;

			pContextFlags->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
			pContextFlags->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			pContextFlags->m_ParamFlt = FLT_PARAM_AND;
			pContextFlags->m_ParamSize = sizeof(ULONG);
			*(ULONG*)pContextFlags->m_ParamValue = _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT;

			hResult = MKL_AddFilter ( &FltId, m_pClientContext, "*", 1, 
				FLT_A_POPUP, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0,
				0, AnyProcessing, NULL, pContextFlags, NULL
				);
		}
#endif

// File rename
#if 1
		if (SUCCEEDED( hResult ))
			hResult = MKL_AddFilter ( &FltId, m_pClientContext, "*", ReqTimeout, 
			FLT_A_POPUP, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation,
			0, AnyProcessing, NULL, NULL
			);

#endif

// Registry 
#if 1

		const DWORD arrRegFilters[] = {
			Interceptor_CreateKey,
			Interceptor_DeleteKey,
			Interceptor_DeleteValueKey,
			//Interceptor_EnumerateKey,
			//Interceptor_EnumerateValueKey,
			//Interceptor_FlushKey,
			//Interceptor_InitializeRegistry,
			//Interceptor_LoadKey2,
			//Interceptor_NotifyChangeKey,
			//Interceptor_OpenKey,
			//Interceptor_QueryKey,
			//Interceptor_QueryMultipleValueKey,
			//Interceptor_QueryValueKey,
			Interceptor_ReplaceKey,
			Interceptor_RestoreKey,
			//Interceptor_SaveKey,
			//Interceptor_SetInformationKey,
			Interceptor_SetValueKey,
			//Interceptor_UnloadKey,
			Interceptor_RenameKey,
		};

		for (size_t i=0; i<countof(arrRegFilters); i++)
		{
			if (!SUCCEEDED( hResult ))
				break;
			hResult = MKL_AddFilter( &FltId, m_pClientContext, "*", ReqTimeout, 
				FLT_A_POPUP, FLTTYPE_REGS, arrRegFilters[i], 0, 0, AnyProcessing, NULL, NULL );
		}
#endif

		if (!SUCCEEDED( hResult ))
		{
			PR_TRACE(( g_root, prtERROR, TR "register filters failed: 0x%x", hResult ));
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		}
		
		if (SUCCEEDED( hResult ))
			hResult = MKL_ChangeClientActiveStatus( m_pClientContext, TRUE );
		if (!SUCCEEDED( hResult ))
		{
			PR_TRACE(( g_root, prtERROR, TR "change status 2 active failed: 0x%x", hResult ));
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		}
	}
	return error;
}

tDWORD cProcmonEventProvider::GetFileRevision(hIO io)
{
	if (!m_bInited)
	{
		PR_TRACE((g_root, prtERROR, TR "GetFileRevision() called while EvP not inited"));
		return cUNKNOWN_FILE_REVISION;
	}
	if (!io)
	{
		PR_TRACE((g_root, prtERROR, TR "GetFileRevision() io=NULL"));
		return cUNKNOWN_FILE_REVISION;
	}
	if (io->propGetPID() != PID_NATIVE_FIO)
	{
		PR_TRACE((g_root, prtERROR, TR "GetFileRevision() called with io pid=%X", io->propGetPID()));
		return cUNKNOWN_FILE_REVISION;
	}
	HANDLE hFile = io->propGetPtr(plNATIVE_HANDLE);
	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
	{
		PR_TRACE((g_root, prtERROR, TR "GetFileRevision() invalid handle=%X", hFile));
		return cUNKNOWN_FILE_REVISION;
	}
	ULONG nRevision = cUNKNOWN_FILE_REVISION;
	HRESULT res = MKL_GetFidbox2(m_pClientContext, hFile, &nRevision);
	if (FAILED(res))
	{
		PR_TRACE((g_root, prtERROR, TR "GetFileRevision() failed with %X", res));
		return cUNKNOWN_FILE_REVISION;
	}
	PR_TRACE((g_root, prtIMPORTANT, TR "GetFileRevision() succeeded with rev=%X", nRevision));
	return nRevision;
}


tERROR cProcmonEventProvider::Done()
{
	PR_TRACE((g_root, prtIMPORTANT, TR "Stopping..."));
	
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	m_bInited = false;

	m_bStop = true;
	HRESULT hResult = MKL_ChangeClientActiveStatus( m_pClientContext, FALSE );
	WaitLastEvent();

	PR_TRACE((g_root, prtIMPORTANT, TR "closing prague objects..."));

	if (m_pThPoolDrv)
	{
		m_pThPoolDrv->sysCloseObject();
		m_pThPoolDrv = NULL;
	}

	if (m_pThPoolEvents)
	{
		m_pThPoolEvents->sysCloseObject();
		m_pThPoolEvents = NULL;
	}

	if (m_pClientContext)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "unregister client"));
		MKL_ClientUnregister( &m_pClientContext );
	}

	m_EventSync.LockSync();
	while (true)
	{
		if (IsListEmpty( &m_Events ))
			break;
		PLIST_ENTRY Flink = m_Events.Flink;
		PEventRoot pRoot = CONTAINING_RECORD( Flink, EventRoot, m_List );
		RemoveEntryList( &pRoot->m_List );
		PR_TRACE(( g_root, prtIMPORTANT, TR "free events root for pid %d", pRoot->m_pid ));
		pfMemFree( NULL, (void**) &pRoot );
	}

	m_EventSync.UnLockSync();
	return errOK;
}


void
cProcmonEventProvider::WaitLastEvent()
{
	PSingleEvent pse = NULL;

	MKLIF_REPLY_EVENT DrvVerdict;	// default answer
	memset( &DrvVerdict, 0, sizeof(DrvVerdict) );

	while(true)
	{
		pse = NULL;

		m_EventSync.LockSync();

		if (!IsListEmpty( &m_Events ))
		{
			PLIST_ENTRY Flink = m_Events.Flink;

			PEventRoot pRoot = NULL;
			while (Flink != &m_Events)
			{
				pRoot = CONTAINING_RECORD( Flink, EventRoot, m_List );
				Flink = Flink->Flink;

				if (!IsListEmpty( &pRoot->m_EventList ))
				{
					pse = CONTAINING_RECORD( pRoot->m_EventList.Flink, SingleEvent, m_List );

					RemoveEntryList( &pse->m_List );

					break;
				}
			}
		}

		m_EventSync.UnLockSync();

		if (!pse)
			break; // no events found
		else
		{
			HRESULT hResult = MKL_ReplyMessage( m_pClientContext, pse->m_pMessage, &DrvVerdict );
			if (!SUCCEEDED(hResult) )
			{
				PR_TRACE((g_root, prtERROR, TR "WaitLastEvent: skip event error (hResult): 0x%x", hResult));
			}

			pfMemFree( NULL, &pse->m_pMessage );
			pfMemFree(NULL, (void**) &pse );
		}
	}
}

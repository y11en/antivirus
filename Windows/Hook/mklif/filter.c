#include "filter.h"
#include "eresource.h"
#include "client.h"
#include "InvThread.h"
#include "PPage.c"
#include "runproc.h"
#include "klsecur.h"

#include "filter.tmh"

#define _INVALID_CLIENT_ID		0
#define _INVALID_FILTER_ID		0

typedef enum _teQUEUE_VAL {
	_QUEUE_TOP,
		_QUEUE_BOTTOM,
		_QUEUE_POSITION,
}eQUEUE_VAL;

//
ULONG	g_FilterID = _INVALID_FILTER_ID;

typedef struct _sFUNC_DATA
{
	SINGLE_LIST_ENTRY	m_Entry;
	SINGLE_LIST_ENTRY	m_Tree;
	ULONG				m_FuncId;
	LIST_ENTRY			m_List;
}FUNC_DATA, *PFUNC_DATA;


#define _Hook2Idx( _hook ) (_hook - 1)
typedef struct _sHOOKS_DATA
{
	ERESOURCE			m_Sync;
	SINGLE_LIST_ENTRY	m_FiltersTree;
	LONG				m_FiltersCount;
}HOOKS_DATA, *PHOOKS_DATA;

HOOKS_DATA gHookFilters[HOOK_MAX_ID];

// -------------
#ifdef WIN2K
	#define PFuncData_Init
	#define PFuncData_Done
	#define PFuncData_Allocate		ExAllocatePoolWithTag( NonPagedPool, sizeof(FUNC_DATA), tag_filters )
	#define PFuncData_Free( _p )	ExFreePool( _p )

#else
	NPAGED_LOOKASIDE_LIST gNPL_PFunc;
	#define PFuncData_Init			ExInitializeNPagedLookasideList( &gNPL_PFunc, NULL, NULL, 0, sizeof(FUNC_DATA), tag_filters, 0)
	#define PFuncData_Done			ExDeleteNPagedLookasideList( &gNPL_PFunc )
	#define PFuncData_Allocate		ExAllocateFromNPagedLookasideList( &gNPL_PFunc )
	#define PFuncData_Free( _p )	ExFreeToNPagedLookasideList( &gNPL_PFunc, _p )
#endif // WIN2K
//

typedef struct _DI_FILTER
{
	IMPLIST_ENTRY	m_List;

	ULONG			m_HookID;
	ULONG			m_FunctionMj;
	ULONG			m_FunctionMi;
	ULONG			m_ClientId;
	BOOLEAN			m_bClientPaused;
	ULONG			m_FilterID;
	ULONG			m_Flags;
	ULONG			m_ParamsCount;
	BYTE			m_Params[0];		// FILTER_PARAM 
}DI_FILTER, *PDI_FILTER;

KSPIN_LOCK		gDi_SpinLock;
IMPLIST_ENTRY	gDi_FltList;

tefVerdict Di_FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject);
BOOLEAN Di_FltCheckParams(PFILTER_TRANSMIT pFltIn);
PDI_FILTER Di_AllocateFilter(PFILTER_TRANSMIT pFltIn);
void Di_AddFilter(PDI_FILTER pDiFilter, eQUEUE_VAL eSite, ULONG FilterID);
void Di_DelFilter(FilterID);
void Di_FreeFlt(PDI_FILTER pDiFilter);
void Di_FilterSwitch(ULONG ClientId, ULONG FilterID, BOOLEAN bEnable);
void Di_ResetFiltersForApp(ULONG ClientId);
void Di_PrepareFilterParams(PDI_FILTER pDiFilter);

//////////////////////////////////////////////////////////////////////////

QSUBCLIENT gNonPopupClient;

//////////////////////////////////////////////////////////////////////////
NTSTATUS
SendDevIoCtl2DrvByName(
					   ULONG IoControlCode,
					   PVOID InputBuffer,
					   ULONG InputBufferLength,
					   PVOID OutputBuffer,
					   ULONG OutputBufferLength,
					   PCWSTR pDrvName,
					   ULONG_PTR* pRetSize)
{
	HANDLE						hExtDrv = NULL;
	OBJECT_ATTRIBUTES	ObjAttr;
	UNICODE_STRING		us;
	IO_STATUS_BLOCK		ioStatus;
	PDEVICE_OBJECT		DevObj;
	PFILE_OBJECT			fileObject;
	NTSTATUS					ntStatus;
	KEVENT						Event;
	PIRP							Irp;
	PIO_STACK_LOCATION irpSp;
	RtlInitUnicodeString(&us, pDrvName);
	InitializeObjectAttributes(&ObjAttr,&us,OBJ_CASE_INSENSITIVE,NULL,NULL);
	ntStatus=ZwCreateFile(&hExtDrv,SYNCHRONIZE|FILE_ANY_ACCESS,&ObjAttr,&ioStatus,NULL,0,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_OPEN,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
	if(NT_SUCCESS(ntStatus) && hExtDrv)
	{
		ntStatus = ObReferenceObjectByHandle(hExtDrv,FILE_READ_DATA,NULL,KernelMode, (PVOID*) &fileObject,NULL);
		if(NT_SUCCESS(ntStatus))
		{
			if((DevObj=IoGetRelatedDeviceObject(fileObject))!=NULL)
			{
				KeInitializeEvent(&Event,NotificationEvent,FALSE);
				Irp = IoBuildDeviceIoControlRequest(IoControlCode,DevObj,InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,FALSE,&Event,&ioStatus);
				if(Irp!=NULL)
				{
					irpSp=IoGetNextIrpStackLocation(Irp);
					irpSp->FileObject = fileObject;
					ntStatus=IoCallDriver(DevObj,Irp);
					if(ntStatus == STATUS_PENDING)
					{
						KeWaitForSingleObject(&Event,Executive,KernelMode,FALSE,(PLARGE_INTEGER)NULL);
						ntStatus = ioStatus.Status;
					}
					if (pRetSize != NULL)
						*pRetSize = ioStatus.Information;
				}
				else
					ntStatus = STATUS_UNSUCCESSFUL;
			}
			else
				ntStatus = STATUS_UNSUCCESSFUL;
			ObDereferenceObject(fileObject);
		}
		ZwClose(hExtDrv);
	}

	return ntStatus;
}


BOOLEAN
GeneralStart_ExternalDrv (
	ULONG Ioctl_GetVersion,
	ULONG Ioctl_StartFiltering,
	PCWSTR pwcDrvNAME,
	ULONG ApiVersion,
	ULONG HookID
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	EXT_START_FILTERING	StartFiltering;
	ULONG ExtDrvApiVersion = 0;

	DoTraceEx( TRACE_LEVEL_WARNING, DC_FILTER, "extdrv request to %S", pwcDrvNAME);
	
	status = SendDevIoCtl2DrvByName (
		Ioctl_GetVersion,
		NULL,
		0,
		&ExtDrvApiVersion,
		sizeof(ExtDrvApiVersion),
		pwcDrvNAME,
		NULL
		);

	DoTraceEx( TRACE_LEVEL_WARNING, DC_FILTER, "extdrv request getversion %!STATUS!", status);

	if(NT_SUCCESS( status ))
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_FILTER, "extdrv request version returned 0x%x (expected 0x%x)",
			ExtDrvApiVersion, ApiVersion);
		
		if(ExtDrvApiVersion == ApiVersion)
		{
			StartFiltering.m_HookID = HookID;

			InitStartFiltering(&StartFiltering);

			status = SendDevIoCtl2DrvByName (
				Ioctl_StartFiltering,
				&StartFiltering,
				sizeof(StartFiltering),
				NULL,
				0,
				pwcDrvNAME,
				NULL
				);

			DoTraceEx( TRACE_LEVEL_WARNING, DC_FILTER, "Start filtering result %!STATUS!",
				status
				);

			if(NT_SUCCESS( status ))
				return TRUE;
		}
	}
	
	return FALSE;
}

BOOLEAN GeneralIoctl_ExternalDrv(ULONG Ioctl, PCWSTR pwcDrvNAME, ULONG HookID)
{
	NTSTATUS status = SendDevIoCtl2DrvByName (
		Ioctl,
		&HookID,
		sizeof(HookID),
		NULL,
		0,
		pwcDrvNAME,
		NULL
		);

	DoTraceEx( TRACE_LEVEL_WARNING, DC_FILTER, "Stop filtering %S result %!STATUS!",
		pwcDrvNAME,
		status
		);

	if(NT_SUCCESS( status ))
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

#include "../hook/klin_api.h"
#include "../hook/klick_api.h"

BOOLEAN KlickStart(VOID)
{
	return GeneralStart_ExternalDrv(KLICK_GET_VERSION, KLICK_START_FILTER, L"\\Device\\KLICK", KLICK_API_VERSION, FLTTYPE_KLICK);
}

BOOLEAN KlickStop(VOID)
{
	return GeneralIoctl_ExternalDrv(KLICK_STOP_FILTER, L"\\Device\\KLICK", FLTTYPE_KLICK);	
}

VOID
KlickRefreshData()
{
	GeneralIoctl_ExternalDrv(KLICK_RESET_TRUSTED_STREAMS, L"\\Device\\KLICK", FLTTYPE_KLICK);	
}

BOOLEAN KlinStart(VOID)
{
	return GeneralStart_ExternalDrv(KLICK_GET_VERSION, KLIN_START_FILTER, L"\\Device\\KLIN", KLIN_API_VERSION, FLTTYPE_KLIN);
}

BOOLEAN KlinStop(VOID)
{
	return GeneralIoctl_ExternalDrv(KLIN_STOP_FILTER, L"\\Device\\KLIN", FLTTYPE_KLIN);	
}

//////////////////////////////////////////////////////////////////////////
VOID
DrvRestartHook (
	)
{
	if (gHookFilters[_Hook2Idx( FLTTYPE_KLICK )].m_FiltersCount)
		KlickStart();

	if (gHookFilters[_Hook2Idx( FLTTYPE_KLIN )].m_FiltersCount)
		KlinStart();
}

BOOLEAN
DrvRequestHook(ULONG HookID)
{
	switch(HookID)
	{
	case FLTTYPE_KLICK:
		return KlickStart();

	case FLTTYPE_KLIN:
		return KlinStart();
	}
	return TRUE;
}

BOOLEAN
DrvRequestUnhook(ULONG HookID)
{
	switch(HookID)
	{
	case FLTTYPE_KLICK:
		return KlickStop();

	case FLTTYPE_KLIN:
		return KlinStop();
	}
	return TRUE;
}

BOOLEAN
RequestHook(DWORD HookID)
{
	if (FLTTYPE_EMPTY == HookID || HookID > HOOK_MAX_ID)
		return FALSE;
	
	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "RequestHook 0x%x. current counter %d",
		HookID, gHookFilters[_Hook2Idx( HookID )].m_FiltersCount );

	if (!gHookFilters[_Hook2Idx( HookID )].m_FiltersCount)
		DrvRequestHook( HookID );

	if (FLTTYPE_KLICK == HookID)
		KlickRefreshData();
	
	InterlockedIncrement( &gHookFilters[_Hook2Idx( HookID )].m_FiltersCount );

	return TRUE;
}

void
RequestUnhook (
	DWORD HookID
	)
{
	LONG OldValue = InterlockedDecrement( &gHookFilters[_Hook2Idx( HookID )].m_FiltersCount );

	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "RequestUnhook 0x%x. current counter %d",
		HookID, OldValue + 1 );

	if (FLTTYPE_KLICK == HookID)
		KlickRefreshData();

	if (!OldValue)
		DrvRequestUnhook( HookID );
}

__inline BOOLEAN
IsParameterUnicode (
	DWORD ParamID
	)
{
	if ((ParamID == _PARAM_OBJECT_URL_W)
		|| (ParamID == _PARAM_OBJECT_URL_DEST_W)
		|| (ParamID == _PARAM_OBJECT_VOLUME_NAME_W)
		|| (ParamID == _PARAM_OBJECT_VOLUME_NAME_DEST_W)
		)
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOLEAN
GlobalFiltersInit(void)
{
	DWORD cou;

	KeInitializeSpinLock(&gDi_SpinLock);
	_impInitializeListHead(&gDi_FltList);

	PFuncData_Init;

	RtlZeroMemory( gHookFilters, sizeof( gHookFilters ));
	for (cou = 0; cou < HOOK_MAX_ID; cou++)
		ExInitializeResourceLite( &gHookFilters[cou].m_Sync );

	gNonPopupClient.m_ActivityState = _INTERNAL_APP_STATE_ACTIVE | _INTERNAL_APP_STATE_DEAD;
	gNonPopupClient.m_ActivityWatchDog = DEADLOCKWDOG_TIMEOUT;
	gNonPopupClient.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
	gNonPopupClient.m_pNext = 0;
	gNonPopupClient.m_ProcessId = 0;
	
	gNonPopupClient.m_SubClientCommType = _SUB_CLIENT_COMM_TYPE_NONE;
	gNonPopupClient.m_pPortContext = NULL;

	gNonPopupClient.m_Tasks = 0;
	gNonPopupClient.m_ThreadId = 0;
	
	return TRUE;
}

void
GlobalFiltersDone (
	)
{
	DWORD cou;

	// free memory at
	for (cou = 0; cou < HOOK_MAX_ID; cou++)
	{
		PSINGLE_LIST_ENTRY pListMj = NULL;

		pListMj = gHookFilters[cou].m_FiltersTree.Next;
		while (pListMj)
		{
			PFUNC_DATA pDataMj = CONTAINING_RECORD( pListMj, FUNC_DATA, m_Entry );
			PSINGLE_LIST_ENTRY pListMi = pDataMj->m_Tree.Next;

			while (pListMi)
			{
				PFUNC_DATA pDataMi = CONTAINING_RECORD( pListMi, FUNC_DATA, m_Entry );

				if (!IsListEmpty( &pDataMi->m_List))
				{
					_dbg_break_;
				}
				
				pListMi = pListMi->Next;

				PFuncData_Free( pDataMi );
			}

			pListMj = pListMj->Next;
			
			PFuncData_Free( pDataMj );
		}

		ExDeleteResourceLite( &gHookFilters[cou].m_Sync );
	}

	PFuncData_Done;
}

__inline BOOLEAN
MatchOkayW(PWCHAR Pattern)
{
	if(*Pattern && *Pattern!=L'*')
		return FALSE;
	return TRUE;
}

__inline BOOLEAN MatchOkayA(PCHAR Pattern)
{
	if(*Pattern && *Pattern!='*')
		return FALSE;
	return TRUE;
}

__inline BOOLEAN
MatchWithPatternW(PWCHAR Pattern, PWCHAR Name, BOOLEAN CaseSensivity)
{
	WCHAR ch;

	if(*Pattern==L'*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			if (CaseSensivity == FALSE)
				ch = RtlUpcaseUnicodeChar(*Name);
			else
				ch = *Name;
			if ((ch == L'*') || (*Pattern == ch))
			{
				if(MatchWithPatternW(Pattern+1,Name+1, CaseSensivity)) 
					return TRUE;
			}
			
			Name++;
		}
		return MatchOkayW(Pattern);
	} 
	
	while(*Name && *Pattern != L'*')
	{
		if (CaseSensivity == TRUE)
			ch = *Name;
		else
			ch = RtlUpcaseUnicodeChar(*Name);
		
		if(*Pattern == ch)
		{
			Pattern++;
			Name++;
		} else
			return FALSE;
	}
	if(*Name)
		return MatchWithPatternW(Pattern,Name, CaseSensivity);
	
	return MatchOkayW(Pattern);
}

__inline BOOLEAN
MatchWithPatternA(PCHAR Pattern, PCHAR Name, BOOLEAN CaseSensivity)
{
	CHAR ch;
	
	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			if (CaseSensivity == FALSE)
				ch = RtlUpperChar(*Name);
			else
				ch = *Name;
			if ((ch == '*') || (*Pattern == ch))
			{
				if(MatchWithPatternA(Pattern+1,Name+1, CaseSensivity)) 
					return TRUE;
			}
			
			Name++;
		}
		return MatchOkayA(Pattern);
	} 
	
	while(*Name && *Pattern != '*')
	{
		if (CaseSensivity == TRUE)
			ch = *Name;
		else
			ch = RtlUpperChar(*Name);
		
		if(*Pattern == ch)
		{
			Pattern++;
			Name++;
		} else
			return FALSE;
	}
	if(*Name)
		return MatchWithPatternA(Pattern,Name, CaseSensivity);
	
	return MatchOkayA(Pattern);
}

__inline BOOLEAN
MatchWithPattern(PCHAR Pattern, PCHAR Name, BOOLEAN CaseSensivity, DWORD ParamID)
{
	if (IsParameterUnicode(ParamID) == TRUE)
	{
		PWCHAR pwchPattern = (PWCHAR) Pattern;
//		DbPrint(DC_PARAMS, DL_SPAM, ("check ustring (sens %d) pattern '%S' with '%S'\n", CaseSensivity, 
//			pwchPattern, (PWCHAR) Name));
		if (!*pwchPattern)
		{
			_dbg_break_;
			return TRUE;
		}
		return MatchWithPatternW(pwchPattern, (PWCHAR) Name, CaseSensivity);
	}
	
	return MatchWithPatternA(Pattern, Name, CaseSensivity);
}

// Usage -----------------------------------------------------------------------------------

DWORD CalcFilterSize(PFILTER_TRANSMIT pFltIn)
{
	DWORD FilterSize;
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
#ifdef __DBG__
	PCHAR pch;
#endif
	FilterSize = sizeof(FILTER_TRANSMIT);
	pParam = (PFILTER_PARAM) pFltIn->m_Params;
	for (cou = 0; cou < pFltIn->m_ParamsCount; cou++)
	{
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		FilterSize += tmp;
		pPointer = (BYTE*)pParam + tmp;
#ifdef __DBG__
		pch = pParam->m_ParamValue;
#endif
		pParam = (PFILTER_PARAM) pPointer;
	}
	return FilterSize;
}

// --

void
Filters_Release (
	PQFLT pQFlt
	)
{
	ULONG HookID = pQFlt->m_HookID;
	ReleaseResource( &gHookFilters[_Hook2Idx( HookID )].m_Sync );
}

PFUNC_DATA
Filters_GetListMi (
	ULONG HookId,
	ULONG Mj,
	ULONG Mi,
	BOOLEAN bForceAlloc
	)
{
	PSINGLE_LIST_ENTRY pList = NULL;
	PFUNC_DATA pDataMj = NULL;
	PFUNC_DATA pDataMi = NULL;

	if (FLTTYPE_EMPTY == HookId || HookId > HOOK_MAX_ID)
		return NULL;

	if (bForceAlloc)
		AcquireResourceExclusive( &gHookFilters[_Hook2Idx( HookId )].m_Sync );
	else
	{
		if (!gHookFilters[_Hook2Idx( HookId )].m_FiltersCount)
			return NULL;
		AcquireResourceShared( &gHookFilters[_Hook2Idx( HookId )].m_Sync );
	}

	pList = gHookFilters[_Hook2Idx( HookId )].m_FiltersTree.Next;
	if (pList)
	{
		while (pList)
		{
			pDataMj = CONTAINING_RECORD( pList, FUNC_DATA, m_Entry );
			if (pDataMj->m_FuncId == Mj)
				break;

			pList = pList->Next;
		}
	}
	
	if (!pList)
	{
		if (!bForceAlloc)
		{
			ReleaseResource( &gHookFilters[_Hook2Idx( HookId )].m_Sync );
			return NULL;
		}

		pDataMj = (PFUNC_DATA) PFuncData_Allocate;
		if (!pDataMj)
		{
			ReleaseResource( &gHookFilters[_Hook2Idx( HookId )].m_Sync );
			return NULL;
		}
		RtlZeroMemory( pDataMj, sizeof(FUNC_DATA) );
		InitializeListHead( &pDataMj->m_List );
		pDataMj->m_FuncId = Mj;
		PushEntryList( &gHookFilters[_Hook2Idx( HookId )].m_FiltersTree, &pDataMj->m_Entry );
	}

	pList = pDataMj->m_Tree.Next;
	while (pList)
	{
		pDataMi = CONTAINING_RECORD( pList, FUNC_DATA, m_Entry );
		if (pDataMi->m_FuncId == Mi)
			return pDataMi;

		pList = pList->Next;
	}
	
	if (!bForceAlloc)
	{
		ReleaseResource( &gHookFilters[_Hook2Idx( HookId )].m_Sync );
		return NULL;
	}

	pDataMi = (PFUNC_DATA) PFuncData_Allocate;
	if (!pDataMi)
	{
		ReleaseResource( &gHookFilters[_Hook2Idx( HookId )].m_Sync );
		return NULL;
	}
	
	RtlZeroMemory( pDataMi, sizeof(FUNC_DATA) );
	InitializeListHead( &pDataMi->m_List );
	pDataMi->m_FuncId = Mi;
	PushEntryList( &pDataMj->m_Tree, &pDataMi->m_Entry );

	return pDataMi;
}

void
PrepareFilterParams (
	PQFLT pQFlt
	)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
	int paramcou;
	CHAR ch;
	
	BOOLEAN bHaveMappedParams = FALSE;
	
	_UpperStringZeroTerninatedA(pQFlt->m_pProcName, pQFlt->m_pProcName);
	
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		// prepare param
		pParam->m_ParamFlags &= ~_FILTER_PARAM_INTERNAL_OPTIMIZER1;
		
		if (pParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT)
			bHaveMappedParams = TRUE;
		switch (pParam->m_ParamFlt)
		{
		case FLT_PARAM_WILDCARD:
		case FLT_PARAM_MASKUNSENS_LIST:
		case FLT_PARAM_MASKUNSENS_LISTW:
			{
				// to upper str
				int tmplen;
				int dwLastLen = pParam->m_ParamSize;
				pPointer = pParam->m_ParamValue;
				
				while (dwLastLen > 0)
				{
					if ((FLT_PARAM_MASKUNSENS_LISTW == pParam->m_ParamFlt) || IsParameterUnicode(pParam->m_ParamID))
					{
						_UpperStringZeroTerninatedW((PWCHAR) pPointer, (PWCHAR) pPointer);
						tmplen = (wcslen((PWCHAR) pPointer) + 1) * sizeof(WCHAR);
						if (tmplen == 2)
							dwLastLen = 0;
						else
						{
							pPointer += tmplen;
							dwLastLen -= tmplen;
						}
					}
					else
					{
						PCHAR pch = pPointer;
						tmplen = strlen(pPointer);
						for (paramcou = 0; paramcou < tmplen; paramcou++)
						{
							ch = RtlUpperChar(*pch);
							*pch = ch;
							pch++;
						}
						
						tmplen = tmplen + 1;
						if (tmplen == 1)
							dwLastLen = 0;	
						else
						{
							pPointer += tmplen;
							dwLastLen -= tmplen;
						}
					}
				}
			}
			break;
		default:
			break;
		}
		// end prepare
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}
	
	if (bHaveMappedParams == TRUE)
		pQFlt->m_Flags |= FLT_A_HAVE_MAPPED_PARAMS;
	else
		pQFlt->m_Flags &= ~FLT_A_HAVE_MAPPED_PARAMS;
}

NTSTATUS
Filters_Add (
	PFILTER_TRANSMIT pFltIn,
	eQUEUE_VAL eSite
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PFUNC_DATA pFilters = NULL;
	PQCLIENT pClient = AcquireClient( pFltIn->m_AppID );

	UNREFERENCED_PARAMETER( eSite );
	if (!pClient)
		return STATUS_UNSUCCESSFUL;

	DoTraceEx( TRACE_LEVEL_INFORMATION, DC_FILTER, "Adding new filter for app 0x%x: HoMjMi 0x%x:%x:%x flags 0x%x%S",
		pFltIn->m_AppID,
		pFltIn->m_HookID,
		pFltIn->m_FunctionMj,
		pFltIn->m_FunctionMi,
		pFltIn->m_Flags,
		pFltIn->m_Flags & FLT_A_DISPATCH_MIRROR ? L"(dispatch)" : L""
		);

	// forbidden flags combination
	if ((pFltIn->m_Flags & FLT_A_POPUP) && (pFltIn->m_Flags & FLT_A_DELETE_ON_MARK))
	{
		_dbg_break_;
		return STATUS_UNSUCCESSFUL;
	}

	if (!RequestHook( pFltIn->m_HookID ))
		return STATUS_UNSUCCESSFUL;

	pFilters = Filters_GetListMi( pFltIn->m_HookID, pFltIn->m_FunctionMj, pFltIn->m_FunctionMi, TRUE );
	if (pFilters)
	{
		ULONG ParamSize = CalcFilterTransmitSize(pFltIn) - sizeof(FILTER_TRANSMIT);
		PQFLT pQFltMemory = ExAllocatePoolWithTag( NonPagedPool, sizeof(QFLT) + ParamSize, tag_filter );
		PDI_FILTER DiFilter = NULL;

		if (pQFltMemory && pFltIn->m_Flags & FLT_A_DISPATCH_MIRROR)
		{
			DiFilter = Di_AllocateFilter( pFltIn );
			if (DiFilter)
			{
				if (pClient->m_ClientFlags & _CLIENT_FLAG_PAUSED)
					DiFilter->m_bClientPaused = TRUE;
			}
			else
			{
				ExFreePool( pQFltMemory );
				pQFltMemory = NULL;
			}
		}
		
		if (pQFltMemory)
		{
			status = STATUS_SUCCESS;

			pClient->m_FiltersCount++;

			g_FilterID++;
			if (_INVALID_FILTER_ID == g_FilterID)
				g_FilterID++;
			pQFltMemory->m_FilterID = g_FilterID;

			pQFltMemory->m_ClientId = pFltIn->m_AppID;
			pQFltMemory->m_Expiration = pFltIn->m_Expiration;
			pQFltMemory->m_HookID = pFltIn->m_HookID;
			pQFltMemory->m_Mj = pFltIn->m_FunctionMj;
			pQFltMemory->m_Mi = pFltIn->m_FunctionMi;
			pQFltMemory->m_ProcessingType =  pFltIn->m_ProcessingType;
			pQFltMemory->m_Flags = pFltIn->m_Flags;
			pQFltMemory->m_ParamsCount = pFltIn->m_ParamsCount;
			memcpy(pQFltMemory->m_Params, pFltIn->m_Params, ParamSize);
			RtlStringCbCopyA(pQFltMemory->m_pProcName, sizeof(pQFltMemory->m_pProcName), pFltIn->m_ProcName);
			pQFltMemory->m_Timeout = pFltIn->m_Timeout;
			
			PrepareFilterParams( pQFltMemory );

			if (IsListEmpty( &pFilters->m_List ))
				InsertTailList( &pFilters->m_List, &pQFltMemory->m_List );
			else
			{
				PLIST_ENTRY Flink = pFilters->m_List.Flink;
				while(Flink != &pFilters->m_List)
				{
					PQFLT pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );

					if (pFltIn->m_AppID == pQFlt->m_ClientId)
					{
						switch (eSite)
						{
						case _QUEUE_TOP:
							InsertHeadList( pQFlt->m_List.Blink, &pQFltMemory->m_List );
							break;

						case _QUEUE_POSITION:
							status = STATUS_NOT_FOUND;
							while(Flink != &pFilters->m_List)
							{
								pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );
								if (pFltIn->m_FilterID == pQFlt->m_FilterID)
								{
									status = STATUS_SUCCESS;
									InsertHeadList( &pQFlt->m_List, &pQFltMemory->m_List );
									break;
								}

								Flink = Flink->Flink;
							}
							break;
						
						case _QUEUE_BOTTOM:
						default:
							while(Flink != &pFilters->m_List)
							{
								pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );
								if (pFltIn->m_AppID != pQFlt->m_ClientId)
								{
									InsertHeadList( pQFlt->m_List.Blink, &pQFltMemory->m_List );
									break;
								}

								Flink = Flink->Flink;
								if (Flink == &pFilters->m_List)
								{	
									InsertHeadList( Flink->Blink, &pQFltMemory->m_List );
									break;
								}
							}
							break;
						}
						
						break;
					}
					else
					{
						if (pClient->m_Priority > GetClientPriority( pQFlt->m_ClientId ))
						{
							InsertHeadList( pQFlt->m_List.Blink, &pQFltMemory->m_List );
							break;
						}
					}

					Flink = Flink->Flink;

					if (Flink == &pFilters->m_List)
					{
						//end of list
						InsertHeadList( &pQFlt->m_List, &pQFltMemory->m_List );
					}
				}
			}
		}

		if (NT_SUCCESS( status ))
			pFltIn->m_FilterID = pQFltMemory->m_FilterID;

		ReleaseResource( &gHookFilters[_Hook2Idx( pFltIn->m_HookID )].m_Sync );

		if (NT_SUCCESS( status ))
		{
			if (DiFilter)
				Di_AddFilter( DiFilter, eSite, pFltIn->m_FilterID ); // здесь рассихронизация!
		}
		else
		{
			if (pQFltMemory)
				ExFreePool( pQFltMemory );

			if (DiFilter)
			{
				ExFreePool( DiFilter );
				DiFilter = NULL;
			}

			RequestUnhook( pFltIn->m_HookID );
		}
	}

	ReleaseClient( pClient );

	return status;
}

PQFLT
Filters_Find (
	ULONG ClientId,
	ULONG FilterId
	)
{
	DWORD cou;

	for (cou = 0; cou < HOOK_MAX_ID; cou++)
	{
		PSINGLE_LIST_ENTRY pListMj = NULL;
		AcquireResourceShared( &gHookFilters[cou].m_Sync );

		pListMj = gHookFilters[cou].m_FiltersTree.Next;
		while (pListMj)
		{
			PFUNC_DATA pDataMj = CONTAINING_RECORD( pListMj, FUNC_DATA, m_Entry );
			PSINGLE_LIST_ENTRY pListMi = pDataMj->m_Tree.Next;

			while (pListMi)
			{
				PFUNC_DATA pDataMi = CONTAINING_RECORD( pListMi, FUNC_DATA, m_Entry );

				if (!IsListEmpty( &pDataMi->m_List))
				{
					PLIST_ENTRY Flink = pDataMi->m_List.Flink;
					while (Flink != &pDataMi->m_List)
					{
						PQFLT pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );
						if (pQFlt->m_ClientId == ClientId)
						{
							if (_INVALID_FILTER_ID == FilterId || pQFlt->m_FilterID == FilterId)
								return pQFlt;
						}
						Flink = Flink->Flink;
					}
				}
				
				pListMi = pListMi->Next;
			}

			pListMj = pListMj->Next;
		}

		ReleaseResource( &gHookFilters[cou].m_Sync );
	}
	
	return NULL;
}

PQFLT
Filters_FindNext (
	ULONG ClientId,
	ULONG StartFilterId
	)
{
	DWORD cou;
	
	for (cou = 0; cou < HOOK_MAX_ID; cou++)
	{
		PSINGLE_LIST_ENTRY pListMj = NULL;
		AcquireResourceShared( &gHookFilters[cou].m_Sync );

		pListMj = gHookFilters[cou].m_FiltersTree.Next;
		while (pListMj)
		{
			PFUNC_DATA pDataMj = CONTAINING_RECORD( pListMj, FUNC_DATA, m_Entry );
			PSINGLE_LIST_ENTRY pListMi = pDataMj->m_Tree.Next;

			while (pListMi)
			{
				PFUNC_DATA pDataMi = CONTAINING_RECORD( pListMi, FUNC_DATA, m_Entry );

				if (!IsListEmpty( &pDataMi->m_List))
				{
					PLIST_ENTRY Flink = pDataMi->m_List.Flink;
					while (Flink != &pDataMi->m_List)
					{
						PQFLT pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );
						if (pQFlt->m_ClientId == ClientId)
						{
							if (_INVALID_FILTER_ID == StartFilterId)
								return pQFlt;

							if (pQFlt->m_FilterID == StartFilterId)
								StartFilterId = _INVALID_FILTER_ID;
						}
						Flink = Flink->Flink;
					}
				}
				
				pListMi = pListMi->Next;
			}

			pListMj = pListMj->Next;
		}

		ReleaseResource( &gHookFilters[cou].m_Sync );
	}
	
	return NULL;	
}

void
Filters_Free (
	PQFLT pQFlt
	)
{
	RemoveEntryList( &pQFlt->m_List );
	ExFreePool( pQFlt );
}

NTSTATUS
Filters_Delete (
	ULONG ClientId,
	ULONG FilterId
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	PQFLT pQFlt = NULL;
	PQCLIENT pClient = AcquireClient( ClientId );
	
	pQFlt = Filters_Find( ClientId, FilterId );
	if (pQFlt)
	{
		ULONG HookID = pQFlt->m_HookID;
		Filters_Free( pQFlt );
		if (pClient)
			pClient->m_FiltersCount--;
		ReleaseResource( &gHookFilters[_Hook2Idx( HookID )].m_Sync );

		RequestUnhook( HookID );

		status = STATUS_SUCCESS;
	}

	Di_DelFilter( FilterId );

	if (pClient)
		ReleaseClient( pClient );

	return status;
}

void
Filters_Clear (
	ULONG ClientId
	)
{
	PQFLT pQFlt = NULL;
	ULONG FilterId;

	do
	{
		pQFlt = Filters_Find( ClientId, _INVALID_FILTER_ID );
		if (!pQFlt)
			break;
		
		FilterId = pQFlt->m_FilterID;
		ReleaseResource( &gHookFilters[_Hook2Idx( pQFlt->m_HookID )].m_Sync );

		Filters_Delete( ClientId, FilterId );
	} while (pQFlt);
}

void
Filters_FreeAppFilters (
	ULONG ClientId
	)
{
	Di_ResetFiltersForApp( ClientId );
	Filters_Clear( ClientId );
}

ULONG
Filters_CalcParamSize (
	PQFLT pQFlt,
	ULONG* pParamSize
	)
{
	ULONG FltSize;
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	ULONG tmp;
	ULONG cou;

	FltSize = sizeof(QFLT);
	*pParamSize = 0;
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		FltSize += tmp;
		*pParamSize += tmp;
		pPointer = (BYTE*)pParam + tmp;
		
		pParam = (PFILTER_PARAM) pPointer;
	}
	
	return FltSize;
}

DWORD CalcMappedParamSize(PQFLT pQFlt, DWORD* pParamCount)
{
	DWORD MappedParamSize;
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD cou;
	
	MappedParamSize = 0;
	*pParamCount = 0;
	
	
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		if (pParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT)
		{
			(*pParamCount)++;
			MappedParamSize += sizeof(SINGLE_PARAM) + pParam->m_ParamSize;
		}
		
		pPointer = (BYTE*)pParam + sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pParam = (PFILTER_PARAM) pPointer;
	}
	
	return MappedParamSize;
}

PFILTER_PARAM
GetParamPtrFromFilter(PQFLT pQFlt, ULONG ParamID)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		if (pParam->m_ParamID == ParamID)
			return pParam;

		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}

	return NULL;
}

void CopyMappedParams(PMAPPED_PARAMS pMappedParams, PQFLT pQFlt)
{
	PFILTER_PARAM pParam;
	PSINGLE_PARAM pParamDest;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
	
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	pParamDest = (PSINGLE_PARAM) pMappedParams->m_SingleParams;
	
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		
		if (pParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT)
		{
			pParamDest->ParamID = pParam->m_ParamID;
			pParamDest->ParamSize = pParam->m_ParamSize;
			pParamDest->ParamFlags = _SINGLE_PARAM_FLAG_MAPPED_FILTER_PARAM;
			memcpy(pParamDest->ParamValue, pParam->m_ParamValue, pParam->m_ParamSize);
			
			pPointer = (BYTE*)pParamDest + sizeof(SINGLE_PARAM) + pParam->m_ParamSize;
			pParamDest = (PSINGLE_PARAM) pPointer;
		}
		
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}
}

ULONG CalcFilterTransmitSize (
	PFILTER_TRANSMIT pFltIn
	)
{
	DWORD FilterSize;
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
#if DBG == 1
	PCHAR pch;
#endif
	FilterSize = sizeof(FILTER_TRANSMIT);
	pParam = (PFILTER_PARAM) pFltIn->m_Params;
	for (cou = 0; cou < pFltIn->m_ParamsCount; cou++)
	{
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		FilterSize += tmp;
		pPointer = (BYTE*)pParam + tmp;
#if DBG == 1
		pch = pParam->m_ParamValue;
#endif
		pParam = (PFILTER_PARAM) pPointer;
	}
	return FilterSize;
}

BOOLEAN
CheckEventParams (
	PFILTER_EVENT_PARAM pEvent,
	ULONG EventSize
	)
{
	PSINGLE_PARAM pSingleParam = NULL;
	DWORD cou;

	if (EventSize < sizeof(FILTER_EVENT_PARAM))
	{
		_dbg_break_;
		return FALSE;
	}

	EventSize -= sizeof(FILTER_EVENT_PARAM);

	pSingleParam = (PSINGLE_PARAM) pEvent->Params;
	for (cou =  0; cou < pEvent->ParamsCount; cou++)
	{
		if (EventSize < pSingleParam->ParamSize + sizeof(SINGLE_PARAM))
		{
			_dbg_break_;
			return FALSE;
		}

		EventSize -= (pSingleParam->ParamSize + sizeof(SINGLE_PARAM));
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}

	return TRUE;
}

BOOLEAN
Filters_Switch (
	ULONG ClientId,
	ULONG FilterId,
	BOOLEAN bEnable
	)
{
	PQFLT pQFlt = Filters_Find( ClientId, FilterId );
	if (pQFlt)
	{
		if (bEnable)
			ClearFlag( pQFlt->m_Flags, FLT_A_DISABLED );
		else
			SetFlag( pQFlt->m_Flags, FLT_A_DISABLED );

		ReleaseResource( &gHookFilters[_Hook2Idx( pQFlt->m_HookID )].m_Sync );

		return TRUE;
	}

	return FALSE;
}

NTSTATUS
Filters_ChangeParams (
	ULONG ClientId,
	ULONG FilterId,
	ULONG ParamsCount,
	PFILTER_PARAM pParams
	)
{
	NTSTATUS Status = STATUS_NOT_FOUND;
	PQFLT pQFlt = NULL;

	if (1 != ParamsCount)
		return STATUS_NOT_SUPPORTED;

	if (!FilterId)
		return STATUS_NOT_SUPPORTED;

	pQFlt = Filters_Find( ClientId, FilterId );

	if (!pQFlt)
	{
		_dbg_break_;
	}
	else
	{
		PFILTER_PARAM pFltParam = GetParamPtrFromFilter( pQFlt, pParams->m_ParamID );
		if (!pFltParam)
			Status = STATUS_NOT_FOUND;
		else
		{
			if (pFltParam->m_ParamSize != pParams->m_ParamSize)
				Status = STATUS_NOT_SUPPORTED;
			else
			{
				memcpy(pFltParam, pParams, sizeof(FILTER_PARAM) + pParams->m_ParamSize);
				Status = STATUS_SUCCESS;	
			}
		}

		ReleaseResource( &gHookFilters[_Hook2Idx( pQFlt->m_HookID )].m_Sync );
	}

	return Status;
}

// -----------------------------------------------------------------------------------------
VOID
Filters_FillOutData (
	PFILTER_TRANSMIT pFilterOut,
	PQFLT pQFlt,
	ULONG FilterParamsSize
	)
{
	pFilterOut->m_AppID = pQFlt->m_ClientId;
	pFilterOut->m_HookID = pQFlt->m_HookID;
	pFilterOut->m_FunctionMj = pQFlt->m_Mj;
	pFilterOut->m_FunctionMi = pQFlt->m_Mi;
	pFilterOut->m_ProcessingType = pQFlt->m_ProcessingType;
	pFilterOut->m_Expiration = pQFlt->m_Expiration;
	pFilterOut->m_FilterID = pQFlt->m_FilterID;
	pFilterOut->m_Flags = pQFlt->m_Flags;
	pFilterOut->m_ParamsCount = pQFlt->m_ParamsCount;
	strncpy(pFilterOut->m_ProcName, pQFlt->m_pProcName, PROCNAMELEN);
	pFilterOut->m_Timeout = pQFlt->m_Timeout;

	RtlCopyMemory( pFilterOut->m_Params, pQFlt->m_Params, FilterParamsSize );
}

NTSTATUS
Filters_GetNext (
	ULONG ClientId,
	ULONG StartFilterId,
	PFILTER_TRANSMIT pOutputFilter,
	ULONG OutputBufferLength
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	PQFLT pQFlt = Filters_FindNext( ClientId, StartFilterId );
	if (pQFlt)
	{
		ULONG ParamSize = 0;
		ULONG HookID = pQFlt->m_HookID;
		
		Filters_CalcParamSize( pQFlt, &ParamSize );
		if ((ParamSize + sizeof(FILTER_TRANSMIT)) > OutputBufferLength)
			status = STATUS_BUFFER_TOO_SMALL;
		else
		{
			Filters_FillOutData( pOutputFilter, pQFlt, ParamSize );

			status = STATUS_SUCCESS;
		}

		Filters_Release( pQFlt );
	}

	return status;
}

// -----------------------------------------------------------------------------------------
#pragma intrinsic(memcmp)

BOOLEAN
CheckSingleParam (
	PFILTER_EVENT_PARAM pEventParam,
	PFILTER_PARAM pFilterParam,
	DWORD* pdwCacheValue
	)
{
	BOOLEAN bMatch = TRUE;
	PSINGLE_PARAM pEvParam;
	BYTE *pPointer;
	DWORD cou;
	
	BOOLEAN ParamFound = FALSE;
	
	pEvParam = (PSINGLE_PARAM) pEventParam->Params;
	for (cou = 0; cou < pEventParam->ParamsCount && bMatch; cou++)
	{
		if (pEvParam->ParamID == pFilterParam->m_ParamID) 
		{
			ParamFound = TRUE;
#if DBG == 1
			if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_DEBUG_BREAK)
			{
				_dbg_break_;
			}
#endif
			switch (pFilterParam->m_ParamFlt)
			{
			case FLT_PARAM_WILDCARD:
				{
					//compare wildcard
					if (FALSE == MatchWithPattern(pFilterParam->m_ParamValue, pEvParam->ParamValue, FALSE, 
						pFilterParam->m_ParamID))
						bMatch = FALSE;

					/*DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "id: %d %S: '%S' - '%S'",
						pFilterParam->m_ParamID,
						bMatch ? L"matched" : L"not matched",
						(PWCHAR) pEvParam->ParamValue,
						(PWCHAR) pFilterParam->m_ParamValue
						);*/
				}
				break;
			case FLT_PARAM_NOP:
				break;
			case FLT_PARAM_AND:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else
				{
					bMatch = FALSE;

					if (pEvParam->ParamSize == sizeof(ULONG))
					{
						ULONG u1 = *(ULONG*) pEvParam->ParamValue;
						ULONG u2 = *(ULONG*) pFilterParam->m_ParamValue;

						if (u1 & u2)
							bMatch = TRUE;

						/*DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "id: %d %S: 0x%x - 0x%x",
							pFilterParam->m_ParamID,
							bMatch ? L"matched" : L"not matched",
							u1,
							u2
							);*/
					}
					else
					{
						BYTE b1, b2;
						DWORD tmpi;
						for (tmpi = 0; tmpi < (pFilterParam->m_ParamSize) && (!bMatch); tmpi++)
						{
							b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
							b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
							if ((b1 & b2) != 0)
								bMatch = TRUE;
						}
					}
				}
				break;
			case FLT_PARAM_EUA:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else
				{
					if (memcmp(pEvParam->ParamValue, pFilterParam->m_ParamValue, pFilterParam->m_ParamSize))
						bMatch = FALSE;
				}
				break;
			case FLT_PARAM_WILDCARDSENS:
				{
					//compare wildcard
					if (FALSE == MatchWithPattern(pFilterParam->m_ParamValue, pEvParam->ParamValue, TRUE, 
						pFilterParam->m_ParamID))
						bMatch = FALSE;
				}
				break;
			case FLT_PARAM_MASKUNSENS_LIST:
			case FLT_PARAM_MASK_LIST:
			case FLT_PARAM_MASKUNSENS_LISTW:
			case FLT_PARAM_MASKSENS_LISTW:
				{
					int tmplen;
					BYTE *p1 = pFilterParam->m_ParamValue;
					int dwLastLen = pFilterParam->m_ParamSize;

					int zerolen = sizeof(CHAR);
					BOOLEAN bUni = FALSE;
					BOOLEAN bSens = FALSE;

					bMatch = FALSE;

					if ((FLT_PARAM_MASK_LIST == pFilterParam->m_ParamFlt) || (FLT_PARAM_MASKSENS_LISTW == pFilterParam->m_ParamFlt))
						bSens = TRUE;
					
					if ((FLT_PARAM_MASKUNSENS_LISTW == pFilterParam->m_ParamFlt) || 
						(FLT_PARAM_MASKSENS_LISTW == pFilterParam->m_ParamFlt) || 
						IsParameterUnicode(pFilterParam->m_ParamID))
					{
						bUni = TRUE;
						zerolen = sizeof(WCHAR);
					}

					while ((dwLastLen > zerolen) && (!bMatch))
					{
						if (bUni)
						{
							bMatch = MatchWithPatternW((PWCHAR) p1, (PWCHAR)pEvParam->ParamValue, bSens);
							/*DbPrint(DC_PARAMS, DL_SPAM, ("check ustring parameter %x (%s) pattern '%S' with '%S'\n", 
								pFilterParam->m_ParamID, bMatch ? "ok" : "not matched", 
								p1, pEvParam->ParamValue));*/

						}
						else
							bMatch = MatchWithPatternA(p1, pEvParam->ParamValue, bSens);
						
						if (!bMatch)
						{
							if (bUni)
							{
								tmplen = (wcslen((PWCHAR) p1) + 1) * sizeof(WCHAR);
								dwLastLen -= tmplen;
								p1 += tmplen;
							}
							else
							{
								tmplen = strlen(p1) + 1;
								dwLastLen -= tmplen;
								p1 += tmplen;
							}
						}
					}
				}
				break;

			case FLT_PARAM_EQU_LIST:
				{
					BYTE *p1 = pFilterParam->m_ParamValue;
					DWORD dwLastLen = pFilterParam->m_ParamSize;
					bMatch = FALSE;
					while ((dwLastLen > 0) && (!bMatch))
					{
						if (dwLastLen < pEvParam->ParamSize)
							break;
						if (memcmp(pEvParam->ParamValue, p1, pEvParam->ParamSize) == 0)
							bMatch = TRUE;
						else
						{
							p1 += pEvParam->ParamSize;
							dwLastLen -= pEvParam->ParamSize;
						}
					}
#if DBG == 1
					if ((pFilterParam->m_ParamSize % pEvParam->ParamSize) != 0)
					{
//						DbPrint(DC_FILTER,DL_WARNING, ("EQU_LIST operator for ParamID %d - size mismatch: event %d, param %d\n", 
//							pFilterParam->m_ParamID, pEvParam->ParamSize, pFilterParam->m_ParamSize));
						_dbg_break_;
					}
#endif
				}
				break;
			case FLT_PARAM_ABV:
				if (pEvParam->ParamSize < pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else
				{
					BYTE b1, b2;
					DWORD tmpi;
					for (tmpi = 0; tmpi < (pFilterParam->m_ParamSize) && bMatch; tmpi++)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 < b2)
							bMatch = FALSE;
					}
				}
				break;
			case FLT_PARAM_BEL:
				if (pEvParam->ParamSize > pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else {
					BYTE b1, b2;
					DWORD tmpi;
					for (tmpi = 0; tmpi < (pFilterParam->m_ParamSize) && bMatch; tmpi++)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 > b2)
							bMatch = FALSE;
					}
				}
				break;
			case FLT_PARAM_MORE:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
				{
//					DbPrint(DC_FILTER,DL_WARNING, ("Param size mismatch in FLT_PARAM_MORE for ParamID %d)!\n", 
//						pFilterParam->m_ParamID));
					bMatch = FALSE;
				}
				else
				{
					BYTE b1, b2;
					int tmpi;
					for (tmpi = pFilterParam->m_ParamSize - 1; (tmpi >= 0) && bMatch; tmpi--)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 > b2)
							bMatch = FALSE;
						else if (b1 < b2)
							break;
					}
				}
				break;
			case FLT_PARAM_LESS:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
				{
//					DbPrint(DC_FILTER,DL_WARNING, ("Param size mismatch in FLT_PARAM_LESS for ParamID %d)!\n", 
//						pFilterParam->m_ParamID));
					bMatch = FALSE;
				}
				else {
					BYTE b1, b2;
					int tmpi;
					for (tmpi = pFilterParam->m_ParamSize - 1; (tmpi >= 0) && bMatch; tmpi--)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 < b2) 
							bMatch = FALSE;
						else if (b1 > b2) 
							break;
					}
				}
				break;
			case FLT_PARAM_MASK:
				//Filter param = value + mask; sizeof(value) = sizeof(mask); total param size = sizeof(value) + sizeof(mask)
				if (2*pEvParam->ParamSize != pFilterParam->m_ParamSize)
				{
//					DbPrint(DC_FILTER,DL_WARNING, ("Param size mismatch in FLT_PARAM_MASK for ParamID %d)!\n", 
//						pFilterParam->m_ParamID));
					bMatch = FALSE;
				}
				else
				{
					BYTE b1, b2, msk;
					DWORD tmpi;
					for (tmpi = 0; (tmpi < pEvParam->ParamSize) && bMatch; tmpi++)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						msk = *(((BYTE*) pFilterParam->m_ParamValue) + pEvParam->ParamSize + tmpi);
						if ((b1 & msk) != (b2 & msk))
							bMatch = FALSE;
					}
				}
				break;
			default:
				{
					bMatch = FALSE;
//					DbPrint(DC_FILTER,DL_WARNING, ("Unsupported operation (%d)!\n", pFilterParam->m_ParamFlt));
				}
			}
			
			if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_INVERS_OP)
				bMatch = !bMatch;
			
			if (bMatch && (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE))
			{
				if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHEUPREG)
				{
					*pdwCacheValue = CalcCacheIDUnSens(pEvParam->ParamSize, pEvParam->ParamValue, *pdwCacheValue, 
						IsParameterUnicode(pFilterParam->m_ParamID));
				}
				else
					*pdwCacheValue = CalcCacheID(pEvParam->ParamSize, pEvParam->ParamValue, *pdwCacheValue);
			}
		}
		
		if (ParamFound)
		{
			if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_SINGLEID)
			{
				break;
			}
		}

		pPointer = (BYTE*)pEvParam + sizeof(SINGLE_PARAM) + pEvParam->ParamSize;
		pEvParam = (PSINGLE_PARAM) pPointer;
	}

	if (!ParamFound)
	{
		if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MUSTEXIST)
			return FALSE;
	}

	if (bMatch)
	{
		if ((pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE) && 
			(pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT))
		{
			*pdwCacheValue = CalcCacheID(pFilterParam->m_ParamSize, pFilterParam->m_ParamValue, *pdwCacheValue);
		}
	}
	
	return bMatch;
}

BOOLEAN
IsNeedFilterEventSingle (
	ULONG HookID,
	ULONG FuncMj,
	ULONG FuncMi
	)
{
	UNREFERENCED_PARAMETER( FuncMj );
	UNREFERENCED_PARAMETER( FuncMi );

	if (gHookFilters[_Hook2Idx( HookID )].m_FiltersCount )
		return TRUE;

	return FALSE;
}

BOOLEAN
__stdcall
IsNeedFilterEventEx (
	ULONG HookID,
	ULONG FuncMj,
	ULONG FuncMi,
	HANDLE ThreadID
	)
{
	BOOLEAN bRet = FALSE;
	
	if(IsInvisible( ThreadID, 0 ))
		return FALSE;
	
	return IsNeedFilterEventSingle( HookID, FuncMj, FuncMi );
}

BOOLEAN
__stdcall
IsNeedFilterEvent (
	ULONG HookID,
	ULONG FuncMj,
	ULONG FuncMi
	)
{
	return IsNeedFilterEventEx( HookID, FuncMj, FuncMi, PsGetCurrentThreadId() );
}

PSINGLE_PARAM
GetSingleParamPtr (
	PFILTER_EVENT_PARAM pEventParam,
	ULONG ParamID
	)
{
	DWORD cou;
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEventParam->Params;
	for (cou =  0; cou < pEventParam->ParamsCount; cou++)
	{
		if (pSingleParam->ParamID == ParamID)
			return pSingleParam;
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}
	
	return NULL;
}

void
FilterFillSID (
	PFILTER_EVENT_PARAM pEventParam
	)
{
	DWORD sid_length;
	PSINGLE_PARAM pSidParam;
	pSidParam = GetSingleParamPtr(pEventParam, _PARAM_OBJECT_SID);
	if (pSidParam != NULL)
	{
		if (pSidParam->ParamValue[0] == 0)
		{
			sid_length = pSidParam->ParamSize = __SID_LENGTH;
//			if (SeGetSID(pSidParam->ParamValue, &sid_length) == FALSE)
//				DbPrint(DC_FILTER, DL_SPAM, ("can't get sid on %s\n", pEventParam->ProcName));
		}
	}
}

ULONG
_IsFiltered(PFILTER_EVENT_PARAM pEventParam, ULONG ClientId, ULONG StartClientId, PFILTER_SUB pFilterSub)
{
	PFUNC_DATA pFilters = NULL;
	PLIST_ENTRY Flink = NULL;
	PQFLT pQFlt = NULL;
	ULONG FltIDExpired = _INVALID_FILTER_ID;
	
	PQCLIENT pClient = NULL;
	PQSUBCLIENT pSubClient = NULL;

	ULONG SkipClient = 0;

	if (FLTTYPE_EMPTY == pEventParam->HookID)
		return _SYSTEM_APPLICATION;
	
	if (pFilterSub->m_pMappedFilterParams)
	{
		ExFreePool( pFilterSub->m_pMappedFilterParams );
		pFilterSub->m_pMappedFilterParams = NULL;
	}
	
	/*if (!pEventParam->ProcName[0])
		GetProcName( pEventParam->ProcName, NULL );*/
	
	FilterFillSID( pEventParam );
							
	pFilters = Filters_GetListMi( pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi, FALSE );
	if (!pFilters)
		return _SYSTEM_APPLICATION;

	if (IsListEmpty( &pFilters->m_List ))
	{
		ReleaseResource( &gHookFilters[_Hook2Idx( pEventParam->HookID )].m_Sync );
		return _SYSTEM_APPLICATION;
	}

	Flink = pFilters->m_List.Flink;
	while (Flink != &pFilters->m_List)
	{
		pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );

		if (_SYSTEM_APPLICATION == StartClientId )
			break;
		
		if (StartClientId != pQFlt->m_ClientId)
			Flink = Flink->Flink;
		else
		{
			while (Flink != &pFilters->m_List)
			{
				pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );
				if (StartClientId != pQFlt->m_ClientId)
				{
					StartClientId = _SYSTEM_APPLICATION; // next client
					break;
				}
				if (pFilterSub->m_FilterID == pQFlt->m_FilterID )
				{
					Flink = Flink->Flink; // take next filter
					break;
				}
				Flink = Flink->Flink;
			}
		}
	}

	while (Flink != &pFilters->m_List)
	{
		DWORD cou;
		PFILTER_PARAM pFilterParam;
		
		BYTE *pPointer;
		
		pFilterSub->m_dwCacheValue = _INVALID_VALUE;

		pQFlt = CONTAINING_RECORD( Flink, QFLT, m_List );
		
		if ((pQFlt->m_Expiration) && (IsTimeExpired( pQFlt->m_Expiration )))
		{
			FltIDExpired = pQFlt->m_FilterID; // filter expired! will be deleted on exit
			
			Flink = Flink->Flink;
			continue;
		}

		if (pQFlt->m_Flags & FLT_A_DISABLED)	// filter disabled
		{
			Flink = Flink->Flink;
			continue;
		}

		if (ClientId && ClientId != pQFlt->m_ClientId)
		{
			// search exact client
			Flink = Flink->Flink;
			continue;
		}

		if (SkipClient && SkipClient == pQFlt->m_ClientId)
		{
			// skip this client by FLT_A_STOPPROCESSING in filter
			Flink = Flink->Flink;
			continue;
		}

			
		if (!pClient || (pClient->m_AppID != pQFlt->m_ClientId))
		{
			if (pClient)
			{	
				ReleaseClient(pClient);
				pClient = NULL;
			}

			if (pSubClient)
			{
				if (pSubClient != &gNonPopupClient)
					ReleaseSubClient(pSubClient);

				pSubClient = NULL;
			}
			
			pClient = AcquireClient(pQFlt->m_ClientId);
			if (pClient)
			{
				pSubClient = GetFreeSubClient(pClient);
				if (!pSubClient	&& (!(pClient->m_ClientFlags & (_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_PAUSED))))
					pSubClient = &gNonPopupClient;

			}
		}

		if (pSubClient)
		{
			if ((pQFlt->m_ProcessingType == pEventParam->ProcessingType) || 
				(pQFlt->m_ProcessingType == AnyProcessing) || 
				(pEventParam->ProcessingType == CheckProcessing))
			{
				ULONG RetClientId = _SYSTEM_APPLICATION;
				BOOLEAN bParamFound = FALSE;
				if((!pQFlt->m_pProcName[0]) || (MatchWithPatternA( pQFlt->m_pProcName, pEventParam->ProcName, FALSE )))
				{
					BOOLEAN bFilterMatch = TRUE;
					ULONG ParamId = 0;
					for (cou = 0; (cou < pQFlt->m_ParamsCount); cou++)
					{
						if (!cou)
							pFilterParam = (PFILTER_PARAM) pQFlt->m_Params;
						else {
							pPointer = (BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize;
							pFilterParam = (PFILTER_PARAM) pPointer;
						}

						ParamId = pFilterParam->m_ParamID;
						
						if (FlagOn( pFilterParam->m_ParamFlags, _FILTER_PARAM_FLAG_SPECIAL_THISISPROCID ))
						{
							if (pFilterParam->m_ParamSize)
							{
								if (sizeof(ULONG) != pFilterParam->m_ParamSize)
								{
									bFilterMatch = FALSE;
									break;
								}
								else
								{
									if (*(ULONG*)pFilterParam->m_ParamValue != pEventParam->ProcessID)
									{
										bFilterMatch = FALSE;
										break;
									}
								}
							}
							
							if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE)
							{
								pFilterSub->m_dwCacheValue = CalcCacheID(sizeof(ULONG), 
									pFilterParam->m_ParamValue, pFilterSub->m_dwCacheValue);
							}
						}
						else if(!CheckSingleParam( pEventParam, pFilterParam, &(pFilterSub->m_dwCacheValue) ))
						{
							bFilterMatch = FALSE;
							break;
						}
					}

					if (bFilterMatch)
					{
						if (FlagOn( pQFlt->m_Flags, FLT_A_STOPPROCESSING ))
						{
							bFilterMatch = FALSE;
							SkipClient = pQFlt->m_ClientId;
							continue;
						}

						InterlockedIncrement(&pSubClient->m_Tasks);
						pFilterSub->m_pSubClient = pSubClient;
						
						pFilterSub->m_FilterID = pQFlt->m_FilterID;
						pFilterSub->m_Flags = pQFlt->m_Flags;
						pFilterSub->m_Timeout = pQFlt->m_Timeout;
						RetClientId = pQFlt->m_ClientId;
						
						if (pFilterSub->m_Flags & FLT_A_PROCESSCACHEABLE)
							pFilterSub->m_dwCacheValue = CalcCacheID(strlen(pEventParam->ProcName), 
							pEventParam->ProcName, pFilterSub->m_dwCacheValue);
						
						if (pFilterSub->m_Flags & FLT_A_TOUCH)
							FltIDExpired = pQFlt->m_FilterID; //may overwrite expired by timeout filter
						
						if (pQFlt->m_Flags & FLT_A_HAVE_MAPPED_PARAMS)
						{
							DWORD MappedParamsCount;
							DWORD MappedParamSize;
							
							MappedParamSize = CalcMappedParamSize(pQFlt, &MappedParamsCount);
							if (MappedParamSize)
							{
								pFilterSub->m_pMappedFilterParams = ExAllocatePoolWithTag(NonPagedPool,
									MappedParamSize + sizeof(MAPPED_PARAMS),'zboS');
								if (pFilterSub->m_pMappedFilterParams)
								{
									pFilterSub->m_pMappedFilterParams->m_ParamsCount = MappedParamsCount;
									pFilterSub->m_pMappedFilterParams->m_Param_Size = MappedParamSize;
									CopyMappedParams(pFilterSub->m_pMappedFilterParams, pQFlt);
								}
							}
						}

						ReleaseResource( &gHookFilters[_Hook2Idx( pEventParam->HookID )].m_Sync );
						ReleaseClient( pClient );
						
						return RetClientId;
					}
				}
			}
		}
		
		Flink = Flink->Flink;
	}

	ReleaseResource( &gHookFilters[_Hook2Idx( pEventParam->HookID )].m_Sync );

	if (pSubClient && pSubClient != &gNonPopupClient)
		ReleaseSubClient(pSubClient);

	if (pClient)
		ReleaseClient(pClient);
	
	if (_INVALID_FILTER_ID != FltIDExpired)
		Filters_Delete( _INVALID_CLIENT_ID, FltIDExpired );
	
	return _SYSTEM_APPLICATION;
}


// -----------------------------------------------------------------------------------------
// Function name	: FilterEvent
// Description	    : обработка события от хука
// Return type		: tefVerdict
// Argument         : ULONG Function	- function number
// Argument         : PCHAR ProcName	- process name
// Argument         : PCHAR ObjName		- object name
// Argument         : DWORD UnsafeValue	- 0 if ReplyWaiting absolutly safe (deadlock free)

tefVerdict
FilterEventImp (
	PFILTER_EVENT_PARAM pEventParam,
	PEVENT_OBJ pEventObj,
	ULONG ClientId
	)
{
	//! Помещать ли в лог информацию об ошибках при обработке event-а?
	PSINGLE_PARAM pSingleParam;
	
	PQCLIENT pClient;
	ULONG ClientIdTmp;
	FILTER_SUB FilterSub;
	DWORD dwCache;	// 0 - not cahced, 1 - cached but temporary dirty, 2 - cached
	DWORD EventFlags;
	BOOLEAN bNeedProcessing;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;

	Timing_Def;
	Timing_In;

	if (pEventParam->ProcessID)
		hProcess = UlongToHandle( pEventParam->ProcessID );
	else
	{
		hProcess = PsGetCurrentProcessId();
		pEventParam->ProcessID = *(ULONG*)&hProcess;
#if DBG == 1
		{
			HANDLE hProctmp = 0;
			*(ULONG*)&hProctmp = pEventParam->ProcessID;

			if (hProcess != hProctmp)
				_dbg_break_;
		}
#endif
	}

	if (pEventParam->ThreadID)
		hThread = ULongToHandle( pEventParam->ThreadID );
	else
	{
		hThread = PsGetCurrentThreadId();
		pEventParam->ThreadID = HandleToUlong( hThread );
	}
		
	if (IsInvisible( hThread, hProcess ))
		return efVerdict_NotFiltered;

	if (KeGetCurrentIrql() >= DISPATCH_LEVEL)
	{
		if (pEventParam->ProcessingType == PreDispatch)
			return Di_FilterEvent(pEventParam, NULL);

		return efVerdict_ClientRequest;
	}

	RtlZeroMemory(&FilterSub, sizeof(FilterSub));
	FilterSub.m_Verdict = efVerdict_NotFiltered;
	FilterSub.m_FilterID = _INVALID_FILTER_ID;

	pSingleParam = (PSINGLE_PARAM) pEventParam->Params;
	
	if (!pEventParam->UnsafeValue)
	{
		if (!CheckProc_IsAllowProcessExecuting())
			return efVerdict_Deny;
	}
	
	ClientIdTmp = _IsFiltered(pEventParam, ClientId, _SYSTEM_APPLICATION, &FilterSub);
	
	if (pEventParam->ProcessingType == CheckProcessing)
	{
		if (FilterSub.m_pSubClient)
		{
			if (FilterSub.m_pSubClient->m_Tasks > 0)
				InterlockedDecrement(&FilterSub.m_pSubClient->m_Tasks);

			if (FilterSub.m_pSubClient != &gNonPopupClient)
			{
				ReleaseSubClient(FilterSub.m_pSubClient);
				FilterSub.m_pSubClient = 0;
			}
		}
		if (ClientIdTmp == _SYSTEM_APPLICATION)
			FilterSub.m_Verdict = efVerdict_NotFiltered;
		else
		{
			FilterSub.m_Verdict = efVerdict_Allow;
			
			if (FilterSub.m_Flags & FLT_A_POPUP)
				FilterSub.m_Verdict = efVerdict_ClientRequest;
			else
			{
				if (FilterSub.m_Flags == FLT_A_DEFAULT)
					FilterSub.m_Verdict = efVerdict_Default;
				if (FilterSub.m_Flags & FLT_A_DENY)
					FilterSub.m_Verdict = efVerdict_Deny;
			}
		}
		
		if (FilterSub.m_pMappedFilterParams != NULL)
			ExFreePool(FilterSub.m_pMappedFilterParams);
		
		return FilterSub.m_Verdict;
	}
	
	if (ClientIdTmp == _SYSTEM_APPLICATION)
	{
		Timing_Out( _timing_filter, 0, 0 );
		return efVerdict_NotFiltered;
	}

	while (ClientIdTmp != _SYSTEM_APPLICATION)
	{
		pClient = AcquireClient( ClientIdTmp );
		ClientIdTmp = _SYSTEM_APPLICATION;
		
		if (pClient == NULL)
		{
//			DbPrint(DC_FILTER,DL_FATAL_ERROR, ("Filtered but client not found!\n"));
			if (FilterSub.m_pSubClient != &gNonPopupClient)
			{
				ReleaseSubClient(FilterSub.m_pSubClient);
				FilterSub.m_pSubClient = 0;
			}
		}
		else
		{
			FilterSub.m_Verdict = efVerdict_Default;
			EventFlags = _EVENT_FLAG_NONE;
			
			bNeedProcessing = FALSE;
			
			if(FilterSub.m_Flags & FLT_A_DENY)
				FilterSub.m_Verdict = efVerdict_Deny;
			else if(FilterSub.m_Flags & FLT_A_PASS)
				FilterSub.m_Verdict = efVerdict_Allow;
			
			if (pEventParam->ProcessingType == PostProcessing)
				EventFlags |= _EVENT_FLAG_POSTPROCESSING;

			// cache -----------------------------------------------------------------------------------
			dwCache = _CL_CACHE_NOTCHECKED;
			
			if (pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
			{
				if (FilterSub.m_Flags & FLT_A_RESETCACHE)
				{
					Obj_CacheReset(pClient);
					
					DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "Cache reset" );
				}
				else
				{
					if (FilterSub.m_Flags & FLT_A_USECACHE)
					{
						dwCache = Obj_IsCached(pClient, FilterSub.m_dwCacheValue, TRUE, &FilterSub.m_Verdict);
						if (dwCache == _CL_CACHE_CACHED)
						{
							DoTraceEx( TRACE_LEVEL_INFORMATION, DC_CACHE, "Cache hit verdict %d CacheValue: 0x%x",
								FilterSub.m_Verdict,
								FilterSub.m_dwCacheValue
								);

							FilterSub.m_Flags &= ~FLT_A_CHECKNEXTFILTER;
						}
						else
						{
							DoTraceEx( TRACE_LEVEL_INFORMATION, DC_CACHE, "Cache miss CacheValue: 0x%x",
								FilterSub.m_dwCacheValue
								);
						}
					}
				}
				
				if (FilterSub.m_Flags & FLT_A_SAVE2CACHE)
				{
					DWORD tmpCached;
					tefVerdict OldVerdict;
					if (dwCache == _CL_CACHE_NOTCHECKED)
						tmpCached = Obj_IsCached(pClient, FilterSub.m_dwCacheValue, TRUE, &OldVerdict);
					else
						tmpCached = dwCache;
					if (FilterSub.m_Flags & FLT_A_PASS)
					{
						if (tmpCached == _CL_CACHE_NOTCACHED)
						{
							Obj_CacheAdd(pClient, FilterSub.m_dwCacheValue, FALSE, FilterSub.m_Verdict, 0);
							
							DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "Cache add verdict %d CacheValue: 0x%x",
								FilterSub.m_Verdict, FilterSub.m_dwCacheValue );

						}
						else if (tmpCached == _CL_CACHE_CACHED_DIRTY)
						{
							Obj_CacheUpdate(pClient, FilterSub.m_dwCacheValue, FilterSub.m_Verdict, 0);

							DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "Cache update verdict %d CacheValue: 0x%x",
								FilterSub.m_Verdict, FilterSub.m_dwCacheValue );
						}
					}
					else
					{
						if (tmpCached == _CL_CACHE_NOTCACHED)
						{
							Obj_CacheAdd(pClient, FilterSub.m_dwCacheValue, TRUE, FilterSub.m_Verdict, 0);

							DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "Cache add verdict %d CacheValue: 0x%x",
								FilterSub.m_Verdict, FilterSub.m_dwCacheValue );
						}
					}
				}
			}
			
			// end cache -------------------------------------------------------------------------------
			if (dwCache != _CL_CACHE_CACHED)
				bNeedProcessing = TRUE;

			if (!FilterSub.m_pSubClient)
				bNeedProcessing = FALSE;
			else
			{
				switch (FilterSub.m_pSubClient->m_SubClientCommType)
				{
				case _SUB_CLIENT_COMM_TYPE_PORT:
					if (bNeedProcessing)
					{
						bNeedProcessing = FALSE;
						if (FilterSub.m_pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
						{
							if (FilterSub.m_Flags & (FLT_A_POPUP | FLT_A_INFO))
							{
								PMKLIF_EVENT_TRANSMIT pEventTr = QueueAllocEventTr( pEventParam, &FilterSub, EventFlags );
								if (!pEventTr)
									InterlockedIncrement( &pClient->m_FaultsSend );
								else
								{
									NTSTATUS status_comm = Comm_SingleEvent (
										pClient,
										FilterSub.m_pSubClient->m_pPortContext,
										pEventTr,
										pEventObj,
										&FilterSub,
										&FilterSub.m_Verdict
										);
									
									if ((STATUS_TIMEOUT == status_comm)
										&& FlagOn( pClient->m_ClientFlags, _CLIENT_FLAG_STRONG_PROCESSING )
										&& (FilterSub.m_pSubClient != &gNonPopupClient))
									{
										// reprocess events
										ULONG SubClientsCount = 0;
										PQSUBCLIENT* pSubList = NULL;
									
										DoTraceEx( TRACE_LEVEL_WARNING, DC_FILTER, "reprocess event!" );

										pSubList = Client_GetSubClientList (
											pClient,
											FilterSub.m_pSubClient,
											&SubClientsCount
											);

										if (pSubList)
										{										
											ULONG sub_cou = 0;
											PQSUBCLIENT pSubItem = NULL;
											for (sub_cou = 0; sub_cou < SubClientsCount; sub_cou++)
											{
												pSubItem = pSubList[sub_cou];

												if (Client_LockCommSub( pClient, pSubItem ))
												{
													if (FilterSub.m_pSubClient->m_Tasks > 0)
														InterlockedDecrement( &FilterSub.m_pSubClient->m_Tasks );
													ReleaseSubClient( FilterSub.m_pSubClient );
													
													InterlockedIncrement( &pSubItem->m_Tasks );
													FilterSub.m_pSubClient = pSubItem;

													status_comm = Comm_SingleEvent (
														pClient,
														FilterSub.m_pSubClient->m_pPortContext,
														pEventTr,
														pEventObj,
														&FilterSub,
														&FilterSub.m_Verdict
														);

													if (STATUS_NO_MEMORY == status_comm)
														break;

													if (STATUS_PORT_DISCONNECTED == status_comm)
														continue;
												}
											}

											ExFreePool( pSubList );
										}
									}

									ExFreePool(pEventTr);
								}
							}
						}
					}
					break;
				
				default:
					if (FilterSub.m_pSubClient != &gNonPopupClient)
					{
						bNeedProcessing = FALSE;
						_dbg_break_;
					}
					else
					{
					}
				}
			}
			
			if (bNeedProcessing)
			{
				_dbg_break_;
			}
			
			if (FilterSub.m_pSubClient)
			{
				if (FilterSub.m_pSubClient->m_Tasks > 0)
					InterlockedDecrement(&FilterSub.m_pSubClient->m_Tasks);
			}

			if (FilterSub.m_pSubClient != &gNonPopupClient)
			{
				ReleaseSubClient(FilterSub.m_pSubClient);
				FilterSub.m_pSubClient = 0;
			}

			if ((pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE) && (FilterSub.m_Flags & FLT_A_DROPCACHE))
			{
				Obj_CacheDel(pClient, FilterSub.m_dwCacheValue);
				
				DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "Cache del CacheValue: 0x%x",
					FilterSub.m_dwCacheValue );
			}
#if DBG == 1
			if (FilterSub.m_Flags & FLT_A_DBG_BREAK)
			{
//				DbPrint(DC_FILTER, DL_WARNING, ("Filter with make //.. making breakpoint in dbg version'\n"));
				_dbg_break_;
			}
#endif
			if (efIsAllow( FilterSub.m_Verdict ))
			{
				if (!FlagOn( FilterSub.m_Verdict, efVerdict_ContinueSearch ))
					FilterSub.m_FilterID = _INVALID_FILTER_ID;
				
				ClientIdTmp = _IsFiltered( pEventParam, ClientId, pClient->m_AppID, &FilterSub );
			}
			
			ReleaseClient(pClient);
		}
	}
	
	if (FilterSub.m_pMappedFilterParams != NULL)
		ExFreePool(FilterSub.m_pMappedFilterParams);
	
	Timing_Out( _timing_filter, 0, 0 );
	return FilterSub.m_Verdict;
}

VERDICT
__stdcall
FilterEvent (
	PFILTER_EVENT_PARAM pEventParam,
	PEVENT_OBJ pEventObj
	)
{
	VERDICT Verdict;
	UNREFERENCED_PARAMETER( pEventObj );

	Verdict = FilterEventImp( pEventParam, NULL, 0 );

	if (FlagOn( Verdict, efVerdict_NotFiltered ))
		return Verdict_NotFiltered;

	if (FlagOn( Verdict, efVerdict_ClientRequest ))
		return Verdict_UserRequest;

	if (efVerdict_Default == Verdict)
		return Verdict_Default;

	if (efIsDeny( Verdict ))
		return Verdict_Discard;

	return Verdict_Pass;
}
					
// -----------------------------------------------------------------------------------------
NTSTATUS
FilterTransmit (
	PFILTER_TRANSMIT pFilterTransmitIn,
	PFILTER_TRANSMIT pFilterTransmitOut,
	ULONG OutputBufferLength
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	PQCLIENT pClient = AcquireClient( pFilterTransmitIn->m_AppID );
	if (!pClient)
		return STATUS_NOT_FOUND;

	if (FLTCTL_RESET_FILTERS == pFilterTransmitIn->m_FltCtl)
		Filters_Clear( pClient->m_AppID );

	ReleaseClient( pClient );
	
	switch(pFilterTransmitIn->m_FltCtl)
	{
	case FLTCTL_FIRST:
		status = Filters_GetNext( pFilterTransmitIn->m_AppID, _INVALID_FILTER_ID, pFilterTransmitOut, OutputBufferLength );
		if (NT_SUCCESS( status ))
			pFilterTransmitIn->m_FilterID = pFilterTransmitOut->m_FilterID;
		else
			status = STATUS_NOT_FOUND;
		break;

	case FLTCTL_NEXT:
		status = Filters_GetNext( pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, pFilterTransmitOut, OutputBufferLength );
		if (NT_SUCCESS( status ))
			pFilterTransmitIn->m_FilterID = pFilterTransmitOut->m_FilterID;
		else
			status = STATUS_NOT_FOUND;
		break;

	case FLTCTL_ADD:
		status = Filters_Add( pFilterTransmitIn, _QUEUE_BOTTOM );
		break;

	case FLTCTL_ADD2TOP:
		status = Filters_Add( pFilterTransmitIn, _QUEUE_TOP );
		break;

	case FLTCTL_ADD2POSITION:
		status = Filters_Add( pFilterTransmitIn, _QUEUE_POSITION );
		break;

	case FLTCTL_DEL:
		status = Filters_Delete( _INVALID_CLIENT_ID, pFilterTransmitIn->m_FilterID );
		break;

	case FLTCTL_DISABLE_FILTER:
		if (!Filters_Switch( pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, FALSE ))
			status = STATUS_UNSUCCESSFUL;
		else
			Di_FilterSwitch( pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, FALSE );
		break;

	case FLTCTL_ENABLE_FILTER:
		if (!Filters_Switch( pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, TRUE ))
			status = STATUS_UNSUCCESSFUL;
		else
			Di_FilterSwitch( pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, TRUE );
		break;

	case FLTCTL_RESET_FILTERS:
		status = STATUS_SUCCESS;
		break;

	case FLTCTL_CHANGE_FILTER_PARAM:
		status = Filters_ChangeParams( pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, 
			pFilterTransmitIn->m_ParamsCount, (FILTER_PARAM*) pFilterTransmitIn->m_Params );
		break;

	default:
		status = STATUS_NOT_FOUND;
		break;
	}
	
	return status;
}

void
InitStartFiltering (
	PEXT_START_FILTERING pStartFiltering
	)
{
	pStartFiltering->m_FltVersion = HOOK_INTERFACE_NUM;
	pStartFiltering->m_pIsNeedFilterEvent = IsNeedFilterEvent;
	pStartFiltering->m_pFilterEvent = (FILTER_EVENT_FUNC) FilterEvent;
	pStartFiltering->m_pIsNeedFilterEventEx = IsNeedFilterEventEx;
}

// --- Dispatch filtering

BOOLEAN Di_FilterParams(PDI_FILTER pDiFilter, PFILTER_EVENT_PARAM pEventParam)
{
	BOOLEAN bRet = TRUE;
	ULONG cou;
	BYTE *pPointer;
	
	PFILTER_PARAM pFilterParam = (PFILTER_PARAM) pDiFilter->m_Params;

	for (cou = 0; (cou < pDiFilter->m_ParamsCount) && bRet; cou++)
	{
		if ((pFilterParam->m_ParamFlt != FLT_PARAM_NOP) ||
			pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MUSTEXIST)
		{
			bRet = CheckSingleParam( pEventParam, pFilterParam, NULL );
		}
		
		pPointer = (BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize;
		pFilterParam = (PFILTER_PARAM) pPointer;
	}

	return bRet;
}

tefVerdict
Di_FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject)
{
	tefVerdict Verdict = efVerdict_NotFiltered;
	KIRQL NewIrql;

	UNREFERENCED_PARAMETER(pEventObject);

	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	if (!_impIsListEmpty( &gDi_FltList ))
	{
		BOOLEAN bMatch;
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;

		ULONG SkipClientId = _SYSTEM_APPLICATION;

		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_bClientPaused
				|| pDiFilter->m_Flags & FLT_A_DISABLED
				|| SkipClientId == pDiFilter->m_ClientId
				)
				continue;
			
			if (pDiFilter->m_HookID != pEventParam->HookID
				|| pDiFilter->m_FunctionMj != pEventParam->FunctionMj
				|| pDiFilter->m_FunctionMi != pEventParam->FunctionMi
				)
				continue;

			bMatch = Di_FilterParams(pDiFilter, pEventParam);

			if (bMatch)
			{
				if (FLT_A_STOPPROCESSING & pDiFilter->m_Flags)
					SkipClientId = pDiFilter->m_ClientId;	// skip client
				else
				{
					if (pDiFilter->m_Flags & (FLT_A_POPUP | FLT_A_LOG | FLT_A_NOTIFY | FLT_A_INFO))
						Verdict = efVerdict_ClientRequest;
					else
					{
						if (pDiFilter->m_Flags & FLT_A_DENY)
							Verdict = efVerdict_Deny;
						else if (pDiFilter->m_Flags & FLT_A_PASS)
							Verdict = efVerdict_Allow;
						else
							Verdict = efVerdict_Default;
					}

					break;
				}
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);

	return Verdict;
}

BOOLEAN Di_FltCheckParams(PFILTER_TRANSMIT pQFlt)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;

	ULONG cou;

	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		if (pParam->m_ParamFlt == FLT_PARAM_WILDCARD)
			return FALSE;

		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}

	return TRUE;
}

PDI_FILTER Di_AllocateFilter(PFILTER_TRANSMIT pFltIn)
{
	PDI_FILTER pDiFilter = NULL;

	ULONG ParamSize = 0;
	
	ParamSize = CalcFilterTransmitSize(pFltIn) - sizeof(FILTER_TRANSMIT);

	pDiFilter = ExAllocatePoolWithTag(NonPagedPool, ParamSize + sizeof(DI_FILTER), 'DSOB');
	if (!pDiFilter)
		return pDiFilter;

	pDiFilter->m_HookID = pFltIn->m_HookID;
	pDiFilter->m_FunctionMj = pFltIn->m_FunctionMj;
	pDiFilter->m_FunctionMi = pFltIn->m_FunctionMi;
	pDiFilter->m_ClientId = pFltIn->m_AppID;
	pDiFilter->m_bClientPaused = FALSE;
	pDiFilter->m_FilterID = 0;
	pDiFilter->m_Flags = pFltIn->m_Flags;
	pDiFilter->m_ParamsCount = pFltIn->m_ParamsCount;

	memcpy(pDiFilter->m_Params, pFltIn->m_Params, ParamSize);
	Di_PrepareFilterParams(pDiFilter);

	return pDiFilter;
}

void Di_FreeFlt(PDI_FILTER pDiFilter)
{
	ExFreePool(pDiFilter);
}

void Di_AddFilter(PDI_FILTER pDiFilter, eQUEUE_VAL eSite, ULONG FilterID)
{
	KIRQL NewIrql;

	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	if (eSite == _QUEUE_POSITION)
	{
		_dbg_break_;
	}

	pDiFilter->m_FilterID = FilterID;

	switch (eSite)
	{
	case _QUEUE_TOP:
		_impInsertHeadList(&gDi_FltList, &pDiFilter->m_List);
		break;
	case _QUEUE_BOTTOM:
		_impInsertTailList(&gDi_FltList, &pDiFilter->m_List);
		break;
	case _QUEUE_POSITION:
		_dbg_break_;
		break;
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_DelFilter(ULONG FilterID)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	if (!_impIsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_FilterID == FilterID)
			{
				_impRemoveEntryList(&pDiFilter->m_List);
				Di_FreeFlt(pDiFilter);
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_ResetFiltersForApp(ULONG ClientId)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	if (!_impIsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_ClientId == ClientId)
			{
				_impRemoveEntryList(&pDiFilter->m_List);
				Di_FreeFlt(pDiFilter);
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_FilterSwitch(ULONG ClientId, ULONG FilterID, BOOLEAN bEnable)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	if (!_impIsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if ((pDiFilter->m_ClientId == ClientId) && (pDiFilter->m_FilterID == FilterID))
			{
				if (bEnable)
					pDiFilter->m_Flags &= ~FLT_A_DISABLED;
				else
					pDiFilter->m_Flags |= FLT_A_DISABLED;
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

VOID
Di_PauseClient (
	ULONG ClientId,
	BOOLEAN bGoSleep
	)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	if (!_impIsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_ClientId == ClientId)
			{
				if (bGoSleep)
					pDiFilter->m_bClientPaused = TRUE;
				else
					pDiFilter->m_bClientPaused = FALSE;
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_PrepareFilterParams(PDI_FILTER pDiFilter)
{
	PFILTER_PARAM pFilterParam = (PFILTER_PARAM) pDiFilter->m_Params;
	ULONG cou;
	BYTE *pPointer;

	for (cou = 0; (cou < pDiFilter->m_ParamsCount); cou++)
	{
		pFilterParam->m_ParamFlags &= ~_FILTER_PARAM_FLAG_CACHABLE;
		
		pPointer = (BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize;
		pFilterParam = (PFILTER_PARAM) pPointer;
	}
}

// ---------------------------------------------------------------------------
#define SizeOfStringFiltersKeyName 8
WCHAR FiltersKeyNameBuffer[SizeOfStringFiltersKeyName+1]=L"\\Filters";
UNICODE_STRING FiltersKeyName = {SizeOfStringFiltersKeyName * sizeof(WCHAR),
	( SizeOfStringFiltersKeyName + 1) * sizeof(WCHAR),
	FiltersKeyNameBuffer};

WCHAR *LogFNameValBuffer=L"Client";
WCHAR *BldValBuffer=L"Build";

#define _SAVE_FILTER_SIZE	4096*2 
BOOLEAN
SaveFilters (
	PQCLIENT pClient
	)
{
	NTSTATUS				ntstatus;
	HANDLE  				ParKeyHandle;
	HANDLE					KeyHandle;
	ULONG					i;
	DWORD					dwFilterSize;
	DWORD					StartFilterID;
	PFILTER_TRANSMIT		pTmpFlt=NULL;
	PCLIENT_SAV				pClientRec=NULL;

	OBJECT_ATTRIBUTES	objectAttributes;
	UNICODE_STRING		FltKeyName;
	UNICODE_STRING		ValueName;
	WCHAR					ValueBuff[10];
	FltKeyName.Buffer=NULL;
	FltKeyName.Length=0;
	FltKeyName.MaximumLength=MAXPATHLEN;


// obsolete
//	if (!(pClient->m_ClientFlags & _CLIENT_FLAG_SAVE_FILTERS))
//		return STATUS_SUCCESS;
	
	pTmpFlt = ExAllocatePoolWithTag(PagedPool, _SAVE_FILTER_SIZE, 'fSeB');
	pClientRec = ExAllocatePoolWithTag(PagedPool,sizeof(CLIENT_SAV),'fSeB');
	
	if((pTmpFlt == NULL) || (pClientRec == NULL))
	{
		if(pClientRec != NULL)
			ExFreePool(pClientRec);
		if(pTmpFlt != NULL)
			ExFreePool(pTmpFlt);
		return FALSE;
	}
	
	memset(pClientRec, 0, sizeof(CLIENT_SAV));

	if(!(FltKeyName.Buffer=ExAllocatePoolWithTag(PagedPool,FltKeyName.MaximumLength,'fSeB')))
	{
		ExFreePool(pTmpFlt);
		ExFreePool(pClientRec);
		return FALSE;
	}

	AddInvisibleThread( PsGetCurrentThreadId() );

	InitializeObjectAttributes(&objectAttributes,&Globals.m_RegParams,OBJ_CASE_INSENSITIVE,NULL,NULL);
	if((ntstatus=ZwCreateKey(&ParKeyHandle,KEY_WRITE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
	{
		if((ntstatus=RtlIntegerToUnicodeString(pClient->m_AppID,0,&FltKeyName))==STATUS_SUCCESS)
		{
			InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
			if((ntstatus=ZwCreateKey(&KeyHandle,KEY_SET_VALUE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
			{
				i=HOOK_REGESTRY_INTERFACE_NUM;

				RtlInitUnicodeString(&ValueName,BldValBuffer);
				ZwSetValueKey(KeyHandle,&ValueName,0,REG_DWORD,&i,sizeof(i));

				pClientRec->Priority=pClient->m_Priority;
				pClientRec->ClientFlags = pClient->m_ClientFlags;
				if (pClient->m_pSubClient != NULL)
					pClientRec->BlueScreenTimeout = pClient->m_pSubClient->m_BlueScreenTimeout;
				else
					pClientRec->BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
				pClientRec->CacheSize = pClient->m_CacheSize;

				RtlInitUnicodeString(&ValueName,LogFNameValBuffer);
				ntstatus = ZwSetValueKey(KeyHandle,&ValueName,0,REG_BINARY,pClientRec,sizeof(CLIENT_SAV));
				ZwClose(KeyHandle);
				if((ntstatus=RtlAppendUnicodeStringToString(&FltKeyName,&FiltersKeyName))==STATUS_SUCCESS)
				{
					InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
					if((ntstatus=ZwCreateKey(&KeyHandle,KEY_WRITE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
					{
						ZwDeleteKey(KeyHandle);
						ZwClose(KeyHandle);
						if((ntstatus=ZwCreateKey(&KeyHandle,KEY_SET_VALUE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
						{
							i = 0;
							StartFilterID = -1;
							while(NT_SUCCESS( Filters_GetNext( pClient->m_AppID, StartFilterID, pTmpFlt, _SAVE_FILTER_SIZE)))
							{
								dwFilterSize = CalcFilterSize(pTmpFlt);
								if (dwFilterSize)
								{
									#if WINVER<0x0501
										swprintf(ValueBuff, L"%09u",i++);
									#else
										RtlStringCbPrintfW(ValueBuff, sizeof(ValueBuff), L"%09u", i);
									#endif

									RtlInitUnicodeString(&ValueName,ValueBuff);
									ntstatus=ZwSetValueKey(KeyHandle,&ValueName, 0, REG_BINARY, pTmpFlt, dwFilterSize);
								}
								StartFilterID = pTmpFlt->m_FilterID;
								i++;
							}
							ZwClose(KeyHandle);
						}
					}
				}
			}
		}
		ZwClose(ParKeyHandle);
	}

	DelInvisibleThread( PsGetCurrentThreadId(), FALSE );

	ExFreePool(FltKeyName.Buffer);
	
	ExFreePool(pTmpFlt);
	ExFreePool(pClientRec);

	return TRUE;
}

NTSTATUS
SaveFiltersForApp (
	ULONG ClientId
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PQCLIENT pClient = AcquireClient( ClientId );
	if (pClient)
	{
		if (SaveFilters( pClient ))
			status = STATUS_SUCCESS;

		ReleaseClient( pClient );
	}

	return status;
}

NTSTATUS
LoadFilters()
{
	OBJECT_ATTRIBUTES	objectAttributes;
	UNICODE_STRING		FltKeyName;
	UNICODE_STRING		ValueName;
	WCHAR					ValueBuff[10];
	ULONG					Res;
	NTSTATUS				ntstatus;
	HANDLE					ParKeyHandle;
	HANDLE					KeyHandle;
	HANDLE					FltKeyHandle;
	ULONG					i,KeyIndex=0,ClientId;
	PVOID					LoadBuff=NULL;
	PFILTER_TRANSMIT		pTmpFlt=NULL;
	PCHAR					FltKeyNameBuff = NULL;

	if(!((LoadBuff=ExAllocatePoolWithTag(PagedPool,PAGE_SIZE,'fSeB')) &&
		(FltKeyNameBuff=ExAllocatePoolWithTag(PagedPool,MAXPATHLEN,'fSeB'))))
	{
		if(FltKeyNameBuff)
			ExFreePool(FltKeyNameBuff);
		if(LoadBuff)
			ExFreePool(LoadBuff);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	AddInvisibleThread( PsGetCurrentThreadId() );

	InitializeObjectAttributes(&objectAttributes,&Globals.m_RegParams,OBJ_CASE_INSENSITIVE,NULL,NULL);
	ntstatus = ZwOpenKey(&ParKeyHandle,KEY_READ,&objectAttributes);
	if(NT_SUCCESS(ntstatus))
	{
		while(ZwEnumerateKey(ParKeyHandle,KeyIndex++,KeyBasicInformation,FltKeyNameBuff,MAXPATHLEN,&Res)==STATUS_SUCCESS)
		{
			((PKEY_BASIC_INFORMATION)FltKeyNameBuff)->Name[((PKEY_BASIC_INFORMATION)FltKeyNameBuff)->NameLength/sizeof(WCHAR)]=0;
			RtlInitUnicodeString(&FltKeyName,((PKEY_BASIC_INFORMATION)FltKeyNameBuff)->Name);
			FltKeyName.MaximumLength=MAXPATHLEN;
			if(RtlUnicodeStringToInteger(&FltKeyName,0,&ClientId)==STATUS_SUCCESS)  // MS bug -- Win2k/chk always return STATUS_SUCCESS
			{
				InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
				if((ntstatus=ZwOpenKey(&KeyHandle,KEY_READ,&objectAttributes))==STATUS_SUCCESS)
				{
					RtlInitUnicodeString(&ValueName,BldValBuffer);
					if((ntstatus=ZwQueryValueKey(KeyHandle,&ValueName,KeyValuePartialInformation,LoadBuff,PAGE_SIZE,&Res))==STATUS_SUCCESS)
					{
						if(*(ULONG*)(((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data)==HOOK_REGESTRY_INTERFACE_NUM)
						{
							RtlInitUnicodeString(&ValueName,LogFNameValBuffer);
							if((ntstatus=ZwQueryValueKey(KeyHandle,&ValueName,KeyValuePartialInformation,LoadBuff,PAGE_SIZE,&Res))==STATUS_SUCCESS)
							{
								if(RegisterAppFromSav(ClientId,(PCLIENT_SAV)(((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data)))
								{
									if((ntstatus=RtlAppendUnicodeStringToString(&FltKeyName,&FiltersKeyName))==STATUS_SUCCESS)
									{
										InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
										if((ntstatus=ZwCreateKey(&FltKeyHandle,KEY_WRITE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
										{
											i = 0;
											do {
												#if WINVER<0x0501
													swprintf(ValueBuff, L"%09u",i++);
												#else
													RtlStringCbPrintfW(ValueBuff, sizeof(ValueBuff), L"%09u",i++);
												#endif
												
												RtlInitUnicodeString(&ValueName,ValueBuff);
												ntstatus = ZwQueryValueKey(FltKeyHandle,&ValueName,KeyValuePartialInformation,LoadBuff,PAGE_SIZE,&Res );
												if(STATUS_SUCCESS == ntstatus)
												{
													pTmpFlt=(PFILTER_TRANSMIT) &((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data;
													Filters_Add( pTmpFlt, _QUEUE_BOTTOM );
												}
											} while (ntstatus==STATUS_SUCCESS);
											ZwClose(FltKeyHandle);
										}
									}
								}
							}
						}
					}
					ZwClose(KeyHandle);
				}
			}
		}
		
		ZwClose(ParKeyHandle);
	}

	DelInvisibleThread( PsGetCurrentThreadId(), FALSE );

	ExFreePool(FltKeyNameBuff);
	ExFreePool(LoadBuff);
	return ntstatus;
}

NTSTATUS
Filters_GetCountFor (
	ULONG ClientId,
	PULONG pFiltersCount
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PQCLIENT pClient = AcquireClient( ClientId );
	if (!pClient)
		return STATUS_NOT_FOUND;

	*pFiltersCount = pClient->m_FiltersCount;

	status = STATUS_SUCCESS;

	ReleaseClient( pClient );

	return status;
}

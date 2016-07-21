#include "InvThread.h"

typedef struct _INVISIBLELIST
 {
	struct _INVISIBLELIST	*Next;
	DWORD					RefCount;
	ULONG					ThreadID;
} INVISIBLELIST, *PINVISIBLELIST;

#ifdef _HOOK_NT_
#include "lists.h"

typedef struct _INVISIBLEPROCITEM
{
    IMPLIST_ENTRY	m_List;
    HANDLE			m_ProcessId;
} INVISIBLEPROCITEM, *PINVISIBLEPROCITEM;

IMPLIST_ENTRY	gInvisibleProcList;

#endif

PINVISIBLELIST g_pInvQueue = NULL;

ULONG gHint_NonInvisibleThread = 0;
ULONG gHint_InvisibleThread = 0;

BOOLEAN bInvsMutexInited = FALSE;

#ifdef _HOOK_NT_
KSPIN_LOCK gInvSpinLock;
NPAGED_LOOKASIDE_LIST gNListInvThreads;
NPAGED_LOOKASIDE_LIST* gpNListInvThreads = &gNListInvThreads;

#define AllocateInvisibleStruct ExAllocateFromNPagedLookasideList(gpNListInvThreads);
#define FreeInvisibleStruct(_pinv) ExFreeToNPagedLookasideList(gpNListInvThreads, _pinv);

#define InvLock(_retcode) KIRQL NewIrql; \
	if (!bInvsMutexInited) return _retcode; \
	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

#define InvLockVoid KIRQL NewIrql; \
	if (!bInvsMutexInited) return; \
	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

#define InvUnLock KeReleaseSpinLock( &gInvSpinLock, NewIrql );

#else
PERESOURCE gpInvResource = NULL;
#define AllocateInvisibleStruct ExAllocatePoolWithTag(NonPagedPool, sizeof(INVISIBLELIST), '|boS');
#define FreeInvisibleStruct(_pinv) ExFreePool(_pinv)

#define InvLock(_retcode) if (!bInvsMutexInited) return _retcode; \
	AcquireResource( gpInvResource, TRUE );

#define InvLockVoid if (!bInvsMutexInited) return; \
	AcquireResource( gpInvResource, TRUE );

#define InvUnLock ReleaseResource( gpInvResource );

#endif // _HOOK_NT_

VOID
GlobalInvThreadInit()
{
#ifdef _HOOK_NT_
	KeInitializeSpinLock( &gInvSpinLock );
	_impInitializeListHead(&gInvisibleProcList);
	ExInitializeNPagedLookasideList(gpNListInvThreads, NULL, NULL, 0, sizeof(INVISIBLELIST), 'ibos', 0);
#else
	gpInvResource = InitKlgResourse();
#endif // _HOOK_NT_
	bInvsMutexInited = TRUE;
}

VOID
GlobalInvThreadDone()
{
	InvisibleReleaseAll();
	bInvsMutexInited = FALSE;
#ifdef _HOOK_NT_
	ExDeleteNPagedLookasideList(gpNListInvThreads);
#else
	FreeKlgResourse(&gpInvResource);
#endif //_HOOK_NT_
}


//////////////////////////////////////////////////////////////////////////
#ifdef _HOOK_NT_

BOOLEAN InvProc_IsInvisible (
	HANDLE ProcessId
	)
{
	PINVISIBLEPROCITEM ProcItem;
	if (!IsListEmpty( &gInvisibleProcList ))
	{
		PINVISIBLEPROCITEM ProcItemTmp;
		PIMPLIST_ENTRY Flink;
		Flink = gInvisibleProcList.Flink;

		while (Flink != &gInvisibleProcList)
		{
			ProcItemTmp = CONTAINING_RECORD( Flink, INVISIBLEPROCITEM, m_List );
			Flink = Flink->Flink;

			if (ProcessId == ProcItemTmp->m_ProcessId)
				return TRUE;
		}
	}

	return FALSE;
}

VOID
InvProc_RemoveFromList (
	HANDLE ProcessId
	)
{
	InvLockVoid;
	
	if (!IsListEmpty( &gInvisibleProcList ))
	{
		PINVISIBLEPROCITEM ProcItem;
		PINVISIBLEPROCITEM ProcItemTmp;
		PIMPLIST_ENTRY Flink;
		Flink = gInvisibleProcList.Flink;

		while (Flink != &gInvisibleProcList)
		{
			ProcItemTmp = CONTAINING_RECORD( Flink, INVISIBLEPROCITEM, m_List );
			Flink = Flink->Flink;

			if (ProcessId == ProcItemTmp->m_ProcessId)
			{
				_impRemoveEntryList( &ProcItemTmp->m_List );
				ExFreePool( ProcItemTmp );

				break;
			}
		}
	}

	InvUnLock;
}

VOID
InvProc_AddNewImp (
	HANDLE ProcessId
	)
{
	PINVISIBLEPROCITEM ProcItem;
	ProcItem = ExAllocatePoolWithTag( NonPagedPool, sizeof(INVISIBLEPROCITEM), 'pbos' );
	if (ProcItem)
	{
		ProcItem->m_ProcessId = ProcessId;
		_impInsertTailList( &gInvisibleProcList, &ProcItem->m_List );

		DbPrint(DC_INVTH, DL_IMPORTANT, ("Process %x is invisible now!\n", ProcessId));
	}
}

VOID
InvProc_AddNew (
	HANDLE ProcessId
	)
{
	InvLockVoid;

	if (!InvProc_IsInvisible( ProcessId ))
		InvProc_AddNewImp( ProcessId );

	InvUnLock;
}


VOID
InvProc_CheckNewChild (
	HANDLE ParentProcessId,
	HANDLE ProcessId
	)
{
	PINVISIBLEPROCITEM ProcItem;

	InvLockVoid;

	if (!IsListEmpty( &gInvisibleProcList ))
	{
		PINVISIBLEPROCITEM ProcItemTmp;
		PIMPLIST_ENTRY Flink;
		Flink = gInvisibleProcList.Flink;

		while (Flink != &gInvisibleProcList)
		{
			ProcItemTmp = CONTAINING_RECORD( Flink, INVISIBLEPROCITEM, m_List );
			Flink = Flink->Flink;

			if (ParentProcessId == ProcItemTmp->m_ProcessId)
			{
				InvProc_AddNewImp( ProcessId );
				break;
			}
		}
	}

	InvUnLock;
}

#endif
//////////////////////////////////////////////////////////////////////////


BOOLEAN
IsInvisibleThread(ULONG ThreadID) 
{
	PINVISIBLELIST pInvQueue;
	
	InvLock(FALSE);

#ifdef _HOOK_NT_
	if (InvProc_IsInvisible(PsGetCurrentProcessId()))
	{
		InvUnLock;
		DbPrint(DC_INVTH, DL_NOTIFY, ("Process %x is invisible\n", PsGetCurrentProcessId()));
		return TRUE;
	}
#endif

	if (gHint_InvisibleThread == ThreadID)
	{
		InvUnLock;
		DbPrint(DC_INVTH, DL_SPAM, ("ThreadID %x is invisible\n", ThreadID));
		return TRUE;
	}
	else if (gHint_NonInvisibleThread == ThreadID)
	{
		InvUnLock;
		return FALSE;
	}
	
	pInvQueue = g_pInvQueue;
	while (pInvQueue != NULL)
	{
		if (pInvQueue->ThreadID == ThreadID)
		{
			InvUnLock;
			DbPrint(DC_INVTH, DL_SPAM, ("ThreadID %x is invisible\n", ThreadID));
			InterlockedExchange(&gHint_InvisibleThread, ThreadID);
			return TRUE;
		}
		pInvQueue = pInvQueue->Next;
	}

	InterlockedExchange(&gHint_NonInvisibleThread, ThreadID);

	InvUnLock;
	
	return FALSE;
}

NTSTATUS
AddInvisibleThread(ULONG ThreadID)
{
	PINVISIBLELIST pInvQueue;
	PINVISIBLELIST pInvQueuePrev;
	
	InvLock(STATUS_UNSUCCESSFUL);

	InterlockedExchange(&gHint_NonInvisibleThread, 0);
	
	pInvQueue = g_pInvQueue;
	pInvQueuePrev = NULL;
	while (pInvQueue != NULL)
	{
		if (pInvQueue->ThreadID == ThreadID)
		{
			
			DbPrint(DC_INVTH,DL_NOTIFY, ("ThreadID %x already setted (RefCount++)\n", ThreadID));			
			pInvQueue->RefCount++;
			
			InvUnLock;
			return STATUS_SUCCESS;
		}
		pInvQueuePrev = pInvQueue;
		pInvQueue = pInvQueue->Next;
	}
	
	DbPrint(DC_INVTH,DL_NOTIFY, ("ThreadID %x add\n", ThreadID));
	pInvQueue = AllocateInvisibleStruct;
	
	if (pInvQueue != NULL)
	{
		pInvQueue->ThreadID = ThreadID;
		pInvQueue->Next = g_pInvQueue;
		pInvQueue->RefCount = 1;
		
		g_pInvQueue = pInvQueue;
		
		InvUnLock;
	
		return STATUS_SUCCESS;
	}
	
	DbPrint(DC_INVTH,DL_ERROR, ("Invis not added for ThreadID %x\n", ThreadID));	
	
	InvUnLock;
	
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS
DelInvisibleThread(ULONG ThreadID, BOOLEAN bHardFlash)
{
	PINVISIBLELIST pInvQueue;
	PINVISIBLELIST pInvQueuePrev;
	
	InvLock(STATUS_UNSUCCESSFUL);

	InterlockedExchange(&gHint_InvisibleThread, 0);
	
	pInvQueue = g_pInvQueue;
	pInvQueuePrev = NULL;
	while (pInvQueue != NULL)
	{
		if (pInvQueue->ThreadID == ThreadID)
		{
			pInvQueue->RefCount--;
			if ((pInvQueue->RefCount == 0) || (bHardFlash == TRUE))
			{
				if (pInvQueuePrev == NULL)
					g_pInvQueue = pInvQueue->Next;
				else
					pInvQueuePrev->Next = pInvQueue->Next;
				
				FreeInvisibleStruct(pInvQueue);
				
				DbPrint(DC_INVTH,DL_NOTIFY, ("ThreadID %x del\n", ThreadID));
			}
			
			InvUnLock;
			return STATUS_SUCCESS;
		}
		pInvQueuePrev = pInvQueue;
		pInvQueue = pInvQueue->Next;
	}
	DbPrint(DC_INVTH,DL_INFO, ("ThreadID %x not found \n", ThreadID));
	
	InvUnLock;
	
	return STATUS_UNSUCCESSFUL;
}

ULONG
GetInvisibleThreadCounter(ULONG ThreadID)
{
	ULONG Counter = 0;
	PINVISIBLELIST pInvQueue;
	
	InvLock(0);

	InterlockedExchange(&gHint_InvisibleThread, 0);
	
	pInvQueue = g_pInvQueue;
	while (pInvQueue != NULL)
	{
		if (pInvQueue->ThreadID == ThreadID)
		{
			Counter = pInvQueue->RefCount;
			break;
		}

		pInvQueue = pInvQueue->Next;
	}
	
	InvUnLock;
	
	return Counter;

}

VOID
InvisibleReleaseAll(VOID)
{
	PINVISIBLELIST pInvQueue;
	PINVISIBLELIST Curr;
	ULONG Counter = 0;
	
	InvLockVoid;

	InterlockedExchange(&gHint_InvisibleThread, 0);
	
	pInvQueue = g_pInvQueue;
	Curr = pInvQueue;
	while(Curr)
	{
		DbPrint(DC_INVTH,DL_WARNING, ("InvQueue thread lost - %0x deleted\n", Curr->ThreadID));
		pInvQueue = Curr->Next;
		FreeInvisibleStruct(Curr);
		Curr = pInvQueue;
		Counter++;
	}
	g_pInvQueue = NULL;
	
	DbPrint(DC_INVTH, DL_NOTIFY, ("InvQueue - %d deleted\n", Counter));	
	
	InvUnLock;
	
	return;
}

NTSTATUS
InvisibleTransmit(PINVISIBLE_TRANSMIT pInvTransmit, PINVISIBLE_TRANSMIT pInvTransmitOut)
{
	NTSTATUS Status = STATUS_SUCCESS;

	if (INVCTL_GETCOUNTER == pInvTransmit->m_InvCtl)
	{
		if(pInvTransmitOut != NULL)
		{
			pInvTransmitOut->m_ThreadID = pInvTransmit->m_ThreadID;
			
			pInvTransmitOut->m_InvCtl = GetInvisibleThreadCounter(pInvTransmitOut->m_ThreadID);

			return STATUS_SUCCESS;
		}

		return STATUS_INVALID_PARAMETER;
	}
	
	if (IsExistClient(pInvTransmit->m_AppID))
	{
		switch(pInvTransmit->m_InvCtl)
		{
		case INVCTL_ADDBYID:
			if (IsInvisibleThread((ULONG)PsGetCurrentThreadId()))
			{
				AddInvisibleThread(pInvTransmit->m_ThreadID);
				if (pInvTransmitOut != NULL)
					pInvTransmitOut->m_ThreadID = pInvTransmit->m_ThreadID;
			}
			else
			{
				Status = STATUS_NOT_SUPPORTED;
			}
			break;

		case INVCTL_ADD:
			{
				ULONG ThreadID = (ULONG) PsGetCurrentThreadId();
				if (pInvTransmitOut != NULL)
					pInvTransmitOut->m_ThreadID = ThreadID;
				
				Status = AddInvisibleThread(ThreadID);
			}
			break;
		case INVCTL_DEL:
			DbPrint(DC_INVTH,DL_NOTIFY, ("IOCTLHOOK_InvisibleThreadOperations - del\n"));
			{
				ULONG ThreadID;
				ThreadID = (ULONG)PsGetCurrentThreadId();

				if (pInvTransmitOut != NULL) 
					pInvTransmitOut->m_ThreadID = ThreadID;
				
				Status = DelInvisibleThread(ThreadID, FALSE);
			}
			break;
		case INVCTL_RESET:
			InvisibleReleaseAll();
			break;
		case INVCTL_DELBYID:
			DbPrint(DC_INVTH,DL_NOTIFY, ("IOCTLHOOK_InvisibleThreadOperations - del by id\n"));
			{
				if (pInvTransmit == NULL) 
				{
					Status = STATUS_INVALID_PARAMETER;
					break;
				}
				
				pInvTransmitOut->m_ThreadID = pInvTransmit->m_ThreadID;
				Status = DelInvisibleThread(pInvTransmit->m_ThreadID, FALSE);
			}
			break;
#ifdef _HOOK_NT_
		case INVCTL_PROCADD:
			InvProc_AddNew( PsGetCurrentProcessId( ) );
			if (pInvTransmit->m_ThreadID)
				InvProc_AddNew((HANDLE) pInvTransmit->m_ThreadID );
			break;
#endif
		default:
			Status = STATUS_NOT_FOUND;
			break;
		}
	}
	else
	{
		DbPrint(DC_INVTH,DL_ERROR, ("IOCTLHOOK_InvisibleThreadOperations - Client not found(%d)\n", 
			pInvTransmit->m_AppID));
		Status = STATUS_NOT_FOUND;
	}
	return Status;
}


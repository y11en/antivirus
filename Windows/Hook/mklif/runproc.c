#include "runproc.h"
#include "../Hook/HookSpec.h"
#include "lists.h"
#include "eresource.h"
//+ ------------------------------------------------------------------------------------------

typedef struct _PROC_LISTITEM
{
    IMPLIST_ENTRY	m_List;
    HANDLE			m_ProcessId;
	ULONG			m_RefCount;
	BOOLEAN			m_bSuspend;
	BOOLEAN			m_bAllowExecuting;
	BOOLEAN			m_bSkipThis;
	KEVENT			m_HoldEvent;
} PROC_LISTITEM, *PPROC_LISTITEM;

IMPLIST_ENTRY	gProcList;
ERESOURCE		gProcListResource;

void
GlobalProcCheckInit()
{
//	DbPrint(DC_SYS, DL_IMPORTANT, ("GlobalProcCheckInit\n"));
	
	_impInitializeListHead(&gProcList);
	ExInitializeResourceLite(&gProcListResource);
}

void
GlobalProcCheckDone()
{
//	DbPrint(DC_SYS, DL_IMPORTANT, ("GlobalProcCheckDone\n"));
	//todo: free list
	ExDeleteResourceLite(&gProcListResource);
}

PPROC_LISTITEM CheckProc_AcquireProcess(HANDLE ProcessId)
{
	if (!_impIsListEmpty(&gProcList))
	{
		PPROC_LISTITEM ProcItemTmp;
		PIMPLIST_ENTRY Flink;
		Flink = gProcList.Flink;

		while(Flink != &gProcList)
		{
			ProcItemTmp = CONTAINING_RECORD(Flink, PROC_LISTITEM, m_List);
			Flink = Flink->Flink;

			if (ProcItemTmp->m_bSkipThis)
			{
				if (!ProcItemTmp->m_RefCount)
				{
					// delete this process from list
					_impRemoveEntryList(&ProcItemTmp->m_List);

					ExFreePool(ProcItemTmp);
				}
			}
			else if (ProcItemTmp->m_ProcessId == ProcessId)
				return ProcItemTmp;
		}
	}

	return NULL;
}

BOOLEAN CheckProc_IsAllowProcessExecuting()
{
	PPROC_LISTITEM ProcItem;

	BOOLEAN bAllow = TRUE;
	HANDLE CurrentProcessId = PsGetCurrentProcessId();
	
	AcquireResourceExclusive(&gProcListResource);

	ProcItem = CheckProc_AcquireProcess(CurrentProcessId);

	if (!ProcItem)
	{
		ReleaseResource(&gProcListResource);
	}
	else
	{
		if (ProcItem->m_bSuspend)
		{
			ProcItem->m_RefCount++;

//			DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %d waiting\n", CurrentProcessId));	

			ReleaseResource(&gProcListResource);
			KeWaitForSingleObject(&ProcItem->m_HoldEvent, Suspended, KernelMode, FALSE, NULL);
			AcquireResourceExclusive(&gProcListResource);

//			DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %d resumed\n", CurrentProcessId));	
			ProcItem->m_RefCount--;
		}

		bAllow = ProcItem->m_bAllowExecuting;
		if (!bAllow)
		{
//			DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %p disabled\n", CurrentProcessId));	
		}

		ReleaseResource(&gProcListResource);
	}
	
	return bAllow;
}

void
CheckProc_RemoveFromList(HANDLE ProcessID)
{
	PPROC_LISTITEM ProcItem;
	AcquireResourceExclusive(&gProcListResource);

	ProcItem = CheckProc_AcquireProcess(ProcessID);
	if (ProcItem)
	{
		ProcItem->m_bSkipThis = TRUE;

//		DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %d removed\n", ProcessID));	
	}

	ReleaseResource(&gProcListResource);
}


NTSTATUS
CheckProc_ProcessFunc(HANDLE ProcessID, ULONG Request)
{
	PPROC_LISTITEM ProcItem;

//	DbPrint(DC_SYS, DL_IMPORTANT, ("Request 0x%x on process %p\n", Request, ProcessID));

	switch (Request)
	{
	case _AVPG_IOCTL_PROCESS_SUSPEND:
		{
			AcquireResourceExclusive(&gProcListResource);
			ProcItem = CheckProc_AcquireProcess(ProcessID);

			if (ProcItem)
			{
				ProcItem->m_bAllowExecuting = FALSE;
				ProcItem->m_bSuspend = TRUE;
				KeClearEvent(&ProcItem->m_HoldEvent);
			}
			else
			{
				ProcItem = ExAllocatePoolWithTag(NonPagedPool, sizeof(PROC_LISTITEM), 'Pbos');
				if (ProcItem)
				{
					ProcItem->m_bSkipThis = FALSE;
					ProcItem->m_ProcessId = ProcessID;
					ProcItem->m_bSuspend = TRUE;
					ProcItem->m_bAllowExecuting = FALSE;
					ProcItem->m_RefCount = 0;
					KeInitializeEvent(&ProcItem->m_HoldEvent, NotificationEvent, FALSE);

					_impInsertTailList(&gProcList, &ProcItem->m_List);
				}
			}

			ReleaseResource(&gProcListResource);
		}
		break;
	case _AVPG_IOCTL_PROCESS_RESUME:
		{
			AcquireResourceExclusive(&gProcListResource);
			ProcItem = CheckProc_AcquireProcess(ProcessID);

			if (ProcItem)
			{
				ProcItem->m_bAllowExecuting = TRUE;
				ProcItem->m_bSuspend = FALSE;
				KeSetEvent(&ProcItem->m_HoldEvent, 0, FALSE);
			}

			ReleaseResource(&gProcListResource);
		}
		break;
	case _AVPG_IOCTL_PROCESS_DISCARDOPS:
			AcquireResourceExclusive(&gProcListResource);
			ProcItem = CheckProc_AcquireProcess(ProcessID);

			if (ProcItem)
			{
				ProcItem->m_bAllowExecuting = FALSE;
				ProcItem->m_bSuspend = FALSE;
				KeSetEvent(&ProcItem->m_HoldEvent, 0, FALSE);
			}
			ReleaseResource(&gProcListResource);
		break;
	default:
		{
//			DbPrint(DC_SYS, DL_WARNING, ("CheckProc: unknown request %d\n", Request));	
			_dbg_break_;
		}
		break;
	}

	return STATUS_SUCCESS;
}

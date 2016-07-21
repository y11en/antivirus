#include "runproc.h"
#include "../Hook/HookSpec.h"

#ifndef _HOOK_NT_

BOOLEAN IsAllowProcessExecuting()
{
	return TRUE;
}

void
GlobalProcCheckInit()
{
}

void
GlobalProcCheckDone()
{
}

NTSTATUS
ProcessFunc(ULONG ProcessID, ULONG Request)
{
	return STATUS_UNSUCCESSFUL;
}

BOOLEAN CheckProc_IsAllowProcessExecuting()
{
	return TRUE;
}

#else

#include "../lists.h"
//+ ------------------------------------------------------------------------------------------

typedef struct _PROC_LISTITEM
{
    IMPLIST_ENTRY	m_List;
    ULONG			m_ProcessId;
	ULONG			m_RefCount;
	BOOLEAN			m_bSuspend;
	BOOLEAN			m_bAllowExecuting;
	BOOLEAN			m_bRequestDelete;
	KEVENT			m_HoldEvent;
} PROC_LISTITEM, *PPROC_LISTITEM;

IMPLIST_ENTRY	gProcList;
ERESOURCE		gProcListResource;

void
GlobalProcCheckInit()
{
	_impInitializeListHead(&gProcList);
	ExInitializeResourceLite(&gProcListResource);
}

void
GlobalProcCheckDone()
{
	DbgBreakPoint();
	ExDeleteResourceLite(&gProcListResource);
}

PPROC_LISTITEM CheckProc_AcquireProcess(ULONG ProcessId)
{
	if (!IsListEmpty(&gProcList))
	{
		PPROC_LISTITEM ProcItemTmp;
		PIMPLIST_ENTRY Flink;
		Flink = gProcList.Flink;

		while(Flink != &gProcList)
		{
			ProcItemTmp = (PPROC_LISTITEM) Flink;
			Flink = Flink->Flink;

			if (ProcItemTmp->m_bRequestDelete)
			{
				if (!ProcItemTmp->m_RefCount)
				{
					// delete this process from list
					_impRemoveEntryList(&ProcItemTmp->m_List);

					ExFreePool(ProcItemTmp);
					continue;
				}
			}
			
			if (ProcItemTmp->m_ProcessId == ProcessId)
				return ProcItemTmp;
		}
	}

	return NULL;
}

BOOLEAN CheckProc_IsAllowProcessExecuting()
{
	PPROC_LISTITEM ProcItem;

	BOOLEAN bAllow = TRUE;
	ULONG CurrentProcessId = (ULONG) PsGetCurrentProcessId();
	
	_RESOURCE_LOCK_W(&gProcListResource);

	ProcItem = CheckProc_AcquireProcess(CurrentProcessId);

	if (!ProcItem)
	{
		_RESOURCE_UNLOCK(&gProcListResource);
	}
	else
	{
		if (ProcItem->m_bSuspend)
		{
			ProcItem->m_RefCount++;

			DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %d waiting\n", CurrentProcessId));	

			_RESOURCE_UNLOCK(&gProcListResource);
			KeWaitForSingleObject(&ProcItem->m_HoldEvent, Suspended, KernelMode, FALSE, NULL);
			_RESOURCE_LOCK_W(&gProcListResource);

			DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %d resumed\n", CurrentProcessId));	
			ProcItem->m_RefCount--;
		}

		bAllow = ProcItem->m_bAllowExecuting;
		if (!bAllow)
		{
			DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %d disabled\n", CurrentProcessId));	
		}

		_RESOURCE_UNLOCK(&gProcListResource);
	}
	
	return bAllow;
}

void
CheckProc_RemoveFromList(ULONG ProcessID)
{
	PPROC_LISTITEM ProcItem;
	_RESOURCE_LOCK_W(&gProcListResource);

	ProcItem = CheckProc_AcquireProcess(ProcessID);
	if (ProcItem)
	{
		ProcItem->m_bRequestDelete = TRUE;

		DbPrint(DC_SYS, DL_IMPORTANT, ("CheckProc: process %d removed\n", ProcessID));	
	}

	_RESOURCE_UNLOCK(&gProcListResource);
}


NTSTATUS
CheckProc_ProcessFunc(ULONG ProcessID, ULONG Request)
{
	PPROC_LISTITEM ProcItem;

	DbPrint(DC_SYS, DL_IMPORTANT, ("Request %#x on process %d (%#x)\n", Request, ProcessID, ProcessID));

	switch (Request)
	{
	case _AVPG_IOCTL_PROCESS_SUSPEND:
		{
			_RESOURCE_LOCK_W(&gProcListResource);
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
					ProcItem->m_bRequestDelete = FALSE;
					ProcItem->m_ProcessId = ProcessID;
					ProcItem->m_bSuspend = TRUE;
					ProcItem->m_bAllowExecuting = FALSE;
					ProcItem->m_RefCount = 0;
					KeInitializeEvent(&ProcItem->m_HoldEvent, NotificationEvent, FALSE);

					_impInsertTailList(&gProcList, &ProcItem->m_List);
				}
			}

			_RESOURCE_UNLOCK(&gProcListResource);
		}
		break;
	case _AVPG_IOCTL_PROCESS_RESUME:
		{
			_RESOURCE_LOCK_W(&gProcListResource);
			ProcItem = CheckProc_AcquireProcess(ProcessID);

			if (ProcItem)
			{
				ProcItem->m_bAllowExecuting = TRUE;
				ProcItem->m_bSuspend = FALSE;
				KeSetEvent(&ProcItem->m_HoldEvent, 0, FALSE);
			}

			_RESOURCE_UNLOCK(&gProcListResource);
		}
		break;
	case _AVPG_IOCTL_PROCESS_DISCARDOPS:
			_RESOURCE_LOCK_W(&gProcListResource);
			ProcItem = CheckProc_AcquireProcess(ProcessID);

			if (ProcItem)
			{
				ProcItem->m_bAllowExecuting = FALSE;
				ProcItem->m_bSuspend = FALSE;
				KeSetEvent(&ProcItem->m_HoldEvent, 0, FALSE);
			}
			_RESOURCE_UNLOCK(&gProcListResource);
		break;
	default:
		{
			DbPrint(DC_SYS, DL_WARNING, ("CheckProc: unknown request %d\n", Request));	
			DbgBreakPoint();
		}
		break;
	}

	return STATUS_SUCCESS;
}

#endif // _HOOK_NT_



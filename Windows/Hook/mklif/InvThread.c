#include "InvThread.h"
#include "eresource.h"
#include "client.h"

#include "invthread.tmh"

typedef struct _INVISIBLEITEM
 {
	LIST_ENTRY				m_List;
	HANDLE					m_Object;
	ULONG					m_RefCount;
	BOOLEAN					m_bRecursive;
} INVISIBLEITEM, *PINVISIBLEITEM;

KSPIN_LOCK gInvSpinLock;

NPAGED_LOOKASIDE_LIST gNListInv;
NPAGED_LOOKASIDE_LIST* gpNListInv = &gNListInv;
#define _inv_thread_size 1024
LONG gInvThreads[_inv_thread_size];

#define _inv_proc_size 16
typedef struct _INV_PROC_ITEM
{
	HANDLE	m_hProcess;
	LONG	m_RefCount;
	BOOLEAN m_bRecursive;
}INV_PROC_ITEM, *PINV_PROC_ITEM;

INV_PROC_ITEM gInvProcesses[_inv_proc_size];
LONG gIvProcCount = 0;

#if defined(_WIN64)
	__inline
	ULONG _thread_idx(HANDLE _thread)
	{
		if (_thread > (HANDLE) _inv_thread_size)
			return -1;

		return HandleToUlong(_thread)/4;
	}
#else
	#define _thread_idx(_thread) HandleToUlong(_thread)/4
#endif

#ifdef WIN2K
	#define AllocateInvisibleStruct ExAllocatePoolWithTag( NonPagedPool, sizeof(INVISIBLEITEM), 'pBOS' )
	#define FreeInvisibleStruct( _p ) ExFreePool( _p )
#else
	#define AllocateInvisibleStruct ExAllocateFromNPagedLookasideList( gpNListInv );
	#define FreeInvisibleStruct( _p ) ExFreeToNPagedLookasideList(gpNListInv, _p );
#endif // WIN2K

LIST_ENTRY	gInvisibleThreadList;
LIST_ENTRY	gInvisibleProcList;

BOOLEAN
GlobalInvThreadInit()
{
	DoTraceEx( TRACE_LEVEL_WARNING, DC_INVISIBLE, "GlobalInvThreadInit" );

	KeInitializeSpinLock( &gInvSpinLock );
	ExInitializeNPagedLookasideList( gpNListInv, NULL, NULL, 0, sizeof(INVISIBLEITEM), 'ibos', 0 );
	RtlZeroMemory( &gInvThreads, sizeof(gInvThreads) );
	RtlZeroMemory( &gInvProcesses, sizeof(gInvProcesses) );
	
	InitializeListHead( &gInvisibleThreadList );
	InitializeListHead( &gInvisibleProcList );
	
	return TRUE;
}

VOID
GlobalInvThreadDone()
{
	DoTraceEx( TRACE_LEVEL_WARNING, DC_INVISIBLE, "GlobalInvThreadDone..." );

	InvisibleReleaseAll();
	
	ExDeleteNPagedLookasideList( gpNListInv );
	gpNListInv = 0;

	DoTraceEx( TRACE_LEVEL_WARNING, DC_INVISIBLE, "GlobalInvThreadDone" );
}

BOOLEAN
IsInvisible (
	HANDLE ThreadID,
	HANDLE hProcess
	) 
{
	BOOLEAN bInvisible = FALSE;
	KIRQL NewIrql;
	ULONG thid = _thread_idx( ThreadID );
	int cou;
	BOOLEAN bRecursive = FALSE;

	if (thid < _inv_thread_size)
	{
		if (gInvThreads[thid])
			return TRUE;

		if (!gIvProcCount)
			return FALSE;
	}

	if (!hProcess)
		hProcess = PsGetCurrentProcessId();
	if (!hProcess || hProcess == UlongToHandle(4) || hProcess == UlongToHandle(8))
		return FALSE;

	for (cou = 0; cou < RTL_NUMBER_OF(gInvProcesses); cou++)
	{
		if (hProcess == gInvProcesses[cou].m_hProcess)
		{
			if (gInvProcesses[cou].m_RefCount)
				return TRUE;

			return FALSE;
		}
	}

	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	if ( InvProc_IsInvisible( hProcess, &bRecursive ))
		bInvisible = TRUE;

	if (!bInvisible)
	{
		if (!IsListEmpty( &gInvisibleThreadList ))
		{
			PINVISIBLEITEM pItem = NULL;
			PLIST_ENTRY Flink = gInvisibleThreadList.Flink;

			while (Flink != &gInvisibleThreadList)
			{
				pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );

				if (pItem->m_Object == ThreadID)
				{
					bInvisible = TRUE;
					break;
				}
				
				Flink = Flink->Flink;
			}
		}
	}

	KeReleaseSpinLock( &gInvSpinLock, NewIrql );

	return bInvisible;
}

NTSTATUS
AddInvisibleThread(HANDLE ThreadID)
{
	PINVISIBLEITEM pItem = NULL;
	KIRQL NewIrql;
	ULONG thid = _thread_idx( ThreadID );

	if (thid < _inv_thread_size)
	{
		InterlockedIncrement( &gInvThreads[thid] );

		return STATUS_SUCCESS;
	}
	
	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	if (!IsListEmpty( &gInvisibleThreadList ))
	{
		PLIST_ENTRY Flink = gInvisibleThreadList.Flink;

		while (Flink != &gInvisibleThreadList)
		{
			pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );

			if (pItem->m_Object == ThreadID)
			{
				pItem->m_RefCount++;

				KeReleaseSpinLock( &gInvSpinLock, NewIrql );
				
				return STATUS_SUCCESS;
			}
			
			Flink = Flink->Flink;
		}
	}
	
	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_INVISIBLE, "ThreadID %p add\n", ThreadID );

	pItem = AllocateInvisibleStruct;
	
	if (pItem != NULL)
	{
		pItem->m_Object = ThreadID;
		pItem->m_RefCount = 1;
		
		InsertTailList( &gInvisibleThreadList, &pItem->m_List );		
		
		KeReleaseSpinLock( &gInvSpinLock, NewIrql );
	
		return STATUS_SUCCESS;
	}
	
	KeReleaseSpinLock( &gInvSpinLock, NewIrql );
	
	return STATUS_UNSUCCESSFUL;
}

ULONG
GetInvisibleThreadCounter (
	HANDLE ThreadID
	)
{
	PINVISIBLEITEM pItem = NULL;
	ULONG thid = _thread_idx( ThreadID );
	KIRQL NewIrql;

	if (thid < _inv_thread_size)
		return (ULONG) gInvThreads[thid];
	
	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	if (!IsListEmpty( &gInvisibleThreadList ))
	{
		PLIST_ENTRY Flink = gInvisibleThreadList.Flink;

		while (Flink != &gInvisibleThreadList)
		{
			pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );

			if (pItem->m_Object == ThreadID)
			{
				ULONG Counter = pItem->m_RefCount;

				KeReleaseSpinLock( &gInvSpinLock, NewIrql );
				
				return Counter;
			}
			
			Flink = Flink->Flink;
		}
	}

	KeReleaseSpinLock( &gInvSpinLock, NewIrql );

	return 0;
}

NTSTATUS
DelInvisibleThread(HANDLE ThreadID, BOOLEAN bHardFlash)
{
	//DoTraceEx( TRACE_LEVEL_WARNING, DC_INVISIBLE, "DelInvisibleThread %p", ThreadID );
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	KIRQL NewIrql;
	ULONG thid = _thread_idx( ThreadID );

	if (thid < _inv_thread_size)
	{
		if (bHardFlash)
			InterlockedExchange( &gInvThreads[thid], 0 );
		else
		{
			LONG newinv = InterlockedDecrement( &gInvThreads[thid] );
			if (newinv < 0)
			{
				_dbg_break_;
				InterlockedExchange( &gInvThreads[thid], 0 );
			}
		}

		return STATUS_SUCCESS;
	}
	
	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	if (!IsListEmpty( &gInvisibleThreadList ))
	{
		PINVISIBLEITEM pItem = NULL;
		PLIST_ENTRY Flink = gInvisibleThreadList.Flink;

		while (Flink != &gInvisibleThreadList)
		{
			pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );
			Flink = Flink->Flink;

			if (pItem->m_Object == ThreadID)
			{
				pItem->m_RefCount--;
				if ((pItem->m_RefCount == 0) || (bHardFlash))
				{
					RemoveEntryList( &pItem->m_List );
					FreeInvisibleStruct( pItem );
				}

				status = STATUS_SUCCESS;
				break;
			}
		}
	}

	KeReleaseSpinLock( &gInvSpinLock, NewIrql );
	
	return status;
}

VOID
InvisibleReleaseAll()
{
	ULONG Counter = 0;
	KIRQL NewIrql;

	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	if (!IsListEmpty( &gInvisibleThreadList ))
	{
		PINVISIBLEITEM pItem = NULL;
		PLIST_ENTRY Flink = gInvisibleThreadList.Flink;

		while (Flink != &gInvisibleThreadList)
		{
			pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );
			Flink = Flink->Flink;

			RemoveEntryList( &pItem->m_List );
			FreeInvisibleStruct( pItem );
		}
	}


	if (!IsListEmpty( &gInvisibleProcList ))
	{
		PINVISIBLEITEM pItem = NULL;
		PLIST_ENTRY Flink = gInvisibleProcList.Flink;

		while (Flink != &gInvisibleProcList)
		{
			pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );
			Flink = Flink->Flink;

			RemoveEntryList( &pItem->m_List );
			FreeInvisibleStruct( pItem );
		}
	}
	
	KeReleaseSpinLock( &gInvSpinLock, NewIrql );
	
	return;
}

NTSTATUS
InvisibleTransmit(PINVISIBLE_TRANSMIT pInvTransmit, PINVISIBLE_TRANSMIT pInvTransmitOut)
{
	NTSTATUS Status = STATUS_NOT_FOUND;

	if (INVCTL_GETCOUNTER == pInvTransmit->m_InvCtl)
	{
		if(pInvTransmitOut != NULL)
		{
			pInvTransmitOut->m_ThreadID = pInvTransmit->m_ThreadID;
			
			pInvTransmitOut->m_InvCtl = GetInvisibleThreadCounter(UlongToHandle( pInvTransmitOut->m_ThreadID ));

			return STATUS_SUCCESS;
		}

		return STATUS_INVALID_PARAMETER;
	}

	
	if (IsExistClient(pInvTransmit->m_AppID))
	{
		switch(pInvTransmit->m_InvCtl)
		{
		case INVCTL_ADD:
			{
				HANDLE hThread = PsGetCurrentThreadId();
				ULONG ThreadID = PtrToUlong(hThread);
				if (pInvTransmitOut != NULL)
					pInvTransmitOut->m_ThreadID = ThreadID;
				
				Status = AddInvisibleThread(hThread);
			}
			break;
		case INVCTL_DEL:
//			DbPrint(DC_INVTH,DL_NOTIFY, ("IOCTLHOOK_InvisibleThreadOperations - del\n"));
			{
				ULONG ThreadID;
				HANDLE hThread = PsGetCurrentThreadId();
				ThreadID = PtrToUlong(hThread);

				if (pInvTransmitOut != NULL) 
					pInvTransmitOut->m_ThreadID = ThreadID;
				
				Status = DelInvisibleThread(hThread, FALSE);
			}
			break;
		case INVCTL_RESET:
			InvisibleReleaseAll();
			break;
		case INVCTL_DELBYID:
//			DbPrint(DC_INVTH,DL_NOTIFY, ("IOCTLHOOK_InvisibleThreadOperations - del by id\n"));
			{
				if (pInvTransmit == NULL) 
				{
					Status = STATUS_INVALID_PARAMETER;
					break;
				}
				
				pInvTransmitOut->m_ThreadID = pInvTransmit->m_ThreadID;
				{
					HANDLE hThread = 0;
					*(ULONG*)&hThread = pInvTransmit->m_ThreadID;
					Status = DelInvisibleThread(hThread, FALSE);
				}
			}
			break;
		}
	}
	else
	{
//		DbPrint(DC_INVTH,DL_ERROR, ("IOCTLHOOK_InvisibleThreadOperations - Client not found(%d)\n", 
//			pInvTransmit->m_AppID));
		Status = STATUS_NOT_FOUND;
		_dbg_break_;
	}
	return Status;
}

// + invisible processes

BOOLEAN InvProc_IsInvisible (
	HANDLE ProcessId,
	BOOLEAN *pbRecursive
	)
{
	int cou;
	for (cou = 0; cou < RTL_NUMBER_OF(gInvProcesses); cou++)
	{
		if (ProcessId == gInvProcesses[cou].m_hProcess)
		{
			if (gInvProcesses[cou].m_RefCount)
			{
				*pbRecursive = gInvProcesses[cou].m_bRecursive;
				return TRUE;
			}
		}
	}

	if (!IsListEmpty( &gInvisibleProcList ))
	{
		PINVISIBLEITEM pItem = NULL;
		PLIST_ENTRY Flink = gInvisibleProcList.Flink;

		while (Flink != &gInvisibleProcList)
		{
			pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );
			Flink = Flink->Flink;

			if (ProcessId == pItem->m_Object)
			{
				*pbRecursive = pItem->m_bRecursive;
				return TRUE;
			}
		}
	}

	return FALSE;
}

VOID
InvProc_RemoveFromList (
	HANDLE ProcessId
	)
{
	KIRQL NewIrql;
	int cou = 0;

	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	for (cou = 0; cou < RTL_NUMBER_OF(gInvProcesses); cou++)
	{
		if (ProcessId == gInvProcesses[cou].m_hProcess)
		{
			InterlockedDecrement( &gIvProcCount );
			gInvProcesses[cou].m_hProcess = NULL;
			gInvProcesses[cou].m_RefCount = 0;
			gInvProcesses[cou].m_bRecursive = 0;
		}
	}

	if (!IsListEmpty( &gInvisibleProcList ))
	{
		PINVISIBLEITEM pItem = NULL;
		PLIST_ENTRY Flink = gInvisibleProcList.Flink;

		while (Flink != &gInvisibleProcList)
		{
			pItem = CONTAINING_RECORD( Flink, INVISIBLEITEM, m_List );
			Flink = Flink->Flink;

			if (ProcessId == pItem->m_Object)
			{
				InterlockedDecrement( &gIvProcCount );
				RemoveEntryList( &pItem->m_List );
				FreeInvisibleStruct( pItem );

				break;
			}
		}
	}

	KeReleaseSpinLock( &gInvSpinLock, NewIrql );
}

VOID
InvProc_AddNewImp (
	HANDLE ProcessId,
	BOOLEAN bRecursive
	)
{
	PINVISIBLEITEM pItem = NULL;
	int cou;
	for (cou = 0; cou < RTL_NUMBER_OF(gInvProcesses); cou++)
	{
		if (!gInvProcesses[cou].m_hProcess)
		{
			InterlockedIncrement( &gIvProcCount );
			gInvProcesses[cou].m_hProcess = ProcessId;
			gInvProcesses[cou].m_RefCount = 1;
			gInvProcesses[cou].m_bRecursive = bRecursive;

			return;
		}
	}
	
	pItem = AllocateInvisibleStruct;
	if (pItem)
	{
		pItem->m_Object = ProcessId;
		pItem->m_RefCount = 1;
		pItem->m_bRecursive = bRecursive;

		InsertTailList( &gInvisibleProcList, &pItem->m_List );
		InterlockedIncrement( &gIvProcCount );
	}
}

VOID
InvProc_AddNew (
	HANDLE ProcessId,
	BOOLEAN bRecursive
	)
{
	KIRQL NewIrql;
	BOOLEAN bRecTmp;
	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	if (!InvProc_IsInvisible( ProcessId, &bRecTmp ))
		InvProc_AddNewImp( ProcessId, bRecursive );

	KeReleaseSpinLock( &gInvSpinLock, NewIrql );
}

VOID
InvProc_CheckNewChild (
	HANDLE ParentProcessId,
	HANDLE ProcessId
	)
{
	KIRQL NewIrql;
	BOOLEAN bRecursive = FALSE;
	KeAcquireSpinLock( &gInvSpinLock, &NewIrql );

	if (InvProc_IsInvisible( ParentProcessId, &bRecursive ))
	{
		if (bRecursive)
			InvProc_AddNewImp( ProcessId, bRecursive );
	}

	KeReleaseSpinLock( &gInvSpinLock, NewIrql );
}

NTSTATUS
AddInvisibleProcess (
	HANDLE hProcess,
	BOOLEAN bRecursive
	)
{
	InvProc_AddNew( hProcess, bRecursive );

	if (bRecursive)
	{
		HANDLE hParent = GetParent( hProcess );
		if (hParent)
			InvProc_AddNew( hParent, bRecursive );
	}

	return STATUS_SUCCESS;
}

BOOLEAN
InvThread_IsInvisible (
	HANDLE ThreadId
	)
{
	ULONG invc = GetInvisibleThreadCounter( ThreadId );
	if (invc)
		return TRUE;

	return FALSE;
}

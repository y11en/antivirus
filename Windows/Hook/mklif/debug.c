#include "debug.h"

typedef struct _sTimingItem {
	LIST_ENTRY		m_List;
	ULONG			m_HookId;
	ULONG			m_Mj;
	PWCHAR			m_Name;
	LARGE_INTEGER	m_TimePre;
	LARGE_INTEGER	m_CallCountPre;
	LARGE_INTEGER	m_TimePost;
	LARGE_INTEGER	m_CallCountPost;
}TimingItem, *PTimingItem;


#if DBG == 0

	#pragma message ("----------------------- Release detected - DbgBreakPoint, HOOKKdPrint are disabled")

#else

	#pragma message ("----------------------- Debug detected - DbgBreakPoint, HOOKKdPrint are enabled")
	

#endif DBG

void
Timing_Register (
	ULONG HookId,
	ULONG Mj,
	PWCHAR pwchName
	)
{
	PTimingItem pItem = NULL;

	pItem = ExAllocatePoolWithTag( NonPagedPool, sizeof(TimingItem), tag_timing );
	if (!pItem)
		return;

	RtlZeroMemory( pItem, sizeof(TimingItem) );
	pItem->m_HookId = HookId;
	pItem->m_Mj = Mj;
	pItem->m_Name = pwchName;

	InsertHeadList( &Globals.m_Timing, &pItem->m_List );
}

void
Timing_Init (
	)
{
	Timing_Register( _timing_file, IRP_MJ_CREATE, L"Create" );
	Timing_Register( _timing_file, IRP_MJ_CLEANUP, L"Cleanup" );
	Timing_Register( _timing_file, IRP_MJ_READ , L"Read" );
	Timing_Register( _timing_file, IRP_MJ_WRITE, L"Write" );
	Timing_Register( _timing_file, IRP_MJ_SET_INFORMATION, L"SetInformation" );
	Timing_Register( _timing_file, IRP_MJ_FILE_SYSTEM_CONTROL, L"FileSystemControl" );
	Timing_Register( _timing_file, IRP_MJ_QUERY_INFORMATION, L"QueryInformation" );
	Timing_Register( _timing_file, IRP_MJ_QUERY_EA, L"QuearyEa" );
	Timing_Register( _timing_file, IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION, L"ExecuteSection" );
	Timing_Register( _timing_file, IRP_MJ_NETWORK_QUERY_OPEN, L"NetworkQueryOpen" );
	Timing_Register( _timing_comm, 0, L"Comm_Sendin" );
	Timing_Register( _timing_comm, 1, L"Comm_SendOut" );
	Timing_Register( _timing_filter, 0, L"FilterEvent" );
	Timing_Register( _timing_filter, 1, L"BuildName" );
	Timing_Register( _timing_filter, 2, L"CommRequest" );
	Timing_Register( _timing_filter, 3, L"ObjectRequest" );
}

void
Timing_Clear (
	)
{
	PTimingItem pItem = NULL;
	PLIST_ENTRY Flink = NULL;
	
	if (IsListEmpty( &Globals.m_Timing ))
		return;

	Flink = Globals.m_Timing.Flink;
	while ( Flink != &Globals.m_Timing )
	{
		pItem = CONTAINING_RECORD( Flink, TimingItem, m_List );

		Flink = Flink->Flink;
		RemoveEntryList( Flink->Blink );

		ExFreePool( pItem );
	}
}

void
Timing_OutImp (
	ULONG HookId,
	ULONG Mj,
	ULONG ProcessingType,
	PLARGE_INTEGER ptime
	)
{
	PTimingItem pItem = NULL;
	PLIST_ENTRY Flink = NULL;
	LARGE_INTEGER curtime;

	if (!FlagOn( Globals.m_DrvFlags, _DRV_FLAG_TIMING))
		return;

	if(!ptime->QuadPart)
		return;

	KeQuerySystemTime( &curtime );

	curtime.QuadPart -= ptime->QuadPart;
	
	if (!curtime.QuadPart)
		return;

	if (!IsListEmpty( &Globals.m_Timing ))
	{
		Flink = Globals.m_Timing.Flink;
		while ( Flink != &Globals.m_Timing )
		{
			pItem = CONTAINING_RECORD( Flink, TimingItem, m_List );
			Flink = Flink->Flink;

			if (pItem->m_HookId == HookId && pItem->m_Mj == Mj)
			{
				AcquireResourceExclusive( &Globals.m_TimingLock );
				if (ProcessingType == 0)	// pre
				{
					pItem->m_TimePre.QuadPart += curtime.QuadPart;
					pItem->m_CallCountPre.QuadPart += 1;
				}
				else if (ProcessingType == 1)	// post
				{
					pItem->m_TimePost.QuadPart += curtime.QuadPart;
					pItem->m_CallCountPost.QuadPart += 1;
				}
				else
				{
					_dbg_break_;
				}
				ReleaseResource( &Globals.m_TimingLock );

				break;
			}
		}
	}
}

ULONG
Timing_Place (
	ULONG Pos,
	PVOID pBuffer,
	PWCHAR pwchName,
	ULONG HookId,
	ULONG Mj,
	ULONG ProcessingType,
	LARGE_INTEGER CallCount,
	LARGE_INTEGER Time
	)
{
	ULONG namelen = (wcslen( pwchName ) + 1 ) * sizeof(WCHAR);
	ULONG Size = namelen;

	//		HookId			Mj				ProcType		CallCount				Time
	Size += sizeof(ULONG) + sizeof(ULONG) + sizeof(ULONG) + sizeof(LARGE_INTEGER) + sizeof(LARGE_INTEGER);

	if (pBuffer)
	{
		__int8* ptr = pBuffer;
		ptr += Pos;

		memcpy( ptr, pwchName, namelen );
		ptr += namelen;

		memcpy( ptr, &HookId, sizeof(ULONG) );
		ptr += sizeof(ULONG);

		memcpy( ptr, &Mj, sizeof(ULONG) );
		ptr += sizeof(ULONG);

		memcpy( ptr, &ProcessingType, sizeof(ULONG) );
		ptr += sizeof(ULONG);

		memcpy( ptr, &CallCount, sizeof(LARGE_INTEGER) );
		ptr += sizeof(LARGE_INTEGER);

		memcpy( ptr, &Time, sizeof(LARGE_INTEGER) );
		ptr += sizeof(LARGE_INTEGER);
	}

	return Size;
}

NTSTATUS
Timing_Get (
	PVOID* ppBuffer,
	PULONG pBufferSize
	)
{
	PTimingItem pItem = NULL;
	PLIST_ENTRY Flink = NULL;
	ULONG Size = 0;
	int cou;

	*pBufferSize = 0;

	if (!FlagOn( Globals.m_DrvFlags, _DRV_FLAG_TIMING))
		SetFlag( Globals.m_DrvFlags, _DRV_FLAG_TIMING);

	AcquireResourceExclusive( &Globals.m_TimingLock );

	if (!IsListEmpty( &Globals.m_Timing ))
	{
		for (cou = 0; cou < 2; cou++)
		{
			if (cou)
			{
				if (!Size)
					break;

				*ppBuffer = ExAllocatePoolWithTag( PagedPool, Size, tag_timing_out );
				if (!*ppBuffer)
					break;
				
				RtlZeroMemory( *ppBuffer, Size );
				*pBufferSize = Size;
				Size = 0;
			}
			
			Flink = Globals.m_Timing.Flink;
			while ( Flink != &Globals.m_Timing )
			{
				pItem = CONTAINING_RECORD( Flink, TimingItem, m_List );
				Flink = Flink->Flink;

				if (pItem->m_CallCountPre.QuadPart)
					Size += Timing_Place( Size, *ppBuffer, pItem->m_Name, pItem->m_HookId, pItem->m_Mj, 0, pItem->m_CallCountPre, pItem->m_TimePre );
				
				if (pItem->m_CallCountPost.QuadPart)
					Size += Timing_Place( Size, *ppBuffer, pItem->m_Name, pItem->m_HookId, pItem->m_Mj, 1, pItem->m_CallCountPost, pItem->m_TimePost );
			}
		}
	}

	ReleaseResource( &Globals.m_TimingLock );

	if (Size)
		return STATUS_SUCCESS;

	return STATUS_UNSUCCESSFUL;
}
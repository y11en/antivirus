ATTACK_OPTIONS	AttackOptions;
KEVENT			ClearStatisticEvent;

KL_EVENT		IDS_ResetEvent;
SPIN_LOCK		IDS_Lock;

BOOLEAN			IDS_ResetFlag = FALSE;

LIST_ENTRY		IdsTrustList;
SPIN_LOCK		IdsTrustListLock;

BOOLEAN			bBlockALL = FALSE;
KL_TIME			BlockALL_StopTime;

STRUCT_STORE	GlobalStore;

#include "alert.c"
#include "element.c"
#include "flood.c"
#include "scan.c"
#include "dos.c"
#include "parse.c"
#include "trust.c"
//-----------------------------------------------------------------------------------------------

ULONG	InitializeGlobalStore( VOID )
{
	PSTAT_ELEMENT			StatElement;	
	ULONG					i;	

#ifndef ISWIN9X
	PNOTIFY_ATTACK_CONTEXT	NotifyAttackContext;
    PNDIS_WORK_ITEM			pWorkItem;
#endif
	
	// Лист всех протоколов
	InitializeListHead  (&GlobalStore.ProtocolList);

	// лист всех биндингов (адаптеров)
	InitializeListHead  (&GlobalStore.AdapterList);
	
	InitializeListHead  (&GlobalStore.FreeAdapterList);
	NdisAllocateSpinLock(&GlobalStore.FreeAdapterListLock);
	
	InitializeListHead	(&GlobalStore.Attacks.BlockIpList);
	NdisAllocateSpinLock(&GlobalStore.Attacks.BlockIpListLock);	
	
	InitializeListHead  (&GlobalStore.AttacksStorage.ProtectList);	
	NdisAllocateSpinLock(&GlobalStore.AttacksStorage.ProtectListLock);
	
	InitializeListHead  (&GlobalStore.Attacks.ProtectList);	
	NdisAllocateSpinLock(&GlobalStore.Attacks.ProtectListLock);
	GlobalStore.Attacks.ProtectListCount = 0;
	
    InitializeListHead (&GlobalStore.Attacks.FloodProtectList);	
    AllocateSpinLock(&GlobalStore.Attacks.FloodProtectListLock);
    GlobalStore.Attacks.FloodProtectListCount = 0;

	InitializeListHead  (&GlobalStore.WorkItemList);
	NdisAllocateSpinLock(&GlobalStore.WorkItemListLock);
	GlobalStore.WorkItemListCount = 0;	

	InitializeListHead(&GlobalStore.Attacks.NotifyAttackList);
	AllocateSpinLock  (&GlobalStore.Attacks.NotifyAttackListLock);

	// Выделим память для учета статистики по 1000 различным хостам
	for (i=0; i<1000; i++)
	{
		StatElement = ExAllocatePoolWithTag (NonPagedPool, sizeof (STAT_ELEMENT), COMMON_TAG );
		if (StatElement == NULL)
		{		
			break;
		}
		RtlZeroMemory (StatElement, sizeof (STAT_ELEMENT));
		InsertTailList(&GlobalStore.AttacksStorage.ProtectList, &StatElement->Linkage);
	}

#ifndef ISWIN9X    
	// кэш для 0x5000 INCOMING пакетов... Если закончится, то пакеты мы принимать не сможем... sorry
	for (i=0; i<0x5000; ++i)
	{
		pWorkItem = ExAllocatePoolWithTag(NonPagedPool, sizeof(NDIS_WORK_ITEM), 'iwng');
		InsertTailList(&GlobalStore.WorkItemList, (PLIST_ENTRY)pWorkItem);
		++GlobalStore.WorkItemListCount;
	}

	for (i=0; i<100; i++)
	{
		NotifyAttackContext = ExAllocatePoolWithTag (NonPagedPool, sizeof (NOTIFY_ATTACK_CONTEXT), IDS_NOTIFY_CONTEXT);
		RtlZeroMemory (NotifyAttackContext, sizeof (NOTIFY_ATTACK_CONTEXT));
		InsertTailList(&GlobalStore.Attacks.NotifyAttackList, &NotifyAttackContext->Linkage);
	}

#endif

	GlobalStore.outPlainPacket = ExAllocatePoolWithTag (NonPagedPool, 0xFFFF, COMMON_TAG);
	
	KeInitializeEvent (&GlobalStore.InPacketsEvent,  SynchronizationEvent, FALSE);
	KeInitializeEvent (&GlobalStore.outPacketsEvent, SynchronizationEvent, FALSE);

    InitializeHelkern();

    InitializeListHead(&IdsTrustList);
	AllocateSpinLock  (&IdsTrustListLock);

    KlInitializeEvent(&IDS_ResetEvent, IO_NO_INCREMENT, FALSE);
	AllocateSpinLock (&IDS_Lock);	

	return 0;
}
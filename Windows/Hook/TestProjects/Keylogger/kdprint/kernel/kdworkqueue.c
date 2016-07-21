#include "kdworkqueue.h"

static BOOLEAN g_bWorkQueue_Inited = FALSE;

static KSPIN_LOCK g_slWorkItemList;
static LIST_ENTRY gWorkItemList_Head;

static HANDLE g_hProcessingThread = NULL;

static volatile BOOLEAN g_bUnInit = FALSE;
static volatile BOOLEAN g_bThreadExited = FALSE;

VOID NTAPI KDWorker(IN PVOID Context)
{
	LARGE_INTEGER SleepTime;
	BOOLEAN bListNotEmpty = TRUE;

	SleepTime.QuadPart = -200000L;	// 20 msecs

// вываливаемся по сигналу, когда все элементы выбраны
	while (!g_bUnInit || bListNotEmpty)
	{
		KIRQL OldIrql;

		bListNotEmpty = FALSE;

		KeAcquireSpinLock(&g_slWorkItemList, &OldIrql);

		if (!IsListEmpty(&gWorkItemList_Head))
		{
			WORK_QUEUE_ITEM WorkItem = *((PWORK_QUEUE_ITEM)gWorkItemList_Head.Flink);

			RemoveHeadList(&gWorkItemList_Head);

			KeReleaseSpinLock(&g_slWorkItemList, OldIrql);
// ускоряем выгрузку
			if (!g_bUnInit)
				WorkItem.WorkerRoutine(WorkItem.Parameter);

			bListNotEmpty = TRUE;
		}
		else
			KeReleaseSpinLock(&g_slWorkItemList, OldIrql);

// ускоряем выгрузку
		if (!g_bUnInit)		
			KeDelayExecutionThread(KernelMode, FALSE, &SleepTime);
	}

	
	g_bThreadExited = TRUE;
}

NTSTATUS KDInitWorkQueue()
{
	NTSTATUS ntStatus;
	HANDLE hProcessingThread;

	if (g_bWorkQueue_Inited)
		return STATUS_SUCCESS;

	InitializeListHead(&gWorkItemList_Head);
	KeInitializeSpinLock(&g_slWorkItemList);

	g_bUnInit = FALSE;
	ntStatus = PsCreateSystemThread(&hProcessingThread, 0, NULL, NULL, NULL, KDWorker, NULL);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	g_hProcessingThread = hProcessingThread;

	g_bWorkQueue_Inited = TRUE;
	g_bThreadExited = FALSE;

	return STATUS_SUCCESS;
}

VOID KDUnInitWorkQueue()
{
	LARGE_INTEGER Interval;

	if (!g_bWorkQueue_Inited)
		return;

	g_bWorkQueue_Inited = FALSE;

// завершаем наш поток, ZwWaitForSingleObject сразу же возвращается, не дождавшись выхода потока..
	Interval.QuadPart = -1000000L;		// 100 msecs
	g_bUnInit = TRUE;
	while (!g_bThreadExited)
		KeDelayExecutionThread(KernelMode, FALSE, &Interval);
}

VOID KDQueueWorkItem(IN PWORK_QUEUE_ITEM pWorkItem, IN ULONG Unused OPTIONAL)
{
	KIRQL OldIrql;

	if (!g_bWorkQueue_Inited)
		return;

	KeAcquireSpinLock(&g_slWorkItemList, &OldIrql);	

	InsertTailList(&gWorkItemList_Head, (PLIST_ENTRY)pWorkItem);

	KeReleaseSpinLock(&g_slWorkItemList, OldIrql);
}

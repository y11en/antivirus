#include <ntifs.h>
#include <stdio.h>
#include <stdarg.h>
#include "kdprint.h"
#include "..\kdshared.h"
#include "kdworkqueue.h"

#define PROCESS_VM_OPERATION      (0x0008)

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcess(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId OPTIONAL
);
NTSYSAPI
LONG
NTAPI
KeReleaseMutant (
    IN PRKMUTANT Mutant,
    IN KPRIORITY Increment,
    IN BOOLEAN Abandoned,
    IN BOOLEAN Wait
    );


static volatile BOOLEAN g_bKDPrint_Inited = FALSE;
static PRKMUTANT g_pKDSectionMutant = NULL;
static PKDPRINT_SECTION g_pKDSectionBase = NULL;

static volatile ULONG g_KDPrintWorkItem_Count = 0;


// вызывать в контексте системной нити после основной инициализации
VOID _KDPrintPostInit_Worker(IN PVOID Context)
{
	g_pKDSectionBase->m_Signature = 0x21081979;
	g_pKDSectionBase->m_BufferTop = 0;

	g_KDPrintWorkItem_Count = 0;

	g_bKDPrint_Inited = TRUE;

	ExFreePool(Context);
}

NTSTATUS KDPrintInit(IN HANDLE hSectionMutant, IN HANDLE hSection)
{	
	NTSTATUS ntStatus;
	PVOID pSectionBase = NULL;
	SIZE_T ViewSize = 0;
	PRKMUTANT pSectionMutant;
	UNICODE_STRING cntdSystemProcessName;
	OBJECT_ATTRIBUTES SystemProcessOA;
	HANDLE hSystemProcess;
	CLIENT_ID ClientID;
	OBJECT_ATTRIBUTES ThreadOA;
	HANDLE hThread;
	PRKTHREAD pThread;
	PWORK_QUEUE_ITEM pPostInit_Worker;

	if (g_bKDPrint_Inited)
		return STATUS_SUCCESS;

// получаем объект юзерового мьютекса
	ntStatus = ObReferenceObjectByHandle(hSectionMutant, 0, NULL, UserMode, &pSectionMutant, NULL);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

//открываем процесс System
	InitializeObjectAttributes(&SystemProcessOA, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
	ClientID.UniqueProcess = (HANDLE)4;
	ClientID.UniqueThread = 0;
	ntStatus = ZwOpenProcess(&hSystemProcess, PROCESS_VM_OPERATION, &SystemProcessOA, &ClientID);
	if (!NT_SUCCESS(ntStatus))
	{
		ObDereferenceObject(pSectionMutant);
		return ntStatus;
	}

// маппируем секцию в АП процесса System
	ntStatus = ZwMapViewOfSection(hSection, hSystemProcess, &pSectionBase, 0, 0, NULL, 
			&ViewSize, ViewUnmap, 0, PAGE_READWRITE);
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hSystemProcess);
		ObDereferenceObject(pSectionMutant);
		return ntStatus;
	}

	g_pKDSectionMutant = pSectionMutant;
	g_pKDSectionBase = (PKDPRINT_SECTION)pSectionBase;

	ntStatus = KDInitWorkQueue();
	if (!NT_SUCCESS(ntStatus))
	{
		ZwUnmapViewOfSection(hSystemProcess, pSectionBase);
		ZwClose(hSystemProcess);
		ObDereferenceObject(pSectionMutant);
		return ntStatus;
	}

// постинициализация в контексте системной нити
	pPostInit_Worker = (PWORK_QUEUE_ITEM)ExAllocatePoolWithTag(NonPagedPool, sizeof(WORK_QUEUE_ITEM), 'kdPI');
	if (!pPostInit_Worker)
	{
		KDUnInitWorkQueue();
		ZwUnmapViewOfSection(hSystemProcess, pSectionBase);
		ZwClose(hSystemProcess);
		ObDereferenceObject(pSectionMutant);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	ExInitializeWorkItem(pPostInit_Worker, _KDPrintPostInit_Worker, pPostInit_Worker);
	ExQueueWorkItem(pPostInit_Worker, DelayedWorkQueue);

	ZwClose(hSystemProcess);
	
	return ntStatus;
}

// вызывать только из контекста системной нитки
VOID KDPrintUnInit()
{
	if (!g_bKDPrint_Inited)
		return;
	
	g_bKDPrint_Inited = FALSE;

	KDUnInitWorkQueue();
	ZwUnmapViewOfSection(NtCurrentProcess(),  g_pKDSectionBase);
	ObDereferenceObject(g_pKDSectionMutant);
}

// вызываем только в контекте системной нитки
VOID _KDPrintAtPassive(IN PCHAR Str2Output)
{
	LARGE_INTEGER Timeout;
	NTSTATUS ntStatus;

// чтобы надолго не зависала обработка отсавшихся work item'ов при выгрузке
	if (!g_bKDPrint_Inited)
		return;

// захватывем мутант
	Timeout.QuadPart = -20000000L;	// 2 secs
	ntStatus = KeWaitForSingleObject(g_pKDSectionMutant, Executive, KernelMode, FALSE, &Timeout);
	if (ntStatus == STATUS_SUCCESS)
	{
		LARGE_INTEGER ExpireTime;
		ULONG WaitCount;
		ULONG cbStr2Output;

		cbStr2Output = strlen(Str2Output)+1;

		ExpireTime.QuadPart = -200000L;	// 20 msecs
// ждем освобождения буфера
		WaitCount = 0;
		while (g_pKDSectionBase->m_BufferTop+cbStr2Output > KD_BUFFER_LEN && WaitCount < 100)
		{
// чтобы клиентские потоки могли завладеть секцией и выгрести данные
			KeReleaseMutant(g_pKDSectionMutant, IO_NO_INCREMENT, FALSE, FALSE);

			KeDelayExecutionThread(KernelMode, FALSE, &ExpireTime);

			ntStatus = KeWaitForSingleObject(g_pKDSectionMutant, Executive, KernelMode, FALSE, &Timeout);
			if (ntStatus != STATUS_SUCCESS)
				return;

			WaitCount++;
		}
		if (WaitCount >= 100)
		{
			KeReleaseMutant(g_pKDSectionMutant, IO_NO_INCREMENT, FALSE, FALSE);
			return;
		}

		strcpy(&g_pKDSectionBase->m_Buffer[g_pKDSectionBase->m_BufferTop], Str2Output);
		g_pKDSectionBase->m_BufferTop += cbStr2Output;

		KeReleaseMutant(g_pKDSectionMutant, IO_NO_INCREMENT, FALSE, FALSE);
	}
}

VOID _KDPrintWorker(IN PVOID Context)
{
	_KDPrintAtPassive((PCHAR)Context+sizeof(WORK_QUEUE_ITEM));

	ExFreePool(Context);
// контроль количества поставленных в очередь элементов
	InterlockedDecrement((PLONG)&g_KDPrintWorkItem_Count);
}

VOID _KDPrint(IN PCHAR Str2Output)
{
	PWORK_QUEUE_ITEM pWorkItem;
	PWCHAR WorkerStr;
	KIRQL Irql;
	ULONG cbStr;

	if (!g_bKDPrint_Inited)
		return;
	
	Irql = KeGetCurrentIrql();
	if (Irql > DISPATCH_LEVEL)
		return;

// не будем задвигать слишком много item'ов
	if (g_KDPrintWorkItem_Count > 1000)
		return;

// экономим пул
	cbStr = strlen(Str2Output)+1;
// в хвосте выделенной памяти будет наша строка
	pWorkItem = ExAllocatePoolWithTag(NonPagedPool, sizeof(WORK_QUEUE_ITEM)+cbStr, 'kdWI');
	if (!pWorkItem)
		return;
	strcpy((PCHAR)pWorkItem+sizeof(WORK_QUEUE_ITEM), Str2Output);

	ExInitializeWorkItem(pWorkItem, _KDPrintWorker, pWorkItem);
	KDQueueWorkItem(pWorkItem, DelayedWorkQueue);
	InterlockedIncrement((PLONG)&g_KDPrintWorkItem_Count);
}

VOID KDPrint(PCHAR FormatStr, ...)
{
	PCHAR OutputStr;

	OutputStr = (PCHAR)ExAllocatePoolWithTag(NonPagedPool, KD_MAX_STR_SIZE, 'ppOS');
	if (OutputStr)
	{
		va_list Mark;

		va_start(Mark, FormatStr);
		_vsnprintf(OutputStr, KD_MAX_STR_SIZE, FormatStr, Mark);
		va_end(Mark);

		_KDPrint(OutputStr);

		ExFreePool(OutputStr);
	}
}

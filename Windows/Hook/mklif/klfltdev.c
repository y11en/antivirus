#include "klfltdev.h"
#include "..\klfltdev\ioctl.h"

NTSTATUS 
SendDevIoCtl2KLFltDevByName(
	__in ULONG IoControlCode, 
	__in PVOID InputBuffer,
	__in ULONG InputBufferLength,
	__out PVOID OutputBuffer,
	__in ULONG OutputBufferLength, 
	__in PCWSTR pDrvName, 
	__out ULONG* pRetSize
)
{
	HANDLE						hExtDrv;
	OBJECT_ATTRIBUTES	ObjAttr;
	UNICODE_STRING		us;
	IO_STATUS_BLOCK		ioStatus;
	PDEVICE_OBJECT		DevObj;
	PFILE_OBJECT			fileObject;
	NTSTATUS					ntStatus;
	KEVENT						Event;
	PIRP							Irp;
	PIO_STACK_LOCATION irpSp;
	
	RtlInitUnicodeString(&us, pDrvName);
	InitializeObjectAttributes(&ObjAttr,&us,OBJ_CASE_INSENSITIVE,NULL,NULL);
	ntStatus=ZwCreateFile(&hExtDrv,SYNCHRONIZE|FILE_ANY_ACCESS,&ObjAttr,&ioStatus,NULL,0,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_OPEN,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
	if(NT_SUCCESS(ntStatus)) {
		ntStatus=ObReferenceObjectByHandle(hExtDrv,FILE_READ_DATA,NULL,KernelMode, (PVOID*) &fileObject,NULL);
		if(NT_SUCCESS(ntStatus)) {
			if((DevObj=IoGetRelatedDeviceObject(fileObject))!=NULL) {
				KeInitializeEvent(&Event,NotificationEvent,FALSE);
				Irp=IoBuildDeviceIoControlRequest(IoControlCode,DevObj,InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,FALSE,&Event,&ioStatus);
				if(Irp!=NULL) {
					irpSp=IoGetNextIrpStackLocation(Irp);
					irpSp->FileObject = fileObject;
					ntStatus=IoCallDriver(DevObj,Irp);
					if(ntStatus==STATUS_PENDING) {
						KeWaitForSingleObject(&Event,Executive,KernelMode,FALSE,(PLARGE_INTEGER)NULL);
						ntStatus = ioStatus.Status;
					}
					if (pRetSize != NULL)
						*pRetSize = (ULONG)ioStatus.Information;
				} else {
					ntStatus=STATUS_UNSUCCESSFUL;
				}
			} else {
				ntStatus=STATUS_UNSUCCESSFUL;
			}
			ObDereferenceObject(fileObject);
		} 
		ZwClose(hExtDrv);
	}

	return ntStatus;
}

NTSTATUS 
KLFltDev_SetRule(
	__in PVOID InputBuffer,
	__in ULONG InputBufferLength,
	__out PVOID OutputBuffer,
	__in ULONG OutputBufferLength, 
	__out ULONG* pRetSize
)
{
	return SendDevIoCtl2KLFltDevByName(
					IOCTL_KLFLTDEV_ADDRULE, 
					InputBuffer,
					InputBufferLength,
					OutputBuffer,
					OutputBufferLength, 
					NTDEVICE_NAME_STRING, 
					pRetSize
					);
}

NTSTATUS 
KLFltDev_GetRulesSize(
	__out PVOID OutputBuffer,
	__in ULONG OutputBufferLength, 
	__out ULONG* pRetSize
)
{
	return SendDevIoCtl2KLFltDevByName(
		IOCTL_KLFLTDEV_GETRULES_SIZE, 
		NULL,
		0,
		OutputBuffer,
		OutputBufferLength, 
		NTDEVICE_NAME_STRING, 
		pRetSize
		);
}

NTSTATUS 
KLFltDev_GetRules(
	__out PVOID OutputBuffer,
	__in ULONG OutputBufferLength, 
	__out ULONG* pRetSize
)
{
	return SendDevIoCtl2KLFltDevByName(
		IOCTL_KLFLTDEV_GETRULES, 
		NULL,
		0,
		OutputBuffer,
		OutputBufferLength, 
		NTDEVICE_NAME_STRING, 
		pRetSize
		);
}

NTSTATUS 
KLFltDev_ApplyRules()
{
	ULONG RetSize;
	return SendDevIoCtl2KLFltDevByName(
		IOCTL_KLFLTDEV_APPLYRULES, 
		NULL,
		0,
		NULL,
		0,
		NTDEVICE_NAME_STRING, 
		&RetSize
		);
}
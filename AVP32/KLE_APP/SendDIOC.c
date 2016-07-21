#include <ntddk.h>
#include "SendDIOC.h"

NTSTATUS SendDIOC(PUNICODE_STRING uDeviceName, ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength,PVOID OutputBuffer,ULONG OutputBufferLength)
{
	HANDLE              hPidDrv;
	OBJECT_ATTRIBUTES   ObjAttr;
	IO_STATUS_BLOCK     ioStatus;
	PDEVICE_OBJECT      DevObj;
	PFILE_OBJECT        fileObject;
	NTSTATUS            ntStatus;
	KEVENT              Event;
	PIRP                Irp;
	//	PIO_STACK_LOCATION irpSp;
	//	RtlInitUnicodeString(&us,L"\\Device\\"KLPID_NAME);
	
	InitializeObjectAttributes(&ObjAttr,uDeviceName,OBJ_CASE_INSENSITIVE,NULL,NULL);
	ntStatus=ZwCreateFile(&hPidDrv,SYNCHRONIZE|FILE_ANY_ACCESS,&ObjAttr,&ioStatus,NULL,0,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_OPEN,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
	if(NT_SUCCESS(ntStatus)) {
		ntStatus=ObReferenceObjectByHandle(hPidDrv,FILE_READ_DATA,NULL,KernelMode,(PVOID*)&fileObject,NULL);
		if(NT_SUCCESS(ntStatus)) {
			if((DevObj=IoGetRelatedDeviceObject(fileObject))!=NULL) {
				KeInitializeEvent(&Event,NotificationEvent,FALSE);
				Irp=IoBuildDeviceIoControlRequest(IoControlCode,DevObj,InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,FALSE,&Event,&ioStatus);
				if(Irp!=NULL) {
					//					irpSp=IoGetNextIrpStackLocation(Irp);
					//					irpSp->FileObject = fileObject;
					ntStatus=IoCallDriver(DevObj,Irp);
					if(ntStatus==STATUS_PENDING) {
						KeWaitForSingleObject(&Event,Executive,KernelMode,FALSE,(PLARGE_INTEGER)NULL);
						ntStatus = ioStatus.Status;
					}
				} else {
					//					HOOKKdPrint(4, ("HOOK: IoBuildDeviceIoControlRequest failed\n"));
					ntStatus=STATUS_UNSUCCESSFUL;
				}
			} else {
				//				HOOKKdPrint(1, ("HOOK: IoGetRelatedDeviceObject %S failed \n",us.Buffer));
				ntStatus=STATUS_UNSUCCESSFUL;
			}
			ObDereferenceObject(fileObject);
		} else {
			//			HOOKKdPrint(1, ("HOOK: ObReferenceObjectByHandle %S failed status=%x\n",us.Buffer,ntStatus));
		}
		ZwClose(hPidDrv);
	} else {
		//		HOOKKdPrint(1, ("HOOK: ZwCreateFile %S failed status=%x\n",us.Buffer,ntStatus));
	}
	return ntStatus;
}

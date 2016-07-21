#include "fidbox.h"
#include "..\osspec.h"
#include "specfunc.h"
#include "objidindex.h"
#include "glbenvir.h"
#include "..\klsecur.h"
#include "..\hook\hookspec.h"

tFile gFidBoxFileData;
tFile gFidBoxFileIndex;
// Gruzdev -----------------------------------------------------------------------------------
tFile gFidBox2FileData;
tFile gFidBox2FileIndex;
//--------------------------------------------------------------------------------------------

BOOLEAN gbAllowFidBox = FALSE;
BOOLEAN gbDisableInSession = FALSE;

tHeap gFidBoxHeap;
OBJID_CTX* gpFidBox = 0;
BOOLEAN gbFidBox_InExit = FALSE;
ERESOURCE gFidBoxLock;
ULONG gFidBoxItemSize = 0;
// Gruzdev -----------------------------------------------------------------------------------
tHeap gFidBox2Heap;
OBJID_CTX* gpFidBox2 = 0;
BOOLEAN gbFidBox2_InExit = FALSE;
ERESOURCE gFidBox2Lock;
ULONG gFidBox2ItemSize = 0;
//--------------------------------------------------------------------------------------------

BOOLEAN gbINetSwitftDisabled = FALSE;

BOOLEAN
IsINetSwiftDisabled()
{
	BOOLEAN bDisabled = FALSE;

	HKEY				ParKeyHandle;
	UNICODE_STRING		ValueName;
	PKEY_VALUE_PARTIAL_INFORMATION pinfo = NULL;
	OBJECT_ATTRIBUTES	objectAttributes;

	NTSTATUS ntstatus;

	DWORD dwsize = sizeof(KEY_VALUE_PARTIAL_INFORMATION) + 32;

	if((pinfo = ExAllocatePoolWithTag(PagedPool, dwsize,'.BOS')))
	{
		InitializeObjectAttributes(&objectAttributes, &RegParams, OBJ_CASE_INSENSITIVE,NULL, NULL);
		ntstatus = ZwOpenKey(&ParKeyHandle, KEY_READ, &objectAttributes);
		if(NT_SUCCESS(ntstatus))
		{
			ULONG TmpLen = 0;
			RtlInitUnicodeString(&ValueName, L"INetSwiftDisable");
			ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
							pinfo, dwsize, &TmpLen);
			
			if(NT_SUCCESS(ntstatus) && pinfo->DataLength == sizeof(DWORD))
			{
				PULONG pTmp = (PULONG) pinfo->Data;
				if (*pTmp)
					bDisabled = TRUE;
			}

			RtlInitUnicodeString(&ValueName, L"NoINetSwift");
			ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
							pinfo, dwsize, &TmpLen);
			
			if(NT_SUCCESS(ntstatus) && pinfo->DataLength == sizeof(DWORD))
			{
				PULONG pTmp = (PULONG) pinfo->Data;
				if (*pTmp)
					bDisabled = TRUE;
			}

			ZwClose(ParKeyHandle);
		}

		ExFreePool(pinfo);
	}

	return bDisabled;
}

BOOLEAN
IsISwiftDisabled()
{
	BOOLEAN bDisabled = FALSE;

	HKEY				ParKeyHandle;
	UNICODE_STRING		ValueName;
	PKEY_VALUE_PARTIAL_INFORMATION pinfo = NULL;
	OBJECT_ATTRIBUTES	objectAttributes;

	NTSTATUS ntstatus;

	DWORD dwsize = sizeof(KEY_VALUE_PARTIAL_INFORMATION) + 32;

	if((pinfo = ExAllocatePoolWithTag(PagedPool, dwsize,'.BOS')))
	{
		InitializeObjectAttributes(&objectAttributes, &RegParams, OBJ_CASE_INSENSITIVE,NULL, NULL);
		ntstatus = ZwOpenKey(&ParKeyHandle, KEY_READ, &objectAttributes);
		if(NT_SUCCESS(ntstatus))
		{
			ULONG TmpLen = 0;
			RtlInitUnicodeString(&ValueName, L"NoISwift");
			ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
							pinfo, dwsize, &TmpLen);
			
			if(NT_SUCCESS(ntstatus) && pinfo->DataLength == sizeof(DWORD))
			{
				PULONG pTmp = (PULONG) pinfo->Data;
				if (*pTmp)
					bDisabled = TRUE;
			}

			ZwClose(ParKeyHandle);
		}

		ExFreePool(pinfo);
	}

	return bDisabled;
}



//+ ------------------------------------------------------------------------------------------

bool __stdcall Fid_HeapAlloc(tHeap* pHeap, size_t size, void** ppMem)
{
	void* pMem = NULL;
	if (!ppMem)
		return false;
	pMem = ExAllocatePoolWithTag(PagedPool, size, 'iBOs');
	if (pMem)
		memset(pMem, 0, size);
	
	*ppMem = pMem;

	return (pMem != NULL);
}

bool __stdcall Fid_HeapFree(tHeap* pHeap, void* pMem)
{
	if (!pMem)
		return false;
	
	ExFreePool(pMem);

	return true;
}

bool __stdcall Fid_HeapRealloc(tHeap* pHeap, void* pMem, size_t size, void** ppMem)
{
	DbgBreakPoint();
	return false;
}

void __stdcall Fid_HeapDestroy(tHeap* pHeap)
{
}

void Fid_InitHeap(tHeap* pHeap, HANDLE hHeap)
{
	pHeap->hHeap = hHeap;
	pHeap->Alloc = Fid_HeapAlloc;
	pHeap->Realloc = Fid_HeapRealloc;
	pHeap->Free = Fid_HeapFree;
	pHeap->Destroy = Fid_HeapDestroy;
}

/*PWCHAR FdGetRealFName(PWCHAR pwchName, PWCHAR pwchFname)
{
	PWCHAR pwstrUniNameRet = NULL;
	NTSTATUS ntStatus;
	HANDLE hFile = NULL;
	PFILE_OBJECT pFileObject;
	
	ntStatus = OpenFile(pwchName, &hFile, GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE, 
		0, FILE_OPEN_IF, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_TEMPORARY,
		FILE_SYNCHRONOUS_IO_NONALERT | FILE_DELETE_ON_CLOSE);

	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ObReferenceObjectByHandle(hFile, 0, *IoFileObjectType, KernelMode, (PVOID*) &pFileObject, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			PDEVICE_OBJECT pDevice = NULL;
			if (pFileObject->DeviceObject != NULL)
			{
				pDevice = pFileObject->DeviceObject;
				
				if (pDevice->Vpb != NULL)
				{
					if (pDevice->Vpb->RealDevice != NULL)
						pDevice = pDevice->Vpb->RealDevice;
				}
				
				if (pDevice->Flags & DO_DEVICE_INITIALIZING)
					pDevice = NULL;
			}

			if (pDevice && (pDevice->Flags & DO_DEVICE_HAS_NAME))
			{
				PWCHAR pDeviceName = ExAllocatePoolWithTag(NonPagedPool, MAXPATHLEN * sizeof(WCHAR), 'WboS');
				if (pDeviceName != NULL)
				{
					if (GetDeviceName(pDevice, pDeviceName, MAXPATHLEN * sizeof(WCHAR)) != NULL)
					{
						ULONG len = wcslen(pDeviceName) * sizeof(WCHAR);
						pwstrUniNameRet = ExAllocatePoolWithTag(PagedPool, 
							len + sizeof(L"\\System Volume Information\\") + wcslen(pwchFname) * sizeof(WCHAR), 
							'jbos');

						wcscpy(pwstrUniNameRet, pDeviceName);
						wcscat(pwstrUniNameRet, L"\\System Volume Information\\");
						wcscat(pwstrUniNameRet, pwchFname);
					}

					ExFreePool(pDeviceName);
				}

			}

			ObDereferenceObject(pFileObject);
		}

		ZwClose(hFile);
	}

	return pwstrUniNameRet;
}*/

//+ ------------------------------------------------------------------------------------------
static NTSTATUS irpCompletion(
    PDEVICE_OBJECT  deviceObject,
    PIRP            irp,
    PVOID           context
    )
{
    *irp->UserIosb = irp->IoStatus;         // Copy status information to
                                            // the user
    KeSetEvent(irp->UserEvent, 0, FALSE);   // Signal event
    IoFreeIrp(irp);                         // Free IRP
    return STATUS_MORE_PROCESSING_REQUIRED; // Tell the I/O manager to stop
}

bool __stdcall Fid_FileRead(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesRead)
{
	bool bRet = false;
	PVOID pBuffer;
	PDEVICE_OBJECT pDevice = pFile->hHandle->DeviceObject;

	if (pDevice->Vpb && pDevice->Vpb->DeviceObject)
		pDevice = pDevice->Vpb->DeviceObject;
	else
	{
		DbgBreakPoint();
		return false;
	}

	pBuffer = ExAllocatePoolWithTag(NonPagedPool, nSize, 'rBOs');
	if (pBuffer)
	{
		PIRP                irp;
		irp = IoAllocateIrp(pDevice->StackSize, FALSE);

		if (irp)
		{
			PMDL pMdl;
			pMdl = IoAllocateMdl(pBuffer, nSize, FALSE, FALSE, irp);
			if (!pMdl)
				IoFreeIrp(irp);
			else
			{
				KEVENT              event;
				IO_STATUS_BLOCK		UserIosb;
				PIO_STACK_LOCATION  irpSp;

				MmBuildMdlForNonPagedPool(pMdl);

				KeInitializeEvent(&event, NotificationEvent, FALSE);
				
				irp->Flags = IRP_READ_OPERATION;
				irp->MdlAddress = pMdl;
				irp->Tail.Overlay.Thread = PsGetCurrentThread();
				irp->Tail.Overlay.OriginalFileObject = pFile->hHandle;
				irp->RequestorMode = KernelMode;
				irp->UserEvent = &event;
				irp->UserIosb = &UserIosb;

				irpSp = IoGetNextIrpStackLocation(irp);
				irpSp->MajorFunction = IRP_MJ_READ;
				irpSp->MinorFunction = 0;
				irpSp->DeviceObject = pDevice;
				irpSp->FileObject = pFile->hHandle;
				irpSp->Parameters.Read.Length = nSize;
				irpSp->Parameters.Read.Key = 0;
				irpSp->Parameters.Read.ByteOffset = pFile->hHandle->CurrentByteOffset;

				IoSetCompletionRoutine(irp, irpCompletion, 0, TRUE, TRUE, TRUE);
				IoCallDriver(pDevice, irp);
				KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

				if (NT_SUCCESS(UserIosb.Status) && UserIosb.Information == nSize)
				{
					memcpy(pData, pBuffer, nSize);
					bRet = true;
				}
				else
				{
					//DbgBreakPoint();
				}

				IoFreeMdl(pMdl);
			}
		}

		ExFreePool(pBuffer);
	}
	
	return bRet;
}

bool __stdcall Fid_FileWrite(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesWritten)
{
	bool bRet = false;
	PVOID pBuffer;
	PDEVICE_OBJECT pDevice = pFile->hHandle->DeviceObject;

	if (pDevice->Vpb && pDevice->Vpb->DeviceObject)
		pDevice = pDevice->Vpb->DeviceObject;
	else
	{
		DbgBreakPoint();
		return false;
	}

	pBuffer = ExAllocatePoolWithTag(NonPagedPool, nSize, 'rBOs');
	if (pBuffer)
	{
		PMDL pMdl;

		pMdl = IoAllocateMdl(pBuffer, nSize, FALSE, FALSE, 0);
		if (pMdl)
		{
			memcpy(pBuffer, pData, nSize);
			MmBuildMdlForNonPagedPool(pMdl);

			{
				PIRP                irp;
				KEVENT              event;
				IO_STATUS_BLOCK		UserIosb;
			
				irp = IoAllocateIrp(pDevice->StackSize, FALSE);
				if (irp)
				{
					PIO_STACK_LOCATION  irpSp;

					KeInitializeEvent(&event, NotificationEvent, FALSE);
					
					irp->Flags = IRP_WRITE_OPERATION;
					irp->MdlAddress = pMdl;
					irp->Tail.Overlay.Thread = PsGetCurrentThread();
					irp->Tail.Overlay.OriginalFileObject = pFile->hHandle;
					irp->RequestorMode = KernelMode;
					irp->UserEvent = &event;
					irp->UserIosb = &UserIosb;

					irpSp = IoGetNextIrpStackLocation(irp);
					irpSp->MajorFunction = IRP_MJ_WRITE;
					irpSp->MinorFunction = 0;
					irpSp->DeviceObject = pDevice;
					irpSp->FileObject = pFile->hHandle;
					irpSp->Parameters.Write.Length = nSize;
					irpSp->Parameters.Write.Key = 0;
					irpSp->Parameters.Write.ByteOffset = pFile->hHandle->CurrentByteOffset;

					IoSetCompletionRoutine(irp, irpCompletion, 0, TRUE, TRUE, TRUE);
					IoCallDriver(pDevice, irp);
					KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

					if (NT_SUCCESS(UserIosb.Status) && UserIosb.Information == nSize)
					{
						if (pnBytesWritten)
							*pnBytesWritten = UserIosb.Information;

						bRet = true;
					}
					else
					{
						DbgBreakPoint();
					}
				}
			}

			IoFreeMdl(pMdl);
		}

		ExFreePool(pBuffer);
	}
	
	return bRet;

}

bool __stdcall Fid_FileSeek(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos)
{
	tFilePos nNewPos;
	LARGE_INTEGER newPos;
	
	if (SEEK_SET != nSeekMethod)
	{
		DbgBreakPoint();
		return false;
	}
	
	PAGED_CODE();
	
	newPos.LowPart = nPos;
	newPos.HighPart = 0;
	
	
    {
		NTSTATUS status;
		FILE_POSITION_INFORMATION filePositionInformation;
		ULONG size;
		
		filePositionInformation.CurrentByteOffset = newPos;
		
		size = sizeof(filePositionInformation);
		status = IoSetInformation(pFile->hHandle, FilePositionInformation, sizeof(filePositionInformation), 
			&filePositionInformation);
		
		if (NT_SUCCESS(status))
			return true;
	}
	
	return false;
}

bool __stdcall Fid_FileSeekRead(tFile* pFile, tFilePos nPos, unsigned char* pData, size_t nSize, size_t* pnBytesRead)
{
	if (!Fid_FileSeek(pFile, nPos, SEEK_SET, NULL))
		return false;

	return Fid_FileRead(pFile, pData, nSize, pnBytesRead);
}

bool __stdcall Fid_FileSeekWrite(tFile* pFile, tFilePos nPos, unsigned char* pData, size_t nSize, size_t* pnBytesWritten)
{
	if (!Fid_FileSeek(pFile, nPos, SEEK_SET, NULL))
		return false;
	
	return Fid_FileWrite(pFile, pData, nSize, pnBytesWritten);
}


bool __stdcall Fid_FileGetSize(tFile* pFile, tFileSize* pnSize)
{
	tFileSize nSize;

    NTSTATUS status;
    FILE_STANDARD_INFORMATION fileStandardInformation;
	ULONG size;

	PAGED_CODE();

	if (!pnSize)
	{
	   DbgBreakPoint();
	   return false;
	}
	
	size = sizeof(fileStandardInformation);
	status = IoQueryFileInformation(pFile->hHandle, FileStandardInformation, sizeof(fileStandardInformation), 
		&fileStandardInformation, &size);
	
    if (NT_SUCCESS(status))
	{
		*pnSize = fileStandardInformation.EndOfFile.LowPart;	//QuadPart
		return true;
	}
	
	return false;
}

bool __stdcall Fid_FileSetSize(tFile* pFile, tFileSize nSize)
{
	DbgBreakPoint();
	return false;
}

bool __stdcall Fid_FileFlush(tFile* pFile)
{
	NTSTATUS				ntStatus;
	PIRP					Irp;
	IO_STATUS_BLOCK			ioStatus;
	KEVENT					event;
	PIO_STACK_LOCATION		IrpSp;
	
	PDEVICE_OBJECT pDevice = pFile->hHandle->DeviceObject;

	PAGED_CODE();

	if (pDevice->Vpb && pDevice->Vpb->DeviceObject)
		pDevice = pDevice->Vpb->DeviceObject;
	else
	{
		DbgBreakPoint();
		return false;
	}
	
	KeInitializeEvent(&event, NotificationEvent, FALSE);
	
	if(!(Irp = IoBuildSynchronousFsdRequest(IRP_MJ_FLUSH_BUFFERS, pDevice, NULL, 0, 0, &event, &ioStatus)))
	{
		DbgBreakPoint();
		return false;
	}
	
	IrpSp = IoGetNextIrpStackLocation(Irp);
	IrpSp->FileObject = pFile->hHandle;
		
	ntStatus = IoCallDriver(pDevice, Irp);
	if(ntStatus == STATUS_PENDING)
	{
		KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
		ntStatus = ioStatus.Status;
	}

	if (NT_SUCCESS(ntStatus))
		return true;

	return false;
}

bool __stdcall Fid_FileClose(tFile* pFile)
{
	PAGED_CODE();

	if (pFile->hHandle)
	{
		DbPrint(DC_SYS, DL_IMPORTANT, ("dereference fidbox object\n"));
		if (gbDisableInSession)
		{
			PDEVICE_OBJECT pDevice = pFile->hHandle->DeviceObject;
			if (pDevice->Vpb && pDevice->Vpb->DeviceObject)
			{
				pDevice = pDevice->Vpb->DeviceObject;
				SetDeleteInfo(pFile->hHandle, pDevice);
			}

		}
		ObDereferenceObject(pFile->hHandle);
		pFile->hHandle = 0;

		ZwClose(pFile->hFile);
		pFile->hFile = NULL;
	}
	
	return true;
}

void __stdcall Fid_InitFile(tFile* pFile, tFileHandle hHandle)
{
	pFile->hHandle = hHandle;
	pFile->SeekRead = Fid_FileSeekRead;
	pFile->SeekWrite = Fid_FileSeekWrite;
	pFile->Read = Fid_FileRead;
	pFile->Write = Fid_FileWrite;
	pFile->Seek = Fid_FileSeek;
	pFile->GetSize = Fid_FileGetSize;
	pFile->SetSize = Fid_FileSetSize;
	pFile->Flush = Fid_FileFlush;
	pFile->Close = Fid_FileClose;
}


void
FidCloseAllFiles (
	)
{
	ObDereferenceObject( gFidBoxFileData.hHandle );
	ObDereferenceObject( gFidBoxFileIndex.hHandle );

	ZwClose( gFidBoxFileData.hFile );
	ZwClose( gFidBoxFileIndex.hFile );
}


//+ ------------------------------------------------------------------------------------------

void FidBox_InitImpInternal()
{
	NTSTATUS ntStatus;

	PWSTR pwstrUniNameIdx = L"\\SystemRoot\\system32\\drivers\\fidbox.idx";
	PWSTR pwstrUniNameDat = L"\\SystemRoot\\system32\\drivers\\fidbox.dat";

	DbPrint(DC_SYS, DL_IMPORTANT, ("FidBox_InitImp\n"));

/*	pwstrUniNameIdx = FdGetRealFName(L"\\SystemRoot\\fidbox.idx", L"fidbox.idx");
	pwstrUniNameDat = FdGetRealFName(L"\\SystemRoot\\fidbox.dat", L"fidbox.dat");

	if (!pwstrUniNameIdx || !pwstrUniNameDat)
	{
		if (pwstrUniNameDat)
			ExFreePool(pwstrUniNameDat);
		if (pwstrUniNameIdx)
			ExFreePool(pwstrUniNameIdx);
		return;
	}
*/
	if (IsINetSwiftDisabled())
	{
		gbINetSwitftDisabled = TRUE;
		DbPrint(DC_FILTER, DL_ERROR, ("INetSwift disabled\n"));
	}

	ntStatus = OpenFile(pwstrUniNameIdx, &gFidBoxFileIndex.hFile, GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE, 
		0, FILE_OPEN_IF, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN, FILE_SYNCHRONOUS_IO_NONALERT, TRUE);

	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = OpenFile(pwstrUniNameDat, &gFidBoxFileData.hFile, GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE, 
			0, FILE_OPEN_IF, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN, FILE_SYNCHRONOUS_IO_NONALERT, TRUE);

		if (NT_SUCCESS(ntStatus))
		{
			PFILE_OBJECT pfoIndex = 0;
			PFILE_OBJECT pfoDat = 0;

			ntStatus = ObReferenceObjectByHandle(gFidBoxFileIndex.hFile, FILE_READ_DATA | FILE_WRITE_DATA, NULL, KernelMode, (PVOID*) &pfoIndex, NULL);
			if (NT_SUCCESS(ntStatus))
			{
				ntStatus = ObReferenceObjectByHandle(gFidBoxFileData.hFile, FILE_READ_DATA | FILE_WRITE_DATA, NULL, KernelMode, (PVOID*) &pfoDat, NULL);
				if (!NT_SUCCESS(ntStatus))
				{
					ObDereferenceObject(pfoIndex);
				}
			}

			if (NT_SUCCESS(ntStatus))
			{
				Fid_InitHeap(&gFidBoxHeap, 0);
				Fid_InitFile(&gFidBoxFileIndex, pfoIndex);
				Fid_InitFile(&gFidBoxFileData, pfoDat);

				if (ERROR_SUCCESS == OBJID_Init(&gFidBoxHeap, &gFidBoxFileData, &gFidBoxFileIndex, 0, &gpFidBox, &gFidBoxItemSize))
				{
					DbPrint(DC_SYS, DL_IMPORTANT, ("FidBox_Init success\n"));
					//ExFreePool(pwstrUniNameIdx);
					//ExFreePool(pwstrUniNameDat);

					return;
				}
			}
		}
	}

	if (gFidBoxFileIndex.hFile)
	{
		ZwClose(gFidBoxFileIndex.hFile);
		gFidBoxFileIndex.hFile = NULL;
	}
	
	if (gFidBoxFileData.hFile)
	{
		ZwClose(gFidBoxFileData.hFile);
		gFidBoxFileData.hFile = NULL;
	}

//	ExFreePool(pwstrUniNameIdx);
//	ExFreePool(pwstrUniNameDat);
}

void FidBox_InitImp()
{
	if (1381 == *NtBuildNumber) // nt4
		return;

	if (IsISwiftDisabled())
		gbDisableInSession = TRUE;

	if (gbDisableInSession)
		return;

	if (!gbAllowFidBox)
	{
		DbPrint(DC_SYS, DL_NOTIFY, ("FidBox_InitImp - volume locked\n"));
		return;
	}

	if (!_pfZwFsControlFile)
		return;

	if (gbFidBox_InExit)
		return;

	AddInvisibleThread((ULONG) PsGetCurrentThreadId());

	FidBox_InitImpInternal();

	DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);
}

// Gruzdev -------------------------------------------------------------------------------------
void FidBox2_InitImp()
{
	NTSTATUS ntStatus;

	PWSTR pwstrUniNameIdx = L"\\SystemRoot\\system32\\drivers\\fidbox2.idx";
	PWSTR pwstrUniNameDat = L"\\SystemRoot\\system32\\drivers\\fidbox2.dat";

	if (1381 == *NtBuildNumber) // nt4
		return;

	if (gbDisableInSession)
		return;

	if (!gbAllowFidBox)
	{
		DbPrint(DC_SYS, DL_NOTIFY, ("FidBox_InitImp - volume locked\n"));
		return;
	}

	if (!_pfZwFsControlFile)
		return;

	if (gbFidBox2_InExit)
		return;

	DbPrint(DC_SYS, DL_IMPORTANT, ("FidBox2_InitImp\n"));

	AddInvisibleThread((ULONG) PsGetCurrentThreadId());

/*	pwstrUniNameIdx = FdGetRealFName(L"\\SystemRoot\\fidbox2.idx", L"fidbox2.idx");
	pwstrUniNameDat = FdGetRealFName(L"\\SystemRoot\\fidbox2.dat", L"fidbox2.dat");

	if (!pwstrUniNameIdx || !pwstrUniNameDat)
	{
		DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);
		if (pwstrUniNameDat)
			ExFreePool(pwstrUniNameDat);
		if (pwstrUniNameIdx)
			ExFreePool(pwstrUniNameIdx);
		return;
	}*/

	ntStatus = OpenFile(pwstrUniNameIdx, &gFidBox2FileIndex.hFile, GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE, 
		0, FILE_OPEN_IF, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN, FILE_SYNCHRONOUS_IO_NONALERT, TRUE);

	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = OpenFile(pwstrUniNameDat, &gFidBox2FileData.hFile, GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE, 
			0, FILE_OPEN_IF, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN, FILE_SYNCHRONOUS_IO_NONALERT, TRUE);

		if (NT_SUCCESS(ntStatus))
		{
			PFILE_OBJECT pfoIndex = 0;
			PFILE_OBJECT pfoDat = 0;

			ntStatus = ObReferenceObjectByHandle(gFidBox2FileIndex.hFile, FILE_READ_DATA | FILE_WRITE_DATA, NULL, KernelMode, (PVOID*) &pfoIndex, NULL);
			if (NT_SUCCESS(ntStatus))
			{
				ntStatus = ObReferenceObjectByHandle(gFidBox2FileData.hFile, FILE_READ_DATA | FILE_WRITE_DATA, NULL, KernelMode, (PVOID*) &pfoDat, NULL);
				if (!NT_SUCCESS(ntStatus))
				{
					ObDereferenceObject(pfoIndex);
				}
			}

			if (NT_SUCCESS(ntStatus))
			{
				Fid_InitHeap(&gFidBox2Heap, 0);
				Fid_InitFile(&gFidBox2FileIndex, pfoIndex);
				Fid_InitFile(&gFidBox2FileData, pfoDat);

				if (ERROR_SUCCESS == OBJID_Init(&gFidBox2Heap, &gFidBox2FileData, &gFidBox2FileIndex, _FIDBOX2_CLIENT_STRUCT_SIZE, &gpFidBox2, &gFidBox2ItemSize))
				{
					DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);
					DbPrint(DC_SYS, DL_IMPORTANT, ("FidBox2_Init success\n"));

					//ExFreePool(pwstrUniNameIdx);
					//ExFreePool(pwstrUniNameDat);

					return;
				}
			}
		}
	}

	if (gFidBox2FileIndex.hFile)
	{
		ZwClose(gFidBox2FileIndex.hFile);
		gFidBox2FileIndex.hFile = NULL;
	}
	
	if (gFidBox2FileData.hFile)
	{
		ZwClose(gFidBox2FileData.hFile);
		gFidBox2FileData.hFile = NULL;
	}

	DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);

//	ExFreePool(pwstrUniNameIdx);
//	ExFreePool(pwstrUniNameDat);
}

void FidBox_Allow()
{
	if (!gbAllowFidBox)
	{
		DbPrint(DC_SYS, DL_IMPORTANT, ("FidBox_enabled\n"));
	}
	
	gbAllowFidBox = TRUE;

	if (!gpFidBox)
	{
		AcquireResource(&gFidBoxLock, TRUE);

		if(!gpFidBox)
			FidBox_InitImp();

		ReleaseResource(&gFidBoxLock);
	}

	// second box
	if (!gpFidBox2)
	{
		AcquireResource(&gFidBox2Lock, TRUE);

		if(!gpFidBox2)
			FidBox2_InitImp();

		ReleaseResource(&gFidBox2Lock);
	}
}

//----------------------------------------------------------------------------------------------

void
FidBox_GlobalInit()
{
	ExInitializeResourceLite(&gFidBoxLock);
	
	FidBox_InitImp();
// Gruzdev -------------------------------------------------------------------------------------
	ExInitializeResourceLite(&gFidBox2Lock);
	FidBox2_InitImp();
//----------------------------------------------------------------------------------------------
}

void
FidBox_GlobalDone()
{
	AddInvisibleThread((ULONG) PsGetCurrentThreadId());
	
	DbPrint(DC_SYS, DL_IMPORTANT, ("FidBox_GlobalDone\n"));
	AcquireResource(&gFidBoxLock, TRUE);
	if (gpFidBox)
	{
		gbFidBox_InExit = TRUE;
		OBJID_Done(gpFidBox, OBJID_FLAG_FLUSH);
		gpFidBox = NULL;
		gbFidBox_InExit = FALSE;
	}

	ReleaseResource(&gFidBoxLock);

// Gruzdev -------------------------------------------------------------------------------------
	AcquireResource(&gFidBox2Lock, TRUE);
	if (gpFidBox2)
	{
		gbFidBox2_InExit = TRUE;
		OBJID_Done(gpFidBox2, OBJID_FLAG_FLUSH);
		gpFidBox2 = NULL;
		gbFidBox2_InExit = FALSE;
	}

	ReleaseResource(&gFidBox2Lock);
//----------------------------------------------------------------------------------------------

	DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);

	//ExDeleteResourceLite(&gFidBoxLock);
}

#define FSCTL_CREATE_OR_GET_OBJECT_ID   CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 48, METHOD_BUFFERED, FILE_ANY_ACCESS) // , FILE_OBJECTID_BUFFER
//#define FSCTL_GET_OBJECT_ID             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 39, METHOD_BUFFERED, FILE_ANY_ACCESS) // , FILE_OBJECTID_BUFFER

//	DeviceIoControl(hFile, FSCTL_FILESYSTEM_GET_STATISTICS, NULL, 0, &fs, sizeof(fs), &dwBytesRet, NULL);

BOOLEAN IsNTFS(PFILE_OBJECT pFileObject)
{
	BOOLEAN bRet = FALSE;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PIRP                irp;
	KEVENT              event;
	IO_STATUS_BLOCK		UserIosb;
	PDEVICE_OBJECT		pDevice;
	FILESYSTEM_STATISTICS* pFStat;

	if (STATUS_WRONG_VOLUME == pFileObject->FinalStatus)
		return FALSE;

	pFStat = ExAllocatePoolWithTag(PagedPool, sizeof(FILESYSTEM_STATISTICS), 'tbos');
	if (!pFStat)
		return FALSE;

	pDevice = IoGetRelatedDeviceObject(pFileObject);

	irp = IoAllocateIrp(pDevice->StackSize, FALSE);
	if (irp)
	{
		PIO_STACK_LOCATION  irpSp;

		KeInitializeEvent(&event, NotificationEvent, FALSE);

		irp->Flags = IRP_SYNCHRONOUS_API;
		irp->Tail.Overlay.Thread = PsGetCurrentThread();
		irp->Tail.Overlay.OriginalFileObject = pFileObject;
		irp->RequestorMode = KernelMode;
		irp->UserEvent = &event;
		irp->UserIosb = &UserIosb;
		irp->AssociatedIrp.SystemBuffer = pFStat;

		irpSp = IoGetNextIrpStackLocation(irp);
		irpSp->MajorFunction = IRP_MJ_FILE_SYSTEM_CONTROL;
		irpSp->MinorFunction = IRP_MN_USER_FS_REQUEST;
		irpSp->Parameters.FileSystemControl.FsControlCode = FSCTL_FILESYSTEM_GET_STATISTICS;
		irpSp->DeviceObject = pDevice;
		irpSp->FileObject = pFileObject;

		irpSp->Parameters.FileSystemControl.OutputBufferLength = sizeof(FILESYSTEM_STATISTICS);

		IoSetCompletionRoutine(irp, irpCompletion, 0, TRUE, TRUE, TRUE);

		IoCallDriver(pDevice, irp);
		KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

		ntStatus = UserIosb.Status;
		if (NT_SUCCESS(ntStatus) || (UserIosb.Status == STATUS_BUFFER_OVERFLOW && UserIosb.Information >= sizeof(FILESYSTEM_STATISTICS)))
		{
			if (FILESYSTEM_STATISTICS_TYPE_NTFS == pFStat->FileSystemType)
				bRet = TRUE;
		}
	}

	ExFreePool(pFStat);

	return bRet;
}


NTSTATUS FixBox_GetObjectId(HANDLE hFile, PFIDBOX_REQUEST_DATA_EX pFidData)
{
	NTSTATUS				ntStatus;
	IO_STATUS_BLOCK			ioStatus;
	FILE_OBJECTID_BUFFER	oid;

	memset(&oid, 0, sizeof(FILE_OBJECTID_BUFFER));

	ntStatus = _pfZwFsControlFile(hFile, NULL, NULL, NULL, &ioStatus, FSCTL_CREATE_OR_GET_OBJECT_ID, NULL, 0, &oid, sizeof(FILE_OBJECTID_BUFFER));
	if (NT_SUCCESS(ntStatus))
	{
		pFidData->m_DataFormatId = 0;
		memcpy(pFidData->m_VolumeID, oid.BirthVolumeId, sizeof(pFidData->m_VolumeID));
		memcpy(pFidData->m_FileID, oid.ObjectId, sizeof(pFidData->m_FileID));
	}

	return ntStatus;
}

NTSTATUS FixBox_GetObjectIdByFO(PFILE_OBJECT pFileObject, PFIDBOX_REQUEST_DATA_EX pFidData)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PIRP                irp;
	KEVENT              event;
	IO_STATUS_BLOCK		UserIosb;
	PDEVICE_OBJECT		pDevice;
	FILE_OBJECTID_BUFFER* pFileId;

	if (STATUS_WRONG_VOLUME == pFileObject->FinalStatus)
		return STATUS_UNSUCCESSFUL;

	pFileId = ExAllocatePoolWithTag(PagedPool, sizeof(FILE_OBJECTID_BUFFER), 'tbos');
	if (!pFileId)
		return STATUS_NO_MEMORY;

	pDevice = IoGetRelatedDeviceObject(pFileObject);

	irp = IoAllocateIrp(pDevice->StackSize, FALSE);
	if (irp)
	{
		PIO_STACK_LOCATION  irpSp;

		KeInitializeEvent(&event, NotificationEvent, FALSE);

		irp->Flags = IRP_SYNCHRONOUS_API;
		irp->Tail.Overlay.Thread = PsGetCurrentThread();
		irp->Tail.Overlay.OriginalFileObject = pFileObject;
		irp->RequestorMode = KernelMode;
		irp->UserEvent = &event;
		irp->UserIosb = &UserIosb;
		irp->AssociatedIrp.SystemBuffer = pFileId;

		irpSp = IoGetNextIrpStackLocation(irp);
		irpSp->MajorFunction = IRP_MJ_FILE_SYSTEM_CONTROL;
		irpSp->MinorFunction = IRP_MN_USER_FS_REQUEST;
		irpSp->Parameters.FileSystemControl.FsControlCode = FSCTL_CREATE_OR_GET_OBJECT_ID;
		irpSp->DeviceObject = pDevice;
		irpSp->FileObject = pFileObject;

		irpSp->Parameters.FileSystemControl.OutputBufferLength = sizeof(FILE_OBJECTID_BUFFER);

		IoSetCompletionRoutine(irp, irpCompletion, 0, TRUE, TRUE, TRUE);

		IoCallDriver(pDevice, irp);
		KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

		ntStatus = UserIosb.Status;
		if (NT_SUCCESS(ntStatus) || (UserIosb.Status == STATUS_BUFFER_OVERFLOW && UserIosb.Information >= sizeof(FILE_OBJECTID_BUFFER)))
		{
			pFidData->m_DataFormatId = 0;
			memcpy(pFidData->m_VolumeID, pFileId->BirthVolumeId, sizeof(pFidData->m_VolumeID));
			memcpy(pFidData->m_FileID, pFileId->ObjectId, sizeof(pFidData->m_FileID));
		}
		else
		{
			ntStatus = STATUS_NOT_SUPPORTED;
		}
	}

	ExFreePool(pFileId);

	return ntStatus;
}

#define QuadAlign(n) (((n) + sizeof(LONGLONG) - 1) & ~(sizeof(LONGLONG) - 1))

NTSTATUS
GetNetworkFidBoxData(PFILE_OBJECT pFileObject, PFIDBOX_REQUEST_DATA_EX pFidData, ULONG FidDataBufferSize)
{
    NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;
	
	PIRP                irp;
	KEVENT              event;
	IO_STATUS_BLOCK		UserIosb;
	PDEVICE_OBJECT		pDevice;
	ULONG				fidboxlen = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0]) + _INETSWIFT_PREFIX_FULL_BLEN + 1 + sizeof(INETSWIFT_DATA_EX) + FidDataBufferSize;
	ULONG				eareqbufp_size = QuadAlign(sizeof(FILE_GET_EA_INFORMATION) + _INETSWIFT_PREFIX_FULL_BLEN) + 1;

	PVOID				pNetSwiftBuffer;
	FILE_GET_EA_INFORMATION* pNetSwiftAttr = NULL;

	if (STATUS_WRONG_VOLUME == pFileObject->FinalStatus)
		return FALSE;

	pNetSwiftBuffer = ExAllocatePoolWithTag(NonPagedPool, fidboxlen, 'tbos');
	if (!pNetSwiftBuffer)
		return STATUS_NOT_SUPPORTED;

	pNetSwiftAttr = ExAllocatePoolWithTag(PagedPool, eareqbufp_size, 'tbos');
	if (pNetSwiftAttr)
	{
		memset(pNetSwiftAttr, 0, eareqbufp_size);

		pDevice = IoGetRelatedDeviceObject(pFileObject);
		
		if (NT_SUCCESS(ObReferenceObjectByPointer(pDevice, STANDARD_RIGHTS_REQUIRED, *IoDeviceObjectType, KernelMode)))
		{
			irp = IoAllocateIrp(pDevice->StackSize, FALSE);
			if (irp)
			{
				PMDL pMdl;
				pMdl = IoAllocateMdl(pNetSwiftBuffer, fidboxlen, FALSE, FALSE, irp);
				if (!pMdl)
					IoFreeIrp(irp);
				else
				{
					BYTE timf;
					PIO_STACK_LOCATION  irpSp;

					MmBuildMdlForNonPagedPool(pMdl);

					irp->MdlAddress = pMdl;

					KeInitializeEvent(&event, NotificationEvent, FALSE);

					irp->Flags = IRP_BUFFERED_IO;
					irp->Tail.Overlay.Thread = PsGetCurrentThread();
					irp->Tail.Overlay.OriginalFileObject = pFileObject;
					irp->RequestorMode = KernelMode;
					irp->UserEvent = &event;
					irp->UserIosb = &UserIosb;
					irp->AssociatedIrp.SystemBuffer = pNetSwiftBuffer;
					irp->UserBuffer = pNetSwiftBuffer;

					irpSp = IoGetNextIrpStackLocation(irp);
					irpSp->MajorFunction = IRP_MJ_QUERY_EA;
					irpSp->DeviceObject = pDevice;
					irpSp->FileObject = pFileObject;

					irpSp->Flags = SL_RESTART_SCAN | SL_RETURN_SINGLE_ENTRY;

					irpSp->Parameters.QueryEa.EaList = pNetSwiftAttr;
					irpSp->Parameters.QueryEa.EaListLength = eareqbufp_size;
					irpSp->Parameters.QueryEa.Length = fidboxlen;

					pNetSwiftAttr->NextEntryOffset = 0;
					pNetSwiftAttr->EaNameLength = _INETSWIFT_PREFIX_FULL_BLEN;
					memcpy(pNetSwiftAttr->EaName, _INETSWIFT_PREFIX_FULL, pNetSwiftAttr->EaNameLength);

					timf = (BYTE) GetCurTimeFast();
					timf = timf % 20 + 'A';

					pNetSwiftAttr->EaName[pNetSwiftAttr->EaNameLength - 1] = timf;

					IoSetCompletionRoutine(irp, irpCompletion, 0, TRUE, TRUE, TRUE);

					IoCallDriver(pDevice, irp);
					KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

					ntStatus = UserIosb.Status;

					if ((NT_SUCCESS(ntStatus) || (UserIosb.Status == STATUS_BUFFER_OVERFLOW)) && UserIosb.Information >= fidboxlen)
					{
						FILE_FULL_EA_INFORMATION* pInfo = pNetSwiftBuffer;
						if (pInfo->EaNameLength >= _INETSWIFT_PREFIX_BLEN)
						{
							if (!memcmp(pInfo->EaName, _INETSWIFT_PREFIX, _INETSWIFT_PREFIX_BLEN))
							{
								bool bReady = false;
								PINETSWIFT_DATA_EX pSwiftData = NULL;
								ULONG ealen = 0;

								ealen = FIELD_OFFSET( FILE_FULL_EA_INFORMATION, EaName[0]) + pInfo->EaNameLength + 1;
								pSwiftData = (PINETSWIFT_DATA_EX) ((CHAR*) pInfo + ealen);
								_xor_drv_func_ex(pSwiftData, sizeof(INETSWIFT_DATA_EX) + FidDataBufferSize, timf, _xor_dir_back);

								if (_INETSWIFT_TAG == pSwiftData->m_Tag)
								{
									if (_INETSWIFT_VERSION_3 == pSwiftData->m_Version)
									{
										memcpy(pFidData, &pSwiftData->m_Data, sizeof(FIDBOX_REQUEST_DATA_EX) + FidDataBufferSize);
										bReady = true;
									}
								}

								if (!bReady)
									memset(pFidData, 0, sizeof(FIDBOX_REQUEST_DATA_EX) + FidDataBufferSize);
							}
						}
					}
					else
					{
						DbPrint(DC_SYS, DL_ERROR, ("QueryEa - result 0x%x len 0x%x, expected 0x%x\n", 
							ntStatus, UserIosb.Information, fidboxlen));

						memset(pFidData, 0, sizeof(FIDBOX_REQUEST_DATA_EX) + FidDataBufferSize);
						ntStatus = STATUS_SUCCESS;
					}

					IoFreeMdl(pMdl);
				}
			}

			ObDereferenceObject(pDevice);
		}
		
		ExFreePool(pNetSwiftAttr);
	}

	ExFreePool(pNetSwiftBuffer);

	return ntStatus;
}

NTSTATUS
FidBox_GetDataByFO(PFILE_OBJECT pFileObject, PFIDBOX_REQUEST_DATA_EX pFidData, ULONG* pFidDataBufferSize)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

	if (*pFidDataBufferSize < gFidBoxItemSize)
		return STATUS_NOT_SUPPORTED;

	if (!IsNTFS(pFileObject))
		return STATUS_NOT_SUPPORTED;

	ntStatus = FixBox_GetObjectIdByFO(pFileObject, pFidData);
	if (!NT_SUCCESS(ntStatus))
		return STATUS_NOT_SUPPORTED;

	ntStatus = STATUS_SUCCESS; // will return zeroed memory if fail

	AcquireResource(&gFidBoxLock, FALSE);

	if (gpFidBox)
	{
		OBJID_GetObjectData(gpFidBox, pFidData->m_VolumeID, pFidData->m_FileID, pFidData->m_Buffer, gFidBoxItemSize, NULL);
		*pFidDataBufferSize = gFidBoxItemSize;
	}

	ReleaseResource(&gFidBoxLock);

	return ntStatus;
}

NTSTATUS
FidBox_GetData (
	PFIDBOX_REQUEST_GET pFidGet,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	ULONG* pRetSize
	)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PFILE_OBJECT pFileObject;
	
	if(!gpFidBox)
		return STATUS_NOT_SUPPORTED;

	*pRetSize = 0;

	if (!pFidGet->m_hFile || (pFidGet->m_hFile == (ULONG) -1))
		return STATUS_NOT_SUPPORTED;

	ntStatus = ObReferenceObjectByHandle((HANDLE) pFidGet->m_hFile, SYNCHRONIZE, *IoFileObjectType, KernelMode, (PVOID*) &pFileObject, NULL);
	if (!NT_SUCCESS(ntStatus))
		return STATUS_NOT_FOUND;
	
	if (pFileObject->DeviceObject)
	{
		if (FILE_DEVICE_NETWORK_FILE_SYSTEM == pFileObject->DeviceObject->DeviceType)
		{
			if (gbINetSwitftDisabled)
				ntStatus = STATUS_NOT_SUPPORTED;
			else
				ntStatus = GetNetworkFidBoxData(pFileObject, pFidData, FidDataBufferSize);

			ObDereferenceObject(pFileObject);

			if (NT_SUCCESS(ntStatus))
				*pRetSize = sizeof(FIDBOX_REQUEST_DATA);
			else
				*pRetSize = 0;
		
			return ntStatus;
		}
	}
	
	if (IsNTFS(pFileObject))
		ntStatus = FixBox_GetObjectId((HANDLE) pFidGet->m_hFile, pFidData);
	else
		ntStatus = STATUS_NOT_SUPPORTED;

	ObDereferenceObject(pFileObject);
	
	if (!NT_SUCCESS(ntStatus))
		return STATUS_NOT_SUPPORTED;

	AcquireResource(&gFidBoxLock, FALSE);

	if (!gpFidBox || gbFidBox_InExit)
	{
		ReleaseResource(&gFidBoxLock)
		return STATUS_NOT_SUPPORTED;
	}

	if(gpFidBox && gFidBoxItemSize)
	{
		if (FidDataBufferSize != gFidBoxItemSize)
		{
			HRESULT hRes;
			
			DbgBreakPoint();

			OBJID_Done( gpFidBox, OBJID_FLAG_DONTCLOSE_FILE );

			hRes = OBJID_Init( &gFidBoxHeap, &gFidBoxFileData, &gFidBoxFileIndex, FidDataBufferSize, &gpFidBox, &gFidBoxItemSize );
			if (hRes)
			{
				FidCloseAllFiles();
				
				gpFidBox = NULL;
			}
		}

		if (gpFidBox && gFidBoxItemSize  == FidDataBufferSize)
		{
			HRESULT hRes = OBJID_GetObjectData(gpFidBox, pFidData->m_VolumeID, pFidData->m_FileID, pFidData->m_Buffer, FidDataBufferSize, NULL);
			*pRetSize = sizeof(FIDBOX_REQUEST_DATA_EX) + FidDataBufferSize;
			if (hRes)
			{
				pFidData->m_DataFormatId = 0;
				memset(pFidData->m_Buffer, 0, FidDataBufferSize);
			}
		}
	}
	
	ReleaseResource(&gFidBoxLock);

	return ntStatus;
}

NTSTATUS
FidBox_SetData (
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize
	)
{
	NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;

	AcquireResource(&gFidBoxLock, TRUE);

	if(!gpFidBox)
	{
		FidBox_InitImp();
	}

	if(gpFidBox && gFidBoxItemSize)
	{
		if (gbFidBox_InExit)
		{
			// in shutdone proc now
		}
		else if (gFidBoxItemSize != FidDataBufferSize)
		{
			DbgBreakPoint();
		}
		else
		{
			HRESULT hRes = OBJID_SetObjectData(gpFidBox, pFidData->m_VolumeID, pFidData->m_FileID, pFidData->m_Buffer, FidDataBufferSize, NULL);
			if (!hRes)
				ntStatus = STATUS_SUCCESS;
		}
	}
	
	ReleaseResource(&gFidBoxLock);

	return ntStatus;
}

// Gruzdev -------------------------------------------------------------------------------------
NTSTATUS
FidBox2_GetData(PFIDBOX_REQUEST_GET pFidGet, PFIDBOX_REQUEST_DATA pFidData, ULONG* pRetSize)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PFILE_OBJECT pFileObject;
	
	*pRetSize = 0;

	if (!pFidGet->m_hFile || (pFidGet->m_hFile == (ULONG) -1))
		return STATUS_NOT_SUPPORTED;

	ntStatus = ObReferenceObjectByHandle((HANDLE) pFidGet->m_hFile, SYNCHRONIZE, *IoFileObjectType, KernelMode, (PVOID*) &pFileObject, NULL);
	if (!NT_SUCCESS(ntStatus))
		return STATUS_NOT_FOUND;
	
	if (IsNTFS(pFileObject))
		ntStatus = FixBox_GetObjectId((HANDLE) pFidGet->m_hFile, (PFIDBOX_REQUEST_DATA_EX) pFidData);
	else
		ntStatus = STATUS_NOT_SUPPORTED;

	ObDereferenceObject(pFileObject);

	if (!NT_SUCCESS(ntStatus))
		return STATUS_NOT_SUPPORTED;

	gbAllowFidBox = TRUE;

	AcquireResource(&gFidBox2Lock, FALSE);

// this code is added to provide correct first-chance running after FidBox_GlobalDone()
	if(!gpFidBox2)
		FidBox2_InitImp();

	if (!gpFidBox2 || gbFidBox2_InExit)
	{
		ReleaseResource(&gFidBox2Lock)
		return STATUS_NOT_SUPPORTED;
	}

	if (gpFidBox2)
	{
		HRESULT hRes = OBJID_GetObjectData(gpFidBox2, pFidData->m_VolumeID, pFidData->m_FileID, pFidData->m_Buffer, _FIDBOX2_CLIENT_STRUCT_SIZE, NULL);
		if (hRes)
		{
			if (hRes == STATUS_NOT_FOUND)
			{
				hRes = 0;
				memset(pFidData->m_Buffer, 0, sizeof(pFidData->m_Buffer));
			}
			else
			{
				//DbgBreakPoint();
			}
		}
		if (!hRes)
		{
			*pRetSize = sizeof(PFIDBOX_REQUEST_DATA);
			ntStatus = STATUS_SUCCESS;
		}
	}
	
	ReleaseResource(&gFidBox2Lock);

	return ntStatus;
}

NTSTATUS
FidBox2_SetData(PFIDBOX_REQUEST_DATA pFidData)
{
	NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;

	if (pFidData->m_DataFormatId)
		return ntStatus;

	AcquireResource(&gFidBox2Lock, TRUE);

	if(!gpFidBox2)
	{
		FidBox2_InitImp();
	}

	if(gpFidBox2)
	{
		if (gbFidBox2_InExit)
		{
			// in shutdone proc now
		}
		else
		{
			HRESULT hRes = OBJID_SetObjectData(gpFidBox2, pFidData->m_VolumeID, pFidData->m_FileID, pFidData->m_Buffer, _FIDBOX2_CLIENT_STRUCT_SIZE, NULL);
			if (!hRes)
				ntStatus = STATUS_SUCCESS;
		}
	}
	
	ReleaseResource(&gFidBox2Lock);

	return ntStatus;
}

//----------------------------------------------------------------------------------------------

NTSTATUS
FidBox_ClearValidFlag(PWCHAR pwchFile, ULONG namelen)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PWCHAR pFullName;
	BOOLEAN bPrefixAlreadyExist;

	ULONG itemsize = gFidBoxItemSize;

	if (!itemsize)
		return STATUS_UNSUCCESSFUL;

	if (!_pfZwFsControlFile)
		return STATUS_NOT_SUPPORTED;

	if (pwchFile == NULL)
		return STATUS_UNSUCCESSFUL;

	if (gbFidBox_InExit)
		return STATUS_UNSUCCESSFUL;

	if (namelen == 0)
		namelen = (wcslen(pwchFile) + 1) * sizeof(WCHAR);

	if (namelen <= 2)
		return STATUS_UNSUCCESSFUL;
	
	if (pwchFile[0] == L'\\' && pwchFile[1] == L'\\')
		return STATUS_NOT_SUPPORTED;

	if(namelen >= 8 && *(ULONG*)pwchFile==0x3f005c && *(ULONG*)(pwchFile+2)==0x5c003f)
		bPrefixAlreadyExist = TRUE;
	else {
		namelen += sizeof(L"\\??\\");
		bPrefixAlreadyExist=FALSE;
	}

	pFullName = ExAllocatePoolWithTag(NonPagedPool, namelen, 'SboS');
	if (pFullName == NULL)
		return STATUS_MEMORY_NOT_ALLOCATED;

	if(!bPrefixAlreadyExist) 
		wcscpy(pFullName, L"\\??\\");
	else
		pFullName[0] = 0;

	wcscat(pFullName, pwchFile);

	{
		UNICODE_STRING		us;
		OBJECT_ATTRIBUTES	objectAttributes;
		IO_STATUS_BLOCK		ioStatus;

		HANDLE hFile = 0;

		RtlInitUnicodeString(&us, pFullName);

		InitializeObjectAttributes(&objectAttributes, &us, OBJ_CASE_INSENSITIVE, NULL, NULL);
	
		AddInvisibleThread((ULONG) PsGetCurrentThreadId());
		
		ntStatus = ZwCreateFile(&hFile, 0, &objectAttributes, &ioStatus, NULL, FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

		if (!NT_SUCCESS(ntStatus))
		{
			//DbgBreakPoint();
		}
		else
		{
			PFIDBOX_REQUEST_DATA_EX pFidData = NULL;

			pFidData = ExAllocatePoolWithTag( PagedPool, sizeof(FIDBOX_REQUEST_DATA_EX) + itemsize, 'BBoS');

			if (pFidData)
			{
				if (NT_SUCCESS(FixBox_GetObjectId(hFile, pFidData)))
				{
					pFidData->m_DataFormatId = 0;
					memset(pFidData->m_Buffer, 0, itemsize);
					
					FidBox_SetData(pFidData, itemsize);
				}
				
				ExFreePool( pFidData );
			}

			ZwClose(hFile);
		}

		DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);
	}

	ExFreePool(pFullName);
	
	return ntStatus;
}

NTSTATUS
FidBox_Disable ()
{
	gbDisableInSession = TRUE;

	FidBox_GlobalDone();

	return STATUS_SUCCESS;
}
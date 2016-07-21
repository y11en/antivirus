#include "specfunc.h"
#include "../debug.h"
#include "../hook/HookSpec.h"

#include "../InvThread.h"

BOOLEAN gbIsSpecCheckingInProcess = FALSE;

#define _specfn  L"\\SystemRoot\\klif.spi"

PERESOURCE gpSPMutex = NULL;			// to protect client's list

typedef struct _tSpecFileBlock
{
	struct _tSpecFileBlock* m_pNext;
	SPECFILEFUNC_FILEBLOCK	m_Data;
}SpecFileBlock, *PSpecFileBlock;

SpecFileBlock* gpSpecFileBlock = NULL;
SpecFileBlock* gpSpecFileBlockLast = NULL;

ULONG CalcFNLen(PSPECFILEFUNC_FILEBLOCK pBlock)
{
	ULONG fnlen = wcslen((PWCHAR) pBlock->m_FileNames) * sizeof(WCHAR);

	if (fnlen)
	{
		if (_sfop_rename == pBlock->m_FileOp)
			fnlen += wcslen((PWCHAR)(pBlock->m_FileNames + fnlen + sizeof(WCHAR))) * sizeof(WCHAR);
	}
	
	if (fnlen)
	{
		fnlen += sizeof(WCHAR);		// add last 0
		if (_sfop_rename == pBlock->m_FileOp)
			fnlen += sizeof(WCHAR);		// add last 0
	}

	return fnlen;
}
//+ ------------------------------------------------------------------------------------------
SpecFile_FuncResult GetNTError(NTSTATUS ioStatus)
{
	switch (ioStatus)
	{
	case STATUS_OBJECT_NAME_NOT_FOUND:
		return _sfopr_file_not_found;
		break;
	case STATUS_ACCESS_DENIED:
		return _sfopr_access_denied;
		break;
	}

	return _sfopr_notok;
}

#define OBJ_KERNEL_HANDLE       0x00000200L

NTSTATUS OpenFile(PWSTR PathName, PHANDLE FileHandle, ACCESS_MASK AMask, ULONG ShareMask, ULONG CreateDisposition, 
				  ULONG FileAttr, ULONG CreateOptions,
				  BOOLEAN bKernel)
{
	NTSTATUS			ntStatus = !STATUS_SUCCESS;
	UNICODE_STRING		LogFileName;
	OBJECT_ATTRIBUTES	objectAttributes;
	IO_STATUS_BLOCK		ioStatus;
	ULONG attr = OBJ_CASE_INSENSITIVE;
	
	if (bKernel)
		attr |= OBJ_KERNEL_HANDLE;
	
	RtlInitUnicodeString(&LogFileName, PathName);
	InitializeObjectAttributes(&objectAttributes, &LogFileName, attr, NULL, NULL);

	ntStatus = ZwCreateFile(FileHandle, AMask, &objectAttributes, &ioStatus, NULL,
		FileAttr, ShareMask, CreateDisposition, CreateOptions, NULL, 0);

	if (!NT_SUCCESS(ntStatus))
	{
		if (objectAttributes.ObjectName->Length > 4)
		{
			if (PathName[1] == L'\\')
			{
				PathName[1] = L'?';
				ntStatus = OpenFile(PathName, FileHandle, AMask, ShareMask, CreateDisposition, FileAttr, CreateOptions, bKernel);
				PathName[1] = L'\\';
			}
		}
	}

	return ntStatus;
}

NTSTATUS DeleteFile(PWSTR FileName)
{
	HANDLE							FileHandle = NULL;
	FILE_DISPOSITION_INFORMATION	Disposition;
	FILE_END_OF_FILE_INFORMATION	EndOfFileInfo;
	IO_STATUS_BLOCK					IoStatusBlock;
	FILE_BASIC_INFORMATION			FileAttributes;
	NTSTATUS						ntStatus = STATUS_UNSUCCESSFUL;

	PWCHAR pFullName = NULL;
	ULONG namelen = 0;
	
	ntStatus = OpenFile(FileName, &FileHandle, DELETE, 0, FILE_OPEN, FILE_ATTRIBUTE_NORMAL, FILE_SYNCHRONOUS_IO_NONALERT, FALSE);
	if (NT_SUCCESS(ntStatus))
	{
		Disposition.DeleteFile = TRUE;
		EndOfFileInfo.EndOfFile.LowPart=0;
		EndOfFileInfo.EndOfFile.HighPart=0;

		ntStatus = ZwQueryInformationFile(FileHandle,&IoStatusBlock, &FileAttributes, sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
		FileAttributes.FileAttributes = FILE_ATTRIBUTE_NORMAL;
		ntStatus = ZwSetInformationFile(FileHandle, &IoStatusBlock, &FileAttributes, sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
		ntStatus = ZwSetInformationFile(FileHandle, &IoStatusBlock, &EndOfFileInfo, sizeof(FILE_DISPOSITION_INFORMATION), FileEndOfFileInformation);
		ntStatus = ZwSetInformationFile(FileHandle, &IoStatusBlock, &Disposition, sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation);
		
		ZwClose(FileHandle);
	}

	return ntStatus;
}

NTSTATUS SetFilePointer(HANDLE FileHandle, PLARGE_INTEGER pFilePos)
{
	FILE_POSITION_INFORMATION	FilePos;
	IO_STATUS_BLOCK				IoStatusBlock;

	FilePos.CurrentByteOffset = *pFilePos;

	return ZwSetInformationFile(FileHandle, &IoStatusBlock, &FilePos, sizeof(FILE_POSITION_INFORMATION), FilePositionInformation);
}

NTSTATUS CopyFileImp(PWCHAR pOld, PWCHAR pNew)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	NTSTATUS ioStatus;

	HANDLE hFileNew;
	HANDLE hFileOld;
	
	#define _copy_buf_size 4096
	BYTE* pBuffer = ExAllocatePoolWithTag(PagedPool, _copy_buf_size, 'cbos');

	if (pBuffer)
	{
		ioStatus = OpenFile(pOld, &hFileOld, GENERIC_READ | SYNCHRONIZE, 0, FILE_OPEN , 
			FILE_ATTRIBUTE_NORMAL, FILE_SYNCHRONOUS_IO_NONALERT, FALSE);

		if (NT_SUCCESS(ioStatus))
		{
			ioStatus = OpenFile(pNew, &hFileNew, GENERIC_WRITE | SYNCHRONIZE, 0, FILE_SUPERSEDE, 
				FILE_ATTRIBUTE_NORMAL, FILE_SYNCHRONOUS_IO_NONALERT, FALSE);

			if (NT_SUCCESS(ioStatus))
			{
				// read and write
				LARGE_INTEGER Offset;
				ULONG uRead;
				ULONG uWrite;

				Offset.QuadPart = 0;
				do
				{
					ioStatus = ReadFile(hFileOld, pBuffer, _copy_buf_size, &Offset, &uRead);
					
					if (STATUS_END_OF_FILE == ioStatus)
						ioStatus = STATUS_SUCCESS;

					if (NT_SUCCESS(ioStatus))
					{
						if (uRead)
						{
							ioStatus = WriteFile(hFileNew, pBuffer, uRead, &Offset, &uWrite);
							if (uWrite != uRead)
								break;
						}
						
						if (_copy_buf_size != uRead)
						{
							ntStatus = STATUS_SUCCESS;
							break;
						}

						Offset.QuadPart += uRead;
					}
				} while(NT_SUCCESS(ioStatus));

				ZwClose(hFileNew);

				if (!NT_SUCCESS(ntStatus))
				{
					DeleteFile(pNew);
				}
			}

			ZwClose(hFileOld);
		}

		ExFreePool(pBuffer);
	}

	return ntStatus;
}

NTSTATUS RenameFile(PWCHAR FileName, PWCHAR pnew)
{
	HANDLE							FileHandle = NULL;
	PFILE_RENAME_INFORMATION		pRename;
	IO_STATUS_BLOCK					IoStatusBlock;
	FILE_BASIC_INFORMATION			FileAttributes;
	NTSTATUS						ntStatus = STATUS_UNSUCCESSFUL;

	ULONG BufferSize;
	
	BufferSize = (wcslen(pnew) + 1) * sizeof(WCHAR) + sizeof(FILE_RENAME_INFORMATION);
	
	pRename = ExAllocatePoolWithTag(NonPagedPool, BufferSize, 'SboS');
	if (pRename)
	{
		ntStatus = OpenFile(FileName, &FileHandle, DELETE | SYNCHRONIZE, FILE_SHARE_READ, FILE_OPEN, 
			FILE_ATTRIBUTE_NORMAL, FILE_SYNCHRONOUS_IO_NONALERT, FALSE);

		if (NT_SUCCESS(ntStatus))
		{
			pRename->ReplaceIfExists = TRUE;
			pRename->RootDirectory = NULL;
			pRename->FileNameLength = wcslen(pnew) * sizeof(WCHAR);
			wcscpy(pRename->FileName, pnew);

			ntStatus = ZwSetInformationFile(FileHandle, &IoStatusBlock, pRename, BufferSize, FileRenameInformation);
			if (!NT_SUCCESS(ntStatus))
			{
				if (pRename->FileNameLength > 4)
				{
					if (pRename->FileName[1] == L'\\')
					{
						pRename->FileName[1] = L'?';
						ntStatus = ZwSetInformationFile(FileHandle, &IoStatusBlock, pRename, BufferSize, FileRenameInformation);
					}
				}
			}

			DbPrint(DC_SYS, DL_IMPORTANT, ("Rename result: %#x from %S to %S\n", ntStatus, FileName, pnew));

			ZwClose(FileHandle);
		}

		ExFreePool(pRename);
	}

	return ntStatus;
}

NTSTATUS
DeleteCompletion (
	PDEVICE_OBJECT pDeviceObject,
	PIRP Irp,
	PKEVENT SynchronizingEvent )
{
	*Irp->UserIosb = Irp->IoStatus;
	KeSetEvent(SynchronizingEvent,IO_NO_INCREMENT,FALSE);
	IoFreeIrp(Irp);
	
	return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
SetDeleteInfo (
	PFILE_OBJECT pFileObject,
	PDEVICE_OBJECT pDeviceObject )
{
	PIRP irp;
	PIO_STACK_LOCATION irpSp;
	IO_STATUS_BLOCK ioStatus;
	KEVENT event; 
	NTSTATUS NtStatus;

	FILE_DISPOSITION_INFORMATION dispinfo;
	
	irp = IoAllocateIrp(pDeviceObject->StackSize, FALSE);
	if(!irp)
		return STATUS_INSUFFICIENT_RESOURCES;

	dispinfo.DeleteFile = TRUE;

	irp->Tail.Overlay.OriginalFileObject = pFileObject;
	irp->Tail.Overlay.Thread = PsGetCurrentThread();
	irp->RequestorMode = KernelMode;
	ioStatus.Status = STATUS_SUCCESS;
	ioStatus.Information = 0;
	irp->UserBuffer = 0;
	irp->UserIosb = &ioStatus;
	irp->UserEvent = NULL; 
	irp->Flags = 0;
	irp->Flags = IRP_SYNCHRONOUS_API;// This flag is set for operations, such as ZwQueryInformationFile and ZwSetInformationFile, that are always synchronous, even when performed on a file object that was opened for asynchronous I/O. 	
	irpSp = IoGetNextIrpStackLocation(irp);
	irpSp->DeviceObject = pDeviceObject;
	irpSp->FileObject = pFileObject;

	irp->AssociatedIrp.SystemBuffer = &dispinfo;	
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	irpSp->Parameters.SetFile.Length = sizeof(dispinfo);

	KeInitializeEvent(&event,SynchronizationEvent,FALSE);
	IoSetCompletionRoutine(irp, (PIO_COMPLETION_ROUTINE) DeleteCompletion, &event, TRUE, TRUE, TRUE);
	NtStatus=IoCallDriver(pDeviceObject, irp);
	
	if(NtStatus==STATUS_PENDING)
		KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);

	return ioStatus.Status;
}


NTSTATUS GetFileSize(HANDLE FileHandle, PULONG FileSize)
{
	FILE_STANDARD_INFORMATION		EndOfFileInfo;
	IO_STATUS_BLOCK					IoStatusBlock;
	NTSTATUS						Ret = STATUS_UNSUCCESSFUL;
	if (FileSize != NULL)
	{
		Ret = ZwQueryInformationFile(FileHandle, &IoStatusBlock, &EndOfFileInfo, sizeof(EndOfFileInfo), FileStandardInformation);
		if (NT_SUCCESS(Ret)) 
			*FileSize = EndOfFileInfo.EndOfFile.LowPart;
	}
	
	return Ret;
}

NTSTATUS ReadFile(HANDLE FileHandle, PVOID Buffer, ULONG ToRead, PLARGE_INTEGER pOffset, PULONG pRead)
{
	NTSTATUS			ntStatus = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK		ioStatus;
	
	if (Buffer != NULL)
		ntStatus = ZwReadFile(FileHandle, NULL, NULL, NULL, &ioStatus, Buffer, ToRead, pOffset, NULL);

	*pRead = ioStatus.Information;

	return ntStatus;
}

NTSTATUS WriteFile(HANDLE FileHandle, PVOID Buffer, ULONG ToWrite, PLARGE_INTEGER pOffset, PULONG pWrite)
{
	NTSTATUS			ntStatus = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK		ioStatus;
	
	if (Buffer != NULL)
		ntStatus = ZwWriteFile(FileHandle, NULL, NULL, NULL, &ioStatus, Buffer, ToWrite, pOffset, NULL);

	*pWrite = ioStatus.Information;

	return ntStatus;
}

//+ ------------------------------------------------------------------------------------------

VOID
SPImpProcessListImp(PULONG pContainerID)
{
	DbPrint(DC_SYS, DL_IMPORTANT, ("SPImpProcessListImp enter\n"));
	
	if (!gpSpecFileBlock)
	{
		DbPrint(DC_SYS, DL_NOTIFY, ("SPImpProcessListImp nothing to do\n"));
		return;
	}

	if (gbIsSpecCheckingInProcess)
	{
		DbPrint(DC_SYS, DL_IMPORTANT, ("SPImpProcessListImp already in\n"));
		return;
	}

	gbIsSpecCheckingInProcess = TRUE;

	AddInvisibleThread((ULONG) PsGetCurrentThreadId());

	AcquireResource(gpSPMutex, TRUE);

	if (gpSpecFileBlock)
	{
		BOOL bTryBlock;
		NTSTATUS ioStatus;

		SpecFileBlock* pBlock;
		pBlock = gpSpecFileBlock;

		while (pBlock )
		{
			bTryBlock = TRUE;
			
			if ((pBlock->m_Data.m_Result == _sfopr_new_request) || ((pContainerID != NULL) && (*pContainerID != pBlock->m_Data.m_ContainerID)))
				bTryBlock = FALSE;

			if (pBlock->m_Data.m_Result == _sfopr_ok)
				bTryBlock = FALSE;

			if (bTryBlock)
			{
				DbgBreakPoint();

				pBlock->m_Data.m_Result = _sfopr_ok;
				switch (pBlock->m_Data.m_FileOp)
				{
				case _sfop_delete:
					{
						ioStatus = DeleteFile((PWCHAR) pBlock->m_Data.m_FileNames);
						DbPrint(DC_SYS, DL_IMPORTANT, ("Delete result: %#x %S\n", ioStatus, pBlock->m_Data.m_FileNames));
						if (!NT_SUCCESS(ioStatus))
							pBlock->m_Data.m_Result = GetNTError(ioStatus);
					}
					break;
				case _sfop_rename:
					{
						PWCHAR pold = (PWCHAR) pBlock->m_Data.m_FileNames;
						PWCHAR pnew = (PWCHAR) (pBlock->m_Data.m_FileNames + (wcslen(pold) + 1) * sizeof(WCHAR));

						//ioStatus = DeleteFile(pnew);

						//DbPrint(DC_SYS, DL_IMPORTANT, ("Delete in rename result: %#x %S\n", ioStatus, pnew));
		
						//ioStatus = RenameFile(pold, pnew);
						//if (!NT_SUCCESS(ioStatus))
						{
							// copy manualy and delete src
							ioStatus = CopyFileImp(pold, pnew);
							if (NT_SUCCESS(ioStatus))
								DeleteFile(pold);
							else
							{
								DbgBreakPoint();
							}
						}

						if (!NT_SUCCESS(ioStatus))
							pBlock->m_Data.m_Result = GetNTError(ioStatus);
					}
					break;
				default:
					{
						DbgBreakPoint();
						pBlock->m_Data.m_Result = _sfopr_file_not_found;
					}
					break;
				}
			}

			pBlock = pBlock->m_pNext;
		}
	}

	ReleaseResource(gpSPMutex);

	DelInvisibleThread((ULONG) PsGetCurrentThreadId(), TRUE);

	gbIsSpecCheckingInProcess = FALSE;
}

void SPImp(PVOID Context)
{
	SPImpProcessListImp(NULL);

	DbPrint(DC_SYS, DL_IMPORTANT, ("SpecialFileProcessingImp finished\n"));

	PsTerminateSystemThread(STATUS_SUCCESS);
}

void SPImpCommon(PVOID Context)
{
	NTSTATUS ioStatus;
	CHAR* pList = NULL;
	HANDLE ToDelList = NULL;

	DbPrint(DC_SYS, DL_IMPORTANT, ("SpecialFileProcessingImp started\n"));

	AddInvisibleThread((ULONG) PsGetCurrentThreadId());

	AcquireResource(gpSPMutex, TRUE);
	{
		BYTE	ActionTaken = 0;
		
		HANDLE hFile = NULL;
		
		ioStatus = OpenFile(_specfn, &hFile, FILE_READ_DATA, FILE_SHARE_READ, FILE_OPEN, FILE_ATTRIBUTE_NORMAL, 
			FILE_SYNCHRONOUS_IO_NONALERT, FALSE);

		if(NT_SUCCESS(ioStatus) && hFile)
		{
			//! read and process
			DWORD bRead;
			ULONG fnlen;
			LARGE_INTEGER Offset;

			Offset.QuadPart = 0;

			while(TRUE)
			{
				ioStatus = ReadFile(hFile, (PVOID) &fnlen, sizeof(fnlen), &Offset, &bRead);
				if (!NT_SUCCESS(ioStatus) || bRead != sizeof(fnlen))
					break;
				else
				{
					_xor_drv_func(&fnlen, sizeof(fnlen), 1);
					if (!fnlen)
					{
						// strange block size
						DbgBreakPoint();
						break;
					}
					else
					{
						SpecFileBlock* pBlock = ExAllocatePoolWithTag(NonPagedPool, sizeof(SpecFileBlock) + fnlen, 'sBOS');
						if (pBlock)
						{
							ioStatus = ReadFile(hFile, (PVOID) pBlock, sizeof(SpecFileBlock) + fnlen, &Offset, &bRead);
							if (!NT_SUCCESS(ioStatus) || (bRead != sizeof(SpecFileBlock) + fnlen))
								ExFreePool(pBlock);
							else
							{
								_xor_drv_func(pBlock, sizeof(SpecFileBlock) + fnlen, 1);
								pBlock->m_pNext = 0;
								pBlock->m_Data.m_Result = _sfopr_notok;

								if (gpSpecFileBlock)
								{
									gpSpecFileBlockLast->m_pNext = pBlock;
									gpSpecFileBlockLast = pBlock;
								}
								else
								{
									gpSpecFileBlock = pBlock;
									gpSpecFileBlockLast = pBlock;
								}

							}
						}
					}

					Offset.QuadPart += sizeof(SpecFileBlock) + fnlen;
				}
			}

			ioStatus = ZwClose(hFile);
		}
	}
	
	ReleaseResource(gpSPMutex);

	ioStatus = DeleteFile(_specfn);
	DelInvisibleThread((ULONG) PsGetCurrentThreadId(), TRUE);

	SPImp(NULL);
}

//+ ------------------------------------------------------------------------------------------

void SpecialFileProcessing()
{
	NTSTATUS	ntStatus;

	gpSPMutex = InitKlgResourse();
	if (gpSPMutex == NULL)
	{
		DbPrint(DC_CLIENT, DL_ERROR, ("Can't init gpSPMutex\n"));
		DbgBreakPoint();
		return;
	}


	{
		HANDLE	hSpecialFileProcessing = 0;
		NTSTATUS ntStatus = PsCreateSystemThread(&hSpecialFileProcessing, THREAD_ALL_ACCESS, NULL, 0, NULL, SPImpCommon, NULL);

		DbPrint(DC_SYS, DL_IMPORTANT, ("starting SpecialFileProcessing (common)\n"));

		if(!NT_SUCCESS(ntStatus))
		{
			DbPrint(DC_SYS, DL_ERROR, ("can't start SpecialFileProcessingImp (common)\n"));
			return;
		}

		DbPrint(DC_SYS, DL_ERROR, ("SpecialFileProcessingImp - request to run (common)\n"));
		
		ZwClose(hSpecialFileProcessing);
	}
}

NTSTATUS SpecFileRequest(PSPECFILEFUNC_REQUEST pSpecRequest, ULONG ReqSize, PVOID OutputBuffer, ULONG OutputBufferLength, ULONG* pOutSize)
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;

	switch(pSpecRequest->m_Request)
	{
	case _sfr_add:
		{
			pSpecRequest->m_Data.m_Result = _sfopr_new_request;
			switch (pSpecRequest->m_Data.m_FileOp)
			{
			case _sfop_delete:
			case _sfop_rename:
				{
					ULONG fnlen = CalcFNLen(&pSpecRequest->m_Data);
					if (fnlen)
					{
						PSpecFileBlock pNewBlock;
						
						pNewBlock = ExAllocatePoolWithTag(NonPagedPool, sizeof(SpecFileBlock) + fnlen, 'SBOS');
						if (!pNewBlock)
							ntStatus = STATUS_INSUFFICIENT_RESOURCES;
						else
						{
							pNewBlock->m_pNext = NULL;
							memcpy(&pNewBlock->m_Data, &pSpecRequest->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen);
							
							AcquireResource(gpSPMutex, TRUE);
							
							if (gpSpecFileBlock)
							{
								gpSpecFileBlockLast->m_pNext = pNewBlock;
								gpSpecFileBlockLast = pNewBlock;
							}
							else
							{
								gpSpecFileBlock = pNewBlock;
								gpSpecFileBlockLast = pNewBlock;
							}
							
							ReleaseResource(gpSPMutex);
							
							ntStatus = STATUS_SUCCESS;
						}
					}
				}
				break;
			default:
				{
					// strange case
					DbgBreakPoint();
				}
				break;
			}
		}
		break;
	case _sfr_get:
		if (OutputBuffer && OutputBufferLength >= sizeof(ULONG))
		{
			SpecFileBlock* pBlock;
			SpecFileBlock* pBlockFound = NULL;
			ULONG idx = 0;
			AcquireResource(gpSPMutex, TRUE);

			pBlock = gpSpecFileBlock;

			while (!pBlockFound && pBlock)
			{
				if (pBlock->m_Data.m_ContainerID == pSpecRequest->m_Data.m_ContainerID)
				{
					if (idx == pSpecRequest->m_Idx)
						pBlockFound = pBlock;

					idx++;
				}
				pBlock = pBlock->m_pNext;
			}

			if (!pBlockFound)
			{
				*pOutSize = 0;
				ntStatus = STATUS_SUCCESS;
			}
			else
			{
				ULONG fnlen = CalcFNLen(&pBlockFound->m_Data);
				
				*(ULONG*) OutputBuffer = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen;
				
				if (OutputBufferLength < sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen)
				{
					*pOutSize = sizeof(ULONG);

					ntStatus = STATUS_BUFFER_TOO_SMALL;
				}
				else
				{
					memcpy(OutputBuffer, &pBlockFound->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen);
					*pOutSize = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen;

					ntStatus = STATUS_SUCCESS;
				}

			}

			ReleaseResource(gpSPMutex);
		}
		break;
	case _sfr_flush:
		if (!gpSpecFileBlock)
			ntStatus = STATUS_SUCCESS;
		else
		{
			ntStatus = STATUS_UNSUCCESSFUL;

			AddInvisibleThread((ULONG) PsGetCurrentThreadId());

			{
				int			BlockProcessed = 0;
				NTSTATUS	ioStatus;
				BYTE		ActionTaken = 0;

				BYTE*	ptr;
				ULONG	fnlen;
				ULONG	block_len;
				LARGE_INTEGER	Offset;
				DWORD   Written;

				HANDLE hFile = NULL;

				Offset.QuadPart = 0;
				
				DbgBreakPoint();
				ioStatus = OpenFile(_specfn, &hFile, GENERIC_WRITE | SYNCHRONIZE, 0, FILE_OPEN_IF, 
					FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, FILE_SYNCHRONOUS_IO_NONALERT, FALSE);

				if(NT_SUCCESS(ioStatus) && hFile)
				{
					SpecFileBlock* pBlock;
					AcquireResource(gpSPMutex, TRUE);

					ntStatus = STATUS_SUCCESS;

					pBlock = gpSpecFileBlock;
					while (pBlock &&  (STATUS_SUCCESS == ntStatus))
					{
						if (_sfopr_new_request == pBlock->m_Data.m_Result)
						{
							BlockProcessed++;

							fnlen = CalcFNLen(&pBlock->m_Data);

							block_len = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen + sizeof(ULONG);
							ptr = ExAllocatePoolWithTag(NonPagedPool, block_len, 'sBOS');
							if (ptr)
							{
								*(ULONG*)ptr = fnlen;
								memcpy(ptr + sizeof(fnlen), &pBlock->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen);
								_xor_drv_func(ptr, block_len, 0);

								ioStatus = WriteFile(hFile, ptr, block_len, &Offset, &Written);
								if (NT_SUCCESS(ioStatus) && Written == block_len)
									Offset.QuadPart += block_len;
								else
									ntStatus = STATUS_UNSUCCESSFUL;

								ExFreePool(ptr);
							}
						}

						pBlock = pBlock->m_pNext;
					}
					ReleaseResource(gpSPMutex);
					ioStatus = ZwClose(hFile);

					if (!BlockProcessed)
						ioStatus = DeleteFile(_specfn);
				}
			}

			DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);			
		}
		break;
	case _sfr_proceed_container:
		SPImpProcessListImp(&pSpecRequest->m_Data.m_ContainerID);
		ntStatus = STATUS_SUCCESS;
		break;
	default:
		{
			// strange case
			DbgBreakPoint();
		}
		break;
	}

	return ntStatus;
}


VOID
SPImpProcessList()
{
	HANDLE	hSpecialFileProcessing = 0;
	NTSTATUS ntStatus = PsCreateSystemThread(&hSpecialFileProcessing, THREAD_ALL_ACCESS, NULL, 0, NULL, SPImp, NULL);

	DbPrint(DC_SYS, DL_IMPORTANT, ("starting SpecialFileProcessing\n"));

	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS, DL_ERROR, ("can't start SpecialFileProcessingImp\n"));
		return;
	}

	DbPrint(DC_SYS, DL_ERROR, ("SpecialFileProcessingImp - request to run\n"));
	
	ZwClose(hSpecialFileProcessing);
}
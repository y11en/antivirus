#include "specfunc.h"
#include "specfunc.tmh"

#define _specfn  L"\\SystemRoot\\klif.spi"

ERESOURCE gpSPMutex;
LONG gALreadyInProgress = 0;

typedef struct _tSpecFileBlock
{
	struct _tSpecFileBlock* m_pNext;
	SPECFILEFUNC_FILEBLOCK	m_Data;
}SpecFileBlock, *PSpecFileBlock;

SpecFileBlock* gpSpecFileBlock = NULL;
SpecFileBlock* gpSpecFileBlockLast = NULL;


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

ULONG
CalcFNLen (
	PSPECFILEFUNC_FILEBLOCK pBlock
	)
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

PMKAV_VOLUME_CONTEXT GetVolumeFromHandle(IN HANDLE hFile, OUT PFILE_OBJECT *p_pFileObject OPTIONAL)
{
	NTSTATUS ntStatus;
	PMKAV_VOLUME_CONTEXT pContext = NULL;

	PFILE_OBJECT pFileObject;

	ntStatus = ObReferenceObjectByHandle(hFile, 0, *IoFileObjectType, KernelMode, &pFileObject, NULL);
	if (NT_SUCCESS(ntStatus))
	{
		PFLT_VOLUME  pVolume;

		ntStatus = FltGetVolumeFromFileObject(Globals.m_Filter, pFileObject, &pVolume);
		if (NT_SUCCESS(ntStatus))
		{
			ntStatus = FltGetVolumeContext(Globals.m_Filter, pVolume, &pContext);
			if (!NT_SUCCESS(ntStatus))
				pContext = NULL;

			FltObjectDereference(pVolume);
		}
		
		ObDereferenceObject(pFileObject);
	}

	if (pContext && p_pFileObject)
		*p_pFileObject = pFileObject;

	return pContext;
}
 
NTSTATUS OpenFile(PWSTR PathName, PHANDLE FileHandle, ACCESS_MASK AMask, ULONG ShareMask, ULONG CreateDisposition, 
				  ULONG FileAttr, ULONG CreateOptions,
				  BOOLEAN bKernel)
{
	NTSTATUS			ntStatus = !STATUS_SUCCESS;
	UNICODE_STRING		LogFileName;
	OBJECT_ATTRIBUTES	objectAttributes;
	IO_STATUS_BLOCK		ioStatus;
	ULONG attr = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	
	RtlInitUnicodeString(&LogFileName, PathName);
	InitializeObjectAttributes(&objectAttributes, &LogFileName, attr, NULL, NULL);

//	ntStatus = FltCreateFileEx(Globals.m_Filter, NULL, FileHandle, NULL, AMask, &objectAttributes, &ioStatus, NULL, FileAttr,
//		ShareMask, CreateDisposition, CreateOptions, NULL, 0, 0); 
	ntStatus = FltCreateFile(Globals.m_Filter, NULL, FileHandle, AMask, &objectAttributes, &ioStatus, NULL, FileAttr,
		ShareMask, CreateDisposition, CreateOptions, NULL, 0, 0); 

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

NTSTATUS ReadFile(HANDLE FileHandle, PVOID Buffer, ULONG ToRead, PLARGE_INTEGER pOffset, PULONG pRead)
{
	NTSTATUS			ntStatus = STATUS_UNSUCCESSFUL;
	PMKAV_VOLUME_CONTEXT pContext;
	PFILE_OBJECT		pFileObject;

	pContext = GetVolumeFromHandle(FileHandle, &pFileObject);
	if (pContext)
	{
		if (pContext->m_pFltInstance && Buffer)
		{
			ULONG cbRead;
			ntStatus = FltReadFile(pContext->m_pFltInstance, pFileObject, pOffset, ToRead, Buffer, 0, &cbRead, NULL, NULL);
			*pRead = cbRead;;
		}

		ReleaseFltContext( &pContext );
	}

	return ntStatus;
}

NTSTATUS WriteFile(HANDLE FileHandle, PVOID Buffer, ULONG ToWrite, PLARGE_INTEGER pOffset, PULONG pWrite)
{
	NTSTATUS			ntStatus = STATUS_UNSUCCESSFUL;
	PMKAV_VOLUME_CONTEXT pContext;
	PFILE_OBJECT		pFileObject;

	pContext = GetVolumeFromHandle(FileHandle, &pFileObject);
	if (pContext)
	{
		if (pContext->m_pFltInstance && Buffer)
		{
			ULONG cbWritten;

			ntStatus = FltWriteFile(pContext->m_pFltInstance, pFileObject, pOffset, ToWrite, Buffer, 0, &cbWritten, NULL, NULL);
			*pWrite = cbWritten;
		}
		
		ReleaseFltContext( &pContext );
	}

	return ntStatus;
}

NTSTATUS DeleteFile(PWSTR FileName)
{
	HANDLE							FileHandle = NULL;
	FILE_DISPOSITION_INFORMATION	Disposition;
	FILE_END_OF_FILE_INFORMATION	EndOfFileInfo;
	FILE_BASIC_INFORMATION			FileAttributes;
	NTSTATUS						ntStatus = STATUS_UNSUCCESSFUL;

	PWCHAR pFullName = NULL;
	ULONG namelen = 0;
	
	ntStatus = OpenFile(FileName, &FileHandle, DELETE, 0, FILE_OPEN, FILE_ATTRIBUTE_NORMAL, FILE_SYNCHRONOUS_IO_NONALERT, FALSE);
	if (NT_SUCCESS(ntStatus))
	{
		PMKAV_VOLUME_CONTEXT pContext;
		PFILE_OBJECT pFileObject;

		Disposition.DeleteFile = TRUE;
		EndOfFileInfo.EndOfFile.LowPart = 0;
		EndOfFileInfo.EndOfFile.HighPart = 0;

		pContext = GetVolumeFromHandle(FileHandle, &pFileObject);
		if (pContext)
		{
			if (pContext->m_pFltInstance)
			{
				ntStatus = FltQueryInformationFile(pContext->m_pFltInstance, pFileObject, &FileAttributes,
					sizeof(FILE_BASIC_INFORMATION),	FileBasicInformation, NULL); 
				
				FileAttributes.FileAttributes = FILE_ATTRIBUTE_NORMAL;
				
				ntStatus = FltSetInformationFile(pContext->m_pFltInstance, pFileObject, &FileAttributes,
					sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
				
				ntStatus = FltSetInformationFile(pContext->m_pFltInstance, pFileObject, &EndOfFileInfo,
					sizeof(FILE_END_OF_FILE_INFORMATION), FileEndOfFileInformation);
				
				ntStatus = FltSetInformationFile(pContext->m_pFltInstance, pFileObject, &Disposition,
					sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation);
			}

			ReleaseFltContext( &pContext );
		}

		FltClose(FileHandle);
	}

	return ntStatus;
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
					{
						ntStatus = STATUS_SUCCESS;
						break;
					}

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

				FltClose(hFileNew);

				if (!NT_SUCCESS(ntStatus))
				{
					DeleteFile(pNew);
				}
			}

			FltClose(hFileOld);
		}

		ExFreePool(pBuffer);
	}

	return ntStatus;
}

NTSTATUS RenameFile(PWCHAR FileName, PWCHAR pnew)
{
	HANDLE							FileHandle = NULL;
	PFILE_RENAME_INFORMATION		pRename;
	NTSTATUS						ntStatus = STATUS_UNSUCCESSFUL;

	ULONG BufferSize;
	
	BufferSize = (wcslen(pnew) + 1) * sizeof(WCHAR) + sizeof(FILE_RENAME_INFORMATION);
	
	pRename = ExAllocatePoolWithTag( NonPagedPool, BufferSize, tag_specfunk2 );
	if (pRename)
	{
		ntStatus = OpenFile(FileName, &FileHandle, DELETE | SYNCHRONIZE, FILE_SHARE_READ, FILE_OPEN, 
			FILE_ATTRIBUTE_NORMAL, FILE_SYNCHRONOUS_IO_NONALERT, FALSE);
		if (NT_SUCCESS(ntStatus))
		{
			PMKAV_VOLUME_CONTEXT pContext;
			PFILE_OBJECT pFileObject;

			pRename->ReplaceIfExists = TRUE;
			pRename->RootDirectory = NULL;
			pRename->FileNameLength = wcslen(pnew) * sizeof(WCHAR);
			RtlStringCbCopyW( pRename->FileName, pRename->FileNameLength, pnew );

			pContext = GetVolumeFromHandle(FileHandle, &pFileObject);
			if (pContext)
			{
				if (pContext->m_pFltInstance)
				{
					ntStatus = FltSetInformationFile(pContext->m_pFltInstance, pFileObject, pRename, BufferSize, FileRenameInformation);
					if (!NT_SUCCESS(ntStatus))
					{
						if (pRename->FileNameLength > 4)
						{
							if (pRename->FileName[1] == L'\\')
							{
								pRename->FileName[1] = L'?';
								ntStatus = FltSetInformationFile(pContext->m_pFltInstance, pFileObject, pRename, BufferSize, FileRenameInformation);
							}
						}
					}
				}
				
				ReleaseFltContext( &pContext );
			}

			FltClose(FileHandle);
		}

		ExFreePool(pRename);
	}

	return ntStatus;
}


VOID
SPImpProcessListImp(PULONG pContainerID)
{
	LONG al = InterlockedIncrement( &gALreadyInProgress );
	
	if (1 != al)
	{
		InterlockedDecrement (&gALreadyInProgress );
		return;
	}

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "SPImpProcessListImp start" );

	AddInvisibleThread( PsGetCurrentThreadId() );

	AcquireResourceExclusive( &gpSPMutex );

	if (gpSpecFileBlock)
	{
		BOOLEAN bTryBlock;
		NTSTATUS ioStatus;

		SpecFileBlock* pBlock = gpSpecFileBlock;

		while (pBlock )
		{
			bTryBlock = TRUE;
			
			if ((pBlock->m_Data.m_Result == _sfopr_new_request) || ((pContainerID != NULL) && (*pContainerID != pBlock->m_Data.m_ContainerID)))
				bTryBlock = FALSE;

			if (pBlock->m_Data.m_Result == _sfopr_ok)
				bTryBlock = FALSE;

			if (bTryBlock)
			{
				pBlock->m_Data.m_Result = _sfopr_ok;
				switch (pBlock->m_Data.m_FileOp)
				{
				case _sfop_delete:
					{
						ioStatus = DeleteFile((PWCHAR) pBlock->m_Data.m_FileNames);
						
						DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "delete file '%S' status 0x%x",
							(PWCHAR) pBlock->m_Data.m_FileNames, ioStatus);

						if (!NT_SUCCESS(ioStatus))
							pBlock->m_Data.m_Result = GetNTError(ioStatus);
					}
					break;
				case _sfop_rename:
					{
						PWCHAR pold = (PWCHAR) pBlock->m_Data.m_FileNames;
						PWCHAR pnew = (PWCHAR) (pBlock->m_Data.m_FileNames + (wcslen(pold) + 1) * sizeof(WCHAR));

						DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "renaming file '%S' to '%S'...",
							pold, pnew);

						//ioStatus = DeleteFile(pnew);
						//ioStatus = RenameFile(pold, pnew);
						//if (!NT_SUCCESS(ioStatus))
						{
							// copy manualy and delete src
							ioStatus = CopyFileImp(pold, pnew);
							if (NT_SUCCESS(ioStatus))
								DeleteFile(pold);
						}

						if (!NT_SUCCESS(ioStatus))
							pBlock->m_Data.m_Result = GetNTError(ioStatus);

						DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "rename file '%S' to '%S' status 0x%x",
							pold, pnew, ioStatus);
					}
					break;
				default:
					{
						_dbg_break_;
						pBlock->m_Data.m_Result = _sfopr_file_not_found;
					}
					break;
				}
			}

			pBlock = pBlock->m_pNext;
		}
	}

	ReleaseResource( &gpSPMutex );

	DelInvisibleThread( PsGetCurrentThreadId(), TRUE );

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "SPImpProcessListImp end" );
}

IO_WORKITEM_ROUTINE SPImpProcessListImp_Wrapper;

VOID
SPImpProcessListImp_Wrapper (
	__in PDEVICE_OBJECT DeviceObject,
	__in PVOID pItem
	)
{
	UNREFERENCED_PARAMETER( DeviceObject );

	SPImpProcessListImp(NULL);

	IoFreeWorkItem( pItem );

	InterlockedDecrement( &Globals.m_WorkItemsInWork );
}


NTSTATUS
SpecFileRequest(
	PSPECFILEFUNC_REQUEST pSpecRequest,
	ULONG ReqSize,
	PVOID OutputBuffer,
	ULONG OutputBufferLength,
	ULONG_PTR* pOutSize
	)
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;

	UNREFERENCED_PARAMETER(ReqSize);

	*pOutSize = 0;

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "SpecFileRequest 0x%x",
		pSpecRequest->m_Request);

	switch(pSpecRequest->m_Request)
	{
	case _sfr_add:
		{
			pSpecRequest->m_Data.m_Result = _sfopr_new_request;
			switch ( pSpecRequest->m_Data.m_FileOp )
			{
			case _sfop_delete:
			case _sfop_rename:
				{
					ULONG fnlen = CalcFNLen( &pSpecRequest->m_Data );
					if (fnlen)
					{
						PSpecFileBlock pNewBlock;
						
						pNewBlock = ExAllocatePoolWithTag( NonPagedPool, sizeof(SpecFileBlock) + fnlen, tag_specfunk1 );
						if (!pNewBlock)
							ntStatus = STATUS_INSUFFICIENT_RESOURCES;
						else
						{
							pNewBlock->m_pNext = NULL;
							memcpy( &pNewBlock->m_Data, &pSpecRequest->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen );
							
							AcquireResourceExclusive( &gpSPMutex );
							
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
							
							ReleaseResource( &gpSPMutex );
							
							ntStatus = STATUS_SUCCESS;
						}
					}
				}
				break;
			default:
				{
					// strange case
					_dbg_break_;
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
			AcquireResourceShared( &gpSPMutex );

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
					memcpy( OutputBuffer, &pBlockFound->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen );
					*pOutSize = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen;

					ntStatus = STATUS_SUCCESS;
				}

			}

			ReleaseResource( &gpSPMutex );
		}
		break;
	
	case _sfr_flush:
		if (!gpSpecFileBlock)
			ntStatus = STATUS_SUCCESS;
		else
		{
			ntStatus = STATUS_UNSUCCESSFUL;

			AddInvisibleThread( PsGetCurrentThreadId() );

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
				
				ioStatus = OpenFile( _specfn, &hFile, GENERIC_WRITE | SYNCHRONIZE, 0, FILE_OPEN_IF, 
					FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, FILE_SYNCHRONOUS_IO_NONALERT, FALSE );

				if(NT_SUCCESS(ioStatus) && hFile)
				{
					SpecFileBlock* pBlock;
					AcquireResourceExclusive( &gpSPMutex );

					ntStatus = STATUS_SUCCESS;

					pBlock = gpSpecFileBlock;
					while (pBlock &&  (STATUS_SUCCESS == ntStatus))
					{
						if (_sfopr_new_request == pBlock->m_Data.m_Result)
						{
							BlockProcessed++;

							fnlen = CalcFNLen(&pBlock->m_Data);

							block_len = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen + sizeof(fnlen);
							ptr = ExAllocatePoolWithTag( NonPagedPool, block_len, tag_specfunk3 );
							if (ptr)
							{
								*(ULONG*)ptr = fnlen;
								memcpy( ptr + sizeof(fnlen), &pBlock->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen );
								_xor_drv_func( ptr + sizeof(fnlen), sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen, 0 );

								ioStatus = WriteFile( hFile, ptr, block_len, &Offset, &Written );
								if (NT_SUCCESS(ioStatus) && Written == block_len)
									Offset.QuadPart += block_len;
								else
									ntStatus = STATUS_UNSUCCESSFUL;

								ExFreePool( ptr );
							}
						}

						pBlock = pBlock->m_pNext;
					}
					
					ReleaseResource( &gpSPMutex );
					ioStatus = FltClose( hFile );

					if (!BlockProcessed)
						ioStatus = DeleteFile( _specfn );
				}
			}

			DelInvisibleThread( PsGetCurrentThreadId(), FALSE );
		}
		break;
	
	case _sfr_proceed_container:
		SPImpProcessListImp( &pSpecRequest->m_Data.m_ContainerID );
		ntStatus = STATUS_SUCCESS;
		break;
	
	default:
		{
			// strange case
			_dbg_break_;
		}
		break;
	}

	InterlockedDecrement (&gALreadyInProgress );

	return ntStatus;
}

void
SpecialFileProcessingReload (
	)
{
	NTSTATUS ioStatus;
	BYTE	ActionTaken = 0;
	HANDLE hFile = NULL;

	AddInvisibleThread( PsGetCurrentThreadId() );
	AcquireResourceExclusive( &gpSPMutex );

	ioStatus = OpenFile( _specfn, &hFile, FILE_READ_DATA, FILE_SHARE_READ, FILE_OPEN, FILE_ATTRIBUTE_NORMAL, 
		FILE_SYNCHRONOUS_IO_NONALERT, FALSE );

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
				Offset.QuadPart += sizeof(fnlen);
				if (!fnlen)
				{
					// strange block size
					_dbg_break_;
					break;
				}
				else
				{
					SpecFileBlock* pBlock = ExAllocatePoolWithTag(NonPagedPool, sizeof(SpecFileBlock) + fnlen, tag_specfunk4 );
					if (pBlock)
					{
						ioStatus = ReadFile(hFile, &pBlock->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen, &Offset, &bRead);
						if (!NT_SUCCESS(ioStatus) || (bRead != sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen))
							ExFreePool(pBlock);
						else
						{
							_xor_drv_func(&pBlock->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen, 1);
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

				Offset.QuadPart += sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen;
			}
		}

		ioStatus = ZwClose(hFile);
	}
	
	ReleaseResource( &gpSPMutex );

	ioStatus = DeleteFile(_specfn);
	DelInvisibleThread( PsGetCurrentThreadId(), TRUE );
}

void
SpecialFileProcessingInit ()
{
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "SpecialFileProcessingInit" );
	ExInitializeResourceLite( &gpSPMutex );

	SpecialFileProcessingReload();
}

void
SpecialFileProcessingDone ()
{
	SpecFileBlock* pBlock;

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "SpecialFileProcessingDone..." );

	AcquireResourceExclusive( &gpSPMutex );
	
	pBlock  = gpSpecFileBlock;

	while (pBlock)
	{
		SpecFileBlock* pBlockTmp = pBlock;
		pBlock = pBlock->m_pNext;

		ExFreePool( pBlockTmp );
	}

	ReleaseResource( &gpSPMutex );

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "SpecialFileProcessingDone" );

	ExDeleteResourceLite( &gpSPMutex );
}

void
SpecialFileProcessingGo ()
{
	PIO_WORKITEM pItem = IoAllocateWorkItem( Globals.m_FilterControlDeviceObject );
	if (!pItem)
		return;

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "SpecialFileProcessingGo" );

	InterlockedIncrement( &Globals.m_WorkItemsInWork );

	IoQueueWorkItem( pItem, SPImpProcessListImp_Wrapper, DelayedWorkQueue, pItem );
}

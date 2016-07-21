#ifdef _AI_MEM_USE
	#define _MemAlloc_Name(_pooltype, _size, _tag) _AIMem_Alloc(g_pAiMemObjectName, _size, _tag);
	#define _MemFree_Name(_pointer) _AIMem_Free(g_pAiMemObjectName, _pointer);

	#define _MemAlloc_Common(_pooltype, _size, _tag) _AIMem_Alloc(g_pAiMem, _size, _tag);
	#define _MemFree_Common(_pointer) _AIMem_Free(g_pAiMem, _pointer);
#else
	#define _MemAlloc_Name(_pooltype, _size, _tag) ExAllocatePoolWithTag(_pooltype, _size, _tag);
	#define _MemFree_Name(_pointer) ExFreePool(_pointer);

	#define _MemAlloc_Common(_pooltype, _size, _tag) ExAllocatePoolWithTag(_pooltype, _size, _tag);
	#define _MemFree_Common(_pointer) ExFreePool(_pointer);
#endif

#pragma message("----------------------- Unrecognized volume processing is ON")


#ifdef __DBG__
//W2k
#ifndef FO_RANDOM_ACCESS
#define FO_RANDOM_ACCESS                0x00100000
#endif
#ifndef FO_FILE_OPEN_CANCELLED
#define FO_FILE_OPEN_CANCELLED          0x00200000
#endif
#ifndef FO_VOLUME_OPEN
#define FO_VOLUME_OPEN                  0x00400000
#endif
//XP
#ifndef FO_FILE_OBJECT_HAS_EXTENSION
#define FO_FILE_OBJECT_HAS_EXTENSION    0x00800000
#endif
#ifndef FO_REMOTE_ORIGIN
#define FO_REMOTE_ORIGIN                0x01000000
#endif


// -----------------------------------------------------------------------------------------
/*VOID DumpFileObjectFlags(PFILE_OBJECT	fileObject)
{
CHAR Buff[1024];
	if(!fileObject)
		return;
	Buff[0]=0;

	if(fileObject->Flags & FO_FILE_OPEN)
		strcat(Buff,"FO_FILE_OPEN ");
	if(fileObject->Flags & FO_SYNCHRONOUS_IO)
		strcat(Buff,"FO_SYNCHRONOUS_IO ");
	if(fileObject->Flags & FO_ALERTABLE_IO)
		strcat(Buff,"FO_ALERTABLE_IO ");
	if(fileObject->Flags & FO_NO_INTERMEDIATE_BUFFERING)
		strcat(Buff,"FO_NO_INTERMEDIATE_BUFFERING ");
	if(fileObject->Flags & FO_WRITE_THROUGH)
		strcat(Buff,"FO_WRITE_THROUGH ");
	if(fileObject->Flags & FO_SEQUENTIAL_ONLY)
		strcat(Buff,"FO_SEQUENTIAL_ONLY ");
	if(fileObject->Flags & FO_CACHE_SUPPORTED)
		strcat(Buff,"FO_CACHE_SUPPORTED ");
	if(fileObject->Flags & FO_NAMED_PIPE)
		strcat(Buff,"FO_NAMED_PIPE ");
	if(fileObject->Flags & FO_STREAM_FILE)
		strcat(Buff,"FO_STREAM_FILE ");
	if(fileObject->Flags & FO_MAILSLOT)
		strcat(Buff,"FO_MAILSLOT ");
	if(fileObject->Flags & FO_GENERATE_AUDIT_ON_CLOSE)
		strcat(Buff,"FO_GENERATE_AUDIT_ON_CLOSE ");
	if(fileObject->Flags & FO_DIRECT_DEVICE_OPEN)
		strcat(Buff,"FO_DIRECT_DEVICE_OPEN ");
	if(fileObject->Flags & FO_FILE_MODIFIED)
		strcat(Buff,"FO_FILE_MODIFIED ");
	if(fileObject->Flags & FO_FILE_SIZE_CHANGED)
		strcat(Buff,"FO_FILE_SIZE_CHANGED ");
	if(fileObject->Flags & FO_CLEANUP_COMPLETE)
		strcat(Buff,"FO_CLEANUP_COMPLETE ");
	if(fileObject->Flags & FO_TEMPORARY_FILE)
		strcat(Buff,"FO_TEMPORARY_FILE ");
	if(fileObject->Flags & FO_DELETE_ON_CLOSE)
		strcat(Buff,"FO_DELETE_ON_CLOSE ");
	if(fileObject->Flags & FO_OPENED_CASE_SENSITIVE)
		strcat(Buff,"FO_OPENED_CASE_SENSITIVE ");
	if(fileObject->Flags & FO_HANDLE_CREATED)
		strcat(Buff,"FO_HANDLE_CREATED ");
	if(fileObject->Flags & FO_FILE_FAST_IO_READ)
		strcat(Buff,"FO_FILE_FAST_IO_READ ");
	if(fileObject->Flags & FO_RANDOM_ACCESS)
		strcat(Buff,"FO_RANDOM_ACCESS ");
	if(fileObject->Flags & FO_FILE_OPEN_CANCELLED)
		strcat(Buff,"FO_FILE_OPEN_CANCELLED ");
	if(fileObject->Flags & FO_VOLUME_OPEN)
		strcat(Buff,"FO_VOLUME_OPEN ");
	if(fileObject->Flags & FO_FILE_OBJECT_HAS_EXTENSION)
		strcat(Buff,"FO_FILE_OBJECT_HAS_EXTENSION ");
	if(fileObject->Flags & FO_REMOTE_ORIGIN)
		strcat(Buff,"FO_REMOTE_ORIGIN ");
	DbPrint(DC_FILE,DL_SPAM,("Fobj %x flags %s\n",fileObject,Buff));
}*/
#endif


BOOLEAN
IsThisKernelFileObject (
	PFILE_OBJECT FileObject
	)
{
	ULONG stLower;
	ULONG stHigh;

	IoGetStackLimits( &stLower, &stHigh );
	if ((stLower <= (ULONG) FileObject) && ((ULONG)FileObject <= stHigh))
		return TRUE;

	return FALSE;
}

NTSTATUS (__stdcall *pIoAttachDeviceToDeviceStackSafe)(IN PDEVICE_OBJECT  SourceDevice,
										 IN PDEVICE_OBJECT  TargetDevice,
										 IN OUT PDEVICE_OBJECT  *AttachedToDeviceObject) = NULL;

NTSTATUS MyAttachToStack(IN PDEVICE_OBJECT SourceDevice, IN PDEVICE_OBJECT TargetDevice, IN OUT PDEVICE_OBJECT *AttachedToDeviceObject)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	if (pIoAttachDeviceToDeviceStackSafe)
	{
		ntStatus = pIoAttachDeviceToDeviceStackSafe(SourceDevice, TargetDevice, AttachedToDeviceObject);
		return ntStatus;
	}
	
	*AttachedToDeviceObject = IoAttachDeviceToDeviceStack(SourceDevice, TargetDevice);
	if (*AttachedToDeviceObject)
		ntStatus = STATUS_SUCCESS;

	return ntStatus;
}

DWORD NtfsStart = 0;
DWORD NtfsSize = 0;

VOID QueryDeviceFlags(PFSHOOK_EXTENSION pHookExtension, PDEVICE_OBJECT pDev)
{
	WCHAR DeviceName[MAXPATHLEN];
	Uwcsncpy(DeviceName, MAXPATHLEN, pDev->DriverObject->DriverName.Buffer, pDev->DriverObject->DriverName.Length);

	if (MatchWithPatternW(L"*\\NTFS", DeviceName, FALSE))
	{
		DbPrint(DC_FILE, DL_SPAM, ("NTFS on Device %S\n", DeviceName));

		NtfsStart = (DWORD) pDev->DriverObject->DriverStart; 
		NtfsSize = (DWORD) pDev->DriverObject->DriverSize; 

		if (!(gDrvFlags & _DRV_FLAG_NO_ISTREAM_SUPPORT))
		{
			pHookExtension->ContextFlags |= _CONTEXT_OBJECT_FLAG_NTFS;
		}

		//!check removables
		if ((pDev->Characteristics & (FILE_REMOTE_DEVICE | FILE_READ_ONLY_DEVICE | FILE_VIRTUAL_VOLUME)))
			pHookExtension->ContextFlags &= ~_CONTEXT_OBJECT_FLAG_NTFS;

		/*if (!(pDev->Characteristics & (FILE_REMOTE_DEVICE | FILE_READ_ONLY_DEVICE | FILE_REMOVABLE_MEDIA | FILE_VIRTUAL_VOLUME)))
		{
			pHookExtension->ContextFlags |= _CONTEXT_OBJECT_FLAG_FIXEDVOLUME;
		}*/
	}
}

/*VOID QueryFileSysInfo(VOID)
{
//!get file system
PFILE_FS_ATTRIBUTE_INFORMATION pfileFsAttributes;
ULONG               fileFsAttributesSize;
UNICODE_STRING      fileNameUnicodeString;
HANDLE              ntFileHandle;   
PFILE_OBJECT		fileObject;
OBJECT_ATTRIBUTES   objectAttributes;
NTSTATUS            ntStatus;
IO_STATUS_BLOCK     ioStatus;

	WCHAR				cDrive[16] = L"\\DosDevices\\A:\\";

	DbgBreakPoint();

	fileFsAttributesSize = sizeof( FILE_FS_ATTRIBUTE_INFORMATION) + MAXPATHLEN;
	pfileFsAttributes = (PFILE_FS_ATTRIBUTE_INFORMATION) ExAllocatePool( NonPagedPool, fileFsAttributesSize);

	if(!pfileFsAttributes)
		return;

	for(cDrive[12] = 'A'; cDrive[12] <= 'Z'; cDrive[12]++)
	{
		RtlInitUnicodeString(&fileNameUnicodeString, cDrive);
		InitializeObjectAttributes(&objectAttributes, &fileNameUnicodeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
		ntStatus = ZwCreateFile(&ntFileHandle, SYNCHRONIZE | FILE_ANY_ACCESS, 
								 &objectAttributes, &ioStatus, NULL, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 
								 FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE, NULL, 0);
		if(NT_SUCCESS(ntStatus))
		{
			ntStatus = ObReferenceObjectByHandle( ntFileHandle, FILE_READ_DATA, NULL, KernelMode, &fileObject, NULL);
			if(NT_SUCCESS(ntStatus))
			{
				fileFsAttributesSize = sizeof( FILE_FS_ATTRIBUTE_INFORMATION) + MAXPATHLEN;
				if (NT_SUCCESS(IoQueryVolumeInformation(fileObject, FileFsAttributeInformation, fileFsAttributesSize, pfileFsAttributes, &fileFsAttributesSize)))
				{
				    if (!memcmp(pfileFsAttributes, L"NTFS", sizeof(L"NTFS") - sizeof(WCHAR)))
					{
						// this is drive with ntfs
						DbgBreakPoint();
					}
				}
			
				ObDereferenceObject( fileObject );
			}

			ZwClose(ntFileHandle);
		}
	}

	ExFreePool(pfileFsAttributes);
}
*/


NTSTATUS QueryFS4infoCompletion(PDEVICE_OBJECT DeviceObject,PIRP Irp,PKEVENT SynchronizingEvent)
{
	*Irp->UserIosb = Irp->IoStatus;
//	DbPrint(DC_FILE,DL_SPAM, ("QueryFS4infoCompletion Status=%x\n", Irp->IoStatus.Status));
	KeSetEvent(SynchronizingEvent,IO_NO_INCREMENT,FALSE);
	IoFreeIrp(Irp);
	return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS QueryFS4info(PFILE_OBJECT FObj,PDEVICE_OBJECT NextDeviceObject,ULONG BuffLen,PVOID InfoBuff,FILE_INFORMATION_CLASS InfoClass)
{
PIRP irp;
PIO_STACK_LOCATION irpSp;
IO_STATUS_BLOCK ioStatus;
KEVENT event; 
NTSTATUS NtStatus;
	irp=IoAllocateIrp(NextDeviceObject->StackSize,FALSE);
	if(irp==NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	irp->Tail.Overlay.OriginalFileObject = FObj;
	irp->Tail.Overlay.Thread = PsGetCurrentThread(); //Set our current thread as the thread for this irp so that the IO Manager always knows which thread to return to if it needs to get back into the context of the thread that originated this irp.
	irp->RequestorMode = KernelMode;//Set that this irp originated from the kernel so that the IO Manager knows that the buffers do not need to be probed.
	ioStatus.Status = STATUS_SUCCESS;
	ioStatus.Information = 0;
	irp->UserBuffer = 0;
	irp->UserIosb = &ioStatus;
	irp->UserEvent = NULL; 
	irp->Flags = 0;
	irp->Flags = IRP_SYNCHRONOUS_API;// This flag is set for operations, such as ZwQueryInformationFile and ZwSetInformationFile, that are always synchronous, even when performed on a file object that was opened for asynchronous I/O. 	
	irp->AssociatedIrp.SystemBuffer = InfoBuff;
	irpSp = IoGetNextIrpStackLocation(irp);
	irpSp->MajorFunction = IRP_MJ_QUERY_INFORMATION;
	irpSp->DeviceObject = NextDeviceObject;
	irpSp->FileObject = FObj;
	irpSp->Parameters.QueryFile.Length = BuffLen;
	irpSp->Parameters.QueryFile.FileInformationClass = InfoClass;
	KeInitializeEvent(&event,SynchronizationEvent,FALSE);
	IoSetCompletionRoutine(irp, (PIO_COMPLETION_ROUTINE) QueryFS4infoCompletion,&event,TRUE,TRUE,TRUE);
	NtStatus=IoCallDriver(NextDeviceObject,irp);
	if(NtStatus==STATUS_PENDING)
		KeWaitForSingleObject(&event,Executive,KernelMode,FALSE,NULL);
	return ioStatus.Status;
}

// Must run at Irql==PASSIVE_LEVEL
// uNameSize in chars!!!
VOID GetFName(PKLG_NAME_CACHE pNameCache, PFILE_OBJECT fileObject,PWCHAR FullName,PWCHAR FName,ULONG *Deep,BOOLEAN *StoreInCache, ULONG uNameSize, BOOLEAN* pbOverBuf) 
{
ULONG Len=0;
	*FName=0;

	if(*Deep!=0)
	{
		if(NameCacheGet(pNameCache, HASH4FOBJ(fileObject), FullName, uNameSize, pbOverBuf))
			return;
		if (*pbOverBuf)
		{
			FullName[0] = 0;
			return;
		}
	}

	if(fileObject && fileObject->FileName.Buffer && fileObject->FileName.Length)
	{
		BOOLEAN bSkipRelated = FALSE;
//!!
		if((ULONG)fileObject->FileName.Buffer<0x80000000) {
			DbPrint(DC_FILE,DL_ERROR, ("!!! Zopa striaslas' fo=%x\n",fileObject));
			DbgBreakPoint();
			return;
		}
		
		if(fileObject->RelatedFileObject)
		{
			LONG cou = 0;
			PWCHAR pwchTmp = fileObject->FileName.Buffer;
			LONG lenwch = fileObject->FileName.Length / sizeof(WCHAR);
			LONG minlen = min(8, lenwch);
			for (; cou < minlen; cou++)
			{
				if (L':' == pwchTmp[cou])
				{
					if ((cou + 1 < lenwch) && (L'\\' == pwchTmp[cou + 1]))
					{
						FullName[0] = 0;
						bSkipRelated = TRUE;
					}

					break;
				}
			}
		}

		if (bSkipRelated)
		{
			PWCHAR pwchTmp = fileObject->FileName.Buffer;
			LONG lentmp = fileObject->FileName.Length / sizeof(WCHAR);
			if (L'\"' == pwchTmp[0])
			{
				if (L'\"' == pwchTmp[lentmp - 1])
				{
					pwchTmp++;
					lentmp -= 2;
				}
			}
			Uwcsncpy(FullName, uNameSize, pwchTmp, lentmp * sizeof(WCHAR));
		}
		else
		{
			if(fileObject->RelatedFileObject && fileObject->FileName.Buffer[0]!=L'\\')
			{
				if((++(*Deep)) < 2) // Only one level deep !!!
				{
					GetFName(pNameCache, fileObject->RelatedFileObject,FullName,FName,Deep,StoreInCache, uNameSize - wcslen(FullName) - 1, pbOverBuf);
					if (*pbOverBuf)
					{
						FullName[0] = 0;
						return;
					}
					else
					{
						Len = wcslen(FullName);
						if (fileObject->FileName.Buffer[0] != L':')
						{
							if(Len && FullName[Len-1]!='\\')
								*(ULONG*)(FullName + Len++) ='\\';
						}
						FName=FullName+Len;
					}		
				}
				else
				{
					wcscpy(FName,L"...\\");
					FName+=(Len=4);
				}
			}

			uNameSize -= Len;

			Uwcsncpy(FName, uNameSize, fileObject->FileName.Buffer, fileObject->FileName.Length);
		}

		if (uNameSize < fileObject->FileName.Length)
			*pbOverBuf = TRUE;
		
		if (!(*pbOverBuf))
			*StoreInCache = TRUE;
	}
	else
	{
		DbPrint(DC_FILE,DL_ERROR, ("!!! GetFName. fileObject=%x FileName.Buffer=%x FileName.Length=%d\n",
			fileObject,fileObject->FileName.Buffer,fileObject->FileName.Length));
	}
}

ULONG GetVolumeName(PFSHOOK_EXTENSION hookExt,PWCHAR fullPathName)
{
	if(hookExt->LogicalDrive!='?') {
		if(hookExt->LogicalDrive=='\\') {
			//network drive
			wcscpy(fullPathName,L"\\");
			return 1;
		} else {
			// +¦·ª¨ ¯°¬·¨ °¢ª¿¬ê¤¨
			fullPathName[0] = (WCHAR) hookExt->LogicalDrive;
			fullPathName[1] = L':';
			fullPathName[2] = 0;
			return 2;
		}
	} else if(hookExt->FileSystem->DeviceType==FILE_DEVICE_NAMED_PIPE) {
		// Tè¦¸¨
		wcscpy(fullPathName,L"PIPE:");
		return 5;
	} else if(hookExt->FileSystem->DeviceType==FILE_DEVICE_NETWORK_FILE_SYSTEM ||
						hookExt->FileSystem->DeviceType==FILE_DEVICE_DFS){
		wcscpy(fullPathName,L"\\");
		return 1;
	} else if(hookExt->FileSystem->DeviceType==FILE_DEVICE_MULTI_UNC_PROVIDER)
	{
		wcscpy(fullPathName,L"\\");
		return 1;
	} else {
		DbgBreakPoint();
		wcscpy(fullPathName,L"?:");
		return 2;
	}
}

PWCHAR NameShift(PWCHAR fullPathName)
{
ULONG pathLen;
	pathLen=wcslen(fullPathName);
	//Shift reparsed E:\\blabla
	if(pathLen>4 && fullPathName[2]=='\\' && fullPathName[3]=='\\')
		wcscpy(fullPathName+3,fullPathName+4);
	//Shift E:\??\E:\blablabla
	if(pathLen>9 && fullPathName[3]=='?' && fullPathName[4]=='?')
		wcscpy(fullPathName,fullPathName+6);
	//Shift network "\\R:\server\share\file" 
	if(pathLen>5 && fullPathName[3]==':' && fullPathName[4]=='\\')
		wcscpy(fullPathName+1,fullPathName+4);
	//Shift network "\\;T:0\server\share"  
	if(pathLen>7 && fullPathName[2]==';' && fullPathName[4]==':' && fullPathName[6]=='\\')
		wcscpy(fullPathName+1,fullPathName+6);
	//Shift network "\\;L:000000000001009e\server\share"
	if(pathLen>22 && fullPathName[2]==';' && fullPathName[4]==':' && fullPathName[21]=='\\')
		wcscpy(fullPathName+1,fullPathName+21);
	//Shift network "?:\;T:0\server\share"
  if(pathLen>8 && fullPathName[3]==';' && fullPathName[5]==':' && fullPathName[7]=='\\') {
		wcscpy(fullPathName+1,fullPathName+7);
		fullPathName[0]='\\';
	}
	//Shift rename \??\something
	if(pathLen > 4 && fullPathName[1]=='?' && fullPathName[2]=='?') {
		//Shift rename \??\UNC\Server\share\file
		if(pathLen > 8 && fullPathName[4]=='U' && fullPathName[5]=='N' && fullPathName[6]=='C' && fullPathName[7]=='\\') {
			wcscpy(fullPathName+1, fullPathName + 7);
		} else {
			//Shift rename \??\E:\blablabla
			wcscpy(fullPathName, fullPathName + 4);
		}
	}
	
	// shift \:\ - network dos file name
	if (pathLen > 3 && fullPathName[0]=='\\' && fullPathName[1]==':' && fullPathName[2] == '\\')
	{
		wcscpy(fullPathName + 1, fullPathName + 2);
	}
	
	return fullPathName;
}

// uNameSize in chars!!!
PWCHAR GetFullName(PFILE_OBJECT fileObject, PFSHOOK_EXTENSION hookExt,PWCHAR fullPathName, 
				   BOOLEAN DirectoryFile, BOOLEAN UseFOFN, ULONG uNameSize, BOOLEAN* pbOverBuf, 
				   BOOLEAN QueryUnsafe,
				   BOOLEAN* pDirectVolumeAccess)
{
ULONG RelatedFObjectsDeep=0;
BOOLEAN StoreInCache=FALSE;
DWORD dwtmp;
ULONG pathLen;
NTSTATUS NtStatus;

	*pbOverBuf = FALSE;

///FILE_INTERNAL_INFORMATION Fii;
//ULONGLONG index;

	if(!fullPathName)
		return NULL;
	*fullPathName = 0;

	if(fileObject==NULL) {
		*pDirectVolumeAccess = TRUE;
		// DVA fileObject==NULL
		if(GetDeviceName(hookExt->FileSystem, fullPathName,uNameSize)) {
			wcscat(fullPathName,_DIRECT_VOLUME_ACCESS_NAME_W);
			//			CacheStore(HASH4FOBJ(fileObject),fullPathName);//FileObject is NULL or ...
		} else {
			wcscpy(fullPathName,L"?:"_DIRECT_VOLUME_ACCESS_NAME_W);
		}
		
		return fullPathName;
	}

	
/*
#ifdef __DBG__ 
	{
		if(fileObject->FileName.Buffer && fileObject->FileName.Length) {
			if(fileObject->FileName.Buffer[0]=='\\' && fileObject->FileName.Buffer[1]=='$') {
				if(NT_SUCCESS(NtStatus=QueryFS4info(fileObject,hookExt->FileSystem,sizeof(Fii),&Fii,FileInternalInformation))) {
					index=Fii.IndexNumber.QuadPart & ~0xF0000000;
					DbPrint(DC_FILE,DL_IMPORTANT, ("File's index %08x%08x %S\n",Fii.IndexNumber.HighPart,Fii.IndexNumber.LowPart,fileObject->FileName.Buffer));
					if(fileObject->FileName.Buffer[2]=='D')
						DbgBreakPoint();
				}
			}
		}
	}
#endif	
*/
	
#ifdef __DBG__
/*
		if(fileObject->Flags & FO_STREAM_FILE) {
			DbPrint(DC_FILE,DL_SPAM, ("Fo %x FO_STREAM_FILE\n",fileObject));
			//æ¯¿¬¦ v°¸¦ ¤¨·¨vvª¨æê ¬ ¹¿¤¿¹ v°¸¦ ¦v¿ê¨¿¹ ª DVA
			DbgBreakPoint();
		}
*/
/*
		if(fileObject->Flags & FO_NAMED_PIPE) {
			DbPrint(DC_FILE,DL_SPAM, ("Fo %x FO_NAMED_PIPE\n",fileObject));
			// T äê°¹ ªè¦¯¿ ª¬¿  ¦¤àê¤¦, ¹¦¤°ê¦è °¬ ¹¦¡¿ê ¬¿è°ê¦  ¦ °¯¿¿ ???
			DbgBreakPoint();
		}
*/
/*		if(fileObject->Flags & FO_MAILSLOT) {
			DbPrint(DC_FILE,DL_SPAM, ("Fo %x FO_MAILSLOT\n",fileObject));
			DbgBreakPoint();
		}*/
#endif
		
	
	if(NameCacheGet(g_pCommon_NameCache, HASH4FOBJ(fileObject),fullPathName, uNameSize, pbOverBuf)) {
		goto funcexit;
	}

/*
	if(fileObject->Flags & (FO_VOLUME_OPEN | FO_DIRECT_DEVICE_OPEN)) {
	}			
*/
	
	if(fileObject->FileName.Buffer) {
		if(UseFOFN) {
			if(fileObject->FileName.Buffer && fileObject->FileName.Length) {
				GetFName(g_pCommon_NameCache, fileObject,fullPathName,fullPathName+GetVolumeName(hookExt,fullPathName),&RelatedFObjectsDeep,&StoreInCache, uNameSize, pbOverBuf);
			} else {
				GetVolumeName(hookExt,fullPathName);
			}
		} else if (fileObject->Flags & FO_CLEANUP_COMPLETE) {
			DbPrint(DC_FILE, DL_SPAM, ("FO_CLEANUP_COMPLETE is setted for FileObject %#x\n", fileObject));
			//DbgBreakPoint();
			return fullPathName;
		} else {
//you must not pend IRP_SYNCHRONOUS_PAGING_IO writes (these are cache flushes and FlushViewOfFile).			
// mogno proverit etot flagok i ne sprashivat imia
			PFILE_NAME_INFORMATION	fileNameInfo;
			ANSI_STRING	fileName;

			if (QueryUnsafe)
			{
				DbPrint(DC_FILE, DL_INFO, ("QueryFS4info unsafe for fobj %x \n", fileObject));
				goto funcexit;
			}

			if (hookExt->ContextFlags & _CONTEXT_OBJECT_FLAG_NTFS)
			{
	#define DirectoryStrLen 22
				if(fileObject->FileName.Length==DirectoryStrLen)// \$Directory
				__try {
					if(memcmp(fileObject->FileName.Buffer,L"\\$Directory",DirectoryStrLen)==0) {
						DbPrint(DC_FILE,DL_INFO, ("FileName.Buffer = \\$Directory QueryFS4info unsafe\n"));
						goto funcexit;
					}
				} __except (EXCEPTION_EXECUTE_HANDLER) {
				}
/*	#define BitmapStrLen 16
				else if(fileObject->FileName.Length==BitmapStrLen)// \$BitMap
				__try {
					if(memcmp(fileObject->FileName.Buffer,L"\\$BitMap",BitmapStrLen)==0) {
	//					DbPrint(DC_FILE,DL_INFO, ("FileName.Buffer = \\$BitMap QueryFS4info unsafe\n"));
						goto funcexit;
					}
				} __except (EXCEPTION_EXECUTE_HANDLER) {
				}*/
	#define ConvertToNonresidentStr L"\\$ConvertToNonresident"
	#define ConvertToNonresidentStrLen (sizeof(ConvertToNonresidentStr) - sizeof(WCHAR))
				else if(fileObject->FileName.Length==ConvertToNonresidentStrLen)// 
				__try {
					if(memcmp(fileObject->FileName.Buffer, ConvertToNonresidentStr, ConvertToNonresidentStrLen)==0) {
						//DbPrint(DC_FILE, DL_INFO, ("FileName.Buffer = \\$ConvertToNonresident QueryFS4info unsafe\n"));
						goto funcexit;
					}
				} __except (EXCEPTION_EXECUTE_HANDLER) {
				}
			}
			// ¨ ¿Õ¿ ¿¬ê¦ "\$ConvertToNonresident" ° ¤¨ ¤¿¹ PGP ª¿-¨¿ê ª¬¿
/*#define MftStrLen 10
			else if(fileObject->FileName.Length==MftStrLen)// \$Mft
			__try {
				if(memcmp(fileObject->FileName.Buffer,L"\\$Mft",MftStrLen)==0) {
//					DbPrint(DC_FILE,DL_INFO, ("FileName.Buffer = \\$Mft QueryFS4info unsafe\n"));
					goto funcexit;
				}
			} __except (EXCEPTION_EXECUTE_HANDLER) {
			}
#define LogStrLen 18
			else if(fileObject->FileName.Length==LogStrLen)// \$LogFile
			__try {
				if(memcmp(fileObject->FileName.Buffer,L"\\$LogFile",LogStrLen)==0) {
//					DbPrint(DC_FILE,DL_INFO, ("FileName.Buffer = \\$LogFile QueryFS4info unsafe\n"));
					goto funcexit;
				}
			} __except (EXCEPTION_EXECUTE_HANDLER) {
			}*/
#define _QUERY_INFO_LEN	4096
			//KeGetCurrentIrql() - ?
			fileNameInfo = (PFILE_NAME_INFORMATION) _MemAlloc_Name(NonPagedPool, _QUERY_INFO_LEN, 'FSeB');
			
			if(fileNameInfo) {
				if(NT_SUCCESS(NtStatus = QueryFS4info(fileObject,hookExt->FileSystem, _QUERY_INFO_LEN, fileNameInfo,FileNameInformation))) {
					dwtmp = GetVolumeName(hookExt,fullPathName);
					uNameSize -= dwtmp;
					Uwcsncpy(fullPathName+dwtmp,uNameSize,fileNameInfo->FileName,fileNameInfo->FileNameLength);
					StoreInCache = TRUE;
				} else {
					DbPrint(DC_FILE,DL_INFO, ("QueryFS4info failed for fobj %x NtStatus=%x\n", fileObject,NtStatus));
					//DbgBreakPoint();
				}
				_MemFree_Name(fileNameInfo);
			}
		}
	} else {
		//¿¬v° ¢¯¿¬¦ ¬êè°¹ ¹¦¡¤¦  ¦ è¦¸¦ª¨ê¦ query
		if(fileObject->DeviceObject) {
			//		if(fileObject->Vpb && fileObject->Vpb->RealDevice) {
			if(GetDeviceName(fileObject->DeviceObject,fullPathName,uNameSize)) {
				//			if(GetDeviceName(fileObject->Vpb->RealDevice,fullPathName,uNameSize)) {
				*pDirectVolumeAccess = TRUE;
				wcscat(fullPathName,_DIRECT_VOLUME_ACCESS_NAME_W);

				return fullPathName;
			}
		}
		*pDirectVolumeAccess = TRUE;
		wcscpy(fullPathName,L"?:"_DIRECT_VOLUME_ACCESS_NAME_W);
		DbPrint(DC_FILE,DL_WARNING, ("!!! GetFullName. fileObject=%x FileName.Buffer=%x FileName.Length=%d Dir=%d UseFOFN=%d\n",
			fileObject,fileObject->FileName.Buffer,fileObject->FileName.Length,DirectoryFile,UseFOFN));
		//DbgBreakPoint();

		return fullPathName;
	}

	pathLen=wcslen(fullPathName);
	if(DirectoryFile)
	{
		if(pathLen && fullPathName[pathLen-1]!='\\')
			*(ULONG*)(fullPathName + pathLen++) ='\\';
	}

	NameShift(fullPathName);
	
	if(StoreInCache && fullPathName[0]!='?'){
		//!! get sid and save to CacheSec and save by FileObject
		if (*pbOverBuf == FALSE)
			NameCacheStore(g_pCommon_NameCache, HASH4FOBJ(fileObject),fullPathName,(wcslen(fullPathName)+1)*sizeof(WCHAR), FALSE, '2boS');
	} else {
		DbPrint(DC_FILE,DL_SPAM, ("StoreInCache=FALSE. Key %x (%S)\n",(ULONG)fileObject,fullPathName));
	}
funcexit:
/*
#ifdef __DBG__ 
	{
		if(wcslen(fullPathName)>5 && fullPathName[2]=='\\' && fullPathName[3]=='$') {
			if(NT_SUCCESS(NtStatus=QueryFS4info(fileObject,hookExt->FileSystem,sizeof(Fii),&Fii,FileInternalInformation))) {
				DbPrint(DC_FILE,DL_IMPORTANT, ("File's index for %S = %x\n",fullPathName,Fii.IndexNumber.QuadPart & ~0xF0000000));
				DbgBreakPoint();
			} else {
				DbPrint(DC_FILE,DL_SPAM, ("Query file's index for %S return status %x\n",fullPathName,NtStatus));
			}
		}
	}
#endif	
*/
	return fullPathName;
}

PWCHAR GetFullNameByID(PFILE_OBJECT fileObject,PFSHOOK_EXTENSION hookExt,PWCHAR fullPathName)
{
PWCHAR pathOffset=fullPathName;
PUCHAR idbuf;
#define OBJECT_ID_KEY_LENGTH 16

	pathOffset+=GetVolumeName(hookExt,fullPathName);

	if(fileObject->FileName.Length==sizeof(LONGLONG)) {
		PLONGLONG fileref;
		fileref=(PLONGLONG)fileObject->FileName.Buffer;
		swprintf(pathOffset,L"<%016I64x>",*fileref);
	} else if((fileObject->FileName.Length==OBJECT_ID_KEY_LENGTH) ||
						(fileObject->FileName.Length==OBJECT_ID_KEY_LENGTH+sizeof(WCHAR))) {
			idbuf=(PUCHAR)&fileObject->FileName.Buffer[0];
			if(fileObject->FileName.Length!=OBJECT_ID_KEY_LENGTH) {
				// skip win32k backslash at start of buffer
				idbuf=(PUCHAR)&fileObject->FileName.Buffer[1];
			}
			swprintf(pathOffset,L"<%08x-%04hx-%04hx-%04hx-%04hx%08x>",
				*(PULONG)&idbuf[0],
				*(PUSHORT)&idbuf[0+4],
				*(PUSHORT)&idbuf[0+4+2],
				*(PUSHORT)&idbuf[0+4+2+2],
				*(PUSHORT)&idbuf[0+4+2+2+2],
				*(PULONG)&idbuf[0+4+2+2+2+2]);
	} else {
		DbPrint(DC_FILE,DL_WARNING, ("GetFullNameByID. <Unknown ID (Len=%u)>",fileObject->FileName.Length));
		DbgBreakPoint();
	}
	return fullPathName;
}

/**********************************************************************************************/
typedef struct _KL_DRIVE_INFO
{
	PDEVICE_OBJECT			RealDiskDevice;
}KL_DRIVE_INFO, *PKL_DRIVE_INFO;

typedef struct _FSDEV_LIST {
	struct _FSDEV_LIST *Next;
	PDEVICE_OBJECT	hookDev;	
	PDRIVER_OBJECT	fsDrv;		// Hooked driver
	PDEVICE_OBJECT	fsDev;		// Hooked device
	
	// -----------------------------------------------------------------------------------------
	KL_DRIVE_INFO	DriveInfo;
} FSDEV_LIST, *PFSDEV_LIST;

PFSDEV_LIST FsDevList=NULL;
FAST_MUTEX	FsDevListMutex;

typedef struct _UNRECOGNIZED_DEV_LIST
{
	struct _UNRECOGNIZED_DEV_LIST *m_pNext;
	__int64 m_Expiration;
	PDEVICE_OBJECT	m_pUnrecognizedDevice;	
} UNRECOGNIZED_DEV_LIST, *PUNRECOGNIZED_DEV_LIST;

PUNRECOGNIZED_DEV_LIST gpUnrecognizedDevices = NULL;
FAST_MUTEX	FsUnrecognizedDevListMutex;

PDEVICE_OBJECT IsFsDevHooked(PDEVICE_OBJECT pDev)
{
	//IoGetDeviceAttachmentBaseRef
	PFSDEV_LIST DevList;
	PDEVICE_OBJECT Att;
	ExAcquireFastMutex(&FsDevListMutex);
	DevList=FsDevList;
	while(DevList)
	{
		if(DevList->fsDev==pDev)
		{
			ExReleaseFastMutex(&FsDevListMutex);
			DbPrint(DC_FILE, DL_NOTIFY, ("IsFsDevHooked found directly. FSd=%x HookD=%x\n",pDev,DevList->hookDev));
			return DevList->hookDev;
		}
		else
		{
			Att=DevList->fsDev;//!! May be invalid!
			while(Att)
			{
				if(Att==pDev)
				{
					ExReleaseFastMutex(&FsDevListMutex);
					DbPrint(DC_FILE, DL_NOTIFY, ("IsFsDevHooked found in attached. FSd=%x HookD=%x\n",pDev,DevList->hookDev));
					
					return DevList->hookDev;
				}
				
				Att=Att->AttachedDevice;
			}
		}
		DevList=DevList->Next;
	}
	ExReleaseFastMutex(&FsDevListMutex);
	return NULL;
}

BOOLEAN IsMyDriver(PDEVICE_OBJECT pDev)
{
	PFSDEV_LIST DevList;
	PDEVICE_OBJECT Att;
	ExAcquireFastMutex(&FsDevListMutex);
	DevList=FsDevList;
	while(DevList)
	{
		if(DevList->hookDev==pDev)
		{
			ExReleaseFastMutex(&FsDevListMutex);
			return TRUE;
		}
		DevList=DevList->Next;
	}
	
	ExReleaseFastMutex(&FsDevListMutex);
	return FALSE;
}

#ifndef FILE_DEVICE_DVD
#define FILE_DEVICE_DVD                 0x00000033
#endif

VOID CheckDriveLetter(PDEVICE_OBJECT DevObj, WCHAR Letter)
{
	PFSDEV_LIST			DevEnt;

	WCHAR drv[3] = L"?:";
	ExAcquireFastMutex(&FsDevListMutex);
	DevEnt=FsDevList;
	
	while(DevEnt) {
		if(
			((
			((PFSHOOK_EXTENSION)(DevEnt->hookDev->DeviceExtension))->FileSystem==DevObj
			|| DevEnt->hookDev==DevObj // it's for NT4
			) || ( // for w2k
			DevObj->Vpb && 
			( 
			((PFSHOOK_EXTENSION)(DevEnt->hookDev->DeviceExtension))->FileSystem==DevObj->Vpb->DeviceObject
			|| DevEnt->hookDev==DevObj->Vpb->DeviceObject
			|| DevEnt->fsDev==DevObj->Vpb->DeviceObject
			))))
		{
			if(DevEnt->hookDev->DeviceType==FILE_DEVICE_NETWORK_FILE_SYSTEM)// ||
//				DevEnt->hookDev->DeviceType==FILE_DEVICE_DFS) //!! T¦¢¹¦¡¤v ºvæ·°! +¤° ¤¿ ªv¬ê¨ªvàæê °¹à ¬¿èª¿è¨ ª FileObject
				((PFSHOOK_EXTENSION)(DevEnt->hookDev->DeviceExtension))->LogicalDrive=L'\\';
			else
			{
				drv[0] = Letter;

#ifdef __DBG__
				if (Letter == L'?')
				{
					DbgBreakPoint();
				}
#endif
				
				if(DevObj->DeviceType == FILE_DEVICE_DISK ||
					DevObj->DeviceType == FILE_DEVICE_CD_ROM ||
					DevObj->DeviceType == FILE_DEVICE_DVD)
					DevEnt->DriveInfo.RealDiskDevice = DevObj->Vpb->RealDevice;
				else
					DevEnt->DriveInfo.RealDiskDevice =NULL;
				
				NameCacheStore(g_pNameCache_DriveLetter, (ULONG) DevObj, drv,sizeof(drv), FALSE, 'LboS');
				((PFSHOOK_EXTENSION)(DevEnt->hookDev->DeviceExtension))->LogicalDrive=Letter;
			}
#ifdef __DBG__
			DbPrint(DC_FILE,DL_INFO, ("%c: %s Hooker=%x Hooked=%x\n",((PFSHOOK_EXTENSION)(DevEnt->hookDev->DeviceExtension))->LogicalDrive,
								DbgGetDevTypeName(DevEnt->hookDev->DeviceType),
								DevEnt->hookDev,((PFSHOOK_EXTENSION)(DevEnt->hookDev->DeviceExtension))->FileSystem));
#endif //__DBG__
			break;
		}
		DevEnt=DevEnt->Next;
	}
#ifdef __DBG__
	if(!DevEnt) 
		DbPrint(DC_FILE,DL_INFO, ("%c: %s. Hooker not found\n",Letter, DbgGetDevTypeName(DevObj->DeviceType)));
#endif //__DBG__
	ExReleaseFastMutex(&FsDevListMutex);
}

VOID FindDriveLetter()
{
	static WCHAR        targetNameBuffer[260];
	UNICODE_STRING      targetNameUnicodeString;
	OBJECT_ATTRIBUTES	ObjAttr;
	UNICODE_STRING		us;
	HANDLE				hLink;
	NTSTATUS			ntStatus;
	WCHAR				cDrive[] = L"A:";
	IO_STATUS_BLOCK		ioStatus;
	PFILE_OBJECT		fileObject;
	PDEVICE_OBJECT		DevObj;

	HANDLE				hDir_Root;

	NameCacheClean(g_pNameCache_DriveLetter);
	
	RtlInitUnicodeString(&us, (PWCHAR)L"\\??");
	InitializeObjectAttributes(&ObjAttr,&us,OBJ_CASE_INSENSITIVE,NULL,NULL);

	ntStatus = ZwOpenDirectoryObject(&hDir_Root,DIRECTORY_QUERY,&ObjAttr);
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_FILE,DL_ERROR, ("ZwOpenDirectoryObject %S failed, status %x\n",us.Buffer, ntStatus));
		return;
	}
	
	for(cDrive[0]='A';cDrive[0]<='Z';cDrive[0]++)
	{
		DevObj = NULL;
		RtlInitUnicodeString(&us,cDrive);
		InitializeObjectAttributes(&ObjAttr,&us,OBJ_CASE_INSENSITIVE, hDir_Root, NULL);
		ntStatus=ZwOpenSymbolicLinkObject(&hLink,SYMBOLIC_LINK_QUERY,&ObjAttr);
		if(NT_SUCCESS(ntStatus)) {
			RtlZeroMemory(targetNameBuffer,sizeof(targetNameBuffer));
			targetNameUnicodeString.Buffer = targetNameBuffer;
			targetNameUnicodeString.MaximumLength = sizeof(targetNameBuffer);
			ntStatus=ZwQuerySymbolicLinkObject(hLink,&targetNameUnicodeString,NULL);
			ZwClose(hLink);
			if(NT_SUCCESS(ntStatus))
			{
				if(!wcsncmp(targetNameBuffer,L"\\Device",7))
				{
					if (!wcsncmp(targetNameBuffer,L"\\Device\\LanmanRedirector\\",25))
					{
						DbgBreakPoint();
					}
					else
					{
						ntStatus=ZwCreateFile(&hLink,SYNCHRONIZE|FILE_ANY_ACCESS,&ObjAttr,&ioStatus,NULL,0,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_OPEN,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
						
						if(NT_SUCCESS(ntStatus))
						{
							ntStatus=ObReferenceObjectByHandle(hLink,FILE_READ_DATA,NULL,KernelMode,(PVOID*) &fileObject,NULL);
							if(NT_SUCCESS(ntStatus))
							{
								// if((DevObj=IoGetRelatedDeviceObject(fileObject))!=NULL) 
								if((DevObj=fileObject->DeviceObject)!=NULL)
								{
									DbPrint(DC_FILE,DL_INFO, ("--- %S Device=%x Vpb=%x\n",us.Buffer,DevObj,DevObj->Vpb));
									CheckDriveLetter(DevObj,cDrive[0]);
								}
								else
								{
									DbPrint(DC_FILE,DL_ERROR, ("IoGetRelatedDeviceObject %S failed \n",us.Buffer));
								}
								ObDereferenceObject(fileObject);
							}
							else
							{
								DbPrint(DC_FILE,DL_ERROR, ("ObReferenceObjectByHandle %S failed status=%x\n",us.Buffer,ntStatus));
							}
							
							ZwClose(hLink);
						}
						else
						{
							DbPrint(DC_FILE,DL_ERROR, ("ZwCreateFile %S failed status=%x\n",us.Buffer,ntStatus));
						}
					}
				}
				else
				{
					DbPrint(DC_FILE,DL_ERROR, ("%S don't refer to \\Device\\..., seems to be SUBST %S\n",us.Buffer,targetNameBuffer));
				}
			} else {
				DbPrint(DC_FILE,DL_ERROR, ("ZwQuerySymbolicLinkObject %S failed status=%x\n",us.Buffer,ntStatus));
			}
		} else {
#ifdef __DBG__			
			if(ntStatus!=STATUS_OBJECT_NAME_NOT_FOUND)
				DbPrint(DC_FILE,DL_ERROR, ("ZwOpenSymbolicLinkObject %S failed, status %x\n",us.Buffer, ntStatus));
#endif
		}
	}
}

/*BOOL
CheckDrvName(PDRIVER_OBJECT pDev)
{
	BOOLEAN bAllow = TRUE;
	NTSTATUS ntStatus;
	POBJECT_NAME_INFORMATION pDeviceInfo;
	pDeviceInfo = (POBJECT_NAME_INFORMATION)ExAllocatePoolWithTag(PagedPool,0x1000,'dBOS');
	if (pDeviceInfo)
	{
		ULONG RetLen;
		memset(pDeviceInfo, 0, 0x1000);

		ntStatus = ObQueryNameString(pDev, pDeviceInfo, 0x1000, &RetLen);
		if (NT_SUCCESS(ntStatus) && pDeviceInfo->Name.Length && pDeviceInfo->Name.Buffer)
		{
			DbPrint(DC_FILE,DL_ERROR, ("CheckDrvName 0x%x %S\n", pDev, pDeviceInfo->Name.Buffer));

			if (pDeviceInfo->Name.Length == sizeof(L"\\FileSystem\\PSG") - sizeof(WCHAR))
			{
				if (!memcmp(pDeviceInfo->Name.Buffer, L"\\FileSystem\\PSG", pDeviceInfo->Name.Length))
				{
					//bAllow = FALSE;
					//DbgBreakPoint();
				}
			}
		}

		ExFreePool(pDeviceInfo);
	}

	return bAllow;
}*/

/* see NTFSD Message ID <LYR1547-5868-2000.08.29-10.56.50--bes#avp.ru@lists.osr.com>
- Drive letters are not required for volumes - this was true on NT 4.0 (try
it in Disk Administrator - you can remove the drive letter assignment!) but
is trivially accomplished on Windows 2000 using the reparse
point/junction/mount point technology present therein;
- Drive letters need not be unique (see the "subst" command for an example
of this) - they are just "symbolic links" and you can have more than one
- The symbolic links from the "\??" directory can vary on a per-process
basis.
*/

NTSTATUS HookFsDev(PDEVICE_OBJECT pDev)
{
PFSDEV_LIST				FsDevEnt=NULL;
NTSTATUS					ntStatus=STATUS_INSUFFICIENT_RESOURCES;
PDEVICE_OBJECT		HookerDevice;
PFSHOOK_EXTENSION	hookExtension;
	if(UnloadInProgress) {
		DbPrint(DC_FILE,DL_WARNING, ("HookFsDev when UnloadInProgress!\n"));
		return STATUS_UNSUCCESSFUL;
	}
	if(pDev == NULL) {
		DbPrint(DC_FILE,DL_WARNING, ("HookFsDev: pDev is NULL...\n"));
		return STATUS_UNSUCCESSFUL;
	}
	
#ifdef __DBG__
	if(pDev->DeviceType==FILE_DEVICE_DISK_FILE_SYSTEM
		|| pDev->DeviceType==FILE_DEVICE_NETWORK_FILE_SYSTEM
		|| pDev->DeviceType==FILE_DEVICE_CD_ROM_FILE_SYSTEM
		)
	{
	}
	else
	{
//		DbgBreakPoint();
		return STATUS_UNSUCCESSFUL;
	}

#endif
	if(IsFsDevHooked(pDev)) {
		DbPrint(DC_FILE,DL_ERROR, ("HookFsDev trying to hook already hooked device %x. Why?\n",pDev));
		return STATUS_UNSUCCESSFUL;
	}
	if(KeGetCurrentIrql()!=PASSIVE_LEVEL) {
		DbPrint(DC_FILE,DL_ERROR, ("HookFsDev trying to hook at IRQL=%x\n",KeGetCurrentIrql()));
		DbgBreakPoint();
		return STATUS_UNSUCCESSFUL;
	}

	{
		PDEVICE_OBJECT ptmpDev = pDev->AttachedDevice;
		while (ptmpDev)
		{
			if (IsMyDriver(ptmpDev))
			{
				DbPrint(DC_FILE,DL_ERROR, ("HookFsDev trying to hook already hooked device %x. Why? (alternate search)\n",pDev));
				return STATUS_UNSUCCESSFUL;
			}
			ptmpDev = ptmpDev->AttachedDevice;
		}
	}

	//if (CheckDrvName(pDev->DriverObject))
		FsDevEnt = ExAllocatePoolWithTag(NonPagedPool,sizeof(FSDEV_LIST),'dSeB');

	if(FsDevEnt) {
		ntStatus=IoCreateDevice(HookerDriverObject,sizeof(FSHOOK_EXTENSION),NULL,pDev->DeviceType,0,FALSE,&HookerDevice);
		if(NT_SUCCESS(ntStatus)){
			hookExtension=(FSHOOK_EXTENSION*)HookerDevice->DeviceExtension;
			hookExtension->ContextFlags = 0;
			
			if (NT_SUCCESS(MyAttachToStack(HookerDevice, pDev, &hookExtension->FileSystem)))
			{
				ObReferenceObjectByPointer(pDev, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode);
			}
			else
			{
				DbgBreakPoint();
				hookExtension->FileSystem = 0;
			}

			if(hookExtension->FileSystem) {
				HookerDevice->StackSize = hookExtension->FileSystem->StackSize + 2;
				DbPrint(DC_FILE, DL_IMPORTANT, ("FS Device %x successfully hooked. Hooker %x \n",pDev,HookerDevice));
				if(pDev->Flags & DO_BUFFERED_IO) {
					HookerDevice->Flags |= DO_BUFFERED_IO;
				}
				if(pDev->Flags & DO_DIRECT_IO) {
					HookerDevice->Flags |= DO_DIRECT_IO;
				}
	//!! æê¦ ª ·¨â¿¬êª¿ ä·¬ ¿è°¹¿¤ê¨ !! "æè¿¬êv" ª ä¬ v¦è¿è¿ °¬â¿¢¤¦ê?
	if(pDev->Characteristics & FILE_REMOTE_DEVICE) {
		HookerDevice->Characteristics |= FILE_REMOTE_DEVICE;
	}
				if(pDev->DeviceType==FILE_DEVICE_NETWORK_FILE_SYSTEM )//||
//					pDev->DeviceType==FILE_DEVICE_DFS) //!! T¦¢¹¦¡¤v ºvæ·°! ¬¹. CheckDriveLetter
				{
					hookExtension->LogicalDrive='\\';
				}
				else
					hookExtension->LogicalDrive='?';

				QueryDeviceFlags(hookExtension, pDev);

				FsDevEnt->fsDev=pDev;
				FsDevEnt->fsDrv=pDev->DriverObject;//=¨ï°º¨ ¦¤¦ ¤¨¯¦?
				FsDevEnt->hookDev=HookerDevice;
				ExAcquireFastMutex(&FsDevListMutex);
				FsDevEnt->Next=FsDevList;
				FsDevList=FsDevEnt;
				ExReleaseFastMutex(&FsDevListMutex);

				DbPrint(DC_FILE, DL_IMPORTANT, ("FS Device 0x%x with flag address 0x%x (val 0x%x)\n",
					HookerDevice, &HookerDevice->Flags, HookerDevice->Flags));

				HookerDevice->Flags &= ~DO_DEVICE_INITIALIZING;

				return STATUS_SUCCESS;
			} else {
				IoDeleteDevice(HookerDevice);
				ntStatus=!STATUS_SUCCESS;
				DbPrint(DC_FILE,DL_ERROR, ("Attach to Device %x failed\n",pDev));
			}
		} else {
			DbPrint(DC_FILE,DL_ERROR, ("IoCreateDevice failed\n"));
		}
	} else {
		DbPrint(DC_FILE,DL_ERROR, ("HookFsDev failed. Low memory.\n"));
	}
	if(FsDevEnt) {
		ExFreePool(FsDevEnt);
	}
	return ntStatus;
}

NTSTATUS UnHookFsDev(PDEVICE_OBJECT SourceDevice, PDEVICE_OBJECT TargetDevice)
{
PFSDEV_LIST	FsDevEnt;
PFSDEV_LIST	prevFsDevEnt=NULL;
	ExAcquireFastMutex(&FsDevListMutex);
	FsDevEnt=FsDevList;
	while(FsDevEnt && FsDevEnt->hookDev!=SourceDevice) {
		prevFsDevEnt=FsDevEnt;
		FsDevEnt=FsDevEnt->Next;
	}
	if(FsDevEnt) {
		if(prevFsDevEnt) {
			prevFsDevEnt->Next=FsDevEnt->Next;
		} else {
			FsDevList=FsDevEnt->Next;
		}
		
		ExReleaseFastMutex(&FsDevListMutex);

		ObDereferenceObject(FsDevEnt->fsDev);
		
		ExFreePool(FsDevEnt);

		IoDetachDevice(TargetDevice);
		IoDeleteDevice(SourceDevice);

		DbPrint(DC_FILE, DL_IMPORTANT, ("FS Device %x unhooked. Hooker %x\n", TargetDevice, SourceDevice));

		return STATUS_SUCCESS;
	}
	DbPrint(DC_FILE, DL_ERROR, ("UnHookFsDev failed, hooker %x not found\n",SourceDevice));
	ExReleaseFastMutex(&FsDevListMutex);
	return !STATUS_SUCCESS;
}

#ifdef UNLOADABLE
VOID UnHookAllFs(VOID)
{
PFSDEV_LIST	FsDevEnt;
	ExAcquireFastMutex(&FsDevListMutex);
	while(FsDevList) {
		FsDevEnt=FsDevList;
		FsDevList=FsDevList->Next;
		ExReleaseFastMutex(&FsDevListMutex);
		DbPrint(DC_FILE, DL_IMPORTANT, ("FS Device %x unhooked. Hooker %x\n", FsDevEnt->fsDev, FsDevEnt->hookDev));
		ObDereferenceObject(FsDevEnt->fsDev);
		IoDetachDevice(FsDevEnt->fsDev);//!!fast mutex raises the irql to APC_LEVEL
		IoDeleteDevice(FsDevEnt->hookDev);
		ExAcquireFastMutex(&FsDevListMutex);
		ExFreePool(FsDevEnt);
	}
	ExReleaseFastMutex(&FsDevListMutex);
}
#endif

VOID FindFSDev(PDRIVER_OBJECT pDrv, PVOID pUserValues)
{
	PDEVICE_OBJECT		pDev,tmp;
	NTSTATUS			refStatus;
	
	BOOLEAN bMatchType;
	
	if(pDrv == HookerDriverObject)
		return;
	
	pDev = pDrv->DeviceObject;
	while(pDev)
	{
		refStatus=ObReferenceObjectByPointer(pDev,STANDARD_RIGHTS_REQUIRED,NULL,KernelMode);
		if(!NT_SUCCESS(refStatus))
		{
			DbPrint(DC_FILE,DL_ERROR, ("FindFSDev ObReferenceObjectByPointer %x failed, status %x\n",pDev, refStatus));
			DbgBreakPoint();
		}
#ifdef __DBG__
		{
			WCHAR DevName[128];
			PWCHAR pDN;
			pDN = GetDeviceName(pDev,DevName, sizeof(DevName) / sizeof(WCHAR));
			DbPrint(DC_FILE, DL_NOTIFY, ("Device=%x (%S) Type=%s Drv=%x (%S)\n", pDev, pDN ? pDN : L"Unnamed",
				DbgGetDevTypeName(pDev->DeviceType), pDrv,pDrv->DriverName.Buffer));
		}
#endif
		bMatchType = FALSE;
		if(pDev->DeviceType==FILE_DEVICE_DISK_FILE_SYSTEM
			|| pDev->DeviceType==FILE_DEVICE_NETWORK_FILE_SYSTEM
			|| pDev->DeviceType==FILE_DEVICE_CD_ROM_FILE_SYSTEM)
		{
			bMatchType = TRUE;
		}

		if (*NtBuildNumber <= 2195 && pDev->DeviceType == FILE_DEVICE_DFS)
			bMatchType = TRUE;

		if (bMatchType)
		{
			if(!IsFsDevHooked(pDev))
				HookFsDev(pDev);
		}

		tmp = pDev;
		pDev = pDev->NextDevice;
		
		if(NT_SUCCESS(refStatus))
			ObDereferenceObject(tmp);
	}
}

BOOLEAN FSHooked = FALSE;

VOID FsNotification(IN PDEVICE_OBJECT DeviceObject,IN BOOLEAN FsActive)
{
	DbPrint(DC_FILE, DL_IMPORTANT, ("FsNotification Dev=%x %s\n",DeviceObject,FsActive ? "Register":"Unregister"));
	if(FSHooked)
	{
		if(FsActive)
		{
			if(KeGetCurrentIrql() == PASSIVE_LEVEL)
			{
				if(!IsFsDevHooked(DeviceObject))
					HookFsDev(DeviceObject);
			}
			else
			{
				DbPrint(DC_FILE, DL_ERROR, ("FsNotification at IRQL %x\n",KeGetCurrentIrql()));
			}
		}
	}
}

BOOLEAN FIOHook(VOID)
{
	NTSTATUS ntStatus;
	if(!FSHooked)
	{
		ExInitializeFastMutex(&FsDevListMutex);
		ExInitializeFastMutex(&FsUnrecognizedDevListMutex);
		if(NT_SUCCESS(EnumDrivers(FindFSDev, NULL)))
		{
			FindDriveLetter();
			FSHooked = TRUE;
		}
		else
			return FALSE;
	}
	return TRUE;
}

BOOLEAN FIOUnHook(VOID)
{
	if(FSHooked)
	{
		#ifdef UNLOADABLE
			UnHookAllFs();
			FSHooked=FALSE;
		#endif
	}
	return TRUE;
}

//+ ------------------------------------------------------------------------------------------
int gFSNeedRescan = 0;

void FS_CheckNewDevices()
{
	if (!FSHooked)
		return;

	if (gFSNeedRescan < 0)
		gFSNeedRescan = 0;
	
	if (0 == gFSNeedRescan)
		return;

	if (gFSNeedRescan == 1)
	{

// Andrey Gruzdev 19.08.05
		
//		if (*NtBuildNumber > 2195)
//		{
//			//skip enum
//		}
//		else
//		{
			// nt + 2k
			DbPrint(DC_FILE, DL_IMPORTANT, ("FS_CheckNewDevices - EnumDrivers\n"));
			EnumDrivers(FindFSDev, NULL);
			FindDriveLetter();
//		}
	}

	gFSNeedRescan--;
}
/**********************************************************************************************/

NTSTATUS FSCreateCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PFILE_OBJECT			fileObject      = currentIrpStack->FileObject;
	BOOLEAN bOverBuf = FALSE;
	
	ULONG CreateCompletitionContext = (ULONG) Context;
	
	if(!NT_SUCCESS(Irp->IoStatus.Status))
		NameCacheFree(g_pCommon_NameCache, HASH4FOBJ(fileObject));
	else
	{
		if (fileObject != NULL)
		{
			KLG_OBJECT_CONTEXT ObjectContext;
	
			PWCHAR FileName = ExAllocatePoolWithTag(PagedPool, 4096, ',bos');
			if(FileName)
			{
				PVOID pbuftmp = NameCacheGet(g_pCommon_NameCache, HASH4FOBJ(fileObject), FileName, 4096, &bOverBuf);
				if (pbuftmp == NULL || bOverBuf)
				{
					ExFreePool(FileName);
					FileName = NULL;
				}
			}

			if (FileName)
				DbPrint(DC_FILE,DL_SPAM, ("CREATE Compl FO=%x Status=%x (%s)\n",fileObject,Irp->IoStatus.Status, FileName));

			bOverBuf = FALSE;
			{
				ULONG sidlen = __SID_LENGTH;
				SeGetSID(ObjectContext.m_SID, &sidlen);
				SeGetLuid(&ObjectContext.m_Luid);
				ObjectContext.m_Flags = CreateCompletitionContext;

				NameCacheStore(g_pCommon_NameCache_Context, HASH4FOBJ(fileObject->FsContext), &ObjectContext, sizeof(KLG_OBJECT_CONTEXT), TRUE, '3boS');

				bOverBuf = FALSE;
				if (NameCacheGetBinary(g_pCommon_NameCache_Context, HASH4FOBJ(fileObject->FsContext), &ObjectContext, sizeof(ObjectContext), &bOverBuf, NULL) != NULL)			
				{
					if (bOverBuf == FALSE)
					{
						if (ObjectContext.m_Flags & _CONTEXT_OBJECT_FLAG_MODIFIED)
						{
							if (!IsInvisibleThread((ULONG) PsGetCurrentThreadId()))
								FidBox_ClearValidFlag(FileName, 0);
						}
					}
				}
			}

			if (FileName)
				ExFreePool(FileName);
		}
	}
	
	if(Irp->PendingReturned) {
		IoMarkIrpPending(Irp);
	}

	return STATUS_SUCCESS; //???? Seee examp-less
}

NTSTATUS FSDispositionCompletion(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID pContext)
{
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PFILE_OBJECT		fileObject      = currentIrpStack->FileObject;
	PFSHOOK_EXTENSION	hookExt         = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;

	if(NT_SUCCESS(Irp->IoStatus.Status))
	{
		if (fileObject && fileObject->DeletePending)
		{
			if (pContext)
			{
				PFILTER_EVENT_PARAM pParam = (PFILTER_EVENT_PARAM) pContext;

				pParam->ProcessingType = PostProcessing;
				FilterEvent(pParam, NULL); 
			}
		}
	}

	if(Irp->PendingReturned)
		IoMarkIrpPending(Irp);
	
	if (pContext != NULL)
		_MemFree_Common(pContext);

	return STATUS_SUCCESS;
}

NTSTATUS FSRenameCompletion(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PFILE_OBJECT		fileObject      = currentIrpStack->FileObject;
	PFSHOOK_EXTENSION	hookExt         = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;

	if (Context != NULL)
	{
		if(NT_SUCCESS(Irp->IoStatus.Status))
		{
			PFILTER_EVENT_PARAM pParam = Context;
			pParam->ProcessingType = PostProcessing;

			FilterEvent(pParam, NULL); 
		}

		_MemFree_Common(Context);
	}

	if(Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	return STATUS_SUCCESS;
}


NTSTATUS FSCleanupCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
PFILE_OBJECT			fileObject      = currentIrpStack->FileObject;
BOOLEAN bOverBuf;
PFSHOOK_EXTENSION		hookExt         = (PFSHOOK_EXTENSION) DeviceObject->DeviceExtension;

PWCHAR pwchFileName = NULL;
ULONG FileNameLen = 0;
ULONG FileNameSize = _FILE_MAX_PATHLEN;
	if(Irp->PendingReturned) {
		IoMarkIrpPending(Irp);
	} else {
		if(KeGetCurrentIrql()>=DISPATCH_LEVEL) {
			DbPrint(DC_FILE,DL_ERROR, ("FSCleanupCompletion at IRQL %x\n",KeGetCurrentIrql()));
			DbgBreakPoint();
		} else {
			//if (IsNeedFilterEventAsyncChk(FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0,Irp))
			{
				PVOID RequestData;
				DWORD ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) * 4 + FileNameSize + 1 + sizeof(ULONG) + __SID_LENGTH + sizeof(ULONG);
				ReqDataSize += sizeof(SINGLE_PARAM) + sizeof(LUID);

				bOverBuf = FALSE;
				DbPrint(DC_FILE,DL_INFO, ("CLEANUP Compl FO=%x (%s)\n",fileObject,NameCacheGet(g_pCommon_NameCache, HASH4FOBJ(fileObject),NULL,0, &bOverBuf)));

				RequestData =  _MemAlloc_Common(NonPagedPool, ReqDataSize, 'TboS');
				if (RequestData != NULL)
				{
					BOOLEAN bNameGetComplete = FALSE;
					PFILTER_EVENT_PARAM pParam;
					PSINGLE_PARAM pSingleParam;

					pParam = (PFILTER_EVENT_PARAM) RequestData;
					FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, PostProcessing, 4);
					GetProcName(pParam->ProcName, Irp);

					pSingleParam = (PSINGLE_PARAM) pParam->Params;
					SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
					bOverBuf = FALSE;
					if(NameCacheGet(g_pCommon_NameCache, HASH4FOBJ(fileObject), pSingleParam->ParamValue, FileNameSize, &bOverBuf))
					{
						if (bOverBuf == FALSE)
							bNameGetComplete = TRUE;
						else
						{
							PVOID NewRequestData;
							DbPrint(DC_SYS, DL_WARNING, ("NameCacheTruncating - buffer too small in FSCleanupCompletion\n"));

							NewRequestData = _MemAlloc_Common(NonPagedPool, ReqDataSize - FileNameSize + _VERY_LONG_NAME, 'RboS');
							if (NewRequestData == NULL)
							{
								DbPrint(DC_SYS, DL_WARNING, ("no free memory for realloc request buffer\n"));
								DbgBreakPoint();
							}
							else
							{
								memcpy(NewRequestData, RequestData, ReqDataSize);
								FileNameSize = _VERY_LONG_NAME;
								_MemFree_Common(RequestData);

								RequestData = NewRequestData;
								pParam = (PFILTER_EVENT_PARAM) NewRequestData;							
								pSingleParam = (PSINGLE_PARAM) pParam->Params;
								
								bOverBuf = FALSE;
								if(NameCacheGet(g_pCommon_NameCache, HASH4FOBJ(fileObject), pSingleParam->ParamValue, FileNameSize, &bOverBuf))
								{
									if (bOverBuf == FALSE)
										bNameGetComplete = TRUE;
								}	
							}
						}
					}

					pwchFileName = (PWCHAR) pSingleParam->ParamValue;
					
					if (bNameGetComplete == TRUE)	
					{
						PKLG_OBJECT_CONTEXT pObjectContext = NULL;
						KLG_OBJECT_CONTEXT ObjectContext;
						ULONG RefCount = 0;
						bOverBuf = FALSE;
						
						if (NameCacheGetBinary(g_pCommon_NameCache_Context, HASH4FOBJ(fileObject->FsContext), &ObjectContext, sizeof(ObjectContext), &bOverBuf, &RefCount) != NULL)
							pObjectContext = &ObjectContext;

						pSingleParam->ParamSize = (wcslen((PWCHAR)pSingleParam->ParamValue) +1 ) * sizeof(WCHAR);
						FileNameLen = pSingleParam->ParamSize;

						SINGLE_PARAM_SHIFT(pSingleParam);
						SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, _CONTEXT_OBJECT_FLAG_NONE);
						if (pObjectContext != NULL)
						{
							*(ULONG*)pSingleParam->ParamValue = pObjectContext->m_Flags;

							if (pObjectContext->m_Flags & _CONTEXT_OBJECT_FLAG_MODIFIED)
							{
								if (!IsInvisibleThread((ULONG) PsGetCurrentThreadId()))
									FidBox_ClearValidFlag(pwchFileName, FileNameLen);
							}
						}
						

						*(ULONG*)pSingleParam->ParamValue |= hookExt->ContextFlags;
						if (fileObject->DeletePending)
							*(ULONG*)pSingleParam->ParamValue |= _CONTEXT_OBJECT_FLAG_DELETEPENDING;

						SINGLE_PARAM_SHIFT(pSingleParam);
						SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_REFCOUNT, RefCount);
						
						SINGLE_PARAM_SHIFT(pSingleParam);
						SINGLE_PARAM_INIT_SID(pSingleParam);
						if (pObjectContext != NULL)
						{
							memcpy(pSingleParam->ParamValue, pObjectContext->m_SID, pSingleParam->ParamSize);

							SINGLE_PARAM_SHIFT(pSingleParam);
							SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_LUID);
							pSingleParam->ParamSize = sizeof(LUID);
							memcpy(pSingleParam->ParamValue, &pObjectContext->m_Luid, sizeof(LUID));
							pParam->ParamsCount++;
						}

						FilterEvent(pParam, NULL); 
					}

					_MemFree_Common(RequestData);
				}
			}
			NameCacheFree(g_pCommon_NameCache_Context, HASH4FOBJ(fileObject->FsContext));
		}
	}
	return STATUS_SUCCESS; //???? Seee examp-less
}

NTSTATUS ReadDiskSector(PDEVICE_OBJECT DiskDev,__int64 Offset,ULONG *Size,PVOID OutBuff);
NTSTATUS Disk_GetSectorSize(PDEVICE_OBJECT pDevice, USHORT* pBuffer);

NTSTATUS MountVerifyComplFilterCommon(PDEVICE_OBJECT DeviceObject,UCHAR MinorFunction,PIRP Irp)
{
NTSTATUS ntStatus;
PWCHAR							DevName;
PFILTER_EVENT_PARAM pParam;
PSINGLE_PARAM pSingleParam;
DWORD BufferLength;
BOOLEAN bInvisible;
	if(IsNeedFilterEventAsyncChk(FLTTYPE_NFIOR,IRP_MJ_FILE_SYSTEM_CONTROL, MinorFunction,Irp, &bInvisible)==FALSE)
		return STATUS_SUCCESS;
	ntStatus=STATUS_INSUFFICIENT_RESOURCES;
	if(DeviceObject) {
		if((DevName = ExAllocatePoolWithTag(NonPagedPool, MAXPATHLEN*sizeof(WCHAR),'FSeB'))) {
			USHORT sectorsize;
			if(GetDeviceName(DeviceObject,DevName,MAXPATHLEN)) {
				Disk_GetSectorSize(DeviceObject, &sectorsize);
				BufferLength = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) * 2 + MAXPATHLEN*sizeof(WCHAR) + 1 + sectorsize * 4;	// read 4 sectors;
				pParam = (PFILTER_EVENT_PARAM)ExAllocatePoolWithTag(NonPagedPool, BufferLength, 'TboS');
				if (pParam != NULL) {
					__int64 Sector = 0;
					
					FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_NFIOR, IRP_MJ_FILE_SYSTEM_CONTROL, MinorFunction, PostProcessing, 2);
					GetProcName(pParam->ProcName, Irp);
					
					BufferLength -= (sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM));
					pSingleParam = (PSINGLE_PARAM) pParam->Params;
					SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
					wcscpy((PWCHAR)pSingleParam->ParamValue,DevName);
					
					pSingleParam->ParamSize = (wcslen((PWCHAR)pSingleParam->ParamValue) + 1)*sizeof(WCHAR);
					BufferLength -= pSingleParam->ParamSize;
					
					BufferLength -= sizeof(SINGLE_PARAM);
					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_BINARYDATA);
					pSingleParam->ParamSize = BufferLength;
					
					if (NT_SUCCESS(ntStatus=ReadDiskSector(DeviceObject, Sector, &pSingleParam->ParamSize, pSingleParam->ParamValue))) {
						DbPrint(DC_FILE,DL_INFO, ("MountVerifyComplFilterCommon Ok for %s %S\n",pParam->ProcName,DevName));
						FilterEvent(pParam, NULL);
					} else {
						DbPrint(DC_FILE,DL_WARNING, ("MountVerifyComplFilterCommon: Read disk %S failed. Status=%x\n",DevName,ntStatus));
					}
					ExFreePool(pParam);
				} 
			}
			ExFreePool(DevName);
		}
	}
	if(!NT_SUCCESS(ntStatus)) {
		DbPrint(DC_FILE,DL_WARNING, ("MountVerifyComplFilterCommon failed. Status=%x\n",ntStatus));
	}
	
	return ntStatus;
}
//+ ----------------------------------------------------------------------------------------

VOID
PlaceToDelayedQueue(PDEVICE_OBJECT pUnrecognizedDevice)
{
	NTSTATUS refStatus;
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceTmp;
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceEnum;
	
	ExAcquireFastMutex(&FsUnrecognizedDevListMutex);

	pUnrecognizedDeviceEnum = gpUnrecognizedDevices;
	while (pUnrecognizedDeviceEnum != NULL)
	{
		if (pUnrecognizedDeviceEnum->m_pUnrecognizedDevice == pUnrecognizedDevice)
		{
			ExReleaseFastMutex(&FsUnrecognizedDevListMutex);
			return;
		}
		pUnrecognizedDeviceEnum = pUnrecognizedDeviceEnum->m_pNext;
	}

	pUnrecognizedDeviceTmp = ExAllocatePoolWithTag(NonPagedPool, sizeof(UNRECOGNIZED_DEV_LIST),'UBOS');
	if (pUnrecognizedDeviceTmp == NULL)
	{
		DbPrint(DC_FILE, DL_WARNING, ("PlaceToDelayedQueue - low resource\n"));
	}
	else
	{
		refStatus = ObReferenceObjectByPointer(pUnrecognizedDevice, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode);
		if (NT_SUCCESS(refStatus))
		{
			pUnrecognizedDeviceTmp->m_pUnrecognizedDevice = pUnrecognizedDevice;
			pUnrecognizedDeviceTmp->m_pNext = gpUnrecognizedDevices;
			pUnrecognizedDeviceTmp->m_Expiration = GetTimeShift(3000);
			gpUnrecognizedDevices = pUnrecognizedDeviceTmp;

			DbPrint(DC_FILE, DL_INFO, ("PlaceToDelayedQueue - object placed to queue %#x\n", pUnrecognizedDevice));
		}
		else
		{
			DbPrint(DC_FILE, DL_WARNING, ("PlaceToDelayedQueue - ref failed\n"));
			ExFreePool(pUnrecognizedDeviceTmp);
		}
	}

	ExReleaseFastMutex(&FsUnrecognizedDevListMutex);
}

VOID
DeleteFromDelayedQueue(PDEVICE_OBJECT pUnrecognizedDevice)
{
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceEnum;
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDevicePrev = NULL;

	ExAcquireFastMutex(&FsUnrecognizedDevListMutex);

	pUnrecognizedDeviceEnum = gpUnrecognizedDevices;
	while (pUnrecognizedDeviceEnum != NULL)
	{
		if (pUnrecognizedDeviceEnum->m_pUnrecognizedDevice == pUnrecognizedDevice)
		{
			ObDereferenceObject(pUnrecognizedDevice);
			if (pUnrecognizedDevicePrev == NULL)
				gpUnrecognizedDevices = pUnrecognizedDeviceEnum->m_pNext;
			else
				pUnrecognizedDevicePrev->m_pNext = pUnrecognizedDeviceEnum->m_pNext;
			ExFreePool(pUnrecognizedDeviceEnum);
			break;
		}
		pUnrecognizedDevicePrev = pUnrecognizedDeviceEnum;
		pUnrecognizedDeviceEnum = pUnrecognizedDeviceEnum->m_pNext;
	}

	ExReleaseFastMutex(&FsUnrecognizedDevListMutex);
}

VOID
ProcessDelayedQueue()
{
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceEnum;
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDevicePrev = NULL;

	PDEVICE_OBJECT pDeviceToCheck = NULL;

	if (!FSHooked)
		return;

	ExAcquireFastMutex(&FsUnrecognizedDevListMutex);

	pUnrecognizedDeviceEnum = gpUnrecognizedDevices;
	while (pUnrecognizedDeviceEnum != NULL)
	{
		if (IsTimeExpired(pUnrecognizedDeviceEnum->m_Expiration))
		{
			pDeviceToCheck = pUnrecognizedDeviceEnum->m_pUnrecognizedDevice;

			if (pUnrecognizedDevicePrev == NULL)
				gpUnrecognizedDevices = pUnrecognizedDeviceEnum->m_pNext;
			else
				pUnrecognizedDevicePrev->m_pNext = pUnrecognizedDeviceEnum->m_pNext;

			ExFreePool(pUnrecognizedDeviceEnum);
			break;
		}
		
		pUnrecognizedDevicePrev = pUnrecognizedDeviceEnum;
		pUnrecognizedDeviceEnum = pUnrecognizedDeviceEnum->m_pNext;
	}

	ExReleaseFastMutex(&FsUnrecognizedDevListMutex);

	if (pDeviceToCheck != NULL)
	{
		MountVerifyComplFilterCommon(pDeviceToCheck, IRP_MN_VERIFY_VOLUME, NULL);
		ObDereferenceObject(pDeviceToCheck);
	}
}
//+ ----------------------------------------------------------------------------------------
NTSTATUS FSQueryEaInfo(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
	if ((Irp->Flags & IRP_BUFFERED_IO) && Irp->AssociatedIrp.SystemBuffer && (Irp->IoStatus.Information == 4))
	{
		PULONG pSize = (PULONG) Irp->AssociatedIrp.SystemBuffer;
		*pSize += sizeof(INETSWIFT_DATA) + sizeof(FILE_GET_EA_INFORMATION) + 1;
	}

	if(Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	return STATUS_SUCCESS; 

}

NTSTATUS FSRWCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
    if (1 == InterlockedDecrement(&g_C)) // pre-exit
    {
		// lock not needed in this block
		// only one thread at a time may be here
		unsigned __int64 t = GetCurTimeFast();
		g_TW += (t - g_T); // increment wait time by delta
        g_T = t; // set new start period
    }

    if (1 == InterlockedDecrement(&g_C)) // pre-exit (2)
    {
		// lock not needed in this block
		// only one thread at a time may be here
		unsigned __int64 t = GetCurTimeFast();
		g_TW += (t - g_T); // increment wait time by delta
        g_T = t; // set new start period
    }

	if(Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	return STATUS_SUCCESS; 

}



NTSTATUS FSMountCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PFSHOOK_EXTENSION		hookExt         = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	PDEVICE_OBJECT		HookerDevice;
	PFSHOOK_EXTENSION	hookExtension;
	NTSTATUS					ntStatus;
	PCHAR							DevName;
	PVPB							vpb;
	
	DbPrint(DC_FILE,DL_SPAM, ("MOUNT_VOLUME Status=%x DeviceObject=%x\n",Irp->IoStatus.Status,hookExt->FileSystem));
	if(Irp->IoStatus.Status==STATUS_SUCCESS ) {
		DeleteFromDelayedQueue((PDEVICE_OBJECT) Context);
		vpb=((PDEVICE_OBJECT)Context)->Vpb;
#ifdef __DBG__
		if(vpb != currentIrpStack->Parameters.MountVolume.Vpb) {
			DbPrint(DC_FILE,DL_WARNING, ("MOUNT_VOLUME_OK  VPB in IRP stack changed   VpbDevObj %p IrpVpb=%p Vpb=%p\n",
				vpb->DeviceObject,currentIrpStack->Parameters.MountVolume.Vpb,vpb));
		}
		DbPrint(DC_FILE, DL_WARNING, ("MOUNT_VOLUME_OK DeviceObject=%x Context %#x\n", DeviceObject, Context));
			
#endif
		if(NT_SUCCESS(HookFsDev(vpb->DeviceObject)))
		{
			FindDriveLetter();

			if (!(DeviceObject->Characteristics & (FILE_REMOTE_DEVICE | FILE_READ_ONLY_DEVICE | FILE_REMOVABLE_MEDIA | FILE_VIRTUAL_VOLUME)))
			{
				SPImpProcessList();
			}
		}
/*	} else if(Irp->IoStatus.Status==STATUS_FS_DRIVER_REQUIRED) {
		DbgBreakPoint();
		if(Irp->PendingReturned)
			IoMarkIrpPending(Irp);
		return STATUS_SUCCESS; */
	} else { //if(Irp->IoStatus.Status!=STATUS_UNRECOGNIZED_VOLUME) { //!!!!!!!!!!!!!!!!!!!!!
		DbPrint(DC_FILE, DL_INFO, ("UNRECOGNIZED_VOLUME DeviceObject %#x, Context %#x\n",DeviceObject, Context));
		{
			PDEVICE_OBJECT pUnmountedDeviceObject = (PDEVICE_OBJECT) Context;
			PlaceToDelayedQueue(Context);
		}
		if(Irp->PendingReturned)
			IoMarkIrpPending(Irp);
		return STATUS_SUCCESS; 
	}
//--------------------------------
	MountVerifyComplFilterCommon(vpb->RealDevice,currentIrpStack->MinorFunction,Irp);
//--------------------------------------------
	if(Irp->PendingReturned)
		IoMarkIrpPending(Irp);
	return STATUS_SUCCESS; 
}

NTSTATUS FSVerifyCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PFSHOOK_EXTENSION		hookExt         = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	PDEVICE_OBJECT		HookerDevice;
	PFSHOOK_EXTENSION	hookExtension;
	NTSTATUS					ntStatus;
	PCHAR							DevName;
	PVPB							vpb;

	DbPrint(DC_FILE,DL_SPAM, ("VERIFY_VOLUME Status=%x DeviceObject=%x\n",Irp->IoStatus.Status,hookExt->FileSystem));
	if(!NT_SUCCESS( Irp->IoStatus.Status)) {
		DeleteFromDelayedQueue((PDEVICE_OBJECT) Context);
		vpb =((PDEVICE_OBJECT)Context)->Vpb;
		DbPrint(DC_FILE,DL_INFO, ("VERIFY_VOLUME_OK DeviceObject=%x ParamDev=%x ParamVpbRealDev=%x ParamVpbFSDev=%x\n",
			DeviceObject,
			currentIrpStack->Parameters.VerifyVolume.DeviceObject, // \Device\CdRom0
			currentIrpStack->Parameters.VerifyVolume.Vpb->RealDevice, // The same
			currentIrpStack->Parameters.VerifyVolume.Vpb->DeviceObject)); //Created FS device

		MountVerifyComplFilterCommon(vpb->RealDevice,currentIrpStack->MinorFunction,Irp);
	}
	if(Irp->PendingReturned)
		IoMarkIrpPending(Irp);
	return STATUS_SUCCESS; 
}

CHAR SSS[]="%s - %s";

//you must not pend IRP_SYNCHRONOUS_PAGING_IO writes (these are cache flushes and FlushViewOfFile).
NTSTATUS FSDevDispatch(IN PDEVICE_OBJECT HookDevice, IN PIRP Irp)
{
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	//PIO_STACK_LOCATION	nextIrpStack    = IoGetNextIrpStackLocation(Irp);
	PFILE_OBJECT			fileObject      = currentIrpStack->FileObject;
	PFSHOOK_EXTENSION		hookExt         = (PFSHOOK_EXTENSION)HookDevice->DeviceExtension;
	ULONG						ioControlCode	 = currentIrpStack->Parameters.DeviceIoControl.IoControlCode;
	
	PVOID pSetInfoContext = 0;
	
	KIRQL	CurrIrql;
	
	ULONG FunctionMj;
	ULONG FunctionMi;
	
	VERDICT						Verdict = Verdict_NotFiltered;
	NTSTATUS					ntStatus;
	
	PCHAR						Action;
	PSINGLE_PARAM pSingleParamObjectName = NULL;
	PSINGLE_PARAM pSingleParam;
	PSINGLE_PARAM pSecondParam=NULL;
	
	BOOLEAN bThreadIsInvisible = FALSE;
	
	PVOID RequestData;
	PFILTER_EVENT_PARAM pParam;
	BYTE* pPointer;
	ULONG ReqDataSize;
	
	BOOLEAN bDenyAccess = FALSE;
	
	PFILE_RENAME_INFORMATION pRename;
	EVENT_OBJECT_INFO	EventObject;
	
	KLG_OBJECT_CONTEXT ObjectContext;
	PKLG_OBJECT_CONTEXT pStackObjectContext = NULL;

	BOOLEAN bKernelObj = FALSE;
	
	BOOLEAN bOverBuf = FALSE;
	BOOLEAN bDirectVolumeAccess = FALSE;
	ULONG FileNameSize = _FILE_MAX_PATHLEN;
	ULONG RenameNameSize = _FILE_MAX_PATHLEN;

	ULONG CreateCompletitionContext = 0;

	ULONG disposition = 0;

	CurrIrql = KeGetCurrentIrql();

	if (CurrIrql < DISPATCH_LEVEL)
		bKernelObj = IsThisKernelFileObject(fileObject);
	
	ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) * 4 + 1 + sizeof(ULONG) * 2 + __SID_LENGTH
		+ sizeof(SINGLE_PARAM) + sizeof(LUID);
	
	FunctionMj = currentIrpStack->MajorFunction;
	FunctionMi = 0;
	switch(FunctionMj) {
	case IRP_MJ_CREATE:
		bKernelObj = FALSE; // self protection whole unlocker
		break;
	case IRP_MJ_READ:
	case IRP_MJ_WRITE:
	case IRP_MJ_DIRECTORY_CONTROL:
	case IRP_MJ_LOCK_CONTROL:
	case IRP_MJ_PNP_POWER:
		FunctionMi = currentIrpStack->MinorFunction;
		break;
	case IRP_MJ_QUERY_INFORMATION:
		FunctionMi = currentIrpStack->Parameters.QueryFile.FileInformationClass;
		break;
	case IRP_MJ_SET_INFORMATION:
		FunctionMi = currentIrpStack->Parameters.SetFile.FileInformationClass;
		ReqDataSize += sizeof(FILTER_EVENT_PARAM) + RenameNameSize + 1;
		break;
	case IRP_MJ_QUERY_VOLUME_INFORMATION:
		FunctionMi = currentIrpStack->Parameters.QueryVolume.FsInformationClass;
		break;
	case IRP_MJ_SET_VOLUME_INFORMATION:
		FunctionMi = currentIrpStack->Parameters.SetVolume.FsInformationClass;
		break;
	case IRP_MJ_DEVICE_CONTROL:
	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		FunctionMi = IoGetFunctionCodeFromCtlCode(currentIrpStack->Parameters.DeviceIoControl.IoControlCode);//(currentIrpStack->Parameters.DeviceIoControl.IoControlCode & 0x1ffc)>>2;
		break;
	case IRP_MJ_FILE_SYSTEM_CONTROL:
		FunctionMi = currentIrpStack->MinorFunction;
		if(FunctionMi == IRP_MN_USER_FS_REQUEST || FunctionMi == IRP_MN_KERNEL_CALL) {
			FunctionMj=IRP_MJ_USER_FSCTL;
			FunctionMi=IoGetFunctionCodeFromCtlCode(currentIrpStack->Parameters.FileSystemControl.FsControlCode);
		}
		break;
	}
	
	if (CurrIrql < DISPATCH_LEVEL && !bKernelObj)
	{
		BOOLEAN bNeedProcess = FALSE;

		ULONG fidboxlen = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0]) + _INETSWIFT_PREFIX_FULL_BLEN + 1 + sizeof(INETSWIFT_DATA_EX);
		if (IRP_MJ_QUERY_EA == FunctionMj
			&& (4 == (ULONG) PsGetCurrentProcessId())
			&& Irp->UserBuffer
			&& currentIrpStack->Parameters.QueryEa.EaList
			&& currentIrpStack->Parameters.QueryEa.Length > fidboxlen)
		{
			DbPrint(DC_SYS, DL_ERROR, ("QueryEa - list len 0x%x, outbuflen 0x%x (struct size 0x%x, name len 0x%x)\n", 
				currentIrpStack->Parameters.QueryEa.EaListLength,
				currentIrpStack->Parameters.QueryEa.Length,
				sizeof(FILE_FULL_EA_INFORMATION),
				_INETSWIFT_PREFIX_FULL_BLEN + 1));

			if (currentIrpStack->Parameters.QueryEa.EaListLength >= sizeof(FILE_GET_EA_INFORMATION) + _INETSWIFT_PREFIX_FULL_BLEN)
			{
				FILE_GET_EA_INFORMATION* pGetEa = (FILE_GET_EA_INFORMATION*) currentIrpStack->Parameters.QueryEa.EaList;
				if ((pGetEa->EaNameLength == _INETSWIFT_PREFIX_FULL_BLEN)
					&& (!memcmp(pGetEa->EaName, _INETSWIFT_PREFIX, _INETSWIFT_PREFIX_BLEN)))
				{
					FILE_FULL_EA_INFORMATION* pInfo = Irp->UserBuffer;
					PINETSWIFT_DATA_EX pSwiftData;
					ULONG ealen = 0;
					ULONG ReqLen = currentIrpStack->Parameters.QueryEa.Length - fidboxlen;

					BYTE timf;

					timf = pGetEa->EaName[pGetEa->EaNameLength - 1];
					DbgBreakPoint();

					pInfo->NextEntryOffset = 0;
					pInfo->Flags = 0;
					pInfo->EaNameLength = _INETSWIFT_PREFIX_FULL_BLEN;
					memcpy(pInfo->EaName, _INETSWIFT_PREFIX_FULL, pInfo->EaNameLength);

					ealen = FIELD_OFFSET( FILE_FULL_EA_INFORMATION, EaName[0]) + pInfo->EaNameLength + 1;

					pSwiftData = (PINETSWIFT_DATA_EX) ((CHAR*) Irp->UserBuffer + ealen);
					
					memset(pSwiftData, 0, sizeof(INETSWIFT_DATA_EX) + ReqLen);
					pSwiftData->m_Version = _INETSWIFT_VERSION_3;
					pSwiftData->m_Tag = _INETSWIFT_TAG;

					Irp->IoStatus.Status = FidBox_GetDataByFO(fileObject, &pSwiftData->m_Data, &ReqLen);

					pInfo->EaValueLength = sizeof(INETSWIFT_DATA_EX) + ReqLen;

					if (NT_SUCCESS(Irp->IoStatus.Status))
					{
						Irp->IoStatus.Information = ealen + pInfo->EaValueLength;
						DbPrint(DC_SYS, DL_ERROR, ("QueryEa - successfull, ret len 0x%x\n", Irp->IoStatus.Information));
						
						_xor_drv_func_ex(pSwiftData, sizeof(INETSWIFT_DATA_EX) + ReqLen, timf, _xor_dir_go);
					}
					else
					{
						Irp->IoStatus.Information = 0;
					}

					IoCompleteRequest(Irp, IO_NO_INCREMENT);
					
					return STATUS_SUCCESS;
				}
			}
		}

/*		if (fileObject && fileObject->FileName.Buffer && fileObject->FileName.Length)
		{
			DbPrint(DC_SYS, DL_WARNING, ("%S 0x%x 0x%x(%d)\n",
				fileObject->FileName.Buffer,
				currentIrpStack->MajorFunction,
				currentIrpStack->Parameters.FileSystemControl.FsControlCode,
				FunctionMi));
		}*/

#ifdef CHECK_WHAT_AVPM_DO
		bNeedProcess = IsNeedFilterEventNoInvCheck(FLTTYPE_NFIOR, FunctionMj, FunctionMi);
#else
		bNeedProcess = IsNeedFilterEventAsyncChk(FLTTYPE_NFIOR, FunctionMj, FunctionMi, Irp, &bThreadIsInvisible);
#endif // CHECK_WHAT_AVPM_DO
		
		if (!bThreadIsInvisible)
		{
			if ((IRP_MJ_READ == FunctionMj) || (IRP_MJ_WRITE == FunctionMj))
			{
				LONG curc = InterlockedExchangeAdd(&g_C, 2);
				if (0 == curc)
					g_T = GetCurTimeFast(); // reset start period
			}
		}


		if (FunctionMj == IRP_MJ_FILE_SYSTEM_CONTROL && FunctionMi == 0x600c4)
		{
			if (hookExt->FileSystem && hookExt->FileSystem->DeviceType == FILE_DEVICE_DFS)
			{
				gFSNeedRescan += 4;
			}
		}
		
		if (FunctionMj == IRP_MJ_CREATE)
		{
			disposition = (currentIrpStack->Parameters.Create.Options >> 24) & 0xff; 
			switch (disposition) 
			{ 
			 case FILE_CREATE:
				 DbPrint(DC_SYS, DL_SPAM, ("FILE_CREATE\n"));
				 break; 
			 case FILE_OPEN:
				 DbPrint(DC_SYS, DL_SPAM, ("FILE_OPEN\n"));
				 break; 
			 case FILE_SUPERSEDE:
				 DbPrint(DC_SYS, DL_SPAM, ("FILE_SUPERSEDE\n"));
				 // zamena
				 CreateCompletitionContext |= _CONTEXT_OBJECT_FLAG_MODIFIED;
				 break; 
			 case FILE_OPEN_IF:
				 DbPrint(DC_SYS, DL_SPAM, ("FILE_OPEN_IF\n"));
				 break; 
			 case FILE_OVERWRITE:
				 DbPrint(DC_SYS, DL_SPAM, ("FILE_OVERWRITE\n"));
				 // perezapis
				 CreateCompletitionContext |= _CONTEXT_OBJECT_FLAG_MODIFIED;
				 break; 
			 case FILE_OVERWRITE_IF:
				 DbPrint(DC_SYS, DL_SPAM, ("FILE_OVERWRITE_IF\n"));
				 // perezapis
				 CreateCompletitionContext |= _CONTEXT_OBJECT_FLAG_MODIFIED;
				 break; 
			} 
		}

		if (FunctionMj == IRP_MJ_WRITE)
			bNeedProcess = TRUE;
		else if (FunctionMj == IRP_MJ_CREATE)
		{
			bNeedProcess = TRUE;
			/*if (currentIrpStack->Parameters.Create.Options & FILE_COMPLETE_IF_OPLOCKED)
				bNeedProcess = FALSE;*/
		}
		else
		{
			if (FunctionMj == IRP_MJ_SET_INFORMATION)
			{
				if ((FunctionMi == FileRenameInformation) || (FunctionMi == FileDispositionInformation))
					bNeedProcess = TRUE;
			}
		}

		if (bNeedProcess == TRUE)
		{
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			bOverBuf = FALSE;
			
			EventObject.m_ObjectType = _EVENT_FILE_OBJECT;
			EventObject.m_Object = (HANDLE) fileObject;
			EventObject.m_DevObj = (HANDLE) hookExt->FileSystem;
			EventObject.m_pCallback = NULL;
			
			ReqDataSize += sizeof (SINGLE_PARAM) + __SID_LENGTH;
			ReqDataSize += FileNameSize;
			
			RequestData = _MemAlloc_Common(NonPagedPool, ReqDataSize, 'RboS');
			pParam = (PFILTER_EVENT_PARAM)RequestData;
			
			if (pParam != NULL)
			{
				int nNameGetComplete = 0;
				
				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_NFIOR, FunctionMj, FunctionMi, PreProcessing, 3);
				GetProcName(pParam->ProcName, Irp);
				
				while (nNameGetComplete < 2)	// dve popitki
				{
					pSingleParam = (PSINGLE_PARAM) pParam->Params;
					SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
					
					*(WCHAR*)pSingleParam->ParamValue=0;
					if(pParam->FunctionMj==IRP_MJ_CREATE || pParam->FunctionMj==IRP_MJ_CREATE_NAMED_PIPE || pParam->FunctionMj==IRP_MJ_CREATE_MAILSLOT) {
						//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						//				if(NameCacheFree(g_pCommon_NameCache, HASH4FOBJ(fileObject)))
						//					DbgBreakPoint();
						//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						if(currentIrpStack->Parameters.Create.Options & FILE_OPEN_BY_FILE_ID)
							GetFullNameByID(fileObject,hookExt,(PWCHAR) &pSingleParam->ParamValue);
						else
						{
							GetFullName(fileObject,hookExt,(PWCHAR)&pSingleParam->ParamValue,
								(BOOLEAN)(currentIrpStack->Parameters.Create.Options & FILE_DIRECTORY_FILE),TRUE, FileNameSize/sizeof(WCHAR), 
								&bOverBuf, FALSE, &bDirectVolumeAccess);

						}
					}
					else if(FunctionMj==IRP_MJ_FILE_SYSTEM_CONTROL && FunctionMi==IRP_MN_MOUNT_VOLUME) {
						GetDeviceName(currentIrpStack->Parameters.MountVolume.Vpb->RealDevice,(PWCHAR)&pSingleParam->ParamValue,FileNameSize/sizeof(WCHAR));
					}
					else if(FunctionMj==IRP_MJ_FILE_SYSTEM_CONTROL && FunctionMi==IRP_MN_VERIFY_VOLUME) {
						GetDeviceName(currentIrpStack->Parameters.VerifyVolume.Vpb->RealDevice,(PWCHAR)&pSingleParam->ParamValue,FileNameSize/sizeof(WCHAR));
					}
					else
					{
						BOOLEAN bUnsafe = FALSE;
						BOOLEAN bUseFofn = FALSE;

						if (FunctionMj == IRP_MJ_WRITE)
						{
							if (IoIsOperationSynchronous(Irp))	// this function check OTHER flags. not only IRP_SYNCHRONOUS_PAGING_IO!!!
								bUnsafe = TRUE;
						}
						//unmark to resolve guild wars problem (lock on queryname)
						/*else if (IRP_MJ_CLEANUP == FunctionMj)
						{
							if (IoGetTopLevelIrp() || IoIsOperationSynchronous(Irp))
							{
								bUseFofn = TRUE;
								bUnsafe = TRUE;
							}
						}*/

						GetFullName(fileObject,hookExt,(PWCHAR)&pSingleParam->ParamValue,FALSE,bUseFofn, FileNameSize / sizeof(WCHAR), &bOverBuf, bUnsafe, &bDirectVolumeAccess);
					}
					
					if (bOverBuf == FALSE)
						nNameGetComplete = 2;	// mogno vihodit
					else
					{
						PVOID NewRequestData;
						DbPrint(DC_SYS, DL_WARNING, ("NameCacheTruncating - buffer too small in FSDevDispatch (FileName)\n"));
						
						nNameGetComplete++;
						if (nNameGetComplete < 2)
						{
							NewRequestData = _MemAlloc_Common(NonPagedPool, ReqDataSize - FileNameSize + _VERY_LONG_NAME, 'RboS');
							if (NewRequestData == NULL)
							{
								nNameGetComplete = 2;	//jopa, memory ek :(
								DbPrint(DC_SYS, DL_WARNING, ("no free memory for realloc request buffer\n"));
								DbgBreakPoint();
							}
							else
							{
								memcpy(NewRequestData, RequestData, ReqDataSize);
								_MemFree_Common(RequestData);
								
								FileNameSize = _VERY_LONG_NAME;
								pParam = (PFILTER_EVENT_PARAM) NewRequestData;
								RequestData = NewRequestData;
							}
						}
					}
				}
				
				pSingleParam->ParamSize = (wcslen((PWCHAR)pSingleParam->ParamValue) + 1) * sizeof(WCHAR);
				pSingleParamObjectName = pSingleParam;
				
				//////////////// RENAME starts here			
				if ((FunctionMj == IRP_MJ_SET_INFORMATION) && (FunctionMi == FileRenameInformation))
				{
					int nRenameGetComplete = 0;
					pParam->ParamsCount++;
					
					while (nRenameGetComplete < 2)	// dve popitki
					{
						PWCHAR TargetFName;
						
						bOverBuf = FALSE;
						
						pPointer = (BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;
						pSecondParam = (PSINGLE_PARAM)pPointer;
						SINGLE_PARAM_INIT_NONE(pSecondParam, _PARAM_OBJECT_URL_DEST_W);
						TargetFName=(PWCHAR)pSecondParam->ParamValue;
						*TargetFName = 0;
						pSecondParam->ParamSize = 0;
						
						//!!!!! æ¦v¿ SetFile.ReplaceIfExists ¦â¿¤¦ ª¨¡¤¦ ¯và ª¬à·°¬ ¨¦¯°ê¦ª (°¤¨â¿  ¦¬êè¦á¿¬¬°¤º ¬  è¦ª¿è·¦• ¬ê¨ê¦¬¨)
#ifdef __DBG__
						if (!(Irp->Flags & IRP_BUFFERED_IO))
						{
							// system buffer not valid?
							DbPrint(DC_FILE, DL_WARNING, ("system buffer not valid?\n"));
							DbgBreakPoint();
						}
#endif
						pRename = (PFILE_RENAME_INFORMATION)(Irp->AssociatedIrp.SystemBuffer);
						if(currentIrpStack->Parameters.SetFile.FileObject == NULL)
						{
							// simple rename (¬v¦â¨¿ê¬à  è°  ¿è¿°¹¿¤¦ª¨¤°°  ¦ ¬¿ê·¿)
							PWCHAR LastBackslashInSrc;
							ULONG PrefixLenInBytes;
							LastBackslashInSrc = wcsrchr((PWCHAR)pSingleParam->ParamValue,(int)L'\\');
							if(LastBackslashInSrc)
							{
								PrefixLenInBytes = (PCHAR)LastBackslashInSrc - (PCHAR)pSingleParam->ParamValue + sizeof(WCHAR);
								
								if ((PrefixLenInBytes + pRename->FileNameLength) > RenameNameSize)
									bOverBuf = TRUE;
								
								Uwcsncpy(TargetFName, RenameNameSize / sizeof(WCHAR), (PWCHAR)pSingleParam->ParamValue, PrefixLenInBytes);
								Uwcsncpy((PWCHAR)((PCHAR)TargetFName + PrefixLenInBytes), RenameNameSize / sizeof(WCHAR) - PrefixLenInBytes / sizeof(WCHAR),
									pRename->FileName, pRename->FileNameLength);
							}
							else
							{
								DbPrint(DC_FILE,DL_WARNING, ("There aren't backslashes in source filename for simple rename\n"));
								DbgBreakPoint();
							}
						}
						else if(pRename->RootDirectory == NULL)
						{
							// fully qualified rename (¬v¦â¨¿ê¬à  è°  ¿è¿°¹¿¤¦ª¨¤°° ¤¨ v¦·¨v¿)
							if (pRename->FileName[0] != L'\\')
							{
								DbPrint(DC_FILE,DL_WARNING, ("fully qualified rename doesn't starts from root dir\n"));
								DbgBreakPoint();
							}
							else
							{
								if (pRename->FileNameLength > RenameNameSize)
									bOverBuf = TRUE;
								
								Uwcsncpy(TargetFName, RenameNameSize/sizeof(WCHAR), pRename->FileName, pRename->FileNameLength);
								NameShift(TargetFName);
							}
						}
						else
						{
							// relative rename (¬v¦â¨¿ê¬à  è°  ¿è¿°¹¿¤¦ª¨¤°°  ¦ ¬¿ê·¿)
							PFILE_OBJECT dirObject;
							ULONG dirLenInChars;
							ntStatus = ObReferenceObjectByHandle(pRename->RootDirectory,STANDARD_RIGHTS_REQUIRED,NULL,KernelMode,(PVOID *)&dirObject,NULL);
							if(NT_SUCCESS(ntStatus))
							{
								GetFullName(dirObject, hookExt, TargetFName, TRUE, FALSE, RenameNameSize/sizeof(WCHAR), &bOverBuf, FALSE, &bDirectVolumeAccess);
								if (bOverBuf == TRUE)
								{
									DbPrint(DC_SYS, DL_WARNING, ("NameCacheTruncating - buffer too small in FSDevDispatch (Rename FileName)\n"));
									DbgBreakPoint();
								}
								ObDereferenceObject(dirObject);
								dirLenInChars=wcslen(TargetFName);
								
								if (pRename->FileNameLength > RenameNameSize)
								{
									bOverBuf = TRUE;
									DbPrint(DC_SYS, DL_WARNING, ("overbuf in rename\n"));
									DbgBreakPoint();
								}
								Uwcsncpy(TargetFName+dirLenInChars, RenameNameSize / sizeof(WCHAR) - dirLenInChars, pRename->FileName, pRename->FileNameLength);
							}
							else
							{
								DbPrint(DC_FILE,DL_IMPORTANT, ("Reference root dir for rename failed. Status=%x\n",ntStatus));
							}
						}
						
						if (bOverBuf == FALSE)
							nRenameGetComplete = 2; //mogno vihodit
						else
						{
							nRenameGetComplete++;
							if (nRenameGetComplete < 2)
							{
								PVOID NewRequestData;
								NewRequestData = _MemAlloc_Common(NonPagedPool, ReqDataSize - RenameNameSize + _VERY_LONG_NAME, 'RboS');
								if (NewRequestData == NULL)
								{
									nRenameGetComplete = 2;	//jopa, memory ek :(
									DbPrint(DC_SYS, DL_WARNING, ("no free memory for realloc request buffer (rename)\n"));
									DbgBreakPoint();
								}
								else
								{
									memcpy(NewRequestData, RequestData, ReqDataSize);
									_MemFree_Common(RequestData);
									
									RenameNameSize = _VERY_LONG_NAME;
									pParam = (PFILTER_EVENT_PARAM) NewRequestData;
									RequestData = NewRequestData;
									pSingleParam = (PSINGLE_PARAM) pParam->Params;
								}
							}
						}
					}
					pSecondParam->ParamSize = (wcslen((PWCHAR) pSecondParam->ParamValue)+1)*sizeof(WCHAR);
				}
				//////////////// RENAME ends here
				
#ifdef __DBG__
				Action=ExAllocatePoolWithTag(NonPagedPool,MY_PAGE_SIZE,'FSeB');
				if(Action) {
					DbPrint(DC_FILE,DL_INFO, ("%s %s %S %S FO=%x H=%x FS=%x\n",
						pParam->ProcName,
						NTGetFunctionStr(Action,pParam->HookID, pParam->FunctionMj, pParam->FunctionMi),
						pSingleParam->ParamValue,
						pSecondParam?(PWCHAR)pSecondParam->ParamValue:L"",
						fileObject,
						HookDevice,
						hookExt->FileSystem));
					ExFreePool(Action);
				}
				//			DumpFileObjectFlags(fileObject);
#endif //__DBG__
				
				////!!!!!!!!! T=LæL=L+ !!!!!!!!! +L+++æLT=+ !!!!!!!!! æ¦¯ è¨ª·¨ pSingleParam  ¦¬v¿ Rename !!!!!!!!!!!
				if(pSecondParam)
					pSingleParam=pSecondParam;
				////!!! Têè¦â·° ¤¿ ¬¯ª°º¨ê¦ !!!!!! T¹¦êè¿ê¦ ª ¦¸¨ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				
				if(fileObject) {
					if(IoIsOperationSynchronous(Irp)==FALSE) {
						DbPrint(DC_FILE,DL_INFO,("Async operation. Waiting unsafely.\n"));
						pParam->UnsafeValue++;
					}
				}
				
				if (FunctionMj == IRP_MJ_CREATE)
				{
					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ACCESSATTR, (ULONG) currentIrpStack->Parameters.Create.SecurityContext->DesiredAccess);
					pParam->ParamsCount++;
				}
				else
				{
					if (fileObject != NULL)
						if (NameCacheGetBinary(g_pCommon_NameCache_Context, HASH4FOBJ(fileObject->FsContext), &ObjectContext, sizeof(ObjectContext), &bOverBuf, NULL) != NULL)
						{
							pStackObjectContext = &ObjectContext;
						}
				}

				SINGLE_PARAM_SHIFT(pSingleParam);
				
				if (FunctionMj == IRP_MJ_CREATE)
				{
					ULONG tmpFlag = _CONTEXT_OBJECT_FLAG_NONE;

					if (bDirectVolumeAccess)
						tmpFlag |= _CONTEXT_OBJECT_FLAG_DIRECTACCESS;

					if (currentIrpStack->Parameters.Create.Options & FILE_CREATE_TREE_CONNECTION)
					{
						DbPrint(DC_SYS, DL_INFO, ("Create file with FILE_CREATE_TREE_CONNECTION. Skip processing\n"));
						CreateCompletitionContext |= _CONTEXT_OBJECT_FLAG_NETWORKTREE;
						tmpFlag |= _CONTEXT_OBJECT_FLAG_NETWORKTREE;
					}

					/*if (currentIrpStack->Parameters.Create.Options & FILE_DELETE_ON_CLOSE)
					{
						if (fileObject && fileObject->FileName.Buffer && fileObject->FileName.Length)
						{
							DbPrint(DC_SYS, DL_WARNING, ("delonclose %S\n",
								fileObject->FileName.Buffer));
						}
					}*/
								
					if (currentIrpStack->Parameters.Create.Options & FILE_DIRECTORY_FILE)
					{
						tmpFlag |= _CONTEXT_OBJECT_FLAG_DIRECTORY;
						CreateCompletitionContext |= _CONTEXT_OBJECT_FLAG_DIRECTORY;
					}

					if ((disposition == FILE_OVERWRITE_IF) || (disposition == FILE_OVERWRITE) || (disposition == FILE_CREATE))
						tmpFlag |= _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT;
					

					if (currentIrpStack->Parameters.Create.ShareAccess & FILE_SHARE_READ)
						tmpFlag |= _CONTEXT_OBJECT_FLAG_SHAREREAD;

					if (currentIrpStack->Parameters.Create.ShareAccess & FILE_SHARE_WRITE)
						tmpFlag |= _CONTEXT_OBJECT_FLAG_SHAREWRITE;

					if (currentIrpStack->Parameters.Create.SecurityContext->DesiredAccess & (FILE_WRITE_DATA | FILE_APPEND_DATA))
						tmpFlag |= _CONTEXT_OBJECT_FLAG_FORWRITE;

					if (currentIrpStack->Parameters.Create.SecurityContext->DesiredAccess & FILE_READ_DATA)
						tmpFlag |= _CONTEXT_OBJECT_FLAG_FORREAD;

					if (currentIrpStack->Parameters.Create.SecurityContext->DesiredAccess & DELETE)
						tmpFlag |= _CONTEXT_OBJECT_FLAG_FORDELETE;
					
/*
 Pretty hideous implementation of rename, huh? I don't know about
*ALWAYS* as there are several variations of rename. But
SL_OPEN_TARGET_DIRECTORY is, as far as I know, ONL:Y associated with a
rename operation. If you use a debugger you should find this occurs
within IoSetInformation only.
 */
					if (currentIrpStack->Flags & IO_OPEN_TARGET_DIRECTORY)
						tmpFlag |= _CONTEXT_OBJECT_FLAG_SL_OPENTAR_DIR;

					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, tmpFlag);
				}
				else
				{
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, _CONTEXT_OBJECT_FLAG_NONE);
				}
				
				if (pStackObjectContext != NULL)
				{
					*(ULONG*)pSingleParam->ParamValue = pStackObjectContext->m_Flags;
					if (FunctionMj == IRP_MJ_WRITE)
					{
						if (bDenyAccess == FALSE)
						{
							pStackObjectContext->m_Flags |= _CONTEXT_OBJECT_FLAG_MODIFIED;
							NameCacheGetReplaceBinary(g_pCommon_NameCache_Context, HASH4FOBJ(fileObject->FsContext), pStackObjectContext, sizeof(ObjectContext));
						}
					}
				}
				
				*(ULONG*)pSingleParam->ParamValue |= hookExt->ContextFlags;
				if (FunctionMj == IRP_MJ_CREATE && (hookExt->ContextFlags & _CONTEXT_OBJECT_FLAG_NTFS))
				{
					if (MatchWithPatternW(L"*\\System Volume Information\\*", (PWCHAR) pSingleParamObjectName->ParamValue, TRUE))
						*(ULONG*)pSingleParam->ParamValue &= ~_CONTEXT_OBJECT_FLAG_NTFS;
				}

				
				//DumpFileObjectFlags(fileObject);
				
				if (IRP_MJ_CREATE == FunctionMj)
				{
					LUID Luid = {0, 0};
					if (SeGetLuid(&Luid))
					{
						SINGLE_PARAM_SHIFT(pSingleParam);
						SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_LUID);
						pSingleParam->ParamSize = sizeof(LUID);
						memcpy(pSingleParam->ParamValue, &Luid, sizeof(LUID));
						pParam->ParamsCount++;
					}
				}

				// -----------------------------------------------------------------------------------------
				// reserve place for sid
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_SID(pSingleParam);
				// end reserve place for sid
				if (FunctionMj != IRP_MJ_CREATE)
				{
					if (pStackObjectContext != NULL)
						memcpy(pSingleParam->ParamValue, pStackObjectContext->m_SID, pSingleParam->ParamSize);
				}
				
				// -----------------------------------------------------------------------------------------

				if (FunctionMj == IRP_MJ_SET_INFORMATION)
				{
					if ((FunctionMi == FileRenameInformation) || (FunctionMi == FileDispositionInformation))
						FidBox_ClearValidFlag((PWCHAR) pSingleParamObjectName->ParamValue, pSingleParamObjectName->ParamSize);
				}
				
#ifdef CHECK_WHAT_AVPM_DO
				if(IsInvisibleThread((ULONG)PsGetCurrentThreadId()) || IsInvisibleThread(GetRequestorThreadId(Irp)))
					Verdict=Verdict_NotFiltered;
				else
#endif
				{	
					if (bDenyAccess == FALSE)
						Verdict = FilterEvent(pParam, &EventObject);
					else
						Verdict = Verdict_Discard;
				}
				
				if(Verdict==Verdict_Discard &&
					(FunctionMj==IRP_MJ_CLOSE || FunctionMj==IRP_MJ_CLEANUP)) {
					DbPrint(DC_FILE,DL_INFO, ("Verdict changed to Pass for close operation\n"));
					Verdict=Verdict_Pass;
				} else if(!_PASS_VERDICT(Verdict)) {
					if(FunctionMj==IRP_MJ_CREATE || 
						FunctionMj==IRP_MJ_CREATE_NAMED_PIPE || 
						FunctionMj==IRP_MJ_CREATE_MAILSLOT) {
						NameCacheFree(g_pCommon_NameCache, HASH4FOBJ(fileObject));
					}
					ntStatus=Irp->IoStatus.Status=STATUS_ACCESS_DENIED;
					Irp->IoStatus.Information=0;
					IoCompleteRequest(Irp,IO_NO_INCREMENT);
					if(Verdict == Verdict_Kill) {
						DbPrint(DC_FILE,DL_NOTIFY, ("Kill %s\n", pParam->ProcName));
						KillCurrentProcess();
					} else if(Verdict == Verdict_Discard) {
						DbPrint(DC_FILE,DL_NOTIFY, ("Discard %s\n",pSingleParam->ParamValue));
					}
					
					_MemFree_Common(RequestData);
					return ntStatus;
				}
				if (FunctionMj == IRP_MJ_SET_INFORMATION)
					pSetInfoContext = RequestData;
				else
					_MemFree_Common(RequestData);
			}
		} // Is NeedFilter And Invisible check
	}

	switch (FunctionMj)
	{
	case IRP_MJ_READ:
	case IRP_MJ_WRITE:
		if (!bThreadIsInvisible)
		//if ((CurrIrql < DISPATCH_LEVEL) && (!bThreadIsInvisible))
		{
			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine(Irp, FSRWCompletion, NULL, TRUE, TRUE, TRUE);
		}
		else
		{
			IoSkipCurrentIrpStackLocation(Irp);
		}
		break;
	case IRP_MJ_CLOSE:
		NameCacheFree(g_pCommon_NameCache, HASH4FOBJ(fileObject));
		IoSkipCurrentIrpStackLocation(Irp);
		break;

	case IRP_MJ_QUERY_INFORMATION:
		if (FunctionMi == FileEaInformation && (4 == (ULONG) PsGetCurrentProcessId()))
		{
			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine(Irp, FSQueryEaInfo, NULL, TRUE, TRUE, TRUE);
		}
		else
		{
			IoSkipCurrentIrpStackLocation(Irp);
		}
		break;

	case IRP_MJ_FILE_SYSTEM_CONTROL:
		{
			switch (FunctionMi)
			{
			case IRP_MN_MOUNT_VOLUME:
				{
					DbPrint(DC_FILE,DL_SPAM, ("IRP_MN_MOUNT_VOLUME HookDevice=%x ParamDev=%x ParamVpbRealDev=%x ParamVpbFSDev=%x\n",
						HookDevice,
						currentIrpStack->Parameters.MountVolume.DeviceObject,
						currentIrpStack->Parameters.MountVolume.Vpb->RealDevice,
						currentIrpStack->Parameters.MountVolume.Vpb->DeviceObject));
					IoCopyCurrentIrpStackLocationToNext(Irp); 
					IoSetCompletionRoutine(Irp, FSMountCompletion,currentIrpStack->Parameters.MountVolume.Vpb->RealDevice,TRUE,TRUE,TRUE);
				}
				break;
			case IRP_MN_VERIFY_VOLUME:
				{
					DbPrint(DC_FILE,DL_SPAM, ("IRP_MN_VERIFY_VOLUME HookDevice=%x ParamDev=%x ParamVpbRealDev=%x ParamVpbFSDev=%x\n",
						HookDevice,
						currentIrpStack->Parameters.MountVolume.DeviceObject,
						currentIrpStack->Parameters.MountVolume.Vpb->RealDevice,
						currentIrpStack->Parameters.MountVolume.Vpb->DeviceObject));
					IoCopyCurrentIrpStackLocationToNext(Irp); 
					IoSetCompletionRoutine(Irp, FSVerifyCompletion,currentIrpStack->Parameters.MountVolume.Vpb->RealDevice,TRUE,TRUE,TRUE);

				}
				break;
			default:
				IoSkipCurrentIrpStackLocation(Irp);
				break;
			}
		}
		break;
	case IRP_MJ_CREATE:
		{
			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine(Irp, FSCreateCompletion, (PVOID) CreateCompletitionContext,TRUE,TRUE,TRUE);
		}
		break;
	case IRP_MJ_CLEANUP:
		{
			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine(Irp, FSCleanupCompletion, NULL, TRUE, TRUE, TRUE);
		}
		break;
	case IRP_MJ_SET_INFORMATION:
		{
			switch (FunctionMi)
			{
			case FileRenameInformation:
				{
					IoCopyCurrentIrpStackLocationToNext(Irp);
					IoSetCompletionRoutine(Irp, FSRenameCompletion, pSetInfoContext, TRUE, TRUE, TRUE);
				}
				break;
			case FileDispositionInformation:
				{
					IoCopyCurrentIrpStackLocationToNext(Irp);
					IoSetCompletionRoutine(Irp, FSDispositionCompletion, pSetInfoContext,TRUE,TRUE,TRUE);
				}
				break;
			default:
				if (pSetInfoContext)
					_MemFree_Common(pSetInfoContext);
				IoSkipCurrentIrpStackLocation(Irp);
				break;
			}
		}
		break;
	default:
		IoSkipCurrentIrpStackLocation(Irp);
		break;
	}
	
	ntStatus = IoCallDriver(hookExt->FileSystem, Irp);
	
	return ntStatus;
}

void FioInit()
{
	if (*NtBuildNumber > 2195)
	{
		pIoAttachDeviceToDeviceStackSafe = GetExport(BaseOfNtOsKrnl,"IoAttachDeviceToDeviceStackSafe",NULL);
	}
}


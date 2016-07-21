
#define FASTIOPRESENT( _hookExt, _call )                                                \
	(hookExt && hookExt->FileSystem &&																										\
	hookExt->FileSystem->DriverObject->FastIoDispatch &&                                  \
	hookExt->FileSystem->DriverObject->FastIoDispatch->_call &&                          \
	(((ULONG)&_hookExt->FileSystem->DriverObject->FastIoDispatch->_call -                 \
	(ULONG) &_hookExt->FileSystem->DriverObject->FastIoDispatch->SizeOfFastIoDispatch <   \
(ULONG) _hookExt->FileSystem->DriverObject->FastIoDispatch->SizeOfFastIoDispatch )))

#define FASTIO_FUNCNUM(_call) \
	(((ULONG)&HookerDriverObject->FastIoDispatch->_call -  \
(ULONG)&HookerDriverObject->FastIoDispatch->SizeOfFastIoDispatch)/sizeof(PVOID)-1)


NTSTATUS HFIOCommon(ULONG index,PFILE_OBJECT fileObject,PFSHOOK_EXTENSION hookExt)
{
	NTSTATUS retStatus;
	VERDICT	Verdict;
	KIRQL	CurrIrql;
	
	PSINGLE_PARAM pSingleParam;
	PVOID RequestData;
	ULONG ReqDataSize;
	PFILTER_EVENT_PARAM pParam;

	BOOLEAN bOverBuf;
	BOOLEAN bDirectVolumeAccess = FALSE;
	ULONG FileNameSize = _FILE_MAX_PATHLEN;

	retStatus = STATUS_SUCCESS;

	CurrIrql = KeGetCurrentIrql();
	if (CurrIrql >= DISPATCH_LEVEL)
		return retStatus;
		
	if (IsNeedFilterEvent(FLTTYPE_FIOR, index, 0) == FALSE)
		return retStatus;

	ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + FileNameSize + 1;
	ReqDataSize += sizeof (SINGLE_PARAM) + __SID_LENGTH;
	RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
	
	pParam = (PFILTER_EVENT_PARAM) RequestData;
	if (pParam == NULL) {
		DbPrint(DC_FILE,DL_WARNING, ("not enough memory for FilterParam\n"));
	} else {
		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_FIOR, index, 0, PreProcessing, 1);

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
		
		while (TRUE)
		{
			if(index==0x17)
			{//FastIoQueryOpen
				GetFullName(fileObject,hookExt,(PWCHAR)pSingleParam->ParamValue,FALSE,TRUE, FileNameSize / sizeof(WCHAR), &bOverBuf, FALSE, &bDirectVolumeAccess);
			} else {
				GetFullName(fileObject,hookExt,(PWCHAR)pSingleParam->ParamValue,FALSE,FALSE, FileNameSize / sizeof(WCHAR), &bOverBuf, FALSE, &bDirectVolumeAccess);
			}
			
			if (bOverBuf == FALSE)
				break;
			else
			{
				PWCHAR pNewRequestData;
				DbPrint(DC_SYS, DL_WARNING, ("NameCacheTruncating - buffer too small in HFIOCommon\n"));
				DbgBreakPoint();

				if (FileNameSize == _VERY_LONG_NAME)
					break;

				pNewRequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize - FileNameSize + _VERY_LONG_NAME, 'RboS');
				if (pNewRequestData == NULL)
					break;
				else
				{
					memcpy(pNewRequestData, RequestData, ReqDataSize);
					ExFreePool(RequestData);
					
					RequestData = pNewRequestData;
					pParam = (PFILTER_EVENT_PARAM) RequestData;
					pSingleParam = (PSINGLE_PARAM) pParam->Params;
					FileNameSize = _VERY_LONG_NAME;
				}
			}
		}
		
		pSingleParam->ParamSize = (wcslen((PWCHAR)pSingleParam->ParamValue) + 1)*sizeof(WCHAR);

		// -----------------------------------------------------------------------------------------
		// reserve place for sid
		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_SID(pSingleParam);
		pParam->ParamsCount++;
		// end reserve place for sid
		// -----------------------------------------------------------------------------------------

		{
			//DbPrint(DC_FILE,DL_INFO, ("%s %s %S\n",pParam->ProcName, NTFastIOFunctionName[index],pSingleParam->ParamValue));
			DbPrint(DC_FILE, DL_INFO, ("%s\n",NTFastIOFunctionName[index]));
		}

		Verdict = FilterEvent(pParam, NULL);

		if(!_PASS_VERDICT(Verdict)) {
			if(Verdict == Verdict_Kill) {
				DbPrint(DC_FILE,DL_NOTIFY, ("Kill %s\n", pParam->ProcName));
				KillCurrentProcess();
			} else if(Verdict == Verdict_Discard) {
				DbPrint(DC_FILE,DL_NOTIFY, ("Discard %s\n", pSingleParam->ParamValue));
			}
			retStatus = STATUS_ACCESS_DENIED;
		}
		ExFreePool(RequestData);
	}
	return retStatus;
}

BOOLEAN HFIOCheckifPossible (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset,IN ULONG Length,IN BOOLEAN Wait,IN ULONG LockKey,IN BOOLEAN CheckForReadOperation,OUT PIO_STATUS_BLOCK IoStatus,IN PDEVICE_OBJECT DeviceObject)
/*++
    This routine is the fast I/O "pass through" routine for checking to see
    whether fast I/O is possible for this file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be operated on.
    FileOffset - Byte offset in the file for the operation.
    Length - Length of the operation to be performed.
    Wait - Indicates whether or not the caller is willing to wait if the appropriate locks, etc. cannot be acquired
    LockKey - Provides the caller's key for file locks.
    CheckForReadOperation - Indicates whether the caller is checking for a read (TRUE) or a write operation.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
	BOOLEAN retval = FALSE;
	PFSHOOK_EXTENSION hookExt;
	hookExt=(PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	if(FASTIOPRESENT(hookExt,FastIoCheckIfPossible))
	{
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoCheckIfPossible),FileObject,hookExt)==STATUS_SUCCESS)
		{
			retval=hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoCheckIfPossible(
				FileObject,FileOffset,Length,Wait,LockKey,CheckForReadOperation,IoStatus,hookExt->FileSystem);
		}
		else
		{
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	
	return retval;
}

BOOLEAN HFIORead (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset,IN ULONG Length,IN BOOLEAN Wait,IN ULONG LockKey, OUT PVOID Buffer,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject)
/*++
    This routine is the fast I/O "pass through" routine for reading from a
    file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be read.
    FileOffset - Byte offset in the file of the read.
    Length - Length of the read operation to be performed.
    Wait - Indicates whether or not the caller is willing to wait if the appropriate locks, etc. cannot be acquired
    LockKey - Provides the caller's key for file locks.
    Buffer - Pointer to the caller's buffer to receive the data read.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
	BOOLEAN retval = FALSE;
	PFSHOOK_EXTENSION hookExt;
	hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	if(FASTIOPRESENT(hookExt,FastIoRead))
	{
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoRead),FileObject,hookExt)==STATUS_SUCCESS)
		{
			retval=hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoRead(
				FileObject,FileOffset,Length,Wait,LockKey,Buffer,IoStatus,hookExt->FileSystem);
		}
		else
		{
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}

	return retval;
}

BOOLEAN HFIOWrite (IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN BOOLEAN Wait, IN ULONG LockKey, IN PVOID Buffer,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for writing to a
    file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be written.
    FileOffset - Byte offset in the file of the write operation.
    Length - Length of the write operation to be performed.
    Wait - Indicates whether or not the caller is willing to wait if the appropriate locks, etc. cannot be acquired
    LockKey - Provides the caller's key for file locks.
    Buffer - Pointer to the caller's buffer that contains the data to be written.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoWrite)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoWrite),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoWrite( 
				FileObject,FileOffset,Length,Wait,LockKey,Buffer,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOQueryBasicInfo (IN PFILE_OBJECT FileObject, IN BOOLEAN Wait, OUT PFILE_BASIC_INFORMATION Buffer,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for querying basic
    information about the file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be queried.
    Wait - Indicates whether or not the caller is willing to wait if the appropriate locks, etc. cannot be acquired
    Buffer - Pointer to the caller's buffer to receive the information about the file.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoQueryBasicInfo)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoQueryBasicInfo),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoQueryBasicInfo( 
				FileObject,Wait,Buffer,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOQueryStandardInfo (IN PFILE_OBJECT FileObject, IN BOOLEAN Wait, OUT PFILE_STANDARD_INFORMATION Buffer,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for querying standard
    information about the file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be queried.
    Wait - Indicates whether or not the caller is willing to wait if the appropriate locks, etc. cannot be acquired
    Buffer - Pointer to the caller's buffer to receive the information about the file.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoQueryStandardInfo)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoQueryStandardInfo),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoQueryStandardInfo( 
				FileObject,Wait,Buffer,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOLock (IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset,IN PLARGE_INTEGER Length, PEPROCESS ProcessId, ULONG Key,BOOLEAN FailImmediately, BOOLEAN ExclusiveLock,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for locking a byte
    range within a file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be locked.
    FileOffset - Starting byte offset from the base of the file to be locked.
    Length - Length of the byte range to be locked.
    ProcessId - ID of the process requesting the file lock.
    Key - Lock key to associate with the file lock.
    FailImmediately - Indicates whether or not the lock request is to fail if it cannot be immediately be granted.
    ExclusiveLock - Indicates whether the lock to be taken is exclusive (TRUE) or shared.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoLock)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoLock),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoLock( 
				FileObject,FileOffset,Length,ProcessId,Key,FailImmediately,ExclusiveLock,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOUnlockSingle (IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset,IN PLARGE_INTEGER Length, PEPROCESS ProcessId, ULONG Key,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for unlocking a byte
    range within a file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be unlocked.
    FileOffset - Starting byte offset from the base of the file to be unlocked.
    Length - Length of the byte range to be unlocked.
    ProcessId - ID of the process requesting the unlock operation.
    Key - Lock key associated with the file lock.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoUnlockSingle)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoUnlockSingle),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoUnlockSingle(
				FileObject,FileOffset,Length,ProcessId,Key,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOUnlockAll (IN PFILE_OBJECT FileObject, PEPROCESS ProcessId,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for unlocking all
    locks within a file.
    This function simply invokes the file system's cooresponding routine, or
    returns FALSE if the file system does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be unlocked.
    ProcessId - ID of the process requesting the unlock operation.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoUnlockAll)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoUnlockAll),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoUnlockAll( 
				FileObject,ProcessId,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOUnlockAllByKey (IN PFILE_OBJECT FileObject, PVOID ProcessId, ULONG Key,OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for unlocking all
    locks within a file based on a specified key.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be unlocked.
    ProcessId - ID of the process requesting the unlock operation.
    Key - Lock key associated with the locks on the file to be released.
    IoStatus - Pointer to a variable to receive the I/O status of the operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoUnlockAllByKey)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoUnlockAllByKey),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoUnlockAllByKey( 
				FileObject,ProcessId,Key,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIODeviceControl(IN PFILE_OBJECT FileObject, IN BOOLEAN Wait, IN PVOID InputBuffer, 
									IN ULONG InputBufferLength, OUT PVOID OutputBuffer, IN ULONG OutputBufferLength,
									IN ULONG IoControlCode, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject)
/*++
    This routine is the fast I/O "pass through" routine for device I/O 
    control operations on a file.

Arguments:
    FileObject - Pointer to the file object representing the device to be
        serviced.
    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired
    InputBuffer - Optional pointer to a buffer to be passed into the driver.
    InputBufferLength - Length of the optional InputBuffer, if one was
        specified.
    OutputBuffer - Optional pointer to a buffer to receive data from the
        driver.
    OutputBufferLength - Length of the optional OutputBuffer, if one was
        specified.
    IoControlCode - I/O control code indicating the operation to be performed
        on the device.
    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.
    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.
Notes:
    This function does not check the validity of the input/output buffers because
    the ioctl's are implemented as METHOD_BUFFERED.  In this case, the I/O manager
    does the buffer validation checks for us.
--*/
{
BOOLEAN retval=FALSE;
PFSHOOK_EXTENSION hookExt;
	if(DeviceObject==HookerDeviceObject){
		HOOKDeviceControl(InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,
			IoControlCode,&IoStatus->Information,&IoStatus->Status, TRUE, TRUE);
		retval=TRUE;
	}
	else if (DeviceObject==HookerDeviceObjectAlternate)
	{
		HOOKDeviceControl(InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,
			IoControlCode,&IoStatus->Information,&IoStatus->Status, FALSE, TRUE);
		retval=TRUE;		
	} else {
		hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
		if(FASTIOPRESENT(hookExt,FastIoDeviceControl)) {
			if(HFIOCommon(FASTIO_FUNCNUM(FastIoDeviceControl),FileObject,hookExt)==STATUS_SUCCESS) {
				retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoDeviceControl( 
					FileObject,Wait,InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,IoControlCode,IoStatus,hookExt->FileSystem);
			} else {
				IoStatus->Status=STATUS_ACCESS_DENIED;
				IoStatus->Information=0;
			}
		}
	}
	return retval;
}

/*!!!!!!!!!!!!   See SFILTER */
VOID HFIOAcquireFile (PFILE_OBJECT FileObject)
/*++
    This routine attempts to acquire any appropriate locks on a file in
    preparation for a Memory Management call to the driver.
     This function should actually be simple, however, when this feature was
    added to the system the DeviceObject for this driver was not passed in,
    so there is added complexity because this driver must first locate its
    own device object.  This is done by walking the stack of drivers between
    the real device and top-most level driver, and checking to see whether
    or not any of the device objects in the chain belong to this driver.
     If this driver locates its device object (which most of the time it
    should be able to, unless someone is attempting to dismount the device,
    etc.), then this driver attempts to acquire the lock.  Note that not all
    drivers support this function (in fact most do not), so it is possible
    that this entire function becomes a nop.
Arguments:
    FileObject - Pointer to the file object for the file whose lock is to be acquired (if necessary).
--*/
{
PFSHOOK_EXTENSION hookExt;
PDEVICE_OBJECT DeviceObject;
PDEVICE_OBJECT checkDevice;
	if (FileObject->DeviceObject->Vpb == 0) {
		DbPrint(DC_FILE,DL_WARNING, ("!!! AcquireFile - FileObject->DeviceObject->Vpb is NULL!!!\n"));
	}
	else {
		checkDevice=FileObject->DeviceObject->Vpb->DeviceObject;
		while(checkDevice) {
			if(checkDevice->DriverObject==HookerDeviceObject->DriverObject) {
				DeviceObject=checkDevice; 
				hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
				if(FASTIOPRESENT(hookExt,AcquireFileForNtCreateSection)) {
					if(HFIOCommon(FASTIO_FUNCNUM(AcquireFileForNtCreateSection),FileObject,hookExt)==STATUS_SUCCESS) {
						hookExt->FileSystem->DriverObject->FastIoDispatch->AcquireFileForNtCreateSection(FileObject);
					}
				}
				return;
			}
			checkDevice=checkDevice->AttachedDevice;
		}
		DbPrint(DC_FILE,DL_ERROR, ("!!! AcquireFile - Should never get here\n"));
	}
}

/*!!!!!!!!!!!!   See SFILTER */
VOID HFIOReleaseFile (PFILE_OBJECT FileObject)
/*++
    This routine attempts to release any appropriate locks on a file after
    a Memory Management call to the driver.
    This function undoes whatever the SfFastIoAcquireFile acquires.  See that
    routine for a description.
Arguments:
    FileObject - Pointer to the file object for the file whose lock is to be released (if necessary).
--*/
{
PFSHOOK_EXTENSION hookExt;
PDEVICE_OBJECT DeviceObject;
PDEVICE_OBJECT checkDevice;
	if (FileObject->DeviceObject->Vpb == 0) {
		DbPrint(DC_FILE,DL_WARNING, ("!!! HFIOReleaseFile - FileObject->DeviceObject->Vpb is NULL!!!\n"));
	}
	else {
		checkDevice=FileObject->DeviceObject->Vpb->DeviceObject;
		while(checkDevice) {
			if(checkDevice->DriverObject==HookerDeviceObject->DriverObject) {
				DeviceObject=checkDevice;  
				hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
				if(FASTIOPRESENT(hookExt,ReleaseFileForNtCreateSection)) {
					if(HFIOCommon(FASTIO_FUNCNUM(ReleaseFileForNtCreateSection),FileObject,hookExt)==STATUS_SUCCESS) {
						hookExt->FileSystem->DriverObject->FastIoDispatch->ReleaseFileForNtCreateSection( FileObject );
					}
				}
				return;
			}
			checkDevice=checkDevice->AttachedDevice;
		}
		DbPrint(DC_FILE,DL_ERROR, ("!!! ReleaseFile - Should never get here\n"));
	}
}

VOID HFIODetachDevice (PDEVICE_OBJECT SourceDevice, PDEVICE_OBJECT TargetDevice)
/*++
    This routine is invoked on the fast path to detach from a device that
    is being deleted.  This occurs when this driver has attached to a file
    system volume device object, and then, for some reason, the file system
    decides to delete that device (it is being dismounted, it was dismounted
    at some point in the past and its last reference has just gone away, etc.)
Arguments:
    SourceDevice - Pointer to this driver's device object, which is attached to the file system's volume device object.
    TargetDevice - Pointer to the file system's volume device object.
--*/
{
PFSHOOK_EXTENSION hookExt;
	DbPrint(DC_FILE,DL_NOTIFY, ("HFIODetachDevice Source=%x Target=%x\n",SourceDevice,TargetDevice));
	hookExt = (PFSHOOK_EXTENSION)SourceDevice->DeviceExtension;
//	if(FASTIOPRESENT(hookExt,FastIoDetachDevice)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoDetachDevice),NULL,hookExt)==STATUS_SUCCESS) {
			//			hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoDetachDevice(SourceDevice,TargetDevice);
		}
//	}
	UnHookFsDev(SourceDevice,TargetDevice);
}

BOOLEAN HFIOQueryNetworkOpenInfo (IN PFILE_OBJECT FileObject,IN BOOLEAN Wait, OUT struct _FILE_NETWORK_OPEN_INFORMATION *Buffer,OUT struct _IO_STATUS_BLOCK *IoStatus, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for querying network
    information about a file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object to be queried.
    Wait - Indicates whether or not the caller can handle the file system having to wait and tie up the current thread.
    Buffer - Pointer to a buffer to receive the network information about the file.
    IoStatus - Pointer to a variable to receive the final status of the query operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoQueryNetworkOpenInfo)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoQueryNetworkOpenInfo),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoQueryNetworkOpenInfo( 
				FileObject,Wait,Buffer,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

NTSTATUS	HFIOAcquireForModWrite (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER EndingOffset, OUT struct _ERESOURCE **ResourceToRelease,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for acquiring the
    file resource prior to attempting a modified write operation.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object whose resource is to be acquired.
    EndingOffset - The offset to the last byte being written plus one.
    ResourceToRelease - Pointer to a variable to return the resource to  release.  Not defined if an error is returned.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is either success or failure based on whether or not fast I/O is possible for this file.
--*/
{
NTSTATUS retval = STATUS_NOT_SUPPORTED;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,AcquireForModWrite)) {
		if(HFIOCommon(FASTIO_FUNCNUM(AcquireForModWrite),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->AcquireForModWrite( 
				FileObject,EndingOffset,ResourceToRelease,hookExt->FileSystem);
		} else {
			retval = STATUS_ACCESS_DENIED;
		}
	}
	return retval;
}

BOOLEAN HFIOMdlRead (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset, IN ULONG Length,IN ULONG LockKey, OUT PMDL *MdlChain, OUT PIO_STATUS_BLOCK IoStatus,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for reading a file
    using MDLs as buffers.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object that is to be read.
    FileOffset - Supplies the offset into the file to begin the read operation.
    Length - Specifies the number of bytes to be read from the file.
    LockKey - The key to be used in byte range lock checks.
    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL chain built to describe the data read.
    IoStatus - Variable to receive the final status of the read operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,MdlRead)) {
		if(HFIOCommon(FASTIO_FUNCNUM(MdlRead),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->MdlRead( 
				FileObject,FileOffset,Length,LockKey,MdlChain,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOMdlReadComplete (IN PFILE_OBJECT FileObject,IN PMDL MdlChain, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for completing an
    MDL read operation.
    This function simply invokes the next driver's cooresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the MdlRead function is supported by the underlying driver, and
    therefore this function will also be supported, but this is not assumed
    by this driver.
Arguments:
    FileObject - Pointer to the file object to complete the MDL read upon.
    MdlChain - Pointer to the MDL chain used to perform the read operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE, depending on whether or not it is possible to invoke this function on the fast I/O path.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,MdlReadComplete)) {
		if(HFIOCommon(FASTIO_FUNCNUM(MdlReadComplete),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = (BOOLEAN) hookExt->FileSystem->DriverObject->FastIoDispatch->MdlReadComplete( FileObject, 
				MdlChain,hookExt->FileSystem);
		}
	}
	return retval;
}

BOOLEAN HFIOPrepareMdlWrite (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN ULONG LockKey,OUT PMDL *MdlChain, OUT PIO_STATUS_BLOCK IoStatus,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for preparing for an
    MDL write operation.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object that will be written.
    FileOffset - Supplies the offset into the file to begin the write 
        operation.
    Length - Specifies the number of bytes to be write to the file.
    LockKey - The key to be used in byte range lock checks.
    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL chain built to describe the data written.
    IoStatus - Variable to receive the final status of the write operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   IoStatus->Status      = STATUS_NOT_SUPPORTED;
   IoStatus->Information = 0;
   if(FASTIOPRESENT(hookExt,PrepareMdlWrite)) {
		if(HFIOCommon(FASTIO_FUNCNUM(PrepareMdlWrite),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->PrepareMdlWrite( 
				FileObject,FileOffset,Length,LockKey,MdlChain,IoStatus,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOMdlWriteComplete (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset, IN PMDL MdlChain,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for completing an
    MDL write operation.
    This function simply invokes the next driver's cooresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the PrepareMdlWrite function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not
    assumed by this driver.
Arguments:
    FileObject - Pointer to the file object to complete the MDL write upon.
    FileOffset - Supplies the file offset at which the write took place.
    MdlChain - Pointer to the MDL chain used to perform the write operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE, depending on whether or not it is possible to invoke this function on the fast I/O path.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,MdlWriteComplete)) {
		if(HFIOCommon(FASTIO_FUNCNUM(MdlWriteComplete),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->MdlWriteComplete( 
				FileObject,FileOffset,MdlChain,hookExt->FileSystem);
		}
	}
	return retval;
}

BOOLEAN HFIOReadCompressed (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset,IN ULONG Length,IN ULONG LockKey,
														OUT PVOID Buffer,OUT PMDL *MdlChain,OUT PIO_STATUS_BLOCK IoStatus,OUT struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
														IN ULONG CompressedDataInfoLength,IN PDEVICE_OBJECT DeviceObject)
/*++
    This routine is the fast I/O "pass through" routine for reading 
    compressed data from a file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object that will be read.
    FileOffset - Supplies the offset into the file to begin the read operation.
    Length - Specifies the number of bytes to be read from the file.
    LockKey - The key to be used in byte range lock checks.
    Buffer - Pointer to a buffer to receive the compressed data read.
    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL chain built to describe the data read.
    IoStatus - Variable to receive the final status of the read operation.
    CompressedDataInfo - A buffer to receive the description of the  compressed data.
    CompressedDataInfoLength - Specifies the size of the buffer described by the CompressedDataInfo parameter.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoReadCompressed)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoReadCompressed),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoReadCompressed( 
				FileObject,FileOffset,Length,LockKey,Buffer,MdlChain,IoStatus,CompressedDataInfo,CompressedDataInfoLength,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOWriteCompressed (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset,IN ULONG Length,IN ULONG LockKey, 
														 IN PVOID Buffer, OUT PMDL *MdlChain,OUT PIO_STATUS_BLOCK IoStatus,
														 IN struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
														 IN ULONG CompressedDataInfoLength, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for writing 
    compressed data to a file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object that will be written.
    FileOffset - Supplies the offset into the file to begin the write  operation.
    Length - Specifies the number of bytes to be write to the file.
    LockKey - The key to be used in byte range lock checks.
    Buffer - Pointer to the buffer containing the data to be written.
    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL chain built to describe the data written.
    IoStatus - Variable to receive the final status of the write operation.
    CompressedDataInfo - A buffer to containing the description of the compressed data.
    CompressedDataInfoLength - Specifies the size of the buffer described by the CompressedDataInfo parameter.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,FastIoWriteCompressed)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoWriteCompressed),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoWriteCompressed( 
				FileObject,FileOffset,Length,LockKey,Buffer,MdlChain,IoStatus,CompressedDataInfo,CompressedDataInfoLength,hookExt->FileSystem);
		} else {
			IoStatus->Status=STATUS_ACCESS_DENIED;
			IoStatus->Information=0;
		}
	}
	return retval;
}

BOOLEAN HFIOMdlReadCompleteCompressed (IN PFILE_OBJECT FileObject,IN PMDL MdlChain, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for completing an
    MDL read compressed operation.
    This function simply invokes the next driver's cooresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the read compressed function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not 
    assumed by this driver.
Arguments:
    FileObject - Pointer to the file object to complete the compressed read upon.
    MdlChain - Pointer to the MDL chain used to perform the read operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE, depending on whether or not it is possible to invoke this function on the fast I/O path.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,MdlReadCompleteCompressed)) {
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoWriteCompressed),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->MdlReadCompleteCompressed( 
				FileObject, MdlChain, hookExt->FileSystem );
		}
	}
	return retval;
}

BOOLEAN HFIOMdlWriteCompleteCompressed (IN PFILE_OBJECT FileObject,IN PLARGE_INTEGER FileOffset, IN PMDL MdlChain,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for completing a
    write compressed operation.
    This function simply invokes the next driver's cooresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the write compressed function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not 
    assumed by this driver.
Arguments:
    FileObject - Pointer to the file object to complete the compressed write upon.
    FileOffset - Supplies the file offset at which the file write operation began.
    MdlChain - Pointer to the MDL chain used to perform the write operation.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE, depending on whether or not it is possible to invoke this function on the fast I/O path.
--*/
{
BOOLEAN retval = FALSE;
PFSHOOK_EXTENSION hookExt;
   hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
   if(FASTIOPRESENT(hookExt,MdlWriteCompleteCompressed)) {
		if(HFIOCommon(FASTIO_FUNCNUM(MdlWriteCompleteCompressed),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->MdlWriteCompleteCompressed( 
				FileObject, FileOffset, MdlChain, hookExt->FileSystem );
		}
	}
	return retval;
}

BOOLEAN HFIOQueryOpen (IN struct _IRP *Irp,OUT PFILE_NETWORK_OPEN_INFORMATION NetworkInformation,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for opening a file
    and returning network information it.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    Irp - Pointer to a create IRP that represents this open operation.  It is to be used by the file system for 
		  common open/create code, but not actually completed.
    NetworkInformation - A buffer to receive the information required by the network about the file being opened.
    DeviceObject - Pinter to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is TRUE or FALSE based on whether or not fast I/O is possible for this file.
--*/
/*Так падает!!! Стек НАДО наращивать :)
{
BOOLEAN					retval = FALSE;
PFSHOOK_EXTENSION		hookExt;
PIO_STACK_LOCATION	currentIrpStack;
PIO_STACK_LOCATION	nextIrpStack;
	hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	if(FASTIOPRESENT(hookExt,FastIoQueryOpen)) {
		currentIrpStack=IoGetCurrentIrpStackLocation(Irp);
//		nextIrpStack=IoGetNextIrpStackLocation(Irp);
//		*nextIrpStack=*currentIrpStack;
//IoCopyCurrentIrpStackLocationToNext(Irp); //!!????????????????????????
//		nextIrpStack->DeviceObject = hookExt->FileSystem;
//		IoSetNextIrpStackLocation(Irp);
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoQueryOpen),currentIrpStack->FileObject,hookExt)==STATUS_SUCCESS) {
			retval=hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoQueryOpen(Irp,NetworkInformation,hookExt->FileSystem);
		} else {
			Irp->IoStatus.Status=STATUS_ACCESS_DENIED;
			Irp->IoStatus.Information=0;
		}
//IoSkipCurrentStackLocation(Irp);//????????????????
//		Irp->CurrentLocation++;
//		Irp->Tail.Overlay.CurrentStackLocation++;
	}
	return retval;
}*/
{
BOOLEAN					retval = FALSE;
PFSHOOK_EXTENSION		hookExt;
PIO_STACK_LOCATION	currentIrpStack;
PIO_STACK_LOCATION	nextIrpStack;
PFILE_OBJECT			fileObject;
	hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	if(FASTIOPRESENT(hookExt,FastIoQueryOpen)) {
		currentIrpStack=IoGetCurrentIrpStackLocation(Irp);
		fileObject=currentIrpStack->FileObject;
		nextIrpStack=IoGetNextIrpStackLocation(Irp);
		*nextIrpStack=*currentIrpStack;
//IoCopyCurrentIrpStackLocationToNext(Irp); //????????????????????????
		nextIrpStack->DeviceObject = hookExt->FileSystem;
		IoSetNextIrpStackLocation(Irp);
		if(HFIOCommon(FASTIO_FUNCNUM(FastIoQueryOpen),fileObject,hookExt)==STATUS_SUCCESS) {
			retval=hookExt->FileSystem->DriverObject->FastIoDispatch->FastIoQueryOpen(Irp,NetworkInformation,hookExt->FileSystem);
		} else {
			Irp->IoStatus.Status=STATUS_ACCESS_DENIED;
			Irp->IoStatus.Information=0;
		}
//IoSkipCurrentStackLocation(Irp);//????????????????
		Irp->CurrentLocation++;
		Irp->Tail.Overlay.CurrentStackLocation++;
	}
	if(!retval || !NT_SUCCESS(Irp->IoStatus.Status))
		NameCacheFree(g_pCommon_NameCache, HASH4FOBJ(fileObject));
	return retval;
}


NTSTATUS	HFIOReleaseForModWrite (IN PFILE_OBJECT FileObject,IN struct _ERESOURCE *ResourceToRelease, IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for releasing the
    resource previously acquired for performing a modified write operation
    to a file.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object whose resource is to be released.
    ResourceToRelease - Specifies the modified writer resource for the file that is to be released.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is either success or failure based on whether or not fast I/O is possible for this file.
--*/
{
NTSTATUS retval = STATUS_NOT_SUPPORTED;
PFSHOOK_EXTENSION hookExt;
	hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	if(FASTIOPRESENT(hookExt,ReleaseForModWrite)) {
		if(HFIOCommon(FASTIO_FUNCNUM(ReleaseForModWrite),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->ReleaseForModWrite(FileObject,ResourceToRelease,hookExt->FileSystem);
		} else {
			retval = STATUS_ACCESS_DENIED;
		}
	}
	return retval;
}

NTSTATUS	HFIOAcquireForCcFlush (IN PFILE_OBJECT FileObject,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for acquiring the
    appropriate file system resource prior to a call to CcFlush.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object whose resource is to be acquired.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is either success or failure based on whether or not fast I/O is possible for this file.
--*/
{
NTSTATUS retval = STATUS_NOT_SUPPORTED;
PFSHOOK_EXTENSION hookExt;
	hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	if(FASTIOPRESENT(hookExt,AcquireForCcFlush)) {
		if(HFIOCommon(FASTIO_FUNCNUM(AcquireForCcFlush),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->AcquireForCcFlush(FileObject,hookExt->FileSystem);
		} else {
			retval = STATUS_ACCESS_DENIED;
		}
	}
	return retval;
}

NTSTATUS HFIOReleaseForCcFlush (IN PFILE_OBJECT FileObject,IN PDEVICE_OBJECT DeviceObject )
/*++
    This routine is the fast I/O "pass through" routine for releasing the
    appropriate file system resource previously acquired for a CcFlush.
    This function simply invokes the next driver's cooresponding routine, or
    returns FALSE if the next driver does not implement the function.
Arguments:
    FileObject - Pointer to the file object whose resource is to be released.
    DeviceObject - Pointer to this driver's device object, the device on which the operation is to occur.
Return Value:
    The function value is either success or failure based on whether or not fast I/O is possible for this file.
--*/
{
NTSTATUS retval = STATUS_NOT_SUPPORTED;
PFSHOOK_EXTENSION hookExt;
	hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;
	if(FASTIOPRESENT(hookExt,ReleaseForCcFlush)) {
		if(HFIOCommon(FASTIO_FUNCNUM(ReleaseForCcFlush),FileObject,hookExt)==STATUS_SUCCESS) {
			retval = hookExt->FileSystem->DriverObject->FastIoDispatch->ReleaseForCcFlush(FileObject,hookExt->FileSystem);
		} else {
			retval = STATUS_ACCESS_DENIED;
		}
	}
	return retval;
}

FAST_IO_DISPATCH FastIOHook={
	sizeof(FAST_IO_DISPATCH), 
	HFIOCheckifPossible,
	HFIORead,
	HFIOWrite,
	HFIOQueryBasicInfo,
	HFIOQueryStandardInfo,
	HFIOLock,
	HFIOUnlockSingle,
	HFIOUnlockAll,
	HFIOUnlockAllByKey,
	HFIODeviceControl,
	HFIOAcquireFile,
	HFIOReleaseFile,
	HFIODetachDevice,
	HFIOQueryNetworkOpenInfo,
	HFIOAcquireForModWrite,
	HFIOMdlRead,
	HFIOMdlReadComplete,
	HFIOPrepareMdlWrite,
	HFIOMdlWriteComplete,
	HFIOReadCompressed,
	HFIOWriteCompressed,
	HFIOMdlReadCompleteCompressed,
	HFIOMdlWriteCompleteCompressed,
	HFIOQueryOpen,
	HFIOReleaseForModWrite,
	HFIOAcquireForCcFlush,
	HFIOReleaseForCcFlush
};


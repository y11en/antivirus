#include "stdafx.h"

void* _cdecl operator new(UINT size_)
{
        return new(POOL) BYTE[size_];
}


LRESULT WINAPI _AvpCallback(WPARAM wParam,LPARAM lParam)
{
        return 0;
}
_TCHAR* WINAPI _AvpStrlwr(_TCHAR* str_)
{
        int l=strlen(str_)+1;
        KUstring ss(str_, POOL);
        KUstring ds(l*2,POOL);
        RtlUpcaseUnicodeString((PUNICODE_STRING)ds,(PUNICODE_STRING)ss,FALSE);
//      _DebugTrace(TraceInfo,"AvpStrlwr: %.40s\n", str_);
        ds.ToPsz(str_,l);
//      _DebugTrace(TraceInfo,"AvpStrlwr: %.40s\n", str_);

        return str_;
}

_TCHAR* WINAPI _AvpStrrchr(_TCHAR* str_, INT chr_)
{

        KUstring ss(str_, POOL);
        UINT i=ss.Length();
		UINT j=0;
        WCHAR* c=&ss.Last();
        while(i)
        {
			j++;
            i--;
            if(*c==(WCHAR)chr_ )
			{
				char* buf;
				ss.ShortenBy(j);
				UINT l=ss.Size(); //it is size of unicode in bytes, it is definitely more than MBCS size.
				buf=new char[l];
				if(buf){
					ss.ToPsz(buf,l);
					i=strlen(buf);
					delete buf;
					_DebugTrace(TraceInfo,"AvpStrchr: %c %d: %s \n",chr_, i, str_ );
					return str_+i;
				}
				else break;
			}
            c--;

        }
        return NULL;
/*
#ifdef _MBCS
        return (char*)_mbsrchr((const unsigned char*)str_,chr_);
#else
        if(str_)
        {
                _TCHAR* c=str_ + strlen(str_);
                while(c>=str_)
                {
                        if(*c==chr_){
                                        return c;
                        }
                        c--;
                }
        }
        return NULL;
#endif
*/
}

_TCHAR* WINAPI _AvpConvertChar(_TCHAR* str_,INT ct,_TCHAR* cp_)
{
        switch(ct)
        {
        case 5: //Unicode
			break;
        case 1: //No convertion
			break;
        case 0: //OemToAnsi
        default:
            break;

        }
        return str_;
}

/*
static DWORD dwBytesPerSector=512;
static DWORD dwSectorsPerTrack=0;
static DWORD dwTracksPerCylinder=0;
static DWORD dwNumCylinders=0;
*/

NTSTATUS GetDiskGeometry( char* szDeviceName, PDISK_GEOMETRY dg)
{
        ANSI_STRING                     ansiDeviceName;
        UNICODE_STRING          uniDeviceName;
//        HANDLE                          hFileHandle;
        OBJECT_ATTRIBUTES       ObjectAttributes;
        IO_STATUS_BLOCK         IoStatus;
        PIRP                            irp;
        KEVENT                          event;
        PDEVICE_OBJECT          deviceObject;
        PFILE_OBJECT            fileObject;
        NTSTATUS                        status=STATUS_IO_DEVICE_ERROR;
		BOOL verify=0;

        if(dg==NULL) return status;
		memset(dg,0,sizeof(DISK_GEOMETRY));

		DISK_GEOMETRY* Geometry =(DISK_GEOMETRY*) ExAllocatePool(NonPagedPool,sizeof(DISK_GEOMETRY));
        if(!Geometry) goto ex2;

        RtlInitAnsiString(&ansiDeviceName,(PSTR)szDeviceName);
        RtlAnsiStringToUnicodeString(&uniDeviceName,&ansiDeviceName,TRUE);
        InitializeObjectAttributes(&ObjectAttributes,&uniDeviceName,
                OBJ_CASE_INSENSITIVE,NULL,NULL);
again:
        status= IoGetDeviceObjectPointer(
                &uniDeviceName,
                FILE_READ_ATTRIBUTES,
                &fileObject,
                &deviceObject);

        if(!NT_SUCCESS(status)){
			goto ex;
		}

		if (fileObject->DeviceObject->DeviceType != FILE_DEVICE_DISK)
		{
			ObDereferenceObject(fileObject);
            status=STATUS_IO_DEVICE_ERROR;
			goto ex;
		}

        deviceObject=fileObject->DeviceObject;

    irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                        deviceObject,
                                        NULL,
                                        0,
                                        Geometry,
                                        sizeof(DISK_GEOMETRY),
                                        FALSE,
                                        &event,
                                        &IoStatus);



        ObDereferenceObject(fileObject);

        if(!irp)
        {
                status|=STATUS_IO_DEVICE_ERROR;
                goto ex;
        }

        KeInitializeEvent(&event,NotificationEvent,FALSE);
        status = IoCallDriver(deviceObject,irp);


        if(status==STATUS_PENDING)
        {
			KeWaitForSingleObject(&event,Suspended,KernelMode,FALSE,NULL);
            status= IoStatus.Status;
        }
		if (status == STATUS_VERIFY_REQUIRED)
			if(!verify++) goto again;

        if (!NT_SUCCESS(status)) goto ex;
		status=IoStatus.Status;

        memcpy(dg,Geometry,sizeof(DISK_GEOMETRY));

ex:
        RtlFreeUnicodeString(&uniDeviceName);
        ExFreePool(Geometry);
ex2:

		if(dg->BytesPerSector==0)
        {
			if(NT_SUCCESS(status))
				status=STATUS_IO_DEVICE_ERROR;
            dg->BytesPerSector=0x200;
            dg->TracksPerCylinder=80;
            dg->SectorsPerTrack=18;
			dg->Cylinders.LowPart=2;
        }
		_DebugTrace(TraceInfo,"GetDiskGeometry  %d bps:%d spt:%d tpc:%d    %s\n",NT_SUCCESS(status),
              dg->BytesPerSector,dg->SectorsPerTrack,dg->TracksPerCylinder,szDeviceName);

        return status;
}

NTSTATUS NTSectorIO( char* szDeviceName, LARGE_INTEGER* liSeekPos, DWORD dwBytes, BYTE* buffer, int wr)
{
        ANSI_STRING                     ansiDeviceName;
        UNICODE_STRING          uniDeviceName;
//        HANDLE                          hFileHandle;
        OBJECT_ATTRIBUTES       ObjectAttributes;
        IO_STATUS_BLOCK         IoStatus;
        NTSTATUS                        status;
        PIRP                            irp;
        KEVENT                          event;
        PDEVICE_OBJECT          deviceObject;
        PFILE_OBJECT            fileObject;
		BOOL verify=0;


        RtlInitAnsiString(&ansiDeviceName,(PSTR)szDeviceName);
        RtlAnsiStringToUnicodeString(&uniDeviceName,&ansiDeviceName,TRUE);
        InitializeObjectAttributes(&ObjectAttributes,&uniDeviceName,
                OBJ_CASE_INSENSITIVE,NULL,NULL);
again:
        status= IoGetDeviceObjectPointer(
                &uniDeviceName,
                FILE_READ_ATTRIBUTES,
                &fileObject,
                &deviceObject);



        if(!NT_SUCCESS(status)){
			RtlFreeUnicodeString(&uniDeviceName);
            return status;
        }

		if (fileObject->DeviceObject->DeviceType != FILE_DEVICE_DISK)
		{
			RtlFreeUnicodeString(&uniDeviceName);
			ObDereferenceObject(fileObject);
            status=STATUS_IO_DEVICE_ERROR;
            return status;
		}


        deviceObject=fileObject->DeviceObject;

        irp=IoBuildSynchronousFsdRequest(
                wr?IRP_MJ_WRITE:IRP_MJ_READ,
                deviceObject,
                buffer,
                dwBytes,
                liSeekPos,
                &event,
                &IoStatus);

        ObDereferenceObject(fileObject);


        if(!irp)
        {
                RtlFreeUnicodeString(&uniDeviceName);
                return STATUS_IO_DEVICE_ERROR;
        }

        KeInitializeEvent(&event,NotificationEvent,FALSE);
        status = IoCallDriver(deviceObject,irp);


        if(status==STATUS_PENDING)
        {
			KeWaitForSingleObject(&event,Suspended,KernelMode,FALSE,NULL);
            status= IoStatus.Status;
        }

		if (status == STATUS_VERIFY_REQUIRED)
			if(!verify++) goto again;


        RtlFreeUnicodeString(&uniDeviceName);


        if (!NT_SUCCESS(status))
                return status;
        if (!NT_SUCCESS(IoStatus.Status))
                return status;

#if 0 // dump sector
        {
                int                                     count,a,b,c,i;
                count =0;
                for (i=0; i<32; i++)
                {
                        for (a=count; a<count+8; a++)
                                DbgPrint("%02X ",buffer[a]);
                        DbgPrint(" - ");
                        for (b=count+8; b<count+16; b++)
                                DbgPrint("%02X ",buffer[b]);
                        DbgPrint("    ");
                        for (c=count;c<count+16;c++)
                                if(buffer[c] >= ' ' )
                                DbgPrint("%c",buffer[c]);
                        else
                                DbgPrint(".");

                        DbgPrint("\n");
                        count+=16;
                }
        }
 #endif

        return status;
}

int MakeDeviceName(BYTE Disk, char* buf, int szBuf)
{
	BYTE d;
    if(szBuf< 0x20) return 0;
    d=Disk & 0x1F;
    if(Disk & 0x80) 
		return	sprintf(buf,"\\Device\\Harddisk%d\\Partition0",d);
	return		sprintf(buf,"\\DosDevices\\%c:",'A'+d);
}

BOOL IO13(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, LPBYTE Buffer, int wr)
{
	BOOL ret=FALSE;
	LPBYTE buf=NULL;
	char deviceName[0x20];
    MakeDeviceName(Disk,deviceName,0x20);

    DISK_GEOMETRY Geometry;
    GetDiskGeometry(deviceName,&Geometry);

    DWORD SecNo=Sector & 0x003F;
    DWORD CylNo=Sector<<2;
    CylNo&=0x0300;
    CylNo|=Sector>>8;
    LARGE_INTEGER pos;
    pos.HighPart= 0;
    pos.LowPart = (CylNo*Geometry.TracksPerCylinder + Head)*Geometry.SectorsPerTrack + SecNo -1;

    int i=0;
    int s=Geometry.BytesPerSector;
    while(1){
            s>>=1;
            if(s) i++;
            else break;
    }
    pos=RtlLargeIntegerShiftLeft(pos,i);

    _Trace(TraceInfo,"IO13: %28.28s %02X c:%03X s:%02X h:%02X\n", deviceName, Disk, CylNo, SecNo, Head);

	if(Geometry.BytesPerSector > 0x200)
	{
		buf=new BYTE[NumSectors*Geometry.BytesPerSector];
		if(!buf)return ret;
		if(wr)memcpy(buf,Buffer,NumSectors*0x200);
	}
    ret=NT_SUCCESS(NTSectorIO(deviceName,&pos,NumSectors*Geometry.BytesPerSector,buf?buf:Buffer,wr));
	if(buf)
	{
		if(!wr)memcpy(Buffer,buf,NumSectors*0x200);
		delete buf;
	}

    return ret;
}

BOOL WINAPI _AvpRead13 (BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, LPBYTE Buffer)
{
        return IO13(Disk, Sector, Head, NumSectors, Buffer, 0);
}

BOOL WINAPI _AvpWrite13(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, LPBYTE Buffer)
{
        return IO13(Disk, Sector, Head, NumSectors, Buffer, 1);
}

BOOL IO2X(BYTE Drive, WORD Sector ,WORD NumSectors, LPBYTE Buffer, int wr)
{
	BOOL ret=FALSE;
	LPBYTE buf=NULL;
    char deviceName[0x20];
    MakeDeviceName(Drive,deviceName,0x20);

	DISK_GEOMETRY Geometry;
    GetDiskGeometry(deviceName,&Geometry);

    LARGE_INTEGER pos;
    pos.HighPart= 0;
    pos.LowPart = Sector;

    int i=0;
    int s=Geometry.BytesPerSector;
    while(1){
            s>>=1;
            if(s) i++;
            else break;
    }
    pos=RtlLargeIntegerShiftLeft(pos,i);

    _Trace(TraceInfo,"IO2X: %28.28s %02X s:%d\n", deviceName, Drive, Sector);

	if(Geometry.BytesPerSector > 0x200)
	{
		buf=new BYTE[NumSectors*Geometry.BytesPerSector];
		if(!buf)return ret;
		if(wr)memcpy(buf,Buffer,NumSectors*0x200);
	}
    ret=NT_SUCCESS(NTSectorIO(deviceName,&pos,NumSectors*Geometry.BytesPerSector,buf?buf:Buffer,wr));
	if(buf)
	{
		if(!wr)memcpy(Buffer,buf,NumSectors*0x200);
		delete buf;
	}

    return ret;
}

BOOL WINAPI _AvpRead25(
        BYTE Drive,
        DWORD Sector,
        WORD NumSectors,
        LPBYTE Buffer
        )
{
        return IO2X(Drive, (WORD)Sector, NumSectors, Buffer, 0);
}


BOOL WINAPI _AvpWrite26(
        BYTE Drive,
        DWORD Sector,
        WORD NumSectors,
        LPBYTE Buffer
        )
{
        return IO2X(Drive, (WORD)Sector, NumSectors, Buffer, 1);
}

BOOL WINAPI _AvpGetDiskParam (
    BYTE   disk,
    BYTE   drive,
    WORD*   CX,
    BYTE*   DH
        )
{
	DISK_GEOMETRY Geometry;
    char dev[40];
	DWORD Num_Cylinders;
	DWORD Num_Sec_Per_Track;
	DWORD Num_Heads;
	BOOL ret=FALSE;
    if(CX)*CX=0;
	if(DH)*DH=0;
	_DebugTrace(TraceInfo,"AvpGetDiskParam begin disk %X drive %X\n",disk,drive);
	
	
    MakeDeviceName(disk,dev,0x40);
    NTSTATUS stat=GetDiskGeometry(dev,&Geometry);
	
//	if(stat==STATUS_NO_MEDIA_IN_DEVICE) goto ret;
	if(!NT_SUCCESS(stat)) goto ret;

	if(disk<0x20){
		BYTE* Buffer;
		Buffer=new BYTE[0x200];
		if(!Buffer)goto ret;

		_DebugTrace(TraceInfo,"AvpRead25 try\n");
		if(!_AvpRead25(drive,0,1,Buffer))
		{
			delete Buffer;
			goto ret; //drive not ready.
		}
		delete Buffer;

		_DebugTrace(TraceInfo,"AvpRead25 succeeded\n");

//		if(!NT_SUCCESS(stat)){ //only supports int25 
			ret=TRUE;
//		}
	}
	

	if(!NT_SUCCESS(stat)) goto ret;

	Num_Cylinders = Geometry.Cylinders.LowPart;
	Num_Sec_Per_Track = Geometry.SectorsPerTrack;
	Num_Heads = Geometry.TracksPerCylinder;

	if((Num_Cylinders) < 0x400
	&& (Num_Sec_Per_Track) < 0x40
	&& (Num_Heads) <0x100){
		WORD w=((BYTE)(Num_Cylinders -1))<<8;
		w|=(0xC0)&((Num_Cylinders -1)>>2);
		w|=(0x3F)&(BYTE)(Num_Sec_Per_Track);
		if(CX)*CX=w;
		if(DH)*DH=(BYTE)(Num_Heads -1);
		ret=TRUE;
	}

ret:	
	//      _DebugTrace(TraceInfo,"AvpGetDiskParam: disk:%X CX:%X, DH:%X\n",disk,CX?*CX:-1,DH?*DH:-1);
	return ret;

}



DWORD WINAPI _AvpGetFileSize(
    HANDLE  hFile,      // handle of file
    LPDWORD  lpHigh
        )
{
        NTSTATUS ntStatus;
        IO_STATUS_BLOCK IoStatus;
        FILE_STANDARD_INFORMATION FileInformation;

        ntStatus = ZwQueryInformationFile( hFile,
			&IoStatus,
			&FileInformation,
			sizeof( FILE_STANDARD_INFORMATION ),
			FileStandardInformation );

        if ( ntStatus == STATUS_SUCCESS ){
			if(lpHigh!=NULL)*lpHigh=FileInformation.EndOfFile.HighPart;
			return ( FileInformation.EndOfFile.LowPart );
        }

        return (DWORD)-1L;
}





BOOL WINAPI _AvpReadFile (
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,        // number of bytes to read
    LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
    OVERLAPPED FAR*  lpOverlapped       // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
)
{
        NTSTATUS ntStatus;
        IO_STATUS_BLOCK IoStatus;
        FILE_POSITION_INFORMATION FileInformation;
		if(lpNumberOfBytesRead)*lpNumberOfBytesRead=0;

        DWORD fLen=_AvpGetFileSize(hFile,NULL);
        if (fLen==-1)return FALSE;

        ntStatus = ZwQueryInformationFile( hFile,
                   &IoStatus,
                   &FileInformation,
                   sizeof( FILE_POSITION_INFORMATION ),
                   FilePositionInformation );

        if (!NT_SUCCESS(ntStatus))return FALSE;

        if(fLen <= FileInformation.CurrentByteOffset.LowPart )
            nNumberOfBytesToRead=0;
		else{
			DWORD nBytesToEnd = fLen-FileInformation.CurrentByteOffset.LowPart;
			if( nBytesToEnd < nNumberOfBytesToRead)
					nNumberOfBytesToRead = nBytesToEnd;
		}


        if(nNumberOfBytesToRead == 0){
                *lpNumberOfBytesRead=0;
                return TRUE;
        }

        ntStatus = ZwReadFile( hFile,
			NULL,
			NULL,
			NULL,
			&IoStatus,
			lpBuffer,
			nNumberOfBytesToRead,
			NULL,
			NULL );

        if(lpNumberOfBytesRead)*lpNumberOfBytesRead=IoStatus.Information;

//      _DebugTrace(TraceInfo,"AvpReadFile %d %d:%d Ok:%d\n",(int)hFile,nNumberOfBytesToRead,*lpNumberOfBytesRead,NT_SUCCESS(ntStatus));
        if (NT_SUCCESS(ntStatus))
                return TRUE;
        return FALSE;

}

BOOL WINAPI _AvpWriteFile (
    HANDLE  hFile,      // handle to file to write to
    LPCVOID  lpBuffer,  // pointer to data to write to file
    DWORD  nNumberOfBytesToWrite,       // number of bytes to write
    LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
        )
{
        NTSTATUS ntStatus;
        IO_STATUS_BLOCK IoStatus;
        FILE_POSITION_INFORMATION FileInformation;
        if(lpNumberOfBytesWritten)*lpNumberOfBytesWritten=0;
		
        DWORD fLen=_AvpGetFileSize(hFile,NULL);
        if (fLen==-1)
			return FALSE;

        ntStatus = ZwQueryInformationFile( hFile,
               &IoStatus,
               &FileInformation,
               sizeof( FILE_POSITION_INFORMATION ),
               FilePositionInformation );
        if (!NT_SUCCESS(ntStatus))
			return FALSE;

		if (fLen < FileInformation.CurrentByteOffset.LowPart ){
			DWORD count;
			DWORD diff= FileInformation.CurrentByteOffset.LowPart - fLen;
			BYTE* buf=new BYTE[0x4000];
			if(buf==NULL) 
				return FALSE;
			memset(buf,0,0x4000);

			if ( fLen != _AvpSetFilePointer(hFile,fLen,0,SEEK_SET)){
					delete buf;
					return FALSE;
			}
//			if( !_AvpWriteFile(hFile,buf,diff,&count,0)){
//					delete buf;
//					return FALSE;
//			}
			while(diff){
				if( !_AvpWriteFile(hFile,buf,diff>0x4000?0x4000:diff,&count,0)){
					delete buf;
					return FALSE;
				}
				diff-=count;
			}
			delete buf;
		}
/*
		BYTE* lpBuf=(BYTE*)lpBuffer;
		DWORD ToWrite=nNumberOfBytesToWrite;
		*lpNumberOfBytesWritten=0;
		while(ToWrite)
		{

			ntStatus = ZwWriteFile( hFile,
				   NULL,
				   NULL,
				   NULL,
				   &IoStatus,
				   (void*)lpBuf,
				   ToWrite>0x4000?0x4000:ToWrite,
				   NULL,
				   NULL );

	        if (!NT_SUCCESS(ntStatus))
				break;

			*lpNumberOfBytesWritten+=IoStatus.Information;
			lpBuf+=IoStatus.Information;
			ToWrite-=IoStatus.Information;
		}
*/
        ntStatus = ZwWriteFile( hFile,
               NULL,
               NULL,
               NULL,
               &IoStatus,
               (void*)lpBuffer,
               nNumberOfBytesToWrite,
			   NULL,
               NULL );

        if(lpNumberOfBytesWritten)*lpNumberOfBytesWritten=IoStatus.Information;

//        _DebugTrace(TraceInfo,"AvpWriteFile %d %d:%d Ok:%d\n",(int)hFile,nNumberOfBytesToWrite,*lpNumberOfBytesWritten,NT_SUCCESS(ntStatus));

        if (NT_SUCCESS(ntStatus))
                return TRUE;
        return FALSE;

}

static BOOL MyMakeUnicodeString(PUNICODE_STRING pUniFilename,LPCSTR szFilename){
	ANSI_STRING ansiFilename;
	if(strlen(szFilename)>1)
	{
		RtlInitAnsiString( &ansiFilename, szFilename );
		RtlAnsiStringToUnicodeString(  pUniFilename, &ansiFilename, TRUE);
		return TRUE;
	}
	RtlInitUnicodeString( pUniFilename, (PCWSTR)szFilename );
	return FALSE;
	
}

int OpenFile( char *szFilename, int nDesiredAccess, ULONG ulShareAccess, ULONG ulCreateDisposition )
{
	// Example call: int fhandle = OpenFile(uncFilename, FILE_GENERIC_READ|SYNCHRONIZE, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, FILE_OPEN);

	NTSTATUS ntStatus;
	HANDLE FileHandle=(HANDLE)-1;
	OBJECT_ATTRIBUTES ObjectAttributes;
	IO_STATUS_BLOCK IoStatus;

	UNICODE_STRING uniFilename;
	BOOL bUniAlloc=MyMakeUnicodeString(&uniFilename,szFilename);


	InitializeObjectAttributes( &ObjectAttributes, &uniFilename, 
								OBJ_CASE_INSENSITIVE, NULL, NULL );

//    _DebugTrace(TraceInfo,"ZwCreateFile acc=%08X shar=%08X disp=%08X : %s\n", nDesiredAccess, ulShareAccess, ulCreateDisposition, szFilename);


	ntStatus = ZwCreateFile( &FileHandle, 
							 nDesiredAccess|SYNCHRONIZE,
							 &ObjectAttributes,
							 &IoStatus,
							 NULL,
							 FILE_ATTRIBUTE_NORMAL,
							 ulShareAccess,
							 ulCreateDisposition,
							 FILE_SYNCHRONOUS_IO_NONALERT|FILE_COMPLETE_IF_OPLOCKED,
							 NULL,
							 0 );
	if(bUniAlloc)
		RtlFreeUnicodeString( &uniFilename );

#ifdef GDATA
	if ( NT_SUCCESS(ntStatus) ) 
#endif //GDATA
	if ( ntStatus == STATUS_SUCCESS ) 
	{
		return ( ( int )FileHandle );
	}
	else if (FileHandle != (HANDLE)-1)
//	(ntStatus == STATUS_OPLOCK_BREAK_IN_PROGRESS|| ntStatus == STATUS_DELETE_PENDING ) 
	{
		ZwClose( FileHandle );
	}

    _Trace(TraceInfo,"ZwCreateFile FFAILED: NtStatus=%08X  :%s\n", ntStatus, szFilename);

	return -1;
}


static int NumberHandles=0;

HANDLE WINAPI _AvpCreateFile (
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD  dwDesiredAccess,     // access (read-write)
    DWORD  dwShareMode, // share mode
    SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
    DWORD  dwCreationDistribution,      // how to create
    DWORD  dwFlagsAndAttributes,        // file attributes
    HANDLE  hTemplateFile       // handle to file with attributes to copy
        )
{

//        _DebugTrace(TraceInfo,"AvpCreateFile %d: %s access:%08X\n",lpFileName,lpFileName,dwDesiredAccess);

        int access_=0;
        switch(dwCreationDistribution)
        {
        case CREATE_NEW:
                access_=FILE_CREATE;
                break;
        case CREATE_ALWAYS:
                access_=FILE_OVERWRITE_IF;
                break;
        case OPEN_EXISTING:
                access_=FILE_OPEN;
                break;
        case OPEN_ALWAYS:
                access_=FILE_OPEN_IF;
                break;
        case TRUNCATE_EXISTING:
                access_=FILE_OVERWRITE;
                break;
        default:
                access_=FILE_OPEN;
                break;
        }
//		if(dwShareMode & FILE_SHARE_WRITE) dwShareMode|=FILE_SHARE_DELETE;
		dwShareMode |= FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE;

        HANDLE r=(HANDLE)OpenFile((LPTSTR)lpFileName,dwDesiredAccess,dwShareMode,access_);

		if(r!=INVALID_HANDLE_VALUE)
		{
			if(NumberHandles>24)
            {
                    ZwClose( r );
                    r=INVALID_HANDLE_VALUE;
            }
            else NumberHandles++;
        }

        _DebugTrace(TraceInfo,"AvpCreateFile %d :%s\n",r,lpFileName);

        return r;

}

BOOL WINAPI _AvpCloseHandle(
    HANDLE  hFile       // handle to object to close
        )
{
	BOOL ret=NT_SUCCESS(ZwClose( hFile ));

    if(ret && NumberHandles)NumberHandles--;
	

	_DebugTrace(TraceInfo,"AvpCloseHandle %d     Ok:%d\n",(int)hFile, ret);
    return TRUE;
}


DWORD WINAPI _AvpSetFilePointer(
    HANDLE  hFile,      // handle of file
    LONG  lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh,        // address of high-order word of distance to move
    DWORD  dwMoveMethod         // how to move
        )
{
        NTSTATUS ntStatus;
        IO_STATUS_BLOCK IoStatus;
        FILE_POSITION_INFORMATION FileInformation;
        DWORD newPos;
        DWORD fLen=_AvpGetFileSize(hFile,NULL);
		if (fLen==-1)return -1;

        ntStatus = ZwQueryInformationFile( hFile,
			&IoStatus,
			&FileInformation,
			sizeof( FILE_POSITION_INFORMATION ),
			FilePositionInformation );
        if (!NT_SUCCESS(ntStatus))return -1;

        switch ( dwMoveMethod ) {
                case 1:
                        newPos= FileInformation.CurrentByteOffset.LowPart + lDistanceToMove;
                        break;

                case 2:
                        newPos= fLen + lDistanceToMove;
                        break;
                case 0:
                default:
                        newPos= lDistanceToMove;
                        break;
        }
		if( (lDistanceToMove < 0) && (newPos & 0x80000000))	return -1;

        FileInformation.CurrentByteOffset.LowPart = newPos;


        ntStatus = ZwSetInformationFile( hFile,
			 &IoStatus,
			 &FileInformation,
			 sizeof( FILE_POSITION_INFORMATION ),
			 FilePositionInformation );

/*
      _DebugTrace(TraceInfo,"AvpSetFilePointer h=%d met=%d ofs=%d :pos=%d ok=%d\n",
                (int)hFile,
                dwMoveMethod,
                lDistanceToMove,
                FileInformation.CurrentByteOffset.LowPart,
                NT_SUCCESS(ntStatus)
                );
*/
    if(!NT_SUCCESS(ntStatus))
		return -1;
        return FileInformation.CurrentByteOffset.LowPart;
}


BOOL WINAPI _AvpSetEndOfFile(
    HANDLE  hFile       // handle of file
        )
{
/*	DWORD count;
	_AvpWriteFile(hFile,"",0,&count,0);
	return TRUE;
*/
        NTSTATUS ntStatus;
        IO_STATUS_BLOCK IoStatus;
        FILE_POSITION_INFORMATION FileInformation;
//        DWORD newPos;
        DWORD fLen=_AvpGetFileSize(hFile,NULL);
        if (fLen==-1)return FALSE;

        ntStatus = ZwQueryInformationFile( 
			hFile,
			&IoStatus,
			&FileInformation,
			sizeof( FILE_POSITION_INFORMATION ),
			FilePositionInformation );

        if (!NT_SUCCESS(ntStatus))return FALSE;

        ntStatus = ZwSetInformationFile( 
			hFile,
			&IoStatus,
			&FileInformation,
			sizeof( FILE_END_OF_FILE_INFORMATION ),
			FileEndOfFileInformation );



    if(!NT_SUCCESS(ntStatus))
		return FALSE;
    return TRUE;
}



//additional
BOOL WINAPI _AvpGetDiskFreeSpace(
        LPCTSTR lpRootPathName, // address of root path
    LPDWORD lpSectorsPerCluster,        // address of sectors per cluster
    LPDWORD lpBytesPerSector,   // address of bytes per sector
    LPDWORD lpNumberOfFreeClusters,     // address of number of free clusters
    LPDWORD lpTotalNumberOfClusters     // address of total number of clusters
        )
{
        *lpSectorsPerCluster=32;
    *lpBytesPerSector=512;
    *lpNumberOfFreeClusters=0x1000;
    *lpTotalNumberOfClusters=0xF000;

    return 1;
}


DWORD WINAPI _AvpGetFullPathName(
        LPCTSTR lpFileName,     // address of name of file to find path for
    DWORD nBufferLength,        // size, in characters, of path buffer
    LPTSTR lpBuffer,    // address of path buffer
    LPTSTR *lpFilePart  // address of filename in path
        )
{
        DWORD l;
        strncpy(lpBuffer,lpFileName,nBufferLength);
        l=strlen(lpBuffer);
                *lpFilePart=_AvpStrrchr(lpBuffer,'\\');
        if(*lpFilePart) (*lpFilePart)++;
                else *lpFilePart=lpBuffer + l;

//        _DebugTrace(TraceInfo,"AvpGetFullPathName %s (%s) %s\n",lpFileName, *lpFilePart,lpBuffer);
        return l;
}


BOOL WINAPI _AvpDeleteFile(
        LPCTSTR lpFileName      // pointer to name of file to delete
        )
{
        NTSTATUS ntStatus;
        HANDLE FileHandle;
        OBJECT_ATTRIBUTES ObjectAttributes;
        IO_STATUS_BLOCK IoStatus;
        FILE_DISPOSITION_INFORMATION FileInformation;
		

        _DebugTrace(TraceInfo,"AvpDeleteFile (%s)\n", lpFileName);

//return FALSE;

        // The string coming through here is the fully qualified driver path in ASCII.
        // For now we'll display that driver path in all output text.
        // Convert it back to unicode.
        //
		UNICODE_STRING uniFilename;
		BOOL bUniAlloc=MyMakeUnicodeString(&uniFilename,lpFileName);

        InitializeObjectAttributes( &ObjectAttributes, &uniFilename,
                                                                OBJ_CASE_INSENSITIVE, NULL, NULL );

        ntStatus = ZwCreateFile( &FileHandle,
                                                         DELETE|SYNCHRONIZE,
                                                         &ObjectAttributes,
                                                         &IoStatus,
                                                         NULL,
                                                         FILE_ATTRIBUTE_NORMAL,
                                                         0,
                                                         0,
                                                         FILE_SYNCHRONOUS_IO_NONALERT | FILE_COMPLETE_IF_OPLOCKED,
                                                         NULL,
                                                         0 );

		BOOL ret=FALSE;
        if ( ntStatus == STATUS_SUCCESS )       // some error checking DF970604
        {
                FileInformation.DeleteFile = TRUE;
                ntStatus = ZwSetInformationFile( FileHandle,
                                                                                 &IoStatus,
                                                                                 &FileInformation,
                                                                                 sizeof( FILE_DISPOSITION_INFORMATION ),
                                                                                 FileDispositionInformation );


                // File will delete on close
                //
				ZwClose( FileHandle );
                ret=NT_SUCCESS(ntStatus);
        }
		else if (ntStatus == STATUS_OPLOCK_BREAK_IN_PROGRESS)
        {
                ZwClose( FileHandle );
        }
		if(bUniAlloc)
			RtlFreeUnicodeString( &uniFilename );
		
        return ret;
}


DWORD WINAPI _AvpGetFileAttributes(
        LPCTSTR lpFileName      // address of the name of a file or directory
        )
{
        NTSTATUS ntStatus;
        HANDLE FileHandle;
        OBJECT_ATTRIBUTES ObjectAttributes;
        IO_STATUS_BLOCK IoStatus;
		DWORD ret=(DWORD)-1;

		UNICODE_STRING uniFilename;
		BOOL bUniAlloc=MyMakeUnicodeString(&uniFilename,lpFileName);
		
        InitializeObjectAttributes( &ObjectAttributes, &uniFilename,
                                                                OBJ_CASE_INSENSITIVE, NULL, NULL );

        ntStatus = ZwCreateFile( &FileHandle,
                                                         FILE_READ_ATTRIBUTES|SYNCHRONIZE,
                                                         &ObjectAttributes,
                                                         &IoStatus,
                                                         NULL,
                                                         FILE_ATTRIBUTE_NORMAL,
                                                         FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                                                         FILE_OPEN,
                                                         FILE_SYNCHRONOUS_IO_NONALERT |FILE_COMPLETE_IF_OPLOCKED,
                                                         NULL,
                                                         0 );

        if(bUniAlloc)
			RtlFreeUnicodeString( &uniFilename );

        if ( ntStatus != STATUS_SUCCESS )
        {
            if (ntStatus == STATUS_OPLOCK_BREAK_IN_PROGRESS)
			{
                ZwClose( FileHandle );
			}
			goto ret;
        }

		FILE_BASIC_INFORMATION BasicInfo;
		ntStatus = ZwQueryInformationFile( FileHandle,
										   &IoStatus,
										   &BasicInfo,
										   sizeof( FILE_BASIC_INFORMATION ),
										   FileBasicInformation );

		if( ntStatus!=STATUS_SUCCESS )
		{	
			ZwClose( FileHandle );
			goto ret;
		}

		ntStatus = ZwClose( FileHandle );

		if( ntStatus != STATUS_SUCCESS ) 
			goto ret;
		
		ret=BasicInfo.FileAttributes;
ret:

//      _DebugTrace(TraceInfo,"GetAttrib OK:  %08X\n", BasicInfo.FileAttributes);

	
	return ret;;
}


BOOL WINAPI _AvpSetFileAttributes(
        LPCTSTR lpFileName,     // address of filename
    DWORD dwFileAttributes      // address of attributes to set
        )
{
        NTSTATUS ntStatus;
        HANDLE FileHandle;
        OBJECT_ATTRIBUTES ObjectAttributes;
        IO_STATUS_BLOCK IoStatus;
		BOOL ret=FALSE;


		UNICODE_STRING uniFilename;
		BOOL bUniAlloc=MyMakeUnicodeString(&uniFilename,lpFileName);

        InitializeObjectAttributes( &ObjectAttributes, &uniFilename,
                                                                OBJ_CASE_INSENSITIVE, NULL, NULL );


        ntStatus = ZwCreateFile( &FileHandle,
                                                         FILE_READ_ATTRIBUTES|FILE_WRITE_ATTRIBUTES|SYNCHRONIZE,
                                                         &ObjectAttributes,
                                                         &IoStatus,
                                                         NULL,
                                                         FILE_ATTRIBUTE_NORMAL,
                                                         FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                                                         FILE_OPEN,
                                                         FILE_SYNCHRONOUS_IO_NONALERT|FILE_COMPLETE_IF_OPLOCKED,
                                                         NULL,
                                                         0 );

		if(bUniAlloc)
			RtlFreeUnicodeString( &uniFilename );

        if ( ntStatus != STATUS_SUCCESS )
        {
                if (ntStatus == STATUS_OPLOCK_BREAK_IN_PROGRESS)
                {
                    ZwClose( FileHandle );
                }
                goto ret;
        }

        FILE_BASIC_INFORMATION BasicInfo;
        ntStatus = ZwQueryInformationFile( FileHandle,
                                                                           &IoStatus,
                                                                           &BasicInfo,
                                                                           sizeof( FILE_BASIC_INFORMATION ),
                                                                           FileBasicInformation );

        if(ntStatus!=STATUS_SUCCESS){
                ZwClose( FileHandle );
                goto ret;
        }
        BasicInfo.FileAttributes=dwFileAttributes;

        ntStatus = ZwSetInformationFile( FileHandle,
                                                                           &IoStatus,
                                                                           &BasicInfo,
                                                                           sizeof( FILE_BASIC_INFORMATION ),
                                                                           FileBasicInformation );

        if(ntStatus!=STATUS_SUCCESS){
                ZwClose( FileHandle );
                goto ret;
        }

        ntStatus = ZwClose( FileHandle );
        if ( ntStatus != STATUS_SUCCESS )
                goto ret;

		ret=TRUE;
ret:
//	_DebugTrace(TraceInfo,"SetAttrib OK:  %08X\n", dwFileAttributes);
	
	
    return ret;
}


_AvpCheckSlash(LPTSTR s)
{
        if((s+strlen(s)-1)!=_AvpStrrchr(s,'\\'))
                strcat(s,"\\");
        return 0;
}

DWORD WINAPI _AvpGetTempPath(
    DWORD nBufferLength,    // size, in characters, of the buffer
    LPTSTR lpBuffer     // address of buffer for temp. path
        )
{
	AVPScanObject* avpso = *(AVPScanObject**)lpBuffer;
	KLE_SCAN_OBJECT* so=(KLE_SCAN_OBJECT*)(avpso->RefData);
	if(so && so->TempPath){
		so->TempPath->ToPsz(lpBuffer,(WORD)nBufferLength);
		_AvpCheckSlash(lpBuffer);
	}
	else{
		KUstring TempPath(0x200,NonPagedPool);
		TempPath.Assign(LNT_ROOT_PREFIX L"C:\\");
		TempPath.ToPsz(lpBuffer,(WORD)nBufferLength);
	}
	//	_DebugTrace(TraceInfo,"AvpGetTempPath  %s\n", lpBuffer);
     return strlen(lpBuffer);
}

static UINT no=0;

UINT WINAPI _AvpGetTempFileName(
        LPCTSTR lpPathName,     // address of directory name for temporary file
    LPCTSTR lpPrefixString,     // address of filename prefix
    UINT uUnique,       // number used to create temporary filename
    LPTSTR lpTempFileName       // address of buffer that receives the new filename
        )
{
        if(uUnique)no=uUnique;
        else no++;

        strcpy(lpTempFileName,lpPathName);
        strcat(lpTempFileName,lpPrefixString);
        int i=strlen(lpTempFileName);

        UINT t=no%10000;
        lpTempFileName[i++]='0'+t/1000;
        t%=1000;
        lpTempFileName[i++]='0'+t/100;
        t%=100;
        lpTempFileName[i++]='0'+t/10;
        t%=10;
        lpTempFileName[i++]='0'+t;
        lpTempFileName[i++]=0;
        strcat(lpTempFileName,".tmp");


//      _DebugTrace(TraceInfo,"AvpGetTempFileName  %s\n", lpTempFileName);

        return no;
}

DWORD WINAPI _AvpGetDosMemSize(){return 0;}
DWORD WINAPI _AvpGetFirstMcbSeg(){return 0;}

DWORD WINAPI _AvpGetIfsApiHookTable(
	DWORD* table,
	DWORD size //size in DWORDS !!!!!!!!
        )
{
	return 0;
}

DWORD WINAPI _AvpGetDosTraceTable(
	DWORD* table,
	DWORD size //size in DWORDS !!!!!!!!
        )
{
	return 0;
}

DWORD WINAPI _AvpMemoryRead(
    DWORD   dwOffset,     // offset
    DWORD  dwSize,      // size in bytes
    LPBYTE  lpBuffer    // pointer to buffer to read to
        )
{
	return 0;
}

DWORD WINAPI _AvpMemoryWrite(
    DWORD   dwOffset,     // offset
    DWORD  dwSize,      // size in bytes
    LPBYTE  lpBuffer    // pointer to buffer to write from
        )
{
	return 0;
}





BOOL LoadIO(){
/*
#ifdef _MBCS
        setlocale ( LC_ALL, "" );
        _setmbcp(_MB_CP_LOCALE);
#endif
*/
        AvpCallback             =_AvpCallback;
        AvpStrrchr             =_AvpStrrchr;
        AvpStrlwr             =_AvpStrlwr;
        AvpConvertChar  =_AvpConvertChar;

		AvpMemoryRead = _AvpMemoryRead;
		AvpMemoryWrite = _AvpMemoryWrite;
		AvpGetDosMemSize = _AvpGetDosMemSize;
		AvpGetFirstMcbSeg = _AvpGetFirstMcbSeg;
		AvpGetIfsApiHookTable = _AvpGetIfsApiHookTable;
		AvpGetDosTraceTable = _AvpGetDosTraceTable;

        AvpRead13               =_AvpRead13;
        AvpWrite13              =_AvpWrite13;
        AvpRead25               =_AvpRead25;
        AvpWrite26              =_AvpWrite26;
        AvpGetDiskParam  =_AvpGetDiskParam;

        AvpReadFile     =_AvpReadFile;
        AvpWriteFile    =_AvpWriteFile;
        AvpCreateFile   =_AvpCreateFile;
        AvpCloseHandle  =_AvpCloseHandle;
        AvpSetFilePointer =_AvpSetFilePointer;
        AvpSetEndOfFile =_AvpSetEndOfFile;
        AvpGetFileSize  =_AvpGetFileSize;
        AvpGetDiskFreeSpace =_AvpGetDiskFreeSpace;
        AvpGetFullPathName =_AvpGetFullPathName;
        AvpDeleteFile =_AvpDeleteFile;
        AvpSetFileAttributes =_AvpSetFileAttributes;
        AvpGetFileAttributes =_AvpGetFileAttributes;
        AvpGetTempPath =_AvpGetTempPath;
        AvpGetTempFileName =_AvpGetTempFileName;


        return TRUE;
}



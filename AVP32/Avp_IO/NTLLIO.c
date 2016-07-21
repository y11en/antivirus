	 
static BOOL LockVolume( HANDLE hDisk )
{
	DWORD count;
    return DeviceIoControl(	hDisk, FSCTL_LOCK_VOLUME,
		NULL, 0, NULL, 0, &count, NULL);
}

static BOOL UnlockVolume( HANDLE hDisk )
{
	DWORD count;
    return DeviceIoControl(	hDisk, FSCTL_UNLOCK_VOLUME,
		NULL, 0, NULL, 0, &count, NULL);
}

static BOOL GetDiskGeometry( HANDLE hDisk, PDISK_GEOMETRY lpGeometry)
{
    DWORD ReturnedByteCount;
	BOOL ret=FALSE;
	if(lpGeometry==NULL) return ret;
	ret=DeviceIoControl( hDisk,IOCTL_DISK_GET_DRIVE_GEOMETRY,
        NULL, 0, lpGeometry, sizeof(DISK_GEOMETRY),
		&ReturnedByteCount, NULL );
	if(!ret || ReturnedByteCount!=sizeof(DISK_GEOMETRY) || lpGeometry->BytesPerSector==0)
	{
		lpGeometry->BytesPerSector=512;
		lpGeometry->TracksPerCylinder=80;
		lpGeometry->SectorsPerTrack=18;
		lpGeometry->Cylinders.LowPart=2;
	}
	return ret;
}

static int MakeDeviceName(BYTE Disk, char* buf, int szBuf)
{
	BYTE d=Disk & 0x1F; 
	char* ptr="\\\\.\\";
	int i=0;
    if(szBuf< 0x20) return 0;

	for(;*ptr;ptr++,i++)
		buf[i]=*ptr;

    if(Disk & 0x80){
		ptr="PHYSICALDRIVE";
		for(;*ptr;ptr++,i++)	
			buf[i]=*ptr;
		buf[i++]= d+'0';
	}else{
		buf[i++]= d+'A';
		buf[i++]=':';
	}
	buf[i++]=0;
	return i;
}


static BOOL Ntllio13(
	BYTE Disk,
	WORD Sector,
	BYTE Head,
	WORD NumSectors, 
	LPBYTE Buffer,
	BOOL write
	)
{
    DISK_GEOMETRY Geometry;
    DWORD ReturnedByteCount;
	HANDLE hDevice; 
    BOOL fResult; 
	DWORD sector;
	DWORD SecNo;
	DWORD CylNo;
	char dev[0x20];
	MakeDeviceName(Disk, dev, 0x20);

    hDevice = CreateFile(dev, 
		GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,0,NULL);

    if (hDevice == INVALID_HANDLE_VALUE) return FALSE; 

	fResult=LockVolume(hDevice);
//	if ( !fResult ) goto ret;

	fResult=GetDiskGeometry(hDevice,&Geometry);

	SecNo=Sector & 0x003F;
	CylNo=Sector<<2;
	CylNo&=0x0300;
	CylNo|=Sector>>8;
	sector = (CylNo*Geometry.TracksPerCylinder + Head)*Geometry.SectorsPerTrack + SecNo - 1;

	fResult=(((DWORD)-1)!=SetFilePointer(hDevice,sector*Geometry.BytesPerSector,NULL,FILE_BEGIN));
	if(fResult)
	{
		LPBYTE buf=NULL;
		if(Geometry.BytesPerSector > 0x200)
		{
			buf=malloc(NumSectors*Geometry.BytesPerSector);
			if(!buf) fResult=FALSE;
			if(write)memcpy(buf,Buffer,NumSectors*0x200);
		}
		if(fResult)
		{
			if(write)
				fResult=WriteFile(hDevice,buf?buf:Buffer,NumSectors*Geometry.BytesPerSector,&ReturnedByteCount,NULL);
			else
				fResult=ReadFile(hDevice,buf?buf:Buffer,NumSectors*Geometry.BytesPerSector,&ReturnedByteCount,NULL);
		
		}
		if(buf)
		{
			if(!write)memcpy(Buffer,buf,NumSectors*0x200);
			free(buf);
		}

	}

	UnlockVolume(hDevice);
//ret:
	CloseHandle(hDevice); 
	return fResult;
}

static BOOL Ntllio2X(
	BYTE Drive,
	DWORD Sector,
	WORD NumSectors, 
	LPBYTE Buffer,
	BOOL write
	)
{
    DISK_GEOMETRY Geometry;
    DWORD ReturnedByteCount;
	HANDLE hDevice; 
    BOOL fResult; 
	char dev[0x20];
	MakeDeviceName(Drive, dev, 0x20);
 
    hDevice = CreateFile(dev, 
		GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,0,NULL);

    if (hDevice == INVALID_HANDLE_VALUE) return FALSE; 

	fResult=LockVolume(hDevice);
//	if ( !fResult ) goto ret;

	fResult=GetDiskGeometry(hDevice,&Geometry);

	if(fResult){
		fResult=(((DWORD)-1)!=SetFilePointer(hDevice,Sector*Geometry.BytesPerSector,NULL,FILE_BEGIN));
		if(fResult)
		{
			LPBYTE buf=NULL;
			if(Geometry.BytesPerSector > 0x200)
			{
				buf=malloc(NumSectors*Geometry.BytesPerSector);
				if(!buf) fResult=FALSE;
				if(write)memcpy(buf,Buffer,NumSectors*0x200);
			}
			if(fResult)
			{
				if(write)
					fResult=WriteFile(hDevice,buf?buf:Buffer,NumSectors*Geometry.BytesPerSector,&ReturnedByteCount,NULL);
				else
					fResult=ReadFile(hDevice,buf?buf:Buffer,NumSectors*Geometry.BytesPerSector,&ReturnedByteCount,NULL);
			
			}
			if(buf)
			{
				if(!write)memcpy(Buffer,buf,NumSectors*0x200);
				free(buf);
			}
		}
	}

	UnlockVolume(hDevice);
//ret:
	CloseHandle(hDevice); 
	return fResult;
}

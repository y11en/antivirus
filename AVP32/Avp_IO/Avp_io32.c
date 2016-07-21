// The AVP low level IO Library. W95
#include "avp_io32.h"
#include <Stuff/LoadVxd.c>

static HANDLE hDevice=INVALID_HANDLE_VALUE;

BOOL WINAPI DllMain(HINSTANCE hDLLInst,
                  DWORD  fdwReason,
                  LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hDevice = LoadVxd("AVP_IO.VXD",hDLLInst);
		if(hDevice == INVALID_HANDLE_VALUE)
			return FALSE;
		break;
	case DLL_PROCESS_DETACH:	
		if(hDevice!=INVALID_HANDLE_VALUE)
			CloseHandle(hDevice);
		break;

	case DLL_THREAD_ATTACH:		break;
	case DLL_THREAD_DETACH:		break;
	}
	return TRUE;
}


DWORD WINAPI _AvpMemoryRead (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    LPBYTE  buffer	// pointer to buffer to read to 
	)
{
	DWORD count=0;
	void* b=GlobalAlloc(GMEM_FIXED,size);
	if(!b)return 0;
	if (!DeviceIoControl(hDevice, IOCTL_AVPIO_MEMORY_READ, 
		&offset, 2*sizeof(DWORD), b, size, &count, 0 ))
		count=0;
	CopyMemory(buffer,b,count);
	GlobalFree(b);
	return count;
}
	
DWORD WINAPI _AvpMemoryWrite (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    LPBYTE  buffer	// pointer to buffer to read to 
	)
{
	FlatIoCb* cb;
	cb=(FlatIoCb*)GlobalAlloc(GMEM_FIXED,size+sizeof(FlatIoCb));
	if(!cb)return 0;
	cb->offset=offset;
	cb->size=size;
	CopyMemory(cb->buffer,buffer,size);

	if (!DeviceIoControl(hDevice, IOCTL_AVPIO_MEMORY_WRITE, 
		cb, size+sizeof(FlatIoCb), 0, 0, 0, 0 ))
		size=0;
	GlobalFree(cb);
	return size;
}

DWORD WINAPI _AvpGetDosMemSize ()
{
	DWORD size;
	DWORD count;
	if (!DeviceIoControl(hDevice, IOCTL_AVPIO_MEMORY_GET_DOS_MEM_SIZE, 
		0, 0, &size, sizeof(DWORD), &count, 0 ))
		return 0;
	return size;
}

DWORD WINAPI _AvpGetFirstMcbSeg ()
{
	DWORD seg;
	DWORD count;
	if (!DeviceIoControl(hDevice, IOCTL_AVPIO_MEMORY_GET_FIRST_MCB_SEG, 
		0, 0, &seg, sizeof(DWORD), &count, 0 ))
		return 0;
	return seg;
}

DWORD WINAPI _AvpGetIfsApiHookTable (
	DWORD* table, 
	DWORD size //size in DWORDs
	)
{
	DWORD count;
	if (!DeviceIoControl(hDevice, IOCTL_AVPIO_MEMORY_GET_IFSAPI_HOOK_TABLE, 
		0, 0, table, size*sizeof(DWORD), &count, 0 ))
		return 0;
	return count/sizeof(DWORD);
}

DWORD WINAPI _AvpGetDosTraceTable (
	DWORD* table, 
	DWORD size //size in DWORDs
	)
{
	DWORD count;
	if (!DeviceIoControl(hDevice, IOCTL_AVPIO_MEMORY_GET_DOS_TRACE_TABLE, 
		0, 0, table, size*sizeof(DWORD), &count, 0 ))
		return 0;
	return count/sizeof(DWORD);
}

BOOL WINAPI _AvpGetDiskParam (
    BYTE   disk,
    BYTE   drive,
    WORD*   CX,
    BYTE*	DH
	)
{
	BYTE buf[4];
	BYTE inbuf[2];
	DWORD count;
	if(CX)*CX=0;
	if(DH)*DH=0;
	inbuf[0]=disk;
	inbuf[1]=drive;
	if (!DeviceIoControl(hDevice, IOCTL_AVPIO_PHYSICAL_DISK_GET_PARAM, 
		inbuf, 2, buf, 4, &count, 0 ) || count<3)
	{
		return 0;
	}
	if(CX)*CX=*(WORD*)buf;
	if(DH)*DH=buf[2];
	return 1;
}

BOOL WINAPI _AvpRead13 (
	BYTE Disk,
	WORD Sector,
	BYTE Head,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	BOOL ret;
	DWORD count;
	DiskIoCb* dcb=(DiskIoCb*)GlobalAlloc(GMEM_FIXED,sizeof(DiskIoCb));
	if(!dcb)return 0;
	dcb->disk=Disk;
	dcb->sector=Sector;
	dcb->head=Head;
	dcb->numSectors=NumSectors;
	dcb->sectorSize=512;

	ret=DeviceIoControl(hDevice, IOCTL_AVPIO_PHYSICAL_DISK_READ, 
		dcb, sizeof(DiskIoCb), Buffer, NumSectors*512, &count, 0 );
	GlobalFree(dcb);
	return ret;
}


BOOL WINAPI _AvpWrite13(
	BYTE Disk, 
	WORD Sector ,
	BYTE Head ,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	BOOL ret;
	DWORD size=NumSectors*512;
	DiskIoCb* dcb=(DiskIoCb*)GlobalAlloc(GMEM_FIXED,size+sizeof(DiskIoCb));
	if(!dcb)return 0;
	dcb->disk=Disk;
	dcb->sector=Sector;
	dcb->head=Head;
	dcb->numSectors=NumSectors;
	dcb->sectorSize=512;
	CopyMemory(dcb->buffer,Buffer,size);

	ret=DeviceIoControl(hDevice, IOCTL_AVPIO_PHYSICAL_DISK_WRITE, 
		dcb, size+sizeof(DiskIoCb), 0, 0, 0, 0 );
	GlobalFree(dcb);
	return ret;
}


BOOL WINAPI _AvpRead25(
	BYTE Drive,
	DWORD Sector,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	BOOL ret;
	DWORD count;
	DiskIoCb* dcb=(DiskIoCb*)GlobalAlloc(GMEM_FIXED,sizeof(DiskIoCb));
	if(!dcb)return 0;


	dcb->disk=Drive;
	dcb->sector=Sector;
	dcb->numSectors=NumSectors;
	dcb->sectorSize=512;
	ret=DeviceIoControl(hDevice, IOCTL_AVPIO_LOGICAL_DRIVE_READ, 
		dcb, sizeof(DiskIoCb), Buffer, NumSectors*512, &count, 0 );
	GlobalFree(dcb);
	return ret;
}


BOOL WINAPI _AvpWrite26(
	BYTE Drive,
	DWORD Sector,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	BOOL ret;
	DWORD size=NumSectors*512;
	DiskIoCb* dcb=(DiskIoCb*)GlobalAlloc(GMEM_FIXED,size+sizeof(DiskIoCb));
	if(!dcb)return 0;
	dcb->disk=Drive;
	dcb->sector=Sector;
	dcb->numSectors=NumSectors;
	dcb->sectorSize=512;
	CopyMemory(dcb->buffer,Buffer,size);

	ret=DeviceIoControl(hDevice, IOCTL_AVPIO_LOGICAL_DRIVE_WRITE, 
		dcb, size+sizeof(DiskIoCb), 0, 0, 0, 0 );
	GlobalFree(dcb);
	return ret;
}





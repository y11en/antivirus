// The AVP low level IO Library. WNT
#include "avp_io32.h"
#include "ntllio.c"

BOOL WINAPI _DllMain(HINSTANCE hDLLInst,
                  DWORD  fdwReason,
                  LPVOID lpvReserved)
{

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:	break;
	case DLL_PROCESS_DETACH:	break;
	case DLL_THREAD_ATTACH:		break;
	case DLL_THREAD_DETACH:		break;
	}
	return TRUE;
}




DWORD WINAPI _AvpMemoryRead (
    DWORD   Offset,	// offset
    DWORD   Size,	// size in bytes
    LPBYTE  lpBuffer	// pointer to buffer to read to 
	)
{
	return 0;
}
	
DWORD WINAPI _AvpMemoryWrite (
    DWORD   Offset,	// offset
    DWORD   Size,	// size in bytes
    LPBYTE  lpBuffer	// pointer to buffer to write from 
	)
{
	return 0;
}

DWORD WINAPI _AvpGetDosMemSize ()
{
	return 0;
}

DWORD WINAPI _AvpGetFirstMcbSeg ()
{
	return 0;
}

DWORD WINAPI _AvpGetIfsApiHookTable (
	DWORD* table, 
	DWORD size //size in DWORDs
	)
{
	return 0;
}

DWORD WINAPI _AvpGetDosTraceTable (
	DWORD* table, 
	DWORD size //size in DWORDs
	)
{
	return 0;
}

BOOL WINAPI _AvpGetDiskParam( BYTE disk, BYTE drive, WORD* CX, BYTE* DH)
{
	DWORD Num_Cylinders;
	DWORD Num_Sec_Per_Track;
	DWORD Num_Heads;
    DISK_GEOMETRY Geometry;
	HANDLE hDevice; 
    BOOL ret=FALSE; 
	BYTE Buffer[0x400];
	char dev[0x40];

	if(CX)*CX=0;
	if(DH)*DH=0;
 
	if(!ret)if(_AvpRead13(disk,1,0,1,Buffer)) ret=TRUE;
	if(!ret)if(_AvpRead25(drive,0,1,Buffer)) ret=TRUE;
	
	MakeDeviceName(disk, dev, 0x40);
    hDevice = CreateFile(dev, 
		GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,0,NULL);

    if (hDevice!=INVALID_HANDLE_VALUE){
		LockVolume(hDevice);
		if(GetDiskGeometry(hDevice,&Geometry))
		{
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
		}
		UnlockVolume(hDevice);
		CloseHandle(hDevice); 
	}

	return ret;
}


BOOL WINAPI _AvpRead13 (
	BYTE Disk,
	WORD Sector,
	BYTE Head,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	return Ntllio13(Disk,Sector,Head,NumSectors,Buffer,FALSE);
}

BOOL WINAPI _AvpWrite13(
	BYTE Disk, 
	WORD Sector ,
	BYTE Head ,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	return Ntllio13(Disk,Sector,Head,NumSectors,Buffer,TRUE);
}


BOOL WINAPI _AvpRead25(
	BYTE Drive,
	DWORD Sector,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	return Ntllio2X(Drive,Sector,NumSectors,Buffer,FALSE);
}

BOOL WINAPI _AvpWrite26(
	BYTE Drive,
	DWORD Sector,
	WORD NumSectors, 
	LPBYTE Buffer
	)
{
	return Ntllio2X(Drive,Sector,NumSectors,Buffer,TRUE);
}





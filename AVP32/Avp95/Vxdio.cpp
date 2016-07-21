#include <_avpio.h>
#include "stdafx.h"


DWORD _Sleep();

//===================  AVP_IO.VXD =====================================
PDDB AvpIoDDB=NULL; 

extern char kRegKey_GK95PlugIns[];
extern char* basepath_name;

static DWORD GetAvpIoVxdPtr(int pointerid_)
{
    ALLREGS regs;
	if(AvpIoDDB==NULL){
		char ioname[MAX_PATH];
		DWORD bsize=MAX_PATH;
		HKEY hKey=0;
		int l;

		*ioname=0;
		if(ERROR_SUCCESS==RegCreateKey(HKEY_LOCAL_MACHINE, kRegKey_GK95PlugIns, &hKey))
		{
			RegQueryValueEx(hKey, basepath_name, NULL, (DWORD*)REG_SZ, (BYTE*)ioname, &bsize);
			RegCloseKey(hKey);
		}
tryagain:
    	l=strlen(ioname);
		if(l && ioname[l-1]!='\\')strcpy(ioname+l,"\\");

		strcat(ioname,"AVP_IO.VXD");
		switch(sign_check_file(ioname,1,0,0,0)) 
		{
		case 0:
			break;
		case SIGN_FILE_NOT_FOUND:
		case SIGN_CANT_READ:
			if(l){
				*ioname=0;
				goto tryagain;
			}
			AvpCallback(AVP_CALLBACK_ERROR_FILE_OPEN,(DWORD)ioname);
//			goto err;
			break; //manually loaded from nowhere....
		default:
			AvpCallback(AVP_CALLBACK_FILE_INTEGRITY_FAILED,(DWORD)ioname);
			goto err;
		}

		PDEVICEINFO pHandle;
		VXDLDR_LoadDevice(ioname, VXDLDR_INIT_DEVICE, &pHandle, &AvpIoDDB);
		
		if (AvpIoDDB==NULL){
err:
#ifdef DEBUGPRINT
			dprintf("ERROR!: Can't init %s",ioname);
#endif
			return 0;
		}
	}
    memset(&regs, 0, sizeof(regs));
    regs.REDX = pointerid_;                     // pointer ID is passed in edx
    Directed_Sys_Control(AvpIoDDB, BEGIN_RESERVED_PRIVATE_SYSTEM_CONTROL, &regs);
#ifdef DEBUGPRINT
    dprintf("Directed_Sys_Control() returned %x, %x, %x", (int)regs.REBX, (int)regs.RECX, (int)regs.REDX);
#endif
    return regs.REDX;                            // pointer returned in ebx

}


DWORD WINAPI _AvpMemoryRead (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    BYTE*  buffer	// pointer to buffer to read to 
	)
{
	DWORD ptr=GetAvpIoVxdPtr(AvpIo_MemoryRead);
	if(!ptr)return 0;
	return ((t_AvpMemoryRead*)ptr)(offset,size,buffer);
}

	
DWORD WINAPI _AvpMemoryWrite (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    BYTE*  buffer	// pointer to buffer to read to 
	)
{
	DWORD ptr=GetAvpIoVxdPtr(AvpIo_MemoryWrite);
	if(!ptr)return 0;
	return ((t_AvpMemoryWrite*)ptr)(offset,size,buffer);
}

DWORD WINAPI _AvpGetDosMemSize()
{
	DWORD ptr=GetAvpIoVxdPtr(AvpIo_GetDosMemSize);
	if(!ptr)return 0;
	return ((t_AvpGetDosMemSize*)ptr)();
}


DWORD WINAPI _AvpGetFirstMcbSeg()
{
	DWORD ptr=GetAvpIoVxdPtr(AvpIo_GetFirstMcbSeg);
	if(!ptr)return 0;
	return ((t_AvpGetFirstMcbSeg*)ptr)();
}


DWORD WINAPI _AvpGetIfsApiHookTable(DWORD* table, DWORD maxCount)
{
	DWORD ptr=GetAvpIoVxdPtr(AvpIo_GetIfsApiHookTable);
	if(!ptr)return 0;
	return ((t_AvpGetIfsApiHookTable*)ptr)(table,maxCount);
}

DWORD WINAPI _AvpGetDosTraceTable(DWORD* table, DWORD maxCount)
{
	DWORD ptr=GetAvpIoVxdPtr(AvpIo_GetDosTraceTable);
	if(!ptr)return 0;
	return ((t_AvpGetDosTraceTable*)ptr)(table,maxCount);
}

	
BOOL WINAPI _AvpGetDiskParam (
    BYTE   disk,
    BYTE   drive,
    WORD*   CX,
    BYTE*	DH
	)
{
	BOOL ret=FALSE;
	if(CX)*CX=0;
	if(DH)*DH=0;


	_Sleep();

#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO && (disk<0x20))
	{
		char Buffer[1024];
		ret=FPI_IO->fFnRWSectorPhys(disk, 1, 0, 0, 0, Buffer, 512);
		if(!ret)	
			ret=FPI_IO->fFnRWSectorLogc(drive, 0, 0, Buffer, 512);

		FPI_DISK_GEOMETRY_struct dg;
		if(FPI_IO->fFnGetDriveGeometry(drive, &dg))
		{
			DWORD NumCyl=0x3FF;  //dg.Cylinders.LowPart
			if(
//				() < 0x400	&& 
				(dg.SectorsPerTrack) < 0x40		&& 
				(dg.TracksPerCylinder) <0x100)
			{
				WORD w=((BYTE)(NumCyl -1))<<8;
				w|=(0xC0)&((NumCyl -1)>>2);
				w|=(0x3F)&(BYTE)(dg.SectorsPerTrack);
				if(CX)*CX=w;
				if(DH)*DH=(BYTE)(dg.TracksPerCylinder -1);
			}
		}
		if(ret && *CX)
			return ret;
	}
#endif

	DWORD ptr=GetAvpIoVxdPtr(AvpIo_GetDiskParam);
	if(!ptr)return ret;
	return ((t_AvpGetDiskParam*)ptr)(disk,drive,CX,DH);
}

BOOL WINAPI _AvpRead25(	BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
	_Sleep();
#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO)
	{
#ifdef DEBUGPRINT
		dprintf("AVP_IO: RWSectorLog drive:%X sector:%X",Drive,Sector);
#endif
		BOOL r=FPI_IO->fFnRWSectorLogc(Drive, Sector, 0, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif

	DWORD ptr=GetAvpIoVxdPtr(AvpIo_Read25);
	if(!ptr)return 0;
	return ((t_AvpRead25*)ptr)(Drive, Sector, NumSectors, Buffer);
}

BOOL WINAPI _AvpWrite26( BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
	_Sleep();
#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO)
	{
		BOOL r=FPI_IO->fFnRWSectorLogc(Drive, Sector, 1, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif

	DWORD ptr=GetAvpIoVxdPtr(AvpIo_Write26);
	if(!ptr)return 0;
	return ((t_AvpWrite26*)ptr)(Drive, Sector, NumSectors, Buffer);
}

BOOL WINAPI _AvpRead13 (BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
	_Sleep();
#ifdef FPIAPI_VER_MAJOR

	if(FPI_IO)
	{
        WORD SecNo=Sector & 0x003F;
        WORD CylNo=Sector<<2;
        CylNo&=0x0300;
        CylNo|=Sector>>8;

		BOOL r=FPI_IO->fFnRWSectorPhys(Disk, Head, CylNo, SecNo, 0, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif

	DWORD ptr=GetAvpIoVxdPtr(AvpIo_Read13);
	if(!ptr)return 0;
	return ((t_AvpRead13*)ptr)(Disk, Sector, Head, NumSectors, Buffer);
}

BOOL WINAPI _AvpWrite13(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
	_Sleep();
#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO)
	{
        WORD SecNo=Sector & 0x003F;
        WORD CylNo=Sector<<2;
        CylNo&=0x0300;
        CylNo|=Sector>>8;

		BOOL r=FPI_IO->fFnRWSectorPhys(Disk, Head, CylNo, SecNo, 1, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif
 
	DWORD ptr=GetAvpIoVxdPtr(AvpIo_Write13);
	if(!ptr)return 0;
	return ((t_AvpWrite13*)ptr)(Disk, Sector, Head, NumSectors, Buffer);
}

//=============== END  AVP_IO.VXD =====================================


#undef vsprintf
extern "C" int vsprintf( char *buffer, const char *format, va_list argptr )
{
	int ret= LOCK_vsprintf(buffer, format, argptr);
#ifdef DEBUGPRINT
    dprintf("LOCK_VSPRINTF:  %s",buffer );
#endif

	return ret;
}




LRESULT WINAPI _AvpCallback(WPARAM wParam,LPARAM lParam)
{
#ifdef DEBUGPRINT
//    dprintf("AvpCallback %04X",wParam );
#endif

	return 0;
}

_TCHAR* WINAPI _AvpStrlwr(_TCHAR* str_)
{
#ifdef _MBCS
	return strlwr(str_);
#else

	if(str_)
	{
		_TCHAR* c=str_;
		while(*c){
			*c|=0x20;
			c++;
		}
	}
	return str_;
#endif
}

_TCHAR* WINAPI _AvpStrrchr(_TCHAR* str_, INT chr_){

#ifdef _MBCS
	return strrchr(str_,chr_);
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

}


_TCHAR* WINAPI _AvpConvertChar(_TCHAR* str_,INT ct,_TCHAR* cp_)
{
	switch(ct)
	{
	case 1:
		break;
	case 5:
		if (*(DWORD*)cp_==0xFFFE || *(DWORD*)cp_==0xFEFF)
		{
			_QWORD res;
			char buff[0x200];
			memset(buff,0x200,0);
			if(*(WORD*)cp_==0xFEFF){
				char *p=cp_+2;
				char *q=cp_+3;
				while( *p || *q){
					char c=*q;
					*(q++)=*p;
					*(p++)=c;
				}
			}
			UniToBCS((UCHAR*)buff,(string_t)cp_,-1,0x200,BCS_WANSI,&res);
			if(strlen(buff)){
				strcpy(str_,buff);
#ifdef DEBUGPRINT
				dprintf("AVP_IO: -->converted: %s",str_);
#endif
				break;
			}
		}
	default:
	case 0: //OemToAnsi
		{
			WORD UniStr[0x200];
			_QWORD res;
			if(!str_) return FALSE;
			int l=strlen(str_);
			BCSToUni(UniStr,(UCHAR*)str_,l,BCS_OEM,&res);
			UniToBCS((UCHAR*)str_,UniStr,l*2,0x200,BCS_WANSI,&res);
			str_[l]=0;
#ifdef DEBUGPRINT
			dprintf("AVP_IO: -->converted: %s",str_);
#endif
		}
		break;
		
	}
	return str_;
}

BOOL WINAPI _AvpReadFile (
    HANDLE  hFile,	// handle of file to read 
    LPVOID  lpBuffer,	// address of buffer that receives data  
    DWORD  nNumberOfBytesToRead,	// number of bytes to read 
    LPDWORD  lpNumberOfBytesRead,	// address of number of bytes read 
    OVERLAPPED FAR*  lpOverlapped 	// address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
)
{
	BOOL ret=TRUE;
	_Sleep();

	*lpNumberOfBytesRead=filerdwr((int)hFile, lpBuffer, nNumberOfBytesToRead, 0);

	if(*lpNumberOfBytesRead==(DWORD)-1)
	{
		*lpNumberOfBytesRead=0;
		ret=FALSE;
	}
//	if(*lpNumberOfBytesRead==0 && nNumberOfBytesToRead)ret=FALSE;

#ifdef DEBUGPRINT
    dprintf("AvpReadFile %d %d: %d %d",(int)hFile ,nNumberOfBytesToRead, ret, *lpNumberOfBytesRead);
#endif

	return ret;
}

BOOL WINAPI _AvpWriteFile (
    HANDLE  hFile,	// handle to file to write to 
    LPCVOID  lpBuffer,	// pointer to data to write to file 
    DWORD  nNumberOfBytesToWrite,	// number of bytes to write 
    LPDWORD  lpNumberOfBytesWritten,	// pointer to number of bytes written 
    OVERLAPPED FAR*  lpOverlapped 	// addr. of structure needed for overlapped I/O  
	)
{
	BOOL ret=TRUE;
	_Sleep();


	
	*lpNumberOfBytesWritten=filerdwr((int)hFile, (LPVOID)lpBuffer, nNumberOfBytesToWrite, 1);

	if(*lpNumberOfBytesWritten==(DWORD)-1)
	{
		*lpNumberOfBytesWritten=0;
		ret=FALSE;
	}
	if(*lpNumberOfBytesWritten==0 && nNumberOfBytesToWrite)ret=FALSE;

#ifdef DEBUGPRINT
    dprintf("AvpWriteFile %d %d: %d %d",(int)hFile ,nNumberOfBytesToWrite, ret, *lpNumberOfBytesWritten);
#endif

	return ret;
}

#define GENERIC_READ		0x80000000L
#define GENERIC_WRITE		0x40000000L
#define GENERIC_EXECUTE		0x20000000L
#define GENERIC_ALL			0x10000000L

#define FILE_SHARE_READ		0x00000001
#define FILE_SHARE_WRITE	0x00000002
#define FILE_SHARE_DELETE	0x00000004

#define CREATE_NEW			0x00000001
#define CREATE_ALWAYS		0x00000002
#define OPEN_EXISTING		0x00000003
#define OPEN_ALWAYS			0x00000004
#define TRUNCATE_EXISTING	0x00000005

extern DWORD open_reserved_flag;

HANDLE WINAPI _AvpCreateFile (
    LPCTSTR  lpFileName,	// pointer to name of the file 
    DWORD  dwDesiredAccess,	// access (read-write)
    DWORD  dwShareMode,	// share mode 
    SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,	// pointer to security descriptor 
    DWORD  dwCreationDistribution,	// how to create 
    DWORD  dwFlagsAndAttributes,	// file attributes 
    HANDLE  hTemplateFile 	// handle to file with attributes to copy  
	)
{
	int h=-1;
		int access_=0; //!!!!
		int sharemode_=FO_SHARE_DENYNONE;
	//	access_|=FO_GKAWARE;

		int need_trunc=0;
		switch(dwCreationDistribution)
		{
		case CREATE_NEW: 
			access_|=FO_CREATE;
			access_|=FO_EXCL;
			break;
		case CREATE_ALWAYS: 
			access_|=FO_CREATE;
			need_trunc=1;
			break;
		case OPEN_EXISTING: 
			break;
		case OPEN_ALWAYS: 
			access_|=FO_CREATE;
			break;
		case TRUNCATE_EXISTING:
			need_trunc=1;
			break;
		default:
			break;
		}

		if((dwDesiredAccess == GENERIC_WRITE)
		|| (dwDesiredAccess == (GENERIC_WRITE | GENERIC_READ)))
			access_|=FO_RDWR;
		else access_|=FO_RDONLY;

#ifdef DEBUGPRINT
	    dprintf("AvpCreateFile %s : try to open",lpFileName);
#endif
		h=fileopen(lpFileName, access_, sharemode_);

		if(h!=-1 && need_trunc){
			fileseek(h, 0, 0);
			filerdwr(h, "", 0, 1);
		}
		if(h>=80){
			fileclose(h);
			h=-1;
		}

#ifdef DEBUGPRINT
    dprintf("AvpCreateFile %s : %d, size=%d",lpFileName,h ,filegetsize(h));
#endif

	return (HANDLE)h;
}

BOOL WINAPI _AvpCloseHandle(
    HANDLE  hFile 	// handle to object to close  
	)
{
#ifdef DEBUGPRINT
    dprintf("AvpCloseHandle %d",(int)hFile);
#endif

	return fileclose((int) hFile);
}


DWORD WINAPI _AvpSetFilePointer(
    HANDLE  hFile,	// handle of file 
    LONG  lDistanceToMove,	// number of bytes to move file pointer 
    PLONG  lpDistanceToMoveHigh,	// address of high-order word of distance to move  
    DWORD  dwMoveMethod 	// how to move 
	)
{

	DWORD ret=-1;

	ret= fileseek((int)hFile, lDistanceToMove, dwMoveMethod);

#ifdef DEBUGPRINT
    dprintf("AvpSeek %d: %d;%d ->%d",(int)hFile , lDistanceToMove, dwMoveMethod,ret);
#endif
	return ret;
}


BOOL WINAPI _AvpSetEndOfFile(
    HANDLE  hFile	// handle of file 
	)
{
	BOOL ret=FALSE;

	DWORD Number=filerdwr((int)hFile, "", 0, 1);
	if(Number==(DWORD)-1)ret=FALSE;
	else ret=TRUE;
#ifdef DEBUGPRINT
    dprintf("AvpSetEndOfFile %d: %d",(int)hFile ,ret);
#endif
	return ret;
}



DWORD WINAPI _AvpGetFileSize(
    HANDLE  hFile,	// handle of file 
    LPDWORD  lpHigh 
	)
{
	DWORD ret;

	ret=filegetsize((int) hFile);

#ifdef DEBUGPRINT
    dprintf("AvpGetFileSize %d, size:%d",(int)hFile ,ret);
#endif
	return ret;
}


//additional
BOOL WINAPI _AvpGetDiskFreeSpace(
	LPCTSTR lpRootPathName,	// address of root path 
    LPDWORD lpSectorsPerCluster,	// address of sectors per cluster 
    LPDWORD lpBytesPerSector,	// address of bytes per sector 
    LPDWORD lpNumberOfFreeClusters,	// address of number of free clusters  
    LPDWORD lpTotalNumberOfClusters 	// address of total number of clusters  
	)
{
#ifdef DEBUGPRINT
    dprintf("AvpGetDiskFreeSpace %s", lpRootPathName);
#endif
	WORD Error;
	return R0_GetDiskFreeSpace((lpRootPathName[0])&0xDF -'A'+1,
		(LPWORD)lpSectorsPerCluster,(LPWORD)lpNumberOfFreeClusters,
		(LPWORD)lpBytesPerSector, (LPWORD)lpTotalNumberOfClusters, &Error);
	return 0;
}


DWORD WINAPI _AvpGetFullPathName(
	LPCTSTR lpFileName,	// address of name of file to find path for 
    DWORD nBufferLength,	// size, in characters, of path buffer 
    LPTSTR lpBuffer,	// address of path buffer 
    LPTSTR *lpFilePart 	// address of filename in path 
	)
{
    DWORD l;
    strncpy(lpBuffer,lpFileName,nBufferLength);
    l=strlen(lpBuffer);
	*lpFilePart=_AvpStrrchr(lpBuffer,'\\');
    if(*lpFilePart) (*lpFilePart)++;
	else *lpFilePart=lpBuffer;// + l;
#ifdef DEBUGPRINT
    dprintf("AvpGetFullPathName %s (%s)", lpBuffer,*lpFilePart);
#endif
	return l;
}
	

BOOL WINAPI _AvpDeleteFile(
	LPCTSTR lpFileName 	// pointer to name of file to delete  
	)
{
//	if(-1==filedelete(lpFileName))
#ifdef DEBUGPRINT
    dprintf("AvpDeleteFile %s", lpFileName);
#endif

#ifdef INT_VXD_IO
	if(_IntDeleteFile(lpFileName))	return TRUE;
#endif

	WORD err;
	return R0_DeleteFile((LPTSTR)lpFileName,0,&err);
//	return TRUE;
}
	
 
BOOL WINAPI _AvpSetFileAttributes(
	LPCTSTR lpFileName,	// address of filename 
    DWORD dwFileAttributes 	// address of attributes to set 
	)
{
	WORD Err;
#ifdef DEBUGPRINT
    dprintf("AvpSetFileAttr %s",lpFileName);
#endif

	BOOL ret=R0_SetFileAttributes((LPTSTR)lpFileName, (WORD)dwFileAttributes, &Err);
	return ret;

//	if(-1==fileattributes(lpFileName, 1, dwFileAttributes))
//		return 0;
//	else return 1;
}
	
 
DWORD WINAPI _AvpGetFileAttributes(
	LPCTSTR lpFileName 	// address of the name of a file or directory  
	)
{
//	return fileattributes(lpFileName, 0, 0);
#ifdef DEBUGPRINT
    dprintf("AvpGetFileAttr %s",lpFileName);
#endif

	DWORD Attr=(DWORD)-1;
	WORD Err;

#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO){
		FPIFILEHANDLE hFile;
		hFile=FPI_IO->fFnOpen(fpiParentGet(lpFileName),lpFileName,FPIFILEIO_READ,0,0,0);
		if(hFile!=FPIFILEHANDLE_ERROR)
		{
			Attr=FPI_IO->fFnGetAttr((FPIFILEHANDLE)hFile);
			FPI_IO->fFnClose((FPIFILEHANDLE)hFile);
		}
	}
#endif
	if(Attr==(DWORD)-1)
	{
		Attr=0;
		if(!R0_GetFileAttributes((LPTSTR)lpFileName, (WORD*)&Attr, &Err))
			Attr=(DWORD)-1;
	}

	return Attr;
}
	

_AvpCheckSlash(LPTSTR s)
{
	if((s+strlen(s)-1)!=_AvpStrrchr(s,'\\'))
		strcat(s,"\\");
	return 0;
}

char* _TempPath=NULL;

BOOL AVP_SetTempPath(char* name)
{
	char* ptr=new char[strlen(name)+2];
	if(!ptr)return 0;
	strcpy(ptr,name);
	_AvpCheckSlash(ptr);

/*	if (_access(ptr,0)){
		delete ptr;
		return FALSE;
	}
*/
	if(_TempPath)
		delete _TempPath;
	_TempPath=ptr;
	return TRUE;
}


DWORD WINAPI _AvpGetTempPath( DWORD nBufferLength, LPTSTR lpBuffer )
{
	DWORD ret;
	if(_TempPath)
		ret=strlen(strncpy(lpBuffer, _TempPath, nBufferLength));
	else{
		PCHAR pTemp;
		if(Get_Environment_String("TEMP",&pTemp))goto cpy;
		if(Get_Environment_String("TMP",&pTemp))goto cpy;
		if(Get_Environment_String("WINDIR",&pTemp))goto cpy;
		if(Get_Environment_String("WINBOOTDIR",&pTemp))goto cpy;
		pTemp="C:\\";
cpy:
		strncpy(lpBuffer, pTemp, nBufferLength);
		_AvpCheckSlash(lpBuffer);
		ret=strlen(lpBuffer);
		AVP_SetTempPath(lpBuffer);

	#ifdef DEBUGPRINT
		dprintf("AvpGetTempPath  %s", lpBuffer);
	#endif
	}
	return ret;

}
	
static UINT no=0;

UINT WINAPI _AvpGetTempFileName(
	LPCTSTR lpPathName,	// address of directory name for temporary file 
    LPCTSTR lpPrefixString,	// address of filename prefix 
    UINT uUnique,	// number used to create temporary filename 
    LPTSTR lpTempFileName 	// address of buffer that receives the new filename 
	)
{
	if(uUnique)	no=uUnique;
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


#ifdef DEBUGPRINT
    dprintf("AvpGetTempFileName  %s", lpTempFileName);
#endif
	return no;
}
	





BOOL LoadIO(){

#ifdef _MBCS
	setlocale ( LC_ALL, "" );
	_setmbcp(_MB_CP_LOCALE);
#endif

	AvpCallback		=_AvpCallback;
	AvpStrrchr		=_AvpStrrchr;
	AvpStrlwr		=_AvpStrlwr;
	AvpConvertChar	=_AvpConvertChar;

	AvpRead13		=_AvpRead13;
	AvpWrite13		=_AvpWrite13;
	AvpRead25		=_AvpRead25;
	AvpWrite26		=_AvpWrite26;
	
	AvpGetDosMemSize  =_AvpGetDosMemSize;
	AvpGetFirstMcbSeg  =_AvpGetFirstMcbSeg;
	AvpGetIfsApiHookTable  =_AvpGetIfsApiHookTable;
	AvpGetDosTraceTable  =_AvpGetDosTraceTable;
	AvpMemoryRead  =_AvpMemoryRead;
	AvpMemoryWrite  =_AvpMemoryWrite;
	
	AvpGetDiskParam  =_AvpGetDiskParam;

	AvpReadFile	=_AvpReadFile;
	AvpWriteFile	=_AvpWriteFile;
	AvpCreateFile	=_AvpCreateFile;
	AvpCloseHandle	=_AvpCloseHandle;
	AvpSetFilePointer =_AvpSetFilePointer;
	AvpSetEndOfFile =_AvpSetEndOfFile;
	AvpGetFileSize	=_AvpGetFileSize;
	AvpGetDiskFreeSpace =_AvpGetDiskFreeSpace;
	AvpGetFullPathName =_AvpGetFullPathName;
	AvpDeleteFile =_AvpDeleteFile;
	AvpSetFileAttributes =_AvpSetFileAttributes;
	AvpGetFileAttributes =_AvpGetFileAttributes;
	AvpGetTempPath =_AvpGetTempPath;
	AvpGetTempFileName =_AvpGetTempFileName;

	return TRUE;
}


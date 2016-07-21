////////////////////////////////////////////////////////////////////
//
//  REDIRS.CPP
//  Engine PRAGUE DLL Wrapper
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#if defined (__unix__)
	#define AVPIOSTATIC
#endif

#include "engine.h"
#include "static.h"
#include "../../Prague/TempFile2/plugin_temporaryio2.h"

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_tempfile.h>
#include <Prague/iface/i_seqio.h>
#include <AV/plugin/p_avpmgr.h>
#include <AV/iface/i_avpg.h>

#include <Extract/plugin/p_windiskio.h>

#include <ScanAPI/BaseAPI.h>
#include <ScanAPI/Avp_msg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h> 
#include <AV/plugin/p_l_llio.h> 
#include "DupMem.h"

extern "C" tPROPID propid_io_position_ptr;
extern "C" tPROPID propid_io_created;
extern "C" tPROPID propid_processing_error;


#ifndef _WINAPI_
	#define _WINAPI_  WINAPI
#endif

#ifndef FALSE
	#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif


extern "C" LRESULT _WINAPI_ _AvpCallback(WPARAM wParam,LPARAM lParam);

extern "C" hROOT g_root;

BYTE* _New(long size){
	void* ptr=NULL;
//	static tQWORD counter=0;
//	counter+=size;
	CALL_SYS_ObjHeapAlloc(Get_CurrentIO(),&ptr,size);
	return (unsigned char*)ptr;
}	
void _Delete(BYTE* ptr){
	if(ptr) 
		CALL_SYS_ObjHeapFree(g_root,ptr);
}

// Some versions of GCC seems to have serious problem overriding new/delete this way
#if !defined (__unix__)
void* _cdecl operator new(UINT size){
	return obj_malloc(size);
}

void _cdecl operator delete(void* ptr){
	obj_free(ptr);
}
#endif /* !defined (__unix__) */


//extern "C" HANDLE Cache_GetRealHandle(HANDLE  hFile);

DWORD _WINAPI_ _AvpGetTempPath( DWORD nBufferLength, LPTSTR lpBuffer )
{
//	DWORD ret=0;
//	if(lpBuffer && *lpBuffer 
//		&& PR_SUCC(CALL_SYS_ObjectValid(Get_AVP1(),(hOBJECT)Cache_GetRealHandle((*(AVPScanObject**)lpBuffer)->Handle)))
//		){
//		hSTRING TempPath;
//		if(PR_SUCC(CALL_SYS_ObjectCreateQuick((hOBJECT)Cache_GetRealHandle((*(AVPScanObject**)lpBuffer)->Handle),
//			&TempPath,IID_STRING,PID_ANY,SUBTYPE_ANY)))
//		{
//			CALL_SYS_SendMsg((hOBJECT)Cache_GetRealHandle((*(AVPScanObject**)lpBuffer)->Handle),pmc_TEMPFILE,pm_TEMPFILE_GETPATH,TempPath,0,0);
//			CALL_String_Length(TempPath,&ret);
//			CALL_SYS_ObjectClose(TempPath);
//			if(ret) return ret;
//		}
//	}
	strncpy_s(lpBuffer, nBufferLength, "C:\\", nBufferLength);
	return strlen(lpBuffer);
}


UINT  _WINAPI_ _AvpGetTempFileName(
  LPCTSTR lpPathName,      // directory name
  LPCTSTR lpPrefixString,  // file name prefix
  UINT uUnique,            // integer
  LPTSTR lpTempFileName    // file name buffer
)
{
	strcpy_s( lpTempFileName, 6, "{TMP}");
	return uUnique?uUnique:1;
}

_TCHAR* _WINAPI_ _AvpConvertChar(_TCHAR* str_,INT ct,_TCHAR* cp_)
{
#ifndef SYSTOS2
	switch(ct)
	{
	case 1:
		break;
	case 5:
		if (cp_ && (*(WORD*)cp_==0xFFFE || *(WORD*)cp_==0xFEFF))
		{
/*
			char buff[0x200];
			memset(buff,0x200,0);
			if(*(WORD*)cp_==0xFEFF)
				char *p=cp_+2;
				char *q=cp_+3;
				while( *p || *q){
					char c=*q;
					*(q++)=*p;
					*(p++)=c;
				}
			}
			if(WideCharToMultiByte(CP_ACP,0,(LPCWSTR)(cp_+2),-1,buff,0x200,NULL,NULL))
			{
				strcpy(str_,buff);
			}
*/
			
			hSTRING Str;
			if(PR_SUCC(CALL_SYS_ObjectCreateQuick(g_root,&Str,IID_STRING,PID_ANY,SUBTYPE_ANY))){
				if(PR_SUCC(CALL_String_ImportFromBuff(Str,0, cp_+2 ,0x200,(*(WORD*)cp_==0xFEFF)?cCP_UNICODE_BIGENDIAN:cCP_UNICODE,cSTRING_Z))){
					CALL_String_ExportToBuff(Str,0,cSTRING_WHOLE,str_,0x200,cCP_ANSI,cSTRING_Z);
				}
				CALL_SYS_ObjectClose(Str);
			}
		}
		break;
	default:
	case 0:
//		OemToChar(str_,str_);
		hSTRING Str;
		if(PR_SUCC(CALL_SYS_ObjectCreateQuick(g_root,&Str,IID_STRING,PID_ANY,SUBTYPE_ANY))){
			if(PR_SUCC(CALL_String_ImportFromBuff(Str,0,str_,0x200,cCP_OEM,cSTRING_Z))){
				CALL_String_ExportToBuff(Str,0,cSTRING_WHOLE,str_,0x200,cCP_ANSI,cSTRING_Z);
			}
			CALL_SYS_ObjectClose(Str);
		}
		break;
		
	}
#endif
	return str_;
}

_TCHAR* _WINAPI_ _AvpStrlwr(_TCHAR* str_)
{
#ifdef _MBCS
        _strlwr_s(str_, strlen(str_)+1);
		return str_;
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

_TCHAR* _WINAPI_ _AvpStrrchr(_TCHAR* str_, INT chr_){

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


DWORD WINAPI _AvpExecSpecial(
		char* FuncName, 
		DWORD wParam, 
		DWORD lParam
		)
{	
	char buf[0x200];
	tDWORD l;
	hOBJECT obj=(hOBJECT)Get_CurrentIO();
	if(!obj)
		obj=(hOBJECT)Get_AVP1();
	if(!obj)
		obj=(hOBJECT)g_root;
	
	if(!FuncName || !*FuncName)
		l=sprintf_s(buf,countof(buf),"ExecSpecial: bad FuncName parameter");
	else if(!ExecSpecial)
		l=sprintf_s(buf,countof(buf),"ExecSpecial: not linked");
	else{
		tERROR error=errOK;
		DWORD  ret=0;
		//error=ExecSpecial(obj,&ret,FuncName,wParam,lParam);
		DupCallbackCall(error=ExecSpecial(obj,&ret,FuncName,wParam,lParam));
		if(PR_FAIL(error)){
			l=sprintf_s(buf,countof(buf),"ExecSpecial: function failed: error=0x%08X (%s)",(tUINT)error,FuncName);
			switch(error){
			case errSIGN_BAD_KEY:
			case errSIGN_NOT_MATCH:
			case errSIGN_NOTARY_NOT_FOUND:
				_AvpCallback(AVP_CALLBACK_FILE_INTEGRITY_FAILED,(long)"ExecSpecial");
			default:
				break;
			}
		}
		else
			return ret;
	}
	CALL_SYS_SendMsg(obj,pmc_ENGINE_IO, pm_ENGINE_IO_OUTPUT_STRING,Get_AVP1(),buf,&l);
	return 0;
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
	hIO io=Get_CurrentIO();
	if(io==NULL)
		return FALSE;

	tDWORD origin=CALL_SYS_PropertyGetDWord(io,pgOBJECT_ORIGIN);
	if(origin==OID_PHYSICAL_DISK || origin==OID_LOGICAL_DRIVE)
	{
		tQWORD Cylinders=CALL_SYS_PropertyGetQWord(io,propid_boot_Cylinders); 
		tDWORD BytesPerSector=CALL_SYS_PropertyGetDWord(io,propid_boot_BytesPerSector);
		tDWORD SectorsPerTrack=CALL_SYS_PropertyGetDWord(io,propid_boot_SectorsPerTrack);
		tDWORD TracksPerCylinder=CALL_SYS_PropertyGetDWord(io,propid_boot_TracksPerCylinder);

		if(!BytesPerSector)
		{
			Cylinders=         CALL_SYS_PropertyGetQWord(io,plCylinders); 
			BytesPerSector=    CALL_SYS_PropertyGetDWord(io,plBytesPerSector);
			SectorsPerTrack=   CALL_SYS_PropertyGetDWord(io,plSectorsPerTrack); 
			TracksPerCylinder= CALL_SYS_PropertyGetDWord(io,plTracksPerCylinder); 
		}


		if(BytesPerSector) 
			ret=TRUE;
		
		if(SectorsPerTrack
		&& (SectorsPerTrack) < 0x40
		&& (TracksPerCylinder) <0x100
		){
			WORD w=(WORD)(0xFF & (Cylinders -1))<<8;
			w|=0xC0 & ((Cylinders -1)>>2);
			w|=0x3F & (BYTE)(SectorsPerTrack);
			if(CX)*CX=w;
			if(DH)*DH=(BYTE)(TracksPerCylinder -1);
		}
	}
	return ret;
}

#define CALL_SYS_PropertyCopy(a,b,p)  _SYS_PropertyCopy((hOBJECT)a,(hOBJECT)b,p)

tERROR _SYS_PropertyCopy(hOBJECT to, hOBJECT from, tPROPID propid)
{
	tERROR error=errOK;
	hSTRING str;
	tDWORD  size;
	tPTR    buf=0;

	if(PR_SUCC(error=CALL_SYS_PropertyIsPresent(from,propid))){
		switch(PR_PROP_TYPE(propid)){
		case pTYPE_STRING:
		case pTYPE_WSTRING:
			if(PR_SUCC(error=CALL_SYS_ObjectCreateQuick(to,&str,IID_STRING,PID_ANY,SUBTYPE_ANY))){
				if(PR_SUCC(error=CALL_String_ImportFromProp(str,&size,from,propid)))
					error=CALL_String_ExportToProp(str,&size,cSTRING_WHOLE,to,propid);
			}
			break;
		default:
			size=CALL_SYS_PropertySize(from,propid);
			if(size){
				error=CALL_SYS_ObjHeapAlloc(to,&buf,size);
				if(PR_SUCC(error))
					error=CALL_SYS_PropertyGet(from,&size,propid,buf,size);
			}
			if(PR_SUCC(error)) //prop can have zero size.
				error=CALL_SYS_PropertySet(to,&size,propid,buf,size);
			if(buf)
				CALL_SYS_ObjHeapFree(to,buf);
			break;
		}
	}
	return error;
}

BOOL WINAPI _AvpRead25(	BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
	BOOL ret=FALSE;
	hIO temp_disk_io=0;
	hIO disk_io=0;
	hIO io=Get_CurrentIO();
	if(io==NULL)
		return FALSE;
	
	switch(CALL_SYS_PropertyGetDWord(io,pgOBJECT_ORIGIN)){
	case OID_LOGICAL_DRIVE:
		disk_io=io;
		break;
	case OID_PHYSICAL_DISK:
		//open associated logical disk
		if(CALL_SYS_PropertyGetDWord(io,plIsPartition))
		{
			if(PR_SUCC(CALL_SYS_ObjectCreate( io, &temp_disk_io, IID_IO, PID_WINDISKIO, 0 ))){
				CALL_SYS_PropertyCopy(temp_disk_io,io,pgOBJECT_FULL_NAME);
				CALL_SYS_PropertySetDWord(temp_disk_io,pgOBJECT_ORIGIN,OID_LOGICAL_DRIVE);
				if(PR_SUCC(CALL_SYS_ObjectCreateDone( temp_disk_io)))
					disk_io=temp_disk_io;
			}
		}
		break;
	default:
		break;
	}
	if(disk_io){
		tDWORD count=0;
		tDWORD BytesPerSector=    CALL_SYS_PropertyGetDWord(disk_io,plBytesPerSector); 
		tQWORD offset=Sector*BytesPerSector;
		tDWORD size=NumSectors*0x200;        //!!! because Eugene assumes that.

		tERROR error=CALL_IO_SeekRead(disk_io,&count,offset,Buffer,size);
		if(count!=size) 
			error=errOBJECT_READ;
		if(PR_SUCC(error)) 
			ret=TRUE;
		else if(offset==0)
			CALL_SYS_PropertySetDWord( io, propid_processing_error, error);
	}
	if(temp_disk_io)
		CALL_SYS_ObjectClose(temp_disk_io);
	return ret;
}

BOOL WINAPI _AvpWrite26( BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
	BOOL ret=FALSE;
	hIO temp_disk_io=0;
	hIO disk_io=0;
	hIO io=Get_CurrentIO();
	if(io==NULL)
		return FALSE;
	
	switch(CALL_SYS_PropertyGetDWord(io,pgOBJECT_ORIGIN)){
	case OID_LOGICAL_DRIVE:
		disk_io=io;
		break;
	case OID_PHYSICAL_DISK:
		//open associated logical disk
		if(PR_SUCC(CALL_SYS_ObjectCreate( io, &temp_disk_io, IID_IO, PID_WINDISKIO, 0 ))){
			CALL_SYS_PropertyCopy(temp_disk_io,io,pgOBJECT_FULL_NAME);
			CALL_SYS_PropertySetDWord(temp_disk_io,pgOBJECT_ORIGIN,OID_LOGICAL_DRIVE);
			if(PR_SUCC(CALL_SYS_ObjectCreateDone( temp_disk_io)))
				disk_io=temp_disk_io;
		}
		break;
	default:
		break;
	}
	if(disk_io){
		tDWORD count=0;
		tDWORD BytesPerSector=    CALL_SYS_PropertyGetDWord(disk_io,plBytesPerSector); 
		tQWORD offset=Sector*BytesPerSector;
		tDWORD size=NumSectors*0x200;        //!!! because Eugene assumes that.
		
		tERROR error=CALL_IO_SeekWrite(disk_io,&count,offset,Buffer,size);
		if(count!=size) 
			error=errOBJECT_WRITE;
		if(PR_SUCC(error))
			ret=TRUE;
		else
			CALL_SYS_PropertySetDWord( io, propid_processing_error, error);
	}
	if(temp_disk_io)
		CALL_SYS_ObjectClose(temp_disk_io);
	return ret;
}

BOOL ReadWrite13(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer, BOOL write)
{
	BOOL ret=FALSE;
	tDWORD BytesPerSector=0; 
	tDWORD SectorsPerTrack=0; 
	tDWORD TracksPerCylinder=0; 
	hIO temp_disk_io=0;
	hIO disk_io=0;
	hIO io=Get_CurrentIO();
	if(io==NULL)
		return FALSE;
	
	switch(CALL_SYS_PropertyGetDWord(io,pgOBJECT_ORIGIN)){
	case OID_LOGICAL_DRIVE:
		BytesPerSector=CALL_SYS_PropertyGetDWord(io,propid_boot_BytesPerSector);
		SectorsPerTrack=CALL_SYS_PropertyGetDWord(io,propid_boot_SectorsPerTrack);
		TracksPerCylinder=CALL_SYS_PropertyGetDWord(io,propid_boot_TracksPerCylinder);
		if(BytesPerSector)	//this is a floppy image
		{
			disk_io=io;
			PR_TRACE(( g_root, prtNOTIFY, "AVP1\tRead13 - BytesPerSector %d from prop", BytesPerSector));
			break;
		}
		//open associated physical disk
		if(CALL_SYS_PropertyGetDWord(io,plIsPartition))
		{
			if(PR_SUCC(CALL_SYS_ObjectCreate( io, &temp_disk_io, IID_IO, PID_WINDISKIO, 0 ))){
				CALL_SYS_PropertyCopy(temp_disk_io,io,pgOBJECT_FULL_NAME);
				CALL_SYS_PropertySetDWord(temp_disk_io,pgOBJECT_ORIGIN,OID_PHYSICAL_DISK);
				if(PR_SUCC(CALL_SYS_ObjectCreateDone( temp_disk_io)))
					disk_io=temp_disk_io;
			}
		}
		else
		{
			TracksPerCylinder= CALL_SYS_PropertyGetDWord(io,plTracksPerCylinder); 
			SectorsPerTrack=   CALL_SYS_PropertyGetDWord(io,plSectorsPerTrack); 
			BytesPerSector=    CALL_SYS_PropertyGetDWord(io,plBytesPerSector); 

			if (BytesPerSector)
				disk_io=io;
		}
		break;
	case OID_PHYSICAL_DISK:
		disk_io=io;
		break;
	default:
		break;
	}
	if(disk_io){
		tDWORD count=0;
		tDWORD SecNo=Sector & 0x003F;
		tDWORD CylNo=((Sector<<2) & 0x0300) | Sector>>8;
		tDWORD size=NumSectors*0x200;        //!!! because Eugene assumes that.
		if(!BytesPerSector)
		{
			TracksPerCylinder= CALL_SYS_PropertyGetDWord(disk_io,plTracksPerCylinder); 
			SectorsPerTrack=   CALL_SYS_PropertyGetDWord(disk_io,plSectorsPerTrack); 
			BytesPerSector=    CALL_SYS_PropertyGetDWord(disk_io,plBytesPerSector); 
		}

		PR_TRACE(( g_root, prtNOTIFY, "AVP1\tRead13 - BytesPerSector %d", BytesPerSector));
		
		tQWORD offset=BytesPerSector * ((CylNo*TracksPerCylinder + Head)*SectorsPerTrack + SecNo - 1);

		tERROR error=write?
			CALL_IO_SeekWrite(disk_io,&count,offset,Buffer,size):
			CALL_IO_SeekRead(disk_io,&count,offset,Buffer,size);

		if(count!=size) 
		{
			error=write?errOBJECT_WRITE:errOBJECT_READ;
			PR_TRACE(( g_root, prtIMPORTANT, "AVP1\tRead13 - size(0x%x) != count(0x%x)", size, count));
		}
		
		if(PR_SUCC(error)) 
			ret=TRUE;
		else if(offset==0)
		{
			PR_TRACE(( g_root, prtIMPORTANT, "AVP1\tRead13 - read err %terr", error));
			CALL_SYS_PropertySetDWord( io, propid_processing_error, error);
		}
	}
	if(temp_disk_io)
		CALL_SYS_ObjectClose(temp_disk_io);
	return ret;
}

BOOL WINAPI _AvpRead13 (BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
	return ReadWrite13(Disk,Sector,Head,NumSectors,Buffer,FALSE);
}

BOOL WINAPI _AvpWrite13(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
	return ReadWrite13(Disk,Sector,Head,NumSectors,Buffer,TRUE);
}

DWORD WINAPI _AvpMemoryRead (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    LPBYTE  buffer	// pointer to buffer to read to 
	)
{
	DWORD ret=0;
	if(MemoryRead)
	{
		ret=MemoryRead(offset,size,buffer);
		PR_TRACE(( g_root, prtIMPORTANT, "AVP1\tMemoryRead %x (%x) returned %x",offset,size,ret));
	}
	else
		PR_TRACE(( g_root, prtERROR, "AVP1\tMemoryRead is not resolved"));
	return ret;
}
	
DWORD WINAPI _AvpMemoryWrite (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    LPBYTE  buffer	// pointer to buffer to read to 
	)
{
	DWORD ret=0;
	if(MemoryWrite)
	{
		ret=MemoryWrite(offset,size,buffer);
		PR_TRACE(( g_root, prtIMPORTANT, "AVP1\tMemoryWrite %x (%x) returned %x",offset,size,ret));
	}
	else
		PR_TRACE(( g_root, prtERROR, "AVP1\tMemoryWrite is not resolved"));
	return ret;
}

DWORD WINAPI _AvpGetDosMemSize ()
{
	DWORD ret=0;
	if(GetDosMemSize)
	{
		ret=GetDosMemSize();
		PR_TRACE(( g_root, prtIMPORTANT, "AVP1\tGetDosMemSize returned %x",ret));
	}
	else
		PR_TRACE(( g_root, prtERROR, "AVP1\tGetDosMemSize is not resolved"));
	return ret;
}

DWORD WINAPI _AvpGetFirstMcbSeg ()
{
	DWORD ret=0;
	if(GetFirstMcbSeg)
	{
		ret=GetFirstMcbSeg();
		PR_TRACE(( g_root, prtIMPORTANT, "AVP1\tGetFirstMcbSeg returned %x",ret));
	}
	else
		PR_TRACE(( g_root, prtERROR, "AVP1\tGetFirstMcbSeg is not resolved"));
	return ret;
}

DWORD WINAPI _AvpGetIfsApiHookTable (
									 DWORD* table, 
									 DWORD size //size in DWORDs
									 )
{
	DWORD ret=0;
	if(GetIfsApiHookTable)
	{
		ret=GetIfsApiHookTable(table,size);
		PR_TRACE(( g_root, prtIMPORTANT, "AVP1\tGetIfsApiHookTable returned %x",ret));
	}
	else
		PR_TRACE(( g_root, prtERROR, "AVP1\tGetIfsApiHookTable is not resolved"));
	return ret;
}

DWORD WINAPI _AvpGetDosTraceTable (
								   DWORD* table, 
								   DWORD size //size in DWORDs
								   )
{
	if(GetDosTraceTable) return GetDosTraceTable(table,size);
	return 0;
}


DWORD WINAPI _AvpGetFileAttributes( LPCTSTR lpFileName ){
	return 0;
}

BOOL WINAPI _AvpSetFileAttributes( LPCTSTR lpFileName, DWORD dwFileAttributes){
	return 0;
}

BOOL WINAPI _AvpGetDiskFreeSpace(
								 LPCTSTR lpRootPathName,	// address of root path 
								 LPDWORD lpSectorsPerCluster,	// address of sectors per cluster 
								 LPDWORD lpBytesPerSector,	// address of bytes per sector 
								 LPDWORD lpNumberOfFreeClusters,	// address of number of free clusters  
								 LPDWORD lpTotalNumberOfClusters 	// address of total number of clusters  
								 )
{
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
    strncpy_s(lpBuffer,nBufferLength,lpFileName,nBufferLength);
    l=strlen(lpBuffer);
#if !defined (__unix__)    
	*lpFilePart=_AvpStrrchr(lpBuffer,'\\');
#else
	*lpFilePart=_AvpStrrchr(lpBuffer,'/');
#endif
    if(*lpFilePart) (*lpFilePart)++;
	else *lpFilePart=lpBuffer;// + l;
	return l;
}

tERROR AllocateCurPos( hIO io, tQWORD** ppCurPos){
	tERROR error;
	if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc(io,(tPTR*)ppCurPos,sizeof(tQWORD))))
		error=CALL_SYS_PropertySetPtr(io,propid_io_position_ptr,*ppCurPos);
	return error;
}

extern "C" BOOL g_rotate_read;

BOOL WINAPI _AvpReadFile (
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,        // number of bytes to read
    LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
    OVERLAPPED FAR*  lpOverlapped       // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
        )
{
	tERROR error=errOK;
	tQWORD* pCurrPos=0;
	if(lpNumberOfBytesRead)
		*lpNumberOfBytesRead=0;
	if(nNumberOfBytesToRead){

//		if(IID_POSIO==OBJ_IID((hOBJECT)hFile)){
//			error=CALL_PosIO_Read((hPOSIO)hFile,lpNumberOfBytesRead,lpBuffer,nNumberOfBytesToRead);
//			if(PR_SUCC(error))
//				return TRUE;
//		}
//		else
		{
			if(PR_FAIL(error=CALL_SYS_PropertyGet((hIO)hFile,0,propid_io_position_ptr,&pCurrPos,sizeof(pCurrPos))))
				error=AllocateCurPos((hIO)hFile,&pCurrPos);
			if(PR_SUCC(error))
			{
				if(((((tDWORD)*pCurrPos) & 0xFFF )+nNumberOfBytesToRead) >= 0x1000)
					g_rotate_read=TRUE;
// moved to basework because of sync issues
//					error=_AvpCallback(AVP_CALLBACK_ROTATE,0);
//				if(error!=errOPERATION_CANCELED)
				{
					if(PR_SUCC(error=CALL_IO_SeekRead((hIO)hFile,lpNumberOfBytesRead,*pCurrPos,lpBuffer,nNumberOfBytesToRead)))
						*pCurrPos+=*lpNumberOfBytesRead;
				}
			}
		}
	}
	if(PR_FAIL(error))
	{
		switch(error)
		{
		case errOBJECT_SEEK:
		case errOUT_OF_OBJECT:
			error=errEOF;
			break;
		case errLOCKED:
			CALL_SYS_PropertySetDWord((hOBJECT)hFile, propid_processing_error, error);
		default:
			break;
		}
	}
	return PR_SUCC(error);
}

BOOL  WINAPI _AvpWriteFile (
    HANDLE  hFile,      // handle to file to write to
    LPCVOID  lpBuffer,  // pointer to data to write to file
    DWORD  nNumberOfBytesToWrite,       // number of bytes to write
    LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
        )
{
	tERROR error=errOK;
	tQWORD* pCurrPos=0;
	if(nNumberOfBytesToWrite){

again:

//		if(IID_POSIO==OBJ_IID((hOBJECT)hFile)){
//			error=CALL_PosIO_Write((hPOSIO)hFile,lpNumberOfBytesWritten,(VOID*)lpBuffer,nNumberOfBytesToWrite);
//			if(PR_SUCC(error))
//				return TRUE;
//		}
//		else
		{
			if(PR_FAIL(error=CALL_SYS_PropertyGet((hIO)hFile,0,propid_io_position_ptr,&pCurrPos,sizeof(pCurrPos))))
				error=AllocateCurPos((hIO)hFile,&pCurrPos);
			if(PR_SUCC(error)){
				if(((((tDWORD)*pCurrPos) & 0xFFF )+nNumberOfBytesToWrite) >= 0x1000)
					g_rotate_read=TRUE;

// moved to basework because of sync issues
//					error=_AvpCallback(AVP_CALLBACK_ROTATE,0);
//				if(error!=errOPERATION_CANCELED)
				{
					if(PR_SUCC(error=CALL_IO_SeekWrite((hIO)hFile,lpNumberOfBytesWritten,*pCurrPos,(VOID*)lpBuffer,nNumberOfBytesToWrite)))
					{
						*pCurrPos+=*lpNumberOfBytesWritten;
						CALL_SYS_SendMsg((hIO)hFile,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_FLUSH,Get_AVP1(),0,0);
						return TRUE;
					}
				}
			}
		}		
		if(PR_FAIL(error)){
			tDWORD mode=CALL_SYS_PropertyGetDWord((hIO)hFile,pgOBJECT_ACCESS_MODE);
			if(!( mode & fACCESS_WRITE )){
				mode|=fACCESS_WRITE;
				if(PR_SUCC(CALL_SYS_PropertySetDWord((hIO)hFile,pgOBJECT_ACCESS_MODE,mode)))
					goto again;
				PR_TRACE(( Get_AVP1(), prtERROR, "AVP1     ERROR: can't change pgOBJECT_ACCESS_MODE" ));
			}
		}
	}
	if(lpNumberOfBytesWritten)*lpNumberOfBytesWritten=0;
	return ( PR_SUCC(error));
}

BOOL  WINAPI _AvpDeleteFile (
    LPCTSTR  lpFileName        // pointer to name of the file
	)
{
	return TRUE;
}

HANDLE  WINAPI _AvpCreateFile (
									LPCTSTR  lpFileName,        // pointer to name of the file
									DWORD  dwDesiredAccess,     // access (read-write) mode
									DWORD  dwShareMode, // share mode
									SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
									DWORD  dwCreationDistribution,      // how to create
									DWORD  dwFlagsAndAttributes,        // file attributes
									HANDLE  hTemplateFile       // handle to file with attributes to copy
									)
{
	if(lpFileName==0) return INVALID_HANDLE_VALUE;

	tERROR error=errOK;
	//DWORD mode=0;
	hOBJECT io=NULL;
	hOBJECT parent=(hOBJECT)Get_CurrentIO();
	if(parent==NULL) parent=(hOBJECT)Get_AVP1();
	dwDesiredAccess|=GENERIC_READ;
	
	if(dwFlagsAndAttributes & FILE_ATTRIBUTE_TEMPORARY){
		error=CALL_SYS_ObjectCreateQuick( parent, &io, IID_IO, PID_TMPFILE, 0 );
//until we solve problem with buffer
//		error=CALL_SYS_ObjectCreateQuick( parent, &io, IID_POSIO, PID_TEMPIO2, 0 );
		if(PR_FAIL(error)){
			PR_TRACE(( Get_AVP1(), prtERROR, "AVP1     ERROR: can't create TEMPFILE" ));
			return INVALID_HANDLE_VALUE;
		}
		else
		{
			error=CALL_SYS_PropertySetObj(   io,pgOBJECT_BASED_ON,parent);
		}
	}
	else{
		if( PR_SUCC( error=CALL_SYS_ObjectCreate( parent, &io, IID_IO, PID_NATIVE_FIO, 0 ))) {
			if( PR_SUCC( error=CALL_SYS_PropertySetDWord( io, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST ))
				&&  PR_SUCC( error=CALL_SYS_PropertySetDWord( io, pgOBJECT_ACCESS_MODE, fACCESS_READ))){
				error=CALL_SYS_PropertySetStr(io, 0, pgOBJECT_NAME, (VOID*)lpFileName, strlen(lpFileName) + 1,cCP_ANSI);
				if( PR_SUCC( error))
					error=CALL_SYS_ObjectCreateDone( io );
			}
			if(PR_FAIL(error))
				CALL_SYS_ObjectClose(io);
		}
		if(PR_FAIL(error)){
			PR_TRACE(( Get_AVP1(), prtERROR, "AVP1     ERROR: can't create PID_NATIVE_FIO: %s",lpFileName ));
			return INVALID_HANDLE_VALUE;
		}
	}
	
	error=CALL_SYS_PropertySetDWord(io,propid_io_created,1);
	
	if(PR_FAIL(error)){
		PR_TRACE(( Get_AVP1(), prtERROR, "AVP1     ERROR: can't create IO: 0x%08X",lpFileName ));
		return INVALID_HANDLE_VALUE;
	}
	return (HANDLE)io;
}

BOOL  WINAPI _AvpCloseHandle(
								  HANDLE  hFile     // handle to object to close
								  )
{
	if(CALL_SYS_PropertyGetBool((hOBJECT)hFile,pgOBJECT_DELETE_ON_CLOSE)){
		hOBJECT integral_parent_io=CALL_SYS_PropertyGetObj((hOBJECT)hFile,propid_integral_parent_io);
		hOBJECT executable_parent=CALL_SYS_PropertyGetObj((hOBJECT)hFile,propid_object_executable_parent);
		if(integral_parent_io)
			CALL_SYS_PropertySetBool(integral_parent_io,pgOBJECT_DELETE_ON_CLOSE,cTRUE);
		else if(executable_parent)
			CALL_SYS_PropertySetBool(executable_parent,pgOBJECT_DELETE_ON_CLOSE,cTRUE);
		else
		{
			tERROR error;
			tDWORD size=sizeof(error);
			error=CALL_SYS_PropertyGetDWord((hOBJECT)hFile,propid_object_readonly_error);
			if(PR_SUCC(error))
				error=errUNEXPECTED;
			CALL_SYS_SendMsg((hOBJECT)hFile,pmc_IO,pm_IO_DELETE_ON_CLOSE_FAILED,Get_AVP1(),&error,&size);
		}
	}
	
	if(IID_POSIO!=OBJ_IID((hOBJECT)hFile)){
		tPTR pCurrPos;
		if((pCurrPos=CALL_SYS_PropertyGetPtr((hOBJECT)hFile,propid_io_position_ptr))!=0){
			CALL_SYS_ObjHeapFree((hOBJECT)hFile,pCurrPos);
			CALL_SYS_PropertyDelete((hOBJECT)hFile,propid_io_position_ptr);
		}
	}

	if(CALL_SYS_PropertyGetDWord((hOBJECT)hFile,propid_io_created)){
		return PR_SUCC(CALL_SYS_ObjectClose((hOBJECT)hFile));
	}

	return TRUE;
}


DWORD  WINAPI _AvpSetFilePointer(
    HANDLE  hFile,      // handle of file
    LONG  lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh,        // address of high-order word of distance to move
    DWORD  dwMoveMethod         // how to move
        )
{
	tQWORD* pCurrPos;
	tQWORD CurrLen;
	tERROR error;
	tLONGLONG DistanceToMove=(tLONGLONG)(lDistanceToMove | (lpDistanceToMoveHigh?(((tQWORD)(*lpDistanceToMoveHigh))<<32):0));

	if(IID_POSIO==OBJ_IID((hOBJECT)hFile)){
		tQWORD Pos;
		CALL_PosIO_Seek((hPOSIO)hFile,&Pos,DistanceToMove,dwMoveMethod);
		return (DWORD) Pos;
	}


	if(PR_FAIL(error=CALL_SYS_PropertyGet((hIO)hFile,0,propid_io_position_ptr,&pCurrPos,sizeof(pCurrPos))))
		error=AllocateCurPos((hIO)hFile,&pCurrPos);

	if(PR_FAIL(error))
		return (tDWORD) -1;
	
	switch(dwMoveMethod)
	{
	case SEEK_SET:
		if(DistanceToMove<0) 
			return (tDWORD) -1;
		*pCurrPos=DistanceToMove; 
		break;
		
	case SEEK_CUR:
		if(DistanceToMove<0)
		{
			if(*pCurrPos < (tQWORD)(-DistanceToMove))
				return (tDWORD) -1;
			*pCurrPos -=  (tQWORD)(-DistanceToMove);

		}else{
			*pCurrPos +=  (tQWORD)(DistanceToMove);
		}
		break;
		
	case SEEK_END:
		if(PR_FAIL(CALL_IO_GetSize((hIO)hFile,&CurrLen,IO_SIZE_TYPE_EXPLICIT)))
			return (tDWORD) -1;
		if(DistanceToMove<0)
		{
			if(CurrLen < (tQWORD)(-DistanceToMove))
				return (tDWORD) -1;
			*pCurrPos = CurrLen - (tQWORD)(-DistanceToMove);
		}else{
			*pCurrPos = CurrLen + (tQWORD)(DistanceToMove);
		}
		break;
		
	default:
		return (tDWORD) -1;
	}

	return (DWORD)*pCurrPos;
}

BOOL  WINAPI _AvpSetEndOfFile(
    HANDLE  hFile       // handle of file
        )
{
	BOOL ret=0;
	tQWORD* pCurrPos=0;
	tERROR error;
	tQWORD Pos;
	if(IID_POSIO==OBJ_IID((hOBJECT)hFile)){
		error=CALL_PosIO_Seek((hPOSIO)hFile,&Pos,0,cSEEK_CUR);
		pCurrPos=&Pos;
	}
	else{
		if(PR_FAIL(error=CALL_SYS_PropertyGet((hIO)hFile,0,propid_io_position_ptr,&pCurrPos,sizeof(pCurrPos))))
			error=AllocateCurPos((hIO)hFile,&pCurrPos);
	}

	if(PR_SUCC(error)){
again:
		if(PR_SUCC(CALL_IO_SetSize((hIO)hFile,*pCurrPos))){
			CALL_SYS_SendMsg((hIO)hFile,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_FLUSH,Get_AVP1(),0,0);
			ret=1;
		}
		else{
			tDWORD mode=CALL_SYS_PropertyGetDWord((hIO)hFile,pgOBJECT_ACCESS_MODE);
			if(!( mode & fACCESS_WRITE )){
				mode|=fACCESS_WRITE;
				if(PR_SUCC(CALL_SYS_PropertySetDWord((hIO)hFile,pgOBJECT_ACCESS_MODE,mode)))
					goto again;
				PR_TRACE(( Get_AVP1(), prtERROR, "AVP1     ERROR: can't change pgOBJECT_ACCESS_MODE" ));
			}
		}
	}
	return ret;
}

DWORD WINAPI _AvpGetFileSize(
    HANDLE  hFile,      // handle of file
    LPDWORD  lpHigh
        )
{
	tQWORD size;
	if(PR_SUCC(CALL_IO_GetSize((hIO)hFile,&size,IO_SIZE_TYPE_EXPLICIT))){
		if(lpHigh) 
			*lpHigh=(DWORD)(size>>32);
		return (DWORD)size;
	}
	return (tDWORD) -1;
}
/*
BOOL WINAPI AvpFlush(
    HANDLE  hFile       // handle of file
        )
{
	return PR_SUCC(CALL_IO_Flush((hIO)hFile));
}
BOOL WINAPI _AvpFlush(
    HANDLE  hFile       // handle of file
        )
{
	return PR_SUCC(CALL_IO_Flush((hIO)hFile));
}

extern "C" int _Destroy(void)
{
	return 1;
}

extern "C" int _Init(void)
{
	return 1;
}
*/

BOOL LoadIO()
{
    AvpRead13 = _AvpRead13;
	AvpWrite13 = _AvpWrite13;
	AvpRead25 = _AvpRead25;
	AvpWrite26 = _AvpWrite26;
	AvpGetDiskParam = _AvpGetDiskParam;

	AvpGetFirstMcbSeg = _AvpGetFirstMcbSeg;
	AvpGetDosMemSize = _AvpGetDosMemSize;
	AvpGetIfsApiHookTable = _AvpGetIfsApiHookTable;
	AvpGetDosTraceTable = _AvpGetDosTraceTable;
	AvpMemoryRead = _AvpMemoryRead;
	AvpMemoryWrite = _AvpMemoryWrite;

	AvpCallback = _AvpCallback;
	AvpStrrchr = _AvpStrrchr;
	AvpStrlwr = _AvpStrlwr;
	AvpConvertChar = _AvpConvertChar;
	AvpGetTempPath = _AvpGetTempPath;
	AvpGetTempFileName = _AvpGetTempFileName;

	AvpGetFileAttributes = _AvpGetFileAttributes;
	AvpSetFileAttributes = _AvpSetFileAttributes;


	AvpWriteFile = _AvpWriteFile;
	AvpCreateFile = _AvpCreateFile;
	AvpCloseHandle = _AvpCloseHandle;
	AvpReadFile = _AvpReadFile;
	AvpGetFileSize = _AvpGetFileSize;
	AvpSetFilePointer = _AvpSetFilePointer;
	AvpSetEndOfFile = _AvpSetEndOfFile;
	AvpDeleteFile = _AvpDeleteFile;

	
	AvpGetDiskFreeSpace = _AvpGetDiskFreeSpace;
	AvpGetFullPathName  = _AvpGetFullPathName;
	AvpExecSpecial = _AvpExecSpecial;
	
    return TRUE;
}

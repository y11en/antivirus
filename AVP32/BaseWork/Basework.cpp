////////////////////////////////////////////////////////////////////
//
//  BASEWORK.CPP
//  Engine implementation. Main file.
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#ifdef __MWERKS__
	#include <string.h>
	#include "avp_msg.h"
	#include "../commonfiles/byteorder.h"
	#include "bases/retcode3.h"
	#define strchr __strchr
//	#undef _MSC_VER
#endif	

//#define _MBCS
#if (defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))
  #define NOAFX
  #include <os2.h>
  #include <string.h>
  #include "../typedef.h"
  #include "../retcode3.h"
  #include "../../common.fil/scanapi/avp_msg.h"
  #include <tchar.h>
  #define AFXAPI
  #include "../../common.fil/stuff/_carray.h"

  #include "../../common.fil/dupmem/dupmem.h"
  #include <conio.h>
#endif

#if defined (__unix__)
	/* 
	 * Real new version UNIX declarations and definitions 
	 */
	#include <IOCache/fileio.h>
#else
	#include <conio.h>
#endif

#ifndef FILE_ATTRIBUTE_TEMPORARY
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100
#endif
#ifndef FILE_FLAG_DELETE_ON_CLOSE
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#endif


//#define PRINTF_ERRORS 1
#ifdef _DEBUG
//#define s_DEBUG
#endif

#ifdef _DEBUG
//#define P_SEEK_DEBUG
#endif

#ifdef _DEBUG
//#define SAVE_MARKS
#endif

#include <byteorder.h>

#define AVPIOSTATIC

#include <Bases/Format/Base.h>
#include <Bases/Method.h>
#include <_AVPIO.h>
#include <Stuff/CalcSum.h>
#include <Sign/SIGN.h>
#include "Basework.h"
#include "_printf.h"

//#define _MAKE2BYTES 00
#ifdef USE_STATIC_BDATA
BData* bdata=0;
#endif


#ifdef DONT_USE_FILEIO_CACHE
#	define CACHEDEF(x) x
#	define Cache_Init()
#	define Cache_Destroy()
#	define AvpFlush(x)
#else
#	define CACHEDEF(x) Cache_##x
extern "C" int Cache_Init();
extern "C" int Cache_Destroy();
BOOL WINAPI CACHEDEF(AvpFlush)(HANDLE  hFile );
#endif


#ifdef _DEBUG
#define _DPrintf Printf
#else
//#define _DPrintf(args) 
#define _DPrintf //
#endif

#if !( (defined(BASE_EMULATOR)) || (defined(__unix__)) || (defined(__WATCOMC__) && (defined(__DOS__) || defined(__OS2__))))
#define STACK_ALIGN_ON \
BOOL stack_align=0;\
{\
	__asm test esp,3\
	__asm jz no_align\
	__asm push bx\
	__asm mov stack_align,1\
}\
no_align: 
#define STACK_ALIGN_OFF \
if(stack_align)\
	__asm 	pop bx;
#else
#define STACK_ALIGN_ON
#define STACK_ALIGN_OFF
#endif


extern DWORD FAR unsqu_y (BYTE* i_buf,BYTE *o_buf, void* yield);

#define         DOS_BIOS_MAX            15
#define         INT_1C_MAX              0x40
#define         MAX_HOOK_LINES          0x20
#define         MAX_TRACE_LINES         0x20
#define         MAX_MCB_LINES           0x200


LOAD_PROGRESS_STRUCT LPS;

extern "C" BOOL callbackNames=0;
extern "C" DWORD callbackRecords=0;

#if defined (__unix__)
	extern "C" void BAvpSetCallbackNames(BOOL cn) {  callbackNames=cn; }
	extern "C" void BAvpSetCallbackRecords(DWORD cn) {  callbackRecords=cn; }
	extern "C" int match(register char *s, register char *p);
#endif

#if defined (_WIN32)
#define BLOCKSIZE 0x00010000
#else
#define BLOCKSIZE 0x0
#endif

#define  DOSMEM_SIZE 0x110000
#define  DOSMEM_READSIZE 0xA0000

//#define USE_BOMB_HEURISTICS

#define MaxLevelOfEmbedding       128
#define MaxCompressionRate      10240
#define MinSizeLevel         20480000 //20M
#define MaxRepeatBytes      100000000 //100M
#define MaxRepeatStructure         80

#define P_OK    0
#define P_ERR   1
#define P_END   2
#define P_DISK  3
#define P_VOL   4
#define P_PASS  5


//DWORD Check_Sum(BYTE* mass,WORD len){return CalcSum(mass,len);}

void InitCache(BYTE* m,DWORD* d)
{
	int i;
	DWORD s=0;
	for (i=0; i<4 ; i++,m++,d++){
		*d=s;
		s^=s<<8;
		s^=*m;
	}
	for ( ;i<(CACHE_SIZE-1); i++,m++,d++){
		*d=s;
		s^=s<<1;
		s^=*m;
	}
	*d=s;
}

void FreeAllPtr(CPtrArray& array){
	int i;
	while(0!=(i=array.GetSize()))
	{      //!!!!!!!!!!!!!!!!!!!!!!!!
		i--;
		void* ptr=array.GetAt(i);
		if(ptr!=NULL)delete (BYTE*)ptr;
		array.RemoveAt(i);
	}
}

extern "C" unsigned char ImgBoot95[];
extern "C" unsigned char ImgMBR95[];

void* Get_Sector_Image (BYTE SubType)
{
#ifdef _DEBUG
	Printf("GetSectorImage type %d",SubType);
#endif
	void* image=(void*)AvpCallback(AVP_CALLBACK_GET_SECTOR_IMAGE,(DWORD)SubType);
	if(image)return image;
	
	if(SubType==ST_MBR)     return (void*)ImgMBR95;
	else                            return (void*)ImgBoot95;
	
}

int dummy()
{
//      TRACE("Error link call.");
	return 0;
}

DWORD _DoJump(CBaseWork* bw,BYTE* Page)
{
	return bw->DoJump(Page);
}

DWORD _Seek(CBaseWork* bw, LONG lOff)
{
	DWORD r;
	STACK_ALIGN_ON;
	r=CACHEDEF(AvpSetFilePointer)(bw->WorkArea->Handle,lOff,NULL,FILE_BEGIN);
	/*
	Printf("Seek: off:%d handle:%d size:%d",
	lOff,bw->WorkArea->Handle,
	CACHEDEF(AvpGetFileSize)(bw->WorkArea->Handle,NULL));
	*/
	if(r==(DWORD)-1){
		r=0;
#ifdef PRINTF_ERRORS
		Printf("Seek failed %d",lOff);
#endif
	}
	STACK_ALIGN_OFF;
	return r;
}

DWORD _LSeek(CBaseWork* bw, long offset,int from)
{
	DWORD r;
	STACK_ALIGN_ON;
	r=CACHEDEF(AvpSetFilePointer)(bw->WorkArea->Handle,offset,NULL,from);
	/*
	Printf("LSeek: off:%d from:%d  handle:%d size:%d",
	offset,from,bw->WorkArea->Handle,
	CACHEDEF(AvpGetFileSize)(bw->WorkArea->Handle,NULL));
	*/
	if(r==(DWORD)-1){
		r=0;
#ifdef PRINTF_ERRORS
		Printf("LSeek failed: %d",offset);
#endif
	}
	STACK_ALIGN_OFF;
	return r;
}

char BombName[]="\n\0\0Mail Bomb";

DWORD _Read(CBaseWork* bw, void* lpBuf, UINT nCount )
{
	DWORD bytes=0;
	STACK_ALIGN_ON;
	//Printf("Read: handle:%d count:%d",bw->WorkArea->Handle,nCount);
	
	if(bdata->BombSize<0)
		bdata->CancelProcess=1;

	if(!CACHEDEF(AvpReadFile)(bw->WorkArea->Handle,lpBuf,nCount,&bytes,NULL))
	{
#ifdef PRINTF_ERRORS
		Printf("Read failed: %d",nCount);
#endif
	}
	
	STACK_ALIGN_OFF;
	return bytes;
}


#ifndef __unix__
_TCHAR TempDisk[5];
#endif
#define DSK_STEP 10240

BOOL IfCanWrite(CBaseWork* bw, UINT nCount)
{
	static UINT count;
	count+=nCount;
	if(count> DSK_STEP*1024){
		DWORD SectorsPerCluster;        // address of sectors per cluster
		DWORD BytesPerSector;   // address of bytes per sector
		DWORD NumberOfFreeClusters;     // address of number of free clusters
		DWORD TotalNumberOfClusters;    // address of total number of clusters
#ifndef __unix__
		if(AvpGetDiskFreeSpace(TempDisk,
#else
		if(AvpGetDiskFreeSpace(bdata->TempPath,
#endif
				&SectorsPerCluster,
                &BytesPerSector,
                &NumberOfFreeClusters,
				&TotalNumberOfClusters))
			{
				DWORD l=((SectorsPerCluster*BytesPerSector)/256)*(NumberOfFreeClusters/4);
				if(l < DSK_STEP*2){
					bw->WorkArea->RFlags|=RF_DISK_OUT_OF_SPACE;
					bdata->DiskFull=1;
//					Printf("Low space on disk: %d kb  (Sec/Cl=%d Bytes/Sec=%d FreeCl=%d)",l,SectorsPerCluster,BytesPerSector,NumberOfFreeClusters);
//					_cprintf("Low space on disk: %d kb  (Sec/Cl=%d Bytes/Sec=%d FreeCl=%d)",l,SectorsPerCluster,BytesPerSector,NumberOfFreeClusters);
					//                              return 0;
					return FALSE;
				}
			}
		count=0;
	}
	return TRUE;
}


DWORD _Write(CBaseWork* bw, const void* lpBuf, UINT nCount )
{
	DWORD bytes=0;
	STACK_ALIGN_ON;
	BData* _bdata=bdata;
	
	//      Printf("Write: handle:%d count:%d",bw->WorkArea->Handle,nCount);

	if(_bdata->BombSize){
		_bdata->BombSize-=nCount;
		if(_bdata->BombSize==0)
			_bdata->BombSize=-1;
		if(_bdata->BombSize<0)
		{
			_bdata->CancelProcess=1;
			goto ret;
		}
		if(nCount){
			for(UINT i=0; i<nCount; i++)
			{
				_bdata->xbp ^= ((BYTE*)lpBuf)[i];
				_bdata->ibp ++;
				if(_bdata->ibp == MaxRepeatStructure)
				{
					_bdata->ibp=0;
					if(_bdata->BP != _bdata->xbp)
					{
						_bdata->BP = _bdata->xbp;
						_bdata->xbp = 0;
						_bdata->BPCount=0;
					}
					_bdata->BPCount += MaxRepeatStructure;
				}
			}

			if(_bdata->BPCount > MaxRepeatBytes)
			{
				_bdata->BombSize=-1;
				_bdata->CancelProcess=1;
				goto ret;
			}
		}
	}

	if(!IfCanWrite(bw,nCount) || !CACHEDEF(AvpWriteFile)(bw->WorkArea->Handle,lpBuf,nCount,&bytes,NULL))
	{
#ifdef PRINTF_ERRORS
		Printf("Write failed: %d",nCount);
#endif
		bw->WorkArea->RFlags|=RF_IO_ERROR;
    }
ret:
	STACK_ALIGN_OFF;
	return bytes;
}

BOOL _Read13 (CBaseWork* bw, BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, LPBYTE Buffer)
{
	BOOL ret;
	STACK_ALIGN_ON;
	AvpCallback(AVP_CALLBACK_ROTATE,(DWORD)bw->WorkArea->ScanObject);
	ret=AvpRead13(Disk, Sector ,Head ,NumSectors, Buffer);
	if(!ret)
	{
		memset(Buffer,0,NumSectors*0x200);
		if(Sector==1 && Head==0)
			bw->WorkArea->RFlags|=RF_IO_ERROR;
	}
	STACK_ALIGN_OFF;
	return ret;
}

BOOL _Write13(CBaseWork* bw, BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, LPBYTE Buffer)
{
	BOOL ret;
	STACK_ALIGN_ON;
	AvpCallback(AVP_CALLBACK_ROTATE,(DWORD)bw->WorkArea->ScanObject);
	ret=AvpWrite13(Disk, Sector ,Head ,NumSectors, Buffer);
	if(!ret)
		bw->WorkArea->RFlags|=RF_IO_ERROR;
	STACK_ALIGN_OFF;
	return ret;
}

BOOL _Read25(
			 CBaseWork* bw,
			 BYTE Drive,
			 DWORD Sector,
			 WORD NumSectors,
			 LPBYTE Buffer
			 )
{
	BOOL ret;
	STACK_ALIGN_ON;
	AvpCallback(AVP_CALLBACK_ROTATE,(DWORD)bw->WorkArea->ScanObject);
	ret=AvpRead25(Drive, Sector, NumSectors, Buffer);
	if(!ret)
	{
		memset(Buffer,0,NumSectors*0x200);
		if(Sector==0)
			bw->WorkArea->RFlags|=RF_IO_ERROR;
	}
	//else Printf("%02X%02X%02X%02X",Buffer[3],Buffer[4],Buffer[5],Buffer[6]);
	STACK_ALIGN_OFF;
	return ret;
}

BOOL _Write26(
			  CBaseWork* bw,
			  BYTE Drive,
			  DWORD Sector,
			  WORD NumSectors,
			  LPBYTE Buffer
			  )
{
	BOOL ret;
	STACK_ALIGN_ON;
	AvpCallback(AVP_CALLBACK_ROTATE,(DWORD)bw->WorkArea->ScanObject);
	ret=AvpWrite26(Drive, Sector, NumSectors, Buffer);
	if(!ret)
		bw->WorkArea->RFlags|=RF_IO_ERROR;
	STACK_ALIGN_OFF;
	return ret;
}

int _ChSize(CBaseWork* bw,DWORD len)
{
	int ret=0;
	STACK_ALIGN_ON;
	CACHEDEF(AvpSetFilePointer)(bw->WorkArea->Handle,len,NULL,FILE_BEGIN);
	ret=CACHEDEF(AvpSetEndOfFile)(bw->WorkArea->Handle);
	if(!ret)
		bw->WorkArea->RFlags|=RF_IO_ERROR;
	STACK_ALIGN_OFF;
	return ret;
}


BOOL MemoryRead(DWORD offset, DWORD len, BYTE* buffer)
{
    if(bdata->DosMemory && (offset+len)<DOSMEM_SIZE)
		memcpy(buffer,bdata->DosMemory+offset,len);
	else if((offset)<=0x110000)
		memset(buffer,0,len);
    else
		return AvpMemoryRead(offset,len,buffer);
	return TRUE;
}

DWORD Peek(WORD seg, WORD off)
{
    DWORD dw;
	DWORD offset=seg; offset<<=4; offset+=off;
	MemoryRead(offset,4,(BYTE*)&dw);
    return dw;
}

#define DosPeek(seg,off) (*(DWORD*)(bdata->DosMemory+(((DWORD)seg)<<4)+off))

BOOL Poke(WORD seg, WORD off, DWORD value, BYTE size)
{
	DWORD offset=seg; offset<<=4; offset+=off;
    if(bdata->DosMemory && (offset+4)<DOSMEM_SIZE)
		memcpy(bdata->DosMemory+offset,&value,size);
    return AvpMemoryWrite(offset,size,(BYTE*)&value);
}

static int spec_entry=-1;
BOOL SavePages(CBaseWork* bw){
	BOOL ret;
	DWORD entry = bw->WorkArea->ScanObject->Entry;
	bw->WorkArea->ScanObject->Entry=spec_entry--;
	ret=AvpCallback(AVP_CALLBACK_SAVE_PAGES,(DWORD)&bw->WorkArea);
	bw->WorkArea->ScanObject->Entry = entry;
	return ret;
}
BYTE*   _New(long size);
void    _Delete(BYTE* ptr);

WORD _Rotate(CBaseWork* bw, char flag)                                  //rotate symbol in bottom line
{
	WORD ret;
#ifdef _WIN32
	if(flag!=2) return 0;
#endif
	STACK_ALIGN_ON;
	AvpCallback(flag?AVP_CALLBACK_ROTATE:AVP_CALLBACK_ROTATE_OFF,(DWORD)bw->WorkArea->ScanObject);
	if(bdata->CancelProcess || bdata->DiskFull)
		ret=1;
	else
		ret=0;
	STACK_ALIGN_OFF;
	return ret;
}
 
#define COPY_SIZE 0x1000

int             Copy_File(HANDLE ho,HANDLE hi,CBaseWork* bw)         //copy handles
{
	DWORD bytes;
	DWORD rd;
	BYTE  *buf;
	int count=0;
	CACHEDEF(AvpSetFilePointer)(hi,0,NULL,FILE_BEGIN);
	CACHEDEF(AvpSetFilePointer)(ho,0,NULL,FILE_BEGIN);
	if(!CACHEDEF(AvpSetEndOfFile)(ho))return -1;
	buf=new BYTE[COPY_SIZE];
	if(buf==NULL)return -1;
	
	while(CACHEDEF(AvpReadFile)(hi,buf,COPY_SIZE,&bytes,NULL)){
		if(!bytes)break;
		rd=bytes;
		if(!CACHEDEF(AvpWriteFile)(ho,buf,bytes,&bytes,NULL) || bytes!=rd || _Rotate(bw, 1))
		{
			count=-1;
			break;
		}
		count+=bytes;
	}
	_Rotate(bw, 0);
	delete buf;
	return count;
}

int _Copy_P_File(CBaseWork* bw)                             //copy handles
{
	int ret=P_OK;
	STACK_ALIGN_ON;
	int count=Copy_File(bw->WorkArea->Handle,bw->WorkArea->P_Handle,bw);
	if(count == -1){
		bw->WorkArea->RFlags|=RF_IO_ERROR;
		ret=P_ERR;
	}
	STACK_ALIGN_OFF;
	return ret;
}

int             _P_Read(CBaseWork* bw, BYTE* buff,WORD size)   //read from packed handle
{
	DWORD bytes=0;
	STACK_ALIGN_ON;
	if(bdata->BombSize>=0)
		CACHEDEF(AvpReadFile)(bw->WorkArea->P_Handle,buff,size,&bytes,NULL);
	else
		bdata->CancelProcess=1;

#ifdef P_SEEK_DEBUG	
	
	Printf("P_Read: handle:%d size:%d return:%d",
		bw->WorkArea->P_Handle,size,bytes);
	
#endif	
	
	STACK_ALIGN_OFF;
	return bytes;
}

long    _P_Seek(CBaseWork* bw, long offset,int from)        //seek on paked file
{
	//      return CACHEDEF(AvpSetFilePointer)(bw->WorkArea->P_Handle,offset,NULL,from);
	DWORD r;
	STACK_ALIGN_ON;
	r=CACHEDEF(AvpSetFilePointer)(bw->WorkArea->P_Handle,offset,NULL,from);

#ifdef P_SEEK_DEBUG	
	Printf("P_Seek: off:%d from:%d handle:%d size:%d return:%d",
	offset,from,bw->WorkArea->P_Handle,
	CACHEDEF(AvpGetFileSize)(bw->WorkArea->P_Handle,NULL),
	r);
#endif

	if(r==(DWORD)-1){
		r=0;
		//              Printf("P_Seek failed: %0X",GetLastError());
	}
	STACK_ALIGN_OFF;
	return r;
}

int             _V_Read(CBaseWork* bw, BYTE NEAR* buff,WORD size)   //read from tmp handle
{
// mdw no reference to symbol 'bw'
	DWORD bytes=0;
	STACK_ALIGN_ON;
	if(bdata->BombSize<0)
	{
		bdata->CancelProcess=1;
		goto ret;
	}
	if(CACHEDEF(AvpReadFile)(bw->WorkArea->V_Handle,buff,size,&bytes,NULL))
	{
		if(bdata->BombSize >0)	bdata->BombSize+=bytes;
    }
ret:
	STACK_ALIGN_OFF;
	return bytes;
}

int             _V_Write(CBaseWork* bw, BYTE NEAR* buff,WORD nCount)  //write to tmp handle
{
	DWORD bytes=0;
	STACK_ALIGN_ON;
	if(bdata->BombSize){
		bdata->BombSize-=nCount;
		if(bdata->BombSize==0)
			bdata->BombSize=-1;
		if(bdata->BombSize<0)
		{
			bdata->CancelProcess=1;
			goto ret;
		}
	}
	if(!IfCanWrite(bw,nCount) ||!CACHEDEF(AvpWriteFile)(bw->WorkArea->V_Handle,buff,nCount,&bytes,NULL))
		bw->WorkArea->RFlags|=RF_IO_ERROR;
ret:
	STACK_ALIGN_OFF;
	return bytes;
}

long    _V_Seek(CBaseWork* bw, long offset,int from)        //seek on tmp file
{
	DWORD r;
	STACK_ALIGN_ON;
	r=CACHEDEF(AvpSetFilePointer)(bw->WorkArea->V_Handle,offset,NULL,from);
	if(r==(DWORD)-1){
		r=0;
	}
	STACK_ALIGN_OFF;
	return r;
}

int _S_Save(CBaseWork* bw, char * buff, long size)   //save solid data
{
	if(NULL == bw->WorkArea->S_Handle)
	{
		char vname[AVP_MAX_PATH];
		AvpGetTempFileName(bdata->TempPath,"AVP",0,vname);
		bw->WorkArea->S_Handle = CACHEDEF(AvpCreateFile)(vname,
			GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
		if(bw->WorkArea->S_Handle==INVALID_HANDLE_VALUE)
		{
			bw->WorkArea->S_Handle=0;
			bw->WorkArea->RFlags|=RF_IO_ERROR;
			return 0;
		}
	}
	CACHEDEF(AvpSetFilePointer)(bw->WorkArea->S_Handle,0,NULL,FILE_END);
	CACHEDEF(AvpWriteFile)(bw->WorkArea->S_Handle, buff, size, (DWORD*)&size, NULL);
	if(size){
		DWORD count;
		CACHEDEF(AvpWriteFile)(bw->WorkArea->S_Handle, &size, sizeof(size), &count, NULL);
	}
	return size;
}

int _S_Load(CBaseWork* bw, char * buff)              //load solid data
{
	if(NULL == bw->WorkArea->S_Handle)
		return 0;

	DWORD filesize=CACHEDEF(AvpGetFileSize)(bw->WorkArea->S_Handle,NULL);
	if(filesize<=4)
		return 0;

	CACHEDEF(AvpSetFilePointer)(bw->WorkArea->S_Handle,filesize-4,NULL,FILE_BEGIN);

	DWORD bread;
	DWORD size=0;
	CACHEDEF(AvpReadFile)(bw->WorkArea->S_Handle, &size, sizeof(size), &bread, NULL);
	if(size)
	{
		if( size > filesize-4)
			return 0; 
		CACHEDEF(AvpSetFilePointer)(bw->WorkArea->S_Handle,filesize-size-4,NULL,FILE_BEGIN);
		CACHEDEF(AvpReadFile)(bw->WorkArea->S_Handle, buff, size, &bread, NULL);
		CACHEDEF(AvpSetFilePointer)(bw->WorkArea->S_Handle,filesize-size-4,NULL,FILE_BEGIN);
		CACHEDEF(AvpSetEndOfFile)(bw->WorkArea->S_Handle);
    }
	return size;
}

BOOL _S_Free(CBaseWork* bw)
{
	return TRUE;
}

int     _CheckExt(CBaseWork* bw)                                                        //access trough File_Name link
{
	
	char* zn=bw->WorkArea->ZipName;
	char* cp=zn+strlen(zn)+2;
	int ct=*(cp-1);
	if(ct>5)ct=0;
	AvpConvertChar(bw->WorkArea->ZipName,ct,cp);

	if(AvpCallback(AVP_CALLBACK_WA_ARCHIVE_NAME,(long)bw->WorkArea)){
		return 0;
	}
	
	char* vn=bw->WorkArea->ScanObject->VirusName;
	bw->WorkArea->ScanObject->VirusName=bw->WorkArea->ZipName;
	if(AvpCallback(AVP_CALLBACK_OBJECT_ARCHIVE_NAME,(long)bw->WorkArea->ScanObject)){
		bw->WorkArea->ScanObject->VirusName=vn;
		return 0;
	}
	bw->WorkArea->ScanObject->VirusName=vn;
	
	char* p;
	while((p=AvpStrrchr(bw->WorkArea->ZipName,'\\')))
 //mdw
		*p='/';

	if(NULL==(p=AvpStrrchr(bw->WorkArea->ZipName,'/')))
		p=bw->WorkArea->ZipName - 1;

    p++;
	
    if( ! bw->CheckMask(p))
		return 0;
	
    int l=strlen(bw->WorkArea->ZipName);
	
	size_t sSz = l + strlen(bw->WorkArea->ScanObject->Name) + 2;
    char* s=new char[sSz];
	if(s){
		strcpy_s(s, sSz, bw->WorkArea->ScanObject->Name);
		strcat_s(s, sSz, "/");
		strcat_s(s, sSz, bw->WorkArea->ZipName);
		if(17==AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)( const char*)s))
		{
			delete s;
			return 0;
		}
		delete s;
	}
	
    return 1;
}

void _Yield()                                  //rotate symbol in bottom line
{
	AvpCallback(AVP_CALLBACK_ROTATE,0);
}


BOOL _NextVolume(CBaseWork* bw)                                                 //access trough File_Name link
{
	BOOL ret;
	STACK_ALIGN_ON;
	ret=bw->NextVolume();
	STACK_ALIGN_OFF;
	return ret;
}


CBaseWork::CBaseWork()
{
	valid=0;
	m_pPageMemoryManager=NULL;
	HT=0;

#ifdef s_DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"CBaseWork constructor enter");
#endif
	BData* bd=(BData*)DupAlloc(sizeof(BData),1,0);
	if(!bd){
		return;
	}
		
	WorkArea=(CWorkArea*)DupAlloc(sizeof(CWorkArea),1,0);
	if(!WorkArea){
		DupFree(bd);
		return;
	}
	DupSetCustomDWord(0,(DWORD)bd);
		
	Cache_Init();

	valid=1;

	memset(bdata,0,sizeof(BData));
	
	// VIK 09.11.2001
	memset(WorkArea,0,sizeof(CWorkArea));
	

	bdata->MainObjectHandle=INVALID_HANDLE_VALUE;

	ExcludeArray=new CPtrArray;
	MaskArray=new CPtrArray;
	BaseArray=new CPtrArray;
	if(!ExcludeArray || !MaskArray || !ExcludeArray) valid=0;
	
#ifdef s_DEBUG
	if(!MaskArray){
		AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"Error with memory allocation MaskArray");
	}
	if(!BaseArray){
		AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"Error with memory allocation BaseArray");
	}
#endif
	
#ifdef s_DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"Setting names to GlobalNames");
#endif

	SetName[0]=0;
	
#ifdef s_DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"CBaseWork constructor exit");
#endif
}

CBaseWork::~CBaseWork()
{
#ifdef s_DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"CBaseWork destructor enter");
#endif

#if !defined (BASE_EMULATOR)
	GlobalNames.FreeAll();
#endif

	for(int i=0;i<Allocations.GetSize();i++){
		delete (void*)Allocations[i];
	}
    Allocations.RemoveAll();

	UnloadBase(NULL,0);
	
#ifdef s_DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"Unloading base done");
#endif

	FreeRedundantHash();
	AddMask(NULL,MaskArray);
	AddMask(NULL,ExcludeArray);
	if(ExcludeArray) delete ExcludeArray;
	if(MaskArray) delete MaskArray;
	if(BaseArray) delete BaseArray;
	Cache_Destroy();

	if(WorkArea){
		DupFree(WorkArea);
	}
	if(bdata){
		DupFree(bdata);
		DupSetCustomDWord(0,0);
	}

#ifdef s_DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"CBaseWork destructor exit");
#endif
}

int ReadString(char* buf,DWORD size, HANDLE f)
{
	DWORD bread;
	*buf=0;
	if( !CACHEDEF(AvpReadFile)(f,buf,size-1,&bread,NULL) || bread>size )return 0;
	buf[bread]=0;
	DWORD i;
	for( i=0;i<bread;i++ ,buf++){
		if(*buf==0x0D || *buf==0x0A || *buf==0){
			*buf=0;
			buf++;i++;
			if(*buf==0x0D || *buf==0x0A )i++;
			CACHEDEF(AvpSetFilePointer)(f,i-bread,NULL,FILE_CURRENT);
			break;
		}
	}
	return i;
}

void PswRegister(char* psw)
{
	AvpCallback(AVP_CALLBACK_REGISTER_PASSWORD,(LPARAM)psw);
}

LONG CBaseWork::LoadBase(_TCHAR* name, int fmt)
 // mdw: no reference to symbol 'fmt'
{
	LONG ret=1;
	HANDLE f;
	bdata->CancelProcess=0;

#ifdef _DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"Debug Engine");
    AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)"LoadBase enter");
#endif

#if defined (ELF_BASES_SUPPORT)
	// Load bases in ELF format
	if ( !gEmul.LoadElfBases() )
	{
		return 0;
	}
#endif /* defined (ELF_BASES_SUPPORT) */
	
	//              Printf("AvpLoadSet %s",name);
	FreeRedundantHash();
	memset(&LPS,0,sizeof(LPS));
	LPS.curName="";
	LPS.setName=SetName;
	if(name!=NULL)strncpy_s(SetName,AVP_MAX_PATH,name,AVP_MAX_PATH-1);
		
	UnloadBase(NULL,0);
	if(name==NULL || *name==0)	return 1;

#if defined (BASE_EMULATOR)
    gEmul.pTable.AddSymbol ("_Header",(DWORD) WorkArea->Header, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Page_C",(DWORD) WorkArea->Page_C, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Page_A",(DWORD) WorkArea->Page_A, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Page_B",(DWORD) WorkArea->Page_B, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Page_E",(DWORD) WorkArea->Page_E, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Full_Name",(DWORD) WorkArea->FullName, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Name",	(DWORD) &WorkArea->Name, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Ext",		(DWORD) &WorkArea->Ext, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Zip_Name",(DWORD) WorkArea->ZipName, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Object_Type",(DWORD) &WorkArea->WAs.SType, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Sub_Type",(DWORD) &WorkArea->SubType,SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Disk",(DWORD) &WorkArea->WAs.Disk,SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Drive",(DWORD) &WorkArea->WAs.Drive, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_RFlags",(DWORD) &WorkArea->RFlags, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_File_Length",(DWORD) &WorkArea->Length, SYM_CPU_X86);
    gEmul.pTable.AddSymbol ("_Area_Owner",(DWORD) &WorkArea->AreaOwner, SYM_CPU_X86);
	gEmul.pTable.AddSymbol ("_P_N",			(DWORD) &WorkArea->P_N,			SYM_CPU_X86);
	gEmul.pTable.AddSymbol ("_AFlags",		(DWORD) &WorkArea->AFlags,		SYM_CPU_X86);
	gEmul.pTable.AddSymbol ("_Disk_Max_CX", (DWORD) &WorkArea->Disk_Max_CX,	SYM_CPU_X86);
	gEmul.pTable.AddSymbol ("_Disk_Max_DH", (DWORD) &WorkArea->Disk_Max_DH,	SYM_CPU_X86);

#else
	GlobalNames.SetName("_Header",(DWORD)WorkArea->Header,0);
	GlobalNames.SetName("_Page_C",(DWORD)WorkArea->Page_C,0);
	GlobalNames.SetName("_Page_A",(DWORD)WorkArea->Page_A,0);
	GlobalNames.SetName("_Page_B",(DWORD)WorkArea->Page_B,0);
	GlobalNames.SetName("_Page_E",(DWORD)WorkArea->Page_E,0);
	GlobalNames.SetName("_Full_Name",(DWORD)WorkArea->FullName,0);
	GlobalNames.SetName("_Name",(DWORD)&WorkArea->Name,0);
	GlobalNames.SetName("_Ext",(DWORD)&WorkArea->Ext,0);
	GlobalNames.SetName("_Zip_Name",(DWORD)WorkArea->ZipName,0);
	
#ifdef __unix__
	GlobalNames.SetName("_Object_Type",(DWORD)&WorkArea->WAs.SType,0);
#else
	GlobalNames.SetName("_Object_Type",(DWORD)&WorkArea->SType,0);
#endif
	GlobalNames.SetName("_Sub_Type",(DWORD)&WorkArea->SubType,0);
#ifdef __unix__
	GlobalNames.SetName("_Disk",(DWORD)&WorkArea->WAs.Disk,0);
#else
	GlobalNames.SetName("_Disk",(DWORD)&WorkArea->Disk,0);
#endif
#ifdef __unix__
	GlobalNames.SetName("_Drive",(DWORD)&WorkArea->WAs.Drive,0);
#else
	GlobalNames.SetName("_Drive",(DWORD)&WorkArea->Drive,0);
#endif
	GlobalNames.SetName("_RFlags",(DWORD)&WorkArea->RFlags,0);
	GlobalNames.SetName("_File_Length",(DWORD)&WorkArea->Length,0);
	
	GlobalNames.SetName("_Area_Owner",(DWORD)&WorkArea->AreaOwner,0);
	GlobalNames.SetName("_Printf",(DWORD)Printf,0);
	
	GlobalNames.SetName("_Check_Sum",(DWORD)CalcSum,0);
	GlobalNames.SetName("__DoJump",(DWORD)_DoJump,0);
	GlobalNames.SetName("__Read",(DWORD)_Read,0);
	GlobalNames.SetName("__Write",(DWORD)_Write,0);
	GlobalNames.SetName("__Seek",(DWORD)_Seek,0);
	GlobalNames.SetName("__Ch_Size",(DWORD)_ChSize,0);
	GlobalNames.SetName("__Read_13",(DWORD)_Read13,0);
	GlobalNames.SetName("__Write_13",(DWORD)_Write13,0);
	GlobalNames.SetName("__Read_25",(DWORD)_Read25,0);
	GlobalNames.SetName("__Write_26",(DWORD)_Write26,0);
	GlobalNames.SetName("__LSeek",(DWORD)_LSeek,0);
	
	GlobalNames.SetName("__Copy_P_File",(DWORD)_Copy_P_File,0);
	GlobalNames.SetName("__P_Read",(DWORD)_P_Read,0);
	GlobalNames.SetName("__P_Seek",(DWORD)_P_Seek,0);
	GlobalNames.SetName("__V_Read",(DWORD)_V_Read,0);
	GlobalNames.SetName("__V_Write",(DWORD)_V_Write,0);
	GlobalNames.SetName("__V_Seek",(DWORD)_V_Seek,0);
	
	GlobalNames.SetName("__S_Save",(DWORD)_S_Save,0);
	GlobalNames.SetName("__S_Load",(DWORD)_S_Load,0);
	GlobalNames.SetName("__CheckExt",(DWORD)_CheckExt,0);
	GlobalNames.SetName("__Rotate",(DWORD)_Rotate,0);
	GlobalNames.SetName("__NextVolume",(DWORD)_NextVolume,0);
	GlobalNames.SetName("_New",(DWORD)_New,0);
	GlobalNames.SetName("_Delete",(DWORD)_Delete,0);
	
	GlobalNames.SetName("_P_N",(DWORD)&WorkArea->P_N,0);
	GlobalNames.SetName("_AFlags",(DWORD)&WorkArea->AFlags,0);
	
	GlobalNames.SetName("_Disk_Max_CX",(DWORD)&WorkArea->Disk_Max_CX,0);
	GlobalNames.SetName("_Disk_Max_DH",(DWORD)&WorkArea->Disk_Max_DH,0);
	GlobalNames.SetName("_Get_Sector_Image",(DWORD)Get_Sector_Image,0);
	
	GlobalNames.SetName("_Peek",(DWORD)Peek,0);
	GlobalNames.SetName("_Poke",(DWORD)Poke,0);

	GlobalNames.SetName("_SavePages",(DWORD)SavePages,0);
#ifdef _DEBUG
	GlobalNames.SetName("_AvpExecSpecial@12",(DWORD)AvpExecSpecial,0);
#endif

#endif /* defined BASE_EMULATOR */

	char* n;
	char* s=new char[AVP_MAX_PATH];
	if(!s) return 1;
	char* sn=new char[AVP_MAX_PATH];
	if(!sn){
		delete s;
		return 1;
	}
	if(m_pPageMemoryManager==NULL)
	{
		m_pPageMemoryManager=new PageMemoryManagerNS::PageMemoryManager(0, BLOCKSIZE);
		if(m_pPageMemoryManager==NULL)
		{
			delete sn;
			delete s;
			return 1;
		}
	}


	AvpGetFullPathName(SetName,AVP_MAX_PATH,s,&n);
	AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
	
	int stop;

again:	
	f=CACHEDEF(AvpCreateFile)(s,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f!=INVALID_HANDLE_VALUE)
		CACHEDEF(AvpCloseHandle)(f);
	else{
		if(AvpCallback(AVP_CALLBACK_SETFILE_OPEN_FAILED_TRY_AGAIN,(DWORD)s))
			goto again;
	}

	
	stop=AvpCallback(AVP_CALLBACK_CHECK_FILE_INTEGRITY,(DWORD)s);
	switch(stop)
    {
		case SIGN_OK:
			break;
		case SIGN_FILE_NOT_FOUND:
		case SIGN_CANT_READ:
			AvpCallback(AVP_CALLBACK_ERROR_FILE_OPEN,(DWORD)(const char*)SetName);
			break;
		default:
			stop=AvpCallback(AVP_CALLBACK_SETFILE_INTEGRITY_FAILED,(DWORD)s);
			break;
        }


	if(stop) f=INVALID_HANDLE_VALUE;
	else     f=CACHEDEF(AvpCreateFile)(SetName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f!=INVALID_HANDLE_VALUE)
	{
		CPtrArray sa;
		while(ReadString(n,AVP_MAX_PATH-(n-s),f)){
			if(*n==';')
			{
				do n++;	while(*n==' ');
#define _STR_VERDICTS "VERDICTS="
				if(!strncmp(n,_STR_VERDICTS,strlen(_STR_VERDICTS)))
	{
					char* end;
					LPS.Verdicts=(BYTE) strtoul(n+strlen(_STR_VERDICTS),&end,16);
				}
				continue;
			}
			if(*n==' ')continue;
			if(*n== 0 )continue;
			size_t ssSz = strlen(s)+1;
			char* ss=new char[ssSz];
			if(ss){
				strcpy_s(ss,ssSz,s);
				sa.Add(ss);
			}
	}
		CACHEDEF(AvpCloseHandle)(f);

		
		LPS.setLength=sa.GetSize()*50000;

		for(int i=0;i<sa.GetSize() && !stop;i++){
			strncpy_s(sn,AVP_MAX_PATH,(const char*)sa[i],AVP_MAX_PATH-1);

#ifdef _DEBUG
			AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)"LoadBase...");
#endif

again1:	

        AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);

			switch(AvpCallback(AVP_CALLBACK_CHECK_FILE_INTEGRITY,(DWORD)sn))
			{
				case SIGN_OK:
					break;
				case SIGN_FILE_NOT_FOUND:
					if(AvpCallback(AVP_CALLBACK_FILE_OPEN_FAILED_TRY_AGAIN,(DWORD)sn))
						goto again1;
				case SIGN_CANT_READ:
					AvpCallback(AVP_CALLBACK_ERROR_FILE_OPEN,(DWORD)sn);
					continue;
				default:
					if(AvpCallback(AVP_CALLBACK_FILE_INTEGRITY_FAILED,(DWORD)sn))
						stop=1;
					continue;
			}

			CBase* ptr=new CBase;

            if(ptr)
			{
				if ( ptr->IsValid() && ptr->LoadBase((_TCHAR*)sa[i], this) )
					BaseArray->Add(ptr);
				else
					delete ptr;
			}
#ifdef _DEBUG
			AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)"LoadBase done.");
#endif
		}
		while(sa.GetSize())
		{
			delete [] (char*)sa.GetAt(0);
			sa.RemoveAt(0);
		}
	}
#ifdef _DEBUG
    AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)"AvpLoadBase Fixing");
#endif
	
#if defined (BASE_EMULATOR)

#if defined (ELF_BASES_SUPPORT)
	if ( !gEmul.LinkElfBases() )
	{
		return 0;
	}
#endif /* defined (ELF_BASES_SUPPORT) */

	const char * unresolved_symbol = gEmul.InitFunctionPointers();
	if ( unresolved_symbol )
    	AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD) unresolved_symbol);

#else

	DWORD ptr;
	
	ptr=GlobalNames.GetName("_DoStdJump");
	_DoStdJump=ptr?ptr:(DWORD)dummy;
	if(ptr==0){
		ret=0;
		AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD)"_DoStdJump");
	}
	ptr=GlobalNames.GetName("_IsProgram");
	_IsProgram=ptr?ptr:(DWORD)dummy;
	if(ptr==0){
		ret=0;
		AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD)"_IsProgram");
	}
	ptr=GlobalNames.GetName("_GetFirstEntry");
	_GetFirstEntry=ptr?ptr:(DWORD)dummy;
	if(ptr==0){
		ret=0;
		AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD)"_GetFirstEntry");
	}
	ptr=GlobalNames.GetName("_GetNextEntry");
	_GetNextEntry=ptr?ptr:(DWORD)dummy;
	if(ptr==0){
		ret=0;
		AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD)"_GetNextEntry");
	}
	ptr=GlobalNames.GetName("_CureStdFile");
	_CureStdFile=ptr?ptr:(DWORD)dummy;
	if(ptr==0){
		ret=0;
		AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD)"_CureStdFile");
	}
	ptr=GlobalNames.GetName("_CureStdSector");
	_CureStdSector=ptr?ptr:(DWORD)dummy;
	if(ptr==0){
		ret=0;
		AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD)"_CureStdSector");
	}
	ptr=GlobalNames.GetName("_GetArchOffs");
	_GetArchOffs=ptr?ptr:(DWORD)dummy;
	if(ptr==0){
		ret=0;
		AvpCallback(AVP_CALLBACK_ERROR_KERNEL_PROC,(DWORD)"_GetArchOffs");
	}

	ptr=GlobalNames.GetName("_PswRegister");
	if(ptr)
		*(DWORD*)ptr=(DWORD)PswRegister;

	_Psw=(CHAR*)GlobalNames.GetName("_Psw");
	
	_gr01=(DWORD*)GlobalNames.GetName("_gr01");

	ptr=GlobalNames.GetName("_CleanUpEntry");
	_CleanUpEntry=ptr?ptr:(DWORD)dummy;
	
#endif /* defined BASE_EMULATOR */

	if(m_pPageMemoryManager)
		m_pPageMemoryManager->setMemoryProtection ( KL_MEMORY_PROTECTION_EXECUTE );
	
#ifdef _DEBUG
    AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)"AvpLoadBase Exit");
#endif
	AvpCallback(AVP_CALLBACK_ROTATE_OFF,(LPARAM)NULL);

	
	delete [] s;
	delete [] sn;
	GlobalNames.Compact();

	return ret;
}

LONG CBaseWork::UnloadBase(_TCHAR* name, int fmt)
 // mdw: no reference to symbol 'name','fmt'
{
#ifdef _DEBUG
    AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)"UnloadBase enter");
#endif
	int i;
	if(BaseArray)
        while(0<=(i=BaseArray->GetUpperBound()))
        {
			AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
            delete (CBase*)(BaseArray->GetAt(i));
            BaseArray->RemoveAt(i);
        }
#if !defined (BASE_EMULATOR)
        _DoStdJump=(DWORD)dummy;
        _IsProgram=(DWORD)dummy;
        _GetFirstEntry=(DWORD)dummy;
        _GetNextEntry=(DWORD)dummy;
        _CureStdFile=(DWORD)dummy;
        _CureStdSector=(DWORD)dummy;
		_CleanUpEntry=(DWORD)dummy;
#endif /* defined BASE_EMULATOR */

		GlobalNames.FreeAll();

		if(m_pPageMemoryManager)
		{
			delete m_pPageMemoryManager;
			m_pPageMemoryManager=NULL;
		}
		
		AvpCallback(AVP_CALLBACK_ROTATE_OFF,(LPARAM)NULL);
        return 0;
}

LONG CBaseWork::SetFlags(LPARAM lParam)
{
	lParam=lParam;
	return 0;
}

DWORD CBaseWork::DoJump(BYTE* Page)
{
	BYTE buf[64];
	DWORD ret;
	AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
	if(BaseArray)
        for(int j=0;j<BaseArray->GetSize();j++){
			CBase* bp =(CBase*)BaseArray->GetAt(j);
			R_Jump* r=(R_Jump*)(bp->RPtrTable[RT_JUMP]);
			for(int i=0;i<bp->RNumTable[RT_JUMP];i++,r++){
				if(r->Len1){
					int k,m;
					for(k=0; k<32 && k<r->Len1; k++){
                        buf[k] = ((ReadDWordPtr(&r->BitMask1)) &(1<<k)) ? Page[k] : 0; //buf[k]=((r->BitMask1)&(1<<k))?Page[k]:0;
					}
					for(m=0; k<r->Len1; k++,m++){
						buf[k] = ((ReadDWordPtr(&r->BitMask2)) & (1<<m)) ? Page[k] : 0; //buf[k]=((r->BitMask2)&(1<<m))?Page[k]:0;
					}
					
					if ( ReadWordPtr(&r->ControlWord) )
					{
						if ( r->Len1 == 1 )
						{
							if ( (BYTE)(ReadWordPtr(&r->ControlWord)) != (BYTE)(ReadWordPtr((WORD *) buf)) ) 
								continue;
						}
						else if ( ReadWordPtr(&r->ControlWord) != ReadWordPtr((WORD *) buf) )
							continue;
					}

					if ( ReadDWordPtr(&r->Sum1) != CalcSum(buf, r->Len1) )
						continue;
				}

				if ( ReadWordPtr(&r->LinkIdx) != (WORD)(-1) )
				{
#if defined (BASE_EMULATOR)
					char tmpbuf[128];
					sprintf (tmpbuf, "JMP%d.%d", j, ReadWordPtr(&r->LinkIdx));
	#if defined (ELF_BASES_SUPPORT)
					ELF_DECODE_FUNC ptr = 0;
					if ( (ptr = gEmul.LookupElfFunction (tmpbuf)) != 0 )
						ret = (*ptr)();
					else
	#endif /* defined (ELF_BASES_SUPPORT) */
						ret = bp->LinkTable[RT_JUMP][ReadWordPtr(&r->LinkIdx)].ExecuteEntry(WorkArea, 0, "JU", tmpbuf);
#else
					ret = bp->LinkTable[RT_JUMP][ReadWordPtr(&r->LinkIdx)].ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */
					
					if ( r->Method == 0 )			// Special method returns EP or 0
					{             
						if (ret) return ret;        // if EP calculated, then return EP
						else continue;              // else (ret=0) continue database processing
					}                               // Standard method, link returns R_CLEAN/R_PREDETECT

                    if ( ret != R_PREDETECT )
                    	continue;  					// if R_CLEAN continue database processing
				}                                   // else call standard method

#if defined (BASE_EMULATOR)
				ret = i386_DoStdJump (WorkArea, Page, r->Method, r->Data1, r->Data2, r->Data3);
#else
				ret=((DWORD (*)(void*,BYTE,WORD,WORD,WORD))_DoStdJump)
					(Page, r->Method, r->Data1, r->Data2, r->Data3);
#endif /* defined BASE_EMULATOR */

				if ( ret ) return ret;
			}
        }
        return 0;
}

LONG CBaseWork::Dispatch(AVPScanObject* ScanObject, BYTE SubType)
{
	memset(WorkArea,0,sizeof(CWorkArea));
	WorkArea->AreaOwner=this;
	WorkArea->ScanObject=ScanObject;
	WorkArea->Type=OT_FILE;
	WorkArea->SubType=(BYTE)-1;
	
	AvpGetFullPathName(ScanObject->Name,AVP_MAX_PATH,WorkArea->FullName,&WorkArea->Name);
	
	if(NULL==(WorkArea->Ext=AvpStrrchr(WorkArea->Name,'.')))
		WorkArea->Ext=WorkArea->Name + strlen(WorkArea->Name);
	
	WorkArea->Handle=CACHEDEF(AvpCreateFile)(ScanObject->Name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if(WorkArea->Handle==INVALID_HANDLE_VALUE)
	{
		ScanObject->RFlags|=RF_NOT_OPEN;
		return 1;
	}
	
	WorkArea->Length=CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL);
	if(WorkArea->Length==(DWORD)-1) WorkArea->Length=0;
	DWORD bytes;
	CACHEDEF(AvpReadFile)(WorkArea->Handle,WorkArea->Header,0x40,&bytes,NULL);
	CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,0,NULL,FILE_BEGIN);

#if defined (BASE_EMULATOR)
	if ( i386_GetFirstEntry(WorkArea) && SubType )
#else
	if ( ((BOOL (*)())_GetFirstEntry)() && SubType )
#endif /* defined BASE_EMULATOR */
	{
		do
		{
			if ( SubType & WorkArea->SubType )
				break;

#if defined (BASE_EMULATOR)
		} while ( i386_GetNextEntry(WorkArea) );
#else
        } while(((BOOL (*)())_GetNextEntry)());
		((void (*)())_CleanUpEntry)();
#endif /* defined BASE_EMULATOR */
	}

	CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
	return 0;
}

DWORD CBaseWork::ErrorFlush()
{
	if(bdata->MainObjectHandle!=INVALID_HANDLE_VALUE)CACHEDEF(AvpCloseHandle)(bdata->MainObjectHandle);
	bdata->MainObjectHandle=INVALID_HANDLE_VALUE;
	return 1;
}

LONG CBaseWork::ProcessObject(AVPScanObject* ScanObject)
{
	DWORD s_filesize=(DWORD)-1;
	BOOL REPORT_DONE=0;
	int  extract_status=0;
	BOOL FalseAlarm=0;
	DWORD skip=0;
	int entry;
	DWORD ArchOffs=(DWORD)-1;
	BData* _bdata=bdata;
	HANDLE S_Handle=WorkArea->S_Handle;
  #if defined (BASE_EMULATOR)
    const symbol_t * symentry;
  #endif
	AvpCallback(AVP_CALLBACK_OBJECT_BEGIN,(DWORD)ScanObject);

	if(ScanObject->InPack == 0 && ScanObject->InArc == 0)
	{
		_bdata->CancelProcess=0;
		_bdata->ArchHeaderSum=0;
		memset(WorkArea,0,sizeof(CWorkArea));
	}
	if(S_Handle)
	{
		WorkArea->S_Handle=S_Handle;
		s_filesize=CACHEDEF(AvpGetFileSize)(S_Handle,NULL);
	}

	
//	ScanObject->RFlags=0; !!!!
	strncpy_s(WorkArea->FullName,AVP_MAX_PATH,ScanObject->Name,AVP_MAX_PATH-1);
	WorkArea->AreaOwner=this;
	WorkArea->ScanObject=ScanObject;
	WorkArea->Type = ScanObject->Type;
	WorkArea->SubType=(BYTE)-1;
	_bdata->DiskFull=0;

	AvpGetTempPath(512,_bdata->TempPath);
#ifndef __unix__
	memcpy(TempDisk,_bdata->TempPath,3);
	TempDisk[3]=0;
#endif
	
#ifdef __unix__
	switch(ScanObject->SO1.SType){
#else
        switch(ScanObject->SType){
#endif
		case OT_MEMORY_FILE:
			ArchOffs=0; //don't call GetArchOffs ! Causes RF_CORRUPTED
			ScanObject->MFlags &= ~(MF_ARCHIVED|MF_PACKED|MF_CA);
        case OT_FILE:
			{
                int cnt=0;
                R_Unpack* entry0packed=0;
                R_Unpack* entrypacked=0;
				CLink* packlink=0;
				BOOL checkedAsBoot=0;

				if(ScanObject->NativeName)
					goto make_name;

                if(ScanObject->Handle == 0)
                {
					AvpGetFullPathName(ScanObject->Name,AVP_MAX_PATH,WorkArea->FullName,&WorkArea->Name);
					ScanObject->Attrib=AvpGetFileAttributes(WorkArea->FullName);
#ifdef S_ISDIR
					if(S_ISDIR(ScanObject->Attrib))
#else
						if(FILE_ATTRIBUTE_DIRECTORY & ScanObject->Attrib ) //if error also
#endif
                        {
							ScanObject->RFlags|=RF_NOT_OPEN;
							ScanObject->RFlags|=RF_NOT_A_PROGRAMM;
							goto ret;
                        }
                }
                else
                {
make_name:
					if(NULL==(WorkArea->Name=AvpStrrchr(WorkArea->FullName,'/')))
                        if(NULL==(WorkArea->Name=AvpStrrchr(WorkArea->FullName,'\\')))
							WorkArea->Name=WorkArea->FullName - 1;
                        WorkArea->Name++;
                }

                if(NULL==(WorkArea->Ext=AvpStrrchr(WorkArea->Name,'.')))
					WorkArea->Ext=WorkArea->Name + strlen(WorkArea->Name);
				
				//                Printf("FileName :%s",WorkArea->Name);
				
again:
                entry=-1;
				//              memset(PackRec,0,3*sizeof(void*));
				//                              ScanObject->RFlags|=WorkArea->RFlags;
				//                  WorkArea->RFlags=0;
                WorkArea->SubType=(BYTE)-1;
				checkedAsBoot=0;
				entry0packed=0;
				BOOL executable=0;
				bool need_cleanup = false;
				
//	06.04.2001 graf (support for total exclude)
				if(!ScanObject->InPack)
					if(WorkArea->ScanObject->MFlags & MF_MASK_EXCLUDE){
						if(CheckWildcards(WorkArea->Name,ExcludeArray)){
							ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
							goto ret;
							
						}
					}
				
				if(ScanObject->Handle != 0)
					WorkArea->Handle=ScanObject->Handle;
                else{
					_bdata->MainObjectHandle=
						WorkArea->Handle=CACHEDEF(AvpCreateFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,
						GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
                }
                if(WorkArea->Handle==INVALID_HANDLE_VALUE)
                {
//					DWORD err=GetLastError();
#ifdef PRINTF_ERRORS
					Printf("Main Open failed: %s",ScanObject->Name);
#endif
					ScanObject->RFlags|=RF_NOT_OPEN;
					goto ret;
                }
				if(ScanObject->MFlags &	MF_DBG_NO_SCAN)
				{
					ScanObject->RFlags|=RF_NOT_SCANNED;
					goto clo;
				}
                WorkArea->Length=CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL);
				switch(WorkArea->Length)
				{
					case 0:
						goto clo;
					case -1:
						WorkArea->Length=0x100000; //probably  pipe - try 1M
						break;
					default:
						break;
				}

                DWORD bytes;
#ifdef CATCH_ZEROS
				BYTE  hdr40[0x40];
				memset(hdr40,0,0x40);
#endif
                CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,0,NULL,FILE_BEGIN);
                if(!CACHEDEF(AvpReadFile)(WorkArea->Handle,WorkArea->Header,0x40,&bytes,NULL))
				{
					ScanObject->RFlags|=RF_NOT_OPEN;
					goto clo;
				}
#ifdef CATCH_ZEROS
				memcpy(hdr40, WorkArea->Header, 0x40);
#endif
				AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)ScanObject);
				if(_bdata->CancelProcess) goto clo;
				
				if(!ScanObject->InPack)
					if((WorkArea->ScanObject->MFlags & 0xF) == MF_MASK_USER){
						if(!CheckWildcards(WorkArea->Name,MaskArray)){
							ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
							goto extract;
						}
					}        
					

                CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,0,NULL,FILE_BEGIN);
				
                if(!ScanObject->InPack)
				{
					WorkArea->RFlags=0;

					if(!ScanObject->InArc)
					{
#ifdef USE_BOMB_HEURISTICS
						_bdata->BombSize=WorkArea->Length;
						_bdata->BombSize*=MaxCompressionRate;
						if(_bdata->BombSize < MinSizeLevel)
							_bdata->BombSize = MinSizeLevel;
#else
						_bdata->BombSize=0;
#endif
					}

#ifdef __unix__
					if(cnt==0 && ((ScanObject->SO2.Mode & 0xF) < 2)){
#else
                        if(cnt==0 && ((ScanObject->Mode & 0xF) < 2)){
#endif


#if defined (BASE_EMULATOR)
							WORD ret = i386_IsProgram (WorkArea, WorkArea->Header, WorkArea->Ext);
#else
							WORD ret=((WORD (*)(void*, void*))_IsProgram) (WorkArea->Header, WorkArea->Ext);
#endif /* defined BASE_EMULATOR */


#ifdef __unix__
							if(ret==0 || ((ScanObject->SO2.Mode & 0xF)==1 && !(ret&1))){
#else
                                if(ret==0 || ((ScanObject->Mode & 0xF)==1 && !(ret&1))){
#endif
									ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
									goto extract;
                                }
							}
						}
						skip=AvpCallback(AVP_CALLBACK_OBJECT_SKIP_REQEST,(LPARAM)ScanObject);
						if(skip & 1){
							ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
							goto extract;
						}
						
						if(17==AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)ScanObject->Name))
						{
							ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
							goto clo;
						}
						
						cnt++;
						
#ifdef PRINTF_ERRORS
						Printf("RFlags: %08X",ScanObject->RFlags);
#endif

#if defined (BASE_EMULATOR)
					if ( !i386_GetFirstEntry(WorkArea) )
						goto clo;

					symentry = gEmul.pTable.SymLookup("_gr01", SYM_CPU_X86);

					if ( symentry && symentry->addr )
						WriteDWordPtr((DWORD *)symentry->addr, 0/* not supported in nonwindows */);
#else
					if(!((BOOL (*)())_GetFirstEntry)())
						goto clo;
					need_cleanup = true;

	#if defined (__unix__)
						if(_gr01)*_gr01=0; /* not supported in nonwindows */
	#else
						if(_gr01)*_gr01=(DWORD)AvpExecSpecial;
	#endif /* __unix__ */
		
#endif /* defined BASE_EMULATOR */

						do{
#ifdef PRINTF_ERRORS
							Printf("RFlags entry %d: %08X",entry,ScanObject->RFlags);
#endif
							ScanObject->RFlags|=WorkArea->RFlags;
							WorkArea->RFlags=0;
							entrypacked=0;

#ifdef CATCH_ZEROS
							if( entry==0 )
							{
								for(int m=0; WorkArea->Header[m]; m++)
								{
									if( m==HEADER_SIZE )
									{
										if( memcmp(hdr40,WorkArea->Header,0x40) )
										{
											AvpCallback(AVP_CALLBACK_OBJECT_ZERO_HEADER,(DWORD)ScanObject);
											CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,0,NULL,FILE_BEGIN);
											CACHEDEF(AvpReadFile)(WorkArea->Handle,hdr40,0x40,&bytes,NULL);
										}
										break;
									}
								}
							}
#endif
							
							entry++;
							AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)ScanObject);
							if(_bdata->CancelProcess) goto clo;
							
							if(ScanObject->MFlags & MF_ALLENTRY)
                                ScanObject->Entry=entry;
							InitCache(WorkArea->Header,_bdata->CacheH);
							InitCache(WorkArea->Page_A,_bdata->CacheA);
							InitCache(WorkArea->Page_B,_bdata->CacheB);
							
							R_File_CureData* curedata = NULL;
							R_File_Cutted* rp=(R_File_Cutted*)DoRecords(RT_FILE, &curedata);

							AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)ScanObject);
							if(_bdata->CancelProcess) goto clo;
							
							if(rp!=NULL)
							{
                                WorkArea->RFlags |= RF_DETECT;

								WorkArea->RFlags &=~RF_CURE;
								ScanObject->RFlags &=~RF_CURE;

								switch(rp->CureMethod){
								case FILE_SPC:
								case FILE_DELETE:
								case FILE_FAIL:
									if(rp->LinkIdx == (WORD)(-1))
										WorkArea->RFlags |= RF_CURE_DELETE;
									break;
								default:
									break;
								}
	                            ScanObject->RFlags|=WorkArea->RFlags;
                                AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)ScanObject);

                                int act=0;
                                switch(ScanObject->MFlags & MF_D_)
                                {
                                case MF_D_DENYACCESS:
									break;
                                case MF_D_CUREALL:
									act=1;
									break;
                                case MF_D_DELETEALL:
									act=2;
									break;
                                default:
									if(_bdata->CancelProcess)
										break;
									act=AvpCallback(AVP_CALLBACK_ASK_CURE,(DWORD)ScanObject);
									break;
                                }
								
                                if(ScanObject->InArc && act){
                                    WorkArea->RFlags|=RF_CURE_FAIL;
                                    if(act==1)
										act=0;
								}
								
                                if(act==1)
                                {
									if(!ScanObject->Handle)
									{
										CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
										if(ScanObject->Attrib & FILE_ATTRIBUTE_READONLY)
										{
											if(AvpSetFileAttributes(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,(ScanObject->Attrib)&~FILE_ATTRIBUTE_READONLY))
												WorkArea->AttrChanged=1;
											else WorkArea->Handle=INVALID_HANDLE_VALUE;
										}
										if(WorkArea->Handle!=INVALID_HANDLE_VALUE)
										{
											_bdata->MainObjectHandle=
												WorkArea->Handle=CACHEDEF(AvpCreateFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,
												GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
										}
										if(WorkArea->Handle==INVALID_HANDLE_VALUE)
										{
											WorkArea->RFlags|=RF_NOT_OPEN;
											WorkArea->RFlags|=RF_CURE_FAIL;

											_bdata->MainObjectHandle=
											WorkArea->Handle=CACHEDEF(AvpCreateFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,
												GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);

											if(WorkArea->Handle==INVALID_HANDLE_VALUE)
											{
												goto noclo;
											}
											goto next_entry;											
										}
										
									}
									switch(CureFile(rp, curedata))
									{
									case 1:
										if(!ScanObject->Handle)
											if(!CACHEDEF(AvpCloseHandle)(WorkArea->Handle)){
												WorkArea->RFlags|=RF_CURE_FAIL;
												break;
											}
										
										WorkArea->RFlags&=~RF_WARNING;
										WorkArea->RFlags&=~RF_SUSPIC;
										ScanObject->RFlags|=WorkArea->RFlags;
										AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)ScanObject);
										
										if(!_bdata->CancelProcess)  goto again;
										break;
									case 2:
										break;
									default:
										WorkArea->RFlags|=RF_CURE_FAIL;
										break;
									}
									goto clo;
                                }
                                if(act==2){
									WorkArea->RFlags|=RF_DELETE;
									goto clo;
                                }
								
                                goto next_entry;
								
							}
							
							
							if(!checkedAsBoot && !(WorkArea->SubType & (ST_OLE2|ST_SCRIPT))) //     CheckAsBoot();
//							if(!checkedAsBoot && *(WORD*)(WorkArea->Header+0x1fe)==0xAA55) // CheckAsBoot();
							{
								checkedAsBoot=1;
								R_Sector* rp=(R_Sector*)DoRecords(RT_SECTOR);
								if(rp!=NULL)
								{
									WorkArea->RFlags |= RF_DETECT;
									WorkArea->RFlags |= RF_CURE_DELETE;

									WorkArea->RFlags &=~RF_CURE;
									ScanObject->RFlags &=~RF_CURE;
									
									ScanObject->RFlags|=WorkArea->RFlags;
									AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)ScanObject);
									int act=0;
									switch(ScanObject->MFlags & MF_D_)
									{
									case MF_D_CUREALL:
                                        act=1;
                                        break;
									case MF_D_DELETEALL:
                                        act=2;
                                        break;
									case MF_D_DENYACCESS:
                                        break;
									default:
										if(_bdata->CancelProcess)
											break;
                                        act=AvpCallback(AVP_CALLBACK_ASK_CURE,(DWORD)ScanObject);
                                        break;
									}
									switch(act)
									{
									case 0:
                                        break;
									case 1:
                                        if(2!=AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)WorkArea->ScanObject)){
											WorkArea->RFlags |= RF_CURE_FAIL;
											break;
										}
									case 2:
										WorkArea->RFlags|=RF_DELETE;
										goto clo;
									}
									goto next_entry;
								}
							}
							
							if(!entry && (ScanObject->MFlags & MF_PACKED))
							{
                                entry0packed=entrypacked=(R_Unpack*)DoRecords(RT_UNPACK);
								if(entry0packed)
									packlink=_bdata->BasePtr->LinkTable[RT_UNPACK] + ReadWordPtr(&entry0packed->LinkIdx);
							}
							
							if(!FalseAlarm){
                                if((FalseAlarm=CheckFalseAlarm())){
									WorkArea->RFlags&=~(RF_SUSPIC|RF_WARNING);
									ScanObject->RFlags&=~(RF_SUSPIC|RF_WARNING);
// Graf & Monastyrsky 09.11.05
//									goto next_entry;
                                }
								else if((ScanObject->MFlags & MF_CA)
									&& !entrypacked
									&& ((ScanObject->MFlags & MF_ALLENTRY)
									||      ! (WorkArea->RFlags & (RF_SUSPIC|RF_WARNING)))
									) 
									Analyze();
							}
							
#if defined (BASE_EMULATOR)
							if((gEmul.pfnGetArchOffs && ArchOffs==(DWORD)-1)
#else
							if((_GetArchOffs && ArchOffs==(DWORD)-1)
#endif /* defined BASE_EMULATOR */
//								&&(
//								(ScanObject->MFlags & MF_ARCHIVED)  //|MF_MAILBASES|MF_MAILPLAIN
//								|| ((ScanObject->MFlags & MF_SFX_ARCHIVED) 
//								    &&	WorkArea->SubType != 0xff && !!(WorkArea->SubType & ST_EXE|ST_COM|ST_NE))
//								)
// Graf - support of prague unpackers

								)
							{
#if defined (BASE_EMULATOR)
								ArchOffs = i386_GetArchOffs(WorkArea);
#else
								ArchOffs=((DWORD (*)())_GetArchOffs)();
#endif /* defined BASE_EMULATOR */

							}
							
next_entry:
							if(ScanObject->MFlags & MF_CALLBACK_SAVE)
							{
								if(!ScanObject->InPack && !ScanObject->InArc)
									AvpCallback(AVP_CALLBACK_SAVE_PAGES,(DWORD)WorkArea);
							}

							ScanObject->RFlags|=WorkArea->RFlags;

							if(entrypacked){
								ScanObject->RFlags &= ~(RF_SUSPIC|RF_WARNING|RF_CORRUPT);
							}
							if(ScanObject->RFlags & RF_CORRUPT)
							{
								if( (ScanObject->RFlags & (RF_SUSPIC|RF_WARNING|RF_DETECT)) /* || (ScanObject->InPack)*/)
									ScanObject->RFlags &= ~RF_CORRUPT;
								else
									AvpCallback(AVP_CALLBACK_WA_CORRUPT,(DWORD)WorkArea);
							}

							WorkArea->RFlags=0;
							if(ScanObject->RFlags & RF_DETECT
							&& !(ScanObject->RFlags & (RF_CURE|RF_DELETED))
							&& !(ScanObject->MFlags & MF_ALLENTRY)){
									goto bailout;
							}
							

						if(_Psw && *_Psw ){
							AvpCallback(AVP_CALLBACK_REGISTER_PASSWORD,(LPARAM)_Psw);
							*_Psw=0;
						}
						

#if defined (BASE_EMULATOR)
					} while ( i386_GetNextEntry(WorkArea) );
#else
                    }while(((BOOL (*)())_GetNextEntry)());
#endif /* defined BASE_EMULATOR */

					
bailout:
					if(_bdata->BombSize)
					{
						if(ScanObject->InArc > MaxLevelOfEmbedding
						|| ScanObject->InPack > MaxLevelOfEmbedding
						|| _bdata->BombSize < 0
						)
						{
							_bdata->BombSize=0;
							_bdata->CancelProcess=1;
							if(!ScanObject->InArc)
							{
								REPORT_DONE=0;
								ScanObject->SuspicionName=BombName+3;
								ScanObject->RFlags|=RF_SUSPIC;
							}
							goto clo;
						}

					}


					if(ScanObject->RFlags & RF_DETECT)
						if(!(ScanObject->RFlags & RF_CURE))
						goto extract;
					
					if(ScanObject->MFlags & MF_PACKED)
					{
						if(entry)
						{
							R_Unpack* rp=(R_Unpack*)DoRecords(RT_UNPACK);
							if( rp && (entry0packed != rp) )
							{
								if (need_cleanup)
								{
									((void (*)())_CleanUpEntry)();
									need_cleanup = false;
								}
								ProcessPacked(ScanObject, rp, _bdata->BasePtr->LinkTable[RT_UNPACK] + ReadWordPtr(&rp->LinkIdx));
								if(ScanObject->RFlags & RF_DETECT)
									goto extract;
							}
						}

						if(entry0packed){
							if(entry)
	#if defined (BASE_EMULATOR)
								i386_GetFirstEntry(WorkArea);
	#else
								((BOOL (*)())_GetFirstEntry)();
	#endif /* defined BASE_EMULATOR */

							if (need_cleanup)
							{
								((void (*)())_CleanUpEntry)();
								need_cleanup = false;
							}
							ProcessPacked(ScanObject, entry0packed, packlink);
						}

					}
					
					
extract:
					if (need_cleanup)
					{
						((void (*)())_CleanUpEntry)();
						need_cleanup = false;
					}

					if(!(skip & 2)){
#ifdef PRINTF_ERRORS
					Printf("RFlags extract: %08X",ScanObject->RFlags);
#endif
						if(!(ScanObject->RFlags & RF_DETECT) 
							||(ScanObject->RFlags & RF_CURE)
							)
						{
							extract_status=ProcessArchive(0);
							
							if(*(WORD*)(WorkArea->Header)==0x4D5A
								|| *(WORD*)(WorkArea->Header)==0x5a4d)
							{
								executable=1;
								if(!extract_status && (ScanObject->MFlags & (MF_ARCHIVED|MF_MAILBASES|MF_MAILPLAIN|MF_SFX_ARCHIVED)))
								{
									WORD b=(*(WORD*)(WorkArea->Header+2) & 0x1FF);
									DWORD a=(*(WORD*)(WorkArea->Header+4)) - (b?1:0);
									a*=0x200;
									a+=b;
									if(CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL) > a && a != ArchOffs)
										extract_status=ProcessArchive(a);
								}
							}
							
							if(!extract_status &&  ArchOffs!=(DWORD)-1 && ArchOffs)
								extract_status=ProcessArchive(ArchOffs);
						}
						
						ScanObject->RFlags|=WorkArea->RFlags;
						
						if(extract_status==1 && !executable)
							REPORT_DONE = 1;
						
						if(ScanObject->RFlags & RF_DETECT) goto clo;
					}

					if(!(skip & 1)){
						if(!extract_status && !ScanObject->InPack && ScanObject->MFlags & MF_REDUNDANT)
							ProcessRedundant(ScanObject);
						
						if(ScanObject->InPack)
							goto clo;
						else if(ScanObject->RFlags & RF_DETECT)
							goto clo;
						else if(ScanObject->RFlags & RF_WARNING)
						{
							if(ScanObject->MFlags & MF_ALLWARNINGS)
								goto clo;
							ScanObject->RFlags |= RF_CURE_DELETE;
							AvpCallback(AVP_CALLBACK_OBJECT_WARNING,(DWORD)ScanObject);
						}
						else if(ScanObject->RFlags & RF_SUSPIC)
						{
							ScanObject->RFlags |= RF_CURE_DELETE;
							AvpCallback(AVP_CALLBACK_OBJECT_SUSP,(DWORD)ScanObject);
						}
					}	

clo:
#ifdef PRINTF_ERRORS
					Printf("RFlags clo: %08X",ScanObject->RFlags);
#endif
					if(ScanObject->Handle != WorkArea->Handle){
						CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
						WorkArea->Handle=_bdata->MainObjectHandle=INVALID_HANDLE_VALUE;
					}
noclo:
					if(!ScanObject->Handle){
						if(WorkArea->AttrChanged){
							if(AvpSetFileAttributes(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,ScanObject->Attrib))
								WorkArea->AttrChanged=0;
						}
					}
					_S_Free(this);

					if (need_cleanup)
					{
						((void (*)())_CleanUpEntry)();
						need_cleanup = false;
			}
			}
		  break;
		  
		  
        case OT_MBR:
#ifdef __unix__
			ScanObject->SO1.Drive=ScanObject->SO1.Disk;
#else
			ScanObject->Drive=ScanObject->Disk;
#endif
			
        case OT_BOOT:
			{
#ifdef __unix__
				if(0==AvpGetDiskParam(ScanObject->SO1.Disk,ScanObject->SO1.Drive,&(WorkArea->Disk_Max_CX),&(WorkArea->Disk_Max_DH)))
#else
				if(0==AvpGetDiskParam(ScanObject->Disk,ScanObject->Drive,&(WorkArea->Disk_Max_CX),&(WorkArea->Disk_Max_DH)))
#endif
				{
					ScanObject->RFlags|=RF_NOT_OPEN;
					ScanObject->RFlags|=RF_NOT_A_PROGRAMM;
                    goto ret;
				}
				
				if(1 & AvpCallback(AVP_CALLBACK_OBJECT_SKIP_REQEST,(LPARAM)ScanObject)){
					ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
					break;
				}
				if(17==AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)ScanObject->Name)){
					ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
					break;
				}
				
				WorkArea->Name=WorkArea->FullName;
				WorkArea->Ext="";
				WorkArea->Length=512;
				
				int curecount=0;
				
again_sector:
				entry=-1;
				WorkArea->RFlags=0;

#if defined (BASE_EMULATOR)
				if (!i386_GetFirstEntry(WorkArea)) break;
#else
				if(!((BOOL (*)())_GetFirstEntry)()) break;
#endif /* defined BASE_EMULATOR */

				//      CheckCRC();
				
				do{
					ScanObject->RFlags|=WorkArea->RFlags;
					WorkArea->RFlags=0;
                    entry++;
                    if(WorkArea->ScanObject->MFlags & MF_ALLENTRY)
						ScanObject->Entry=entry;
                    InitCache(WorkArea->Page_A,_bdata->CacheA);
					
                    R_Sector* rp=(R_Sector*)DoRecords(RT_SECTOR);
                    if(rp!=NULL)
                    {
						WorkArea->RFlags |= RF_DETECT;

						WorkArea->RFlags &=~RF_CURE;
						ScanObject->RFlags &=~RF_CURE;
						
						BYTE Method = SECT_OVERWRITE;
						if(rp->CureObjAndMethod1 & WorkArea->SubType)	Method=rp->CureObjAndMethod1 & 0xF;
						else if(rp->CureObjAndMethod2 & WorkArea->SubType)	Method=rp->CureObjAndMethod2 & 0xF;
						switch(Method){
						case SECT_SPC:
							if(rp->LinkIdx!=(WORD)(-1))
								break;
						case SECT_OVERWRITE:
						case SECT_FAIL:
							WorkArea->RFlags |= RF_CURE_DELETE;
							break;
						default:
							break;
						}
						ScanObject->RFlags|=WorkArea->RFlags;
						AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)ScanObject);
						int act;
						switch(ScanObject->MFlags & MF_D_)
						{
						case MF_D_CUREALL:
							act=1;break;
						case MF_D_DELETEALL:
							act=2;break;
						case MF_D_DENYACCESS:
							act=0;break;
						default:
							act=AvpCallback(AVP_CALLBACK_ASK_CURE,(DWORD)ScanObject);
							break;
						}
						if(act==0){
							;
						}
						else if(act==7){ //was overwritten outside
								WorkArea->RFlags&=~(RF_WARNING|RF_SUSPIC|RF_DELETE);
								WorkArea->RFlags&=~RF_CURE_FAIL;
								WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
								ScanObject->RFlags&=~RF_CURE_FAIL;
								if(_bdata->CancelProcess)goto mess_sect;
								if(curecount++<13)
									goto again_sector;
						}
						else if(act==2){
del:
							WorkArea->RFlags|=RF_DELETE;
							if(CureSector(rp)){
								WorkArea->RFlags&=~(RF_WARNING|RF_SUSPIC|RF_DELETE);
								WorkArea->RFlags&=~RF_CURE_FAIL;
								WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
								ScanObject->RFlags&=~RF_CURE_FAIL;
								AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)ScanObject);
								
								if(_bdata->CancelProcess)goto mess_sect;
								if(curecount++<13)
									goto again_sector;
							}
							WorkArea->RFlags|=RF_CURE_FAIL;
						}
						else{
							if(CureSector(rp)){
								WorkArea->RFlags&=~RF_WARNING;
								WorkArea->RFlags&=~RF_SUSPIC;
								WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
								AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)ScanObject);
								if(_bdata->CancelProcess)goto mess_sect;
								if(curecount++<13)
									goto again_sector;
							}

							if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)ScanObject))
								goto del;
						}
						goto mess_sect;
                    }
					
                    if(CheckFalseAlarm())goto mess_sect;
                    ScanObject->RFlags |=WorkArea->RFlags;
					//                      Warnings
                    if((ScanObject->MFlags & MF_CA)
						&& ((ScanObject->MFlags & MF_ALLENTRY)
						||      ! (WorkArea->ScanObject->RFlags & (RF_SUSPIC|RF_WARNING)))
						)
                    {
						BYTE save=WorkArea->SubType;
						WorkArea->SubType=ST_COM;
						Analyze();
						WorkArea->SubType=save;
                    }
					
					if(!REPORT_DONE && !(ScanObject->RFlags & RF_DETECT)){
						if((ScanObject->RFlags & RF_WARNING) && !(ScanObject->MFlags & MF_ALLWARNINGS))
						{
							ScanObject->RFlags |= RF_CURE_DELETE;
							AvpCallback(AVP_CALLBACK_OBJECT_WARNING,(DWORD)ScanObject);
						}
						if((ScanObject->RFlags & RF_SUSPIC)  && !(ScanObject->RFlags & RF_WARNING)){
							ScanObject->RFlags |= RF_CURE_DELETE;
							AvpCallback(AVP_CALLBACK_OBJECT_SUSP,(DWORD)ScanObject);
						}
					}
					
					
mess_sect: 
					if(ScanObject->MFlags & MF_CALLBACK_SAVE)
						AvpCallback(AVP_CALLBACK_SAVE_PAGES,(DWORD)WorkArea);
					
					if(ScanObject->MFlags & MF_ALLENTRY){
						ScanObject->Entry=entry;
						ScanObject->RFlags |=WorkArea->RFlags;
						ScanObject->RFlags &=~RF_REPORT_DONE;
						REPORT_DONE=1;
						if(ScanObject->RFlags & RF_CURE_FAIL)
							AvpCallback(AVP_CALLBACK_OBJECT_CURE_FAIL,(DWORD)ScanObject);
						AvpCallback(AVP_CALLBACK_OBJECT_DONE,(DWORD)ScanObject);
						if(_bdata->CancelProcess)       break;
					}
					else break;
				
					WorkArea->RFlags=0;

#if defined (BASE_EMULATOR)
				} while ( i386_GetNextEntry(WorkArea) );
#else
                }while(((BOOL (*)())_GetNextEntry)());
#endif /* defined BASE_EMULATOR */
			}
			break;
		  
        case OT_MEMORY:
			{
				if(1 & AvpCallback(AVP_CALLBACK_OBJECT_SKIP_REQEST,(LPARAM)ScanObject)){
					ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
					break;
				}
				if(17==AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)ScanObject->Name)){
					ScanObject->RFlags |= RF_NOT_A_PROGRAMM;
					break;
				}
				
                WorkArea->Name=WorkArea->FullName;
                WorkArea->Ext="";
                WorkArea->Length=0;
                CheckMemory(ScanObject);
			}
			break;
        default:
			ScanObject->RFlags|=WorkArea->RFlags;
			ScanObject->RFlags|=RF_KERNEL_FAULT;
			return 1;
        }
		
ret:
        ScanObject->RFlags|=WorkArea->RFlags;
		
        if(ScanObject->RFlags & RF_DELETE
#ifdef __unix__
			&& ScanObject->SO1.SType==OT_FILE)
#else
			&& ScanObject->SType==OT_FILE)
#endif
		{
			if(ScanObject->InPack)
				REPORT_DONE=1;
			else if(ScanObject->InArc){
				if(!(ScanObject->MFlags & MF_DELETE_ARCHIVES))
					ScanObject->RFlags&=~RF_DELETE;
			}
			else{
				BOOL r=CACHEDEF(AvpDeleteFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name);
				if(!r){
					if(ScanObject->Attrib & FILE_ATTRIBUTE_READONLY)
						AvpSetFileAttributes(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,ScanObject->Attrib&~FILE_ATTRIBUTE_READONLY);
					r=CACHEDEF(AvpDeleteFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name);
				}
				if(r)
				{
					ScanObject->RFlags|=RF_DELETED;
					WorkArea->RFlags&=~RF_CURE_FAIL;
					ScanObject->RFlags&=~RF_CURE_FAIL;
					AvpCallback(AVP_CALLBACK_OBJECT_DELETE,(DWORD)ScanObject);
				}
			}
        }
		
        if(REPORT_DONE)
			ScanObject->RFlags|=RF_REPORT_DONE;
		if(_bdata->DiskFull)ScanObject->RFlags|=RF_DISK_OUT_OF_SPACE;
        if(!ScanObject->InPack){
			if(ScanObject->RFlags & RF_CURE_FAIL && !ScanObject->InArc)
				AvpCallback(AVP_CALLBACK_OBJECT_CURE_FAIL,(DWORD)ScanObject);
		}
		AvpCallback(AVP_CALLBACK_OBJECT_DONE,(DWORD)ScanObject);
		if(!ScanObject->InPack && !ScanObject->InArc)
		{
			if(_bdata->AllocRegistry)
			{
				int i=_bdata->AllocRegistry->GetSize();
				while(i--)
				{
					char* ptr=(char*) _bdata->AllocRegistry->GetAt(i);
#ifdef _DEBUG
					Printf("ERROR: Not released memory (size %d)",*(DWORD*)ptr);
#endif
					delete ptr;
					_bdata->AllocRegistry->RemoveAt(i);
				}
				delete _bdata->AllocRegistry;
				_bdata->AllocRegistry=NULL;
			}
		}
		
		//      }__finally{
		if(S_Handle)
		{
			if(s_filesize != (DWORD)-1)
			{
				CACHEDEF(AvpSetFilePointer)(S_Handle,s_filesize,NULL,FILE_BEGIN);
				CACHEDEF(AvpSetEndOfFile)(S_Handle);
			}
		}
		else
		{
			if(WorkArea->S_Handle)
			{
				CACHEDEF(AvpCloseHandle)(WorkArea->S_Handle);
				WorkArea->S_Handle=NULL;
			}
		}
        return _bdata->CancelProcess;
		//      }
}


int  CBaseWork::CureFile(R_File_Cutted* rp, R_File_CureData* curedata){
	WORD ret;
	
    if ( ReadWordPtr(&rp->LinkIdx) !=0xffff )	//WORD(-1)) !!!!!!!!!!!!!!!!!!!!!!!
	{
		if(bdata->BasePtr->LinkTable[RT_FILE]==NULL)
			ret=R_CURE_FAIL;
		else
		{
#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
			ELF_DECODE_FUNC ptr = 0;
			if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
            	ret = (*ptr)();
			else
	#endif /* defined (ELF_BASES_SUPPORT) */
			ret=bdata->BasePtr->LinkTable[RT_FILE][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1, "CF", (char*) ReadDWordPtr(&rp->NameIdx));
#else
			ret=bdata->BasePtr->LinkTable[RT_FILE][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1);
#endif /* defined BASE_EMULATOR */
		}

		switch(ret){
		case R_CURE:
			WorkArea->RFlags|=RF_CURE;
			return 1;
		case R_CURE_FAIL:
			WorkArea->RFlags|=RF_CURE_FAIL;
			if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)WorkArea->ScanObject))
			{
				WorkArea->RFlags|=RF_DELETE;
				return 2;
			}
			return 0;
		case R_PRECURE:
			break;
		case R_DELETE:
			WorkArea->RFlags|=RF_CURE_DELETE;
			if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)WorkArea->ScanObject))
			{
				WorkArea->RFlags|=RF_DELETE;
				return 2;
			}
			return 0;
		default:
		case R_USER:
			WorkArea->RFlags|=RF_INTERNAL_ERROR;
			if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)WorkArea->ScanObject))
			{
				WorkArea->RFlags|=RF_DELETE;
				return 2;
			}
			return 0;
		}
	}

#if defined (BASE_EMULATOR)
	ret = i386_CureStdFile(WorkArea, rp->CureMethod, curedata->CureData1, curedata->CureData2, curedata->CureData3, curedata->CureData4, curedata->CureData5);
#else
	ret=((WORD (*)(BYTE, WORD,WORD,WORD,WORD,WORD))_CureStdFile)(rp->CureMethod,
		curedata->CureData1, curedata->CureData2, curedata->CureData3, curedata->CureData4, curedata->CureData5);
#endif /* defined BASE_EMULATOR */

	switch(ret){
	case R_CURE:
		WorkArea->RFlags|=RF_CURE;
		return 1;
	case R_DELETE:
		WorkArea->RFlags|=RF_CURE_DELETE;
		if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)WorkArea->ScanObject))
		{
			WorkArea->RFlags|=RF_DELETE;
			return 2;
		}
		return 0;
	default:
	case R_CURE_FAIL:
		WorkArea->RFlags|=RF_CURE_FAIL;
		if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)WorkArea->ScanObject))
		{
			WorkArea->RFlags|=RF_DELETE;
			return 2;
		}
		return 0;
	}
}

int  CBaseWork::CureSector(R_Sector* rp){
	BYTE Method=0;
	WORD ret;

	if ( WorkArea->RFlags & RF_DELETE )
		Method = SECT_OVERWRITE;
	else if ( ReadWordPtr(&rp->LinkIdx) != 0xffff )  //WORD(-1)) !!!!!!!!!!!!!!!!!!!
	{
		if ( bdata->BasePtr->LinkTable[RT_SECTOR]==NULL)
			ret=R_CURE_FAIL;
		else
		{
#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
			ELF_DECODE_FUNC ptr = 0;
			if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
            	ret = (*ptr)();
			else
	#endif /* defined (ELF_BASES_SUPPORT) */
			ret=bdata->BasePtr->LinkTable[RT_SECTOR][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1, "CS", (char*)ReadDWordPtr(&rp->NameIdx));
#else
			ret=bdata->BasePtr->LinkTable[RT_SECTOR][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1);
#endif /* defined BASE_EMULATOR */
		}

		switch(ret){
		case R_CURE:
			WorkArea->RFlags|=RF_CURE;
			return 1;
		case R_CURE_FAIL:
			WorkArea->RFlags|=RF_CURE_FAIL;
			return 0;
		case R_DELETE:
			Method=SECT_OVERWRITE;
			break;
		case R_PRECURE:
			break;
		case R_USER:
		default:
			WorkArea->RFlags|=RF_INTERNAL_ERROR;
			return 0;
		}
	}
	
	ret=R_CURE_FAIL;
	WORD caddr1=0;
	WORD caddr2=0;
	BYTE cdata=0;
	
	if(Method==0){
		if(rp->CureObjAndMethod1 & WorkArea->SubType){
			Method=rp->CureObjAndMethod1 & 0xF;
			caddr1=rp->CureAddr11;
			caddr2=rp->CureAddr12;
			cdata= rp->CureData1;
		}
		else if(rp->CureObjAndMethod2 & WorkArea->SubType){
			Method=rp->CureObjAndMethod2 & 0xF;
			caddr1=rp->CureAddr21;
			caddr2=rp->CureAddr22;
			cdata= rp->CureData2;
		}
		if(Method==SECT_OVERWRITE)
//			if(2!=AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)WorkArea->ScanObject))
				return 0;
	}
		
#if defined (BASE_EMULATOR)
		ret = i386_CureStdSector(WorkArea, Method, caddr1, caddr2, cdata);
#else
		ret=((WORD (*)(BYTE, WORD,WORD,WORD))_CureStdSector)(Method, caddr1, caddr2, cdata);
#endif /* defined BASE_EMULATOR */
	
	switch(ret)
	 {
	  case R_CURE:
		WorkArea->RFlags|=RF_CURE;
		return 1;
	  case R_CURE_FAIL:
	  default:
		WorkArea->RFlags|=RF_CURE_FAIL;
		return 0;
	 } 
 }


CBase::~CBase(){
	//!Sobko
	if (pFileShort)
		delete []pFileShort;

	if (CureData)
	{
		delete[] CureData;
		delete[] CureDataIndex;
	}

	for (int cou = 0; cou<(sizeof(gsub_type)/sizeof(gsub_type[0])); cou++)
	{
		if (gsub_type[cou].pSubQueue)
			delete gsub_type[cou].pSubQueue;
	}
	//! Sobko end

	for(int i=0;i<NUMBER_OF_RECORD_TYPES;i++){
		AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
		if(RPtrTable[i])
			delete RPtrTable[i];
		if(LinkTable[i])
			delete [] LinkTable[i];
	}
	if(p_Names)delete p_Names;
	AvpCallback(AVP_CALLBACK_ROTATE_OFF,(LPARAM)NULL);
}

CBase::CBase(){
	valid=1;

	memset(gsub_type,0,sizeof(gsub_type));
	pFileShort = NULL;

    for(int i=0;i<NUMBER_OF_RECORD_TYPES;i++){
		RNumTable[i]=0;
		RPtrTable[i]=NULL;
		LinkTable[i]=NULL;
    }
    BaseName="";
    p_Names=NULL;
    n_Names=0;
	CureData=NULL;
	CureDataIndex=NULL;
	CureDataSize=0;
}

//#define DEF_SAVE_LINK
#ifdef DEF_SAVE_LINK
static int lcount;
#endif

#define LBXOR_MAKE_SERIAL_DWORD(i) (((BYTE)(i)) | (((BYTE)(i+1))<<8) | (((BYTE)(i+2))<<16) | (((BYTE)(i+3))<<24))
#define LBXOR(data, i) ((DWORD*)(data))[0] ^= LBXOR_MAKE_SERIAL_DWORD(i)
#define LBXOR16(data, n) LBXOR(data+16*n, 16*n); LBXOR(data+4+16*n, 4+16*n); LBXOR(data+8+16*n, 8+16*n); LBXOR(data+12+16*n, 12+16*n);

static void unxor(BYTE* data, DWORD size)
{
	DWORD i, c = size / 256;
	for (i=0; i<c; i++)
	{
		LBXOR16(data, 0);  LBXOR16(data, 1);  LBXOR16(data, 2);  LBXOR16(data, 3);
		LBXOR16(data, 4);  LBXOR16(data, 5);  LBXOR16(data, 6);  LBXOR16(data, 7);
		LBXOR16(data, 8);  LBXOR16(data, 9);  LBXOR16(data, 10); LBXOR16(data, 11);
		LBXOR16(data, 12); LBXOR16(data, 13); LBXOR16(data, 14); LBXOR16(data, 15);
		data += 256;
	}	
	c = size % 256;
	for (i=0; i<c; i++)
		data[i] ^= (BYTE)i;
}

int CBase::LoadBase(_TCHAR* baseName, CBaseWork* bw)
{
	if(baseName!=NULL)BaseName=(char*)baseName;
	int rCount=0;
	UINT i;
	char* p;
	pBW=bw;
	
#ifdef DEF_SAVE_LINK
	lcount =0;
#endif
	
	AVP_BaseHeader          BaseHeader;
	CPtrArray       BlockHeaderArray;
	AVP_BlockHeader*        bhp;

	HANDLE f;
	f=CACHEDEF(AvpCreateFile)(baseName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f==INVALID_HANDLE_VALUE)
	{
		AvpCallback(AVP_CALLBACK_ERROR_FILE_OPEN,(DWORD)baseName);
		return 0;
	}
	
	LPS.curName=(char*)baseName;
	LPS.curPos=0;
	LPS.curLength=CACHEDEF(AvpGetFileSize)(f,NULL);
	
	//BaseHeader read
	DWORD bread;
	
	CACHEDEF(AvpReadFile)(f,&BaseHeader,sizeof(BaseHeader),&bread,0);
	LPS.curPos=bread;
	
	if ( ReadDWordPtr ((DWORD*)BaseHeader.Authenticity)==AVP_MAGIC){
		CACHEDEF(AvpSetFilePointer)(f,0,0,FILE_BEGIN);
		CACHEDEF(AvpReadFile)(f,&BaseHeader.Authenticity,sizeof(BaseHeader)-0x40,&bread,0);
		LPS.curPos=bread;
	}
	
	if ( ReadDWordPtr(&BaseHeader.Magic)!=AVP_MAGIC){
		CACHEDEF(AvpSetFilePointer)(f,0x90,0,FILE_BEGIN);
		CACHEDEF(AvpReadFile)(f,&BaseHeader.Magic,sizeof(BaseHeader)-0x80,&bread,0);
	}

	if ( ReadDWordPtr(&BaseHeader.Magic)!=AVP_MAGIC){
abort_:
		AvpCallback(AVP_CALLBACK_ERROR_BASE,(DWORD)baseName);
		CACHEDEF(AvpSetFilePointer)(f,0,0,FILE_END);
		goto clo;
	}

	if(BaseHeader.BlockHeaderTableSize==0)goto clo;

#ifdef GMTMODIFICATIONTIME
#define ModificationTime GMTModificationTime
#endif

	LPS.NumberOfBases++;
	LPS.CurBaseYear = ReadWordPtr (&BaseHeader.ModificationTime.wYear);
	LPS.CurBaseMonth = (BYTE) ReadWordPtr (&BaseHeader.ModificationTime.wMonth);
	LPS.CurBaseDay = (BYTE) ReadWordPtr (&BaseHeader.ModificationTime.wDay);

	LPS.CurBaseTime = (DWORD) *(BYTE*)(&BaseHeader.ModificationTime.wHour);
	LPS.CurBaseTime <<= 8;
	LPS.CurBaseTime |= (DWORD) *(BYTE*)(&BaseHeader.ModificationTime.wMinute);
	LPS.CurBaseTime <<= 8;
	LPS.CurBaseTime |= (DWORD) *(BYTE*)(&BaseHeader.ModificationTime.wSecond);
	LPS.CurBaseTime <<= 8;
	LPS.CurBaseTime |= (DWORD) (ReadWordPtr (&BaseHeader.ModificationTime.wMilliseconds))/10;

	if(AvpCallback(AVP_CALLBACK_LOAD_BASE,(DWORD)&LPS))
		goto clo;
	AvpCallback(AVP_CALLBACK_LOAD_PROGRESS,(DWORD)&LPS);
	AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
	
	if (( LPS.CurBaseYear > LPS.LastUpdateYear)
		||(( LPS.CurBaseYear == LPS.LastUpdateYear)
			&&(( LPS.CurBaseMonth > LPS.LastUpdateMonth)
				||(( LPS.CurBaseMonth == LPS.LastUpdateMonth)
					&&(( LPS.CurBaseDay > LPS.LastUpdateDay)
					||(( LPS.CurBaseDay == LPS.LastUpdateDay)
						&&( LPS.CurBaseTime > LPS.LastUpdateTime)
						)
					)
				)
			)
		)
	)
	{
		LPS.LastUpdateYear = LPS.CurBaseYear;
		LPS.LastUpdateMonth = LPS.CurBaseMonth;
		LPS.LastUpdateDay = LPS.CurBaseDay;
		LPS.LastUpdateTime = LPS.CurBaseTime;
	}

	
	CACHEDEF(AvpSetFilePointer)(f,ReadDWordPtr(&BaseHeader.BlockHeaderTableFO),0,FILE_BEGIN);
	for ( i = 0; i < ReadDWordPtr(&BaseHeader.BlockHeaderTableSize); i++){
		bhp=new AVP_BlockHeader;
		if(bhp==NULL)goto abort_;

		CACHEDEF(AvpReadFile)(f,bhp,sizeof(AVP_BlockHeader),&bread,0);
		LPS.curPos+=bread;
		if(bread!=sizeof(AVP_BlockHeader)){
			delete bhp;
			goto abort_;
		}

		BlockHeaderArray.Add(bhp);
	}
	
	for( i = 0; i < ReadDWordPtr(&BaseHeader.BlockHeaderTableSize); i++){
		AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
		bhp=(AVP_BlockHeader*)BlockHeaderArray[i];
		CACHEDEF(AvpSetFilePointer)(f,ReadDWordPtr(&bhp->BlockFO),0,FILE_BEGIN);
		LPS.curPos+=ReadDWordPtr(&bhp->CompressedSize);
		
		//                      if(bhp->NumRecords==0)continue;
		
		switch(ReadWordPtr(&bhp->BlockType))
		{
		case BT_NAME:
		case BT_RECORD:
		case BT_LINK32:
			break;
		case BT_LINK16:
		case BT_EDIT:
		case BT_COMMENT:
		default:        continue;
		}
		
		BYTE* buf=new BYTE[ReadDWordPtr(&bhp->CompressedSize)+1];
		
		if(buf==NULL)goto abort_;
		
		CACHEDEF(AvpReadFile)(f,buf,ReadDWordPtr(&bhp->CompressedSize),&bread,0);
		
		if(bread!=ReadDWordPtr(&bhp->CompressedSize)
			|| ReadDWordPtr(&bhp->CRC)!=CalcSum(buf,ReadDWordPtr(&bhp->CompressedSize))
			)
		{
db:
		delete [] buf;
		goto abort_;
		}
		AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
		
		if(ReadWordPtr(&bhp->Compression)){
			//for(UINT i=0;i<ReadDWordPtr(&bhp->CompressedSize);i++) buf[i]^=(BYTE)i;
			unxor(buf, ReadDWordPtr(&bhp->CompressedSize));
			BYTE* b=new BYTE[ReadDWordPtr(&bhp->UncompressedSize)+1];
			if(b==NULL)goto db;
#ifdef _WIN32
			bread=unsqu_y(buf,b,NULL);
#else
			bread=unsqu_y(buf,b,(void*)_Yield);
#endif
			if(bread!=ReadDWordPtr(&bhp->UncompressedSize))
			{
				delete [] b;
				goto db;
			}
			delete [] buf;
			buf=b;
		}
		AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
		
		rCount++;
		
		switch(ReadWordPtr(&bhp->BlockType)){
		case BT_NAME:
			{
				p_Names=(char*)buf;
				n_Names=ReadDWordPtr(&bhp->NumRecords);
				for(int x=ReadDWordPtr(&bhp->UncompressedSize);x;x--)
					p_Names[x]=p_Names[x-1];
				*p_Names='\n';
			}
			break;
		case BT_RECORD:
			RPtrTable[ReadWordPtr(&bhp->RecordType)]=buf;
			RNumTable[ReadWordPtr(&bhp->RecordType)]=ReadDWordPtr(&bhp->NumRecords);
			//                                rCount+=bhp->NumRecords;
			LPS.recCount+=ReadDWordPtr(&bhp->NumRecords);
			LPS.RecCountArray[ReadWordPtr(&bhp->RecordType)]+=ReadDWordPtr(&bhp->NumRecords);
			break;
		case BT_LINK32:
			{
#ifdef DEF_SAVE_LINK
				char* Name=new char[512];
				if(Name==NULL) goto abort_;
				char* p=Name;
				AvpGetFullPathName(baseName,512,Name,&p);
				
				if(NULL==(p=AvpStrrchr(p,'.')))
					p=Name + strlen(Name);
//                    int v=strlen(p);
//                    while(v-- && p[v]!='.');
//                    if(v==-1)p+=strlen(p);
//                    else p+=v;
				sprintf(p,".%03d",lcount);
				HANDLE vf=CACHEDEF(AvpCreateFile)(Name,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
				if(vf!=INVALID_HANDLE_VALUE){
					DWORD bread;
					CACHEDEF(AvpWriteFile)(vf,buf+8, ReadDWordPtr(&bhp->UncompressedSize)-8, &bread, NULL);
					CACHEDEF(AvpCloseHandle)(vf);
				}
				delete [] Name;
#endif
				BYTE* ptr=buf;
				CLink* cl=LinkTable[ReadWordPtr(&bhp->RecordType)]= new CLink[ReadDWordPtr(&bhp->NumRecords)];
				if(cl==NULL) goto abort_;
				for(DWORD i=0;i<ReadDWordPtr(&bhp->NumRecords);i++){
					AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
					DWORD l = ReadDWordPtr((DWORD*)(ptr+4)) + 2*sizeof(DWORD);
					if(!cl[i].Link(ptr+l,bw)){
						AvpCallback(AVP_CALLBACK_ERROR_LINK,(DWORD)&LPS);
					}

#ifdef LINK_RECORD_SAVE				
					if(callbackRecords)
						cl[i].SaveImage(ptr+l,ReadDWordPtr((DWORD*)ptr)-l);
#endif
					ptr += ReadDWordPtr((DWORD*)ptr);
				}
			}
			delete [] buf;
			break;
		default:
			break;
		}
		AvpCallback(AVP_CALLBACK_LOAD_PROGRESS,(DWORD)&LPS);
    }

	p=p_Names+1;
	for(i=0;i<n_Names;i++){
		char* e=p;
		while(*e!='\n')
			e++;
		*e=0;
		if(*p!='#'){
			AvpCallback(AVP_CALLBACK_ROTATE,0);
			LPS.virCount++;
			if(callbackNames && !bdata->CancelProcess
				//                                              &&      bhp->RecordType!=RT_UNPACK
				//                                              &&      bhp->RecordType!=RT_EXTRACT
				)AvpCallback(AVP_CALLBACK_LOAD_RECORD_NAME,(DWORD)p);
		}
		p=e+1;
	}
	for(i=0;i<NUMBER_OF_RECORD_TYPES;i++){
		void* rTable=RPtrTable[i];
		
		for(int j=0;j<RNumTable[i];j++)
		{
			DWORD* pName=NULL;
			WORD* pLink=NULL;
			void* record=NULL;

			switch(i){
			case RT_MEMORY:
				record=((R_Memory*)rTable)+j;
				pName=&((R_Memory*)record)->NameIdx;
				pLink=&((R_Memory*)record)->LinkIdx;
				break;
			case RT_SECTOR:
				record=((R_Sector*)rTable)+j;
				pName=&((R_Sector*)record)->NameIdx;
				pLink=&((R_Sector*)record)->LinkIdx;
				break;
			case RT_FILE:
				record=((R_File*)rTable)+j;
				pName=&((R_File*)record)->NameIdx;
				pLink=&((R_File*)record)->LinkIdx;
				break;
			case RT_CA:
				record=((R_CA*)rTable)+j;
				pName=&((R_CA*)record)->NameIdx;
				pLink=&((R_CA*)record)->LinkIdx;
				break;
			case RT_UNPACK:
				record=((R_Unpack*)rTable)+j;
				pName=&((R_Unpack*)record)->NameIdx;
				pLink=&((R_Unpack*)record)->LinkIdx;
				break;
			case RT_EXTRACT:
				record=((R_Extract*)rTable)+j;
				pName=&((R_Extract*)record)->NameIdx;
				pLink=&((R_Extract*)record)->LinkIdx;
				break;
			case RT_KERNEL:
				record=((R_Kernel*)rTable)+j;
				pLink=&((R_Kernel*)record)->LinkIdx;
				break;
			case RT_JUMP:
				record=((R_Jump*)rTable)+j;
				pLink=&((R_Jump*)record)->LinkIdx;
				break;
			default:        
				continue;
			}

			if(pName)	WriteDWordPtr(pName,ReadDWordPtr(pName) + (DWORD) p_Names + 1);
#ifdef LINK_RECORD_SAVE				
			if(callbackRecords&1){
				bw->WorkArea->CurRecord=record;
				bw->WorkArea->CurRecordType=i;
				bw->WorkArea->CurRecordStatus=1;
				bw->WorkArea->CurRecordLinkObj=0;
				bw->WorkArea->CurRecordLinkObjSize=0;
				if(pLink && (((WORD)-1)!=*pLink)){
					bw->WorkArea->CurRecordLinkObj=LinkArrayTable[i]->GetAt(*pLink)))->GetImage();
					bw->WorkArea->CurRecordLinkObjSize=LinkArrayTable[i]->GetAt(*pLink)))->GetImageSize();
				}
				AvpCallback(AVP_CALLBACK_WA_RECORD,(DWORD)(bw->WorkArea));
				if(pLink && (((WORD)-1)!=*pLink))
					LinkArrayTable[i]->GetAt(*pLink)))->DeleteImage();
			}
#endif// LINK_RECORD_SAVE				
		}
	}
clo:
	LPS.curPos=LPS.curLength;
	AvpCallback(AVP_CALLBACK_LOAD_PROGRESS,(DWORD)&LPS);
	AvpCallback(AVP_CALLBACK_LOAD_BASE_DONE,(DWORD)&LPS);
	
	FreeAllPtr(BlockHeaderArray);
	CACHEDEF(AvpCloseHandle)(f);
	AvpCallback(AVP_CALLBACK_ROTATE_OFF,(LPARAM)NULL);
	
//+ ------------------------------------------------------------------------------------------
	if (!PrepareFileRecords())
	{
		rCount = 0;
	}
//+ ------------------------------------------------------------------------------------------
	
	return rCount;
}
// optimized for speed.
/*
DWORD CacheSum(BYTE* ptr, int off, int Len)
{
switch(off){
case HEADER_BEGIN:      return CacheH[Len];
case PAGE_A_BEGIN:      return CacheA[Len];
case PAGE_B_BEGIN:      return CacheB[Len];
default:                        return CalcSum(ptr+off, Len);
}
}
*/

UINT CBase::GetSubtypeBitNo(BYTE SubType)
{
	switch(SubType)
	{
	case 1<<0: return 0;
	case 1<<1: return 1;
	case 1<<2: return 2;
	case 1<<3: return 3;
	case 1<<4: return 4;
	case 1<<5: return 5;
	case 1<<6: return 6;
	case 1<<7: return 7;
	default:
		return 8;
	}
}

static R_File_CureData g_ZeroCureData = { 0, 0, 0, 0, 0 };

R_File_CureData* CBase::GetCureData(WORD index)
{
	if (CureData)
	{
		WORD* first = CureDataIndex;
		WORD count = CureDataSize;
		while (count > 1)
		{
			WORD new_count = count / 2;
			WORD* mid = first + new_count;
			if (*mid <= index)
				first = mid, count -= new_count;
			else
				count = new_count;
		}
		if (count == 1 && *first == index)
			return &CureData[first-CureDataIndex];
	}
	return &g_ZeroCureData;
}

BOOL CBase::PrepareFileRecords()
{
	R_File* rp0=(R_File*)(RPtrTable[RT_FILE]);
	DWORD SubType = 0;
	
	BOOL bConverted = FALSE;
	
	if (!RNumTable[RT_FILE])
		return TRUE;

	BYTE* FileTypeTable = new BYTE[RNumTable[RT_FILE]];
	if(FileTypeTable == NULL)
		return FALSE;
	
	R_File* rptr = rp0;
	CureDataSize = 0;
	for (int i = 0; i < RNumTable[RT_FILE]; i++, rptr++)
	{
		FileTypeTable[i] = rptr->Type;
		if (rptr->CureData1 != 0 || rptr->CureData2 != 0 || rptr->CureData3 != 0 || rptr->CureData4 != 0 || rptr->CureData5 != 0)
			CureDataSize++;
	}
#ifdef _DEBUG
	// Индекс CureData имеет тип WORD
	if (CureDataSize > 0xFFFF)
		__asm int 3;
#endif

	R_File_Cutted* pCutted = new R_File_Cutted[RNumTable[RT_FILE]];
	R_File_Cutted* pCuttedSave = pCutted;

	if (!pCutted)
	{
		delete[] FileTypeTable;
		return FALSE;
	}

	if (CureDataSize)
	{
		CureData = new R_File_CureData[CureDataSize];
		CureDataIndex = new WORD[CureDataSize];
		if (!CureData || !CureDataIndex)
		{
			delete[] CureData; CureData = NULL;
			delete[] FileTypeTable;
			delete[] pCuttedSave;
			return FALSE;
		}

		R_File* rptr = rp0;
		for (size_t i = 0; i < CureDataSize; rptr++)
		{
			if (rptr->CureData1 == 0 && rptr->CureData2 == 0 && rptr->CureData3 == 0 && rptr->CureData4 == 0 && rptr->CureData5 == 0)
				continue;
			memcpy(&CureData[i], (BYTE*)&rptr->CureData1, sizeof(R_File_CureData));
			CureDataIndex[i] = rptr - rp0;
			++i;
		}
	}

	BYTE* startpos;

	//+ ------------------------------------------------------------------------------------------
	while (TRUE)
	{
		BYTE* t = 0;
		BYTE* t_end = 0;
		BYTE* pSubQueue = 0;

		R_File* trp = rp0;
		BYTE* lt = t_end = FileTypeTable;
		BYTE* lt_end = lt + RNumTable[RT_FILE];

		DWORD total_bytes = 0;
		BYTE nashe = 0;
		BYTE nenashe = 0;
		UINT nSubTypeBitNo = GetSubtypeBitNo((BYTE)SubType);

		if (!bConverted)
		{
			pFileShort = new R_File_Short[RNumTable[RT_FILE]];
			if (!pFileShort)
			{
				delete[] FileTypeTable;
				delete[] pCutted;
				delete[] CureData; CureData = NULL;
				return FALSE;
			}
		}
		
		R_File_Short* tfs = pFileShort;
		
		for(;lt<lt_end;lt++)
		{
			if (!bConverted)
			{
				memcpy(pCutted, ((BYTE*)trp) + sizeof(R_File_Short), sizeof(R_File_Cutted));
				pCutted++;

				if (tfs)
				{
					WORD ControlWord = trp->ControlWord;
					WORD Off1 = trp->Off1;
					BYTE Len1 = trp->Len1;
					tfs->ControlWord = ControlWord;
					tfs->Off1 = Off1;
					tfs->Len1 = Len1;
					trp++;
					tfs++;
				}
			}

			if(*lt & SubType)
			{
				if (nenashe != 0)
				{
					total_bytes += 2;
					nenashe = 0;
					nashe = 0;
				}
				
				nashe++;
				if (nashe == 255)
				{
					total_bytes += 2;
					nashe = 0;
					nenashe = 0;
				}

				t_end = lt+1;
				if (!t)
					t = lt;
			}
			else
			{
				if (t)
				{
					nenashe++;
					if (nenashe == 255)
					{
						total_bytes += 2;
						nashe = 0;
						nenashe = 0;
					}
				}
			}
		}
		
		if (nashe || nenashe)
			total_bytes += 2;
		total_bytes += 2;

		if (!t)
		{
			t = t_end;
			total_bytes = 0;
		}

		startpos = t;

		if (total_bytes)
		{
			pSubQueue = new BYTE[total_bytes];
			gsub_type[nSubTypeBitNo].pSubQueue = pSubQueue;
			if (!pSubQueue)
			{
				delete[] FileTypeTable;
				delete[] pFileShort;
				delete[] pCutted;
				delete[] CureData;
				return FALSE;
			}
			else
			{
				nashe = 0;
				nenashe = 0;

				lt = t;
				lt_end = t_end;
#ifdef _DEBUG
				gsub_type[nSubTypeBitNo].nash_region_size = t_end - t;
#endif
				for(;lt<lt_end;lt++)
				{
					if(*lt & SubType)
					{
						if (nenashe)
						{
							*pSubQueue = nashe; pSubQueue++;
							*pSubQueue = nenashe; pSubQueue++;
							
							nashe = 1;
							nenashe = 0;
						}
						else
						{
							nashe++;
							
							if (nashe == 255)
							{
								*pSubQueue = nashe; pSubQueue++;
								*pSubQueue = nenashe; pSubQueue++;
								
								nashe = 0;
								nenashe = 0;
							}
						}
					}
					else
					{
						nenashe++;
						if (nenashe == 255)
						{
							*pSubQueue = nashe; pSubQueue++;
							*pSubQueue = nenashe; pSubQueue++;
							
							nashe = 0;
							nenashe = 0;
						}
					}
				}

				if (nashe || nenashe)
				{
				*pSubQueue = nashe; pSubQueue++;
				*pSubQueue = nenashe; pSubQueue++;
				}

				*pSubQueue = 0; pSubQueue++;
				*pSubQueue = 0;
			}
		}

		bConverted = true;
	
		gsub_type[nSubTypeBitNo].idx = startpos - FileTypeTable;

		if (SubType)
			SubType = SubType << 1;
		else
			SubType = 1;

		if ((BYTE)SubType == 0)
			break;
	}

	//+ ------------------------------------------------------------------------------------------
	delete[] FileTypeTable;

	delete[] RPtrTable[RT_FILE];
	RPtrTable[RT_FILE] = pCuttedSave;

	return TRUE;
}

#define CacheSum(ptr, off, Len) \
	(off==PAGE_A_BEGIN ? _bdata->CacheA[Len]:\
	(off==HEADER_BEGIN ? _bdata->CacheH[Len]:\
	(off==PAGE_B_BEGIN ? _bdata->CacheB[Len]:\
CalcSum(ptr+off, Len) )))


void* CBaseWork::DoRecords(WORD Type, R_File_CureData** curedata)
{
	BData* _bdata=bdata;
	_bdata->WarLen=0;
		if(BaseArray)
			for(int i=0;i<BaseArray->GetSize();i++)
			{
				_bdata->BasePtr=(CBase*)BaseArray->GetAt(i);
			void* rp=_bdata->BasePtr->DoRecords(Type,WorkArea,curedata);
				if(rp) return rp;
			}
			return NULL;
}


void* CBase::DoRecords(WORD type,CWorkArea* WorkArea, R_File_CureData** curedata){
	DWORD SubType=(DWORD)WorkArea->SubType;
	BData* _bdata=bdata;
	switch(type)
	{
	case RT_FILE:
		{
			DWORD SubType=(DWORD)WorkArea->SubType;
			UINT nSubTypeBitNo = GetSubtypeBitNo((BYTE)SubType);
			BYTE* pSubQueue = gsub_type[nSubTypeBitNo].pSubQueue;

			if(!pSubQueue)
				break;

			R_File_Cutted* rp0 = (R_File_Cutted*)(RPtrTable[type]);

			unsigned int nashe_ = *pSubQueue; pSubQueue++;
			unsigned int nenashe_ = *pSubQueue; pSubQueue++;

			R_File_Cutted* rp=(R_File_Cutted*)(rp0 + (gsub_type[nSubTypeBitNo].idx - 1));
			R_File_Short* rps;
			rps = pFileShort + (gsub_type[nSubTypeBitNo].idx - 1);

			while (true)
			{
				rp++;
				rps++;

				if (!nashe_)
				{
					do
					{
						rp += nenashe_;
						rps += nenashe_;

						nashe_ = *pSubQueue; pSubQueue++;
						nenashe_ = *pSubQueue; pSubQueue++;

						if (!nashe_ && !nenashe_)
						{
#ifdef _DEBUG
							if ((DWORD)(rp - rp0) != gsub_type[nSubTypeBitNo].nash_region_size + gsub_type[nSubTypeBitNo].idx)
							{
								// if break - call Sobko or Mike and show :)
								_asm int 3;
							}
#endif
							break;
						}
						
					} while(!nashe_);

					if (!nashe_)
						break;
				}
				nashe_--;


				if(rps->Len1>2){
					if ( ReadWordPtr(&rps->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rps->Off1))) ) continue;
				}
				else switch(rps->Len1){
				case 1:
					if (((BYTE) ReadWordPtr(&rps->ControlWord)) != *(BYTE*)(WorkArea->Header + ReadWordPtr(&rps->Off1)) ) continue;
					goto cklnk;

				case 2:
					if (ReadWordPtr(&rps->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rps->Off1))) ) continue;
					goto cklnk;

				case 0:
					if ( ReadWordPtr(&rp->LinkIdx) == (WORD)(-1)) continue;
					goto lnk;

// profiler optimized - moved up. //Sobko & Graf
//				default:
//					if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) ) continue;
				}
				
				if ( ReadDWordPtr(&rp->Sum1) != CacheSum(WorkArea->Header, ReadWordPtr(&rps->Off1), rps->Len1)) continue;
cklnk:
				if ( ReadWordPtr(&rp->LinkIdx) != (WORD)(-1)) {
					WORD ret;
lnk:
#ifdef __unix__
						if(WorkArea->ScanObject->SO1.SType == OT_MEMORY_FILE) continue;
#else
						if(WorkArea->ScanObject->SType == OT_MEMORY_FILE) continue;
#endif

#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
						ELF_DECODE_FUNC ptr = 0;
						if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
							ret = (*ptr)();
						else
	#endif /* defined (ELF_BASES_SUPPORT) */
						ret = LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0, "FI", (char*)ReadDWordPtr(&rp->NameIdx));
#else
						ret = LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */

						if(ret==0) continue;
						if(ret==R_DETECT)goto on;
						if(ret!=R_PREDETECT) continue;
				}

				if ( !rp->Len2 )
                	continue;

				if ( ReadDWordPtr(&rp->Sum2) != CalcSum(WorkArea->Header + ReadWordPtr(&rp->Off2), rp->Len2) )
				{
					if ( rps->Len1 < 8 )
						continue;

					if ( SubType == ST_OLE2 )
						continue;

					if ( (ReadWordPtr(&rp->Off2) == (PAGE_C_BEGIN+0x4000) )
					&& (0x00E8 == ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off2))) )
#if defined (BASE_EMULATOR)
					&& !i386_IsProgram (WorkArea, WorkArea->Header, WorkArea->Ext)
#else
					&& !(((WORD (*)(void*, void*))pBW->_IsProgram) (WorkArea->Header, WorkArea->Ext))
#endif /* defined BASE_EMULATOR */
						)continue;
					
					WorkArea->RFlags|=RF_WARNING;
					if ( WorkArea->ScanObject->MFlags & MF_ALLWARNINGS)
					{
						char* wn = WorkArea->ScanObject->WarningName;
						WorkArea->ScanObject->WarningName = (char*)ReadDWordPtr(&rp->NameIdx);
						//WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
						AvpCallback(AVP_CALLBACK_OBJECT_WARNING,(DWORD)WorkArea->ScanObject);
						WorkArea->ScanObject->WarningName=wn;
					}
					if(rps->Len1>_bdata->WarLen){
						_bdata->WarLen=rps->Len1;
						WorkArea->ScanObject->WarningName=(char*) ReadDWordPtr(&rp->NameIdx);
					}
					continue;
				}
				
on:
				WorkArea->ScanObject->VirusName = (char*) ReadDWordPtr(&rp->NameIdx);

				if(callbackRecords&2){
					WorkArea->CurRecord=rp;
					WorkArea->CurRecordType=type;
					WorkArea->CurRecordStatus=2;

					AvpCallback(AVP_CALLBACK_WA_RECORD,(DWORD)(WorkArea));
				}
				
#ifdef _MAKE2BYTES
				if(rp->Len1==1)
					WriteWord (&rp->ControlWord, *(BYTE*)(WorkArea->Header + ReadWordPtr(&rp->Off1)));
				else
					WriteWord (&rp->ControlWord, *(WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1)));
#endif

				if (rp && curedata)
					*curedata = GetCureData(rp - rp0);
				return rp;
			}
		}
		break;
	case RT_SECTOR:
		{
			DWORD sum;
			R_Sector* rp;
			rp=(R_Sector*)(RPtrTable[type]);
			for(int i=0;i<RNumTable[type];i++,rp++){
				BYTE* ptr=WorkArea->Header + ReadWordPtr(&rp->Off1);
				switch(rp->Len1){
				case 1:
					if ( ((BYTE)ReadWordPtr(&rp->ControlWord)) != *(BYTE*)(WorkArea->Header + ReadWordPtr(&rp->Off1)) ) continue;
					goto lnk_sect;
				case 2:
					if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr ((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) ) continue;
					goto lnk_sect;
				case 0:
					if ( ReadWordPtr(&rp->LinkIdx) == (WORD)(-1)) continue;
					goto lnk_sect;
				default:
					if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) ) continue;
					break;
				}
				
				sum = ((ptr==WorkArea->Page_A) && (rp->Len1<CACHE_SIZE))?
					(_bdata->CacheA[rp->Len1]):
				(CalcSum(ptr, rp->Len1));
				
				if ( ReadDWordPtr(&rp->Sum1) != sum ) continue;
				
lnk_sect:
				if ( ReadWordPtr(&rp->LinkIdx) != (WORD)(-1))
				{
					WORD ret;

#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
					ELF_DECODE_FUNC ptr = 0;
					if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
						ret = (*ptr)();
					else
	#endif /* defined (ELF_BASES_SUPPORT) */
						ret=LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0, "CS", (char*)ReadDWordPtr(&rp->NameIdx));
#else
						ret=LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */

					if(ret==0)continue;
					if(ret==R_DETECT)goto on_sector;
					if(ret!=R_PREDETECT)continue;
				}
				if(rp->Len2==0)continue;
				//if((Page=pagePtr(rp->Page_2))==NULL) goto on;
				if ( ReadDWordPtr(&rp->Sum2) != CalcSum(WorkArea->Header + ReadWordPtr(&rp->Off2), rp->Len2)){
					if(rp->Len1>=8){
						WorkArea->RFlags|=RF_WARNING;
						if(WorkArea->ScanObject->MFlags & MF_ALLWARNINGS){
							char* wn=WorkArea->ScanObject->WarningName;
							WorkArea->ScanObject->WarningName=(char*) ReadDWordPtr(&rp->NameIdx);
							//WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
							AvpCallback(AVP_CALLBACK_OBJECT_WARNING,(DWORD)WorkArea->ScanObject);
							WorkArea->ScanObject->WarningName=wn;
						}
						if(rp->Len1>_bdata->WarLen){
							_bdata->WarLen=rp->Len1;
							WorkArea->ScanObject->WarningName=(char*) ReadDWordPtr(&rp->NameIdx);
						}
					}
					continue;
				}
				
on_sector:
				WorkArea->ScanObject->VirusName=(char*) ReadDWordPtr(&rp->NameIdx);

				if(callbackRecords&2){
					WorkArea->CurRecord=rp;
					WorkArea->CurRecordType=type;
					WorkArea->CurRecordStatus=2;
					AvpCallback(AVP_CALLBACK_WA_RECORD,(DWORD)(WorkArea));
				}
#ifdef _MAKE2BYTES
				if(rp->Len1==1)
					WriteWord (&rp->ControlWord, *(BYTE*)(WorkArea->Header + ReadWordPtr(&rp->Off1)));
				else
					WriteWord (&rp->ControlWord, *(WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1)));
#endif
				return rp;
			}
		}
		break;
		
	case RT_UNPACK:
	case RT_EXTRACT:
		{
			R_Unpack* rp=(R_Unpack*)(RPtrTable[type]);
			int i=RNumTable[type];
			for(;i;i--,rp++)
			{
				if(!(rp->Type & SubType))continue;
				if(type==RT_EXTRACT){
					DWORD NameLen = strlen((const char *)ReadDWordPtr(&rp->NameIdx));
					DWORD TypeName = (NameLen + 1 >= 4 ? ReadDWordPtr((DWORD*)(ReadDWordPtr(&rp->NameIdx))) : 0xFFFFFFFF);
					switch (TypeName)
					{
//					case 0x65626d85:	//	'ebmE'  // F^*^!!!!!!!!!!!  
					case 0x65626d45:	//	'ebmE'
						if(WorkArea->ScanObject->MFlags & MF_SKIP_EMBEDDED)
							continue;
						break;

					case 0x6c69614d:	//  'liaM'
						if(*((char*)(ReadDWordPtr(&rp->NameIdx))+4)==0){
							if( !(WorkArea->ScanObject->MFlags & MF_MAILPLAIN))
								continue;
						}else{
							if( !(WorkArea->ScanObject->MFlags & MF_MAILBASES))
								continue;
						}
						break;

					case 0x4175746f:    // 'otuA'
						break;

					default:
						if( WorkArea->ScanObject->MFlags & MF_ARCHIVED)
							break;

						if( WorkArea->ScanObject->MFlags & MF_SFX_ARCHIVED ){
							if( 'kcaP'== ReadDWordPtr((DWORD*)WorkArea->Header) )
								break;
							if(SubType & (ST_EXE|ST_NE)){
								if(SubType != 0xff)
									break;

							}
							continue;
						}
						continue;
					}
				}
				switch(rp->Len1){
				case 1:
					if ( ((BYTE)ReadWordPtr(&rp->ControlWord)) != *(BYTE*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) continue;
					else goto sum2;
				case 2:
					if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) ) continue;
				case 0: goto sum2;
				default:
					if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) ) continue;
				}
				
				if ( ReadDWordPtr(&rp->Sum1) != CalcSum(WorkArea->Header + ReadWordPtr(&rp->Off1), rp->Len1)) continue;
sum2:
				//              _cprintf("check Sum2 len2=%d.\n",rp->Len2);
				if(rp->Len2)
					if ( ReadDWordPtr(&rp->Sum2) != CalcSum(WorkArea->Header + ReadWordPtr(&rp->Off2), rp->Len2))continue;
					
					//              _cprintf("rp->LinkIdx=%08X %s\n",rp->LinkIdx,(char*)(rp->NameIdx));
					if ( ReadWordPtr(&rp->LinkIdx) != (WORD)(-1))
					{
						WORD ret;
							WorkArea->P_N=0;
							HANDLE pHandle=WorkArea->P_Handle;
							WorkArea->P_Handle=WorkArea->Handle;
							WorkArea->RFlags&=~RF_ENCRYPTED;

#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
							ELF_DECODE_FUNC ptr = 0;
							if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
				            	ret = (*ptr)();
							else
	#endif /* defined (ELF_BASES_SUPPORT) */
								ret = LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0, "UN", (char*)ReadDWordPtr(&rp->NameIdx));
#else
								ret = LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */

							WorkArea->P_Handle=pHandle;

							if(callbackRecords&2){
								WorkArea->CurRecord=rp;
								WorkArea->CurRecordType=type;
								WorkArea->CurRecordStatus=2;
								AvpCallback(AVP_CALLBACK_WA_RECORD,(DWORD)(WorkArea));
							}

							if(ret==P_OK) 
								return rp;
					}
			}
		}
		return NULL;
		
	case RT_CA:
		{
			//                      R_CA* Frp=NULL;
			R_CA* rp=(R_CA*)(RPtrTable[type]);
			int i=RNumTable[type];
			for(;i;i--,rp++)
			{
				if(rp->Len2)continue;
				
				if(WorkArea->Type & OT_SECTOR){
					if(rp->Type != ST_COM)continue;
				}
				else{
					if(!(rp->Type & SubType))continue;
				}
				
				if ( ReadWordPtr(&rp->LinkIdx) != (WORD)(-1))
				{
					WORD ret;
#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
						ELF_DECODE_FUNC ptr = 0;
						if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
							ret = (*ptr)();
						else
	#endif /* defined (ELF_BASES_SUPPORT) */
						ret=LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0, "CA", (char*) ReadDWordPtr(&rp->NameIdx));
#else
						ret=LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */

						if(ret==R_DETECT)goto on_ca;
				}
				
				if ( ReadWordPtr(&rp->ControlWord) )
					switch(rp->Len1)
					{
					case 1:
						if(((BYTE)ReadWordPtr(&rp->ControlWord)) != *(BYTE*)(WorkArea->Header + ReadWordPtr(&rp->Off1)))
							continue;
						goto s2_ca;
					case 2:
						if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) )
							continue;
						goto s2_ca;
					case 0:
						//                                      if(rp->Len2) goto s2_ca;
						continue;
					default:
						if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) )
							continue;
					}

				if ( ReadDWordPtr(&rp->Sum1) != CacheSum(WorkArea->Header, ReadWordPtr(&rp->Off1), rp->Len1))
					continue;
				
				
s2_ca:
				//                              if(rp->Len2)  //this is False Alarm record
				//                                      if(rp->Sum2 == CalcSum(WorkArea->Header + rp->Off2, rp->Len2))
				//                                              return rp;
				//                                      else continue;
on_ca:
				//                              if(Frp==NULL)Frp=rp;
				if(callbackRecords&2){
					WorkArea->CurRecord=rp;
					WorkArea->CurRecordType=type;
					WorkArea->CurRecordStatus=2;
					AvpCallback(AVP_CALLBACK_WA_RECORD,(DWORD)(WorkArea));
				}
				return rp;
			}
			//                      return Frp;
			return NULL;
		}
		break;
 // mdw: unreachable code 
		
	case RT_MEMORY:
		{
			if(_bdata->DosMemory==NULL)break;
			BYTE Method;
			WORD Start_Segm;
			WORD End_Segm;
			WORD Mem_Segm;
			WORD Mem_Offs;
			BYTE ch;
			DWORD Sum;
			WORD Line;
			BYTE Len;
			int i;
			R_Memory* rp;
			
			DWORD offset;
			BYTE buf[256];
			DWORD cur_offset;
			BYTE cur_buf[5];
			
			rp=(R_Memory*)(RPtrTable[type]);
			for(i=RNumTable[type];i;i--,rp++)
			{
				//        AvpCallback(AVP_CALLBACK_ROTATE,NULL);
				
				Method=rp->Method;
				ch=rp->ControlByte;
				Sum = ReadDWordPtr(&rp->Sum1);
				Len=rp->Len1;
Mcb_To_Cut:
				Start_Segm=0;
				End_Segm=_bdata->BIOS_Mem;
				
				switch (Method){
					
				case MEM_W95_ADDR:
					offset=rp->Segment;
					offset<<=16;
					cur_offset=offset+rp->CureOffset;
					offset+=rp->Offset;
					if(!MemoryRead(offset,Len,buf))continue;
					if(!MemoryRead(cur_offset,rp->CureLength,cur_buf))continue;
					if(Len!=0 && buf[0]==ch  && (CalcSum(buf,Len)==Sum) && memcmp(cur_buf,rp->CureData,rp->CureLength))
					{
						WorkArea->ScanObject->VirusName = (char*) ReadDWordPtr(&rp->NameIdx);
						WorkArea->RFlags|=RF_DETECT;
						WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
						AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)WorkArea->ScanObject);
						DWORD cure=AvpCallback(AVP_CALLBACK_ASK_CURE_MEMORY,(DWORD)WorkArea->ScanObject);
						if(cure<2)
						{
							AvpMemoryWrite(offset,rp->CureLength,rp->CureData);
							WorkArea->RFlags|=RF_CURE;
							WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
							AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)WorkArea->ScanObject);
						}
					}
					continue;
				case MEM_W95_IFS_API:
					offset=rp->Segment;
					offset<<=16;
					offset+=rp->Offset;
					for(Line=0;Line<_bdata->Num_Hook_Lines;Line++){
						if(!MemoryRead(offset+_bdata->Hook_Lines[Line],Len,buf))continue;
						if(!MemoryRead(rp->CureOffset+_bdata->Hook_Lines[Line],rp->CureLength,cur_buf))continue;
						if(Len!=0 && buf[0]==ch  && (CalcSum(buf,Len)==Sum) && memcmp(cur_buf,rp->CureData,rp->CureLength))
						{
							WorkArea->ScanObject->VirusName = (char*) ReadDWordPtr(&rp->NameIdx);
							WorkArea->RFlags|=RF_DETECT;
							WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
							AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)WorkArea->ScanObject);
							DWORD cure=AvpCallback(AVP_CALLBACK_ASK_CURE_MEMORY,(DWORD)WorkArea->ScanObject);
							if(cure<2)
							{
#ifdef _DEBUG
								{
									MemoryRead(rp->CureOffset+_bdata->Hook_Lines[Line],Len,buf);
									Printf("Memory before cure %08X: %02X %02X %02X %02X",rp->CureOffset+_bdata->Hook_Lines[Line],buf[0],buf[1],buf[2],buf[3]);
								}
#endif
								AvpMemoryWrite(rp->CureOffset+_bdata->Hook_Lines[Line],rp->CureLength,rp->CureData);
#ifdef _DEBUG
								{
									MemoryRead(rp->CureOffset+_bdata->Hook_Lines[Line],Len,buf);
									Printf("Memory after cure  %08X: %02X %02X %02X %02X",rp->CureOffset+_bdata->Hook_Lines[Line],buf[0],buf[1],buf[2],buf[3]);
								}
#endif
								WorkArea->RFlags|=RF_CURE;
								WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
								AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)WorkArea->ScanObject);
							}
						}
					}
					continue;
					
				case MEM_W32_API:
					continue;
					
				case MEM_MCB:
					Start_Segm=_bdata->MCB_Lines[0];
					break;
				case MEM_CUT:
					Start_Segm=_bdata->Z_MCB;
					if(_bdata->Z_MCB>_bdata->DOS_Mem) Start_Segm=_bdata->DOS_Mem;
					if(Start_Segm>_bdata->BIOS_Mem) Start_Segm=_bdata->BIOS_Mem;
					if(Start_Segm>=0xA000) continue;
					if(Start_Segm<0x9800) Start_Segm=0x9800;
					break;
				case MEM_ADDRESS:
					Start_Segm=End_Segm=rp->Segment;
					break;
				case MEM_SCAN:
					Start_Segm=_bdata->MCB_Lines[0];
					End_Segm=0x1000;
					break;
				case MEM_FULL_SCAN:
					Start_Segm=0; End_Segm=_bdata->BIOS_Mem;
					break;
				case MEM_TRACE:
					Start_Segm=(WORD)(_bdata->Trace_Lines[0]>>16);
					break;
				case MEM_SPC:
					{
#if defined (BASE_EMULATOR)
						WORD ret;
	#if defined (ELF_BASES_SUPPORT)
						ELF_DECODE_FUNC ptr = 0;
						if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
							ret = (*ptr)();
						else
	#endif /* defined (ELF_BASES_SUPPORT) */
							ret=LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1, "CM", (char*)ReadDWordPtr(&rp->NameIdx));
#else
						WORD ret=LinkTable[type][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1);
#endif /* defined BASE_EMULATOR */

						if(ret!=R_CLEAN)
						{
							WorkArea->ScanObject->VirusName = (char*) ReadDWordPtr(&rp->NameIdx);
							WorkArea->RFlags|=RF_DETECT;
							WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
							AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)WorkArea->ScanObject);

							AvpCallback(AVP_CALLBACK_ASK_CURE_MEMORY,(DWORD)WorkArea->ScanObject);

							WorkArea->RFlags|=RF_CURE;
							WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
							AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)WorkArea->ScanObject);
						}
					}
					continue;
					
				default:
					continue;
				}
				
				Mem_Segm=Start_Segm;
				Mem_Offs=rp->Offset;
				Line=0;
				
				if (Method==MEM_TRACE)
					Mem_Offs+=(WORD)_bdata->Trace_Lines[0];
				
Loop:
				if (ch==(BYTE)DosPeek(Mem_Segm,Mem_Offs) && Len!=0 )
				{
					WORD off=rp->CureOffset;
					if(Method==MEM_TRACE)
						off+=(WORD)_bdata->Trace_Lines[Line];

					if(CalcSum(_bdata->DosMemory+(((DWORD)Mem_Segm)<<4)+Mem_Offs,Len)==Sum && memcmp(_bdata->DosMemory+(((DWORD)Mem_Segm)<<4)+off,rp->CureData,rp->CureLength))
					{
						WorkArea->ScanObject->VirusName=(char*)ReadDWordPtr(&rp->NameIdx);
						WorkArea->RFlags|=RF_DETECT;
						WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
						AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)WorkArea->ScanObject);
						DWORD cure=AvpCallback(AVP_CALLBACK_ASK_CURE_MEMORY,(DWORD)WorkArea->ScanObject);
						if(cure<2)
						{
							for (WORD k=0;k<rp->CureLength;k++)
								Poke(Mem_Segm, k+off, rp->CureData[k],1);
							
							WorkArea->RFlags|=RF_CURE;
							WorkArea->ScanObject->RFlags|=WorkArea->RFlags;
							AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)WorkArea->ScanObject);
						}
					}
				}
				switch (Method){
				case MEM_CUT:
				case MEM_SCAN:
				case MEM_FULL_SCAN:
					Mem_Segm++;
					if (Mem_Segm<=End_Segm)
						goto Loop;
					break;
					
				case MEM_MCB:
					Line++;
					if (Line<_bdata->Num_MCB_Lines){
						Mem_Segm=_bdata->MCB_Lines[Line];
						goto Loop;
					}
					Method=MEM_CUT;
					goto Mcb_To_Cut;
					
				case MEM_TRACE:
					Line++;
					if (Line<_bdata->Num_Trace_Lines){
						Mem_Segm=(WORD)(_bdata->Trace_Lines[Line]>>16);
						Mem_Offs=rp->Offset+(WORD)_bdata->Trace_Lines[Line];
						goto Loop;
					}
					break;
					
				default:
					break;
				}
            }
        }
        break;
				
		// RT_ END!!!
    default:
		WorkArea->RFlags|=RF_INTERNAL_ERROR;
		break;
	}
	return NULL;
}

int CBaseWork::CheckMask(_TCHAR* name)
{
    if(WorkArea->ScanObject->MFlags & MF_MASK_EXCLUDE){
		if(CheckWildcards(name,ExcludeArray))
			return 0;
	}
	if((WorkArea->ScanObject->MFlags & 0xF) == MF_MASK_USER){
		return CheckWildcards(name,MaskArray);
	}        
	return 1;
}

int CBaseWork::AddMask(_TCHAR* ch,CPtrArray* array)
{
	if(array==0)return 0;
	if(ch==NULL || *ch==0)
	{
		while(array->GetSize())
		{
			delete (char*)array->GetAt(0);
			array->RemoveAt(0);
		}
	}
	else{
		DWORD l=strlen(ch)+1;
		char* c=new char[l];
		if(c){
			strcpy_s(c,l,ch);
			char* p=c;
			for(DWORD i=0;i<(l-2);i++,p++){
				if(*p=='.' && *(p+1)=='*')
					switch( *(p+2)){
					case 0:
					case '\\':
					case '/':
						*p='*';
					default:
						continue;
					}
			}
			array->Add(AvpStrlwr(c));
		}
	}
	
	return array->GetSize();
}

BOOL CBaseWork::Analyze()
{
	R_CA* Frp=(R_CA*)DoRecords(RT_CA);
	if(Frp){
		WorkArea->ScanObject->SuspicionName=(char*)ReadDWordPtr(&Frp->NameIdx);
		WorkArea->RFlags|=RF_SUSPIC;
		AvpCallback(AVP_CALLBACK_WA_SUSP,(DWORD)WorkArea);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseWork::CheckFalseAlarm()
{
	if(!BaseArray) return FALSE;
	BData* _bdata=bdata;
	
	for(int k=0;k<BaseArray->GetSize();k++)
	{
		_bdata->BasePtr=(CBase*)BaseArray->GetAt(k);
		
		R_CA* rp=(R_CA*)(_bdata->BasePtr->RPtrTable[RT_CA]);
		int i=_bdata->BasePtr->RNumTable[RT_CA];
		for(;i;i--,rp++)
		{
			if(rp->Len2==0) continue;
			if(WorkArea->Type & OT_SECTOR){
				if(rp->Type != ST_COM)continue;
			}
			else{
				if(!(rp->Type & WorkArea->SubType))continue;
			}
			
			if(ReadWordPtr(&rp->ControlWord))
			{
				switch(rp->Len1){
				case 1:
					if ( ((BYTE)ReadWordPtr(&rp->ControlWord)) != *(BYTE*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) continue;
					goto cklnk;
				case 2:
					if (ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) ) continue;
					goto cklnk;
				case 0:
					if ( ReadWordPtr(&rp->LinkIdx) == (WORD)(-1)) continue;
					goto lnk;
				default:
					if ( ReadWordPtr(&rp->ControlWord) != ReadWordPtr((WORD*)(WorkArea->Header + ReadWordPtr(&rp->Off1))) ) continue;
				}
			}
			
			if ( ReadDWordPtr(&rp->Sum1) != CacheSum(WorkArea->Header, ReadWordPtr(&rp->Off1), rp->Len1))continue;
cklnk:
			if ( ReadWordPtr(&rp->LinkIdx) != (WORD)(-1)){
				WORD ret;
lnk:
#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
					ELF_DECODE_FUNC ptr = 0;
					if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
						ret = (*ptr)();
					else
	#endif /* defined (ELF_BASES_SUPPORT) */
						ret=_bdata->BasePtr->LinkTable[RT_CA][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0, "CA", (char*)ReadDWordPtr(&rp->NameIdx));
#else
					ret=_bdata->BasePtr->LinkTable[RT_CA][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */

					if(ret==R_DETECT)goto on;
					if(ret!=R_PREDETECT)continue;
				}
			//                              if(!rp->Len2)continue;
			if ( ReadDWordPtr(&rp->Sum2) != CalcSum(WorkArea->Header + ReadWordPtr(&rp->Off2), rp->Len2))
				continue;
on:
			AvpCallback(AVP_CALLBACK_OBJECT_FALSE_ALARM,(DWORD)(WorkArea->ScanObject));
			WorkArea->RFlags&=~(RF_WARNING|RF_SUSPIC);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL WINAPI X_AvpMoveFile(
						 HANDLE hi,
						  LPCTSTR lpNewFileName       // address of new name for the file
						  )
{
	HANDLE ho;
	BOOL r=FALSE;
	
#define BUF_SIZ 0x1000
	char* buf=new char[BUF_SIZ];
	if(buf==NULL)return FALSE;
	
	ho=CACHEDEF(AvpCreateFile)(lpNewFileName,
		GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
	if(ho==INVALID_HANDLE_VALUE) goto c2;
	
	CACHEDEF(AvpSetFilePointer)(hi,0,NULL,FILE_BEGIN);

	do{
		DWORD i;
		r=CACHEDEF(AvpReadFile)(hi,buf,BUF_SIZ,&i,0);
		if(r)
			r=CACHEDEF(AvpWriteFile)(ho,buf,i,&i,0);
		if(i==0)break;
	}while(r);
	
	CACHEDEF(AvpCloseHandle)(ho);
c2:     delete buf;
		
    return r;
}

BOOL CBaseWork::ProcessPacked(AVPScanObject* ScanObject, R_Unpack* rp, CLink* link)
{
	BOOL REPORT_DONE=FALSE;
	WORD ret;
	BData* _bdata=bdata;

#if defined (ELF_BASES_SUPPORT)
	ELF_DECODE_FUNC ptr = 0;
#endif /* defined (ELF_BASES_SUPPORT) */

	if(_bdata->CancelProcess)
        return REPORT_DONE;
	
	if(!strcmp("Embedded CAB",(char*)ReadDWordPtr(&rp->NameIdx)))
	{
		if(!(ScanObject->MFlags & MF_ARCHIVED|MF_SFX_ARCHIVED))
	        return REPORT_DONE;
	}

	CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,0,NULL,FILE_BEGIN);
	
	ScanObject->RFlags &=~RF_SUSPIC;

	char* vn=ScanObject->VirusName;
	ScanObject->VirusName=(char*)ReadDWordPtr(&rp->NameIdx);
	ScanObject->RFlags|=WorkArea->RFlags;
	AvpCallback(AVP_CALLBACK_OBJECT_PACKED,(DWORD)ScanObject);
	ScanObject->RFlags&=~RF_ENCRYPTED;
	ScanObject->VirusName=vn;
	
	char* vname=new char[AVP_MAX_PATH];
	char* fname=new char[AVP_MAX_PATH];
	if(!vname || !fname){
		if(fname) delete fname;
		if(vname) delete vname;
		return REPORT_DONE;
	}
	vname[0]=0;
	fname[0]=0;
	HANDLE vHandle=0;
	HANDLE sHandle=WorkArea->Handle;
	HANDLE pHandle=WorkArea->P_Handle;
	
	WorkArea->P_Handle=WorkArea->Handle;
	char* prefix="AVP";
	
	AvpGetTempFileName(_bdata->TempPath,prefix,0,fname);
	
	if(WorkArea->V_Handle){
		CACHEDEF(AvpSetFilePointer)(WorkArea->V_Handle,0,NULL,0);
		CACHEDEF(AvpSetEndOfFile)(WorkArea->V_Handle);
	}else{
		AvpGetTempFileName(_bdata->TempPath,prefix,0,vname);
		WorkArea->V_Handle = vHandle = CACHEDEF(AvpCreateFile)(vname,
			GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
		if(WorkArea->V_Handle==INVALID_HANDLE_VALUE)
		{
			WorkArea->V_Handle=0;
			WorkArea->RFlags|=RF_IO_ERROR;
			goto noclo;
		}
	}
	
	WorkArea->Handle=CACHEDEF(AvpCreateFile)(fname,
		GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
	if(WorkArea->Handle==INVALID_HANDLE_VALUE)
	{
		WorkArea->RFlags|=RF_IO_ERROR;
		goto noclo;
	}
	_bdata->BPCount=0;

	AvpCallback(AVP_CALLBACK_WA_PACKED,(DWORD)WorkArea);
	
	_ChSize(this,0);
	
	//Printf("Create file: handle:%d size:%d",WorkArea->Handle,CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL));

	WorkArea->P_N=1;
#if defined (BASE_EMULATOR)
#if defined (ELF_BASES_SUPPORT)
	if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
		ret = (*ptr)();
	else
#endif /* defined (ELF_BASES_SUPPORT) */
		ret=link->ExecuteEntry(WorkArea, 0, "UN", (char*) ReadDWordPtr(&rp->NameIdx));
#else
	ret=link->ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */

	switch(ret){
	case P_OK:
	case P_DISK:
	case P_ERR:
	{
		DWORD len=CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL);
		if(ret==P_DISK || WorkArea->RFlags & RF_DISK_OUT_OF_SPACE)
		{
			WorkArea->RFlags|=RF_IO_ERROR;
			WorkArea->RFlags|=RF_DISK_OUT_OF_SPACE;
		}
		if(ret==P_ERR)
		{
			if(!(WorkArea->RFlags & RF_DISK_OUT_OF_SPACE))
			{
				char* vn=ScanObject->VirusName;
				ScanObject->VirusName=(char*)ReadDWordPtr(&rp->NameIdx);
				AvpCallback(AVP_CALLBACK_OBJECT_UNKNOWN,(DWORD)ScanObject);
				ScanObject->VirusName=vn;
			}
			if(WorkArea->Length == len)
				break;
		}

		if(0 < len)
		{
			strncpy_s(WorkArea->ZipName,AVP_MAX_PATH,(const char*)ReadDWordPtr(&rp->NameIdx),AVP_MAX_PATH-1);
			AvpCallback(AVP_CALLBACK_WA_PACKED_NAME,(long)WorkArea);
			
			if(ScanObject->MFlags & MF_CALLBACK_SAVE)
			{
				CACHEDEF(AvpFlush)(WorkArea->Handle);
				CACHEDEF(AvpFlush)(WorkArea->V_Handle);
				CACHEDEF(AvpFlush)(WorkArea->P_Handle);
				AvpCallback(AVP_CALLBACK_SAVE_PACKED,(DWORD)WorkArea);
			}
			
//				DWORD NativeName=ScanObject->NativeName;
//				ScanObject->NativeName=0;
			HANDLE soHandle=ScanObject->Handle;
			ScanObject->Handle=WorkArea->Handle;
			ScanObject->InPack++;
			DWORD rf=ScanObject->RFlags;
			ScanObject->RFlags=0;
			DWORD type=ScanObject->Type;
			ScanObject->Type=OT_FILE;
			
			DWORD error=0;
			if(!(rf&RF_IN_CURABLE_EMBEDDED))
				error=AvpCallback(AVP_CALLBACK_WA_PROCESS_OBJECT,(long)WorkArea);
			if(error==0) error=ProcessObject(ScanObject);
			
			REPORT_DONE=1;
			ScanObject->Type=type;
			ScanObject->InPack--;
			ScanObject->Handle=soHandle;
//				ScanObject->NativeName=NativeName;

			if(ScanObject->RFlags & RF_CURE)
			{
				if(ScanObject->Handle){
					if(WorkArea->Length!=(DWORD)Copy_File(ScanObject->Handle,WorkArea->Handle,this)){
						WorkArea->RFlags|=RF_NOT_OPEN;
						WorkArea->RFlags|=RF_CURE_FAIL;
					}
					break;
				}
				CACHEDEF(AvpCloseHandle)(sHandle);
				if(ScanObject->Attrib & FILE_ATTRIBUTE_READONLY)
					AvpSetFileAttributes(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,ScanObject->Attrib&~FILE_ATTRIBUTE_READONLY);
//					CACHEDEF(AvpDeleteFile)(ScanObject->Name);
				if(!X_AvpMoveFile(WorkArea->Handle,ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name)){
					WorkArea->RFlags|=RF_NOT_OPEN;
					WorkArea->RFlags|=RF_CURE_FAIL;
				}
				if(ScanObject->Attrib & FILE_ATTRIBUTE_READONLY)
					AvpSetFileAttributes(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,ScanObject->Attrib);

				CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
				WorkArea->Handle=INVALID_HANDLE_VALUE;
				
				sHandle=CACHEDEF(AvpCreateFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,  //!!!!!!!!!!!!!!!!!!
					GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
				if(sHandle==INVALID_HANDLE_VALUE)
				{
					WorkArea->RFlags|=RF_NOT_OPEN;
					WorkArea->RFlags|=RF_CURE_FAIL;
					ScanObject->RFlags|=rf;
					goto noclo;
				}
				WorkArea->Length=CACHEDEF(AvpGetFileSize)(sHandle,NULL);
				if(WorkArea->Length==(DWORD)-1)WorkArea->Length=0;

			}
			ScanObject->RFlags|=rf;
		}
	}
		break;

	case P_END:
	default:
		break;
	}
	if(WorkArea->Handle && WorkArea->Handle!=INVALID_HANDLE_VALUE)
		CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
noclo:
	if(vHandle && vHandle!=INVALID_HANDLE_VALUE){
		CACHEDEF(AvpCloseHandle)(vHandle);
		WorkArea->V_Handle=0;
	}

	WorkArea->Handle=sHandle;
	WorkArea->P_Handle=pHandle;
	if(fname){
		CACHEDEF(AvpDeleteFile)(fname);
		delete fname;
	}
	if(vname){
		CACHEDEF(AvpDeleteFile)(vname);
		delete vname;
	}
	
	vn=ScanObject->VirusName;
	ScanObject->VirusName=(char*)ReadDWordPtr(&rp->NameIdx);
	AvpCallback(AVP_CALLBACK_OBJECT_PACKED_DONE,(DWORD)ScanObject);
	ScanObject->VirusName=vn;
		
	ScanObject->RFlags&=~RF_ENCRYPTED;
    return REPORT_DONE;
}

int CBaseWork::ProcessArchive(DWORD offset)
{
	int l;
	int paRet=0;
	DWORD bread=0;
	AVPScanObject* ScanObject=WorkArea->ScanObject;
	BData* _bdata=bdata;
	
//this is transfer of control to Prague.
	WorkArea->P_N=offset;
	paRet=AvpCallback(AVP_CALLBACK_WA_PROCESS_ARCHIVE,(DWORD)WorkArea);
	WorkArea->P_N=0;
	if(paRet)return paRet;
	

	memset(WorkArea->Header,0,0x40);
	CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,offset,NULL,FILE_BEGIN);
	CACHEDEF(AvpReadFile)(WorkArea->Handle,WorkArea->Header,0x40,&bread,0);
	CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,offset,NULL,FILE_BEGIN);

	DWORD hs=CalcSum(WorkArea->Header,0x40) ^ (WorkArea->Length - offset);

	if(!bread){
		return FALSE;
	}


	if( hs && 
	(_bdata->ArchHeaderSum==hs
	||	ScanObject->ArchHeaderSum==hs)){
		ScanObject->RFlags|=RF_REPORT_DONE;
		return 2; //report as file, but stop extract
	}
	
	WorkArea->ZipName[0]=0;
	WorkArea->P_N=0;
	R_Extract* rp=(R_Extract*)DoRecords(RT_EXTRACT);//		inside	P_N=0
	CBase* ArcBasePtr=_bdata->BasePtr;
	
	if(rp)
	{
		DWORD ArchHeaderSum=_bdata->ArchHeaderSum;
//		_bdata->ArchHeaderSum=hs;
		ScanObject->ArchHeaderSum=hs;
			
		ScanObject->RFlags &=~RF_REPORT_DONE;
		ScanObject->RFlags|=RF_ARCHIVE;
		
		AVPScanObject* SO=new AVPScanObject;
		char* fname=new char[AVP_MAX_PATH];
		if(!SO || !fname){
			if(SO) delete SO;
			if(fname) delete fname;
			return paRet;
		}
		fname[0]=0;
		memcpy(SO,ScanObject,sizeof(AVPScanObject));
		AVPScanObject* pSO=WorkArea->ScanObject;
		WorkArea->ScanObject=SO;

		WorkArea->RFlags&=~RF_REPORT_DONE;
		ScanObject->RFlags&=~RF_REPORT_DONE;

		char* vn=ScanObject->VirusName;
		DWORD connection=ScanObject->Connection;
		ScanObject->Connection=offset;
		if(WorkArea->ZipName[0])
			ScanObject->VirusName=WorkArea->ZipName;
		else
			ScanObject->VirusName=(char*)ReadDWordPtr(&rp->NameIdx);
		paRet=AvpCallback(AVP_CALLBACK_OBJECT_ARCHIVE,(DWORD)ScanObject);
		AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)ScanObject->Name);
		ScanObject->VirusName=vn;
		ScanObject->Connection=connection;
		if(paRet) goto skip_archive;
		
		HANDLE pHandle; pHandle=WorkArea->P_Handle;
		DWORD  rflags;	rflags=WorkArea->RFlags;
		WORD  aflags;
		
		//              CACHEDEF(AvpCloseHandle)(WorkArea->P_Handle);
		
		WorkArea->P_Handle=WorkArea->Handle;
		AvpGetTempFileName(_bdata->TempPath,"AVP",0,fname);
		WorkArea->Handle=CACHEDEF(AvpCreateFile)(fname,
			GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
		
		char* pName; pName=WorkArea->Name;
		char* pExt; pExt=WorkArea->Ext;
		
		if(WorkArea->Handle==INVALID_HANDLE_VALUE)
		{
			ScanObject->RFlags|=RF_IO_ERROR;
		}
		else {
			AvpCallback(AVP_CALLBACK_WA_ARCHIVE,(DWORD)WorkArea);
next:
			_bdata->BPCount=0;
			SO->RFlags=0;
			WorkArea->RFlags=0;
			
			_ChSize(this,0);
			//                      memset(WorkArea->ZipName,0,0x512);
			
			strncpy_s(WorkArea->FullName,AVP_MAX_PATH,ScanObject->Name,AVP_MAX_PATH-1);
			WorkArea->Name=pName;
			WorkArea->Ext=pExt;
			
			WorkArea->Length=CACHEDEF(AvpGetFileSize)(WorkArea->P_Handle,NULL);
			if(WorkArea->Length==(DWORD)-1) WorkArea->Length=0;

			WorkArea->P_N=1;
            WORD ret;
			DWORD error=0;

#if defined (BASE_EMULATOR)
	#if defined (ELF_BASES_SUPPORT)
			ELF_DECODE_FUNC ptr = 0;
			if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
            	ret = (*ptr)();
			else
	#endif /* defined (ELF_BASES_SUPPORT) */
				ret=ArcBasePtr->LinkTable[RT_EXTRACT][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0, "EX", (char*)ReadDWordPtr(&rp->NameIdx));
#else

			ret=ArcBasePtr->LinkTable[RT_EXTRACT][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 0);
#endif /* defined BASE_EMULATOR */

			switch(ret){
			case P_OK:
			case P_DISK:
			case P_ERR:
				if(ret==P_DISK || WorkArea->RFlags & RF_DISK_OUT_OF_SPACE)
				{
					WorkArea->RFlags|=RF_IO_ERROR;
					WorkArea->RFlags|=RF_DISK_OUT_OF_SPACE;
				}
				if(ret==P_ERR && !(WorkArea->RFlags & (RF_DISK_OUT_OF_SPACE|RF_CORRUPT|RF_ERROR)))
				{
					char* vn=ScanObject->VirusName;
					ScanObject->VirusName=(char*)ReadDWordPtr(&rp->NameIdx);
					ScanObject->RFlags|=WorkArea->RFlags;
					AvpCallback(AVP_CALLBACK_OBJECT_UNKNOWN,(DWORD)ScanObject);
					ScanObject->VirusName=vn;
					paRet=1;
				}

				if((0 < CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL) )
					|| (WorkArea->RFlags & RF_PROTECTED))
				{
					AvpCallback(AVP_CALLBACK_WA_ARCHIVE_NAME,(long)WorkArea);
					
					if(ScanObject->MFlags & MF_CALLBACK_SAVE)
					{
						CACHEDEF(AvpFlush)(WorkArea->Handle);
						CACHEDEF(AvpFlush)(WorkArea->V_Handle);
						CACHEDEF(AvpFlush)(WorkArea->P_Handle);
						AvpCallback(AVP_CALLBACK_SAVE_ARCHIVED,(DWORD)WorkArea);
					}
					if(_bdata->CancelProcess) {
						paRet=1;
						break;
					}
					memcpy(SO,ScanObject,sizeof(AVPScanObject));
					WorkArea->ScanObject=SO;
					SO->Type=OT_FILE;
					SO->NativeName=0;
					l=strlen(WorkArea->ZipName);
					if(l>AVP_MAX_PATH-0x18){
						strcpy_s(WorkArea->ZipName+AVP_MAX_PATH-0x1e,6," ... ");
						l=strlen(WorkArea->ZipName);
					}
					if((l+strlen(SO->Name))>= AVP_MAX_PATH-0x10)
						strcpy_s(SO->Name+AVP_MAX_PATH-0x18-l,6," ... ");
					strcat_s(SO->Name,AVP_MAX_PATH,"/");
					strcat_s(SO->Name,AVP_MAX_PATH,WorkArea->ZipName);
					SO->InPack=0;
					SO->InArc++;
					
					SO->Handle=WorkArea->Handle;
					SO->RFlags=0;
					aflags=WorkArea->AFlags;
					SO->Size=CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL);
					if(SO->Size==(DWORD)-1) SO->Size=0;
					//mdw
					SO->ArchHeaderSum=ArchHeaderSum;
					
					if(WorkArea->RFlags & RF_PROTECTED)
					{
						AvpCallback(AVP_CALLBACK_OBJECT_BEGIN,(DWORD)SO);
						SO->RFlags|=WorkArea->RFlags;
						AvpCallback(AVP_CALLBACK_OBJECT_DONE,(DWORD)SO);
						goto next;
					}
					
					if((ArcBasePtr->LinkTable[RT_EXTRACT][ReadWordPtr(&rp->LinkIdx)].EntryC)
						&& (strlen((const char *)ReadDWordPtr(&rp->NameIdx)) >= 4)
						&& (0x65626d45 == ReadDWordPtr((DWORD*)(ReadDWordPtr(&rp->NameIdx))))
						// "Embe"
						)
					{
						SO->RFlags|=RF_IN_CURABLE_EMBEDDED;
					}
					else
					{
						error=AvpCallback(AVP_CALLBACK_WA_PROCESS_OBJECT,(long)WorkArea);
					}
					
					if(error==0)
						error=ProcessObject(SO);
					
					_bdata->ArchHeaderSum=SO->ArchHeaderSum;
					ArchHeaderSum=SO->ArchHeaderSum;
					
					// infected gugol also possible...
					//				if( SO->RFlags & 0xF ){
					//					ArchHeaderSum=0;
					//					ScanObject->ArchHeaderSum=0;
					//					_bdata->ArchHeaderSum=0;
					//				}
					
					WorkArea->ScanObject=pSO;
					WorkArea->AFlags=aflags;
					//CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
					
					ScanObject->RFlags|=SO->RFlags & ~(RF_CORRUPT|RF_REPORT_DONE|RF_IN_CURABLE_EMBEDDED);
					
					if(SO->RFlags & RF_DETECT)
					{
						ScanObject->VirusName=SO->VirusName;
						if(!ScanObject->InArc && SO->RFlags & RF_CURE_FAIL)
						{
							//						paRet=0;
							break;
						}
					}
					else if(SO->RFlags & RF_WARNING)
					{
						ScanObject->WarningName=SO->WarningName;
						ScanObject->VirusName=SO->WarningName;
					}
					else if(SO->RFlags & RF_SUSPIC)
					{
						ScanObject->SuspicionName=SO->SuspicionName;
						ScanObject->VirusName=SO->SuspicionName;
					}
					if(_bdata->CancelProcess) {
						paRet=1;
						break;
					}
				}
				if(ret==P_OK)
					goto next;
				break;

			case P_END:
			default:
				if(WorkArea->RFlags & (RF_CORRUPT|RF_ERROR)){
					ScanObject->RFlags|=WorkArea->RFlags;
				}
				paRet=1;
				break;
			}
			
			CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
			CACHEDEF(AvpDeleteFile)(fname);
		}

		WorkArea->ScanObject=pSO;
		WorkArea->Handle=WorkArea->P_Handle;
		WorkArea->P_Handle=pHandle;
		WorkArea->RFlags=rflags;

		if((ScanObject->RFlags & RF_CURE_FAIL) 
			&& ArcBasePtr->LinkTable[RT_EXTRACT][ReadWordPtr(&rp->LinkIdx)].EntryC)
		{
			if(/*!_bdata->CancelProcess &&*/ !ScanObject->Handle ){

				CACHEDEF(AvpCloseHandle)(WorkArea->Handle);
				if(ScanObject->Attrib & FILE_ATTRIBUTE_READONLY){
					if(AvpSetFileAttributes(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,(ScanObject->Attrib)&~FILE_ATTRIBUTE_READONLY)){
						WorkArea->AttrChanged=1;
					}
					else WorkArea->Handle=INVALID_HANDLE_VALUE;
				}
				if(WorkArea->Handle!=INVALID_HANDLE_VALUE)
				{
					_bdata->MainObjectHandle=
						WorkArea->Handle=CACHEDEF(AvpCreateFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,
						GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
				}
				
				if(WorkArea->Handle==INVALID_HANDLE_VALUE)
				{
					WorkArea->RFlags|=RF_NOT_OPEN;
					_bdata->MainObjectHandle=
					WorkArea->Handle=CACHEDEF(AvpCreateFile)(ScanObject->NativeName?(char*)ScanObject->NativeName:ScanObject->Name,
						GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
				}
			}
			WORD ret=R_FAIL;
			if( !ScanObject->InArc	&&	WorkArea->Handle!=INVALID_HANDLE_VALUE)
			{
				WorkArea->Length=CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL);
				if(WorkArea->Length==(DWORD)-1)WorkArea->Length=0;

#if defined (BASE_EMULATOR)
#if defined (ELF_BASES_SUPPORT)
				ELF_DECODE_FUNC ptr = 0;
				if ( (ptr = gEmul.LookupElfFunction ((const char*)ReadDWordPtr(&rp->NameIdx))) != 0 )
					ret = (*ptr)();
				else
#endif /* defined (ELF_BASES_SUPPORT) */
					ret=ArcBasePtr->LinkTable[RT_EXTRACT][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1, "EX", (char*)ReadDWordPtr(&rp->NameIdx));
#else
				ret=ArcBasePtr->LinkTable[RT_EXTRACT][ReadWordPtr(&rp->LinkIdx)].ExecuteEntry(WorkArea, 1);
#endif /* defined BASE_EMULATOR */
			}

			paRet=0;
			
			switch(ret){
			case R_CURE:
				ScanObject->RFlags&=~RF_CURE_FAIL;
				ScanObject->RFlags|=RF_CURE;
				AvpCallback(AVP_CALLBACK_OBJECT_CURE,(DWORD)ScanObject);
				break;
			case R_DELETE:
				WorkArea->RFlags|=RF_CURE_DELETE;
				if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)ScanObject))
					ScanObject->RFlags|=RF_DELETE;
				break;
			case R_PRECURE:
			default:
				ScanObject->RFlags|=RF_CURE_FAIL;
				AvpCallback(AVP_CALLBACK_OBJECT_CURE_FAIL,(DWORD)ScanObject);
				if(2==AvpCallback(AVP_CALLBACK_ASK_DELETE,(DWORD)ScanObject))
					ScanObject->RFlags|=RF_DELETE;
				break;
			}
		}
		delete SO;
		delete fname;

		DWORD sav_Entry;
		sav_Entry=ScanObject->Entry;
		ScanObject->Entry=offset;
		vn=ScanObject->VirusName;
		ScanObject->VirusName=(char*)ReadDWordPtr(&rp->NameIdx);
		AvpCallback(AVP_CALLBACK_OBJECT_ARCHIVE_DONE,(DWORD)ScanObject);
		ScanObject->Entry=sav_Entry;
		ScanObject->VirusName=vn;

skip_archive:
		;
	}
    return paRet;
}

#ifdef _MAKE2BYTES
static write2b=0;
#endif

BOOL CBaseWork::CheckMemory(AVPScanObject* ScanObject)
{
	BData* _bdata=bdata;
#ifdef _MAKE2BYTES
	if(write2b++>1
        && IDYES==AfxMessageBox("Write 2bytes in all bases?",MB_YESNO))
		for(int i=0;i<BaseArray->GetSize();i++)
			((CBase*)BaseArray[i])->Write2b();
#endif
		
        char MZ='M';
		
        if(_bdata->DosMemory==NULL){
			_bdata->DosMemory=new BYTE[DOSMEM_SIZE];
			if(_bdata->DosMemory==NULL)goto del;
			memset(_bdata->DosMemory,0,DOSMEM_SIZE);
			if(!AvpMemoryRead(0,DOSMEM_READSIZE,_bdata->DosMemory))goto del;
        }
        _bdata->MCB_Lines=new WORD[MAX_MCB_LINES];
        if(_bdata->MCB_Lines==NULL)goto del;
        _bdata->Trace_Lines=new DWORD[MAX_TRACE_LINES];
        if(_bdata->Trace_Lines==NULL)goto del;
		_bdata->Hook_Lines=new DWORD[MAX_HOOK_LINES];
        if(_bdata->Hook_Lines==NULL)goto del;
		
        _bdata->DOS_Mem=(WORD)(AvpGetDosMemSize()>>4);
		//        BIOS_Mem=(DOS_Mem+0x0FFF) & 0xF000;   // end of DOS memory (usually A000h on PC)
        _bdata->BIOS_Mem=0xA000;        // end of DOS memory (usually A000h on PC)
		
        _bdata->Num_MCB_Lines=0;
        WORD MCB_Seg;
		MCB_Seg=(WORD)AvpGetFirstMcbSeg();
        _bdata->Z_MCB=_bdata->MCB_Lines[_bdata->Num_MCB_Lines++]=MCB_Seg+1;
        while(_bdata->Num_MCB_Lines<(MAX_MCB_LINES-1))
        {
			MCB_Seg+=(WORD)DosPeek(MCB_Seg,3)+1;
			if(MCB_Seg<=0xA000)     _bdata->Z_MCB=MCB_Seg;
			char mz=(BYTE)DosPeek(MCB_Seg,0);
			switch(mz)
			{
			case 'Z':
				_bdata->MCB_Lines[_bdata->Num_MCB_Lines++]=MCB_Seg;
			case 'M':
				_bdata->MCB_Lines[_bdata->Num_MCB_Lines++]=MCB_Seg+1;
				MZ=mz;
				continue;
			default:
				if(MZ=='Z')     break;
				else{
					_bdata->Z_MCB=0xA000;
					goto nxtmetod;
				}
			}
			break;
			
        }
        _bdata->MCB_Lines[_bdata->Num_MCB_Lines++]=MCB_Seg;
nxtmetod:
		_bdata->Num_Trace_Lines=AvpGetDosTraceTable(_bdata->Trace_Lines,MAX_TRACE_LINES);
		_bdata->Num_Hook_Lines=AvpGetIfsApiHookTable(_bdata->Hook_Lines,MAX_HOOK_LINES);
#ifdef _DEBUG
		{
			for(UINT i=0;i<_bdata->Num_Hook_Lines;i++){
				BYTE buf[4];
				MemoryRead(_bdata->Hook_Lines[i],4,buf);
				Printf("Hook #%d  %08X: %02X %02X %02X %02X",
					i,_bdata->Hook_Lines[i],buf[0],buf[1],buf[2],buf[3]);
			}
		}
#endif
		
        DoRecords(RT_MEMORY);
		
del:
        if(_bdata->DosMemory)delete _bdata->DosMemory;
        if(_bdata->MCB_Lines)delete _bdata->MCB_Lines;
        if(_bdata->Trace_Lines)delete _bdata->Trace_Lines;
        if(_bdata->Hook_Lines)delete _bdata->Hook_Lines;
        _bdata->DosMemory=NULL;
		
        return FALSE;
}

BOOL CBaseWork::NextVolume()
{
#ifdef _DEBUG
	//      _asm int 3;
#endif
	
	if(WorkArea->ScanObject->InArc)return FALSE;
	HANDLE handle=CACHEDEF(AvpCreateFile)(WorkArea->FullName,
		GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if(handle==INVALID_HANDLE_VALUE)return FALSE;
	if(!AvpCallback(AVP_CALLBACK_IS_PRAGUE_HERE,0))
		CACHEDEF(AvpCloseHandle)(WorkArea->P_Handle);
	WorkArea->P_Handle=handle;
	WorkArea->Length=CACHEDEF(AvpGetFileSize)(WorkArea->P_Handle,NULL);
	if(WorkArea->Length==(DWORD)-1) WorkArea->Length=0;
	strncpy_s(WorkArea->ScanObject->Name,AVP_MAX_PATH,WorkArea->FullName,AVP_MAX_PATH-1);
	
	//      AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)WorkArea->ScanObject->Name);
	return TRUE;
}
/*
#ifdef _MAKE2BYTES

  void CBase::Write2b(){
  AVP_BaseHeader          BaseHeader;
  AVP_BlockHeader bhp;
  HANDLE f;
  
	f=CACHEDEF(AvpCreateFile)(BaseName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f!=INVALID_HANDLE_VALUE)
	{
	DWORD bread;
	CACHEDEF(AvpReadFile)(f,&BaseHeader,sizeof(BaseHeader),&bread,0);
	if(BaseHeader.Magic==AVP_MAGIC)
	{
	UINT i;
	for(i=0;i<BaseHeader.BlockHeaderTableSize;i++){
	BYTE* rp;
	CACHEDEF(AvpSetFilePointer)(f,BaseHeader.BlockHeaderTableFO+i*sizeof(AVP_BlockHeader),0,FILE_BEGIN);
	CACHEDEF(AvpReadFile)(f,&bhp,sizeof(AVP_BlockHeader),&bread,0);
	if(bread!=sizeof(AVP_BlockHeader))      break;
	if(bhp.BlockType!=BT_RECORD) continue;
	if(bhp.RecordType==RT_FILE)
	rp=(BYTE*)RPtrTable[RT_FILE];
	else if(bhp.RecordType==RT_SECTOR)
	rp=(BYTE*)RPtrTable[RT_SECTOR];
	else continue;
	
	  BYTE* b=new BYTE[bhp.UncompressedSize];
	  CACHEDEF(AvpSetFilePointer)(f,bhp.BlockFO,0,FILE_BEGIN);
	  CACHEDEF(AvpReadFile)(f,b, bhp.UncompressedSize, &bread, NULL);
	  int offs=3;
	  for(DWORD j=0;j<bhp.NumRecords;j++, offs+=bhp.RecordSize)
	  *(WORD*)(b+offs)=*(WORD*)(rp+offs);
	  CACHEDEF(AvpSetFilePointer)(f,bhp.BlockFO,0,FILE_BEGIN);
	  CACHEDEF(AvpWriteFile)(f,b, bhp.UncompressedSize, &bread, NULL);
	  
		bhp.CRC=CalcSum(b,bhp.UncompressedSize);
		CACHEDEF(AvpSetFilePointer)(f,BaseHeader.BlockHeaderTableFO+i*sizeof(AVP_BlockHeader),0,FILE_BEGIN);
		CACHEDEF(AvpWriteFile)(f,&bhp, sizeof(AVP_BlockHeader), &bread, NULL);
		delete b;
		}
		}
		CACHEDEF(AvpCloseHandle)(f);
        }
		}
		#endif
*/
typedef struct _R_FILE_BOTH
{
        R_File_Short*    rf_s;
        R_File_Cutted*   rf_c;
}R_File_Both;

BOOL CBaseWork::ProcessRedundant(AVPScanObject* ScanObject)
{
	//      BYTE* ptr=WorkArea->Header;
	//      for(int i=0;i<PAGE_C_BEGIN;i++,ptr++){
	
	BYTE* ptr=WorkArea->Page_C;
	DWORD bytes;
	BData* _bdata=bdata;

	if(!CreateRedundantHash())
		return FALSE;
		
	WorkArea->Length=CACHEDEF(AvpGetFileSize)(WorkArea->Handle,NULL);
	if(WorkArea->Length==(DWORD)-1) WorkArea->Length=0;

	CACHEDEF(AvpSetFilePointer)(WorkArea->Handle,0,NULL,FILE_BEGIN);
	CACHEDEF(AvpReadFile)(WorkArea->Handle,WorkArea->Page_C,PAGE_C_SIZE,&bytes,NULL);
	for(int i=0,k=0; (UINT)i<WorkArea->Length; i++,k++,ptr++){
		if(k>=PAGE_C_SIZE/2){
			memcpy(WorkArea->Page_C,WorkArea->Page_C+PAGE_C_SIZE/2, PAGE_C_SIZE/2);
			CACHEDEF(AvpReadFile)(WorkArea->Handle,WorkArea->Page_C+PAGE_C_SIZE/2,PAGE_C_SIZE/2,&bytes,NULL);
			k=0;
			ptr=WorkArea->Page_C;
			_Rotate(this,1);
			if(_bdata->CancelProcess)break;
		}
		
		int j=0;
		if(HT)
			j=HT[HT_MASK & *(WORD*)ptr]->GetSize();
		while(j--){
			R_File_Both* rf=(R_File_Both*)HT[HT_MASK & *(WORD*)ptr]->GetAt(j);

			if ( *(WORD*)ptr != ReadWordPtr(&rf->rf_s->ControlWord) )
				continue;
			if ( CalcSum(ptr,rf->rf_s->Len1) != ReadDWordPtr(&rf->rf_c->Sum1) )
				continue;
			if ( CalcSum(ptr + ReadWordPtr(&rf->rf_c->Off2) - ReadWordPtr(&rf->rf_s->Off1), rf->rf_c->Len2 ) != ReadDWordPtr(&rf->rf_c->Sum2) )
				continue;
			
			ScanObject->VirusName=(char*)ReadDWordPtr(&rf->rf_c->NameIdx);
			WorkArea->RFlags |= RF_DETECT;
			ScanObject->RFlags|=WorkArea->RFlags;
			AvpCallback(AVP_CALLBACK_OBJECT_DETECT,(DWORD)ScanObject);
			
			int act=AvpCallback(AVP_CALLBACK_ASK_CURE,(DWORD)ScanObject);
			if(act==1){
				WorkArea->RFlags|=RF_CURE_FAIL;
			}
			else if(act==2){
				WorkArea->RFlags|=RF_DELETE;
			}
			return TRUE;
		}
		if(_bdata->CancelProcess)break;
	}
	return FALSE;
}


BOOL CBaseWork::CreateRedundantHash()
{
	if(HT)
		return TRUE;
	if(!BaseArray)
		return FALSE;

	HT=(CPtrArray**) new DWORD[HT_MASK+1];
	if(HT)
	{
		for(int i=0;i<=HT_MASK;i++)
		{
			AvpCallback(AVP_CALLBACK_ROTATE,0);
			HT[i]=new CPtrArray;
			if(HT[i]==NULL)
			{
				FreeRedundantHash();
				return FALSE;
			}
		}
	}

    for(int i=0;i<BaseArray->GetSize();i++)
    {
		bdata->BasePtr=(CBase*)BaseArray->GetAt(i);
		int j=bdata->BasePtr->RNumTable[RT_FILE];
		while(j--)
		{
			R_File_Short*  rfs=bdata->BasePtr->pFileShort + j;
			R_File_Cutted* rfc=(R_File_Cutted*)(bdata->BasePtr->RPtrTable[RT_FILE]) + j;

			if(rfs->Len1>=8
			&& ReadWordPtr(&rfc->LinkIdx) == (WORD)-1
			&& ReadWordPtr(&rfc->Off2) < PAGE_C_BEGIN )
			{
				R_File_Both* rf=(R_File_Both*)new(R_File_Both);
				if(rf)
				{
					rf->rf_s=rfs;
					rf->rf_c=rfc;
					HT[HT_MASK & ReadWordPtr(&rfs->ControlWord)]->Add(rf);
				}
			}
		}
    }
	return TRUE;
}

void CBaseWork::FreeRedundantHash()
{
#ifdef _DEBUG
    AvpCallback(AVP_CALLBACK_OBJ_NAME,(long)"Free Hash");
#endif
	if(HT){
        for(int i=0;i<=HT_MASK;i++)
		{
			if(HT[i])
			{
				int j=j=HT[i]->GetSize();
				while(j--)
				{
					R_File_Both* ptr=(R_File_Both*)HT[i]->GetAt(j);
					if(ptr)
						delete	ptr;
				}
				HT[i]->RemoveAll();
				delete HT[i];
				AvpCallback(AVP_CALLBACK_ROTATE,0);
			}
		}
		delete HT;
		HT=NULL;
	}
}

#if !defined(_CPP_NARROW_INLINES_DEFINED)
_TCHAR* strchr( _TCHAR* s, int c )
{
	int i=AVP_MAX_PATH;
	while(*s!=c){
		if(!*s || !i)
		{
			s=NULL;
			break;
		}
		s++;
		i--;
	}
	return s;
}
#endif

_TCHAR* FindPrevString(_TCHAR* pi)
{
	if(*(pi-1)=='\n') return NULL;
	pi--;
	while(*(--pi))if(*pi=='\n') break;
	return pi+1;
}

#define GET_NAME_BUF_SIZE 512

_TCHAR* CBaseWork::MakeFullName(_TCHAR* dest,const _TCHAR* src)
{
	*dest=0;
	if(src==NULL) return dest;
	
	_TCHAR s[GET_NAME_BUF_SIZE];
	_TCHAR *p=(char*)src;
	
	
	while(p && *p=='#')p=FindPrevString(p);
	if(p==NULL || (strlen(p)>GET_NAME_BUF_SIZE-10)) return dest;
	
	int l=GET_NAME_BUF_SIZE-strlen(p)-10;
	strcpy_s(s+l,GET_NAME_BUF_SIZE-l,p);
	
	int i;
	for(i=0;p[i]=='.' && i<16;i++) l++;
	
	while(i && NULL!=(p=FindPrevString(p))){
		_TCHAR* cp;
		if(*p=='#')continue;
again:
		cp=p;
		for(int k=i-1;k;k--){
			cp=strchr(cp,'.');
			if(cp==NULL)cp=".";
			cp++;
		}
		if(*cp!='.'){
			s[--l]='.';
			int n;
			for(n=0; n<128 && cp[n]!=0 && cp[n]!='.' && l>2 ; n++)l--;
			memcpy(s+l,cp,n);
			if(--i)
				goto again;
		}
	}
	strcpy_s(dest,strlen(s+l)+1,s+l);
	return dest;
}


#define MATCH_MATCH
#ifdef MATCH_MATCH
// 7.41.  Напишите функцию match(строка,шаблон); для проверки соответствия строки  упро-
// щенному регулярному выражению в стиле Шелл. Метасимволы шаблона:
//
//         * - любое число любых символов (0 и более);
//         ? - один любой символ.
//                       Усложнение:
//         [буквы]  - любая из перечисленных букв.
//         [!буквы] - любая из букв, кроме перечисленных.
//         [h-z]    - любая из букв от h до z включительно.
//
// Указание: для проверки "остатка" строки используйте рекурсивный вызов этой  же  функ-
// ции.
//      Используя эту функцию, напишите программу,  которая  выделяет  из  файла  СЛОВА,
// удовлетворяющие  заданному шаблону (например, "[Ии]*о*т"). Имеется в виду, что каждую
// строку надо сначала разбить на слова, а потом проверить каждое слово.

//    #include <stdio.h>
//    #include <string.h>
//    #include <locale.h>

    // Проверить, что smy лежит между smax и smin
int syinsy(unsigned smin, unsigned smy, unsigned smax)
     {
      char left   [2];
      char right  [2];
      char middle [2];

      left  [0]   = smin;  left  [1]   = '\0';
      right [0]   = smax;  right [1]   = '\0';
      middle[0]   = smy;   middle[1]   = '\0';

//      return (strcoll(left, middle) <= 0 && strcoll(middle, right) <= 0);
        return (strcmp(left, middle) <= 0 && strcmp(middle, right) <= 0);
     }

    #define  U(c) ((c) & 0377)  // подавление расширения знака
    #define  QUOT    '\\'       // экранирующий символ
    #ifndef  MATCH_ERR
//    # define MATCH_ERR printf("Нет ]\n")
    # define MATCH_ERR
    #endif

    // * s - сопоставляемая строка
    // * p - шаблон. Символ \ отменяет спецзначение метасимвола.

int match(register char *s, register char *p)
     {
      register int    scc; // текущий символ строки
      int     c, cc, lc;   // lc - предыдущий символ в [...] списке
      int     ok, notflag;

      for(;;)
       {
        scc = U(*s++);          // очередной символ строки
        switch (c = U (*p++))
         { // очередной символ шаблона
/*
          case QUOT:          // a*\*b
            c = U (*p++);
            if( c == 0 ) return(0); // ошибка: pattern
            else goto def;
*/
		  case '[':   // любой символ из списка
            ok = notflag = 0;
            lc = 077777;      // достаточно большое число
            if(*p == '!')
             { notflag=1; p++; }

            while ((cc=U(*p++))!=0)
             {
              if(cc == ']')
               {    // конец перечисления
                if (ok)
                      break;      // сопоставилось
                return (0);     // не сопоставилось
               }
              if(cc == '-')
               {    // интервал символов
                if (notflag)
                 {
                  // не из диапазона - OK
                  if (!syinsy (lc, scc, U (*p++)))
                     ok++;
                  // из диапазона - неудача
                  else return (0);
                 }
                else
                 {
                  // символ из диапазона - OK
                  if (syinsy (lc, scc, U (*p++)))
                     ok++;
                 }
               }
              else
               {
/*               
				 if (cc == QUOT)
                 {      // [\[\]]
                  cc = U(*p++);
                  if(!cc) return(0);// ошибка
                 }
*/
				 if (notflag)
                 {
                  if (scc && scc != (lc = cc))
                    ok++;          // не входит в список
                  else return (0);
                 }
                else
                 {
                  if (scc == (lc = cc)) // входит в список
                    ok++;
                 }
               }
             }
            if (cc == 0)
             {    // конец строки
              MATCH_ERR;
              return (0);        // ошибка
             }
            continue;
          case '*':   // любое число любых символов
            if(!*p)  return (1);
            for(s--; *s; s++)
                if(match (s, p))
                    return (1);
            return (0);
          case 0:
            return (scc == 0);
          default: 
//def:
            if(c != scc) return (0);
            continue;
          case '?':   // один любой символ
            if (scc == 0) return (0);
            continue;
         }
       }
     }


// Обратите внимание на то, что в UNIX расширением  шаблонов  имен  файлов,  вроде  *.c,
// занимается  не  операционная система (как в MS DOS), а программа-интерпретатор команд
// пользователя (shell: /bin/sh, /bin/csh, /bin/ksh).   Это  позволяет  обрабатывать  (в
// принципе) разные стили шаблонов имен.

#endif /* MATCH_MATCH */

int CBaseWork::CheckWildcards(_TCHAR *name, CPtrArray *array)
{
	int ret=0;
	if(name==0 || array==0) return 0;
	int nx=array->GetSize();
	if (nx==0)
		return 0;

	size_t fnSz = strlen(WorkArea->ScanObject->Name)+1;
	char* fn = new char[fnSz];
	if(!fn) return 0;
	strcpy_s(fn,fnSz,WorkArea->ScanObject->Name);
	AvpStrlwr(fn);//for directory match
	
	size_t chSz = strlen(name)+1;
	char* ch = new char[chSz];
	if(!ch){
		delete fn;
		return 0;
	}
	strcpy_s(ch,chSz,name);
	AvpStrlwr(ch);
	
	for(int j=0;j<nx;j++)
	{
		char* c=(char*)array->GetAt(j);
		if(!strcmp(fn,c) || match(ch,c))
		{
			ret=1;
			goto ret;
		}
	}
ret:
	delete fn;
	delete ch;
	return ret;
}




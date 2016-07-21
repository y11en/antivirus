#include "StdAfx.h"

//#define _NO_TRY_EXCEPT

#if defined(_WIN32) && !defined (_NO_TRY_EXCEPT)
# define _TRY __try
# define _FINALLY(x) __finally
#else
# define _TRY
# define _FINALLY(x)
#endif

#if !defined(__FILE__LINE__)
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)

// Use __FILE__LINE__ as a string containing "File.x(line#)" for example:
//     #pragma message( __FILE__LINE__ "Remove this line after testing.")
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
// The above example will display the file name and line number in the build window
// in such a way as to allow you to double click on it to go to the line.
#endif // __FILE__LINE__

#if defined USE_FM
	#include <../fpi/fpiapi.h>
	#include <../fpi/fpi_ids.h>
#endif

#if defined(__unix__)
	#include <sys/stat.h>
	#include <unistd.h>
	#include <string.h>
#endif

#include <_AVPIO.h>
#include "TFileData.h"
#include <DupMem/DupMem.h>
#include <ScanAPI/BaseAPI.h>
#include <BaseWork/SpcAlloc.h>

#ifndef SEEK_SET
#include <stdio.h>
#endif

#ifdef __MWERKS__
	#include <string.h>
#endif	

#ifdef INTERLOCK
extern "C" LONG InMyOpenClose;
#define INTERLOCK_InterlockedIncrement(x) InterlockedIncrement(x)
#define INTERLOCK_InterlockedDecrement(x) InterlockedDecrement(x)
#else
#define INTERLOCK_InterlockedIncrement(x)
#define INTERLOCK_InterlockedDecrement(x)
#endif

#ifdef MULTITHREAD
CRITICAL_SECTION csCacheData;
#else
#pragma message (__FILE__LINE__ "warning: SINGLETHREADED compilation!")
#define EnterCriticalSection(x) ;
#define LeaveCriticalSection(x) ;
#define InitializeCriticalSection(x) ;
#define DeleteCriticalSection(x) ;
#endif

#if defined (__MWERKS__) && !defined (USE_FM)

#define CSECT_SIZE    0x00001000  //4K
#define CSECTORS              16  //64K

#elif defined (__unix__) && !defined (USE_FM)

#define CSECT_SIZE    0x00010000  //64K
#define CSECTORS               8  //512K

#elif defined (FPIAPI_VER_MAJOR)

#define CSECT_SIZE    0x00001000  //4K
#define CSECTORS               8  //32K

#else //other windows clients

#define CSECT_SIZE    0x00001000  //4K
#define CSECTORS              32  //128K

#endif

#define CSECT_MASK (~(CSECT_SIZE-1))


#ifdef FPIAPI_VER_MAJOR

#if FPIMAX_READLENGTH < CSECT_SIZE
#error "FPIMAX_READLENGTH < CSECT_SIZE !!!"
#endif
extern LPFPIFILEIOFNTABLE	FPI_IO;
extern DWORD open_reserved_flag;
extern FPICONTAINERHANDLE fpiParentGet(LPCSTR filename_);
extern BOOL _QueryAbort(DWORD refdata_);

#endif//FPIAPI_VER_MAJOR

#ifdef USE_WIN32_API
#define AVPDEF(x) x
#else 
#define AVPDEF(x) Avp##x
#endif

typedef struct CACHE_TAGs
{
    DWORD sector;                 /* sector in cache */
	BOOL  dirty;                  // dirty flag
	DWORD count;
    TFileData* tFile;
	BYTE* data;
    struct CACHE_TAGs *next;      /* next block in LRU list */
    struct CACHE_TAGs *prev;      /* prev block in LRU list */
}  tCACHE_TAG ;

typedef struct CACHE_DATAs
{
    BYTE CacheData[CSECTORS*CSECT_SIZE];
	tCACHE_TAG* Head;
	tCACHE_TAG* Tail;
    tCACHE_TAG  TagArray[CSECTORS];
#ifdef _DEBUG
    DWORD CacheHits;
    DWORD CacheMisses;
#endif
}  tCACHE_DATA;

tCACHE_DATA* CACHE_DATA=0;

static void MoveBlockToHead(tCACHE_TAG *cblock)
{
	if(cblock->prev)
	{
		if(cblock->next)
			cblock->next->prev = cblock->prev;
		else 
			CACHE_DATA->Tail = cblock->prev;
		cblock->prev->next = cblock->next;      /* remove block from block list */
		cblock->next = CACHE_DATA->Head;        /* insert current block at head */
		cblock->next->prev = cblock;
		cblock->prev = 0;
		CACHE_DATA->Head = cblock;
	}
}

static void MoveBlockToTail(tCACHE_TAG *cblock)
{
	if(cblock->next)
	{
		if(cblock->prev)
			cblock->prev->next = cblock->next;
		else 
			CACHE_DATA->Head = cblock->next;
		cblock->next->prev = cblock->prev;      /* remove block from block list */
		cblock->prev = CACHE_DATA->Tail;        /* insert current block at tail */
		cblock->prev->next = cblock;
		cblock->next = 0;
		CACHE_DATA->Tail = cblock;
	}
}

static BOOL FlushCblock(tCACHE_TAG* cblock)
{
	BOOL ok=TRUE;
	if(cblock->dirty)
	{
		cblock->dirty=FALSE;
		_TRY
		{
			if(cblock->tFile->Magic != 0xADDAADDA)
				return FALSE;

			if(cblock->tFile->CurrLen > cblock->sector)
			{
				UINT size=cblock->tFile->CurrLen - cblock->sector;
				if(size>CSECT_SIZE) size=CSECT_SIZE;
				cblock->count=0;
				
#ifdef FPIAPI_VER_MAJOR
				switch(cblock->tFile->HandleFS)
				{
				case FS_FPI:
					if(cblock->tFile->RealPos != cblock->sector)
					{
						DWORD seek=FPI_IO->fFnSetPos((FPIFILEHANDLE)cblock->tFile->Handle,cblock->sector,SEEK_SET);
						if(seek==(DWORD)-1)
							ok=FALSE;
						else
							cblock->tFile->RealPos=seek;
					}
					if(ok)
						ok = FPI_IO->fFnWrite((FPIFILEHANDLE)cblock->tFile->Handle,cblock->data,size,&cblock->count);
					_QueryAbort(0);
					break;
				case FS_WIN32:
				default:
#endif//FPIAPI_VER_MAJOR
					if(cblock->tFile->RealPos != cblock->sector)
					{
						DWORD seek=AVPDEF(SetFilePointer)(cblock->tFile->Handle,cblock->sector,0,SEEK_SET);
						if(seek==(DWORD)-1)
							ok=FALSE;
						else
							cblock->tFile->RealPos=seek;
					}
					if(ok)
						ok=AVPDEF(WriteFile)(cblock->tFile->Handle,cblock->data,size,&cblock->count,0);
#ifdef FPIAPI_VER_MAJOR
					break;
				}
#endif//FPIAPI_VER_MAJOR
				if(ok){
					cblock->tFile->RealPos+=cblock->count;
					if(size != cblock->count)
						ok=FALSE;
				}
				if(!ok)
					cblock->tFile->WriteBackFailed=TRUE;
			}
		}
		_FINALLY(1)
		{
		}
	}
	return ok;
}

static void* FileSector(TFileData* tFile, DWORD sector, DWORD* count, BOOL dirty)
{
    tCACHE_TAG *cblock;
	BOOL ok =1;
	BOOL hit=0;
	
	if(count)*count=0;
	if(tFile==NULL) return NULL;

	cblock = CACHE_DATA->Head;  /* assume that first block is HEAD of list */

    while ( cblock )
    {
		if( cblock->tFile == NULL )
			break;

        if ((cblock->tFile == tFile ) &&
			(cblock->sector == sector ))
		{
			hit=1;
			break;
		}
        cblock = cblock->next;
    }
	if(!cblock)cblock = CACHE_DATA->Tail;
	MoveBlockToHead(cblock);

#ifdef _DEBUG
    if(hit) CACHE_DATA->CacheHits++;
	else    CACHE_DATA->CacheMisses++;
#endif

    if ( !hit )  /* cache miss */
    {
		/* check old content for modification */
		if(cblock->tFile && cblock->dirty)
			FlushCblock(cblock);
		
		memset(cblock->data,0,CSECT_SIZE);

		cblock->tFile = tFile;
		cblock->sector = sector;
		cblock->dirty = dirty;
		cblock->count=0;
#ifdef FPIAPI_VER_MAJOR
		switch(tFile->HandleFS)
		{
		case FS_FPI:
			if(tFile->RealPos != sector)
			{
				DWORD seek=FPI_IO->fFnSetPos((FPIFILEHANDLE)tFile->Handle,sector,SEEK_SET);
				if(seek==(DWORD)-1)
					ok=FALSE;
				else
					tFile->RealPos=seek;
			}
			if(ok){
				ok = FPI_IO->fFnRead((FPIFILEHANDLE)tFile->Handle,cblock->data,CSECT_SIZE,&cblock->count);
				if(dirty)
					ok = TRUE;
			}
			_QueryAbort(0);
			break;
		default:
		case FS_WIN32:
#endif//FPIAPI_VER_MAJOR
			if(tFile->RealPos != sector)
			{
				DWORD seek=AVPDEF(SetFilePointer)(tFile->Handle,sector,0,SEEK_SET);
				if(seek==(DWORD)-1)
					ok=FALSE;
				else
					tFile->RealPos=seek;
			}
			if(ok)
			{
				ok = AVPDEF(ReadFile)(tFile->Handle,cblock->data,CSECT_SIZE,&cblock->count,0);
				if(dirty)
					ok = TRUE;
			}
#ifdef FPIAPI_VER_MAJOR
			break;
		}
#endif//FPIAPI_VER_MAJOR
		
		if(ok){
			tFile->RealPos+=cblock->count;
		}
    }
	if(dirty)
	{
		cblock->dirty = dirty;
		cblock->count = CSECT_SIZE;
		cblock->tFile->FlushState=CACHE_FILE_FLUSH_STATE_MAPPED_MODIFIED;
	}
	else{
		if(cblock->tFile->FlushState == CACHE_FILE_FLUSH_STATE_NOT_MAPPED)
			cblock->tFile->FlushState = CACHE_FILE_FLUSH_STATE_MAPPED;
	}
	if(count)
		*count=cblock->count;

	if(ok)
		return cblock->data;
	else
	    return NULL;
}

static BOOL FileFlushAndRelease( TFileData* tFile, BOOL flush_all)
{
    tCACHE_TAG *cblock;
    tCACHE_TAG *sblock;
	switch(tFile->FlushState)
	{
	case CACHE_FILE_FLUSH_STATE_NOT_MAPPED:
		return !tFile->WriteBackFailed;
	case CACHE_FILE_FLUSH_STATE_MAPPED:
		if(flush_all)
			tFile->FlushState=CACHE_FILE_FLUSH_STATE_NOT_MAPPED;
		else
			return !tFile->WriteBackFailed;
		break;
	case CACHE_FILE_FLUSH_STATE_MAPPED_MODIFIED:
		if(flush_all)
			tFile->FlushState=CACHE_FILE_FLUSH_STATE_NOT_MAPPED;
		else
			tFile->FlushState=CACHE_FILE_FLUSH_STATE_MAPPED;
		break;
	default:
		break;
	}
    
	EnterCriticalSection(&csCacheData);
	_TRY
	{
		cblock = CACHE_DATA->Head;
		while ( cblock && cblock->tFile ) //this is ok, all blocks after should be free.
		{
			BOOL move=FALSE;
			if( cblock->tFile == tFile )
			{
				move=flush_all;
				if(tFile->CurrLen <= cblock->sector )
					move = TRUE;
				else 
					move |= FlushCblock(cblock);
				
			}
			sblock = cblock->next;
			if(move){
				cblock->tFile = NULL;
				MoveBlockToTail(cblock);
			}
			cblock = sblock;
		}
	}
	_FINALLY(1)
	{
		LeaveCriticalSection(&csCacheData);
	}
    return !tFile->WriteBackFailed;
}

extern "C" BOOL g_rotate_read=0;

BOOL WINAPI Cache_AvpReadFile (
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,        // number of bytes to read
    LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
    OVERLAPPED FAR*  lpOverlapped       // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
        )
{
	BOOL  ok=TRUE;
    BYTE* data;
	BYTE* buffer=(BYTE*)lpBuffer;
	TFileData* tFile=(TFileData*)hFile;
    if(lpNumberOfBytesRead)
		*lpNumberOfBytesRead = 0;
	if( hFile == 0 || hFile == (HANDLE)-1)
		return 0;
	if(tFile->WriteBackFailed) 
		return FALSE;

    if (tFile->CurrPos > tFile->CurrLen) 
		nNumberOfBytesToRead=0;
	else if ((tFile->CurrPos + nNumberOfBytesToRead) > tFile->CurrLen)
		nNumberOfBytesToRead = tFile->CurrLen - tFile->CurrPos;

    while ( nNumberOfBytesToRead )
    {
		UINT  readoff = tFile->CurrPos & ~(CSECT_MASK);
		UINT  toread = 0;
		DWORD count=0;
		BOOL  rotate;
		EnterCriticalSection(&csCacheData);
		_TRY{
			data = (BYTE *)FileSector(tFile,tFile->CurrPos & CSECT_MASK,&count,FALSE);
			if(data==NULL){
				ok= FALSE;
			}
			else
			{
				data += readoff;
				if ( count > readoff )
				{
					toread = count - readoff;
					if ( toread > nNumberOfBytesToRead ) toread = nNumberOfBytesToRead;
					memcpy(buffer,data,toread);
				}
				else
					ok= FALSE;
			}
		}
		_FINALLY(1)
		{
			rotate=g_rotate_read;
			g_rotate_read=0;
			LeaveCriticalSection(&csCacheData);
		}
		if(rotate && ok)
			ok=!AvpCallback(AVP_CALLBACK_ROTATE,0);

		if( !ok )
			break;

		if(lpNumberOfBytesRead)
			*lpNumberOfBytesRead += toread;
        tFile->CurrPos += toread;
        buffer += toread;
        nNumberOfBytesToRead -= toread;
    }
	
    return ok;
}

BOOL  WINAPI Cache_AvpWriteFile (
    HANDLE  hFile,      // handle to file to write to
    LPCVOID  lpBuffer,  // pointer to data to write to file
    DWORD  nNumberOfBytesToWrite,       // number of bytes to write
    LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
        )
{
	BOOL  ok=TRUE;
    BYTE* data;
	BYTE* buffer=(BYTE*)lpBuffer;
	TFileData* tFile=(TFileData*)hFile;
    if(lpNumberOfBytesWritten)
	    *lpNumberOfBytesWritten = 0;
	if(tFile->WriteBackFailed)
		return FALSE;

    while ( nNumberOfBytesToWrite && ok)
    {
        UINT readoff = tFile->CurrPos & ~(CSECT_MASK);
        UINT toread  = CSECT_SIZE - readoff;
		BOOL  rotate;
		EnterCriticalSection(&csCacheData);
		_TRY{
			data = (BYTE *)FileSector(tFile, tFile->CurrPos & CSECT_MASK, NULL, TRUE);
			if(data==NULL){
				ok=FALSE;
			}
			else{
				data += readoff;
				if ( toread > nNumberOfBytesToWrite ) toread = nNumberOfBytesToWrite;
				memcpy(data,buffer,toread);
			}
			if(ok){
				if(lpNumberOfBytesWritten)
					*lpNumberOfBytesWritten += toread;
				tFile->CurrPos += toread;
				buffer += toread;
				nNumberOfBytesToWrite -= toread;
				if(tFile->CurrPos > tFile->CurrLen)
				{
					tFile->SetEOF=1;
					tFile->CurrLen=tFile->CurrPos;
				}
			}
		}
		_FINALLY(1)
		{
			rotate=g_rotate_read;
			g_rotate_read=0;
			LeaveCriticalSection(&csCacheData);
		}
		if(rotate && ok)
			ok=!AvpCallback(AVP_CALLBACK_ROTATE,0);
    }

	if( ok && !tFile->WriteChecked){
		tFile->WriteChecked=1;

		if(!FileFlushAndRelease(tFile,FALSE))
		{
		    if(lpNumberOfBytesWritten)
				*lpNumberOfBytesWritten = 0;
			return FALSE;
		}
	}
    return ok;
}


BOOL  WINAPI Cache_AvpDeleteFile (
    LPCTSTR  lpFileName        // pointer to name of the file
	)
{
		return AVPDEF(DeleteFile)(lpFileName);
}

HANDLE  WINAPI Cache_AvpCreateFile (
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD  dwDesiredAccess,     // access (read-write) mode
    DWORD  dwShareMode, // share mode
    SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
    DWORD  dwCreationDistribution,      // how to create
    DWORD  dwFlagsAndAttributes,        // file attributes
    HANDLE  hTemplateFile       // handle to file with attributes to copy
        )
{
	TFileData * tFile = new TFileData;
	if(!tFile) return INVALID_HANDLE_VALUE;
	memset(tFile, 0, sizeof(TFileData));
	tFile->Magic = 0xADDAADDA;
	tFile->Handle=INVALID_HANDLE_VALUE;
	dwDesiredAccess|=GENERIC_READ;

	if(dwFlagsAndAttributes & FILE_ATTRIBUTE_TEMPORARY)
		tFile->WriteChecked=1;

#ifdef FPIAPI_VER_MAJOR
	if(!(dwFlagsAndAttributes & (FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE)))
	if(FPI_IO)
	{
		int access_=0;

		_QueryAbort(0);
		switch(dwDesiredAccess)
		{
		default:
		case 0:
		case GENERIC_READ:
			access_=FPIFILEIO_READ;
			break;
		case GENERIC_WRITE:
			access_=FPIFILEIO_WRITE;
			break;
		case GENERIC_WRITE | GENERIC_READ:
			access_=FPIFILEIO_RDWR;
			break;
		}
		switch(dwCreationDistribution){
		case CREATE_NEW:
		case CREATE_ALWAYS:
			access_=FPIFILEIO_CREATE;
		default:
			break;
		}
		
		FPIFILEHANDLE fh=FPI_IO->fFnOpen(fpiParentGet(lpFileName),lpFileName,access_,	
			open_reserved_flag,0,0);

		if(fh!=FPIFILEHANDLE_ERROR)
		{
			tFile->Handle=(HANDLE)fh;
			tFile->HandleFS = FS_FPI;
			tFile->CurrLen=FPI_IO->fFnGetSize((FPIFILEHANDLE)tFile->Handle);
		}
	}
#endif//FPIAPI_VER_MAJOR

	if (tFile->Handle==INVALID_HANDLE_VALUE)
	{
		if(hTemplateFile && lpFileName==NULL)
		{
			tFile->Handle=hTemplateFile;
			tFile->NoClose=1;
			tFile->InitPos=AVPDEF(SetFilePointer)(tFile->Handle,0,0,SEEK_CUR);
			AVPDEF(SetFilePointer)(tFile->Handle,0,0,SEEK_SET);
		}
		else
		{
			INTERLOCK_InterlockedIncrement(&InMyOpenClose);
			tFile->Handle = AVPDEF(CreateFile) (  lpFileName, dwDesiredAccess, dwShareMode, 
				lpSecurityAttributes, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile);
			INTERLOCK_InterlockedDecrement(&InMyOpenClose);
		}
		if( tFile->Handle!=INVALID_HANDLE_VALUE)
		{
			tFile->CurrLen=AVPDEF(GetFileSize)(tFile->Handle,0);
#ifdef FPIAPI_VER_MAJOR
			tFile->HandleFS=FS_WIN32;
#endif
		}
	}

	if (tFile->Handle==INVALID_HANDLE_VALUE){
		delete tFile;
		return INVALID_HANDLE_VALUE;
	}
	else
		return (HANDLE)tFile;
}

BOOL  WINAPI Cache_AvpCloseHandle(
    HANDLE  hObject     // handle to object to close
        )
{
	BOOL ok;
	TFileData* tFile=(TFileData*)hObject;
	
	if (INVALID_HANDLE_VALUE == tFile)
		return TRUE;

	ok  = FileFlushAndRelease(tFile,TRUE);

#ifdef FPIAPI_VER_MAJOR
	switch(tFile->HandleFS)
	{
	case FS_FPI:
		if(tFile->SetEOF){
			if(tFile->RealPos != tFile->CurrLen)
			{
				DWORD seek=FPI_IO->fFnSetPos((FPIFILEHANDLE)tFile->Handle,tFile->CurrLen,SEEK_SET);
				if(seek==(DWORD)-1)
					ok=FALSE;
				else
					tFile->RealPos=seek;
			}
			ok &= FPI_IO->fFnSetEnd((FPIFILEHANDLE)tFile->Handle);
		}
		ok &= FPI_IO->fFnClose((FPIFILEHANDLE)tFile->Handle);
		_QueryAbort(0);
		break;
	default:
	case FS_WIN32:
#endif//FPIAPI_VER_MAJOR
		if(tFile->SetEOF){
			if(tFile->RealPos != tFile->CurrLen)
			{
				DWORD seek=AVPDEF(SetFilePointer)(tFile->Handle,tFile->CurrLen,0,SEEK_SET);
				if(seek==(DWORD)-1)
					ok=FALSE;
				else
					tFile->RealPos=seek;
			}
			ok &= AVPDEF(SetEndOfFile)(tFile->Handle);
		}

		if(tFile->NoClose)
		{
			if(tFile->InitPos)
				AVPDEF(SetFilePointer)(tFile->Handle,tFile->InitPos,0,SEEK_SET);
		}
		else
		{
			INTERLOCK_InterlockedIncrement(&InMyOpenClose);
			ok &= AVPDEF(CloseHandle)(tFile->Handle);
			INTERLOCK_InterlockedDecrement(&InMyOpenClose);
		}
#ifdef FPIAPI_VER_MAJOR
		break;
	}
#endif//FPIAPI_VER_MAJOR
	tFile->Magic = 0xAFFAAFFA;
	delete tFile;
	return ok;
}

DWORD  WINAPI Cache_AvpSetFilePointer(
    HANDLE  hFile,      // handle of file
    LONG  lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh,        // address of high-order word of distance to move
    DWORD  dwMoveMethod         // how to move
        )
{
	TFileData* tFile=(TFileData*)hFile;
	if( hFile == 0 || hFile == (HANDLE)-1)
		return -1;

	switch(dwMoveMethod)
	{
	case SEEK_SET:
		if(lDistanceToMove<0) 
			return (DWORD)-1;
		tFile->CurrPos=lDistanceToMove; break;

	case SEEK_CUR:
		if(lDistanceToMove<0)
		{
			if(tFile->CurrPos < (DWORD)(-lDistanceToMove))
				return (DWORD)-1;
		}else{
			if(tFile->CurrPos + lDistanceToMove < tFile->CurrPos) 
				return (DWORD)-1;
		}
		tFile->CurrPos+=lDistanceToMove; break;

	case SEEK_END:
		if(lDistanceToMove<0)
		{
			if(tFile->CurrLen < (DWORD)(-lDistanceToMove))
				return (DWORD)-1;
		}else{
			if(tFile->CurrLen + lDistanceToMove < tFile->CurrLen) 
				return (DWORD)-1;
		}
		tFile->CurrPos=tFile->CurrLen+lDistanceToMove; break;
	
	default:
		return (DWORD)-1;
	}

	return tFile->CurrPos;
}

BOOL  WINAPI Cache_AvpSetEndOfFile(
    HANDLE  hFile       // handle of file
        )
{
	BOOL ok =1;
	TFileData* tFile=(TFileData*)hFile;
	if(tFile->WriteBackFailed)
		return FALSE;

	EnterCriticalSection(&csCacheData);
	_TRY
	{
		tFile->CurrLen=tFile->CurrPos;
		tFile->SetEOF=1;
		ok  = FileFlushAndRelease(tFile,FALSE);
#ifdef FPIAPI_VER_MAJOR
		switch(tFile->HandleFS)
		{
		case FS_FPI:
			if(tFile->RealPos != tFile->CurrLen)
			{
				DWORD seek=FPI_IO->fFnSetPos((FPIFILEHANDLE)tFile->Handle,tFile->CurrLen,SEEK_SET);
				if(seek==(DWORD)-1)
					ok=FALSE;
				else
					tFile->RealPos=seek;
			}
			ok &= FPI_IO->fFnSetEnd((FPIFILEHANDLE)tFile->Handle);
			_QueryAbort(0);
			break;
		case FS_WIN32:
		default:
#endif//FPIAPI_VER_MAJOR
			if(tFile->RealPos != tFile->CurrLen)
			{
				DWORD seek=AVPDEF(SetFilePointer)(tFile->Handle,tFile->CurrLen,0,SEEK_SET);
				if(seek==(DWORD)-1)
					ok=FALSE;
				else
					tFile->RealPos=seek;
			}
			ok &= AVPDEF(SetEndOfFile)(tFile->Handle);
#ifdef FPIAPI_VER_MAJOR
			break;
		}
#endif//FPIAPI_VER_MAJOR
	}
	_FINALLY(1)
	{
		LeaveCriticalSection(&csCacheData);
	}
	return ok;
}

DWORD WINAPI Cache_AvpGetFileSize(
    HANDLE  hFile,      // handle of file
    LPDWORD  lpHigh
        )
{
	TFileData* tFile=(TFileData*)hFile;
	return tFile->CurrLen;
}

BOOL WINAPI AvpFlush(
    HANDLE  hFile       // handle of file
        )
{
	return 1;
}

BOOL WINAPI Cache_AvpFlush(
    HANDLE  hFile       // handle of file
        )
{
	TFileData* tFile=(TFileData*)hFile;
	if( hFile == 0 || hFile == (HANDLE)-1)
		return 1;
	return FileFlushAndRelease(tFile,FALSE);
}

LONG g_nCacheInitCount=0;

extern "C" int Cache_Destroy(void)
{
	
	if(0==(--g_nCacheInitCount))
	{
		if(CACHE_DATA)
		{
			SpcFree(CACHE_DATA,SPCALLOC_GLOBAL);
			CACHE_DATA=0;
		}
		DeleteCriticalSection(&csCacheData);
	}
	return 1;
}

extern "C" int Cache_Init(void)
{
	if(0==(g_nCacheInitCount++))
	{
		InitializeCriticalSection(&csCacheData);
		EnterCriticalSection(&csCacheData);
		CACHE_DATA=(tCACHE_DATA*)SpcAlloc(sizeof(tCACHE_DATA),SPCALLOC_GLOBAL);
		if(CACHE_DATA)
		{
			int i;
			tCACHE_TAG *cblock;
			tCACHE_TAG *pblock=0;
		
			memset(CACHE_DATA, 0, sizeof(tCACHE_DATA));
			CACHE_DATA->Head = CACHE_DATA->TagArray;
			cblock = CACHE_DATA->Head;
			
			for( i=0;i<CSECTORS;i++)
			{
				cblock->data= CACHE_DATA->CacheData + i*CSECT_SIZE;
				cblock->prev = pblock;
				pblock=cblock;
				cblock++;
				pblock->next = cblock;
			}
			pblock->next = 0;
			CACHE_DATA->Tail=pblock;
		}
		LeaveCriticalSection(&csCacheData);
	}
	return CACHE_DATA?1:0;
}

extern "C" HANDLE Cache_GetRealHandle(HANDLE  hFile)
{
	TFileData* tFile=(TFileData*)hFile;
	FileFlushAndRelease(tFile,FALSE);
	return ((TFileData*)hFile)->Handle;
}

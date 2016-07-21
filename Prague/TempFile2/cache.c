#include <wincompat.h>
#include "pr_oid.h"
#include "../tempfile/tempfile.h"
#if !defined (_WIN32)
#include <sys/mman.h>
#if !defined (MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

#define CSECT_SIZE 0x1000
#define CSECT_MASK (~((tQWORD)CSECT_SIZE-1))
#define CSECTORS 256

typedef struct CACHE_TAGs
{
    tQWORD sector;                 // sector in cache and dirty bit 
    hi_PosIO io;
	tBYTE* data;
	tBOOL  dirty;
	tDWORD count;
    struct CACHE_TAGs *next;      // next block in LRU list 
    struct CACHE_TAGs *prev;      // prev block in LRU list 
}  tCACHE_TAG ;

typedef struct CACHE_DATAs
{
    tCACHE_TAG  TagArray[CSECTORS];
	tCACHE_TAG* Head;
	tCACHE_TAG* Tail;
	
#ifdef _DEBUG
    tDWORD Hits;
    tDWORD Miss;
	tDWORD PushOuts;
	tDWORD Writes;
	tDWORD Reads;
#endif
}  tCACHE_DATA;

tCACHE_DATA* CACHE_DATA=0;
tBYTE* CacheMemory=NULL;
CRITICAL_SECTION hCS_Cache;

tERROR SectorCacheDestroy(hOBJECT parent)
{
	tERROR error=errOK;
	EnterCriticalSection(&hCS_Cache);
	if(CACHE_DATA){
		error=CALL_SYS_ObjHeapFree(parent,CACHE_DATA);
		CACHE_DATA=0;
	}
	if(CacheMemory)
	{
#if defined (_WIN32)
		VirtualFree(CacheMemory,0,MEM_RELEASE);
#else
                munmap ( CacheMemory, CSECTORS*CSECT_SIZE );
#endif
		CacheMemory=NULL;
	}
	LeaveCriticalSection(&hCS_Cache);
	DeleteCriticalSection(&hCS_Cache);
	return error;
}


tERROR SectorCacheInit(hOBJECT parent)
{
	tERROR error=errOK;
	InitializeCriticalSection(&hCS_Cache);
	EnterCriticalSection(&hCS_Cache);
	
	if(PR_SUCC(error) && CACHE_DATA==0)
	{
		tCACHE_DATA* CD;
#if defined (_WIN32)
		CacheMemory=VirtualAlloc(NULL, CSECTORS*CSECT_SIZE, MEM_RESERVE, PAGE_READWRITE);
#else
                CacheMemory = mmap ( 0, CSECTORS*CSECT_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
#endif
		if(CacheMemory==NULL)
			error=errNOT_ENOUGH_MEMORY;
		if(PR_SUCC(error))
			error=CALL_SYS_ObjHeapAlloc(parent,(tPTR*)&CD,sizeof(tCACHE_DATA));
		
		if(PR_SUCC(error)){
			int i;
			tCACHE_TAG *cblock;
			tCACHE_TAG *pblock=0;
		
			CACHE_DATA=CD;
			
			CACHE_DATA->Head = CACHE_DATA->TagArray;
			cblock = CACHE_DATA->Head;
			
			for( i=0;i<CSECTORS;i++)
			{
				cblock->data= CacheMemory + i*CSECT_SIZE;
				cblock->prev = pblock;
				pblock=cblock;
				cblock++;
				pblock->next = cblock;
			}
			pblock->next = 0;
			CACHE_DATA->Tail=pblock;
		}
	}
	LeaveCriticalSection(&hCS_Cache);
	return error;
}

static void SectorCacheMoveBlockToHead(tCACHE_TAG *cblock)
{
	if(cblock->prev)
	{
		if(cblock->next)
			cblock->next->prev = cblock->prev;
		else 
			CACHE_DATA->Tail = cblock->prev;
		cblock->prev->next = cblock->next;      // remove block from block list 
		cblock->next = CACHE_DATA->Head;        // insert current block at head 
		cblock->next->prev = cblock;
		cblock->prev = 0;
		CACHE_DATA->Head = cblock;
	}
}

static void SectorCacheMoveBlockToTail(tCACHE_TAG *cblock)
{
	cblock->io = NULL;
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

tERROR OpenRealIO(hi_PosIO io)
{
	tERROR error=errOK;
	if(io->data->real_io==NULL)
	{
		if(io->data->m_szFileName[0] == 0)
		{
			hSTRING str;
			if(io->data->m_szPath[0] == 0)
			{
				tDWORD size=0;
				error=CALL_SYS_SendMsg(io, pmc_TEMPFILE, pm_TEMPFILE_GETPATH, io->data->hStr, 0, 0);
				CALL_String_Length(io->data->hStr, &size);
				if(error!=errOK_DECIDED || size==0)
				{
					if(0 == GetTempPath(sizeof(io->data->m_szPath), io->data->m_szPath))
						strcpy(io->data->m_szPath,"C:\\");
					error = CALL_String_ImportFromBuff(io->data->hStr,0,io->data->m_szPath,0,cCP_ANSI,cSTRING_Z);
				}
				if(PR_SUCC(error)) error = CALL_String_ExportToBuff(io->data->hStr, &size, cSTRING_WHOLE, io->data->m_szPath, sizeof(io->data->m_szPath), cCP_ANSI, cSTRING_Z);
			}
			if(PR_SUCC(error))
				error=CALL_SYS_ObjectCreateQuick(io, &str, IID_STRING, PID_ANY, SUBTYPE_ANY );
			if(PR_SUCC(error))
				error=CALL_String_ImportFromStr(str,0,io->data->hStr,cSTRING_WHOLE);
			if(PR_SUCC(error))
				error=CALL_SYS_SendMsg(io, pmc_TEMPFILE, pm_TEMPFILE_GETNAME, str, 0, 0);
			if(error==errOK_DECIDED)
				error=CALL_String_ExportToBuff( str, 0, cSTRING_WHOLE, io->data->m_szFileName, sizeof(io->data->m_szFileName), cCP_ANSI, cSTRING_Z);
			else 
				GetTempFileName(io->data->m_szPath, "PR", 0 , io->data->m_szFileName);
			
		}
		if(PR_SUCC(error)){
			hIO hIo;
			if (PR_SUCC(error = CALL_SYS_ObjectCreate(io, &hIo, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY)))
			{
				error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS | fOMODE_TEMPORARY | fOMODE_SHARE_DENY_DELETE | fOMODE_SHARE_DENY_RW);
				if (PR_SUCC(error))
					error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_ACCESS_MODE, fACCESS_RW);
				if (PR_SUCC(error))
					error = CALL_SYS_PropertySetStr(hIo, 0, pgOBJECT_NAME, io->data->m_szFileName, 0, cCP_ANSI );
				if (PR_SUCC(error))
					error = CALL_SYS_ObjectCreateDone(hIo);	
				if (PR_FAIL(error))
					CALL_SYS_ObjectClose(hIo);
				else
					io->data->real_io=hIo;
			}
		}
	}
	return error;
}

tERROR SectorCacheMapSector(hi_PosIO io, tBYTE** bufptr ,tQWORD sector, tBOOL dirty, DWORD* count)
{
	tERROR error=errOK;
    tCACHE_TAG *cblock=0;
	tBOOL hit=cFALSE;

	cblock = CACHE_DATA->Head;  // assume that first block is HEAD of list 

    while( cblock && cblock->io ) // ok. after zero there should not be used blocks.
    {
        if (cblock->io == io  &&  cblock->sector == sector ){
			hit=cTRUE;
			break;
        }
        cblock = cblock->next;
    }
	if(!cblock)
		cblock = CACHE_DATA->Tail;
	
	SectorCacheMoveBlockToHead(cblock);

#ifdef _DEBUG
    if(hit) CACHE_DATA->Hits++;
	else{
		CACHE_DATA->Miss++;
		if(cblock->io)
			CACHE_DATA->PushOuts++;
	}
#endif
	
    if( !hit )
    {
		// check old content for modification 
		if( cblock->dirty && cblock->io ){
			if(cblock->io->data->FileSize > cblock->sector){
				tQWORD size=cblock->io->data->FileSize - cblock->sector;
				if(size>CSECT_SIZE) 
					size=CSECT_SIZE;

				if(cblock->io->data->real_io == 0)
					cblock->io->data->WriteBackError=OpenRealIO(cblock->io);

				if(cblock->io->data->real_io){
					cblock->io->data->WriteBackError=
						CALL_IO_SeekWrite((hi_PosIO)cblock->io->data->real_io,&cblock->count,cblock->sector,cblock->data,(tDWORD)size);

					if(PR_SUCC(cblock->io->data->WriteBackError))
						cblock->io->data->SwapSize = max(cblock->io->data->SwapSize, cblock->sector + cblock->count);
					
#ifdef _DEBUG
					CACHE_DATA->Writes++;
#endif
				}
			}
        }
		if( io->data->real_io && io->data->FileSize > sector ){
			if(sector >= io->data->SwapSize){
				if(!dirty){
					PR_TRACE( (io,prtDANGER,"Attempt to read not wtitten data",error) );				
				}
				memset(cblock->data,0,CSECT_SIZE);
			}else{
				error=CALL_IO_SeekRead((hi_PosIO)io->data->real_io,&cblock->count,sector,cblock->data,CSECT_SIZE);
				if(dirty || PR_SUCC(error)) 
					error=errOK;
#ifdef _DEBUG
				CACHE_DATA->Reads++;
#endif
			}
		}
		cblock->sector = sector;
		cblock->io = io;
		cblock->dirty = 0;
    }
	if(dirty){
		cblock->count=CSECT_SIZE;
		cblock->dirty |= dirty;
	}
	if(count)
		*count=cblock->count;
	if(bufptr)
		*bufptr=cblock->data;
	
    return error;
}



tERROR SectorCacheFlushIO( hi_PosIO io, tBOOL release, tBOOL write)
{
	tERROR error=errOK;
    tCACHE_TAG *cblock;

	if(write && io->data->FileSize){
		if(io->data->real_io == 0)
			error=OpenRealIO(io);
		if(io->data->real_io)
			error=CALL_IO_SetSize((hi_PosIO)io->data->real_io,io->data->FileSize);
	}
	
	EnterCriticalSection(&hCS_Cache);
    cblock = CACHE_DATA->Head;
    while( cblock && cblock->io )
	{
		if( cblock->io == io ){
			tBOOL move=release;
			if( io->data->FileSize > cblock->sector )
			{
				if( write && cblock->dirty )
				{
					tQWORD size=io->data->FileSize - cblock->sector;
					if(size>CSECT_SIZE) size=CSECT_SIZE;
					if(io->data->real_io){
						tERROR err=CALL_IO_SeekWrite((hi_PosIO)io->data->real_io,&cblock->count,cblock->sector,cblock->data,(tDWORD)size);
						if(PR_SUCC(err))
							io->data->SwapSize = max(io->data->SwapSize, cblock->sector + cblock->count);
						else
							error|=err;
					}
					else 
						error= errOBJECT_NOT_INITIALIZED;
				}
			}
			else  move=1;

			if(move){
				tCACHE_TAG *sb=cblock->next;
				SectorCacheMoveBlockToTail(cblock);
				cblock = sb;
				continue;
			}
		}
        cblock = cblock->next;
    }
	LeaveCriticalSection(&hCS_Cache);
		
	if(PR_SUCC(error))
		error=io->data->WriteBackError;
    return error;
}





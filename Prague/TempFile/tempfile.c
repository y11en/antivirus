// -------- Wednesday,  19 July 2000,  14:33 --------
// Project -- Prague
// Sub project -- Temporary File IO
// Purpose -- Temp Files
// ...........................................
// Author -- Andy Nikishin, Graf
// File Name -- tempfile.c
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------

// Фича -- по-умолчанию атрибут DELETE_ON_CLOSE установлен в cFALSE и игнорируется.
#if defined (_WIN32)

#define IO_PRIVATE_DEFINITION

#include <Prague/prague.h>
#include <wincompat.h>
#if defined (_WIN32)
#pragma hdrstop
#else
#include <sys/mman.h>
#if !defined (MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

#include <Prague/pr_oid.h>
#include <Prague/plugin/p_tempfile.h>


#include <Prague/iface/i_root.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_csect.h>

PR_MAKE_TRACE_FUNC;

#define CSECT_SIZE        0x8000
#define CSECT_SECTOR(x)   ( x & ~((tQWORD)CSECT_SIZE-1))
#define CSECT_OFFSET(x)   (tDWORD)( x & ((tQWORD)CSECT_SIZE-1)) 
#define CSECTORS          1024 /*1024*/
#define DELTA_SECTORS      32
#define LEAVE_SECTORS       4

hROOT g_root = NULL;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Interface comment )
// --------------------------------------------
// --- 74d331e1_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
// IO interface implementation
// Short comments -- input/output interface
// Extended comment
//   Defines behavior of input/output of an object
//   Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position. 
// AVP Prague stamp end( IO, Interface comment )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Data structure )
// Interface "IO". Inner data structure
typedef struct tag_TempFilesData 
{
	hIO      real_io; // -- 
	tDWORD   m_dwfAccessMode; // -- 
	tDWORD   m_dwfOpenMode;
	tQWORD   FileSize; // -- 
	tORIG_ID m_ObjectOrigin;
	tBYTE    byFillChar;
	tBOOL    bDeleteOnClose;
	hOBJECT  hFatherOS;
	tCHAR    m_szObjectName[MAX_PATH]; // -- 
	tCHAR    m_szObjectPath[MAX_PATH]; // -- 
	hSTRING  m_hStrFullFileName;


	tDWORD   Mapped;
	tERROR   WriteBackError;
	tQWORD   SwapSize; // -- 
	tBOOL    InClose; // -- 
	tQWORD   MaxMappedSector;


	tBYTE    XorByte;
	tBYTE    XorBytePrevious;
	tDWORD   XorByteIndex;
	tDWORD   StructureControlSize;
	tDWORD   RepeatCount;
	tDWORD   MaxRepeatCount;

} TempFilesData;
// AVP Prague stamp end( IO, Data structure )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, object declaration )
typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	TempFilesData*     data;   // pointer to the "IO" data structure
} *hi_IO;
// AVP Prague stamp end( IO, object declaration )
// --------------------------------------------------------------------------------

typedef   tERROR (pr_call *fnpIO_SeekRead)   ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SeekWrite)  ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SetSize)    ( hi_IO _this, tQWORD new_size );                    // -- ;
typedef   tERROR (pr_call *fnpIO_GetSize)    ( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );                  // -- returns requested size of the object;
typedef   tERROR (pr_call *fnpIO_Flush)      ( hi_IO _this );                                     // -- flush internal buffers;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External method table )
// Interface "IO". External method table.
struct iIOVtbl  
{
	fnpIO_SeekRead    SeekRead;
	fnpIO_SeekWrite   SeekWrite;
	fnpIO_GetSize     GetSize;
	fnpIO_SetSize     SetSize;
	fnpIO_Flush       Flush;
}; // IO


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal method table prototypes )
// Interface "IO". Internal method table. Forward declarations
static tERROR IO_ObjectInit( hi_IO _this ) ;
static tERROR IO_ObjectInitDone( hi_IO _this );
static tERROR IO_ObjectPreClose( hi_IO _this );
// AVP Prague stamp end( IO, Internal method table prototypes )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal method table )
// Interface "IO". Internal method table.
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       IO_ObjectInit,
	(tIntFnObjectInitDone)   IO_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   IO_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL 
};
// AVP Prague stamp end( IO, Internal method table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External method table prototypes )
// Interface "IO". External method table. Forward declarations
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SetSize( hi_IO _this,  tQWORD new_size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call IO_Flush( hi_IO _this );
// AVP Prague stamp end( IO, External method table prototypes )
// --------------------------------------------------------------------------------
static iIOVtbl e_IO_vtbl = 
{
	IO_SeekRead,
	IO_SeekWrite,
	IO_GetSize,
	IO_SetSize,
	IO_Flush
};

// AVP Prague stamp end( IO, External method table )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Forwarded property methods declarations )
// Interface "IO". Get/Set property methods
tERROR pr_call IO_PROP_TempIO_PropertyGet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call IO_PROP_TempIO_PropertySet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( IO, Forwarded property methods declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// Interface "IO". Property table
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY     (pgINTERFACE_VERSION, 2 )
	mSHARED_PROPERTY_PTR (pgINTERFACE_COMMENT, "Temporary File IO", 18 )
	mSHARED_PROPERTY     (pgOBJECT_AVAILABILITY, fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE | fAVAIL_DELETE_ON_CLOSE )
	mLOCAL_PROPERTY_BUF  (pgOBJECT_ORIGIN, TempFilesData, m_ObjectOrigin, cPROP_BUFFER_READ|cPROP_BUFFER_WRITE )

	mLOCAL_PROPERTY_BUF (pgOBJECT_NAME, TempFilesData, m_szObjectName, cPROP_BUFFER_READ | cPROP_BUFFER_WRITE)
	mLOCAL_PROPERTY_BUF (pgOBJECT_PATH, TempFilesData, m_szObjectPath, cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_FN  (pgOBJECT_FULL_NAME,   IO_PROP_TempIO_PropertyGet, IO_PROP_TempIO_PropertySet )
	mLOCAL_PROPERTY_FN  (pgOBJECT_ACCESS_MODE, IO_PROP_TempIO_PropertyGet, IO_PROP_TempIO_PropertySet )
	
	mLOCAL_PROPERTY_FN  (pgOBJECT_OPEN_MODE,       IO_PROP_TempIO_PropertyGet, IO_PROP_TempIO_PropertySet)
	mLOCAL_PROPERTY_BUF (pgOBJECT_FILL_CHAR,       TempFilesData, byFillChar,      cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF (pgOBJECT_DELETE_ON_CLOSE, TempFilesData, bDeleteOnClose,  cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF (pgOBJECT_BASED_ON,        TempFilesData, hFatherOS,       cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )

mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------



typedef struct CACHE_TAGs
{
    tQWORD sector;                 // sector in cache and dirty bit 
    hi_IO  io;
	tBYTE* data;
	tBOOL  dirty;
	tBOOL  committed;
    struct CACHE_TAGs *next;      // next block in LRU list 
    struct CACHE_TAGs *prev;      // prev block in LRU list 
}  tCACHE_TAG ;

typedef struct TF_CACHE_DATAs
{
    tCACHE_TAG  TagArray[CSECTORS];
	tCACHE_TAG* Head;
	tCACHE_TAG* Tail;
	tDWORD io_count;
	tDWORD sector_commited_count;
	tDWORD sector_allocated_count;
}  tTF_CACHE_DATA;

tTF_CACHE_DATA* CACHE_DATA=NULL;
tBYTE *CacheMemory=NULL;
CRITICAL_SECTION hCS_Cache;


tERROR SectorCacheDestroy(hOBJECT parent)
{
	tERROR error=errOK;
	EnterCriticalSection(&hCS_Cache);
	if(CACHE_DATA)
	{
		error=CALL_SYS_ObjHeapFree(parent,CACHE_DATA);
		CACHE_DATA=NULL;
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
	return error;
}


tERROR SectorCacheInit(hOBJECT parent)
{
	tERROR error=errOK;
	EnterCriticalSection(&hCS_Cache);

	if(PR_SUCC(error) && CACHE_DATA==0)
	{
		tTF_CACHE_DATA* CD;
#if defined (_WIN32 )
		CacheMemory=VirtualAlloc(NULL, CSECTORS*CSECT_SIZE, MEM_RESERVE, PAGE_READWRITE);
#else
                CacheMemory = mmap ( 0, CSECTORS*CSECT_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
#endif
		if(CacheMemory==NULL)
			error=errNOT_ENOUGH_MEMORY;
		if(PR_SUCC(error))
			error=CALL_SYS_ObjHeapAlloc(parent,(tPTR*)&CD,sizeof(tTF_CACHE_DATA));

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

tERROR OpenRealIO(hi_IO io)
{
	tERROR error=errOK;
	if(io->data->real_io==NULL)
	{
		tCHAR    szFileName[MAX_PATH]; // -- 
		tCHAR    szPath[MAX_PATH]; // -- 
		hSTRING hStr=0;
		tDWORD size=0;

		error=CALL_SYS_ObjectCreateQuick( io, &hStr, IID_STRING, PID_ANY, SUBTYPE_ANY );
		if(PR_SUCC(error)) 
			error=CALL_SYS_SendMsg(io, pmc_TEMPFILE, pm_TEMPFILE_GETPATH, hStr, 0, 0);

		CALL_String_Length(hStr, &size);
		if(error!=errOK_DECIDED || size==0)
		{
			if(0 == GetTempPath(sizeof(szPath), szPath))
				strcpy_s(szPath,countof(szPath),"C:\\");
			error = CALL_String_ImportFromBuff(hStr,0,szPath,0,cCP_ANSI,cSTRING_Z);
		}
		if(PR_SUCC(error)) 
			error = CALL_String_ExportToBuff(hStr, &size, cSTRING_WHOLE, szPath, sizeof(szPath), cCP_ANSI, cSTRING_Z);
		if(PR_SUCC(error))
			error=CALL_SYS_SendMsg(io, pmc_TEMPFILE, pm_TEMPFILE_GETNAME, hStr, 0, 0);
		if(error==errOK_DECIDED)
			error=CALL_String_ExportToBuff( hStr, 0, cSTRING_WHOLE, szFileName, sizeof(szFileName), cCP_ANSI, cSTRING_Z);
		else 
		{
			if( 0 == GetTempFileName(szPath, "PR", 0 , szFileName))
			{	
				if( 0== GetTempFileName("C:\\", "PR", 0 , szFileName))
					error=errOBJECT_NOT_FOUND;
			}
		}

		if(hStr)
			CALL_SYS_ObjectClose(hStr);

		if(PR_SUCC(error)){
			hIO hIo;
			if (PR_SUCC(error = CALL_SYS_ObjectCreate(io, &hIo, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY)))
			{
				error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS | fOMODE_TEMPORARY | fOMODE_SHARE_DENY_DELETE | fOMODE_SHARE_DENY_RW);
				if (PR_SUCC(error))
				{
					tDWORD access_flags = fACCESS_RW;
//					if( LOBYTE(LOWORD(GetVersion())) > 5 )
						access_flags |= fACCESS_FILE_FLAG_NO_BUFFERING;

					error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_ACCESS_MODE, access_flags);
				}
				if (PR_SUCC(error))
					error = CALL_SYS_PropertySetStr(hIo, 0, pgOBJECT_NAME, szFileName, 0, cCP_ANSI );
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

tERROR SectorCacheMapSector(hi_IO io, tBYTE** bufptr ,tQWORD sector, tBOOL dirty)
{
	tDWORD dwresult;
	tERROR error=errOK;
	tBOOL hit=cFALSE;
    tCACHE_TAG *cblock=CACHE_DATA->Tail;
	tBOOL reuse_last=io->data->Mapped > CSECTORS/2;

	if( io->data->Mapped && ( reuse_last || (io->data->MaxMappedSector >= sector) ))
	{
		tDWORD MappedCount=0;

		cblock = CACHE_DATA->Head;

		while( cblock->next ) 
		{
			if (cblock->io == io)
			{
				MappedCount++;
				if( cblock->sector == sector )
				{
					hit=cTRUE;
					break;
				}
				if( reuse_last && (MappedCount == io->data->Mapped))
					break;
			}

			cblock = cblock->next;
			if( !cblock->io )
			{
				if( !cblock->committed)
					cblock=CACHE_DATA->Tail;
				break;
			}
		}
	}

	if( !cblock->committed )
	{
#if defined (_WIN32)
		if(VirtualAlloc(cblock->data, CSECT_SIZE, MEM_COMMIT, PAGE_READWRITE))
		{
			cblock->committed=cTRUE;
			CACHE_DATA->sector_commited_count++;
		}
		else 
			return errNOT_ENOUGH_MEMORY;
#endif
	}

	SectorCacheMoveBlockToHead(cblock);
	
    if( !hit )
    {
		if( cblock->io )
		{
			if( cblock->dirty && (cblock->io->data->FileSize > cblock->sector))
			{
				tQWORD size=cblock->io->data->FileSize - cblock->sector;
				if(size>CSECT_SIZE) 
					size=CSECT_SIZE;
				if(cblock->io->data->real_io == 0)
				{
					error=CALL_SYS_ObjectValid((hOBJECT)io,(hOBJECT)cblock->io);
					if(PR_SUCC(error))
						error=OpenRealIO(cblock->io);
				}
				if(PR_SUCC(error))
					error=CALL_IO_SeekWrite((hi_IO)cblock->io->data->real_io,&dwresult,cblock->sector,cblock->data,CSECT_SIZE);
				if(PR_FAIL(error))
					return error;
				cblock->io->data->SwapSize = max(cblock->io->data->SwapSize, cblock->sector + CSECT_SIZE);
				cblock->io->data->Mapped--;
			}
        }
		else
			CACHE_DATA->sector_allocated_count++;

		if( io->data->real_io && io->data->FileSize > sector )
		{
			if(sector >= io->data->SwapSize)
			{
				if(!dirty)
					PR_TRACE( (io,prtDANGER,"Attempt to read not wtitten data") );				
				memset(cblock->data,0,CSECT_SIZE);
			}
			else
			{
				error=CALL_IO_SeekRead((hi_IO)io->data->real_io,&dwresult,sector,cblock->data,CSECT_SIZE);
				if(dirty || PR_SUCC(error)) 
					error=errOK;
			}
		}
		cblock->sector = sector;
		cblock->io = io;
		cblock->dirty = 0;
		io->data->Mapped++;
		if(sector > io->data->MaxMappedSector)
			io->data->MaxMappedSector=sector;
    }
	if(dirty)
		cblock->dirty=dirty;
	if(bufptr)
		*bufptr=cblock->data;
	
    return error;
}


tERROR SectorCacheReleaseIOSectors( hi_IO io, tBOOL release_all)
{
	tERROR error=errOK;
	tBOOL  decommit=cFALSE;
    tCACHE_TAG *cblock;
	if(!CACHE_DATA)
		return errOBJECT_ALREADY_FREED;

	EnterCriticalSection(&hCS_Cache);

	if(release_all)
	{
		CACHE_DATA->io_count--;
		if( (!CACHE_DATA->io_count && (CACHE_DATA->sector_commited_count > LEAVE_SECTORS))
			|| (DELTA_SECTORS < (CACHE_DATA->sector_commited_count - CACHE_DATA->sector_allocated_count)) )
			decommit=cTRUE;

	}

	cblock=CACHE_DATA->Head;

	while( cblock )
	{
		if( cblock->io == io )
		{
			if( release_all || cblock->sector >= io->data->FileSize )
			{
				tCACHE_TAG *sb=cblock->next;
				cblock->io = NULL;
				CACHE_DATA->sector_allocated_count--;
				SectorCacheMoveBlockToTail(cblock);
				cblock = sb;
				io->data->Mapped--;
				if(io->data->Mapped || decommit)
					continue;
				break;
			}
		}
		
		if( !cblock->io )
		{
			if( decommit )
			{
				if(CACHE_DATA->sector_commited_count > LEAVE_SECTORS)
					if(cblock->committed)
					{
						cblock->committed=cFALSE;
						CACHE_DATA->sector_commited_count--;
#if defined (_WIN32)
						VirtualFree(cblock->data, CSECT_SIZE, MEM_DECOMMIT);
#endif
					}
			}
			else
				break;
		}
        cblock = cblock->next;
    }
	

	if(release_all)
		io->data->MaxMappedSector=0;

	LeaveCriticalSection(&hCS_Cache);

	if(release_all)
		if(io->data->real_io)
			error = CALL_SYS_ObjectClose(io->data->real_io);

    return error;
}









// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property method implementation )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR pr_call IO_PROP_TempIO_PropertyGet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
	tERROR error = errOK;

	switch( prop )
	{

		case pgOBJECT_FULL_NAME:
			if(_this->data->m_hStrFullFileName)
				error= CALL_String_ExportToBuff(_this->data->m_hStrFullFileName, out_size, cSTRING_WHOLE, buffer, size, cCP_ANSI, cSTRING_Z);
			else
				error=errPROPERTY_NOT_FOUND;
			return error;

		case pgOBJECT_ACCESS_MODE:
			if(size >= sizeof(tDWORD))
			{
				if(buffer)
					*((tDWORD*)buffer) = _this->data->m_dwfAccessMode;
				else
					return errPARAMETER_INVALID;
			}
			else
				error = errBUFFER_TOO_SMALL;
			if(out_size)
				*out_size = size;
			break;

		case pgOBJECT_OPEN_MODE:
			if(size >= sizeof(tDWORD))
			{
				if(buffer)
					*((tDWORD*)buffer) = _this->data->m_dwfOpenMode;
				else
					return errPARAMETER_INVALID;
			}
			else
				error = errBUFFER_TOO_SMALL;
			if(out_size)
				*out_size = size;
			break;
	}
	return error;
}
// AVP Prague stamp end( IO, Property method implementation )
// --------------------------------------------------------------------------------

tERROR pr_call IO_PROP_TempIO_PropertySet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
	tERROR error = errOK;
	if ( !buffer )
		return errPARAMETER_INVALID;
	switch( prop )
	{
		case pgOBJECT_FULL_NAME:
			if(_this->data->m_hStrFullFileName==0)
				error=CALL_SYS_ObjectCreateQuick( _this, &_this->data->m_hStrFullFileName, IID_STRING, PID_ANY, SUBTYPE_ANY );
			if(_this->data->m_hStrFullFileName==0)
				return error;
			return CALL_String_ImportFromBuff(_this->data->m_hStrFullFileName,out_size,buffer,size,cCP_ANSI,cSTRING_Z);
		case pgOBJECT_ACCESS_MODE:
			_this->data->m_dwfAccessMode = *((tDWORD*)buffer);
			if(out_size)
				*out_size = sizeof(tDWORD);
			break;
		case pgOBJECT_OPEN_MODE:
			_this->data->m_dwfOpenMode = *((tDWORD*)buffer);
			if(out_size)
				*out_size = sizeof(tDWORD);
			break;
	}
	return error;
}


tERROR IO_ObjectInit( hi_IO _this ) 
{
	_this->data->m_ObjectOrigin = OID_GENERIC_IO;
	_this->data->bDeleteOnClose = cFALSE;
	_this->data->m_dwfAccessMode = fACCESS_RW;
	_this->data->m_dwfOpenMode = fOMODE_CREATE_ALWAYS;

	_this->data->StructureControlSize = 80;
	_this->data->MaxRepeatCount = 100000000;

	EnterCriticalSection(&hCS_Cache);
	CACHE_DATA->io_count++;
	LeaveCriticalSection(&hCS_Cache);
	return errOK;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal (kernel called) interface method implementation )
// --- Interface "IO". Method "ObjectInitDone"
// Extended method comment
//   Notification that all necessary properties have been set and object must go to operation state
// Behaviour comment
//   
// Result comment
//   Returns value differ from errOK if object cannot function properly
tERROR IO_ObjectInitDone( hi_IO _this ) 
{
	tERROR error = errOK;
	return error;
}
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal (kernel called) interface method implementation )
// --- Interface "IO". Method "ObjectClose"
// Extended method comment
//   Kernel warns object it must be closed
// Behaviour comment
//   Object takes all necessary "before closing" actions
// Result comment
//   
tERROR IO_ObjectPreClose( hi_IO _this ) 
{
	tERROR error=errOK;

	_this->data->InClose=1;
	error=SectorCacheReleaseIOSectors((hi_IO)_this, 1);

	if(_this->data->m_hStrFullFileName)
		CALL_SYS_ObjectClose(_this->data->m_hStrFullFileName);
	_this->data->m_hStrFullFileName=0;
	
	return error;
}
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekRead"
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error=errOK;
	tBOOL  eof=0;
    if(result)
		*result = 0;

	if( !buffer )
		return errPARAMETER_INVALID;
	else if(_this->data->InClose)
		return errOBJECT_ALREADY_FREED;
    else if (offset > _this->data->FileSize) 
		return errOUT_OF_OBJECT;
    else if(PR_FAIL(_this->data->WriteBackError))
		return _this->data->WriteBackError;
	else if ((offset + size) > _this->data->FileSize){
		size = (tDWORD) (_this->data->FileSize - offset);
		eof=1;
	}
	
    while( size && PR_SUCC(error) )
    {
		tBYTE* data;
		tDWORD readoff = CSECT_OFFSET(offset);
		tDWORD count = CSECT_SIZE - readoff;
		if( count > size ) count = size;
		
		EnterCriticalSection(&hCS_Cache);
		if(PR_SUCC(error=SectorCacheMapSector(_this, &data, CSECT_SECTOR(offset), 0)))
			memcpy(buffer, data + readoff, count);
		else
			count=0;
		LeaveCriticalSection(&hCS_Cache);
		
		if(result)
			*result += count;
        offset += count;
        ((tBYTE*)buffer) += count;
        size -= count;
    }

	if(eof && PR_SUCC(error))
		error=errEOF;
	
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekWrite"
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD p_size ) 
{
	tERROR error=errOK;
	tDWORD size=p_size;
    if(result)
		*result = 0;

	if( !buffer )
		return errPARAMETER_INVALID;
    else if(PR_FAIL(_this->data->WriteBackError))
		return _this->data->WriteBackError;
	else if(_this->data->InClose)
		return errOK;

	if(p_size  && _this->data->MaxRepeatCount)
	{
		tUINT i;
		for(i=0; i<p_size; i++)
		{
			_this->data->XorByte ^= ((tBYTE*)buffer)[i];
			_this->data->XorByteIndex ++;
			if(_this->data->XorByteIndex == _this->data->StructureControlSize)
			{
				_this->data->XorByteIndex=0;
				if(_this->data->XorBytePrevious != _this->data->XorByte)
				{
					_this->data->XorBytePrevious = _this->data->XorByte;
					_this->data->XorByte = 0;
					_this->data->RepeatCount=0;
				}
				_this->data->RepeatCount += p_size;
				break;
			}
		}

		if(_this->data->RepeatCount > _this->data->MaxRepeatCount)
			return errOBJECT_TOO_LONG_RECURRING_STRUCTURES;
	}


    while( size && PR_SUCC(error) )
    {
		tBYTE* data;
		tDWORD readoff = CSECT_OFFSET(offset);
		tDWORD count = CSECT_SIZE - readoff;
		if( count > size ) count = size;
		
		EnterCriticalSection(&hCS_Cache);
		if(PR_SUCC(error=SectorCacheMapSector(_this, &data, CSECT_SECTOR(offset), 1)))
			memcpy(data + readoff, buffer, count);
		else
			count=0;
		
		if(result)
			*result += count;
        offset += count;
        ((tBYTE*)buffer) += count;
        size -= count;
		
		if( _this->data->FileSize < offset)
			_this->data->FileSize = offset;

		LeaveCriticalSection(&hCS_Cache);
	}
	
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "Resize"
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size )
{
	tERROR error = errOK;
	EnterCriticalSection(&hCS_Cache);
	_this->data->RepeatCount=0;
	if(_this->data->FileSize != new_size)
	{
		_this->data->FileSize = new_size;
		error=SectorCacheReleaseIOSectors((hi_IO)_this,0);
	}
	LeaveCriticalSection(&hCS_Cache);
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "Size"
// Extended method comment
//   object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameter "type":
//   Size type is one of the following:
//     -- explicit
//     -- approximate
//     -- estimate
//   
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type ) 
{
	if(result)
	{
		*result= (tQWORD)_this->data->FileSize;
		return errOK;
	}
	return errPARAMETER_INVALID;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------


tERROR pr_call IO_Flush( hi_IO _this )
{
	return _this->data->WriteBackError;
	//ok - no need to flush tempfiles
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Registration call )
// Interface "IO". Register function
tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* error ) 
{
	switch( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH : 
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		// register my interfaces
		g_root = (hROOT)hInstance;
		*error = CALL_Root_RegisterIFace(
			(hROOT)hInstance,
			IID_IO,                                 // interface identifier
			TEMPIO_ID,                              // plugin identifier
			0x00000000,                             // subtype identifier
			0x00000002,                             // interface version
			VID_ANDY,                               // interface developer
			&i_IO_vtbl,                             // internal(kernel called) function table
			(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
			&e_IO_vtbl,                             // external function table
			(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
			IO_PropTable,                           // property table
			mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
			sizeof(TempFilesData),                  // memory size
			0                                       // interface flags
		);
		if ( PR_FAIL(*error  ))
			return cFALSE;

		InitializeCriticalSection(&hCS_Cache);
		*error=SectorCacheInit((hOBJECT)hInstance);
		// register my custom property ids
		// register my exports
		// resolve  my imports
		// use some system resources
		if ( PR_FAIL(*error  ))
			return cFALSE;
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		// free system resources
		// unregister my custom property ids -- you can drop it, kernel do it by itself
		// release    my imports		         -- you can drop it, kernel do it by itself
		// unregister my exports		         -- you can drop it, kernel do it by itself
		// unregister my interfaces          -- you can drop it, kernel do it by itself
		*error=SectorCacheDestroy((hOBJECT)hInstance);
		DeleteCriticalSection(&hCS_Cache);
		g_root = NULL;
		break;
	}
	return cTRUE;
}
// AVP Prague stamp end( IO, Registration call )
// --------------------------------------------------------------------------------

#endif //_WIN32


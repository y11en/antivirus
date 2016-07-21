// Version History:
// -----------------
// previous history omited
// 29.09.2003 - added support for extractions ntfs streams and comments
// 30.09.2003 - fixed crash in crypt.c  (wrong compiler optimization)
// 07.10.2003 - fixed bug in unpack20.c (wrong intrinsic function)
// 25.11.2003 - fixed bug in ReadAllBlocks() (decrypt salt skipped)
// 10.12.2003 - fixed bug in RarAskPassword() for encrypted filenames
// 19.01.2004 - improved password message + new proto in ObjCreate
// 04.02.2004 - fixed bug in mv chunk's hashes (crash)
// 24.02.2004 - added filetime updating after rebuild
// 03.03.2004 - fixed bug in ntfs stream extraction (wrong name size)
// 03.03.2004 - improved "read-only" flag for password protected objects
// 05.03.2004 - added pgOBJECT_VOLUME_NAME property for objects
// 05.03.2004 - send pm_OS_VOLUME_SET_INCOMPLETE message if not all vols
// 12.03.2004 - fixed bug in VM_RET processing (crash on invalid params)
// 23.03.2004 - no more heal in corrupted archives
// 25.03.2004 - new head healed as file head (bug with healing comments)
// 25.03.2004 - enabled cure/delete for single passwordprotected files
// 26.03.2004 - fixed bug with recovery record skip
// 30.03.2004 - fixed bug (attach next volume after exe stub for 2.x rar)
// 30.03.2004 - added avialability for each generated io
// 30.03.2004 - fixed bug (crash on ObjPtr == NULL)
// 30.03.2004 - fixed hash collision
// 31.03.2004 - fixed bug with healing new types (stm, cmt)
// 01.04.2004 - improved mv not-full-set detection
// 06.04.2004 - fixed bug in can-delete io property
// 08.04.2004 - improved unicode-to-ansi conver for streams names
// 08.04.2004 - improved hash (added file-blocks headers)
// 12.04.2004 - improved unicode names (use oem encoding)
// 14.04.2004 - fixed bug in multivol name backtrace
// 23.04.2004 - fixed bug: crash on LoadState()/SaveState() zero ptr
// 26.04.2004 - fixed bug in solid comments processing (window resizing)
// 19.05.2004 - fixed memory leak in load/save solid state (ppm suballoc)
// 26.05.2004 - removed errOK check in password ask
// 02.06.2004 - return errOBJECT_DATA_CORRUPT if crc check fail
// 16.06.2004 - fixed wrong corrupted flag on multivolume archives
// 22.07.2004 - improved international password processing
// 29.07.2004 - fixed crash on exchange (pmmd context overrun)
// 17.08.2004 - improved ppmd overrun checks
// 03.09.2004 - improved ppmd overrun checks
// 09.09.2004 - fixed bug in import (e_loader added)
// 11.09.2004 - improved processing multivol password protected archives
// 14.09.2004 - fixed bug in FileBufferUpdate()
// 24.09.2004 - added propertyes for psw-exe suspicion
// 22.12.2004 - fixed object leak in RAR_ObjCreate()
// 23.12.2004 - fixed bug in object name parser
// 28.02.2005 - fixed bug in healign pswd + non-pswd files in arc
// 04.03.2005 - added PPM overrun checks in PPMC_decodeSymbol1()
// 10.03.2005 - fixed bug in PPM overrun check in PPMC_decodeSymbol1()
// 06.05.2005 - added PPM overrun checks in PPM_UpdateModel()
// 16.05.2005 - added PPM overrun checks in PPM_CreateSuccessor()
// 22.06.2006 - fixed bug in PPMC_decodeSymbol1() (zero divide)
// 26.07.2005 - fixed bug in SA_RemoveNode() (crash on wrong ptr)
// 22.08.2005 - fixed bug in MemAllocHere() (crash on zero unitcount)
// 18.01.2006 - ignore block header crc (for malformed archives)
// 30.01.2006 - fixed cmt healing in solid arc
// 15.06.2007 - workaround for hacked version field in file headers
// 25.06.2007 - fixed uninitialized pointer in RAR_ObjectInit()
// 27.07.2007 - fixed memory overrun in VM_StandardFilterExecute()

// ----------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  19 December 2002,  15:53 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- RAR Archiver
// Sub project -- rar
// Purpose     -- RAR Archives processor
// Author      -- Dmitry Glavatskikh
// File Name   -- main.c
// Additional info
// RAR Archives processor. Works together with UniArc
// AVP Prague stamp end

// #define _PRAGUE_TRACE_

// ----------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define MiniArchiver_PRIVATE_DEFINITION
// AVP Prague stamp end

// ----------------------------------------------------------------------------

#include <Prague/prague.h>
#include "main.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/i_list.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_seqio.h>

#include <Extract/iface/i_minarc.h>

#include "Hash/MD5/plugin_hash_md5.h"
#include "./base/const.h"
#include "./base/proto.h"

#if defined DEBUG_PASS || defined DEBUG_PASS_LOG
# include <stdio.h>
#endif

#include <stdarg.h>
PR_MAKE_TRACE_FUNC;

// ----------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// MiniArchiver interface implementation
// Short comments -- Mini Plugin for Universal Archiver
// AVP Prague stamp end

static const tBYTE RAR_COMMENT[]   = "archive comment";
static const tBYTE RAR_DUMMYNAME[] = "dummy name";
static const tBYTE RAR_SIGN_PART[] = { 0x21,0x1A,0x07,0x00 };
static const tBYTE RAR_ENDARC[]    = { 0xC4,0x3D,0x7B,0x00,0x40,0x07,0x00 };
static const tBYTE RAR_OLDMHD[]    = { 0xCF,0x90,0x73,0x00,0x00,0x0D,0x00,
                                       0x00,0x00,0x00,0x00,0x00,0x00 };

#define MAX_OBJ_NAME       (1024)
#define MAX_OBJ_PATH       (2048)
#define MAX_OBJ_FULLNAME   (4096)
#define MAX_OBJ_VOLNAME    (1024)

// one node in volumes array
typedef struct sVOL_NODE {
  tCHAR   VolName[MAX_OBJ_VOLNAME];    // name w/o path
  hIO     hVolIO;                      // attached io
  hSEQ_IO hVolSeqIO;                   // attached seq_io
  tQWORD  VolHash;                     // volume hash
} tVOL_NODE, *pVOL_NODE;

// one base block node
typedef struct sBLOCK_NODE {
  struct  sVOL_NODE* Vol;              // block volume
  struct  sBASE_HDR BHdr;              // base header
  tDWORD  BlkOffs;                     // block offset in volume
  tDWORD  BlkSize;                     // total block size
} tBLOCK_NODE, *pBLOCK_NODE;

// one file part (for split)
typedef struct sPART_NODE {
  struct  sVOL_NODE* Volume;           // ptr to volume node
  struct  sRAR_FILE_HEADER FH;         // block-file header
  tDWORD  BlkOffs;                     // file offset of block header
  tDWORD  DataOffs;                    // offset of packed data in volume
  tDWORD  DataSize;                    // packed data size
  tDWORD  PartSize;                    // full size of this file part
  tUINT   BHdrNum;                     // current base hdr number
  tUINT   HdrFlag;                     // internal flags
} tPART_NODE, *pPART_NODE;

// one node in file array
typedef struct sFILE_NODE {
  struct  sPART_NODE* Parts;           // ptr to first file thunk
  tQWORD  FullSize;                    // total size of unpacked data
  tQWORD  TmpOffs;                     // position in temp-storage, -1 if none
  tDWORD  PartCnt;                     // number of file thunks
  tUINT   HdrFlag;                     // internal flags
  tBOOL   IsFolder;                    // it's folder flag
  tBOOL   ReadOnly;                    // read only flag
} tFILE_NODE, *pFILE_NODE;

#define RAR_FLG_HDRCRC   1             /* wrong header crc */
#define RAR_FLG_HDRCMT   2             /* file is comment  */

// ----------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface MiniArchiver. Inner data structure

typedef struct tag_RAR_Data {
  // names and strings
  tCHAR   ArcName[MAX_OBJ_NAME];       // input file name
  tCHAR   ArcPath[MAX_OBJ_PATH];       // inputh path
  tCHAR   ArcFull[MAX_OBJ_FULLNAME];   // input full name

  // counters
  tDWORD  BlkNum;            // total number of blocks
  tDWORD  FileNum;           // total number of files
  tDWORD  VolNum;            // total number of volumes
  tDWORD  PartNum;           // total number of files thinks
  tQWORD  CurrFile;          // current file in enum

  // arrays and structures
  struct  sBLOCK_NODE* Blks; // array of blocks nodes
  struct  sFILE_NODE* Files; // array of file nodes
  struct  sPART_NODE* Parts; // array of file thunks
  struct  sVOL_NODE*  Vols;  // array of volumes nodes
  struct  sRAR RAR;          // main rar structure for decompressors

  // additional stuff
  hHASH   hMD5;              // md5 hash for volumes
  hIO     hArcIO;            // input IO
  hSEQ_IO hSeqIn;            // input SeqIO
  hObjPtr hPtr;              // obj-ptr
  hHEAP   hHeap;             // working heap
  hOBJECT hOrigIO;           // base io

  // for temp data storage
  tQWORD  LastUnp;           // last unpacked id
  hIO     hTmpIO;            // temp data io
  hSEQ_IO hTmpSeq;           // temp data seq_io
  tVOID*  pTmpBuff;          // temp copy buffer

  tBOOL   ErrInSolid;        // error in solid arc

  // for compressor
  struct  sPACK20 Pack;      // packer context
  
  // for decryptor
  hSTRING hPswd;             // password container
  struct  sRAR  RarSave;     // saved context
  hIO     hUniArcIO;         // uni-arc io

  // for propertyes
  tBOOL   FullRO;            // fully read-only (solid or corrupt)
  tQWORD  VolHash;           // current volume hash
  tBOOL   AsSingle;          // pre-read all volumes
  tQWORD  CurrProp;          // current property
  tDWORD  OpenMode;          // open mode :-)
  tDWORD  AccessMode;        // access mode
  tDWORD  Avail;             // avialable operations
  tQWORD  CurFile;           // current file num
  tBOOL   IsThunk;           // archive is incomplete thunk
  tBOOL   NotFullSet;        // not all volumes founded
#ifdef DEBUG_PASS_LOG
  FILE*   FPasLog;           // debug passwords log
#endif
} tRAR_DATA, RAR_Data;

// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_MiniArchiver {
  const iMiniArchiverVtbl* vtbl; // pointer to the "MiniArchiver" virtual table
  const iSYSTEMVTBL*       sys;  // pointer to the "SYSTEM" virtual table
  RAR_Data*                data; // pointer to the "MiniArchiver" data structure
} *hi_MiniArchiver;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call RAR_Recognize(hOBJECT _that);
tERROR pr_call RAR_ObjectInit(hi_MiniArchiver _this);
tERROR pr_call RAR_ObjectInitDone(hi_MiniArchiver _this);
tERROR pr_call RAR_ObjectPreClose(hi_MiniArchiver _this);
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_MiniArchiver_vtbl = {
  (tIntFnRecognize)        RAR_Recognize,
  (tIntFnObjectNew)        NULL,
  (tIntFnObjectInit)       RAR_ObjectInit,
  (tIntFnObjectInitDone)   RAR_ObjectInitDone,
  (tIntFnObjectCheck)      NULL,
  (tIntFnObjectPreClose)   RAR_ObjectPreClose,
  (tIntFnObjectClose)      NULL,
  (tIntFnChildNew)         NULL,
  (tIntFnChildInitDone)    NULL,
  (tIntFnChildClose)       NULL,
  (tIntFnMsgReceive)       NULL,
  (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpMiniArchiver_Reset)          ( hi_MiniArchiver _this );             // -- First call of Next method after Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_Next)           ( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId ); // -- First call of Next method after creating or Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_ObjCreate)      ( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO); // -- Create IO from current enumerated object;
typedef   tERROR (pr_call *fnpMiniArchiver_SetAsCurrent)   ( hi_MiniArchiver _this, tQWORD qwObjectId ); // -- Set Object (qwObjectId) as currently enumerated object;
typedef   tERROR (pr_call *fnpMiniArchiver_RebuildArchive) ( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo ); // -- ;
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iMiniArchiverVtbl {
  fnpMiniArchiver_Reset           Reset;
  fnpMiniArchiver_Next            Next;
  fnpMiniArchiver_ObjCreate       ObjCreate;
  fnpMiniArchiver_SetAsCurrent    SetAsCurrent;
  fnpMiniArchiver_RebuildArchive  RebuildArchive;
}; // "MiniArchiver" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call RAR_Reset( hi_MiniArchiver _this );
tERROR pr_call RAR_Next( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId );
tERROR pr_call RAR_ObjCreate( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO);
tERROR pr_call RAR_SetAsCurrent( hi_MiniArchiver _this, tQWORD qwObjectId );
tERROR pr_call RAR_RebuildArchive( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iMiniArchiverVtbl e_MiniArchiver_vtbl = {
  RAR_Reset,
  RAR_Next,
  RAR_ObjCreate,
  RAR_SetAsCurrent,
  RAR_RebuildArchive
};
// AVP Prague stamp end



// ----------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call RAR_PropGet(hi_MiniArchiver _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size);
tERROR pr_call RAR_PropSet(hi_MiniArchiver _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size);
// AVP Prague stamp end

// archive related
tERROR ArchiveInit(hi_MiniArchiver _this, tRAR* data, tINPUT* input);
tERROR ReadCatalog(hi_MiniArchiver _this, tRAR* data);
tERROR ReadBaseHdr(tINPUT* input, tBASE_HDR* base);
tERROR UnpackFile(tRAR* data, tINPUT* input, tRAR_FILE_HEADER* fh);
tERROR AddTempFile(tRAR_DATA* Data, hSEQ_IO hSeqIn, tFILE_NODE* FNode); 
tERROR GetTempFile(tRAR_DATA* Data, hIO hOutIO, tFILE_NODE* FNode);
tERROR RarUnpackFile(tRAR_DATA* Data, tQWORD ID, hSEQ_IO hSeqOut);

tERROR InitArchive(hi_MiniArchiver _this);
tERROR ReadAllBlocks(hi_MiniArchiver _this);

// io related
tERROR CopySeqIOData(tRAR_DATA* Data, hSEQ_IO hSeqIn, tQWORD InpOffs,
                     hSEQ_IO hSeqOut, tQWORD OutOffs, tQWORD Count);
tERROR CreateTempIO(hi_MiniArchiver _this, hIO* hResIO);
tERROR CreateTempSeqIO(hi_MiniArchiver _this, hIO* hResIO, hSEQ_IO* hResSeqIO);
tERROR CreateSeqIO(hIO hBaseIO, hSEQ_IO* hResSeqIO);
tERROR CreateNamedIO(hi_MiniArchiver _this, tCHAR* Name, hIO* hResIO,
                     hSEQ_IO* hResSeqIO);
// names related
tBOOL  NextVolumeNewName(tCHAR* Curr, tCHAR* NewName, tUINT Ext);
tERROR GetPrevVolumeName(tCHAR* CurrName, tCHAR* NewName);

// hash related
tERROR CalcCountHash(hHASH hHash, tBYTE* Buff, tUINT Cnt, tQWORD* Seed);
tERROR AddFile(tRAR_DATA* Data, tCHAR* Name, hSEQ_IO Dst, hIO Src,
               tINT Level, tDWORD* WrtSize, tRAR_FILE_HEADER* FH);

tBOOL  RarAskPassword(tRAR_DATA* Data);
tERROR GetBaseHdrPart(hIO hInpIO, tDWORD* FOffs, tBYTE Buff[16]);

static tERROR RAR_DupeIO(hi_MiniArchiver _this, tQWORD ID, hIO* hRes);
static tBOOL  RAR_IsThunk(tRAR_DATA* Data);

                     
// ----------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MiniArchiver". Static(shared) property table variables
static const tVERSION Version = 1;         // must be READ_ONLY at runtime
static const tSTRING  Comment = "RAR";     // must be READ_ONLY at runtime
static const tBOOL    IsSense = cFALSE;    // must be READ_ONLY at runtime
static const tDWORD   MaxName = MAX_PATH;  // must be READ_ONLY at runtime
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(RAR_PropTable)
  mSHARED_PROPERTY_PTR ( pgINTERFACE_VERSION,         Version, sizeof(Version) )
  mSHARED_PROPERTY_VAR ( pgINTERFACE_COMMENT,         Comment, 4               )
  mSHARED_PROPERTY_PTR ( pgNAME_CASE_SENSITIVE,       IsSense, sizeof(IsSense) )
  mSHARED_PROPERTY_PTR ( pgNAME_MAX_LEN,              MaxName, sizeof(MaxName) )
  mSHARED_PROPERTY     ( pgOBJECT_OS_TYPE,           ((tOS_ID)OS_TYPE_GENERIC) )
  
  mLOCAL_PROPERTY_FN   ( pgOBJECT_NAME,               RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_PATH,               RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_FULL_NAME,          RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgIS_SOLID,                  RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_SIZE_Q,             RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_ATTRIBUTES,         RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_HASH,               RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgIS_READONLY,               RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_AVAILABILITY,       RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_ORIGIN,             RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_VOLUME_NAME,        RAR_PropGet, NULL        )

  mLOCAL_PROPERTY_FN   ( pgOBJECT_OPEN_MODE,          RAR_PropGet, RAR_PropSet )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_ACCESS_MODE,        RAR_PropGet, RAR_PropSet )
  mLOCAL_PROPERTY_FN   ( pgMINIARC_PROP_OBJECT_ID,    RAR_PropGet, RAR_PropSet )
  mLOCAL_PROPERTY_FN   ( pgIS_FOLDER,                 RAR_PropGet, RAR_PropSet )
  mLOCAL_PROPERTY_FN   ( pgMULTIVOL_AS_SINGLE_SET,    RAR_PropGet, RAR_PropSet )

  mLOCAL_PROPERTY_FN   ( pgOBJECT_SIZE_Q,             RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_COMPRESSED_SIZE,    RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_COMPRESSION_METHOD, RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_PASSWORD_PROTECTED, RAR_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_SIGNATURE,          RAR_PropGet, NULL        )

  mSHARED_PROPERTY     ( pgOBJECT_NAME_CP,           ((tCODEPAGE)(cCP_OEM))    )
  mSHARED_PROPERTY     ( pgOBJECT_PATH_CP,           ((tCODEPAGE)(cCP_OEM))    )
  mSHARED_PROPERTY     ( pgOBJECT_FULL_NAME_CP,      ((tCODEPAGE)(cCP_OEM))    )
mPROPERTY_TABLE_END(RAR_PropTable)
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "MiniArchiver". Register function
tERROR pr_call RAR_Register( hROOT root ) 
{
  tERROR error;

  PR_TRACE_A0( root, "Enter MiniArchiver::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_MINIARCHIVER,                       // interface identifier
    PID_RAR,                                // plugin identifier
    0x00000000,                             // subtype identifier
    Version,                                // interface version
    VID_GLAV,                               // interface developer
    &i_MiniArchiver_vtbl,                   // internal(kernel called) function table
    (sizeof(i_MiniArchiver_vtbl)/sizeof(tPTR)),// internal function table size
    &e_MiniArchiver_vtbl,                      // external function table
    (sizeof(e_MiniArchiver_vtbl)/sizeof(tPTR)),// external function table size
    RAR_PropTable,                          // property table
    mPROPERTY_TABLE_SIZE(RAR_PropTable),    // property table size
    sizeof(RAR_Data),                       // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"MiniArchiver(IID_MINIARCHIVER) registered [error=0x%08x]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave MiniArchiver::Register method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end

// ##### *** PROPERTYES PROCESSING *** ########################################

// check property presence
static tERROR CheckPropObj(tRAR_DATA* Data, tPROPID Prop, tQWORD Obj)
{
  if ( NULL == Data->Files )
    return(errOBJECT_NOT_CREATED);

  switch(Prop)
  {
  case pgIS_FOLDER:
  case pgOBJECT_PATH:
  case pgOBJECT_NAME:
  case pgOBJECT_SIZE_Q:
  case pgOBJECT_FULL_NAME:
  case pgOBJECT_ATTRIBUTES:
  case pgOBJECT_VOLUME_NAME:
  case pgOBJECT_COMPRESSED_SIZE:
  case pgOBJECT_COMPRESSION_METHOD:
  case pgOBJECT_PASSWORD_PROTECTED:
  case pgOBJECT_SIGNATURE:
    if ( NULL == Data->Files )
      return(errOBJECT_NOT_INITIALIZED);
    if ( Obj > Data->FileNum )
      return(errNOT_FOUND);
    break;
  }

  return(errOK);
}

#define OBJECT_SIGN_SIZE  16  // size of unique file id

// get plugin property
tERROR pr_call RAR_PropGet(hi_MiniArchiver _this, tDWORD* out_size, 
			   tPROPID prop, tCHAR* buffer, tDWORD size)
{
  tDWORD Size;
  tERROR Error;
  tQWORD CurProp;
  tRAR_DATA* Data;
  tCHAR* Name;
  tCHAR* NPtr;
  tINT   NLen;
  PR_TRACE_A1(_this, "Enter *GET* method RAR_PropGet for property %u", prop);
    
  Size  = 0;
  Error = (errOK);
  Data  = (tRAR_DATA*)(_this->data);
  if ( NULL != buffer && 0 != size )
  {
    // check current property ptr
    if ( Data->CurrProp == LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF) )
    {
      Data->CurrProp = Data->CurrFile;
    }
    else
    {
      if ( Data->CurrProp != 0 )
        Data->CurrProp = (Data->CurrProp - 1);
    }
    
    CurProp = Data->CurrProp;
    switch( prop )
    {
    case pgOBJECT_NAME:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;

      // parse name (find slash)
      Name = (tCHAR*)(&Data->Files[Data->CurrProp].Parts[0].FH.name[0]);
      NPtr = &Name[NLen = _toi32(strlen(Name))];
      while( NPtr > Name && (NPtr[-1] != '\\' && NPtr[-1] != '/') )
        --NPtr;

      // save name part
      Size = _toui32(NLen - (NPtr - &Name[0]) + 1);
      if ( Size > size )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      memcpy(buffer, NPtr, Size);
      break;
    case pgOBJECT_FULL_NAME:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;

      Name = Data->Files[Data->CurrProp].Parts[0].FH.name;
      Size = _toui32(strlen(Name) + 1);
      if ( Size > size )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      memcpy(buffer, Name, Size);
      break;
    case pgOBJECT_VOLUME_NAME:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;

      Name = Data->Files[Data->CurrProp].Parts[0].Volume->VolName;
      Size = _toui32(strlen(Name) + 1);
      if ( Size > size )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      memcpy(buffer, Name, Size);
      break;
    case pgOBJECT_PATH:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;

      // parse name (find slash)
      Name = (tCHAR*)(&Data->Files[Data->CurrProp].Parts[0].FH.name[0]);
      NPtr = &Name[strlen(Name)];
      while( NPtr > Name && (NPtr[0] != '\\' && NPtr[0] != '/') )
        --NPtr;

      Size = _toui32(NPtr - &Name[0]);
      if ( (Size + 1) > size )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }

      memcpy(buffer, Name, Size);
      buffer[Size] = '\0';
      break;
    case pgOBJECT_SIZE_Q:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size  = sizeof(tQWORD);
      if ( size < sizeof(tQWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tQWORD*)(buffer) = (tQWORD)(Data->Files[Data->CurrProp].FullSize);
      break;
    case pgOBJECT_ATTRIBUTES:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = sizeof(tDWORD);
      if ( size < sizeof(tDWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tDWORD*)(buffer) = Data->Files[Data->CurrProp].Parts[0].FH.file_attr;
      break;
    case pgOBJECT_HASH:
      Size = sizeof(tQWORD);
      if ( size < sizeof(tQWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tQWORD*)(buffer) = (tQWORD)(Data->VolHash);
      break;
    case pgIS_READONLY:
      Size = sizeof(tBOOL);
      if ( size < sizeof(tBOOL) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      if ( cFALSE == Data->FullRO )
      {
        // return prop for current file
        if ( CurProp < Data->FileNum )
        {
          *(tBOOL*)(buffer) = Data->Files[CurProp].ReadOnly;
        }
        else
        {
          *(tBOOL*)(buffer) = (cTRUE);
        }
      }
      else
      {
        // whole file readonly
        *(tBOOL*)(buffer) = (cTRUE);
      }
      break;
    case pgOBJECT_AVAILABILITY:
      Size = sizeof(tDWORD);
      if ( size < sizeof(tDWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tDWORD*)(buffer) = Data->Avail;
      break;
    case pgMULTIVOL_AS_SINGLE_SET:
      Size = sizeof(tBOOL);
      if ( size < sizeof(tBOOL) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tBOOL*)(buffer) = (tBOOL)(Data->AsSingle);
      break;
    case pgOBJECT_OPEN_MODE:
      Size = sizeof(tDWORD);
      if ( size < sizeof(tDWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tDWORD*)(buffer) = (tDWORD)(Data->OpenMode);
      break;
    case pgOBJECT_ACCESS_MODE:
      Size = sizeof(tDWORD);
      if ( size < sizeof(tDWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tDWORD*)(buffer) = (tDWORD)(Data->AccessMode);
      break;
    case pgIS_SOLID:
      Size = sizeof(tBOOL);
      if ( size < sizeof(tBOOL) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tBOOL*)(buffer) = (tBOOL)(Data->RAR.MHD_Flags & MHD_SOLID);
      break;
    case pgIS_FOLDER:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = sizeof(tBOOL);
      if ( size < sizeof(tBOOL) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tBOOL*)(buffer) = (tBOOL)(Data->Files[Data->CurrProp].IsFolder);
      break;
    case pgOBJECT_ORIGIN:
      Size = sizeof(tORIG_ID);
      if ( size < sizeof(tORIG_ID) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tORIG_ID*)(buffer) = (tORIG_ID)(OID_GENERIC_IO);
      break;
    case pgOBJECT_COMPRESSED_SIZE:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = sizeof(tDWORD);
      if ( size < sizeof(tDWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tDWORD*)(buffer) = (tDWORD)(Data->Files[Data->CurrProp].Parts->FH.psize);
      break;
    case pgOBJECT_COMPRESSION_METHOD:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = sizeof(tDWORD);
      if ( size < sizeof(tDWORD) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tDWORD*)(buffer) = (tDWORD)(Data->Files[Data->CurrProp].Parts->FH.method);
      break;
    case pgOBJECT_PASSWORD_PROTECTED:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = sizeof(tBOOL);
      if ( size < sizeof(tBOOL) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tBOOL*)(buffer) = (tBOOL)((Data->Files[Data->CurrProp].Parts->FH.base.flags & LHD_PASSWORD) != 0);
      break;
    case pgOBJECT_SIGNATURE:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = OBJECT_SIGN_SIZE;
      if ( size < OBJECT_SIGN_SIZE )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      ((tDWORD*)(buffer))[0] = Data->Files[Data->CurrProp].Parts->FH.base.crc;
      ((tDWORD*)(buffer))[1] = Data->Files[Data->CurrProp].Parts->FH.usize;
      ((tDWORD*)(buffer))[2] = ((tDWORD*)&Data->Files[Data->CurrProp].Parts->FH.salt)[0];
      ((tDWORD*)(buffer))[3] = ((tDWORD*)&Data->Files[Data->CurrProp].Parts->FH.salt)[1];
      break;
    default:
      Error = (errPROPERTY_NOT_IMPLEMENTED);
    }
  }
  else if ( NULL == buffer && 0 == size ) 
  {
    switch( prop )
    {
    case pgOBJECT_NAME:
      Size = (MAX_OBJ_NAME);
      break;
    case pgOBJECT_FULL_NAME:
      Size = (MAX_OBJ_FULLNAME);
      break;
    case pgOBJECT_PATH:
      Size = (MAX_OBJ_PATH);
      break;
    case pgOBJECT_VOLUME_NAME:
      Size = (MAX_OBJ_VOLNAME);
      break;
    case pgOBJECT_HASH:
    case pgOBJECT_SIZE_Q:
      Size = sizeof(tQWORD);
      break;
    case pgMULTIVOL_AS_SINGLE_SET:
    case pgIS_READONLY:
    case pgIS_FOLDER:
    case pgIS_SOLID:
    case pgOBJECT_PASSWORD_PROTECTED:
      Size = sizeof(tBOOL);
      break;
    case pgOBJECT_OPEN_MODE:
    case pgOBJECT_ATTRIBUTES:
    case pgOBJECT_ACCESS_MODE:
    case pgOBJECT_AVAILABILITY:
    case pgOBJECT_COMPRESSED_SIZE:
    case pgOBJECT_COMPRESSION_METHOD:
      Size = sizeof(tDWORD);
      break;
    case pgOBJECT_ORIGIN:
      Size = sizeof(tORIG_ID);
      break;
    case pgOBJECT_SIGNATURE:
      Size = OBJECT_SIGN_SIZE;
      break;
    default:
      Error = (errPROPERTY_NOT_IMPLEMENTED);
    }
  }
  else
  {
    Size  = 0;
    Error = (errPARAMETER_INVALID);
  }

loc_exit:
  if ( NULL != out_size )
    *out_size = Size;
  Data->CurrProp = LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF);
  PR_TRACE_A3(_this, "Leave *GET* method RAR_PropGet for property %u, ret tDWORD = %u(size), error = 0x%08x", prop, Size, Error);
  return(Error);
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_PropSet(hi_MiniArchiver _this, tUINT* out_size,
			   tPROPID prop, tCHAR* buffer, tUINT size)
{
  tDWORD Size;
  tERROR Error;
  tRAR_DATA*  Data;
  PR_TRACE_A1(_this, "Enter *SET* method RAR_PropSet for property %u", prop);

  Size  = 0;
  Error = (errOK);
  Data  = (tRAR_DATA*)(_this->data);
  if ( NULL != buffer && 0 != size )
  {
    switch( prop )
    {
    case pgMINIARC_PROP_OBJECT_ID:
      Data->CurrProp = *(tQWORD*)(buffer);
      break;
    case pgMULTIVOL_AS_SINGLE_SET:
      Data->AsSingle = *(tBOOL*)(buffer);
      break;
    case pgOBJECT_OPEN_MODE:
      Data->OpenMode = *(tDWORD*)(buffer);
      break;
    case pgOBJECT_ACCESS_MODE:
      Data->AccessMode = *(tDWORD*)(buffer);
      break;
    default:
      Error = (errPROPERTY_NOT_IMPLEMENTED);
      goto loc_exit;
    }
  }
  else if ( NULL == buffer && 0 == size ) 
  {
    switch( prop )
    {
    case pgMINIARC_PROP_OBJECT_ID:
      Size = sizeof(tQWORD);
      break;
    case pgMULTIVOL_AS_SINGLE_SET:
      Size = sizeof(tBOOL);
      break;
    case pgOBJECT_OPEN_MODE:
    case pgOBJECT_ACCESS_MODE:
      Size = sizeof(tDWORD);
      break;
    default:
      Error = (errPROPERTY_NOT_IMPLEMENTED);
      goto loc_exit;
    }
  }
  else
  {
    Size  = 0;
    Error = (errPARAMETER_INVALID);
  }

loc_exit:
  if ( NULL != out_size )
    *out_size = Size;
  PR_TRACE_A3(_this, "Leave *SET* method RAR_PropSet for property %u, ret tDWORD = %u(size), error = 0x%08x", prop, Size, Error);
  return(Error);
}

// ##### *** MINIARCHIVER METHODS *** #########################################

tERROR pr_call RAR_Recognize( hOBJECT _that )
{
  tERROR Error;
  tBYTE  Mark[8];
  PR_TRACE_A2(0,"Enter MiniArchiver::Recognize method for object %p (iid=%u)",
    _that, CALL_SYS_PropertyGetDWord(_that, pgINTERFACE_ID) );

#if 0
  {
    tRAR_DATA* Data = (tRAR_DATA*)(0);
    Error = (tBYTE*)(&Data->RAR.Password[0]) - (tBYTE*)(&Data->ArcFull[0]);
  }
#endif
  
  /* read main signature */
  if ( errOK != CALL_IO_SeekRead((hIO)(_that), NULL, 0, Mark, sizeof Mark) )
  {
    Error = (errINTERFACE_INCOMPATIBLE);
    goto loc_exit;
  }

  /* check main archive signature */
  if ( Mark[0] != 'R' || Mark[1] != 'a' || Mark[2] != 'r' ||
       memcmp(&Mark[3], &RAR_SIGN_PART[0], sizeof(RAR_SIGN_PART)) )
  {
    Error = (errINTERFACE_INCOMPATIBLE);
    goto loc_exit;
  }
  else
  {
    Error = (errOK);
    goto loc_exit;
  }

loc_exit:
  PR_TRACE_A2(0,"Leave MiniArchiver::Recognize method for object %p (iid=%u)",
    _that, CALL_SYS_PropertyGetDWord(_that, pgINTERFACE_ID) );
  return(Error);
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_ObjectInit(hi_MiniArchiver _this)
{
  tINPUT* Input;
  tRAR*  RAR;
  tERROR Error;
  tQWORD Hash;
  hHASH  hHash;
  hIO    hArcIO;
  tBYTE  Buff[0x20];
  tBYTE  RDBuff[0x20];
  tWORD  MHDFlags;
  tBYTE  MHDSalt[SALT_SIZE];
  tDWORD BCnt;
  tBASE_HDR  BHdr;
  tRAR_DATA* Data;
  hSEQ_IO hSeqIO;
  
  PR_TRACE_A2(0,"Enter MiniArchiver::ObjectInit method for object %p (iid=%u)",
    _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

  Data = (tRAR_DATA*)(_this->data);
  memset(Data, 0, sizeof(tRAR_DATA));
  RAR = &Data->RAR;
  hSeqIO = (NULL);

  // get archive io
  if ( NULL == (hArcIO = (hIO)CALL_SYS_ParentGet((hOBJECT)(_this), IID_IO)) )
  {
    Error = (errOBJECT_CANNOT_BE_INITIALIZED);
    goto loc_exit;
  }

  // create hash
  Hash = (tQWORD)(0);
  memset(&Buff[0], 0, sizeof(Buff));
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hHash,
    IID_HASH, PID_HASH_MD5, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  // create password container
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &Data->hPswd,
    IID_STRING, PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  // link crypt ctx with password container
  Data->RAR.Crypt.hPswd = Data->hPswd;

  // create seq_io
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(hArcIO, &hSeqIO,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  Data->hArcIO = hArcIO;
  Data->RAR.hSeqIn = hSeqIO;

  // read archive main header
  memset(&Buff[0], 0, sizeof(Buff));
  Error = CALL_IO_SeekRead(hArcIO, NULL, 7, &Buff, sizeof(tBASE_HDR));
  if ( PR_FAIL(Error) )
    goto loc_exit;
 
  // get main archive flags
  MHDFlags = ((tBASE_HDR*)(&Buff[0]))->flags;

  // init archiver engine
  RAR->LastError = (errOK);
  RAR->hThis = (hOBJECT)(_this);
  Input = (tINPUT*)(&RAR->infile);
  RAR->NextBlock = 0;
  Data->RAR.TotPSize = LONG_LONG_CONST(0xffffffffffffffff);
  InputInitFile(Input, RAR, &RDBuff[0], sizeof(RDBuff));
  Data->hUniArcIO = (hIO)CALL_SYS_ParentGet((hOBJECT)_this, IID_OS);

psw_retry1: // retry password
  if ( (MHDFlags & MHD_PASSWORD) )
  {
    if ( cFALSE == RarAskPassword(Data) )
    {
      Error = (errOBJECT_PASSWORD_PROTECTED);
      goto loc_exit; // password needed
    }
  }

  // try parse all headers
  BCnt = 0;
  while(cTRUE)
  {
    // setup decryptor
    RAR->Decrypt = (cFALSE);
    RarSeek(&RAR->infile, RAR->NextBlock);
    if ( (MHDFlags & MHD_PASSWORD) &&
         (RAR->NextBlock >= (SIZEOF_MARKHEAD + SIZEOF_NEWMHD)) )
    {
      // read salt for headers decryptor
      PackRead(RAR, MHDSalt, SALT_SIZE);
      RAR_SetEncryption(RAR, 30, &RAR->Password[0], MHDSalt);
      RAR->Decrypt = (cTRUE);
      RarSeek(Input, RAR->NextBlock + SALT_SIZE);
    }

    // read base block header
    if ( PR_FAIL(Error = ReadBaseHdr(Input, &BHdr)) )
    {
      if ( errOPERATION_CANCELED == RAR->LastError )
        return(errOPERATION_CANCELED);

      if ( Input->eof || cFALSE == RAR->Decrypt )
        break; // end of file, stop read

      goto psw_retry1; // try next password
    }

    if ( errOPERATION_CANCELED == RAR->LastError )
      return(errOPERATION_CANCELED);

    // calc block hash
    memset(Buff, 0, sizeof(Buff));
    *(tWORD*) (&Buff[0])  = BHdr.crc;
    *(tBYTE*) (&Buff[2])  = BHdr.type;
    *(tWORD*) (&Buff[3])  = BHdr.flags;
    *(tWORD*) (&Buff[5])  = BHdr.size;
    *(tDWORD*)(&Buff[7])  = BHdr.hdr_size;
    *(tDWORD*)(&Buff[11]) = BHdr.extra_size;
    *(tDWORD*)(&Buff[15]) = RAR->NextBlock;
    *(tDWORD*)(&Buff[19]) = BCnt++;
    if ( PR_FAIL(Error = CalcCountHash(hHash, &Buff[0], 0x20, &Hash)) )
      goto loc_exit;

    if ( BHdr.type == FILE_HEAD || BHdr.type == NEWSUB_HEAD )
    {
      // additional hash for file blocks
      memset(Buff, 0, sizeof(Buff));
      *(tDWORD*)(&Buff[0])  = RarReadDword(Input);
      *(tBYTE*) (&Buff[4])  = RarReadByte(Input);
      *(tDWORD*)(&Buff[5])  = RarReadDword(Input);
      *(tDWORD*)(&Buff[9])  = RarReadDword(Input);
      *(tBYTE*) (&Buff[13]) = RarReadByte(Input);
      *(tBYTE*) (&Buff[14]) = RarReadByte(Input);
      *(tWORD*) (&Buff[15]) = RarReadWord(Input);
      *(tDWORD*)(&Buff[17]) = RarReadDword(Input);
      if ( PR_FAIL(Error = CalcCountHash(hHash, &Buff[0], 0x20, &Hash)) )
        goto loc_exit;
    }

    if ( BHdr.type == ENDARC_HEAD )
      break; // done scan

    // go to next block
    if ( cFALSE != RAR->Decrypt )
      RAR->NextBlock += ALIGN(BHdr.size + BHdr.extra_size, 16) + SALT_SIZE;
    else
      RAR->NextBlock += (BHdr.size + BHdr.extra_size);
  }

  // save needed data
  Data->hMD5    = hHash;
  Data->VolHash = Hash;
  Error = (errOK);

loc_exit:
  if ( NULL != hSeqIO )
  {
    CALL_SYS_ObjectClose(hSeqIO);
    Data->RAR.hSeqIn = (NULL);
  }
  PR_TRACE_A2(0, "Leave MiniArchiver::ObjectInit method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
  return(Error);
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_ObjectInitDone(hi_MiniArchiver _this)
{
  hIO hArcIO;
  hOS OS;
  tUINT  I;
  tDWORD Len;
  tERROR Error;
  hObjPtr hPtr;
  hOBJECT IO;
  tRAR_DATA* Data;
  PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectInitDone method" );

  Error = (errOK);
  Data  = (tRAR_DATA*)(_this->data);
  Data->LastUnp = (tQWORD)LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF);
  Data->RAR.hThis  = (hOBJECT)(_this);
  Data->ErrInSolid = (cFALSE);

#ifdef DEBUG_PASS_LOG
  Data->FPasLog = fopen("c:\\rar_pass.log", "a+t");
#endif

  // create working heap
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &Data->hHeap,
    IID_HEAP, PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  hArcIO = Data->hArcIO;
  Data->OpenMode   = CALL_SYS_PropertyGetDWord(hArcIO, pgOBJECT_OPEN_MODE);
  Data->AccessMode = CALL_SYS_PropertyGetDWord(hArcIO, pgOBJECT_ACCESS_MODE);

  hPtr = (NULL);
  Data->hPtr = NULL;
  if( NULL != (OS = (hOS)CALL_SYS_ParentGet(hArcIO, IID_OS)) )
  {
    IO = (hOBJECT)(hArcIO);
    while ( PR_FAIL(Error = CALL_OS_PtrCreate(OS, &hPtr, IO)) )
    {
      if ( NULL == (IO = CALL_SYS_ParentGet(IO, IID_IO)) )
      {
        hPtr = (NULL);
        break;
      }
    }
    if ( NULL != hPtr )
    {
      Data->hPtr = hPtr;
      Data->hOrigIO = IO;
    }
  }
  else
  {
    if( NULL != (hPtr = (hObjPtr)CALL_SYS_ParentGet(hArcIO, IID_OBJPTR)) )
    {
      if ( PR_SUCC(CALL_ObjPtr_Clone(hPtr, &hPtr)) )
      {
        IO = (hOBJECT)(hArcIO);
        while ( PR_FAIL(Error = CALL_ObjPtr_ChangeTo(hPtr, IO)) )
        {
          if ( (IO = CALL_SYS_ParentGet(IO, IID_IO)) == NULL )
          {
            CALL_SYS_ObjectClose(hPtr);
            hPtr = NULL;
            break;
          }
        }

        if ( NULL != hPtr )
        {
          Data->hPtr = hPtr;
          Data->hOrigIO = IO;
        }
      }
    }
    else
    {
      if ( PID_NATIVE_FIO == (tPID)CALL_SYS_PropertyGetDWord((hOBJECT)hArcIO, pgPLUGIN_ID) )
      {
        if ( PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &Data->hPtr, IID_OBJPTR, PID_NATIVE_FIO, SUBTYPE_ANY)) )
        {
          if (PR_FAIL(CALL_ObjPtr_ChangeTo(Data->hPtr, (hOBJECT)hArcIO)))
          {
            CALL_SYS_ObjectClose(Data->hPtr);
            Data->hPtr = NULL;
          }
        }
      }
    }
  }

  // init engine
  if ( PR_FAIL(Error = InitArchive(_this)) )
  {
    goto loc_exit;
  }

  // check for thunks
  if ( cFALSE != (Data->IsThunk = RAR_IsThunk(Data)) )
  {
    Data->FileNum = 0;           // no files in thunk
    Data->NotFullSet = (cTRUE);  // incomplete volumes set
  }

  if ( cFALSE == Data->FullRO )
  {
    Data->Avail = (fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND |
      fAVAIL_TRUNCATE | fAVAIL_CHANGE_MODE | fAVAIL_DELETE_ON_CLOSE);
  }
  else
  {
    Data->Avail = (fAVAIL_READ);
  }

  // send volumes hashes
  for ( I = 0; I < Data->VolNum; I++ )
  {
    Len = sizeof(tQWORD);
    if ( PR_FAIL(Error = CALL_SYS_SendMsg(_this, pmc_OS_VOLUME,
      pm_OS_VOLUME_HASH,Data->Vols[I].hVolIO,&Data->Vols[I].VolHash,&Len)) )
    {
      goto loc_exit;
    }
  }

  // send incomplete volumes message
  if ( cFALSE != Data->NotFullSet )
  {
    if ( PR_FAIL(Error = CALL_SYS_SendMsg(_this, pmc_OS_VOLUME,
      pm_OS_VOLUME_SET_INCOMPLETE, NULL, NULL, NULL)) )
    {
      goto loc_exit;
    }
  }

loc_exit:
  PR_TRACE_A1(_this, "Leave MiniArchiver::ObjectInitDone method, ret tERROR = 0x%08x", Error);
  return(Error);
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_Reset(hi_MiniArchiver _this)
{
  tRAR_DATA* Data;
  tERROR    Error;
  PR_TRACE_A0( _this, "Enter MiniArchiver::Reset method");

  Error = (errOK);
  Data = (tRAR_DATA*)(_this->data);
  Data->CurrFile = (tQWORD)(0);
  Data->CurrProp = (tQWORD)LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF);

  PR_TRACE_A1(_this, "Leave MiniArchiver::Reset method, ret tERROR = 0x%08x", Error);
  return(Error);
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_Next(hi_MiniArchiver _this, tQWORD* result, 
                               tQWORD qwLastObjectId) 
{
  tQWORD Result;
  tERROR Error;
  tRAR_DATA* Data;
  PR_TRACE_A0( _this, "Enter MiniArchiver::Next method" );

  Result = 0;
  Error  = (errOK);
  if( NULL == result)
  {
    Error = (errPARAMETER_INVALID);
    goto loc_exit;
  }

  Data = (tRAR_DATA*)(_this->data);
  if ( qwLastObjectId >= Data->FileNum )
  {
    Result = (0);
    Error = (errEND_OF_THE_LIST);
    goto loc_exit;
  }

  Data->CurrFile = qwLastObjectId;
  Result = (qwLastObjectId + 1);

loc_exit:
  if ( NULL != result )
    *result = Result;
  PR_TRACE_A2(_this, "Leave MiniArchiver::Next method, ret tQWORD = %I64u, error = 0x%08x", Result, Error);
  return(Error);
}

// ----------------------------------------------------------------------------

#ifdef DEBUG_DUMP
static tVOID DG_DumpIO(hIO IO, tCHAR* FName)
{
  hSEQ_IO hSeqIO;
  tBYTE  Buff[2048];
  FILE*  F;
  tUINT  Rdt;
  tCHAR* Name;

  CreateSeqIO(IO, &hSeqIO);
  CALL_SeqIO_Seek(hSeqIO, NULL, 0, cSEEK_SET);

  Name = &FName[strlen(FName)];
  while( Name >= FName && *Name != '\\' && *Name != '/' )
    Name--;

  Name++;
  if ( NULL == (F = fopen(Name, "wb")) )
    return; 
  fseek(F, 0, SEEK_SET);

  while(1)
  {
    Rdt = 0;
    if ( !PR_SUCC(CALL_SeqIO_Read(hSeqIO, &Rdt, &Buff[0], sizeof(Buff))) )
      break;
    if ( Rdt == 0 )
      break;
    fwrite(&Buff[0], Rdt, 1, F);
  }

  fclose(F);
  CALL_SYS_ObjectClose(hSeqIO);
}
#else
# define DG_DumpIO(a,b)
#endif

// ----------------------------------------------------------------------------

tERROR pr_call RAR_ObjCreate(hi_MiniArchiver _this, hIO* result,
			     tQWORD qwObjectIdToCreate, hIO hUniArcIO)
{
  tFILE_NODE* Node;
  tRAR_DATA*  Data;
  hSEQ_IO  hSeqOut;
  tERROR     Error;
  tBOOL  CanDelete;
  hIO       Result;
  PR_TRACE_A0( _this, "Enter MiniArchiver::ObjCreate method" );

  Result  = NULL;
  hSeqOut = NULL;
  Error   = errOK;
  if ( NULL == result )
  {
    Error = errPARAMETER_INVALID;
    goto loc_exit;
  }

  // find file node
  Data = (tRAR_DATA*)(_this->data);
  if ( qwObjectIdToCreate <= 0 || qwObjectIdToCreate > Data->FileNum )
  {
    Error = (errOBJECT_NOT_FOUND);
    goto loc_exit;
  }
  
  // create output io
  if ( PR_FAIL(Error = CreateTempIO(_this, &Result)) )
  {
    goto loc_exit;
  }

  Node = &Data->Files[qwObjectIdToCreate - 1];

#ifdef DEBUG_PASS
  Data->RAR.PswCnt = 0;
#endif
#ifdef DEBUG_PASS_LOG
  if ( Data->FPasLog )
  {
    fprintf(Data->FPasLog, " -> RAR_ObjCreate(): \"%s\"\n", Node->Parts->FH.name);
    fflush(Data->FPasLog);
  }
#endif

  // check for already unpacked
  if ( Node->TmpOffs != (tQWORD)LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF) )
  {
    if ( PR_FAIL(Error = GetTempFile(Data, Result, Node)) ||
         PR_FAIL(Error = CALL_IO_Flush(Result)) )
    {
      goto loc_exit;
    }

    goto loc_exit; // got from temp file
  }

  if ( cFALSE != Data->ErrInSolid )
  {
    struct sRAR_FILE_HEADER* fh;

    // can't process solid arc after error
    fh = &Data->Files[qwObjectIdToCreate-1].Parts->FH;
    if ( fh->base.flags & LHD_PASSWORD )
      Error = (errOBJECT_PASSWORD_PROTECTED); // invalid password
    else if ( Data->RAR.InvalidCrc )
    {
      /* make fake output */
      if ( PR_FAIL(Error = CreateSeqIO(Result, &hSeqOut)) )
        goto loc_exit;
      
      Error = warnOBJECT_DATA_CORRUPTED;
    }
    else
      Error = (errOBJECT_INVALID); // corrupt
    goto loc_exit;
  }

  // create seq_io
  if ( PR_FAIL(Error = CreateSeqIO(Result, &hSeqOut)) )
  {
    goto loc_exit;
  }

#if 0 /* print full name */
  printf("%s\n", Node->Parts->FH.name);
  if ( strstr(Node->Parts->FH.name, "Word97\\Marker\\MARKER.D2") )
    _asm nop
#endif
  
  // unpack file
  Data->hUniArcIO = hUniArcIO;
  Error = RarUnpackFile(Data, qwObjectIdToCreate-1, hSeqOut);
  CALL_SYS_ObjectClose(hSeqOut); hSeqOut = NULL;
  if ( PR_FAIL(Error) || Error == warnOBJECT_DATA_CORRUPTED )
  {
#ifdef DEBUG_DUMP
    printf(" *** ERROR DUMPING: %s *** \n", Node->Parts[0].FH.name);
    DG_DumpIO(Result, Node->Parts[0].FH.name);
#endif
    if ( Data->RAR.MHD_Flags & MHD_SOLID )
    {
      Data->ErrInSolid = (cTRUE);
      Data->FullRO = (cTRUE);
    }
    else if ( Error != errOBJECT_PASSWORD_PROTECTED )
    {
      Data->FullRO = (cTRUE);
    }
    goto loc_exit;
  }
  
#ifdef DEBUG_DUMP
    printf(" *** DEBUG DUMPING: %s *** \n", Node->Parts[0].FH.name);
    DG_DumpIO(Result, Node->Parts[0].FH.name);
#endif

  // can delete ?
  if ( NULL != Result )
  {
    // set availability for io
    if ( Data->FullRO || Node->ReadOnly )
      CanDelete = (cFALSE);
    else
      CanDelete = (cTRUE);

    if ( PR_FAIL(Error = CALL_SYS_PropertySet(Result, 0,
      pgOBJECT_DELETE_ON_CLOSE, &CanDelete, sizeof(tBOOL))) )
    {
      goto loc_exit;
    }
  }

#ifdef DEBUG_DUMP
//  printf(" *** DUMPING: %s *** \n", Node->Parts[0].FH.name);
//  DG_DumpIO(Result, Node->Parts[0].FH.name);
#endif

loc_exit:
  if ( Error != errOK && Error != warnOBJECT_DATA_CORRUPTED )
  {
    /* close bad io's */
    if ( NULL != hSeqOut )
    {
      CALL_SYS_ObjectClose(hSeqOut);
      hSeqOut = NULL;
    }
    if ( NULL != Result )
    {
      CALL_SYS_ObjectClose(Result);
      Result = NULL;
    }
  }
  if ( NULL != result )
    *result = Result;
  PR_TRACE_A2(_this, "Leave MiniArchiver::ObjCreate method, ret hIO = %p, error = 0x%08x", Result, Error);
  return(Error);
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_SetAsCurrent(hi_MiniArchiver _this,tQWORD qwObjectId)
{
  tERROR Error;
  tRAR_DATA* Data;
  PR_TRACE_A0( _this, "Enter MiniArchiver::SetAsCurrent method" );

  Error = (errOK);
  Data = (tRAR_DATA*)(_this->data);
  Data->CurrFile = (qwObjectId);

  PR_TRACE_A1(_this, "Leave MiniArchiver::SetAsCurrent method, ret tERROR = 0x%08x", Error);
  return(Error);;
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_ObjectPreClose(hi_MiniArchiver _this)
{
  tRAR_DATA* Data;

  Data = (tRAR_DATA*)(_this->data);
  if ( NULL != Data->hPtr )
  {
    CALL_SYS_ObjectClose(Data->hPtr);
    Data->hPtr = (NULL);
  }
  if ( NULL != Data->hHeap )
  {
    CALL_SYS_ObjectClose(Data->hHeap);
    Data->hHeap = (NULL);
  }

#ifdef DEBUG_PASS_LOG
  if ( Data->FPasLog )
  {
    fprintf(Data->FPasLog, "RAR_ObjectPreClose()\n\n");
    fclose(Data->FPasLog);
    Data->FPasLog = NULL;
  }
#endif

  return(errOK);
}

// ----------------------------------------------------------------------------

tERROR pr_call RAR_RebuildArchive(hi_MiniArchiver _this,
				  hUNIARCCALLBACK hCallBack,
				  hIO hOutputIo)
{
  tRAR_DATA* Data;
  tDWORD  I, FCnt;
  tERROR  Error;
  tFILE_NODE*  FNode;
  tBLOCK_NODE* BNode;
  hNEWOBJECT hNewObj;
  hSEQ_IO hSeqOut;
  hIO     hNewIO;
  hIO     hInpIO;
  tBOOL   EndArc;
  tQWORD  OutOffs;
  tCHAR*  Name;
  tINT    Level;
  tDWORD  Wrt;
  tBOOL   Dupe, Pack;
  tCHAR   NewName[MAX_NAME+8];
  PR_TRACE_A0( _this, "Enter MiniArchiver::RebuildArchive method" );

  Error = (errOK);
  Data  = (tRAR_DATA*)(_this->data);
  if ( Data->FullRO )
  {
    Error = (errNOT_OK);
    goto loc_exit; /* don't heal corrupt arc */
  }

  // create output seq_io
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(hOutputIo, &hSeqOut,
         IID_SEQIO, PID_ANY, 0)) ||
       PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, 0, cSEEK_SET)) )
  {
    goto loc_exit;
  }

  EndArc = (cFALSE);

  // enumerate and write archive blocks
  for ( I = 0, FCnt = 0, OutOffs = 0; I < Data->BlkNum; I++ )
  {
    BNode = &Data->Blks[I];
    if ( BNode->BHdr.type == ENDARC_HEAD )
    {
      EndArc = (cTRUE);
      break; // stop processing
    }
    else if ( BNode->BHdr.type == MAIN_HEAD &&
             (Data->RAR.MHD_Flags & MHD_SOLID) )
    {
      // write new main header to solid archive
      CALL_SeqIO_Seek(hSeqOut, NULL, OutOffs, cSEEK_SET);
      if ( PR_FAIL(Error = CALL_SeqIO_Write(hSeqOut,&Wrt,(tBYTE*)(RAR_OLDMHD),
        sizeof(RAR_OLDMHD))) )
      {
        return(Error);
      }
      OutOffs += sizeof(RAR_OLDMHD);
      continue;
    }

    if ( BNode->BHdr.type == FILE_HEAD )
    {
      if ( FCnt >= Data->FileNum )
        return(errUNEXPECTED);

      FNode = &Data->Files[FCnt++]; 
      Name  = FNode->Parts->FH.name;

      // check if present in deleted list
      if( PR_SUCC(Error = CALL_UniArchiverCallback_FindObjectInList(hCallBack,
            (hOBJECT*)(&hInpIO), cDELETED_OBJECT, FCnt)) )
      {
        // skip block
        continue;
      }

      Pack  = cFALSE;
      Dupe  = cFALSE;

      // check for presence in modifyed list, or if solid
      if( PR_SUCC(Error = CALL_UniArchiverCallback_FindObjectInList(hCallBack,
        (hOBJECT*)(&hInpIO), cCHANGED_OBJECT, FCnt)) )
      {
        Pack = (cTRUE);
      }
      else if ( FNode->HdrFlag & RAR_FLG_HDRCMT )
      {
        /* it's archive comment */
      }
      else if ( !FNode->IsFolder && (Data->RAR.MHD_Flags & MHD_SOLID) )
      {
        Pack = (cTRUE);
        Dupe = (cTRUE);
      }

      if ( Pack )
      {
        if ( FNode->Parts->FH.method == 0x30 || FNode->Parts->FH.unp_ver < 20 )
          Level = 0; // store
        else
          Level = 1; // pack fastest

        // get io if needed
        if ( cFALSE != Dupe )
        {
          if ( PR_FAIL(Error = RAR_DupeIO(_this, (FCnt-1), &hInpIO)) )
            return(Error);
        }
        
        Wrt = 0; // repack it
        if ( PR_FAIL(Error = AddFile(Data, Name, hSeqOut, hInpIO, Level,
          &Wrt, &FNode->Parts->FH)) )
        {
          goto loc_exit;
        }

        if( NULL != hInpIO )
        {
          CALL_SYS_ObjectClose(hInpIO);
          hInpIO = (NULL);
        }

        OutOffs += Wrt;
        continue;
      }
    }

#if 0  // copy block
    if ( PR_FAIL(Error = CopySeqIOData(Data, BNode->Vol->hVolSeqIO,
      BNode->BlkOffs, hSeqOut, OutOffs, BNode->BlkSize)) )
    {
      goto loc_exit;
    }
#else
    {
      tUINT  Count = BNode->BlkSize;
      tDWORD IOfs  = BNode->BlkOffs;
      tUINT  Size  = 0;
      tBYTE* Buff  = Data->pTmpBuff;
      tDWORD Rdt;

      Error = CALL_SeqIO_Seek(hSeqOut, NULL, OutOffs, cSEEK_SET);
      if ( errOK != Error )
        return(errOBJECT_INVALID);
      while( Count > 0 )
      {
        Rdt  = 0;
        Size = (Count > TMP_BUFF_SIZE) ? (TMP_BUFF_SIZE) : (Count);
        Error = CALL_IO_SeekRead(Data->hArcIO, &Rdt, IOfs, Buff, Size);
        if ( errOK != Error || Rdt != Size )
          return(errOBJECT_INVALID);
        Error = CALL_SeqIO_Write(hSeqOut, NULL, Buff, Size);
        if ( errOK != Error )
          return(errOBJECT_INVALID);
        IOfs += Size;
        Count = (Count - Size);
      }
    }
#endif

    OutOffs += BNode->BlkSize;
  }

  // check for new files
  if(PR_SUCC(CALL_UniArchiverCallback_GetFirstNewObject(hCallBack, &hNewObj)))
  {
    do
    {
      NewName[0] = '\0';
      if ( PR_FAIL(Error = CALL_UniArchiverCallback_GetNewObjectIo(hCallBack,
             (hOBJECT*)(&hNewIO), hNewObj)) )
      {
        goto loc_exit;
      }

      CALL_SYS_PropertyGetStr(hNewIO, 0, pgOBJECT_NAME, &NewName[0], MAX_NAME, cCP_OEM);

      Level = 1; // pack fastest
      if ( Data->FileNum != 0 && Data->Files[0].Parts->FH.unp_ver < 20 )
        Level = 0; // store

      // add file
      Wrt = 0;
      if ( PR_FAIL(Error = AddFile(Data, NewName, hSeqOut, hNewIO, Level,
        &Wrt, (Data->FileNum > 0) ? (&Data->Files->Parts->FH) : (NULL))) )
      {
        goto loc_exit;
      }

      if( NULL != hNewIO )
      {
        CALL_SYS_ObjectClose(hNewIO);
        hNewIO = (NULL);
      }
    }
    while( PR_SUCC(CALL_UniArchiverCallback_GetNextNewObject(hCallBack,
      &hNewObj)) );
  }
  
  // check for endarch header needed
  if ( cFALSE != EndArc )
  {
    // write endarch header
    if ( PR_FAIL(Error = CALL_SeqIO_Write(hSeqOut, NULL,
      (tVOID*)(&RAR_ENDARC[0]), sizeof(RAR_ENDARC))) )
    {
      goto loc_exit;
    }
  }

  // flush out file and close seq_io
  if ( PR_FAIL(Error = CALL_SeqIO_Flush(hSeqOut)) ||
       PR_FAIL(Error = CALL_SYS_ObjectClose(hSeqOut)) )
  {
    goto loc_exit;
  }

loc_exit:
  PR_TRACE_A1( _this, "Leave MiniArchiver::RebuildArchive method, ret tERROR = 0x%08x", Error);
  return(Error);
}

// ##### *** ADDITIONAL FUNCTIONS *** #########################################

tERROR ReadBaseHdr(tINPUT* input, tBASE_HDR* base)
{
  tDWORD offset;
  tERROR  rcode;

  rcode = (errOK);
  offset = RarTell(input);
  memset(base, 0, sizeof(tBASE_HDR));
  base->crc = RarReadWord(input);         // not need to calculate CRC

  if ( !input->eof )
  {
    input->hd_crc = 0xFFFFFFFF;

    base->type  = RarReadByteCrc(input);
    base->flags = RarReadWordCrc(input);
    base->size  = RarReadWordCrc(input);

    if ( base->type < LOW_HEADER_TYPE || base->type > HIGH_HEADER_TYPE )
    {
      // header outside range
      rcode = (errOBJECT_INVALID);
    }
    else
    {
      base->hdr_size = SIZEOF_SHORTBLOCKHEAD;

      if ( base->flags & LONG_BLOCK )
      {
        // long block
        base->hdr_size += sizeof(dword);
        base->extra_size = RarReadDwordCrc(input);
      }
      else
      {
        base->extra_size = 0;
      }

      if ( base->hdr_size > base->size )
      {
        // invalid block size
        rcode = (errOBJECT_INVALID);
      }
    }
  }
  else
  {
    rcode = (errINTERFACE_INCOMPATIBLE);
  }
  
  return(rcode);
}

// ----------------------------------------------------------------------------

tERROR UnpackFile(tRAR* data, tINPUT* input, tRAR_FILE_HEADER* fh)
{
  tERROR rcode;
  tBOOL  Solid;

  data->LastError = rcode = (errOK);
  data->Volume = (tBOOL)((fh->base.flags & LHD_SPLIT_AFTER) != 0);
  data->DestUnpSize = fh->usize;
  memcpy(&data->CurrentFile, fh, sizeof(tRAR_FILE_HEADER));
  BufferUnpackStart(input, fh->psize); // set file mode to unpack
  Solid = (cFALSE);

  if ( fh->method == 0x30 ) // store 
  {
    while ( fh->usize )
    {
      tUINT maywrite;

      maywrite = input->incnt - input->inptr;
      if ( maywrite )
      {
        if ( maywrite > fh->usize )
          maywrite = fh->usize;
        
        data->wr_crc32 = CRC32(data->wr_crc32,input->inbuf+input->inptr,maywrite);
        if ( (tUINT)FileWrite(data, input->inbuf+input->inptr, maywrite) != maywrite)
        {
          rcode = (errOBJECT_WRITE);
          break;
        }

        fh->usize -= maywrite;
        input->inptr += maywrite;
      }
      else
      {
        if ( !FileBufferUpdate(input) )
          break;
      }
    }

    if ( input->eof )
    {
      if ( data->Suspended )
      {
        // user break
        rcode = (errOK);
      }
      else
      {
        // error reading infile
        rcode = (errOBJECT_READ);
      }
    }
  }
  else
  {
    tUINT WinSize;

     WinSize = 1 << (((fh->base.flags & LHD_WINDOWMASK  ) >> 5) + 16);

     if ( data->Window == NULL )
     {
       UnpackCreate(data, WinSize);
#ifdef DEBUG_DUMP
       memset(data->Window, 0x00, WinSize);
#endif
     }
     else
     {
       if ( data->WindowSize < WinSize )
       {
         if ( !data->Save  )
         {
           RarFree(data,data->Window);
           UnpackCreate(data, WinSize);
#ifdef DEBUG_DUMP
           memset(data->Window, 0x00, WinSize);
#endif
         }
         else
         {
           return(errNOT_OK);
         }
       }
     }

    if ( data->Window )
    {
      switch ( fh->unp_ver )
      {
      case 15:
        Solid = (rarbool)((data->MHD_Flags & MHD_SOLID) &&
          (((hi_MiniArchiver)(data->hThis))->data->CurFile >= 1));
        Unpack15(data, input, Solid);
        break;
      case 20:
      case 26:
        Solid = (rarbool)((fh->base.flags & LHD_SOLID) != 0);
        Unpack20(data, input, Solid);
        break;
      default: /* can be hacked version field */
      case 29:
        Solid = (rarbool)((fh->base.flags & LHD_SOLID) != 0);
        Unpack29(data, input, Solid);
        break;
      }
    }
    else
    {
      rcode = (errNOT_ENOUGH_MEMORY);
    }
  }

  if ( (data->MHD_Flags & MHD_SOLID) == 0 ||
        fh->base_type == COMM_HEAD        ||
        fh->base_type == NEWSUB_HEAD)
  {
    data->Save = cFALSE;
  }
  else
  {
    data->Save = cTRUE;
  }

  if ( fh->base.type == COMM_HEAD )
  {
    if ( fh->file_crc != ((~data->wr_crc32) & 0xFFFF) )
    {
      // comment crc failed
      rcode = (errOBJECT_DATA_CORRUPTED);
    }
  }
  else
  {
    if ( errOK != data->LastError || data->CurrentFile.file_crc != ~data->wr_crc32 )
    {
      // file crc failed
      rcode = (errOBJECT_DATA_CORRUPTED);
      data->InvalidCrc = cTRUE;
      if ( Solid )
      {
        data->Suspended = (cTRUE);
      }
    }
  }

  BufferUnpackStop(input);
  return(rcode);
}

// ----------------------------------------------------------------------------

tERROR CopySeqIOData(tRAR_DATA* Data, hSEQ_IO hSeqIn, tQWORD InpOffs,
		     hSEQ_IO hSeqOut, tQWORD OutOffs, tQWORD Count)
{
  tERROR Error;
  tDWORD Size, I;
  tBYTE* Buff;
  
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqIn,  NULL, InpOffs, cSEEK_SET)) )
    return(Error);
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, OutOffs, cSEEK_SET)) )
    return(Error);

  I = 0;
  Buff = Data->pTmpBuff;
  while( (tLONG)(Count) > 0 )
  {
    Size = (Count > TMP_BUFF_SIZE) ? (TMP_BUFF_SIZE) : (tDWORD)(Count);
    
    if ( PR_FAIL(Error = CALL_SeqIO_Read(hSeqIn, NULL, Buff, Size)) )
      return(Error);
    if ( PR_FAIL(Error = CALL_SeqIO_Write(hSeqOut, NULL, Buff, Size)) )
      return(Error);
    if ( (I += 1) >= 20 )
    {
      if ( 0 != (I = Rotate(&Data->RAR)) )
        return(errOPERATION_CANCELED);
      I = 0;
    }
    
    Count = (Count - Size);
  }

  if ( PR_FAIL(Error = CALL_SeqIO_Flush(hSeqOut)) )
    return(Error);
  
  return(errOK);
}

// input file size in 'offset' field
tERROR AddTempFile(tRAR_DATA* Data, hSEQ_IO hSeqIn, tFILE_NODE* FNode)
{
  hSEQ_IO hSeqOut;
  tERROR  Error;
  tDWORD  Count, I;
  tDWORD  Size;
  tVOID*  Buff;

  // check params
  if ( NULL == Data || NULL == hSeqIn || NULL == FNode )
    return(errPARAMETER_INVALID);

  // check for already saved
  if ( FNode->TmpOffs != LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF) )
    return(errOK);

  // seek files
  hSeqOut = Data->hTmpSeq;
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut,&FNode->TmpOffs,0,cSEEK_END)) ||
       PR_FAIL(Error = CALL_SeqIO_Seek(hSeqIn, NULL, 0, cSEEK_SET)) )
  {
    return(Error);
  }
  
  I = 0; // copy data
  Buff = Data->pTmpBuff;
  Count = (tDWORD)FNode->FullSize;
  while( (tLONG)(Count) > 0 )
  {
    Size = (Count > TMP_BUFF_SIZE) ? (TMP_BUFF_SIZE) : (Count);

    if ( PR_FAIL(Error = CALL_SeqIO_Read(hSeqIn, NULL, Buff, Size)) )
      return(Error);
    if ( PR_FAIL(Error = CALL_SeqIO_Write(hSeqOut, NULL, Buff, Size)) )
      return(Error);
    if ( (I += 1) >= 20 )
    {
      if ( Rotate(&Data->RAR) )
        return(errOPERATION_CANCELED);
      I = 0;
    }
    
    Count = (Count - Size);
  }

  // flush data
  if ( PR_FAIL(Error = CALL_SeqIO_Flush(hSeqOut)) )
    return(Error);
  
  return(errOK);
}

// file node must be properly initialized
tERROR GetTempFile(tRAR_DATA* Data, hIO hOutIO, tFILE_NODE* FNode)
{
  hSEQ_IO hSeqOut;
  hSEQ_IO hSeqIn;
  tERROR Error;
  tQWORD Offs;
  tDWORD Count, I;
  tDWORD Size;
  tVOID* Buff;

  if ( NULL == Data || NULL == hOutIO || NULL == FNode )
    return(errPARAMETER_INVALID);

  if ( LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF) == (Offs = FNode->TmpOffs) )
    return(errOBJECT_NOT_FOUND);

  // create io's
  hSeqIn = Data->hTmpSeq;
  if ( PR_FAIL(Error = CreateSeqIO(hOutIO, &hSeqOut)) )
    return(Error);

  // seek files
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqIn,  NULL, Offs, cSEEK_SET)) ||
       PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, 0,    cSEEK_SET)) )
  {
    return(Error);
  }
  
  I = 0; // copy data
  Buff  = Data->pTmpBuff;
  Count = (tDWORD)FNode->FullSize;
  while( (tLONG)(Count) > 0 )
  {
    Size = (Count > TMP_BUFF_SIZE) ? (TMP_BUFF_SIZE) : (Count);
    
    if ( PR_FAIL(Error = CALL_SeqIO_Read(hSeqIn, NULL, Buff, Size)) )
      return(Error);
    if ( PR_FAIL(Error = CALL_SeqIO_Write(hSeqOut, NULL, Buff, Size)) )
      return(Error);
    if ( (I += 1) >= 20 )
    {
      if ( Rotate(&Data->RAR) )
        return(errOPERATION_CANCELED);
      I = 0;
    }
    
    Count = (Count - Size);
  }

  // close seq_io wrapper
  if ( PR_FAIL(Error = CALL_SeqIO_Flush(hSeqOut)) ||
       PR_FAIL(Error = CALL_IO_Flush(hOutIO))     ||
       PR_FAIL(Error = CALL_SYS_ObjectClose(hSeqOut)) )
  {
    return(Error);
  }
  
  return(errOK);
}

// ----------------------------------------------------------------------------

// create some temp io
tERROR CreateTempIO(hi_MiniArchiver _this, hIO* hResIO)
{
  tERROR Error;
  hIO    hNewIO;

  if ( NULL == _this || NULL == hResIO )
    return(errPARAMETER_INVALID);

  *hResIO = hNewIO = (NULL);
  if ( PR_SUCC(Error = CALL_SYS_ObjectCreate(_this, &hNewIO, IID_IO,
    PID_TMPFILE, SUBTYPE_ANY)) )
  {
    CALL_SYS_PropertySetDWord((hOBJECT)(hNewIO), pgOBJECT_ACCESS_MODE,
      fACCESS_RW);
    CALL_SYS_PropertySetDWord((hOBJECT)(hNewIO), pgOBJECT_OPEN_MODE,
      fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_READ);
    
    if ( PR_FAIL(Error = CALL_SYS_ObjectCreateDone(hNewIO)) )
      return(Error);
  }
  
  *hResIO = hNewIO;
  return(errOK);
}

// create some temp io and temp seq_io based on this io
tERROR CreateTempSeqIO(hi_MiniArchiver _this, hIO* hResIO,
                              hSEQ_IO* hResSeqIO)
{
  hSEQ_IO hNewSeqIO;
  tERROR  Error;
  hIO     hNewIO;
  
  if ( NULL == _this || NULL == hResIO || NULL == hResSeqIO )
    return(errPARAMETER_INVALID);

  *hResIO = hNewIO = (NULL);
  *hResSeqIO = hNewSeqIO = (NULL);
  if ( PR_FAIL(Error = CreateTempIO(_this, &hNewIO)) )
    return(Error);
  if ( PR_FAIL(Error = CreateSeqIO(hNewIO, &hNewSeqIO)) )
    return(Error);

  *hResIO  = hNewIO;
  *hResSeqIO = hNewSeqIO;
  return(errOK);
}

// create seq_io based on given io
tERROR CreateSeqIO(hIO hBaseIO, hSEQ_IO* hResSeqIO)
{
  hSEQ_IO hNewSeqIO;
  tERROR  Error;

  if ( NULL == hBaseIO || NULL == hResSeqIO )
    return(errPARAMETER_INVALID);

  *hResSeqIO = hNewSeqIO = (NULL);
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(hBaseIO, &hNewSeqIO,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    return(Error);
  }

  *hResSeqIO = hNewSeqIO;
  return(errOK);
}

// create named io and seq_io based on this io
tERROR CreateNamedIO(hi_MiniArchiver _this, tCHAR* Name, hIO* hResIO,
		     hSEQ_IO* hResSeqIO)
{
  tRAR_DATA* Data;
  hSEQ_IO hNewSeqIO;
  tERROR  Error;
  hIO     hNewIO;
  hSTRING hString;

  if ( NULL == _this || NULL == hResIO || NULL == Name )
    return(errPARAMETER_INVALID);

  Data = (tRAR_DATA*)(_this->data);
  if ( NULL == Data->hPtr )
    return(errNOT_OK); // one file

  // init by zeroes
  hNewSeqIO = (NULL);
  *hResIO = hNewIO = (NULL);
  if ( NULL != hResSeqIO )
    *hResSeqIO = (NULL);

  // create string resource
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hString, IID_STRING,
    PID_ANY, SUBTYPE_ANY)) )
  {
    return(Error);
  }

  CALL_String_ImportFromBuff(hString, NULL, Name, _toui32(strlen(Name)), cCP_ANSI,
    cSTRING_Z);

  // create io
  if ( PR_FAIL(Error = CALL_ObjPtr_IOCreate(Data->hPtr, &hNewIO, hString,
    fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
  {
    return(Error);
  }

  // free string resource
  CALL_SYS_ObjectClose(hString);

  if ( NULL != hResSeqIO )
  {
    // create seq_io based on io
    if ( PR_FAIL(Error = CreateSeqIO(hNewIO, &hNewSeqIO)) )
      return(Error);

    *hResSeqIO = hNewSeqIO;
  }
  
  *hResIO = hNewIO;
  return(errOK);
}

// ----------------------------------------------------------------------------

// compare string (for file extention)
static tBOOL RarExtCmp(tCHAR* Cmp, tCHAR* Name, tUINT Cnt)
{
  while( Cnt )
  {
    if ( *(Name++) != (*(Cmp++) | 0x20 ) )
      break;
    Cnt--;
  }

  return(tBOOL)(!Cnt);
}

// try create next volume file name by new-rar rules
tBOOL NextVolumeNewName(tCHAR* Curr, tCHAR* NewName, tUINT Ext)
{
  tUINT Digit, Limit, Cnt, Next;

  Next = 0;
  Limit = 1;
  Cnt = 0;

  if ( Ext <= 4  )
    return(cFALSE);

  while( Ext-- )
  {
    Digit = Curr[Ext] - '0';
    if ( Digit < 10 )
    {
      Cnt++;
      Next += (Digit * Limit);
      Limit *= 10;
    }
    else
    {
      break;
    }
  }

  Next++;
  Ext++;

  if ( Ext > 5 && RarExtCmp(Curr + Ext - 5, ".part", 5) )
  {
    Ext += Cnt;

    if ( Next < Limit )
    {
      while ( Cnt-- )
      {
        Ext--;
        NewName[Ext] = (tCHAR)(Next % 10 + '0');
        Next /= 10;
      }
      return(cTRUE);
    }
  }
  
  return(cFALSE);
}

// try create next volume name
tERROR GetNextVolumeName(tCHAR* CurrName, tCHAR* NewName)
{
  tERROR Error;
  tUINT  Ext, Next;
  tUINT  Lower;

  Error = (errOBJECT_NOT_FOUND);
  Ext   = _toui32(strlen(CurrName));

  if ( Ext > 4 )
  {
    Ext -= 4;
    if ( CurrName[Ext] == '.' )
    {
      if ( FALSE != NextVolumeNewName(CurrName, NewName, Ext) )
        return(errOK); // new numbering style

      // old numbering style
      Ext++;
      Lower = CurrName[Ext] & ' ';

      if ( RarExtCmp(&CurrName[Ext], "rar", 3) )
      {
        Next = 0;
        Error = (errOK);
      }
      else
      {
        Next = (CurrName[Ext] | ' ') - 'r';

        if ( Next <= ( 'z'-'r')  )
        {
          Next *= 100;
          if ( CurrName[Ext+1] >= '0' && CurrName[Ext+1] <= '9')
          {
            Next += (CurrName[Ext+1] - '0') * 10;
            if ( CurrName[Ext+2] >= '0' && CurrName[Ext+2] <= '9' )
            {
              Next += CurrName[Ext+2] - '0' + 1;

              if ( Next <= 899 )
              {
                // allowed limit
                Error = (errOK);
              }
            }
          }
        }
      }
        
      if ( PR_SUCC(Error) )
      {
        NewName[Ext+0] = (tCHAR)((Next / 100) + ('R' | Lower));
        Next %= 100;
        NewName[Ext+1] = (tCHAR)((Next / 10) + '0');
        Next %= 10;
        NewName[Ext+2] = (tCHAR)(Next + '0');
      }

      return(Error);
    }
  }

  return(errOBJECT_NOT_FOUND);
}

// try create previous volume file name by new-rar rules
tERROR PrevVolumeNewName(tCHAR* Curr, tCHAR* NewName, tUINT Ext)
{
  tUINT Digit, Limit, Cnt, Next;

  Next = 0;
  Limit = 1;
  Cnt = 0;

  if ( Ext <= 4  )
    return(errOBJECT_NOT_FOUND);

  while( Ext-- )
  {
    Digit = Curr[Ext] - '0';
    if ( Digit < 10 )
    {
      Cnt++;
      Next += (Digit * Limit);
      Limit *= 10;
    }
    else
    {
      break;
    }
  }

  Next--;
  Ext++;
  if ( Next == 0 )
    return(errNOT_OK);

  if ( Ext > 5 && RarExtCmp(Curr + Ext - 5, ".part", 5) )
  {
    Ext += Cnt;

    if ( Next < Limit )
    {
      while ( Cnt-- )
      {
        Ext--;
        NewName[Ext] = (tCHAR)(Next % 10 + '0');
        Next /= 10;
      }
      return(errOK);
    }
  }
  
  return(errOBJECT_NOT_FOUND);
}

// try create previous volume name
tERROR GetPrevVolumeName(tCHAR* CurrName, tCHAR* NewName)
{
  tERROR Error;
  tUINT  Ext, Next;
  tUINT  Lower;

  Error = (errOBJECT_NOT_FOUND);
  Ext = _toui32(strlen(CurrName));
  strcpy_s(NewName, Ext + 1, CurrName);

  if ( Ext > 4 )
  {
    Ext -= 4;
    if ( CurrName[Ext] == '.' )
    {
      if ( PR_FAIL(Error = PrevVolumeNewName(CurrName, NewName, Ext)) )
      {
        if ( errNOT_OK == Error )
          return(errOBJECT_NOT_FOUND); // first vol
      }
      else
      {
        return(errOK); // new numbering style
      }

      Ext++;  // old numbering style
      Lower = CurrName[Ext] & ' ';

      if ( RarExtCmp(&CurrName[Ext], "rar", 3) )
        return(errOBJECT_NOT_FOUND); // first or single volume
      if ( RarExtCmp(&CurrName[Ext], "r00", 3) )
      {
        memcpy(&NewName[Ext], "rar", 3);
        return(errOK); // fixed name
      }

      Next = (CurrName[Ext] | ' ') - 'r';

      if ( Next <= ( 'z'-'r')  )
      {
        Next *= 100;
        if ( CurrName[Ext+1] >= '0' && CurrName[Ext+1] <= '9')
        {
          Next += (CurrName[Ext+1] - '0') * 10;
          if ( CurrName[Ext+2] >= '0' && CurrName[Ext+2] <= '9' )
          {
            Next += CurrName[Ext+2] - '0' - 1;
          }
        }
      }
    }
        
    NewName[Ext+0] = (tCHAR)((Next / 100) + ('R' | Lower));
    Next %= 100;
    NewName[Ext+1] = (tCHAR)((Next / 10) + '0');
    Next %= 10;
    NewName[Ext+2] = (tCHAR)(Next + '0');
    return(errOK);
  }

  return(errOBJECT_NOT_FOUND);
}

// ----------------------------------------------------------------------------

// try find and open next volume
tERROR AttachNextVolume(hi_MiniArchiver _this)
{
  tRAR_DATA* Data;
  tERROR  Error;
  hSEQ_IO hNewSeqIO;
  tCHAR   NewName[2048];
  hIO     hNewIO;
  tUINT   Len;

  Data = (tRAR_DATA*)(_this->data);
  Len  = _toui32(strlen(Data->ArcName));
  memcpy(NewName, Data->ArcName, Len+1);

  // check for '.exe' stub
  if ( (Len > 4)                    &&
       (NewName[Len-1]|0x20) == 'e' && 
       (NewName[Len-2]|0x20) == 'x' && 
       (NewName[Len-3]|0x20) == 'e' &&
       (NewName[Len-4]|0x20) == '.' )
  {
    // try open next 1.x-2.x arch
    memcpy(&NewName[Len-4], ".r00", 4);
    if ( PR_FAIL(Error = CreateNamedIO(_this,NewName,&hNewIO, &hNewSeqIO)) )
    {
      // try open next 3.0 arch
      memcpy(&NewName[Len-4], ".rar", 4);
      if ( PR_FAIL(Error = GetNextVolumeName(&Data->ArcName[0],&NewName[0])) )
        return(Error);
      if ( PR_FAIL(Error = CreateNamedIO(_this,NewName,&hNewIO,&hNewSeqIO)) )
        return(Error);
    }
  }
  else
  {
    // generate next name, open next arch
    if ( PR_FAIL(Error = GetNextVolumeName(&Data->ArcName[0], &NewName[0])) )
      return(Error);
    if ( PR_FAIL(Error = CreateNamedIO(_this, NewName, &hNewIO, &hNewSeqIO)) )
      return(Error);
  }

  // save new name, update context
  memcpy(&Data->ArcName[0], &NewName[0], Len+1);
  Data->hArcIO = hNewIO;
  Data->hSeqIn = hNewSeqIO;
  Data->RAR.hSeqIn = hNewSeqIO;
  InputInitFile(&Data->RAR.infile, &Data->RAR, Data->RAR.infile.inbuf,
    Data->RAR.infile.bufsize);
  RarSeek(&Data->RAR.infile, 0);

  return(errOK);
}

// reattach engine to next volume
tERROR ArchiveMerge(tRAR* RAR, tINPUT* Input, tRAR_FILE_HEADER* FH)
{
  tPART_NODE* PNode;
  tFILE_NODE* FNode;
  tVOL_NODE*  VNode;
  tRAR_DATA*  Data;

  Data = ((hi_MiniArchiver)(RAR->hThis))->data;
  FNode = &Data->Files[RAR->CurrFNode];
  if ( (RAR->CurrPNode += 1) >= FNode->PartCnt )
    return(RAR->LastError = errOBJECT_NOT_FOUND);

  PNode = &FNode->Parts[RAR->CurrPNode];
  VNode = PNode->Volume;
  RAR->hSeqIn = VNode->hVolSeqIO;
  RarSeek(&RAR->infile, PNode->DataOffs);
  memcpy(FH, &PNode->FH, sizeof(tRAR_FILE_HEADER));
  InputInitFile(&RAR->infile, RAR, RAR->infile.inbuf, RAR->infile.bufsize);
  Data->RAR.Volume = (rarbool)((FH->base.flags & LHD_SPLIT_AFTER) != 0);
  return(errOK);
}

// ----------------------------------------------------------------------------

tERROR CalcCountHash(hHASH hHash, tBYTE* Buff, tUINT Cnt, tQWORD* Seed)
{
  tERROR Error;
  tBYTE  Hash[32];

  if ( NULL == hHash || NULL == Seed )
    return(errPARAMETER_INVALID);
  
  if ( PR_FAIL(Error = CALL_Hash_Reset(hHash)) )
    return(Error);
  if ( PR_FAIL(Error = CALL_Hash_Update(hHash, Buff, Cnt)) )
    return(Error);
  if ( PR_FAIL(Error = CALL_Hash_GetHash(hHash, Hash, sizeof(Hash))) )
    return(Error);

  *(Seed) ^= *(tQWORD*)(&Hash[0]);
  *(Seed) ^= *(tQWORD*)(&Hash[8]);
  return(errOK);
}

// ----------------------------------------------------------------------------

// just save volume node to volumes-list and reinit engine
tERROR SaveVolumeNode(tRAR_DATA* Data,hLIST hList,tVOL_NODE* Node,tBOOL Next)
{
  tERROR Error;

  if ( Node->VolName[0] == '\0' )
    return(errNOT_OK);

  // add volume node to volumes-list
  if ( PR_FAIL(Error = CALL_List_Add(hList, NULL, Node,
    sizeof(tVOL_NODE), 0, cLIST_LAST)) )
  {
    return(Error);
  }

  Data->VolNum += 1;

  if ( cFALSE == Next )
    return(errOK);

  // make new volume node
  memset(Node, 0, sizeof(tVOL_NODE));
  Data->ArcName[MAX_NAME-1] = '\0';
  strcpy_s(&Node->VolName[0], countof(Node->VolName), &Data->ArcName[0]);
  Node->hVolIO    = Data->hArcIO;
  Node->hVolSeqIO = Data->hSeqIn;
  Node->VolHash   = (tQWORD)(0);

  // reinit reader
  Data->RAR.NextBlock = 0;
  InputInitFile(&Data->RAR.infile, &Data->RAR, Data->RAR.infile.inbuf,
    Data->RAR.infile.bufsize);

  return(errOK);
}

// ----------------------------------------------------------------------------

tERROR FillNames(tRAR_DATA* Data, hIO hArcIO)
{
  Data->ArcFull[0] = '\0';
  Data->ArcName[0] = '\0';
  Data->ArcPath[0] = '\0';

  // get names
  while( NULL != hArcIO && Data->ArcFull[0] == '\0' )
  {
    CALL_SYS_PropertyGetStr(hArcIO, NULL, pgOBJECT_FULL_NAME, 
      &Data->ArcFull[0], sizeof(Data->ArcFull), cCP_ANSI);
    CALL_SYS_PropertyGetStr(hArcIO, NULL,  pgOBJECT_NAME,
      &Data->ArcName[0], sizeof(Data->ArcName), cCP_ANSI);
    CALL_SYS_PropertyGetStr(hArcIO, NULL,  pgOBJECT_PATH,
      &Data->ArcPath[0], sizeof(Data->ArcPath), cCP_ANSI);

    hArcIO = (hIO)CALL_SYS_ParentGet(hArcIO, IID_IO);
  }

  return(errOK);
}

tERROR InitVolumes(hi_MiniArchiver _this)
{
  hIO    Parent;
  hIO    hNewIO;
  tBOOL  First;
  tWORD  Flags;
  tERROR Error;
  tBYTE  Mark[0x10];
  tCHAR  Name[MAX_PATH];
  tRAR_DATA* Data;

  First  = (cTRUE);
  Data   = (tRAR_DATA*)(_this->data);
  hNewIO = Data->hArcIO;
  Parent = Data->hArcIO;
  
  while(TRUE)
  {
    // get io name/path
    if ( PR_FAIL(Error = FillNames(Data, hNewIO)) )
      return(Error);

    // read archive header
    memset(&Mark[0], 0, sizeof(Mark));
    if ( PR_FAIL(Error = CALL_IO_SeekRead(hNewIO, NULL, 0, &Mark[0],
      sizeof(Mark))) )
    {
      if ( cFALSE != First )
        return(Error);
      else
        break; // stop scan
    }

    /* check archive header */
    if ( Mark[0] != 'R' || Mark[1] != 'a' || Mark[2] != 'r' ||
         memcmp(&Mark[3], &RAR_SIGN_PART[0], sizeof(RAR_SIGN_PART)) )
    {
      if ( First )
        return(errINTERFACE_INCOMPATIBLE); // incompatible first volume
      else
        break; // stop scan
    }

    if ( (Data->hArcIO != Parent) &&
         PR_FAIL(Error = CALL_SYS_ObjectClose(Data->hArcIO)) )
    {
      return(Error);
    }

    Data->hArcIO = hNewIO;

    // check archive main flags
    Flags = *(tWORD*)(&Mark[7+2+1]);
    if ( !(Flags & MHD_VOLUME) || (Flags & MHD_FIRSTVOLUME) )
      break; // founded first volume

    // try find previous volumes
    if ( PR_FAIL(Error = GetPrevVolumeName(Data->ArcName, Name)) )
      break;

    // open volume
    if ( PR_FAIL(Error = CreateNamedIO(_this, Name, &hNewIO, NULL)) )
      break;

    First = (cFALSE);
  }

  return(errOK);
}

// ----------------------------------------------------------------------------

tERROR InitArchive(hi_MiniArchiver _this)
{
  tERROR Error;
  tRAR_DATA* Data;

  Data = (tRAR_DATA*)(_this->data);

  // scan for first volume
  if ( cFALSE != Data->AsSingle && PR_FAIL(Error = InitVolumes(_this)) )
    return(Error);

  // get io name/path
  if ( PR_FAIL(Error = FillNames(Data, Data->hArcIO)) )
    return(Error);

#ifdef DEBUG_PASS_LOG
  if ( Data->FPasLog )
  {
    fprintf(Data->FPasLog, "RAR_InitArchive(): %s\n", Data->ArcFull);
    fflush(Data->FPasLog);
  }
#endif

  // create input seq_io
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(Data->hArcIO, &Data->hSeqIn,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    return(Error);
  }
  
  Data->RAR.hSeqIn = Data->hSeqIn;
  Data->RAR.hHeap  = Data->hHeap;
  
  // alloc read buffer
  if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)&Data->RAR.RD_Buff,
    RD_BUFF_SIZE)) )
  {
    return(Error);
  }
  
  // read catalogue
  if ( PR_FAIL(Error = ReadAllBlocks(_this)) )
    return(Error);

  // alloc temp storage buffer
  if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)&Data->pTmpBuff,
    TMP_BUFF_SIZE)) )
  {
    return(Error);
  }
  
  // create temp storage seq_io
  if ( PR_FAIL(Error = CreateTempSeqIO(_this, &Data->hTmpIO, &Data->hTmpSeq)) )
  {
    return(Error);
  }
  
  return(errOK);
}

// ----------------------------------------------------------------------------

// try process all volumes and read all blocks
tERROR ReadAllBlocks(hi_MiniArchiver _this)
{
  tRAR_FILE_HEADER FHdr;     // file header
  tBLOCK_NODE BNode;         // block node
  tFILE_NODE  FNode;         // file node
  tPART_NODE  PNode;         // file thunk node
  tVOL_NODE   VNode;         // volume node
  tRAR_DATA*  Data;          // mini-archiver data
  tLISTNODE   LNode;         // list node
  tBASE_HDR   BHdr;          // block header
  tINPUT*     Input;         // internal
  tERROR      Error;         // error container
  tRAR*       RAR;           // ptr to internal struct
  hLIST       hBList;        // blocks list
  hLIST       hFList;        // files list
  hLIST       hVList;        // volumes list
  hLIST       hPList;        // file thunks (parts) list
  tUINT       I, J;          // just counter
  tBOOL       FirstVol;      // process first volume flag
  tBOOL       Run;           // do process flag
  tWORD       HiPosAV;       // digital sign
  tDWORD      PosAV;         // digital sign offset
  tBYTE       RDBuff[0x40];  // small input buffer
  tBYTE       Mark[0x20];    // for hash calc
  tUINT       Rdt;           // readed bytes
  tBYTE       Salt[SALT_SIZE]; // main headers decryptor salt
  tBOOL       ArcEnd;        // end of arc detected
  tUINT       BCnt;          // blocks counter (for hash)
  tUINT       BType;         // original block type
  tBOOL       FirstSplit;    // first file split before
  tUINT       BlkFlags;      // block header flags
  hSTRING     hName;         // unicode name
  
  // create lists for nodes
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hBList,
         IID_LIST, PID_ANY, SUBTYPE_ANY)) ||
       PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hFList,
         IID_LIST, PID_ANY, SUBTYPE_ANY)) ||  
       PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hVList,
         IID_LIST, PID_ANY, SUBTYPE_ANY)) ||
       PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hPList,
         IID_LIST, PID_ANY, SUBTYPE_ANY)) )
  {
    return(Error);
  }

  // reinit data
  Data = (tRAR_DATA*)(_this->data);
  Data->NotFullSet = (cFALSE);
  Data->BlkNum  = 0;
  Data->FileNum = 0;
  Data->VolNum  = 0;
  Data->PartNum = 0;

  // init archiver engine
  RAR  = (tRAR*)(&Data->RAR);
  Data->RAR.LastError = (errOK);
  Data->RAR.hThis = (hOBJECT)(_this);
  Data->RAR.Window = NULL;                    // data not saved
  Data->RAR.PPM.SubAlloc.HeapStart = NULL;    // data not saved
  Data->RAR.VM.Mem = NULL;
  Data->RAR.Save   = FALSE;
  Data->RAR.last_file = FALSE;
  Data->RAR.hThis = (hOBJECT)(_this);
  Data->RAR.Decrypt  = (cFALSE);
  Input = (tINPUT*)(&RAR->infile);
  Data->RAR.TotPSize = LONG_LONG_CONST(0xffffffffffffffff);
  MemInit(&Data->RAR, sizeof(tSTACK_FILTER), 0x200);
  VM_InitFilters(&Data->RAR);
  InputInitFile(Input, RAR, &RDBuff[0], sizeof(RDBuff));
  
  // init nodes by zeroes (for hard errors)
  memset(&BNode, 0, sizeof(tBLOCK_NODE));
  memset(&FNode, 0, sizeof(tFILE_NODE));
  memset(&PNode, 0, sizeof(tPART_NODE));
  memset(&VNode, 0, sizeof(tVOL_NODE));

  // init hash calc
  BCnt = 0;

  // init current volume info
  strcpy_s(&VNode.VolName[0], countof(VNode.VolName), &Data->ArcName[0]);
  VNode.hVolIO = Data->hArcIO;
  VNode.hVolSeqIO = Data->hSeqIn;

  Data->FullRO = (cFALSE);
  RAR->LastError = (errOK);
  RAR->NextBlock = 0;
  FirstSplit = (cFALSE);
  FirstVol = (cTRUE);
  ArcEnd = (cFALSE);
  hName = (NULL);
  Run = (cTRUE);

  // read catalogue
  while ( cFALSE != Run )
  {
scan_volume:
    if ( (RAR->MHD_Flags & MHD_PASSWORD) && RAR->Password[0] == '\0' )
    {
      if ( cFALSE == RarAskPassword(Data) )
      {
        return(errOBJECT_PASSWORD_PROTECTED); // password needed
      }
    }

    // setup decryptor
    RAR->Decrypt = (cFALSE);
    if ( RAR->NextBlock != RarSeek(&RAR->infile, RAR->NextBlock) )
    {
      Data->FullRO = (cTRUE);
      break; // corrupt
    }
    if ( (RAR->MHD_Flags & MHD_PASSWORD) &&
         (RAR->NextBlock >= (SIZEOF_MARKHEAD + SIZEOF_NEWMHD)) )
    {
      // read salt for headers decryptor
      PackRead(RAR, Salt, SALT_SIZE);
      RAR_SetEncryption(RAR, 30, &RAR->Password[0], Salt);
      RarSeek(&RAR->infile, RAR->NextBlock + SALT_SIZE);
      RAR->Decrypt = (cTRUE);
    }

    memset(&BNode, 0, sizeof(tBLOCK_NODE));
    BNode.BlkOffs = RAR->NextBlock;

    if ( PR_FAIL(Error = ReadBaseHdr(Input, &BHdr)) )
    {
      if ( errOPERATION_CANCELED == RAR->LastError )
        return(errOPERATION_CANCELED);

      if ( cFALSE == Input->eof && cFALSE != RAR->Decrypt )
      {
        RAR->Password[0] = '\0';
        goto scan_volume; // try next password
      }

      if ( cFALSE == Input->eof )
      {
        Data->FullRO = (cTRUE);
        Run = (cFALSE);  // corrupt arc
        continue;
      }

      Run = (cFALSE);
      goto endarc_mark; // try next volume
    }

    if ( errOPERATION_CANCELED == RAR->LastError )
      return(errOPERATION_CANCELED);
    
    // update hash in volume node
    memset(Mark, 0, sizeof(Mark));
    *(tWORD*) (&Mark[0])  = BHdr.crc;
    *(tBYTE*) (&Mark[2])  = BHdr.type;
    *(tWORD*) (&Mark[3])  = BHdr.flags;
    *(tWORD*) (&Mark[5])  = BHdr.size;
    *(tDWORD*)(&Mark[7])  = BHdr.hdr_size;
    *(tDWORD*)(&Mark[11]) = BHdr.extra_size;
    *(tDWORD*)(&Mark[15]) = RAR->NextBlock;
    *(tDWORD*)(&Mark[19]) = BCnt++;
    Error = CalcCountHash(Data->hMD5, &Mark[0], 0x20, &VNode.VolHash);
    if ( PR_FAIL(Error) )
      return(Error);

    // build block node
    memcpy(&BNode.BHdr, &BHdr, sizeof(tBASE_HDR));
    BNode.BlkSize  = (BHdr.size + BHdr.extra_size);
    BNode.Vol = (tVOL_NODE*)(Data->VolNum); // fix after build vol array
    if ( cFALSE != RAR->Decrypt )
      RAR->NextBlock += ALIGN(BHdr.size + BHdr.extra_size, 16) + SALT_SIZE;
    else
      RAR->NextBlock = (BNode.BlkOffs + BNode.BlkSize);

    // add block node to blocks-list
    if ( PR_FAIL(Error = CALL_List_Add(hBList, NULL, &BNode,
      sizeof(tBLOCK_NODE), 0, cLIST_LAST)) )
    {
      return(Error);
    }

    // update counter & block type
    Data->BlkNum += 1;
    BType = BHdr.type;

    // check block type
    switch ( BHdr.type )
    {
    case NEWSUB_HEAD:
    case FILE_HEAD:
      memset(&FHdr, 0, sizeof(tRAR_FILE_HEADER));
      memcpy(&FHdr.base, &BHdr, sizeof(tBASE_HDR));
      FHdr.psize      = BHdr.extra_size;
      FHdr.usize      = RarReadDwordCrc(Input);
      FHdr.host_os    = RarReadByteCrc(Input);
      FHdr.file_crc   = RarReadDwordCrc(Input);
      FHdr.file_time  = RarReadDwordCrc(Input);
      FHdr.unp_ver    = RarReadByteCrc(Input);
      FHdr.method     = RarReadByteCrc(Input);
      FHdr.name_size  = RarReadWordCrc(Input);
      FHdr.file_attr  = RarReadDwordCrc(Input);

      // update hash in volume node
      memset(Mark, 0, sizeof(Mark));
      *(tDWORD*)(&Mark[0])  = FHdr.usize;
      *(tBYTE*) (&Mark[4])  = FHdr.host_os;
      *(tDWORD*)(&Mark[5])  = FHdr.file_crc;
      *(tDWORD*)(&Mark[9])  = FHdr.file_time;
      *(tBYTE*) (&Mark[13]) = FHdr.unp_ver;
      *(tBYTE*) (&Mark[14]) = FHdr.method;
      *(tWORD*) (&Mark[15]) = FHdr.name_size;
      *(tDWORD*)(&Mark[17]) = FHdr.file_attr;
      Error = CalcCountHash(Data->hMD5, &Mark[0], 0x20, &VNode.VolHash);
      if ( PR_FAIL(Error) )
        return(Error);

      if ( FHdr.name_size < 250 )
      {
        RarReadCrc(Input, &FHdr.name[0], FHdr.name_size);
        FHdr.name[FHdr.name_size + 0] = 0;
        FHdr.name[FHdr.name_size + 1] = 0;
      }
      else
      {
        RarReadCrc(Input, FHdr.name, 250);
        RarSkipCrc(Input, FHdr.name_size - 250);
        FHdr.name[250] = 0;
        FHdr.name[251] = 0;
      }

      if ( FHdr.base.flags & LHD_LARGE )
      {
        // large files not supported
        return(errOBJECT_INCOMPATIBLE);
      }

      // check for new types
      if ( FHdr.base.type == NEWSUB_HEAD && !memcmp(FHdr.name,"STM", 4) )
      {
        tBYTE UC[8];
        tINT  I, Size;
        
        Size = FHdr.base.size - (FHdr.name_size + SIZEOF_NEWLHD);
        if ( Size > 0 && Size < 500 )
        {
          if ( NULL == hName )
          {
            // create container for unicode name
            if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hName,
              IID_STRING, PID_ANY, SUBTYPE_ANY)) )
            {
              return(Error);
            }
            if ( PR_FAIL(Error = CALL_String_SetCP(hName, cCP_UNICODE)) )
              return(Error);
          }

          // clear string
          memset(&UC[0], 0, sizeof(UC));
          if ( PR_FAIL(Error = CALL_String_ImportFromBuff(hName, NULL, &UC[0],
            1, cCP_UNICODE, cSTRING_Z)) )
          {
            return(Error);
          }

          // read name
          Size = (Size / 2);
          UC[2] = UC[3] = '\0';
          Rdt  = (3 + SIZEOF_NEWLHD);
          for ( I = 0; I < Size; I++ )
          {
            // read unicode char
            UC[0] = RarReadByteCrc(Input);
            UC[1] = RarReadByteCrc(Input);
            Rdt += 2;

            // strcat name string
            if ( PR_FAIL(Error = CALL_String_AddFromBuff(hName, NULL, &UC[0],
              1, cCP_UNICODE, cSTRING_CONTENT_ONLY)) )
            {
              return(Error);
            }
          }

          // convert unicode to ansi
          if ( PR_FAIL(Error = CALL_String_ExportToBuff(hName, NULL, cSTRING_WHOLE,
             &FHdr.name[0], MAX_NAME, cCP_OEM, cSTRING_Z)) )
          {
            return(Error);
          }
        }
        else
        {
          // create dummy name
          memcpy(&FHdr.name[0], &RAR_DUMMYNAME[0], sizeof(RAR_DUMMYNAME));
        }

        FHdr.base.type = FILE_HEAD;
      }
      else
      {
        Rdt = (FHdr.name_size + SIZEOF_NEWLHD);
      }

      if ( FHdr.base.flags & LHD_SALT )
      {
        RarReadCrc(Input, &FHdr.salt, SALT_SIZE);
        Rdt += (SALT_SIZE);
      }
      
      if ( Rdt < FHdr.base.size )
      {
        // skip extra data at the end
        RarSkipCrc(Input, FHdr.base.size - Rdt);
      }

      BlkFlags = 0;
      if ( (tWORD)(~Input->hd_crc) != FHdr.base.crc )
      {
        // file header crc failed
        Data->FullRO = cTRUE;
        BlkFlags |= RAR_FLG_HDRCRC;
      }

      if ( FHdr.base.type == NEWSUB_HEAD && !memcmp(FHdr.name, "CMT\0", 4) )
      {
        memcpy(FHdr.name, &RAR_COMMENT[0], sizeof(RAR_COMMENT));
        FHdr.base.type = FILE_HEAD;
        BlkFlags |= RAR_FLG_HDRCMT;
      }

      if ( FHdr.base.type == FILE_HEAD )
      {
        // build file-thunk node
        memset(&PNode, 0, sizeof(tPART_NODE));
        memcpy(&PNode.FH, &FHdr, sizeof(tRAR_FILE_HEADER));
        PNode.BlkOffs  = BNode.BlkOffs;
        if ( RAR->MHD_Flags & MHD_PASSWORD )
        {
          PNode.DataOffs = BNode.BlkOffs + ALIGN(BHdr.size, 16) + SALT_SIZE;
        }
        else if ( BHdr.flags & LHD_PASSWORD )
        {
          PNode.DataOffs = RarTell(Input);
        }
        else
        {
          PNode.DataOffs = RarTell(Input);
        }

        PNode.PartSize = (BHdr.hdr_size + BHdr.extra_size);
        PNode.DataSize = (FHdr.psize);
        PNode.Volume   = (tVOL_NODE*)(Data->VolNum); // fix after
        PNode.BHdrNum  = (Data->BlkNum - 1);
        PNode.FH.base_type = (tBYTE)(BType);
        PNode.HdrFlag  = BlkFlags;

        // add file-thunk node to parts list
        if ( PR_FAIL(Error = CALL_List_Add(hPList, NULL, &PNode,
          sizeof(tPART_NODE), 0, cLIST_LAST)) )
        {
          return(Error);
        }

        // check first file split
        if ( cFALSE != FirstVol && 0 == Data->PartNum )
        {
          if ( BHdr.flags & LHD_SPLIT_BEFORE )
            FirstSplit = (cTRUE); // not full set
        }

        // update counter
        Data->PartNum += 1;

        // update file node
        FNode.PartCnt += 1;
        if ( cFALSE == (FHdr.base.flags & LHD_SPLIT_AFTER) )
        {
          // last or single thunk
          FNode.Parts = (tPART_NODE*)(Data->PartNum - FNode.PartCnt); // fix
          FNode.FullSize = FHdr.usize;
          FNode.TmpOffs  = (tQWORD)LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF);
          if ( FNode.PartCnt > 1)
            FNode.ReadOnly = (cTRUE);
          else
            FNode.ReadOnly = (cFALSE);
          Data->NotFullSet = (cFALSE); // clear flag
          
          // add file node to list
          if ( PR_FAIL(Error = CALL_List_Add(hFList, NULL, &FNode,
            sizeof(tFILE_NODE), 0, cLIST_LAST)) )
          {
            return(Error);
          }
          
          // update counter
          Data->FileNum += 1;

          if ( cFALSE == Data->AsSingle && Data->VolNum >= 1 )
          {
            // arc processing done
            Run = (cFALSE);
            break;
          }

          // reinit node
          memset(&FNode, 0, sizeof(tFILE_NODE));
          break;
        }
        else
        {
          Data->NotFullSet = (cTRUE); // temporally
        }
      }
      break;
    case MARK_HEAD:
      break; // must be skipped
    case MAIN_HEAD:
      // read main archive header
      if ( BHdr.size < SIZEOF_NEWMHD )
        return(errINTERFACE_INCOMPATIBLE);
      
      HiPosAV = RarReadWordCrc(Input);
      PosAV = RarReadDwordCrc(Input);

      if ( (tWORD)(~Input->hd_crc) != BHdr.crc )
        return(errINTERFACE_INCOMPATIBLE);

      if ( cFALSE != FirstVol )
      {
        Data->RAR.MHD_Flags = BHdr.flags;

        if ( BHdr.flags & MHD_VOLUME )
          Data->FullRO = (cTRUE);
      }
      
      if ( RAR->MHD_Flags & MHD_PASSWORD )
      {
        Data->FullRO = (cTRUE);
      }
      break;
    case ENDARC_HEAD:
endarc_mark:
      if ( cFALSE == Data->AsSingle && !(FHdr.base.flags & LHD_SPLIT_AFTER) )
      {
        // arc processing done
        Run = (cFALSE);
        continue;
      }
      ArcEnd = (cTRUE);
      break;
    case COMM_HEAD:
      break;
    case AV_HEAD:
    case SUB_HEAD:
    case PROTECT_HEAD:
    case SIGN_HEAD:
    default:
      // not interested in, just skip
      RarSkipCrc(Input, BHdr.size - BHdr.hdr_size);
      if ( (tWORD)(~Input->hd_crc) != BHdr.crc )
      {
        // header crc failed
        Data->FullRO = cTRUE;
      }
      if ( BHdr.type == AV_HEAD )
      {
        Data->FullRO = cTRUE; // coz digital signed
      }
      break;
    }

    if ( ArcEnd )
    {
      if ( PR_SUCC(Error = AttachNextVolume(_this)) )
      {
        // save node to list
        if ( PR_FAIL(Error = SaveVolumeNode(Data, hVList, &VNode, cTRUE)) )
          return(Error);

        // update node data
        memset(&VNode, 0, sizeof(tVOL_NODE));
        VNode.hVolIO = Data->hArcIO;
        VNode.hVolSeqIO = Data->hSeqIn; 
        strcpy_s(&VNode.VolName[0], countof(VNode.VolName), Data->ArcName);

        BCnt = 0;
        Run = (cTRUE);
        ArcEnd = (cFALSE);
        Data->RAR.NextBlock = 0;
        goto scan_volume;  // process next volume
      }

      Run = (cFALSE);
      continue;  // next volume absent
    }
  }

  // check not full set
  if ( cFALSE != FirstSplit )
    Data->NotFullSet = (cTRUE);

  // save last volume node
  VNode.hVolIO = Data->hArcIO;
  VNode.hVolSeqIO = Data->hSeqIn;
  if ( PR_FAIL(Error = SaveVolumeNode(Data, hVList, &VNode, cFALSE)) )
    return(Error);

  // alloc arrays
  if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)(&Data->Blks),
    ((Data->BlkNum+1) * sizeof(tBLOCK_NODE)) + 64)) || NULL == Data->Blks )
  {
    return(errNOT_ENOUGH_MEMORY);
  }
  if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)(&Data->Files),
    ((Data->FileNum+1) * sizeof(tFILE_NODE)) + 64)) || NULL == Data->Files)
  {
    return(errNOT_ENOUGH_MEMORY);
  }
  if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)(&Data->Vols),
    ((Data->VolNum+1) * sizeof(tVOL_NODE)) + 64)) || NULL == Data->Vols)
  {
    return(errNOT_ENOUGH_MEMORY);
  }
  if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)(&Data->Parts),
    ((Data->PartNum+1) * sizeof(tPART_NODE)) + 64)) || NULL == Data->Parts)
  {
    return(errNOT_ENOUGH_MEMORY);
  }

  // init arrays by zeroes (for hard errors)
  memset(Data->Blks,  0, (Data->BlkNum  + 1) * sizeof(tBLOCK_NODE));
  memset(Data->Files, 0, (Data->FileNum + 1) * sizeof(tFILE_NODE));
  memset(Data->Vols,  0, (Data->VolNum  + 1) * sizeof(tVOL_NODE));
  memset(Data->Parts, 0, (Data->PartNum + 1) * sizeof(tPART_NODE));

  I = 0; // make volumes array
  if ( PR_SUCC(Error = CALL_List_First(hVList, &LNode)) )
  {
    do
    {
      if ( PR_FAIL(Error = CALL_List_DataGet(hVList, NULL, LNode,
        &VNode, sizeof(tVOL_NODE))) )
      {
        return(Error);
      }

      memcpy(&Data->Vols[I], &VNode, sizeof(tVOL_NODE));
      if ( (I += 1) >= Data->VolNum )
        break;
    }
    while( PR_SUCC(CALL_List_Next(hVList, &LNode, LNode)) );
  }
  else
  {
    return(Error);
  }

  if ( I != Data->VolNum )
    return(errUNEXPECTED);

  // check for chunk-archive
  if ( 0 == Data->PartNum || 0 == Data->FileNum )
    return(errOK); // empty archive

  I = 0; // make blocks array
  if ( PR_SUCC(Error = CALL_List_First(hBList, &LNode)) )
  {
    do
    {
      if ( PR_FAIL(Error = CALL_List_DataGet(hBList, NULL, LNode,
        &BNode, sizeof(tBLOCK_NODE))) )
      {
        return(Error);
      }

      if ( (tUINT)(BNode.Vol) > Data->VolNum )
        break;

      // fix pointer to volume node
      BNode.Vol = (tVOL_NODE*)(&Data->Vols[(tUINT)(BNode.Vol)]);
      memcpy(&Data->Blks[I], &BNode, sizeof(tBLOCK_NODE));

      if ( (I += 1) >= Data->BlkNum )
        break;
    }
    while( PR_SUCC(CALL_List_Next(hBList, &LNode, LNode)) );
  }
  else
  {
    return(Error);
  }

  if ( I != Data->BlkNum )
    return(errUNEXPECTED);
  
  I = 0; // make file-thunks array
  if ( PR_SUCC(Error = CALL_List_First(hPList, &LNode)) )
  {
    do
    {
      if ( PR_FAIL(Error = CALL_List_DataGet(hPList, NULL, LNode,
        &PNode, sizeof(tPART_NODE))) )
      {
        return(Error);
      }

      if ( (tUINT)(PNode.Volume) > Data->VolNum )
        break;

      // fix pointer to volume node
      PNode.Volume = (tVOL_NODE*)(&Data->Vols[(tUINT)(PNode.Volume)]);
      memcpy(&Data->Parts[I], &PNode, sizeof(tPART_NODE));

      // update block header type (for new types)
      Data->Blks[PNode.BHdrNum].BHdr.type = PNode.FH.base.type;

      if ( (I += 1) >= Data->PartNum )
        break;
    }
    while( PR_SUCC(CALL_List_Next(hPList, &LNode, LNode)) );
  }
  else
  {
    return(Error);
  }

  if ( I != Data->PartNum )
    return(errUNEXPECTED);
  
  I = 0; // make files array
  if ( PR_SUCC(Error = CALL_List_First(hFList, &LNode)) )
  {
    do
    {
      if ( PR_FAIL(Error = CALL_List_DataGet(hFList, NULL, LNode,
        &FNode, sizeof(tFILE_NODE))) )
      {
        return(Error);
      }

      if ( ((tUINT)(FNode.Parts) + FNode.PartCnt) > Data->PartNum )
        break;

      // fix pointer to thunk node
      FNode.Parts = (tPART_NODE*)(&Data->Parts[(tUINT)(FNode.Parts)]);
      if ( FALSE == (FNode.Parts[0].FH.base.flags & LHD_SPLIT_BEFORE) )
      {
        if ( LHD_DIRECTORY == (FNode.Parts->FH.base.flags & LHD_WINDOWMASK) )
          FNode.IsFolder = (cTRUE);
        memcpy(&Data->Files[I], &FNode, sizeof(tFILE_NODE));
        if ( (I += 1) >= Data->FileNum )
          break;
      }
      else
      {
        // skip partial file
        Data->FileNum -= 1;
      }
    }
    while( PR_SUCC(CALL_List_Next(hFList, &LNode, LNode)) );
  }
  else
  {
    return(Error);
  }

  if ( I != Data->FileNum )
    return(errUNEXPECTED);

  if ( Data->VolNum > 1 )
    Data->FullRO = cTRUE;

  // destroy lists
  CALL_SYS_ObjectClose(hPList);
  CALL_SYS_ObjectClose(hVList);
  CALL_SYS_ObjectClose(hFList);
  CALL_SYS_ObjectClose(hBList);

  /* destroy string */
  if ( NULL != hName )
    CALL_SYS_ObjectClose(hName);

  /* process files and apply flags */
  for ( I = 0; I < Data->FileNum; I++ )
  {
    /* check for wrong header crc */
    for ( J = 0; J < Data->Files[I].PartCnt; J++ )
    {
      if ( Data->Files[I].Parts[J].HdrFlag & RAR_FLG_HDRCRC )
      {
        Data->Files[I].HdrFlag |= RAR_FLG_HDRCRC;
        break;
      }
    }

    /* check for comment */
    if ( Data->Files[I].PartCnt == 1 )
      if ( Data->Files[I].Parts[0].HdrFlag & RAR_FLG_HDRCMT )
        Data->Files[I].HdrFlag |= RAR_FLG_HDRCMT;
  }

  return(errOK);
}

// ----------------------------------------------------------------------------

#if 0 // compressor buffers
# define PACK_CODEBUFSIZE     (0x8000)
# define PACK_MAXWINSIZE      (0x100000)
# define PACK_MAXWINMASK      (PACK_MAXWINSIZE-1)
# define PACK_MEMORY          (PACK_MAXWINSIZE+0x24000+0x200)
#else
# define PACK_CODEBUFSIZE     (0x8000)
# define PACK_MAXWINSIZE      (0x40000)
# define PACK_MAXWINMASK      (PACK_MAXWINSIZE-1)
# define PACK_MEMORY          (PACK_MAXWINSIZE+0x24000+0x200)
#endif

tERROR AddFile(tRAR_DATA* Data, tCHAR* Name, hSEQ_IO Dst, hIO Src,
               tINT Level, tDWORD* WrtSize, tRAR_FILE_HEADER* FH)
{
  tNEW_FILE_HEADER FHdr;
  tRAR_FILE_HEADER TFH;
  hSEQ_IO   SrcSeq;
  tERROR    Error;
  tPACK20*  Pack;
  tQWORD    Offs, NOffs;
  tQWORD    USize, Count;
  tDWORD    Size;
  tBYTE*    Buff;
  tDWORD    Year;
  tDWORD    Month;
  tDWORD    Day;
  tDWORD    Hour;
  tDWORD    Minute;
  tDWORD    Second;
  tDWORD    Time;
  tDT       DT;

  if ( PR_FAIL(Error = CALL_IO_GetSize(Src, &USize, IO_SIZE_TYPE_EXPLICIT)) )
    return(Error);

  if ( NULL == FH )
  {
    memset(&TFH, 0, sizeof(tRAR_FILE_HEADER));
    FH = &TFH;
  }
    
  // init compression context
  Pack = &Data->Pack;
  PackInitData(Pack, FALSE);
  if ( NULL == Pack->Window )
  {
    Size = (PACK_MEMORY + 4*PACK_MAXWINSIZE);
    if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)&Pack->Window, Size)) )
      return(Error);
  }
  if ( NULL == Pack->CodeBuf )
  {
    Size = (PACK_CODEBUFSIZE);
    if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hHeap, (tPTR*)&Pack->CodeBuf, Size)) )
      return(Error);
  }

  Pack->hSeqOut = Dst;
  if ( PR_FAIL(Error = CreateSeqIO(Src, &SrcSeq)) )
    return(Error);
  Pack->hSeqInp = SrcSeq;
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(Dst, &Offs, 0, cSEEK_FORWARD)) )
    return(Error);

  memset(&FHdr, 0, sizeof(tNEW_FILE_HEADER));
  FHdr.NameSize = _toui16(strlen(Name));
  FHdr.UnpSize  = (tDWORD)(USize);
  Data->Pack.MaxPackRead = (tDWORD)(USize);
  Data->Pack.PackFileCRC = (0xFFFFFFFF);
  
  // write header and name
  if ( PR_FAIL(Error = CALL_SeqIO_Write(Dst, 0, &FHdr, sizeof(FHdr))) ||
       PR_FAIL(Error = CALL_SeqIO_Write(Dst, 0, Name, FHdr.NameSize)) )
  {
    return(Error);
  }

  if ( Level > 5 )
    Level = 5;

  if ( Level == 0 )
  {
    // store
    Buff  = Data->pTmpBuff;
    Count = Pack->MaxPackRead;
    Pack->PackSize = (tDWORD)(USize);
    while( (tINT)(Count) > 0 )
    {
      Size = (TMP_BUFF_SIZE > Count) ? (tDWORD)(Count) : (TMP_BUFF_SIZE);

      if ( PR_FAIL(Error = CALL_SeqIO_Read(SrcSeq, NULL, Buff, Size)) )
        return(Error);
      Pack->PackFileCRC = CRC32(Pack->PackFileCRC, Buff, Size);
      if ( PR_FAIL(Error = CALL_SeqIO_Write(Dst, NULL, Buff, Size)) )
        return(Error);

      Count -= Size;
    }
  }
  else
  {
    // compress
    Pack20Proc(Pack, Pack->Window, Pack20Read, CodeBlock, cFALSE, Level-1, 
      Pack->CodeBuf, PACK_CODEBUFSIZE, PACK_MAXWINSIZE);
    FlushCodeBuffer(Pack);
    FlushBuf(Pack);
  }
  
  // flush out seq_io
  if ( PR_FAIL(Error = CALL_SeqIO_Flush(Pack->hSeqOut)) ||
       PR_FAIL(Error = CALL_SYS_ObjectClose(Pack->hSeqInp)) )
  {
    return(Error);
  }

  // flush io's
  if ( PR_FAIL(Error = CALL_SeqIO_Flush(Dst)) ||
       PR_FAIL(Error = CALL_SeqIO_Seek(Dst, &NOffs, 0, cSEEK_END)) )
  {
    return(Error);
  }

  // update file header
  FHdr.FileAttr = FH->file_attr;
  FHdr.FileCRC  = ~Pack->PackFileCRC;
  Hour  = Minute = Second = 0;
  Month = Day = 1;
  Year  = 1990;
  Now(&DT);
  DTGet((const tDT*)&DT, &Year,&Month,&Day,&Hour,&Minute,&Second,NULL);
  Time = ((Year - 1980) << 25)|(Month << 21)|(Day << 16)|(Hour << 11)|
    (Minute << 5)|(Second);
  FHdr.FileTime = Time;
  FHdr.Flags    = (0x8000 | 0x40);
  FHdr.HeadSize = (FHdr.NameSize + sizeof(tNEW_FILE_HEADER));
  FHdr.HeadType = (FILE_HEAD);
  FHdr.HostOS   = FH->host_os;
  FHdr.Method   = (Level == 0) ? (0x30) : (0x31);
  FHdr.PackSize = (tDWORD)(NOffs - Offs - FHdr.HeadSize);
  FHdr.UnpVer   = 20;
  FHdr.HeadCRC  = ~(tWORD)(CRC32(CRC32(0xFFFFFFFF, &FHdr.HeadType,
    sizeof(FHdr)-2), Name, FHdr.NameSize));
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(Dst, 0, Offs, cSEEK_SET)) ||
       PR_FAIL(Error = CALL_SeqIO_Write(Dst,0, &FHdr, sizeof(FHdr))) ||
       PR_FAIL(Error = CALL_SeqIO_Flush(Dst)) )
  {
    return(Error);
  }

  if ( PR_FAIL(Error = CALL_SeqIO_Seek(Dst, NULL, NOffs, cSEEK_SET)) )
    return(Error);

  if ( NULL != WrtSize )
    *WrtSize = (tUINT)(NOffs - Offs);

  return(errOK);
}

// ----------------------------------------------------------------------------

tBOOL RarAskPassword(tRAR_DATA* Data)
{
  tERROR Error = (errNOT_OK);
  tRAR*  RAR;

  RAR = &Data->RAR;
  RAR->Password[0] = '\0';
  if ( 0 != Rotate(RAR) )
    return(cFALSE);

#ifdef DEBUG_PASS
  if ( RAR->PswCnt++ == 0 )
    strcpy(RAR->Password, "0");
  else
    return(cFALSE);

  printf("%s\n", RAR->Password);
  CALL_String_ImportFromBuff(Data->hPswd, NULL, RAR->Password,
    strlen(RAR->Password), cCP_ANSI, cSTRING_Z);
  return(cTRUE);
#endif

  // get base io
  if ( NULL == Data->hUniArcIO )
    return(cFALSE); // no io

  // send pswd message
  Error = CALL_SYS_SendMsg(Data->hUniArcIO, pmc_IO, pm_IO_PASSWORD_REQUEST,
    Data->hPswd, NULL, NULL);
  if ( Error == errOK_DECIDED )
  {
    CALL_String_ExportToBuff(Data->hPswd, NULL, cSTRING_WHOLE,
      RAR->Password, MAXPASSWORD-4, cCP_OEM, cSTRING_Z);

#ifdef DEBUG_PASS_LOG
    if ( Data->FPasLog )
    {
      fprintf(Data->FPasLog, " --> RAR_AskPassword(): \"%s\"\n", RAR->Password);
      fflush(Data->FPasLog);
    }
#endif

    return(cTRUE); // password founded
  }

#ifdef DEBUG_PASS_LOG
  if ( Data->FPasLog )
  {
    fprintf(Data->FPasLog, " --> RAR_AskPassword() != errOK_DECIDED\n");
    fflush(Data->FPasLog);
  }
#endif

  return(cFALSE); // no password
}

// ----------------------------------------------------------------------------

static tBOOL RarSaveState(tRAR_DATA* Data)
{
  tBYTE* Wnd;
  tRAR* ORar;
  tRAR* SRar;

  ORar = &Data->RAR;
  SRar = &Data->RarSave;
  if ( NULL != SRar->Window && SRar->WindowSize < ORar->WindowSize )
  {
    Free(&Data->RAR, SRar->Window);
    SRar->Window = (NULL);
  }

  Wnd = SRar->Window;
  if ( NULL == Wnd && NULL != ORar->Window && 0 != ORar->WindowSize )
  {
    if ( NULL == (Wnd = Malloc(&Data->RAR, ORar->WindowSize+64)) )
      return(cFALSE);
  }

  memcpy(SRar, ORar, sizeof(tRAR));
  if ( NULL != (SRar->Window = Wnd) )
  {
    memcpy(SRar->Window, ORar->Window, ORar->WindowSize);
  }

  return(cTRUE);
}

// ----------------------------------------------------------------------------

static tBOOL RarLoadState(tRAR_DATA* Data)
{
#ifdef DEBUG_PASS
  tBOOL  Swp;
  tUINT  Cnt;
#endif
  tBYTE* Wnd;
  tRAR* ORar;
  tRAR* SRar;

  ORar = &Data->RAR;
  SRar = &Data->RarSave;

  // free ppm data
  SA_StopSubAllocator(ORar);

  if ( NULL != SRar->Window && SRar->WindowSize > ORar->WindowSize )
  {
    Free(&Data->RAR, ORar->Window);
    ORar->Window = (NULL);
  }

  Wnd = ORar->Window;
  if ( NULL == Wnd && NULL != SRar->Window )
  {
    if ( NULL == (Wnd = Malloc(&Data->RAR, SRar->WindowSize+64)) )
      return(cFALSE);
  }

#ifdef DEBUG_PASS
  Swp = ORar->PswSwap;
  Cnt = ORar->PswCnt;
#endif

  memcpy(ORar, SRar, sizeof(tRAR));
  if ( NULL != (ORar->Window = Wnd) )
  {
    memcpy(ORar->Window, SRar->Window, SRar->WindowSize);
  }

#ifdef DEBUG_PASS
  ORar->PswSwap = Swp;
  ORar->PswCnt  = Cnt;
#endif
  return(cTRUE);
}

// ----------------------------------------------------------------------------

// calc total packed size (for volumes)
static tQWORD RarGetFilePSize(tRAR_DATA* Data, tQWORD ID)
{
  tFILE_NODE* fn;
  tPART_NODE* fp;
  tQWORD Res;
  tUINT  I;

  Res = 0;
  fn = &Data->Files[ID];
  fp = &fn->Parts[0];
  for ( I = 0; I < fn->PartCnt; I++ )
    Res += fp[I].FH.psize;

  return(Res);
}

// ----------------------------------------------------------------------------

tERROR RarUnpackFile(tRAR_DATA* Data, tQWORD ID, hSEQ_IO hSeqOut)
{
  tRAR_FILE_HEADER fh;
  tFILE_NODE* Node;
  hSEQ_IO hTmpSeq;
  tERROR  Error;
  tQWORD  I, Res;
  tQWORD  TotPSize;
  tRAR*   RAR;
  hIO     hTmpIO;

  RAR = &Data->RAR;
  Data->CurFile = ID;
  RAR->LastError = (errOK);

  if ( RAR->MHD_Flags & MHD_SOLID && ID != Data->LastUnp + 1 )
  {
    if ( ID < (Data->LastUnp+1) )
      Data->LastUnp = LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF); // unpack from begin
    
    // unpack all previous files
    for ( I = (Data->LastUnp+1); I < ID; I++ )
    {
      // check for directory
      if ( cFALSE != Data->Files[I].IsFolder || 0 == Data->Files[I].FullSize )
      {
        Data->LastUnp += 1;
        continue;
      }

      // create tmp output io and seq_io
      if ( PR_FAIL(Error=CreateTempIO((hi_MiniArchiver)RAR->hThis,&hTmpIO)) )
        return(Error);
      if ( PR_FAIL(Error=CreateSeqIO(hTmpIO, &hTmpSeq)) )
      {
        CALL_SYS_ObjectClose(hTmpIO);
        return(Error);
      }

      // calc total packed size
      TotPSize = RarGetFilePSize(Data, I);

psw_retry1: // retry password
      if ( PR_FAIL(Error = CALL_SeqIO_SetSize(hTmpSeq, 0)) ||
           PR_FAIL(Error = CALL_SeqIO_Seek(hTmpSeq, &Res, 0, cSEEK_SET)) )
      {
        CALL_SYS_ObjectClose(hTmpSeq);
        CALL_SYS_ObjectClose(hTmpIO);
        return(Error);
      }
      
      // unpack
      Node = &Data->Files[I];
      Data->hArcIO = Node->Parts->Volume->hVolIO;
      Data->hSeqIn = Node->Parts->Volume->hVolSeqIO;
      RAR->hSeqOut = hTmpSeq;
      RAR->hSeqIn  = Data->hSeqIn;
      RAR->CurrPNode = (0);
      RAR->CurrFNode = (tUINT)(I);
      RAR->TotPSize  = TotPSize;
      RAR->InvalidCrc = cFALSE;
      RarSeek(&RAR->infile, Node->Parts[0].DataOffs);
      InputInitFile(&RAR->infile, RAR, RAR->RD_Buff, RD_BUFF_SIZE);
      memcpy(&fh, &Node->Parts[0].FH, sizeof(tRAR_FILE_HEADER));
      memcpy(&RAR->CurrentFile, &fh, sizeof(tRAR_FILE_HEADER));

      // save decompressor state if needed
      if ( (RAR->MHD_Flags & MHD_SOLID) && (fh.base.flags & LHD_PASSWORD) )
      {
        if ( cFALSE == RarSaveState(Data) )
        {
          CALL_SYS_ObjectClose(hTmpSeq);
          CALL_SYS_ObjectClose(hTmpIO);
          return(errNOT_OK);
        }
      }

      // get first password if needed
      if ( RAR->Password[0] == '\0' && (fh.base.flags & LHD_PASSWORD) )
      {
        if ( cFALSE == RarAskPassword(Data) )
        {
          CALL_SYS_ObjectClose(hTmpSeq);
          CALL_SYS_ObjectClose(hTmpIO);
          return(errOBJECT_PASSWORD_PROTECTED);
        }
      }

      RAR->Decrypt = (cFALSE);
      if ( fh.base.flags & LHD_PASSWORD )
      {
        RAR_SetEncryption(RAR, fh.unp_ver, &RAR->Password[0],
          (fh.base.flags & LHD_SALT) ? (&fh.salt[0]) : (NULL));
        RAR->Decrypt = (cTRUE);
      }

      if ( PR_FAIL(Error = UnpackFile(RAR, &RAR->infile, &fh)) )
      {
        if ( errOPERATION_CANCELED == RAR->LastError )
          return(errOPERATION_CANCELED);

        if ( fh.base.flags & LHD_PASSWORD )
        {
          // restore decompressor state
          if ( RAR->MHD_Flags & MHD_SOLID )
          {
            if ( cFALSE == RarLoadState(Data) )
            {
              CALL_SYS_ObjectClose(hTmpSeq);
              CALL_SYS_ObjectClose(hTmpIO);
              return(errNOT_OK);
            }
          }

          RAR->Password[0] = '\0';
          goto psw_retry1; // try next password
        }

        return(RAR->InvalidCrc) ? (warnOBJECT_DATA_CORRUPTED) : (Error);
      }

      if ( errOPERATION_CANCELED == RAR->LastError )
        return(errOPERATION_CANCELED);

      if ( PR_FAIL(Error = CALL_SeqIO_Flush(hTmpSeq)) ||
           PR_FAIL(Error = CALL_IO_Flush(hTmpIO)) )
      {
        CALL_SYS_ObjectClose(hTmpSeq);
        CALL_SYS_ObjectClose(hTmpIO);
        return(Error);
      }
      
      // save temp data
      if ( PR_FAIL(Error = AddTempFile(Data, hTmpSeq, Node)) )
      {
        CALL_SYS_ObjectClose(hTmpSeq);
        CALL_SYS_ObjectClose(hTmpIO);
        return(Error);
      }
      
      // close seq_io
      RAR->hSeqOut = (NULL);
      CALL_SYS_ObjectClose(hTmpSeq);
      CALL_SYS_ObjectClose(hTmpIO);
      Data->LastUnp += 1;
    } 
  }

  // calc total packed size
  TotPSize = RarGetFilePSize(Data, ID);
  
psw_retry2: // retry password
  if ( PR_FAIL(Error = CALL_SeqIO_SetSize(hSeqOut, 0)) )
    return(Error);
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, &Res, 0, cSEEK_SET)) )
    return(Error);

  // seek to data begin and unpack
  Node = &Data->Files[ID];
  Data->hArcIO = Node->Parts->Volume->hVolIO;
  Data->hSeqIn = Node->Parts->Volume->hVolSeqIO;
  RAR->hSeqOut = hSeqOut;
  RAR->hSeqIn  = Data->hSeqIn;
  RAR->CurrPNode = (0);
  RAR->CurrFNode = (tUINT)(ID);
  RAR->TotPSize  = TotPSize;
  RAR->InvalidCrc = cFALSE;
  RarSeek(&RAR->infile, Node->Parts[0].DataOffs);
  InputInitFile(&RAR->infile, RAR, RAR->RD_Buff, RD_BUFF_SIZE);
  memcpy(&fh, &Node->Parts[0].FH, sizeof(tRAR_FILE_HEADER));
  memcpy(&Data->RAR.CurrentFile, &fh, sizeof(tRAR_FILE_HEADER));

  // save decompressor state if needed
  if ( (RAR->MHD_Flags & MHD_SOLID) && (fh.base.flags & LHD_PASSWORD) )
  {
    if ( cFALSE == RarSaveState(Data) )
      return(errNOT_OK);
  }

  // get first password if needed
  if ( RAR->Password[0] == '\0' && (fh.base.flags & LHD_PASSWORD) )
  {
    if ( cFALSE == RarAskPassword(Data) )
      return(errOBJECT_PASSWORD_PROTECTED);
  }

  RAR->Decrypt = (cFALSE);
  if ( fh.base.flags & LHD_PASSWORD )
  {
    RAR_SetEncryption(RAR, fh.unp_ver, &RAR->Password[0],
     (fh.base.flags & LHD_SALT) ? (&fh.salt[0]) : (NULL));
    RAR->Decrypt = (cTRUE);
  }
  
  if ( PR_FAIL(Error = UnpackFile(RAR, &RAR->infile, &fh)) )
  {
    if ( errOK != RAR->LastError )
      return(RAR->LastError);

    if ( fh.base.flags & LHD_PASSWORD )
    {
      if ( RAR->MHD_Flags & MHD_SOLID )
        if ( cFALSE == RarLoadState(Data) )
          return(errNOT_OK);

      RAR->Password[0] = '\0';
      goto psw_retry2; // try next password
    }

    return(RAR->InvalidCrc) ? (warnOBJECT_DATA_CORRUPTED) : (Error);
  }

  if ( errOK != RAR->LastError )
    return(RAR->LastError);

  if ( PR_FAIL(Error = CALL_SeqIO_Flush(hSeqOut)) )
    return(Error);
  if ( PR_FAIL(Error = AddTempFile(Data, hSeqOut, Node)) )
    return(Error);
    
  Data->RAR.hSeqOut = (NULL);
  Data->LastUnp = ID;
  return(errOK);
}

// ----------------------------------------------------------------------------

tERROR GetBaseHdrPart(hIO hInpIO, tDWORD* FOffs, tBYTE Buff[16])
{
  tBASE_HDR BHdr;
  tERROR Error;
  tDWORD Rdt;

  Rdt = 0;
  if ( PR_FAIL(Error = CALL_IO_SeekRead((hIO)(hInpIO), &Rdt, *FOffs,
    (tVOID*)(&Buff[0]), 16)) || Rdt < 7 )
  {
    return(errOUT_OF_OBJECT);
  }

  // build and check base header
  BHdr.type  = *(tBYTE*)(&Buff[2]);
  BHdr.flags = *(tWORD*)(&Buff[3]);
  BHdr.size  = *(tWORD*)(&Buff[5]);
  if ( BHdr.type < LOW_HEADER_TYPE || BHdr.type > HIGH_HEADER_TYPE )
  {
    return(errINTERFACE_INCOMPATIBLE);
  }

  BHdr.extra_size = 0;
  BHdr.hdr_size = (SIZEOF_SHORTBLOCKHEAD);
  if ( BHdr.flags & LONG_BLOCK )
  {
    if ( Rdt < 11 )
      return(errOUT_OF_OBJECT);

    BHdr.hdr_size  += sizeof(tDWORD);
    BHdr.extra_size = *(tDWORD*)(&Buff[7]);
  }

  if ( BHdr.hdr_size > BHdr.size )
  {
    return(errINTERFACE_INCOMPATIBLE);
  }

  // update file offset
  *(FOffs) += (BHdr.size + BHdr.extra_size);
  return(errOK);
}

// ----------------------------------------------------------------------------

// extract new file io (for solid archives)
static tERROR RAR_DupeIO(hi_MiniArchiver _this, tQWORD ID, hIO* hRes)
{
  tFILE_NODE* FNode;
  tRAR_DATA* Data;
  tERROR Error;
  hSEQ_IO hSeq;
  hIO IO;

  IO = (NULL);
  Data = (tRAR_DATA*)(_this->data);

  // create temp file
  if ( PR_FAIL(Error = CreateTempIO(_this, &IO)) )
    return(Error);

  FNode = &Data->Files[ID];
  if ( FNode->TmpOffs == LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF) )
  {
    // decompress file
    if ( PR_FAIL(Error = CreateSeqIO(IO, &hSeq)) )
      return(Error);
    if ( PR_FAIL(Error = RarUnpackFile(Data,ID,hSeq)) )
      return(Error);
    CALL_SeqIO_Flush(hSeq);
    CALL_SYS_ObjectClose(hSeq);
  }
  else
  {
    // dupe io from temp storage
    if ( PR_FAIL(Error = GetTempFile(Data, IO, FNode)) )
      return(Error);
  }

  // flush io
  if ( PR_FAIL(Error = CALL_IO_Flush(IO)) )
    return(Error);

  // save result
  if ( NULL != hRes )
    *hRes = IO;
  return(errOK);
}

// ----------------------------------------------------------------------------

static tBOOL RAR_IsThunk(tRAR_DATA* Data)
{
  tWORD Flags;
  tUINT I;

  // check for solid volume arc (v2.0 - 3.x)
  Flags = Data->RAR.MHD_Flags;
  if ( cFALSE == (Flags & MHD_SOLID) )
    return(cFALSE);
  if ( cFALSE == (Flags & MHD_VOLUME) )
    return(cFALSE);
  if ( Flags & MHD_FIRSTVOLUME )
    return(cFALSE);
  if ( Data->FileNum == 0 )
    return(cFALSE);

  // check headers
  for ( I = 0; I < Data->BlkNum; I++ )
  {
    struct sBLOCK_NODE* Node;
    tBYTE Type;

    Node = &Data->Blks[I];
    Type = Node->BHdr.type;
    if ( Type != FILE_HEAD && Type != NEWSUB_HEAD )
      continue;
    if ( Node->BHdr.flags & LHD_SPLIT_BEFORE )
      return(cTRUE); // thunk
    break;
  }

  return(cFALSE);
}

// ----------------------------------------------------------------------------


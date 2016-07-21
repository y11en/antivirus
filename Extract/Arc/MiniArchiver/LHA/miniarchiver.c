// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Tuesday,  21 June 2005,  15:17 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- LHA MiniArchiver
// Sub project -- Not defined
// Purpose     -- LHA Archiver
// Author      -- Glavatskikh
// File Name   -- miniarchiver.c
// Additional info
// LHA Extractor/Repacker
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include "lhaext.h"

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_list.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/plugin/p_hash_md5.h>

#include <Extract/iface/i_uacall.h>
#include <Extract/plugin/p_lha.h>
#include <Extract/iface/i_minarc.h>

#include <memory.h>
#include <string.h>
// AVP Prague stamp end


/* ------------------------------------------------------------------------- */


// AVP Prague stamp begin( Data structure,  )
// Interface MiniArchiver. Inner data structure
typedef struct tag_LHAData {
  tBYTE   StartMark[8];
  hIO     hArcIO;      /* input io     */
  hSEQ_IO hSeqIO;      /* input seq_io */
  
  tQWORD  VolHash;     /* volume hash  */
  tDWORD  OpenMode;    /* open mode    */
  tDWORD  AccessMode;  /* access mode  */
  tDWORD  AvailMode;   /* avialability */
  tBOOL   FullRO;      /* readonly     */

  tQWORD  CurrFile;    /* current file in enum   */
  tQWORD  CurrProp;    /* current property id    */

  tDWORD  FileNum;     /* number of files in arc */
  tFILE_NODE* Files;   /* files array            */

  tLHA_CTX Coder;      /* lha coder/decoder */
  tBYTE    EndMark[8];
} LHAData, tLHA_DATA;
// AVP Prague stamp end


// AVP Prague stamp begin( Object declaration,  )
typedef struct tag_hi_MiniArchiver {
  const iMiniArchiverVtbl* vtbl; // pointer to the "MiniArchiver" virtual table
  const iSYSTEMVTBL*       sys;  // pointer to the "SYSTEM" virtual table
  LHAData*                 data; // pointer to the "MiniArchiver" data structure
} *hi_MiniArchiver;
// AVP Prague stamp end

/* external functions */
static tERROR InitArchive(hi_MiniArchiver _this);

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, Recognize )
// Extended method comment
//    Static method. Kernel calls this method wihtout any created objects of this interface.
//    Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//    Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//    It must be quick (preliminary) analysis
// Parameters are:
//   "type" : type of recognized object

static tERROR pr_call LHA_Recognize(hOBJECT _that, tDWORD p_type)
{
  tERROR Error = errINTERFACE_INCOMPATIBLE;
  tBYTE  Buf[8];
  tDWORD Rdt;
  
  PR_TRACE_A2( 0, "Enter MiniArchiver::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

  /* read archive header */
  if( PR_SUCC(CALL_IO_SeekRead((hIO)_that, &Rdt, 0, &Buf[0], sizeof(Buf))) )
  {
    if ( Rdt >= 7 && Buf[2] == '-' && Buf[3] == 'l' && Buf[6] == '-' )
    {
      switch( (Buf[4]<<8)|(Buf[5]) )
      {
      case ('h'<<8)|'0':  /* "-lh0-" */
      case ('h'<<8)|'1':  /* "-lh1-" */
      case ('h'<<8)|'2':  /* "-lh2-" */
      case ('h'<<8)|'3':  /* "-lh3-" */
      case ('h'<<8)|'4':  /* "-lh4-" */
      case ('h'<<8)|'5':  /* "-lh5-" */
      case ('h'<<8)|'6':  /* "-lh6-" */
      case ('h'<<8)|'7':  /* "-lh7-" */
      case ('z'<<8)|'s':  /* "-lzs-" */
      case ('z'<<8)|'5':  /* "-lz5-" */
      case ('z'<<8)|'4':  /* "-lz4-" */
      case ('h'<<8)|'d':  /* "-lhd-" */
        Error = errOK;    /* lha arc */
      }
    }
  }

  PR_TRACE_A2( 0, "Leave MiniArchiver::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:

static tERROR pr_call LHA_ObjectInit(hi_MiniArchiver _this)
{
  tLHA_DATA* Data;
  tERROR  Error;
  hIO     hArcIO;
  PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectInit method" );

  /* init context */
  Data = _this->data;
  Error = errINTERFACE_INCOMPATIBLE;
  memset(Data, 0, sizeof(tLHA_DATA));
  Data->CurrProp = 0xffffffffffffffff;
  Data->CurrFile = 0;

  /* create debug markers */
  Data->StartMark[0] = 'L'; Data->StartMark[1] = 'H';
  Data->StartMark[2] = 'A'; Data->StartMark[3] = 'S';
  Data->StartMark[4] = 'T'; Data->StartMark[5] = 'A';
  Data->StartMark[6] = 'R'; Data->StartMark[7] = 'T';

  Data->EndMark[0] = 'L'; Data->EndMark[1] = 'H';
  Data->EndMark[2] = 'A'; Data->EndMark[3] = '-';
  Data->EndMark[4] = 'E'; Data->EndMark[5] = 'N';
  Data->EndMark[6] = 'D'; Data->EndMark[7] = '\0';

  /* get archive io */
  hArcIO = (hIO)CALL_SYS_ParentGet((hOBJECT)(_this), IID_IO);
  if ( NULL == (Data->hArcIO = hArcIO) )
  {
    Error = (errOBJECT_CANNOT_BE_INITIALIZED);
    goto loc_exit;
  }

  /* extract modes from io */
  Data->OpenMode   = CALL_SYS_PropertyGetDWord(hArcIO, pgOBJECT_OPEN_MODE);
  Data->AccessMode = CALL_SYS_PropertyGetDWord(hArcIO, pgOBJECT_ACCESS_MODE);

  /* create seq_io */
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(hArcIO, &Data->hSeqIO,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  /* init engine */
  if ( PR_FAIL(Error = InitArchive(_this)) )
  {
    goto loc_exit;
  }

loc_exit:
  PR_TRACE_A1(_this, "Leave MiniArchiver::ObjectInit method, ret %terr", Error);
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
static tERROR pr_call LHA_ObjectInitDone(hi_MiniArchiver _this)
{
  tLHA_DATA* Data;
  tERROR Error;
  tDWORD Size;
  PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectInitDone method" );

  Data = _this->data;
  Size = sizeof(tQWORD);
  if ( PR_FAIL(Error = CALL_SYS_SendMsg(_this, pmc_OS_VOLUME,
    pm_OS_VOLUME_HASH, Data->hArcIO, &Data->VolHash, &Size)) )
  {
    goto loc_exit;
  }

  if ( !Data->FullRO )
  {
    Data->AvailMode = (fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND |
      fAVAIL_TRUNCATE | fAVAIL_CHANGE_MODE | fAVAIL_DELETE_ON_CLOSE);
  }
  else
  {
    Data->AvailMode = (fAVAIL_READ);
  }

  Error = errOK;

loc_exit:
  PR_TRACE_A1(_this, "Leave MiniArchiver::ObjectInitDone method, ret %terr", Error);
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
static tERROR pr_call LHA_ObjectPreClose(hi_MiniArchiver _this)
{
  tERROR Error = errOK;
  tLHA_DATA* Data = _this->data;
  PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectPreClose method" );

  /* free heap */
  if ( NULL != Data->Coder.hHeap )
  {
    CALL_SYS_ObjectClose(Data->Coder.hHeap);
    Data->Coder.hHeap = NULL;
  }

  /* free window */
  if ( NULL != Data->Coder.Window )
  {
    CALL_SYS_ObjHeapFree(_this, Data->Coder.Window);
    Data->Coder.Window = NULL;
  }

  /* free files array */
  if ( NULL != Data->Files )
  {
    CALL_SYS_ObjHeapFree(_this, Data->Files);
    Data->Files = NULL;
  }

  PR_TRACE_A1( _this, "Leave MiniArchiver::ObjectPreClose method, ret %terr", Error );
  return(errOK);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Behaviour comment
//    Reset All internal data (variables) to default or initial values. After this call enumerator must start from the very beginning
// Parameters are:
static tERROR pr_call LHA_Reset(hi_MiniArchiver _this)
{
  tERROR Error = errOK;
  tLHA_DATA* Data = _this->data;
  PR_TRACE_A0( _this, "Enter MiniArchiver::Reset method" );

  Data = (tLHA_DATA*)(_this->data);
  Data->CurrProp = (tQWORD)(0xffffffffffffffff);
  Data->CurrFile = (tQWORD)(0);

  PR_TRACE_A1( _this, "Leave MiniArchiver::Reset method, ret %terr", Error );
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Parameters are:
static tERROR pr_call LHA_Next(hi_MiniArchiver _this, tQWORD* result, tQWORD p_qwLastObjectId) 
{
  tERROR Error = errOK;
  tQWORD RetVal = 0;
  tLHA_DATA* Data = _this->data;
  PR_TRACE_A0( _this, "Enter MiniArchiver::Next method" );

  RetVal = 0;
  Error  = (errOK);
  if( NULL == result)
  {
    Error = (errPARAMETER_INVALID);
    goto loc_exit;
  }

  Data = (tLHA_DATA*)(_this->data);
  if ( p_qwLastObjectId >= Data->FileNum )
  {
    RetVal = (0);
    Error  = (errEND_OF_THE_LIST);
    goto loc_exit;
  }

  Data->CurrFile = p_qwLastObjectId;
  RetVal = (p_qwLastObjectId + 1);

loc_exit:
  if ( result != NULL )
    *result = RetVal;
  PR_TRACE_A2( _this, "Leave MiniArchiver::Next method, ret tQWORD = %I64u, %terr", RetVal, Error );
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

/* create some temp io and temp seq_io based on this io */
static tERROR CreateTempSeqIO(hi_MiniArchiver _this, hIO* hResIO,
                              hSEQ_IO* hResSeqIO)
{
  tERROR  Error;
  hSEQ_IO hNewSeqIO = NULL;
  hIO     hNewIO = NULL;

  if ( NULL == _this || NULL == hResIO )
    return(errPARAMETER_INVALID);

  hNewIO = NULL;
  if ( PR_SUCC(Error = CALL_SYS_ObjectCreate(_this, &hNewIO, IID_IO,
    PID_TMPFILE, SUBTYPE_ANY)) )
  {
    CALL_SYS_PropertySetDWord((hOBJECT)(hNewIO), pgOBJECT_ACCESS_MODE,
      fACCESS_RW);
    CALL_SYS_PropertySetDWord((hOBJECT)(hNewIO), pgOBJECT_OPEN_MODE,
      fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_READ);
    
    if ( PR_FAIL(Error = CALL_SYS_ObjectCreateDone(hNewIO)) )
    {
      CALL_SYS_ObjectClose(hNewIO);
      return(Error);
    }
  }
  else
  {
    return(Error);
  }
  
  if ( NULL == hResSeqIO )
  {
    *hResIO = hNewIO;
    return(errOK);
  }
 
  hNewSeqIO = NULL;
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(hNewIO, &hNewSeqIO,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    CALL_SYS_ObjectClose(hNewIO);
    *hResIO = NULL;
    return(Error);
  }

  *hResIO = hNewIO;
  *hResSeqIO = hNewSeqIO;
  return(errOK);
}

/* ------------------------------------------------------------------------- */

#if 0
#include <stdio.h>
static tERROR DumpSeqIO(hSEQ_IO hSrc, tCHAR* Name)
{
  tQWORD Size;
  tQWORD CurOfs;
  tBYTE  Buf[1024];
  tDWORD Cnt;
  FILE*  F;
  
  F = fopen(Name, "wb");
  CALL_SeqIO_Seek(hSrc, &CurOfs, 0, cSEEK_FORWARD);
  CALL_IO_GetSize(hSrc, &Size, IO_SIZE_TYPE_EXPLICIT);
  CALL_SeqIO_Seek(hSrc, 0, 0, cSEEK_SET);

  while ( Size )
  {
    Cnt = (tDWORD)Min(Size, sizeof(Buf));
    CALL_SeqIO_Read(hSrc, NULL, Buf, Cnt);
    fwrite(Buf, 1, Cnt, F);
    Size -= Cnt;
  }

  fclose(F);
  CALL_SeqIO_Seek(hSrc, NULL, CurOfs, cSEEK_SET);
  return(errOK);
}
#endif

// AVP Prague stamp begin( External (user called) interface method implementation, ObjCreate )
// Parameters are:
static tERROR pr_call LHA_ObjCreate(hi_MiniArchiver _this, hIO* result, tQWORD p_qwObjectIdToCreate, hIO p_hUniArcIO)
{
  tERROR Error;
  tLHA_DATA*  Data = _this->data;
  tFILE_NODE* FNode;
  hIO      RetVal;
  hSEQ_IO  hSeqOut;
  tQWORD   NxtOfs;
  tQWORD   CurOfs;
  PR_TRACE_A0( _this, "Enter MiniArchiver::ObjCreate method" );

  /* check param */
  Error   = errOK;
  RetVal  = NULL;
  hSeqOut = NULL;
  if ( NULL == result )
  {
    Error = errPARAMETER_INVALID;
    goto loc_exit;
  }
  if ( NULL == Data->Files || !Data->FileNum )
  {
    Error = errOBJECT_NOT_INITIALIZED;
    goto loc_exit;
  }

  /* find file node */
  if ( !p_qwObjectIdToCreate || p_qwObjectIdToCreate > Data->FileNum )
  {
    Error = (errOBJECT_NOT_FOUND);
    goto loc_exit;
  }

  /* check for directory */
  FNode = &Data->Files[p_qwObjectIdToCreate - 1];
  if ( FNode->IsDirectory )
  {
    Error = errNOT_OK;
    goto loc_exit;
  }

  /* create output io */
  if ( PR_FAIL(Error = CreateTempSeqIO(_this, &RetVal, &hSeqOut)) )
    goto loc_exit; /* can't create dst file */

  /* seek to raw data */
  NxtOfs = FNode->HdrOfs + FNode->HdrSize;
  if ( !PR_SUCC(Error = CALL_SeqIO_Seek(Data->hSeqIO, &CurOfs, NxtOfs, cSEEK_SET)) )
    goto loc_exit;

  /* init reader */
  Data->Coder.Error = errOK;
  Data->Coder.hWriter = hSeqOut;
  memcpy(&Data->Coder.LzHdr, &FNode->LzHdr, sizeof(tLZ_HEADER));
  if ( PR_FAIL(Error = LHA_InitFile(&Data->Coder.IO, Data->hSeqIO, NULL)) )
    goto loc_exit;

  /* extract file */
  if ( PR_FAIL(Error = LHA_ExtractOne(&Data->Coder, &FNode->LzHdr)) )
  {
    Data->FullRO = cTRUE;
    FNode->ReadOnly = cTRUE;
    goto loc_exit;
  }

#if 0
  DumpSeqIO(hSeqOut, "extracted.bin");
#endif

  /* set avialability */
  if ( NULL != RetVal )
  {
    tBOOL CanDel;
    
    CanDel = (Data->FullRO || FNode->ReadOnly || FNode->LzHdr.crc_fail ) ?
      (cFALSE) : (cTRUE);

    if ( PR_FAIL(Error = CALL_SYS_PropertySet(RetVal, 0,
      pgOBJECT_DELETE_ON_CLOSE, &CanDel, sizeof(tBOOL))) )
    {
      goto loc_exit;
    }
  }

  /* all ok */
  Error = errOK;

loc_exit:
  if ( NULL != hSeqOut )
  {
    CALL_SYS_ObjectClose(hSeqOut);
    hSeqOut = NULL;
  }
  if ( Error != errOK )
  {
    /* close bad io's */
    if ( NULL != RetVal )
    {
      CALL_SYS_ObjectClose(RetVal);
      RetVal = NULL;
    }
  }
  if ( result )
    *result = RetVal;
  PR_TRACE_A2( _this, "Leave MiniArchiver::ObjCreate method, ret hIO = %p, %terr", RetVal, Error );
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, SetAsCurrent )
// Parameters are:
static tERROR pr_call LHA_SetAsCurrent(hi_MiniArchiver _this, tQWORD p_qwObjectId) 
{
  tERROR Error;
  tLHA_DATA* Data = _this->data;
  PR_TRACE_A0( _this, "Enter MiniArchiver::SetAsCurrent method" );

  Error = errOK;
  Data->CurrFile = p_qwObjectId;

  PR_TRACE_A1( _this, "Leave MiniArchiver::SetAsCurrent method, ret %terr", Error );
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, RebuildArchive )
// Parameters are:
static tERROR pr_call LHA_RebuildArchive(hi_MiniArchiver _this, hUNIARCCALLBACK p_hCallBack, hIO p_hOutputIo) 
{
  tERROR  Error;
  hIO     hInpIO;
  hSEQ_IO hSeqIn  = NULL;
  hSEQ_IO hSeqSrc = NULL;
  hSEQ_IO hSeqOut = NULL;
  tLHA_DATA* Data = _this->data;
  tFILE_NODE* FNode;
  tQWORD OutOfs;
  tDWORD BlkLen;
  tDWORD Wrt;
  tUINT  I;
  PR_TRACE_A0( _this, "Enter MiniArchiver::RebuildArchive method" );

  /* create io */
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(Data->hArcIO, &hSeqSrc,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(p_hOutputIo, &hSeqOut,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  /* init coder */
  Data->Coder.Glob.method = -1;

  /* enumerate and write archive blocks */
  for ( I = 0, OutOfs = 0; I < Data->FileNum; I++ )
  {
    FNode = &Data->Files[I];

    /* check if present in deleted list */
    if( PR_SUCC(Error = CALL_UniArchiverCallback_FindObjectInList(p_hCallBack,
        (hOBJECT*)(&hInpIO), cDELETED_OBJECT, I+1)) )
    {
      /* skip block */
      continue;
    }

    /* check for presence in modifyed list  */
    if( PR_SUCC(Error = CALL_UniArchiverCallback_FindObjectInList(p_hCallBack,
        (hOBJECT*)(&hInpIO), cCHANGED_OBJECT, I+1)) )
    {
      /* create dst seq io */
      if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(hInpIO, &hSeqIn,
        IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
      {
        goto loc_exit;
      }

      /* create pack heap if needed */
      if ( !Data->Coder.hHeap && PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(
        _this, &Data->Coder.hHeap, IID_HEAP, PID_ANY, SUBTYPE_ANY)) )
      {
        goto loc_exit;
      }

      /* pack */
      Error = LHA_CompressOne(&Data->Coder, FNode, hSeqSrc, hSeqIn, hSeqOut,
        OutOfs, &Wrt);
      CALL_SYS_ObjectClose(hSeqIn); hSeqIn = NULL;
      if ( PR_FAIL(Error) )
        goto loc_exit;

      OutOfs += Wrt;
      continue;
    }

    /* copy block */
    BlkLen = FNode->HdrSize + FNode->LzHdr.packed_size;
    if ( PR_FAIL(Error = CopySeqIOBlock(&Data->Coder, hSeqSrc, hSeqOut,
      FNode->HdrOfs, OutOfs, BlkLen)) )
    {
      goto loc_exit;
    }

    OutOfs += BlkLen;
  }

  /* write last zero header */
  Data->Coder.TmBuf[0] = 0;
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(hSeqOut, NULL, OutOfs, cSEEK_SET)) )
    goto loc_exit;
  Error = CALL_SeqIO_Write(hSeqOut, &Wrt, &Data->Coder.TmBuf[0], 1);
  if ( PR_FAIL(Error) )
    goto loc_exit;
  if ( PR_FAIL(Error = CALL_SeqIO_SetSize(hSeqOut, OutOfs + 1)) )
    goto loc_exit;

  /* done */
  Error = errOK;

loc_exit:
  if ( NULL != Data->Coder.hHeap )
  {
     CALL_SYS_ObjectClose(Data->Coder.hHeap);
    Data->Coder.hHeap = NULL;
  }
  if ( NULL != hSeqIn )
    CALL_SYS_ObjectClose(hSeqIn);
  if ( NULL != hSeqOut )
    CALL_SYS_ObjectClose(hSeqOut);
  if ( NULL != hSeqSrc )
    CALL_SYS_ObjectClose(hSeqSrc);
  PR_TRACE_A1( _this, "Leave MiniArchiver::RebuildArchive method, ret %terr", Error );
  return(Error);
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

static tERROR CalcCountHash(hHASH hHash, tBYTE* Buff, tUINT Cnt, tQWORD* Seed)
{
  tERROR Error;
  tBYTE  Hash[16];

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

/* ------------------------------------------------------------------------- */

static tERROR InitArchive(hi_MiniArchiver _this)
{
  tLHA_DATA*  Data;
  tFILE_NODE* FNode;
  tLISTNODE   LNode;
  tERROR Error;
  hLIST  hList = NULL;
  hHASH  hHash = NULL;
  tUINT  RotCnt = 0;
  tBYTE  HBuf[32];
  tQWORD HdrOfs;
  tQWORD NxtOfs;
  tDWORD Size;
  tDWORD I;
  tBOOL  Stop;

  /* link contextes */
  Data = _this->data;
  Data->Coder.hParent = (hOBJECT)(_this);
  Data->Coder.IO.Lha  = &Data->Coder;

  /* preinit */
  Data->Files   = NULL;
  Data->VolHash = 0xffffffffffffffff;
  Data->FileNum = 0;

  /* seek input to begin */
  if ( PR_FAIL(Error = CALL_SeqIO_Seek(Data->hSeqIO, NULL, 0, cSEEK_SET)) )
    return(Error);

  /* init file system */
  Data->Coder.Error = errOK;
  if ( PR_FAIL(Error = LHA_InitFile(&Data->Coder.IO, Data->hSeqIO, NULL)) )
    goto loc_exit;

  /* create hash */
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hHash,
    IID_HASH, PID_HASH_MD5, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  /* create blocks list */
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &hList, IID_LIST,
    PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  Stop = cFALSE;
  while( !Stop )
  {
    /* send yelding message */
    if ( !(RotCnt++ & 0x3f) && errOK != (Error = LHA_Rotate(&Data->Coder)) )
      goto loc_exit; /* stopped */

    /* get header offset */
    HdrOfs = LHA_Tell(&Data->Coder.IO);

    /* try parse header */
    memset(&Data->Coder.LzHdr, 0, sizeof(tLZ_HEADER));
    Error = LHA_GetHeader(&Data->Coder.IO, &Data->Coder.LzHdr);
    switch( Error )
    {
    case errOK:
      break; /* to next block */
    case errOPERATION_CANCELED:
      goto loc_exit; /* stopped */
    default:
      Error = errOK;
      Stop = cTRUE;
      continue; /* and of arc */
    }

    /* build file info */
    FNode = (tFILE_NODE*)(&Data->Coder.TmBuf[0]);
    memset(FNode, 0, sizeof(tFILE_NODE));
    memcpy(&FNode->LzHdr, &Data->Coder.LzHdr, sizeof(tLZ_HEADER));
    FNode->HdrSize  = (tDWORD)(LHA_Tell(&Data->Coder.IO) - HdrOfs);
    FNode->ReadOnly = (FNode->LzHdr.hdr_parse_fail) ? (cTRUE) : (cFALSE);
    FNode->HdrOfs   = HdrOfs;

    /* check for directory */
    if ( 0 == memcmp(&FNode->LzHdr.method, "-lhd-", METHOD_TYPE_STRAGE) )
      FNode->IsDirectory = cTRUE;

    /* update hash by header */
    if ( PR_FAIL(Error = CalcCountHash(hHash, (tBYTE*)&Data->Coder.LzHdr,
      sizeof(tLZ_HEADER), &Data->VolHash)) )
    {
      goto loc_exit;
    }

    /* update hash by start file data */
    Size = (tDWORD)Min(Data->Coder.LzHdr.packed_size, sizeof(HBuf));
    Size = LHA_ReadBlock(&Data->Coder.IO, &HBuf[0], Size);
    if ( PR_FAIL(Error = CalcCountHash(hHash, &HBuf[0], Size, &Data->VolHash)) )
    {
      goto loc_exit;
    }

    /* update hash by end file data */
    if ( Data->Coder.LzHdr.packed_size > sizeof(HBuf) )
    {
      NxtOfs = HdrOfs + FNode->HdrSize + Data->Coder.LzHdr.packed_size - sizeof(HBuf);
      if ( NxtOfs != LHA_Seek(&Data->Coder.IO, NxtOfs) )
      {
        Stop = cTRUE;
        continue; /* out of object */
      }
      Size = LHA_ReadBlock(&Data->Coder.IO, &HBuf[0], sizeof(HBuf));
      if ( PR_FAIL(Error = CalcCountHash(hHash, &HBuf[0], Size, &Data->VolHash)) )
      {
        goto loc_exit;
      }
    }

    /* save node to list */
    if ( PR_FAIL(Error = CALL_List_Add(hList, NULL, FNode,
      sizeof(tFILE_NODE), 0, cLIST_LAST)) )
    {
      goto loc_exit;
    }

    /* seek to next block */
    NxtOfs = HdrOfs + FNode->HdrSize + FNode->LzHdr.packed_size;
    if ( NxtOfs != LHA_Seek(&Data->Coder.IO, NxtOfs) )
    {
      Stop = cTRUE;
      continue; /* out of object */
    }

    /* next file */
    Data->FileNum++;
  }

  /* check count */
  if ( !Data->FileNum )
  {
    Error = errOUT_OF_OBJECT;
    goto loc_exit;
  }

  /* alloc file array */
  Size = ((Data->FileNum + 1) * sizeof(tFILE_NODE)) + 64;
  if ( PR_FAIL(Error = CALL_SYS_ObjHeapAlloc(_this, (tPTR)&Data->Files, Size)) )
  {
    Error = errNOT_ENOUGH_MEMORY;
    goto loc_exit;
  }

  /* get files from list */
  memset(Data->Files, 0, Size); I = 0;
  if ( PR_SUCC(Error = CALL_List_First(hList, &LNode)) )
  {
    do
    {
      FNode = (tFILE_NODE*)(&Data->Files[I]);
      if ( PR_FAIL(Error = CALL_List_DataGet(hList, NULL, LNode,
        FNode, sizeof(tFILE_NODE))) )
      {
        goto loc_exit;
      }

      if ( (I += 1) >= Data->FileNum )
        break; /* done */
    }
    while( PR_SUCC(CALL_List_Next(hList, &LNode, LNode)) );
  }
  else
  {
    return(Error);
  }

  /* check count */
  if ( I != Data->FileNum )
  {
    Error = errNOT_OK;
    goto loc_exit;
  }

  Error = errOK;

loc_exit:
  if ( NULL != hList )
    CALL_SYS_ObjectClose(hList);
  if ( NULL != hHash )
    CALL_SYS_ObjectClose(hHash);
  return(Error);
}


/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MiniArchiver". Static(shared) property table variables
static const tVERSION cVersion = 1; // must be READ_ONLY at runtime
static const tSTRING  szComment = "LHA Archiver"; // must be READ_ONLY at runtime
static const tBOOL    cbNameCaseSens = cFALSE; // must be READ_ONLY at runtime
static const tBOOL    cbArcSolid  = cFALSE; // must be READ_ONLY at runtime
static const tDWORD   cMaxNameLen = MAX_LHA_NAME; // must be READ_ONLY at runtime
// AVP Prague stamp end

/* check property presence */
static tERROR CheckPropObj(tLHA_DATA* Data, tPROPID Prop, tQWORD Obj)
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
  case pgOBJECT_PASSWORD_PROTECTED:
    if ( NULL == Data->Files )
      return(errOBJECT_NOT_INITIALIZED);
    if ( Obj > Data->FileNum )
      return(errNOT_FOUND);
    break;
  }

  return(errOK);
}

/* ------------------------------------------------------------------------- */

/* get plugin property */
static tERROR pr_call LHA_PropGet(hi_MiniArchiver _this, tDWORD* out_size, 
                                  tPROPID prop, tCHAR* buffer, tDWORD size)
{
  tDWORD Size;
  tERROR Error;
  tQWORD CurProp;
  tLHA_DATA* Data;
  tCHAR* Name;
  tCHAR* NPtr;
  tINT   NLen;
  PR_TRACE_A1(_this, "Enter *GET* method RAR_PropGet for property %u", prop);
    
  Size  = 0;
  Error = (errOK);
  Data  = (tLHA_DATA*)(_this->data);
  if ( NULL != buffer && 0 != size )
  {
    /* check current property ptr */
    if ( Data->CurrProp == (0xFFFFFFFFFFFFFFFF) )
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

      /* parse name (find slash) */
      Name = (tCHAR*)(Data->Files[Data->CurrProp].LzHdr.name);
      if ( Name[0] == '\\' || Name[0] == '/' ) Name++;
      NPtr = &Name[NLen = _toi32(strlen(Name))];
      while( NPtr > Name && (NPtr[-1] != '\\' && NPtr[-1] != '/') )
        --NPtr;

      /* save name part */
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

      Name = Data->Files[Data->CurrProp].LzHdr.name;
      if ( Name[0] == '\\' || Name[0] == '/' ) Name++;
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

      /* parse name (find slash) */
      Name = (tCHAR*)(&Data->Files[Data->CurrProp].LzHdr.name[0]);
      if ( Name[0] == '\\' || Name[0] == '/' ) Name++;
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
      *(tQWORD*)(buffer) = (tQWORD)(Data->Files[Data->CurrProp].LzHdr.original_size);
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
        /* return prop for current file */
        if ( CurProp < Data->FileNum )
          *(tBOOL*)(buffer) = Data->Files[CurProp].ReadOnly;
        else
          *(tBOOL*)(buffer) = (cTRUE);
      }
      else
      {
        /* whole file readonly */
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
      *(tDWORD*)(buffer) = Data->AvailMode;
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
    case pgIS_FOLDER:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = sizeof(tBOOL);
      if ( size < sizeof(tBOOL) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tBOOL*)(buffer) = Data->Files[Data->CurrProp].IsDirectory;
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
    case pgOBJECT_PASSWORD_PROTECTED:
      if ( PR_FAIL(Error = CheckPropObj(Data, prop, CurProp)) )
        goto loc_exit;
      Size = sizeof(tBOOL);
      if ( size < sizeof(tBOOL) )
      {
        Error = (errBUFFER_TOO_SMALL);
        goto loc_exit;
      }
      *(tBOOL*)(buffer) = cFALSE;
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
    case pgOBJECT_PATH:
    case pgOBJECT_VOLUME_NAME:
      Size = MAX_LHA_NAME;
    case pgOBJECT_FULL_NAME:
      Size = 2*MAX_LHA_NAME;
      break;
    case pgOBJECT_HASH:
    case pgOBJECT_SIZE_Q:
      Size = sizeof(tQWORD);
      break;
    case pgIS_READONLY:
    case pgIS_FOLDER:
    case pgOBJECT_PASSWORD_PROTECTED:
      Size = sizeof(tBOOL);
      break;
    case pgOBJECT_OPEN_MODE:
    case pgOBJECT_ATTRIBUTES:
    case pgOBJECT_ACCESS_MODE:
    case pgOBJECT_AVAILABILITY:
      Size = sizeof(tDWORD);
      break;
    case pgOBJECT_ORIGIN:
      Size = sizeof(tORIG_ID);
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
  Data->CurrProp = (0xFFFFFFFFFFFFFFFF);
  PR_TRACE_A3(_this, "Leave *GET* method RAR_PropGet for property %u, ret tDWORD = %u(size), error = 0x%08x", prop, Size, Error);
  return(Error);
}

/* ------------------------------------------------------------------------- */

static tERROR pr_call LHA_PropSet(hi_MiniArchiver _this, tUINT* out_size,
                                  tPROPID prop, tCHAR* buffer, tUINT size)
{
  tDWORD Size;
  tERROR Error;
  tLHA_DATA*  Data;
  PR_TRACE_A1(_this, "Enter *SET* method RAR_PropSet for property %u", prop);

  Size  = 0;
  Error = (errOK);
  Data  = (tLHA_DATA*)(_this->data);
  if ( NULL != buffer && 0 != size )
  {
    switch( prop )
    {
    case pgMINIARC_PROP_OBJECT_ID:
      Data->CurrProp = *(tQWORD*)(buffer);
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

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(MiniArchiver_PropTable)
  mSHARED_PROPERTY_PTR ( pgINTERFACE_VERSION, cVersion, sizeof(cVersion) )
  mSHARED_PROPERTY_VAR ( pgINTERFACE_COMMENT, szComment, 13 )
  mSHARED_PROPERTY_PTR ( pgNAME_CASE_SENSITIVE, cbNameCaseSens, sizeof(cbNameCaseSens) )
  mSHARED_PROPERTY_PTR ( pgNAME_MAX_LEN, cMaxNameLen, sizeof(cMaxNameLen) )
  mSHARED_PROPERTY_PTR ( pgIS_SOLID, cbArcSolid,     sizeof(cbArcSolid) )
  mSHARED_PROPERTY     ( pgOBJECT_OS_TYPE,          ((tOS_ID)OS_TYPE_GENERIC) )

  mSHARED_PROPERTY     ( pgOBJECT_NAME_CP,           ((tCODEPAGE)(cCP_OEM))    )
  mSHARED_PROPERTY     ( pgOBJECT_PATH_CP,           ((tCODEPAGE)(cCP_OEM))    )
  mSHARED_PROPERTY     ( pgOBJECT_FULL_NAME_CP,      ((tCODEPAGE)(cCP_OEM))    )

  mLOCAL_PROPERTY_FN   ( pgOBJECT_NAME,               LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_PATH,               LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_FULL_NAME,          LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_SIZE_Q,             LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_ATTRIBUTES,         LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_HASH,               LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgIS_READONLY,               LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_AVAILABILITY,       LHA_PropGet, NULL        )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_ORIGIN,             LHA_PropGet, NULL        )

  mLOCAL_PROPERTY_FN   ( pgOBJECT_OPEN_MODE,          LHA_PropGet, LHA_PropSet )
  mLOCAL_PROPERTY_FN   ( pgOBJECT_ACCESS_MODE,        LHA_PropGet, LHA_PropSet )
  mLOCAL_PROPERTY_FN   ( pgMINIARC_PROP_OBJECT_ID,    LHA_PropGet, LHA_PropSet )
  mLOCAL_PROPERTY_FN   ( pgIS_FOLDER,                 LHA_PropGet, LHA_PropSet )
mPROPERTY_TABLE_END(MiniArchiver_PropTable)
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_MiniArchiver_vtbl = 
{
  (tIntFnRecognize)         LHA_Recognize,
  (tIntFnObjectNew)         NULL,
  (tIntFnObjectInit)        LHA_ObjectInit,
  (tIntFnObjectInitDone)    LHA_ObjectInitDone,
  (tIntFnObjectCheck)       NULL,
  (tIntFnObjectPreClose)    LHA_ObjectPreClose,
  (tIntFnObjectClose)       NULL,
  (tIntFnChildNew)          NULL,
  (tIntFnChildInitDone)     NULL,
  (tIntFnChildClose)        NULL,
  (tIntFnMsgReceive)        NULL,
  (tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end


// AVP Prague stamp begin( External method table,  )
static iMiniArchiverVtbl e_MiniArchiver_vtbl = 
{
  (fnpMiniArchiver_Reset)          LHA_Reset,
  (fnpMiniArchiver_Next)           LHA_Next,
  (fnpMiniArchiver_ObjCreate)      LHA_ObjCreate,
  (fnpMiniArchiver_SetAsCurrent)   LHA_SetAsCurrent,
  (fnpMiniArchiver_RebuildArchive) LHA_RebuildArchive
};
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Registration call,  )
// Interface "MiniArchiver". Register function
tERROR pr_call MiniArchiver_Register( hROOT root ) 
{
  tERROR error;

  PR_TRACE_A0( root, "Enter MiniArchiver::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_MINIARCHIVER,                       // interface identifier
    PID_LHA,                                // plugin identifier
    0x00000000,                             // subtype identifier
    cVersion,                               // interface version
    VID_GLAV,                               // interface developer
    &i_MiniArchiver_vtbl,                   // internal(kernel called) function table
    (sizeof(i_MiniArchiver_vtbl)/sizeof(tPTR)),// internal function table size
    &e_MiniArchiver_vtbl,                   // external function table
    (sizeof(e_MiniArchiver_vtbl)/sizeof(tPTR)),// external function table size
    MiniArchiver_PropTable,                 // property table
    mPROPERTY_TABLE_SIZE(MiniArchiver_PropTable),// property table size
    sizeof(LHAData),                        // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"MiniArchiver(IID_MINIARCHIVER) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave MiniArchiver::Register method, ret %terr", error );
  return error;
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */




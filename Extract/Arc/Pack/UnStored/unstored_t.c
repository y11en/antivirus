// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  16 March 2005,  18:49 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Unstored decompressor
// Author      -- Dmitry Glavatskikh
// File Name   -- unstored_t.c
// Additional info
//    Unstored decompressor. (Just copy)
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Transformer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>

#include <Extract/plugin/p_unstore.h>

#include <memory.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure

typedef struct tag_tUNSTORE_DATA 
{
  hSEQ_IO   hInputIO;        // --
  hSEQ_IO   hOutputIO;       // --
  tQWORD    qwTransformSize; // --
  tBYTE*    WrkBuff;         // --
  tBYTE*    IntBuff;         // --
  tPTR      CBUser;          // --
  tDWORD    WrkBuffSize;     // --
  tDWORD    IntBuffSize;     // --
  fTRANSFORMER_CALLBACK InputCB;         // --
  fTRANSFORMER_CALLBACK OutputCB;        // --
} tUNSTORE_DATA;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
  const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
  const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
  tUNSTORE_DATA*          data; // pointer to the "Transformer" data structure
} *hi_Transformer;

// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Transformer". Static(shared) property table variables
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
static tERROR pr_call Transformer_ObjectInit( hi_Transformer _this )
{
  tERROR Error = errOK;
  tUNSTORE_DATA* Data;
  PR_TRACE_A0( _this, "Enter Transformer::ObjectInit method" );

  Data = _this->data;
  memset(Data, 0, sizeof(tUNSTORE_DATA));

  PR_TRACE_A1( _this, "Leave Transformer::ObjectInit method, ret %terr", Error );
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
static tERROR pr_call Transformer_ObjectInitDone( hi_Transformer _this )
{
  tERROR Error = errOK;
  PR_TRACE_A0( _this, "Enter Transformer::ObjectInitDone method" );

  /* nothing to do */

  PR_TRACE_A1( _this, "Leave Transformer::ObjectInitDone method, ret %terr", Error );
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
static tERROR pr_call Transformer_ObjectPreClose( hi_Transformer _this )
{
  tERROR Error = errOK;
  tUNSTORE_DATA* Data;
  PR_TRACE_A0( _this, "Enter Transformer::ObjectPreClose method" );

  Data = _this->data;
  if ( NULL != Data->IntBuff )
  {
    CALL_SYS_ObjHeapFree(_this, Data->IntBuff);
    Data->IntBuff = NULL;
  }

  PR_TRACE_A1( _this, "Leave Transformer::ObjectPreClose method, ret %terr", error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

#ifndef MIN
# define MIN(a,b)  ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b)  ((a)>(b)?(a):(b))
#endif

#define INT_BUFF_SIZE  (32*1024)

/* ------------------------------------------------------------------------- */

static tERROR UnstoreCopyData(hi_Transformer _this,tQWORD* Result,tQWORD Size)
{
  tUNSTORE_DATA* Data;
  hSEQ_IO hSrc = NULL;
  hSEQ_IO hDst = NULL;
  tERROR  Error  = errOK;
  tDWORD  Cur, Rdt, Wrt;
  tQWORD  RetVal = 0;
  tBYTE*  Buff = NULL;
  tUINT   BuffSize;
  tUINT   RotCnt;

  /* check params */
  Data = _this->data;
  if ( (!Data->hInputIO && !Data->InputCB) )
    return(errOBJECT_NOT_INITIALIZED);
  if ( (!Data->hOutputIO && !Data->OutputCB) )
    return(errOBJECT_NOT_INITIALIZED);

  /* get work buffer */
  if ( NULL != (Buff = Data->WrkBuff) )
    BuffSize = (tUINT)Data->WrkBuffSize;
  else if ( NULL != (Buff = Data->IntBuff) )
    BuffSize = (tUINT)Data->IntBuffSize;

  /* alloc buffer if needed */
  if ( NULL == Buff || BuffSize == 0 )
  {
    if ( PR_FAIL(Error = CALL_SYS_ObjHeapAlloc(_this, &Buff, INT_BUFF_SIZE)) )
      goto loc_exit; /* no memory */

    Data->IntBuff = Buff;
    Data->IntBuffSize = INT_BUFF_SIZE;
    BuffSize = INT_BUFF_SIZE;
  }

  RotCnt = 0;
  hSrc = Data->hInputIO;
  hDst = Data->hOutputIO;

  /* decode loop */
  while( Size != 0 )
  {
    /* check for stop */
    if ( !(++RotCnt & 0x1f) )
    {
      hSEQ_IO hSeqIo;

      if ( NULL != (hSeqIo = (hSrc != NULL) ? (hSrc) : (hDst)) )
      {
        /* send yeld message */
        if ( errOPERATION_CANCELED == (Error = CALL_SYS_SendMsg(hSeqIo,
          pmc_PROCESSING, pm_PROCESSING_YIELD, _this, 0 ,0)) )
        {
          goto loc_exit; /* stopped */
        }

        Error = errOK;
      }
    }

    Cur = (tDWORD)MIN(Size, BuffSize);

    /* read block */
    if ( NULL == hSrc )
      Rdt = Cur;
    else
    {
      Error = CALL_SeqIO_Read(hSrc, &Rdt, Buff, Cur);
      if ( PR_FAIL(Error) || Rdt == 0 )
        goto loc_exit; /* done */
    }
    if ( NULL != Data->InputCB )
    {
      if ( 0 == (Rdt = Data->InputCB(Data->CBUser, Buff, Rdt)) )
        goto loc_exit; /* done */
    }

    /* write block */
    if ( NULL == Data->OutputCB )
      Wrt = Rdt;
    else
    {
      if ( 0 == (Wrt = Data->OutputCB(Data->CBUser, Buff, Rdt)) )
        goto loc_exit; /* done */
    }
    if ( NULL != hDst )
    {
      Error = CALL_SeqIO_Write(hDst, &Wrt, Buff, Wrt);
      if ( PR_FAIL(Error) || Wrt == 0 )
        goto loc_exit; /* done */
    }

    RetVal += Wrt;
    Size   -= Cur;

    if ( Wrt != Cur )
      break; /* done */
  }

  if ( NULL != hDst )
    CALL_SeqIO_Flush(hDst);
  Error = errOK;

loc_exit:
  *Result = RetVal;
  return(Error);
}

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessChunck )
// Result comment
//    количество реально выданных байт (>=dwSize)
//
// Parameters are:
static tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* Result, tDWORD dwSize )
{
  tERROR Error  = errOK;
  tQWORD RetVal = 0;
  PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

  if ( PR_FAIL(Error = UnstoreCopyData(_this, &RetVal, dwSize)) )
    RetVal = 0;

  if ( Result != NULL )
    *Result = (tDWORD)(RetVal);
  PR_TRACE_A2( _this, "Leave Transformer::ProcessChunck method, ret tDWORD = %u, %terr", (tDWORD)RetVal, Error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, Proccess )
// Result comment
//    размер распакованных данных
// Parameters are:
static tERROR pr_call Transformer_Proccess( hi_Transformer _this, tQWORD* Result )
{
  tUNSTORE_DATA* Data;
  tERROR Error = errOK;
  tQWORD RetVal = 0;
  PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

  /* get input size */
  Data = _this->data;
  if ( NULL != Data->hInputIO )
	  CALL_IO_GetSize((hIO)Data->hInputIO,  &RetVal, IO_SIZE_TYPE_EXPLICIT);
  else
    RetVal = Data->qwTransformSize;

  /* set output size */
  if ( NULL != Data->hOutputIO )
	  CALL_IO_SetSize((hIO)Data->hOutputIO, RetVal);

  /* decode data */
  if ( PR_FAIL(Error = UnstoreCopyData(_this, &RetVal, RetVal)) )
    RetVal = 0;

  if ( Result != NULL )
    *Result = RetVal;
  PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, %terr", RetVal, Error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessQChunck )
// Parameters are:
static tERROR pr_call Transformer_ProcessQChunck( hi_Transformer _this, tQWORD* Result, tQWORD qwSize )
{
  tERROR Error  = errOK;
  tQWORD RetVal = 0;
  PR_TRACE_A0( _this, "Enter Transformer::ProcessQChunck method" );

  if ( PR_FAIL(Error = UnstoreCopyData(_this, &RetVal, qwSize)) )
    RetVal = 0;

  if ( Result != NULL )
    *Result = RetVal;
  PR_TRACE_A2( _this, "Leave Transformer::ProcessQChunck method, ret tQWORD = %I64u, %terr", RetVal, Error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, Decode )
// Parameters are:
static tERROR pr_call Transformer_Decode(hi_Transformer _this, tQWORD* Result)
{
  tUNSTORE_DATA* Data;
  tERROR Error  = errOK;
  tQWORD RetVal = 0;
  PR_TRACE_A0( _this, "Enter Transformer::Decode method" );

  Data  = _this->data;
  Error = UnstoreCopyData(_this, &RetVal, Data->qwTransformSize);
  if ( PR_FAIL(Error) )
    RetVal = 0;

  if ( Result != NULL )
    *Result = RetVal;
  PR_TRACE_A2( _this, "Leave Transformer::Decode method, ret tQWORD = %I64u, %terr", RetVal, Error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Parameters are:
static tERROR pr_call Transformer_Reset( hi_Transformer _this, tINT Method )
{
  tERROR Error = errOK;
  tUNSTORE_DATA* Data;
  PR_TRACE_A0( _this, "Enter Transformer::Reset method" );

  Data = _this->data;
  switch( Method )
  {
  case UNSTORED_RESET_STATE:
    Data->qwTransformSize = 0;
    break;
  case UNSTORED_RESET_FULL:
    if ( NULL != Data->IntBuff )
    {
      CALL_SYS_ObjHeapFree(_this, Data->IntBuff);
      Data->IntBuff = NULL;
    }
    memset(Data, 0, sizeof(tUNSTORE_DATA));
    break;
  default:
    Error = errNOT_IMPLEMENTED;
  }

  PR_TRACE_A1( _this, "Leave Transformer::Reset method, ret %terr", Error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, Transformer_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Unstored transformer", 21 )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, tUNSTORE_DATA, qwTransformSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, tUNSTORE_DATA, hInputIO, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, tUNSTORE_DATA, hOutputIO, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_BUFF, tUNSTORE_DATA, WrkBuff, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_BUFF_SIZE, tUNSTORE_DATA, WrkBuffSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_BUFF, tUNSTORE_DATA, WrkBuff, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_BUFF_SIZE, tUNSTORE_DATA, WrkBuffSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_WINDOW, tUNSTORE_DATA, WrkBuff, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_WINDOW_SIZE, tUNSTORE_DATA, WrkBuffSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_CALLBACK, tUNSTORE_DATA, InputCB, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_CALLBACK, tUNSTORE_DATA, OutputCB, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgCALLBACK_USER, tUNSTORE_DATA, CBUser, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_INPUT_SIZE, tUNSTORE_DATA, qwTransformSize, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(Transformer_PropTable)
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Transformer_vtbl = 
{
  (tIntFnRecognize)         NULL,
  (tIntFnObjectNew)         NULL,
  (tIntFnObjectInit)        Transformer_ObjectInit,
  (tIntFnObjectInitDone)    Transformer_ObjectInitDone,
  (tIntFnObjectCheck)       NULL,
  (tIntFnObjectPreClose)    Transformer_ObjectPreClose,
  (tIntFnObjectClose)       NULL,
  (tIntFnChildNew)          NULL,
  (tIntFnChildInitDone)     NULL,
  (tIntFnChildClose)        NULL,
  (tIntFnMsgReceive)        NULL,
  (tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iTransformerVtbl e_Transformer_vtbl = 
{
  (fnpTransformer_ProcessChunck) Transformer_ProcessChunck,
  (fnpTransformer_Proccess) Transformer_Proccess,
  (fnpTransformer_ProcessQChunck) Transformer_ProcessQChunck,
  (fnpTransformer_Decode)   Transformer_Decode,
  (fnpTransformer_Reset)    Transformer_Reset
};
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Registration call,  )
// Interface "Transformer". Register function
tERROR pr_call Transformer_Register( hROOT root ) 
{
  tERROR error;

  PR_TRACE_A0( root, "Enter Transformer::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_TRANSFORMER,                        // interface identifier
    PID_UNSTORE,                            // plugin identifier
    0x00000000,                             // subtype identifier
    Transformer_VERSION,                    // interface version
    VID_ANDY,                               // interface developer
    &i_Transformer_vtbl,                    // internal(kernel called) function table
    (sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
    &e_Transformer_vtbl,                    // external function table
    (sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
    Transformer_PropTable,                  // property table
    mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
    sizeof(tUNSTORE_DATA),                  // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"Transformer(IID_TRANSFORMER) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave Transformer::Register method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgFORCE_WINDOW_CREATION
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

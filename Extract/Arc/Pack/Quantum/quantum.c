// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  06 February 2004,  15:03 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Quantum Decompressor
// Author      -- Dmitry Glavatskikh
// File Name   -- quantum.c
// Additional info
// Quantum Decompressor for MS CAB Archives
// -------------------------------------------
// AVP Prague stamp end

// ----------------------------------------------------------------------------

// header includes
#include <prague.h>
#include <iface/i_root.h>
#include <iface/i_seqio.h>
#include <memory.h>
#include "quantum.h"
#include "../UnLZX/plugin_lzxdecompressor.h"

#include <stdarg.h>
PR_MAKE_TRACE_FUNC;

#if !defined _DEBUG
# define QATTRIB  __fastcall
# define QINLINE  __inline
#else
# define QATTRIB
# define QINLINE
#endif

// ----------------------------------------------------------------------------

// quantum model symbol
typedef struct sQTM_MODELSYM {
  tWORD sym;
  tWORD cumfreq;
} tQTM_MODELSYM, *pQTM_MODELSYM;

// quantum model
typedef struct sQTM_MODEL {
  tINT   shiftsleft;
  tINT   entries; 
  struct sQTM_MODELSYM* syms;
  tWORD  tabloc[256];
} tQTM_MODEL, pQTM_MODEL;

#define CAB_BLOCKMAX (32768)
#define CAB_INPUTMAX (CAB_BLOCKMAX+6144)

// quantum state
typedef struct sQTM_STATE {
  tBYTE*  window;          // the actual decoding window
  tUINT   window_size;     // window size (1kb ... 2mb)
  tUINT   window_posn;     // current offset within window
  tUINT   cur_inp;         // current offset within input bufer
  tUINT   cur_out;         // current offset within output bufer
  tUINT   max_inp;         // max bytes to read
  tUINT   max_out;         // max bytes to write

  tBYTE   inp_buff[CAB_INPUTMAX+4];

  tBYTE   c_q_length_base[27];
  tBYTE   c_q_length_extra[27];
  tBYTE   c_q_extra_bits[42];
  tUINT   c_q_position_base[42];

  struct  sQTM_MODEL    model7;
  struct  sQTM_MODELSYM m7sym[7+1];

  struct  sQTM_MODEL    model4, model5, model6pos, model6len;
  struct  sQTM_MODELSYM m4sym[0x18 + 1];
  struct  sQTM_MODELSYM m5sym[0x24 + 1];
  struct  sQTM_MODELSYM m6psym[0x2a + 1], m6lsym[0x1b + 1];

  struct  sQTM_MODEL    model00, model40, model80, modelC0;
  struct  sQTM_MODELSYM m00sym[0x40 + 1], m40sym[0x40 + 1];
  struct  sQTM_MODELSYM m80sym[0x40 + 1], mC0sym[0x40 + 1];
} tQTM_STATE, *pQTM_STATE;

// data structures
typedef struct sQTM_DATA {
  hSEQ_IO hInputSeqIo;
  hSEQ_IO hOutputSeqIo;
  tQWORD  qwOutputSize;
  tDWORD  dwInputSize;
  tDWORD  dwWndSize;
  struct  sQTM_STATE QState;
} tQTM_DATA, *pQTM_DATA;

// object declaration
typedef struct tag_hi_Transformer {
  const  iTransformerVtbl* vtbl; // pointer to the transformer virtual table
  const  iSYSTEMVTBL*      sys;  // pointer to the system virtual table
  struct sQTM_DATA*        data; // pointer to the transformer data structure
} *hi_Transformer;

// ##### *** Quantum Decompression *** ########################################

#define QTM(X)            Data->QState.X
#define q_length_base     Data->QState.c_q_length_base
#define q_length_extra    Data->QState.c_q_length_extra
#define q_extra_bits      Data->QState.c_q_extra_bits
#define q_position_base   Data->QState.c_q_position_base

// ----------------------------------------------------------------------------

// initialise a model which decodes symbols from [s] to [s]+[n]-1
static tVOID QATTRIB QTM_InitModel(tQTM_MODEL* m, tQTM_MODELSYM* sym,
                                   tINT n, tINT s) 
{
  tINT i;

  m->shiftsleft = 4;
  m->entries    = n;
  m->syms       = sym;
  memset(m->tabloc, 0xFF, sizeof(m->tabloc)); // clear out look-up table

  for ( i = 0; i < n; i++ ) 
  {
    m->tabloc[i+s]     = i;   // set up a look-up entry for symbol
    m->syms[i].sym     = i+s; // actual symbol
    m->syms[i].cumfreq = n-i; // current frequency of that symbol
  }

  m->syms[n].cumfreq = 0;
}

// ----------------------------------------------------------------------------

static QINLINE tERROR QATTRIB QTM_Init(hi_Transformer _this) 
{
  tQTM_DATA* Data;
  tERROR Error;
  tINT   wndsize;
  tINT   window;
  tINT   msz, i;
  tUINT  j;

  Data    = _this->data;
  window  = Data->dwWndSize;
  wndsize = (1 << window);
  msz     = (window * 2);

  // QTM supports window sizes of 2^10 (1kb) through 2^21 (2mb)
  if ( window < 10 || window > 21 )
    return(errPARAMETER_INVALID);

  QTM(window_size) = wndsize;
  QTM(window_posn) = 0;

  // alloc window
  Data->QState.window = (NULL);
  Error = CALL_SYS_ObjHeapAlloc(_this,(tPTR*)(&Data->QState.window),wndsize);
  if ( PR_FAIL(Error) || NULL == Data->QState.window )
    return(errNOT_ENOUGH_MEMORY);

  // initialise static slot/extrabits tables
  for (i = 0, j = 0; i < 27; i++) 
  {
    q_length_extra[i] = (i == 26) ? 0 : (i < 2 ? 0 : i - 2) >> 2;
    q_length_base[i] = (tBYTE)j; j += 1 << ((i == 26) ? 5 : q_length_extra[i]);
  }
  for (i = 0, j = 0; i < 42; i++) 
  {
    q_extra_bits[i] = (i < 2 ? 0 : i-2) >> 1;
    q_position_base[i] = j; j += 1 << q_extra_bits[i];
  }

  // initialise arithmetic coding models
  QTM_InitModel(&QTM(model7),  &QTM(m7sym)[0], 7, 0);
  QTM_InitModel(&QTM(model00), &QTM(m00sym)[0], 0x40, 0x00);
  QTM_InitModel(&QTM(model40), &QTM(m40sym)[0], 0x40, 0x40);
  QTM_InitModel(&QTM(model80), &QTM(m80sym)[0], 0x40, 0x80);
  QTM_InitModel(&QTM(modelC0), &QTM(mC0sym)[0], 0x40, 0xC0);

  // model 4 depends on table size, ranges from 20 to 24 
  QTM_InitModel(&QTM(model4), &QTM(m4sym)[0], (msz < 24) ? msz : 24, 0);
  // model 5 depends on table size, ranges from 20 to 36 
  QTM_InitModel(&QTM(model5), &QTM(m5sym)[0], (msz < 36) ? msz : 36, 0);
  // model 6pos depends on table size, ranges from 20 to 42
  QTM_InitModel(&QTM(model6pos), &QTM(m6psym)[0], msz, 0);
  QTM_InitModel(&QTM(model6len), &QTM(m6lsym)[0], 27, 0);

  return(errOK);
}

// ----------------------------------------------------------------------------

static tVOID QATTRIB QTM_UpdateModel(struct sQTM_MODEL* model, tINT sym) 
{
  struct sQTM_MODELSYM temp;
  tINT i, j;

  for ( i = 0; i < sym; i++ )
  {
    model->syms[i].cumfreq += 8;
  }

  if ( model->syms[0].cumfreq > 3800 )
  {
    if ( --model->shiftsleft )
    {
      for ( i = model->entries - 1; i >= 0; i-- )
      {
        // -1, not -2; the 0 entry saves this
        model->syms[i].cumfreq >>= 1;
        if (model->syms[i].cumfreq <= model->syms[i+1].cumfreq)
        {
          model->syms[i].cumfreq = model->syms[i+1].cumfreq + 1;
        }
      }
    }
    else
    {
      model->shiftsleft = 50;
      for ( i = 0; i < model->entries ; i++ )
      {
        // no -1, want to include the 0 entry
        // this converts cumfreqs into frequencies, then shifts right
        model->syms[i].cumfreq -= model->syms[i+1].cumfreq;
        model->syms[i].cumfreq++; // avoid losing things entirely
        model->syms[i].cumfreq >>= 1;
      }

      // now sort by frequencies, decreasing order -- this must be an
      // inplace selection sort, or a sort with the same (in)stability
      // characteristics
      
      for ( i = 0; i < model->entries - 1; i++ )
      {
        for ( j = i + 1; j < model->entries; j++ )
        {
          if ( model->syms[i].cumfreq < model->syms[j].cumfreq )
          {
            temp = model->syms[i];
            model->syms[i] = model->syms[j];
            model->syms[j] = temp;
          }
        }
      }
    
      // then convert frequencies back to cumfreq
      for ( i = model->entries - 1; i >= 0; i-- )
      {
        model->syms[i].cumfreq += model->syms[i+1].cumfreq;
      }
    
      // then update the other part of the table
      for ( i = 0; i < model->entries; i++ )
      {
        model->tabloc[model->syms[i].sym] = i;
      }
    }
  }
}

// ----------------------------------------------------------------------------

// Bitstream reading macros (Quantum / normal byte order)
// 
// Q_INIT_BITSTREAM    should be used first to set up the system
// Q_READ_BITS(var,n)  takes N bits from the buffer and puts them in var.
//                     unlike LZX, this can loop several times to get the
//                     requisite number of bits.
// Q_FILL_BUFFER       adds more data to the bit buffer, if there is room
//                     for another 16 bits.
// Q_PEEK_BITS(n)      extracts (without removing) N bits from the bit
//                     buffer
// Q_REMOVE_BITS(n)    removes N bits from the bit buffer
// 
// These bit access routines work by using the area beyond the MSB and the
// LSB as a free source of zeroes. This avoids having to mask any bits.
// So we have to know the bit width of the bitbuffer variable. This is
// defined as ULONG_BITS.
// 
// ULONG_BITS should be at least 16 bits. Unlike LZX's Huffman decoding,
// Quantum's arithmetic decoding only needs 1 bit at a time, it doesn't
// need an assured number. Retrieving larger bitstrings can be done with
// multiple reads and fills of the bitbuffer. The code should work fine
// for machines where ULONG >= 32 bits.
// 
// Also note that Quantum reads bytes in normal order; LZX is in
// little-endian order.
// 

#ifndef CHAR_BIT
# define CHAR_BIT (8)
#endif
#ifndef ULONG_BITS
# define ULONG_BITS (sizeof(tUINT) * CHAR_BIT)
#endif

#define Q_INIT_BITSTREAM do { bitsleft = 0; bitbuf = 0; } while (0)

#define Q_FILL_BUFFER do {                                              \
  if (bitsleft <= (ULONG_BITS - 16)) {                                  \
    unsigned char* inpos = &QTM(inp_buff)[Data->QState.cur_inp];        \
    bitbuf |= ((inpos[0]<<8)|inpos[1]) << (ULONG_BITS-16 - bitsleft);   \
    bitsleft += 16;                                                     \
    QTM(cur_inp) += 2;                                                  \
    if ( QTM(cur_inp) >= CAB_INPUTMAX ) return(errOBJECT_INVALID);      \
  }                                                                     \
} while (0)

#define Q_PEEK_BITS(n)   (bitbuf >> (ULONG_BITS - (n)))
#define Q_REMOVE_BITS(n) ((bitbuf <<= (n)), (bitsleft -= (n)))

#define Q_READ_BITS(v,n) do {                                           \
  (v) = 0;                                                              \
  for (bitsneed = (n); bitsneed; bitsneed -= bitrun) {                  \
    Q_FILL_BUFFER;                                                      \
    bitrun = (bitsneed > bitsleft) ? bitsleft : bitsneed;               \
    (v) = ((v) << bitrun) | Q_PEEK_BITS(bitrun);                        \
    Q_REMOVE_BITS(bitrun);                                              \
  }                                                                     \
} while (0)

#define Q_MENTRIES(model) (QTM(model).entries)
#define Q_MSYM(model,symidx) (QTM(model).syms[(symidx)].sym)
#define Q_MSYMFREQ(model,symidx) (QTM(model).syms[(symidx)].cumfreq)

// GET_SYMBOL(model, var) fetches the next symbol from the stated model
// and puts it in var. it may need to read the bitstream to do this.
#define GET_SYMBOL(m, var) do {                                         \
  range =  ((H - L) & 0xFFFF) + 1;                                      \
  symf = ((((C - L + 1) * Q_MSYMFREQ(m,0)) - 1) / range) & 0xFFFF;      \
                                                                        \
  for (i=1; i < Q_MENTRIES(m); i++) {                                   \
    if (Q_MSYMFREQ(m,i) <= symf) break;                                 \
  }                                                                     \
  (var) = (tBYTE)Q_MSYM(m,i-1);                                         \
                                                                        \
  range = (H - L) + 1;                                                  \
  H = L + ((Q_MSYMFREQ(m,i-1) * range) / Q_MSYMFREQ(m,0)) - 1;          \
  L = L + ((Q_MSYMFREQ(m,i)   * range) / Q_MSYMFREQ(m,0));              \
  while (1) {                                                           \
    if ((L & 0x8000) != (H & 0x8000)) {                                 \
      if ((L & 0x4000) && !(H & 0x4000)) {                              \
        C ^= 0x4000; L &= 0x3FFF; H |= 0x4000;                          \
      }                                                                 \
      else break;                                                       \
    }                                                                   \
    L <<= 1; H = (H << 1) | 1;                                          \
    Q_FILL_BUFFER;                                                      \
    C  = (C << 1) | Q_PEEK_BITS(1);                                     \
    Q_REMOVE_BITS(1);                                                   \
  }                                                                     \
                                                                        \
  QTM_UpdateModel(&(QTM(m)), i);                                        \
} while (0)

// ----------------------------------------------------------------------------

static QINLINE tERROR QATTRIB QTM_Decompress(struct sQTM_DATA* Data) 
{
  struct sQTM_STATE* State;
  register tINT  bitsleft, bitrun, bitsneed;
  register tUINT bitbuf;
  tDWORD Written;
  tERROR Error;
  tINT   extra, togo, match_length, copy_length;
  tBYTE* window;
  tBYTE* runsrc, *rundest;
  tBYTE  selector, sym;
  tUINT  match_offset;
  tUINT  window_posn;
  tUINT  window_size;
  tUINT  range;
  tWORD  symf;
  tINT   i;
  tWORD  H, L, C;

  State  = &Data->QState;
  window = QTM(window);
  window_posn = QTM(window_posn);
  window_size = QTM(window_size);
  togo   = (tINT)(Data->qwOutputSize);
  H = 0xFFFF;
  L = 0;

  // read initial value of C
  Q_INIT_BITSTREAM;
  Q_READ_BITS(C, 16);

  // apply 2^x-1 mask
  window_posn &= (window_size - 1);
  // runs can't straddle the window wraparound
  if ((window_posn + togo) > window_size) 
  {
    // straddled run
    return(errOBJECT_INVALID);
  }

  while (togo > 0) 
  {
    GET_SYMBOL(model7, selector);
    switch (selector) 
    {
    case 0:
      GET_SYMBOL(model00, sym); window[window_posn++] = sym; togo--;
      break;
    case 1:
      GET_SYMBOL(model40, sym); window[window_posn++] = sym; togo--;
      break;
    case 2:
      GET_SYMBOL(model80, sym); window[window_posn++] = sym; togo--;
      break;
    case 3:
      GET_SYMBOL(modelC0, sym); window[window_posn++] = sym; togo--;
      break;
    case 4:
      // selector 4 = fixed length of 3
      GET_SYMBOL(model4, sym);
      Q_READ_BITS(extra, q_extra_bits[sym]);
      match_offset = q_position_base[sym] + extra + 1;
      match_length = 3;
      break;
    case 5:
      // selector 5 = fixed length of 4
      GET_SYMBOL(model5, sym);
      Q_READ_BITS(extra, q_extra_bits[sym]);
      match_offset = q_position_base[sym] + extra + 1;
      match_length = 4;
      break;
    case 6:
      // selector 6 = variable length
      GET_SYMBOL(model6len, sym);
      Q_READ_BITS(extra, q_length_extra[sym]);
      match_length = q_length_base[sym] + extra + 5;
      GET_SYMBOL(model6pos, sym);
      Q_READ_BITS(extra, q_extra_bits[sym]);
      match_offset = q_position_base[sym] + extra + 1;
      break;
    default:
      // selector is bogus
      return(errOBJECT_INVALID);
    }

    // if this is a match
    if (selector >= 4) 
    {
      rundest = window + window_posn;
      togo -= match_length;

      // copy any wrapped around source data
      if (window_posn >= match_offset) 
      {
        // no wrap
        runsrc = rundest - match_offset;
      }
      else 
      {
        runsrc = rundest + (window_size - match_offset);
        copy_length = match_offset - window_posn;
        if (copy_length < match_length) 
        {
          match_length -= copy_length;
          window_posn += copy_length;
          while (copy_length-- > 0)
            *rundest++ = *runsrc++;
          runsrc = window;
        }
      }
      window_posn += match_length;

      // copy match data - no worries about destination wraps
      while (match_length-- > 0)
        *rundest++ = *runsrc++;
    }
  } // while (togo > 0)

  if ( togo != 0 )
  {
    // frame overflow
    return(errOBJECT_INVALID);
  }

  // write output
  Written = 0;
  togo = (tINT)(Data->qwOutputSize);
  window += ((!window_posn) ? window_size : window_posn) - togo;
  Error = CALL_SeqIO_Write(Data->hOutputSeqIo, &Written, window, togo);
  if ( PR_FAIL(Error) || (tINT)Written != togo )
    return(errNOT_OK);

  Data->qwOutputSize = Written;
  QTM(window_posn)   = window_posn;
  return(errOK);
}

// ##### *** Transformer Methods *** ##########################################

// initialize object kernel called method
static tERROR pr_call Transformer_ObjectInitDone(hi_Transformer _this)
{
  tERROR Error;
  PR_TRACE_A0(_this, "Enter Transformer::ObjectInitDone method");

  // init quantum
  memset(&_this->data->QState, 0, sizeof(struct sQTM_STATE));
  Error = QTM_Init(_this);

  PR_TRACE_A1(_this, "Leave Transformer::ObjectInitDone method, ret %terr", Error);
  return(Error);
}
// AVP Prague stamp end

// ----------------------------------------------------------------------------

// finalize object kernel called method
static tERROR pr_call Transformer_ObjectPreClose(hi_Transformer _this)
{
  tQTM_DATA* Data;
  tERROR     Error;
  PR_TRACE_A0(_this, "Enter Transformer::ObjectPreClose method");

  Error = (errOK);
  Data  = _this->data;
  if ( NULL != QTM(window) )
  {
    CALL_SYS_ObjHeapFree(_this, QTM(window));
    QTM(window) = (NULL);
  }

  PR_TRACE_A1(_this, "Leave Transformer::ObjectPreClose method, ret %terr", Error);
  return(Error);
}
// AVP Prague stamp end

// ----------------------------------------------------------------------------

tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* result, tDWORD p_dwSze ) {
  tERROR error;
  tDWORD ret_val;
  PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

  // Place your code here
  error = (errNOT_IMPLEMENTED);
  ret_val = 0;

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave Transformer::ProcessChunck method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}

// ----------------------------------------------------------------------------

// external user called decompression method
tERROR pr_call Transformer_Proccess(hi_Transformer _this, tQWORD* Result)
{
  struct sQTM_DATA* Data;
  tERROR Error;
  tQWORD RetVal;
  tDWORD Readed;
  PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

  Data = _this->data;
  if ( Data->dwInputSize  == 0 || Data->dwInputSize  > CAB_INPUTMAX )
    Data->dwInputSize  = CAB_INPUTMAX;
  if ( Data->qwOutputSize == 0 || Data->qwOutputSize > CAB_BLOCKMAX )
    Data->qwOutputSize = CAB_BLOCKMAX;

  Readed = 0;  // read input data
  memset(QTM(inp_buff), 0, CAB_INPUTMAX);
  Error  = CALL_SeqIO_Read(Data->hInputSeqIo, &Readed, QTM(inp_buff),
    Data->dwInputSize);
  if ( PR_FAIL(Error) || Readed == 0 )
    return(errOBJECT_INVALID);

  RetVal = 0;
  Data->QState.cur_inp = 0;
  Data->QState.max_inp = Readed;
  if ( PR_FAIL(Error = QTM_Decompress(Data)) )
    goto loc_exit;

  RetVal = Data->qwOutputSize;
  
loc_exit:
  if ( NULL != Result )
    *Result = RetVal;
  PR_TRACE_A2(_this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, %terr", RetVal, Error);
  return(Error);
}

// ##### *** Transformer Properties *** #######################################

#define QTM_VERSION  ((tVERSION)(1))
static const tCHAR QTM_Comment[] = "Quantum Decompressor";

// property table
mPROPERTY_TABLE(Transformer_PropTable)
  mLOCAL_PROPERTY_BUF  ( plWINDOW_SIZE,       tQTM_DATA, dwWndSize,    cPROP_BUFFER_WRITE      )
  mLOCAL_PROPERTY_BUF  ( plRUN_INPUT_SIZE,    tQTM_DATA, dwInputSize,  cPROP_BUFFER_WRITE      )
  mLOCAL_PROPERTY_BUF  ( pgTRANSFORM_SIZE,    tQTM_DATA, qwOutputSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF  ( pgOUTPUT_SEQ_IO,     tQTM_DATA, hOutputSeqIo, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF  ( pgINPUT_SEQ_IO,      tQTM_DATA, hInputSeqIo,  cPROP_BUFFER_READ_WRITE )
  mSHARED_PROPERTY     ( pgINTERFACE_VERSION, QTM_VERSION                                      )
  mSHARED_PROPERTY_PTR ( pgINTERFACE_COMMENT, QTM_Comment, sizeof(QTM_Comment)                 )
mPROPERTY_TABLE_END(Transformer_PropTable)

// ----------------------------------------------------------------------------

// internal method table
static iINTERNAL i_Transformer_vtbl = {
  (tIntFnRecognize)         NULL,
  (tIntFnObjectNew)         NULL,
  (tIntFnObjectInit)        NULL,
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

// external method table
static iTransformerVtbl e_Transformer_vtbl = {
  (fnpTransformer_ProcessChunck) Transformer_ProcessChunck,
  (fnpTransformer_Proccess)      Transformer_Proccess
};

// ----------------------------------------------------------------------------

// interface "transformer". register function
static tERROR pr_call Transformer_Register( hROOT root ) 
{
  tERROR error;

  PR_TRACE_A0( root, "Enter Transformer::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_TRANSFORMER,                        // interface identifier
    PID_QUANTUMDECOMPRESSOR,                // plugin identifier
    0x00000000,                             // subtype identifier
    QTM_VERSION,                            // interface version
    VID_GLAV,                               // interface developer
    &i_Transformer_vtbl,                    // internal(kernel called) function table
    (sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
    &e_Transformer_vtbl,                    // external function table
    (sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
    Transformer_PropTable,                  // property table
    mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
    sizeof(tQTM_DATA),                      // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"Transformer(IID_TRANSFORMER) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave Transformer::Register method, ret %terr", error );
  return error;
}

// ----------------------------------------------------------------------------

hROOT  g_root = 0;

// dll entrypoint
tBOOL __stdcall DllMain(tPTR hInstance, tDWORD dwReason, tERROR* pERROR)
{
  switch( dwReason ) 
  {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;
    case PRAGUE_PLUGIN_LOAD :
      g_root = (hROOT)hInstance;
      *pERROR = errOK;

      // register my interfaces
      if ( PR_FAIL(*pERROR=Transformer_Register(g_root)) )
      {
        PR_TRACE(( g_root, prtERROR, "cannot register \"Transformer\" interface"));
        return cFALSE;
      }
      break;
    case PRAGUE_PLUGIN_UNLOAD :
      break;
  }

  return(cTRUE);
}

// ----------------------------------------------------------------------------

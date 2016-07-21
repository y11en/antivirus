// Version History:
// -----------------
// previous history omited
// 22.03.2005 - upgraded as in zlib 1.2.2 (minor bugfixes)
// ----------------------------------------------------------------------------

// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  16 March 2005,  18:17 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин распаковки данных, упакованных методом Deflate
// Author      -- Dmitry Glavatskikh
// File Name   -- inflate_t.c
// Additional info
//    В этом плагине реализована поддержка как стандартного потока Deflate, так и специальной модификации для программы упаковки исполняемых файлов Petite
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Transformer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>

#include <Extract/plugin/p_inflate.h>

#include <memory.h>
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

#define Z_NULL          0

#define DEF_WBITS  (15)
#define MAX_WBITS  (16)

#define Z_DEFLATED   8

#define zmemcpy  memcpy

#if defined(_M_X64)
	#define SHIFT_UNIT (1ull)
#else
	#define SHIFT_UNIT (1U)
#endif

// possible inflate modes between inflate() calls
typedef enum {
    HEAD,       // i: waiting for magic header
    FLAGS,      // i: waiting for method and flags (gzip)
    TIME,       // i: waiting for modification time (gzip)
    OS,         // i: waiting for extra flags and operating system (gzip)
    EXLEN,      // i: waiting for extra length (gzip)
    EXTRA,      // i: waiting for extra bytes (gzip)
    NAME,       // i: waiting for end of file name (gzip)
    COMMENT,    // i: waiting for end of comment (gzip)
    HCRC,       // i: waiting for header crc (gzip)
    DICTID,     // i: waiting for dictionary check value
    DICT,       // waiting for inflateSetDictionary() call
    TYPE,       // i: waiting for type bits, including last-flag bit
    TYPEDO,     // i: same, but skip check to exit inflate on new block
    STORED,     // i: waiting for stored size (length and complement)
    COPY,       // i/o: waiting for input or output to copy stored block
    TABLE,      // i: waiting for dynamic block table lengths
    LENLENS,    // i: waiting for code length code lengths
    CODELENS,   // i: waiting for length/lit and distance code lengths
    LEN,        // i: waiting for length/lit code
    LENEXT,     // i: waiting for length extra bits
    DIST,       // i: waiting for distance code
    DISTEXT,    // i: waiting for distance extra bits
    MATCH,      // o: waiting for output space to copy string
    LIT,        // o: waiting for output space to write literal
    CHECK,      // i: waiting for 32-bit check value
    LENGTH,     // i: waiting for 32-bit length (gzip)
    DONE,       // finished check, done -- remain here until reset
    BAD,        // got a data error -- remain here until reset
    MEM,        // got an inflate() memory error -- remain here until reset
    SYNC        // looking for synchronization bytes to restart inflate()
} inflate_mode;

#define ENOUGH 1440
#define MAXD   154

typedef struct {
    unsigned char  op;          // operation, extra bits, table bits
    unsigned char  bits;        // bits in this part of the code
    unsigned short val;         // offset in table or code value
} code;

typedef enum {
    CODES,
    LENS,
    DISTS
} codetype;

typedef tDWORD (* f_adler)(tDWORD, tBYTE*, tINT);
struct  tag_tINFLATE_DATA;

// state maintained between inflate() calls
struct inflate_state {
    inflate_mode mode;          // current inflate mode
    int last;                   // true if processing last block
    int wrap;                   // bit 0 true for zlib, bit 1 true for gzip
    int havedict;               // true if dictionary provided
    f_adler  adler;             // adler checksum for zlib
    unsigned long check;        // protected copy of check value

    // sliding window
    unsigned wbits;             // log base 2 of requested window size
    unsigned wsize;             // window size or zero if not using window
    unsigned whave;             // valid bytes in the window
    unsigned write;             // window write index
    unsigned char* window;      // allocated sliding window, if needed

    // bit accumulator
    unsigned long hold;         // input bit accumulator
    unsigned bits;              // number of bits in "in"

    // for string and stored block copying
    unsigned length;            // literal or length of data to copy
    unsigned offset;            // distance back to copy string from

    // for table and code decoding
    unsigned extra;             // extra bits needed

    // fixed and dynamic code tables
    code const* lencode;        // starting table for length/literal codes
    code const* distcode;       // starting table for distance codes
    unsigned lenbits;           // index bits for lencode
    unsigned distbits;          // index bits for distcode

    // dynamic table building
    unsigned ncode;             // number of code length code lengths
    unsigned nlen;              // number of length code lengths
    unsigned ndist;             // number of distance code lengths
    unsigned have;              // number of code lengths in lens[]
    code*    next;              // next available space in codes[]
    unsigned short lens[320];   // temporary storage for code lengths
    unsigned short work[288];   // work area for code table building
    code codes[ENOUGH];         // space for code tables
};

// stream context
typedef struct z_stream_s {
  tBYTE*  next_in;                 // next input byte
  tBYTE*  next_out;                // next output byte should be put there
  tUINT   avail_in;                // number of bytes available at next_in
  tUINT   avail_out;               // remaining free space at next_out
  tQWORD  total_in;                // total nb of input bytes read so far
  tQWORD  total_out;               // total nb of bytes output so far
  tDWORD  flags;                   // inflate flags
  struct  tag_tINFLATE_DATA* self; // parent object data
  struct  inflate_state state;     // decoder state
} z_stream, *z_streamp;

// allowed flush values
#define Z_NO_FLUSH           0
#define Z_PARTIAL_FLUSH      1
#define Z_SYNC_FLUSH         2
#define Z_FULL_FLUSH         3
#define Z_FINISH             4
#define Z_BLOCK              5

// return codes
#define Z_OK                 0
#define Z_STREAM_END         1
#define Z_NEED_DICT          2
#define Z_ERRNO            (-1)
#define Z_STREAM_ERROR     (-2)
#define Z_DATA_ERROR       (-3)
#define Z_MEM_ERROR        (-4)
#define Z_BUF_ERROR        (-5)
#define Z_VERSION_ERROR    (-6)

// special flags
#define Z_FLG_DEFAULT        0         // default zlib stream
#define Z_FLG_NOWRAP         1         // no stream header
#define Z_FLG_STORE16        2         // hacked stored block (nsis)
#define Z_FLG_SKIPCHECK      4         // skip adler block
#define Z_FLG_DUMMYCHECK     8         // ignore invalid checksum
#define Z_FLG_DEFLATE64     16         // deflate 64 stream
#define Z_FLG_PETITE        32         // petite stream

/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure

typedef struct tag_tINFLATE_DATA 
{
  hSEQ_IO               hInputIO;   // --
  hSEQ_IO               hOutputIO;  // --
  tBYTE*                Window;     // Decompression window
  tDWORD                WindowSize; // Decompression window size
  tQWORD                ObjPSize;   // Maximum input data size
  tQWORD                ObjUSize;   // Maximum output data size
  tBYTE*                RBuf;       // Input buffer
  tDWORD                RBufSize;   // Input buffer size
  tBYTE*                WBuf;       // Output buffer
  tDWORD                WBufSize;   // Output buffer size
  tPTR                  CBUser;     // IO callbacks user data ptr
  fTRANSFORMER_CALLBACK InputCB;    // Input callback
  fTRANSFORMER_CALLBACK OutputCB;   // Output callback
  tINT                  WndMul;     // Window multipier
  tBOOL                 bPetite;    // Petite stream flag
  tBOOL                 bInf64;     // Inflate64 stream flag
  tBOOL                 bNsis;      // NSIS stream flag
  tBOOL                 bChkZLib;   // Check zlib stream checksum
  hOBJECT               Self;       // Self object
  tBOOL                 bForceWnd;  // Force window creation
  tBYTE*                IntWnd;     // internal window
  tUINT                 IntWndSize; // internal window size
  tBYTE*                IntIOBuf;   // internal io buffer
  tBOOL                 Inited;     // inflate was initialized
  struct z_stream_s     Stream;     // decoder
} tINFLATE_DATA;

// AVP Prague stamp end



/* ------------------------------------------------------------------------- */


// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
  const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
  const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
  tINFLATE_DATA*          data; // pointer to the "Transformer" data structure
} *hi_Transformer;

// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Transformer". Static(shared) property table variables
static const tSTRING InflateTransInterfCmt = "Inflate decompressor";
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

static tINT InflateReset(struct z_stream_s* strm, tBOOL ResetWnd)
{
  struct inflate_state* state;

  if ( strm == Z_NULL ) return(Z_STREAM_ERROR);

  state = (struct inflate_state*)(&strm->state);
  strm->total_in  = 0;
  strm->total_out = 0;
  state->mode = HEAD;
  state->last = 0;
  state->havedict = 0;
  state->hold  = 0;
  state->bits  = 0;
  state->lencode = state->distcode = state->next = state->codes;

  if ( ResetWnd )
  {
    state->wsize = 0;
    state->whave = 0;
    state->window  = NULL;
  }

  return(Z_OK);
}

/* ------------------------------------------------------------------------- */

#define BASE 65521UL    // largest prime smaller than 65536
#define NMAX 5552

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

#define MOD(a) a %= BASE

static tDWORD adler32(tDWORD adler, tBYTE* buf, tINT len)
{
  unsigned long s1 = adler & 0xffff;
  unsigned long s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == Z_NULL)
    return 1L;

  while (len > 0)
  {
    k = len < NMAX ? (int)len : NMAX;
    len -= k;
    while (k >= 16)
    {
      DO16(buf);
      buf += 16;
      k -= 16;
    }
    if (k != 0) do
    {
      s1 += *buf++;
      s2 += s1;
    }
    while (--k);
    
    MOD(s1);
    MOD(s2);
  }

  return (s2 << 16) | s1;
}

/* ------------------------------------------------------------------------- */

static tINT InflateInit(z_stream* strm, tINT flags)
{
  struct inflate_state* state;
  int windowBits;

  if ( strm == Z_NULL ) 
    return Z_STREAM_ERROR;

  state = (struct inflate_state*)(&strm->state);

  state->adler  = Z_NULL;
  state->window = Z_NULL;
  if ( flags & Z_FLG_DEFLATE64 )
    windowBits = MAX_WBITS;
  else
    windowBits = DEF_WBITS;

  if ( flags & Z_FLG_NOWRAP ) 
    state->wrap = 0;
  else
    state->wrap = 1;

  if ( 0 == (flags & (Z_FLG_NOWRAP|Z_FLG_SKIPCHECK|Z_FLG_DUMMYCHECK)) )
    state->adler = adler32;

  state->wbits = (unsigned)windowBits;
  state->window = Z_NULL;
  return InflateReset(strm, cTRUE);
}

/* ------------------------------------------------------------------------- */

static tINT updatewindow(z_stream* strm, tUINT out)
{
  struct inflate_state* state;
  unsigned int copy, dist;

  state = (struct inflate_state*)(&strm->state);

  /* if it hasn't been done already, allocate space for the window */
  if ( state->window == Z_NULL )
  {
    tINFLATE_DATA* Data;
    tERROR Error;
    tUINT  WndSize;

    Data = strm->self;
    WndSize = (1U << state->wbits);
    if ( Data->Window != NULL && Data->WindowSize >= WndSize )
    {
      state->window = Data->Window;
    }
    else if ( Data->IntWnd != NULL && Data->IntWndSize >= WndSize )
    {
      state->window = Data->IntWnd;
    }
    else
    {
      if ( Data->IntWnd != NULL )
      {
        CALL_SYS_ObjHeapFree(Data->Self, Data->IntWnd);
        Data->IntWnd = NULL;
      }

      Error = CALL_SYS_ObjHeapAlloc(Data->Self, &Data->IntWnd, WndSize);
      if ( PR_FAIL(Error) )
        return(1); /* no mem */

      state->window = Data->IntWnd;
      Data->IntWndSize = WndSize;
    }
  }

  /* if window not in use yet, initialize */
  if ( state->wsize == 0 )
  {
    state->wsize = (1U << state->wbits);
    state->write = 0;
    state->whave = 0;
  }

  /* copy state->wsize or less output bytes into the circular window */
  copy = out - strm->avail_out;
  if (copy >= state->wsize)
  {
    zmemcpy(state->window, strm->next_out - state->wsize, state->wsize);
    state->write = 0;
    state->whave = state->wsize;
  }
  else
  {
    dist = state->wsize - state->write;
    if (dist > copy) dist = copy;
    zmemcpy(state->window + state->write, strm->next_out - copy, dist);
    copy -= dist;
    if (copy)
    {
      zmemcpy(state->window, strm->next_out - copy, copy);
      state->write = copy;
      state->whave = state->wsize;
    }
    else
    {
      state->write += dist;
      if (state->write == state->wsize) state->write = 0;
      if (state->whave < state->wsize) state->whave += dist;
    }
  }

  return(0); /* all ok */
}

/* ------------------------------------------------------------------------- */

static const code lenfix32[512] = {
        {96,7,0},{0,8,80},{0,8,16},{20,8,115},{18,7,31},{0,8,112},{0,8,48},
        {0,9,192},{16,7,10},{0,8,96},{0,8,32},{0,9,160},{0,8,0},{0,8,128},
        {0,8,64},{0,9,224},{16,7,6},{0,8,88},{0,8,24},{0,9,144},{19,7,59},
        {0,8,120},{0,8,56},{0,9,208},{17,7,17},{0,8,104},{0,8,40},{0,9,176},
        {0,8,8},{0,8,136},{0,8,72},{0,9,240},{16,7,4},{0,8,84},{0,8,20},
        {21,8,227},{19,7,43},{0,8,116},{0,8,52},{0,9,200},{17,7,13},{0,8,100},
        {0,8,36},{0,9,168},{0,8,4},{0,8,132},{0,8,68},{0,9,232},{16,7,8},
        {0,8,92},{0,8,28},{0,9,152},{20,7,83},{0,8,124},{0,8,60},{0,9,216},
        {18,7,23},{0,8,108},{0,8,44},{0,9,184},{0,8,12},{0,8,140},{0,8,76},
        {0,9,248},{16,7,3},{0,8,82},{0,8,18},{21,8,163},{19,7,35},{0,8,114},
        {0,8,50},{0,9,196},{17,7,11},{0,8,98},{0,8,34},{0,9,164},{0,8,2},
        {0,8,130},{0,8,66},{0,9,228},{16,7,7},{0,8,90},{0,8,26},{0,9,148},
        {20,7,67},{0,8,122},{0,8,58},{0,9,212},{18,7,19},{0,8,106},{0,8,42},
        {0,9,180},{0,8,10},{0,8,138},{0,8,74},{0,9,244},{16,7,5},{0,8,86},
        {0,8,22},{64,8,0},{19,7,51},{0,8,118},{0,8,54},{0,9,204},{17,7,15},
        {0,8,102},{0,8,38},{0,9,172},{0,8,6},{0,8,134},{0,8,70},{0,9,236},
        {16,7,9},{0,8,94},{0,8,30},{0,9,156},{20,7,99},{0,8,126},{0,8,62},
        {0,9,220},{18,7,27},{0,8,110},{0,8,46},{0,9,188},{0,8,14},{0,8,142},
        {0,8,78},{0,9,252},{96,7,0},{0,8,81},{0,8,17},{21,8,131},{18,7,31},
        {0,8,113},{0,8,49},{0,9,194},{16,7,10},{0,8,97},{0,8,33},{0,9,162},
        {0,8,1},{0,8,129},{0,8,65},{0,9,226},{16,7,6},{0,8,89},{0,8,25},
        {0,9,146},{19,7,59},{0,8,121},{0,8,57},{0,9,210},{17,7,17},{0,8,105},
        {0,8,41},{0,9,178},{0,8,9},{0,8,137},{0,8,73},{0,9,242},{16,7,4},
        {0,8,85},{0,8,21},{16,8,258},{19,7,43},{0,8,117},{0,8,53},{0,9,202},
        {17,7,13},{0,8,101},{0,8,37},{0,9,170},{0,8,5},{0,8,133},{0,8,69},
        {0,9,234},{16,7,8},{0,8,93},{0,8,29},{0,9,154},{20,7,83},{0,8,125},
        {0,8,61},{0,9,218},{18,7,23},{0,8,109},{0,8,45},{0,9,186},{0,8,13},
        {0,8,141},{0,8,77},{0,9,250},{16,7,3},{0,8,83},{0,8,19},{21,8,195},
        {19,7,35},{0,8,115},{0,8,51},{0,9,198},{17,7,11},{0,8,99},{0,8,35},
        {0,9,166},{0,8,3},{0,8,131},{0,8,67},{0,9,230},{16,7,7},{0,8,91},
        {0,8,27},{0,9,150},{20,7,67},{0,8,123},{0,8,59},{0,9,214},{18,7,19},
        {0,8,107},{0,8,43},{0,9,182},{0,8,11},{0,8,139},{0,8,75},{0,9,246},
        {16,7,5},{0,8,87},{0,8,23},{64,8,0},{19,7,51},{0,8,119},{0,8,55},
        {0,9,206},{17,7,15},{0,8,103},{0,8,39},{0,9,174},{0,8,7},{0,8,135},
        {0,8,71},{0,9,238},{16,7,9},{0,8,95},{0,8,31},{0,9,158},{20,7,99},
        {0,8,127},{0,8,63},{0,9,222},{18,7,27},{0,8,111},{0,8,47},{0,9,190},
        {0,8,15},{0,8,143},{0,8,79},{0,9,254},{96,7,0},{0,8,80},{0,8,16},
        {20,8,115},{18,7,31},{0,8,112},{0,8,48},{0,9,193},{16,7,10},{0,8,96},
        {0,8,32},{0,9,161},{0,8,0},{0,8,128},{0,8,64},{0,9,225},{16,7,6},
        {0,8,88},{0,8,24},{0,9,145},{19,7,59},{0,8,120},{0,8,56},{0,9,209},
        {17,7,17},{0,8,104},{0,8,40},{0,9,177},{0,8,8},{0,8,136},{0,8,72},
        {0,9,241},{16,7,4},{0,8,84},{0,8,20},{21,8,227},{19,7,43},{0,8,116},
        {0,8,52},{0,9,201},{17,7,13},{0,8,100},{0,8,36},{0,9,169},{0,8,4},
        {0,8,132},{0,8,68},{0,9,233},{16,7,8},{0,8,92},{0,8,28},{0,9,153},
        {20,7,83},{0,8,124},{0,8,60},{0,9,217},{18,7,23},{0,8,108},{0,8,44},
        {0,9,185},{0,8,12},{0,8,140},{0,8,76},{0,9,249},{16,7,3},{0,8,82},
        {0,8,18},{21,8,163},{19,7,35},{0,8,114},{0,8,50},{0,9,197},{17,7,11},
        {0,8,98},{0,8,34},{0,9,165},{0,8,2},{0,8,130},{0,8,66},{0,9,229},
        {16,7,7},{0,8,90},{0,8,26},{0,9,149},{20,7,67},{0,8,122},{0,8,58},
        {0,9,213},{18,7,19},{0,8,106},{0,8,42},{0,9,181},{0,8,10},{0,8,138},
        {0,8,74},{0,9,245},{16,7,5},{0,8,86},{0,8,22},{64,8,0},{19,7,51},
        {0,8,118},{0,8,54},{0,9,205},{17,7,15},{0,8,102},{0,8,38},{0,9,173},
        {0,8,6},{0,8,134},{0,8,70},{0,9,237},{16,7,9},{0,8,94},{0,8,30},
        {0,9,157},{20,7,99},{0,8,126},{0,8,62},{0,9,221},{18,7,27},{0,8,110},
        {0,8,46},{0,9,189},{0,8,14},{0,8,142},{0,8,78},{0,9,253},{96,7,0},
        {0,8,81},{0,8,17},{21,8,131},{18,7,31},{0,8,113},{0,8,49},{0,9,195},
        {16,7,10},{0,8,97},{0,8,33},{0,9,163},{0,8,1},{0,8,129},{0,8,65},
        {0,9,227},{16,7,6},{0,8,89},{0,8,25},{0,9,147},{19,7,59},{0,8,121},
        {0,8,57},{0,9,211},{17,7,17},{0,8,105},{0,8,41},{0,9,179},{0,8,9},
        {0,8,137},{0,8,73},{0,9,243},{16,7,4},{0,8,85},{0,8,21},{16,8,258},
        {19,7,43},{0,8,117},{0,8,53},{0,9,203},{17,7,13},{0,8,101},{0,8,37},
        {0,9,171},{0,8,5},{0,8,133},{0,8,69},{0,9,235},{16,7,8},{0,8,93},
        {0,8,29},{0,9,155},{20,7,83},{0,8,125},{0,8,61},{0,9,219},{18,7,23},
        {0,8,109},{0,8,45},{0,9,187},{0,8,13},{0,8,141},{0,8,77},{0,9,251},
        {16,7,3},{0,8,83},{0,8,19},{21,8,195},{19,7,35},{0,8,115},{0,8,51},
        {0,9,199},{17,7,11},{0,8,99},{0,8,35},{0,9,167},{0,8,3},{0,8,131},
        {0,8,67},{0,9,231},{16,7,7},{0,8,91},{0,8,27},{0,9,151},{20,7,67},
        {0,8,123},{0,8,59},{0,9,215},{18,7,19},{0,8,107},{0,8,43},{0,9,183},
        {0,8,11},{0,8,139},{0,8,75},{0,9,247},{16,7,5},{0,8,87},{0,8,23},
        {64,8,0},{19,7,51},{0,8,119},{0,8,55},{0,9,207},{17,7,15},{0,8,103},
        {0,8,39},{0,9,175},{0,8,7},{0,8,135},{0,8,71},{0,9,239},{16,7,9},
        {0,8,95},{0,8,31},{0,9,159},{20,7,99},{0,8,127},{0,8,63},{0,9,223},
        {18,7,27},{0,8,111},{0,8,47},{0,9,191},{0,8,15},{0,8,143},{0,8,79},
        {0,9,255}
};

static const code distfix32[32] = {
        {16,5,1},{23,5,257},{19,5,17},{27,5,4097},{17,5,5},{25,5,1025},
        {21,5,65},{29,5,16385},{16,5,3},{24,5,513},{20,5,33},{28,5,8193},
        {18,5,9},{26,5,2049},{22,5,129},{64,5,0},{16,5,2},{23,5,385},
        {19,5,25},{27,5,6145},{17,5,7},{25,5,1537},{21,5,97},{29,5,24577},
        {16,5,4},{24,5,769},{20,5,49},{28,5,12289},{18,5,13},{26,5,3073},
        {22,5,193},{64,5,0}
};

static const code lenfix64[512] = {
        {96,7,0},{0,8,80},{0,8,16},{132,8,115},{130,7,31},{0,8,112},
        {0,8,48},{0,9,192},{128,7,10},{0,8,96},{0,8,32},{0,9,160},
        {0,8,0},{0,8,128},{0,8,64},{0,9,224},{128,7,6},{0,8,88},
        {0,8,24},{0,9,144},{131,7,59},{0,8,120},{0,8,56},{0,9,208},
        {129,7,17},{0,8,104},{0,8,40},{0,9,176},{0,8,8},{0,8,136},
        {0,8,72},{0,9,240},{128,7,4},{0,8,84},{0,8,20},{133,8,227},
        {131,7,43},{0,8,116},{0,8,52},{0,9,200},{129,7,13},{0,8,100},
        {0,8,36},{0,9,168},{0,8,4},{0,8,132},{0,8,68},{0,9,232},
        {128,7,8},{0,8,92},{0,8,28},{0,9,152},{132,7,83},{0,8,124},
        {0,8,60},{0,9,216},{130,7,23},{0,8,108},{0,8,44},{0,9,184},
        {0,8,12},{0,8,140},{0,8,76},{0,9,248},{128,7,3},{0,8,82},
        {0,8,18},{133,8,163},{131,7,35},{0,8,114},{0,8,50},{0,9,196},
        {129,7,11},{0,8,98},{0,8,34},{0,9,164},{0,8,2},{0,8,130},
        {0,8,66},{0,9,228},{128,7,7},{0,8,90},{0,8,26},{0,9,148},
        {132,7,67},{0,8,122},{0,8,58},{0,9,212},{130,7,19},{0,8,106},
        {0,8,42},{0,9,180},{0,8,10},{0,8,138},{0,8,74},{0,9,244},
        {128,7,5},{0,8,86},{0,8,22},{65,8,0},{131,7,51},{0,8,118},
        {0,8,54},{0,9,204},{129,7,15},{0,8,102},{0,8,38},{0,9,172},
        {0,8,6},{0,8,134},{0,8,70},{0,9,236},{128,7,9},{0,8,94},
        {0,8,30},{0,9,156},{132,7,99},{0,8,126},{0,8,62},{0,9,220},
        {130,7,27},{0,8,110},{0,8,46},{0,9,188},{0,8,14},{0,8,142},
        {0,8,78},{0,9,252},{96,7,0},{0,8,81},{0,8,17},{133,8,131},
        {130,7,31},{0,8,113},{0,8,49},{0,9,194},{128,7,10},{0,8,97},
        {0,8,33},{0,9,162},{0,8,1},{0,8,129},{0,8,65},{0,9,226},
        {128,7,6},{0,8,89},{0,8,25},{0,9,146},{131,7,59},{0,8,121},
        {0,8,57},{0,9,210},{129,7,17},{0,8,105},{0,8,41},{0,9,178},
        {0,8,9},{0,8,137},{0,8,73},{0,9,242},{128,7,4},{0,8,85},
        {0,8,21},{144,8,3},{131,7,43},{0,8,117},{0,8,53},{0,9,202},
        {129,7,13},{0,8,101},{0,8,37},{0,9,170},{0,8,5},{0,8,133},
        {0,8,69},{0,9,234},{128,7,8},{0,8,93},{0,8,29},{0,9,154},
        {132,7,83},{0,8,125},{0,8,61},{0,9,218},{130,7,23},{0,8,109},
        {0,8,45},{0,9,186},{0,8,13},{0,8,141},{0,8,77},{0,9,250},
        {128,7,3},{0,8,83},{0,8,19},{133,8,195},{131,7,35},{0,8,115},
        {0,8,51},{0,9,198},{129,7,11},{0,8,99},{0,8,35},{0,9,166},
        {0,8,3},{0,8,131},{0,8,67},{0,9,230},{128,7,7},{0,8,91},
        {0,8,27},{0,9,150},{132,7,67},{0,8,123},{0,8,59},{0,9,214},
        {130,7,19},{0,8,107},{0,8,43},{0,9,182},{0,8,11},{0,8,139},
        {0,8,75},{0,9,246},{128,7,5},{0,8,87},{0,8,23},{77,8,0},
        {131,7,51},{0,8,119},{0,8,55},{0,9,206},{129,7,15},{0,8,103},
        {0,8,39},{0,9,174},{0,8,7},{0,8,135},{0,8,71},{0,9,238},
        {128,7,9},{0,8,95},{0,8,31},{0,9,158},{132,7,99},{0,8,127},
        {0,8,63},{0,9,222},{130,7,27},{0,8,111},{0,8,47},{0,9,190},
        {0,8,15},{0,8,143},{0,8,79},{0,9,254},{96,7,0},{0,8,80},
        {0,8,16},{132,8,115},{130,7,31},{0,8,112},{0,8,48},{0,9,193},
        {128,7,10},{0,8,96},{0,8,32},{0,9,161},{0,8,0},{0,8,128},
        {0,8,64},{0,9,225},{128,7,6},{0,8,88},{0,8,24},{0,9,145},
        {131,7,59},{0,8,120},{0,8,56},{0,9,209},{129,7,17},{0,8,104},
        {0,8,40},{0,9,177},{0,8,8},{0,8,136},{0,8,72},{0,9,241},
        {128,7,4},{0,8,84},{0,8,20},{133,8,227},{131,7,43},{0,8,116},
        {0,8,52},{0,9,201},{129,7,13},{0,8,100},{0,8,36},{0,9,169},
        {0,8,4},{0,8,132},{0,8,68},{0,9,233},{128,7,8},{0,8,92},
        {0,8,28},{0,9,153},{132,7,83},{0,8,124},{0,8,60},{0,9,217},
        {130,7,23},{0,8,108},{0,8,44},{0,9,185},{0,8,12},{0,8,140},
        {0,8,76},{0,9,249},{128,7,3},{0,8,82},{0,8,18},{133,8,163},
        {131,7,35},{0,8,114},{0,8,50},{0,9,197},{129,7,11},{0,8,98},
        {0,8,34},{0,9,165},{0,8,2},{0,8,130},{0,8,66},{0,9,229},
        {128,7,7},{0,8,90},{0,8,26},{0,9,149},{132,7,67},{0,8,122},
        {0,8,58},{0,9,213},{130,7,19},{0,8,106},{0,8,42},{0,9,181},
        {0,8,10},{0,8,138},{0,8,74},{0,9,245},{128,7,5},{0,8,86},
        {0,8,22},{65,8,0},{131,7,51},{0,8,118},{0,8,54},{0,9,205},
        {129,7,15},{0,8,102},{0,8,38},{0,9,173},{0,8,6},{0,8,134},
        {0,8,70},{0,9,237},{128,7,9},{0,8,94},{0,8,30},{0,9,157},
        {132,7,99},{0,8,126},{0,8,62},{0,9,221},{130,7,27},{0,8,110},
        {0,8,46},{0,9,189},{0,8,14},{0,8,142},{0,8,78},{0,9,253},
        {96,7,0},{0,8,81},{0,8,17},{133,8,131},{130,7,31},{0,8,113},
        {0,8,49},{0,9,195},{128,7,10},{0,8,97},{0,8,33},{0,9,163},
        {0,8,1},{0,8,129},{0,8,65},{0,9,227},{128,7,6},{0,8,89},
        {0,8,25},{0,9,147},{131,7,59},{0,8,121},{0,8,57},{0,9,211},
        {129,7,17},{0,8,105},{0,8,41},{0,9,179},{0,8,9},{0,8,137},
        {0,8,73},{0,9,243},{128,7,4},{0,8,85},{0,8,21},{144,8,3},
        {131,7,43},{0,8,117},{0,8,53},{0,9,203},{129,7,13},{0,8,101},
        {0,8,37},{0,9,171},{0,8,5},{0,8,133},{0,8,69},{0,9,235},
        {128,7,8},{0,8,93},{0,8,29},{0,9,155},{132,7,83},{0,8,125},
        {0,8,61},{0,9,219},{130,7,23},{0,8,109},{0,8,45},{0,9,187},
        {0,8,13},{0,8,141},{0,8,77},{0,9,251},{128,7,3},{0,8,83},
        {0,8,19},{133,8,195},{131,7,35},{0,8,115},{0,8,51},{0,9,199},
        {129,7,11},{0,8,99},{0,8,35},{0,9,167},{0,8,3},{0,8,131},
        {0,8,67},{0,9,231},{128,7,7},{0,8,91},{0,8,27},{0,9,151},
        {132,7,67},{0,8,123},{0,8,59},{0,9,215},{130,7,19},{0,8,107},
        {0,8,43},{0,9,183},{0,8,11},{0,8,139},{0,8,75},{0,9,247},
        {128,7,5},{0,8,87},{0,8,23},{77,8,0},{131,7,51},{0,8,119},
        {0,8,55},{0,9,207},{129,7,15},{0,8,103},{0,8,39},{0,9,175},
        {0,8,7},{0,8,135},{0,8,71},{0,9,239},{128,7,9},{0,8,95},
        {0,8,31},{0,9,159},{132,7,99},{0,8,127},{0,8,63},{0,9,223},
        {130,7,27},{0,8,111},{0,8,47},{0,9,191},{0,8,15},{0,8,143},
        {0,8,79},{0,9,255}
};

static const code distfix64[32] = {
        {128,5,1},{135,5,257},{131,5,17},{139,5,4097},{129,5,5},
        {137,5,1025},{133,5,65},{141,5,16385},{128,5,3},{136,5,513},
        {132,5,33},{140,5,8193},{130,5,9},{138,5,2049},{134,5,129},
        {142,5,32769},{128,5,2},{135,5,385},{131,5,25},{139,5,6145},
        {129,5,7},{137,5,1537},{133,5,97},{141,5,24577},{128,5,4},
        {136,5,769},{132,5,49},{140,5,12289},{130,5,13},{138,5,3073},
        {134,5,193},{142,5,49153}
};

/* ------------------------------------------------------------------------- */

static tVOID fixedtables(z_stream* strm)
{
  struct inflate_state* state;

  state = (struct inflate_state*)(&strm->state);
  if ( strm->flags & Z_FLG_DEFLATE64 )
  {
    /* deflate64 codes */
    state->lencode = lenfix64;
    state->lenbits = 9;
    state->distcode = distfix64;
    state->distbits = 5;
  }
  else
  {
    /* deflate codes */
    state->lencode = lenfix32;
    state->lenbits = 9;
    state->distcode = distfix32;
    state->distbits = 5;
  }
}

/* ------------------------------------------------------------------------- */

// Load registers with state in inflate() for speed
#define LOAD() \
    do { \
        put = strm->next_out; \
        left = strm->avail_out; \
        next = strm->next_in; \
        have = strm->avail_in; \
        hold = state->hold; \
        bits = state->bits; \
    } while (0)

// Restore state from registers in inflate()
#define RESTORE() \
    do { \
        strm->next_out = put; \
        strm->avail_out = left; \
        strm->next_in = next; \
        strm->avail_in = have; \
        state->hold = hold; \
        state->bits = bits; \
    } while (0)

// Clear the input bit accumulator
#define INITBITS() \
    do { \
        hold = 0; \
        bits = 0; \
    } while (0)

// Get a byte of input into the bit accumulator, or return from inflate()
// if there is no input available.
#define PULLBYTE() \
    do { \
        if (have == 0) goto inf_leave; \
        have--; \
        hold += (unsigned long)(*next++) << bits; \
        bits += 8; \
    } while (0)

// Assure that there are at least n bits in the bit accumulator.  If there is
// not enough available input to do that, then return from inflate().
#define NEEDBITS(n) \
    do { \
        while (bits < (unsigned)(n)) \
            PULLBYTE(); \
    } while (0)

// Return the low n bits of the bit accumulator (n < 16)
#define BITS(n) \
    ((unsigned)hold & ((1U << (n)) - 1))

// Remove n bits from the bit accumulator
#define DROPBITS(n) \
    do { \
        hold >>= (n); \
        bits -= (unsigned)(n); \
    } while (0)

// Remove zero to seven bits as needed to go to a byte boundary
#define BYTEBITS() \
    do { \
        hold >>= bits & 7; \
        bits -= bits & 7; \
    } while (0)

// Reverse the bytes in a 32-bit value
#define REVERSE(q) \
    ((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + \
     (((q) & 0xff00) << 8) + (((q) & 0xff) << 24))

// update checksum
#define UPDATE(check, buf, len) state->adler(check, buf, len)


#define MAXBITS 15

static const unsigned short lbase32[31] = { // Length codes 257..285 base
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};
static const unsigned short lext32[31] = { // Length codes 257..285 extra
        16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18,
        19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 76, 66
};
static const unsigned short dbase32[32] = { // distance codes 0..29 base
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577, 0, 0
};
static const unsigned short dext32[32] = { // distance codes 0..29 extra
        16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
        23, 23, 24, 24, 25, 25, 26, 26, 27, 27,
        28, 28, 29, 29, 64, 64
};
static const unsigned short lbase64[31] = { // length codes 257..285 base
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17,
        19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115,
        131, 163, 195, 227, 3, 0, 0
};
static const unsigned short lext64[31] = { // length codes 257..285 extra
        128, 128, 128, 128, 128, 128, 128, 128, 129, 129, 129, 129,
        130, 130, 130, 130, 131, 131, 131, 131, 132, 132, 132, 132,
        133, 133, 133, 133, 144, 76, 66
};
static const unsigned short dbase64[32] = { // distance codes 0..31 base
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49,
        65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073,
        4097, 6145, 8193, 12289, 16385, 24577, 32769, 49153
};
static const unsigned short dext64[32] = { // distance codes 0..31 extra
        128, 128, 128, 128, 129, 129, 130, 130, 131, 131, 132, 132,
        133, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 138,
        139, 139, 140, 140, 141, 141, 142, 142
};

/* ------------------------------------------------------------------------- */

static tINT inflate_table(z_stream* strm, codetype type, unsigned short* lens,
  unsigned codes, code** table, unsigned* bits, unsigned short* work)
{
    unsigned len;               // a code's length in bits
    unsigned sym;               // index of code symbols
    unsigned min, max;          // minimum and maximum code lengths
    unsigned root;              // number of index bits for root table
    unsigned curr;              // number of index bits for current table
    unsigned drop;              // code bits to drop for sub-table
    int left;                   // number of prefix codes available
    unsigned used;              // code entries in table used
    unsigned huff;              // Huffman code
    unsigned incr;              // for incrementing code, index
    unsigned fill;              // index for replicating entries
    unsigned low;               // low bits for current root entry
    unsigned mask;              // mask for low root bits
    code this;                  // table entry for duplication
    code* next;                 // next available space in table
    const unsigned short* base; // base value table to use
    const unsigned short* extra;// extra bits table to use
    int end;                    // use base and extra for symbol > end
    unsigned short count[MAXBITS+1];    // number of codes of each length
    unsigned short offs[MAXBITS+1];     // offsets in table for each length

    // accumulate lengths for codes (assumes lens[] all in 0..MAXBITS)
    for (len = 0; len <= MAXBITS; len++)
        count[len] = 0;
    for (sym = 0; sym < codes; sym++)
        count[lens[sym]]++;

    // bound code lengths, force root to be within code lengths
    root = *bits;
    for (max = MAXBITS; max >= 1; max--)
        if (count[max] != 0) break;
    if (root > max) root = max;
    if (max == 0) {                     /* no symbols to code at all */
        this.op = (unsigned char)64;    /* invalid code marker */
        this.bits = (unsigned char)1;
        this.val = (unsigned short)0;
        *(*table)++ = this;             /* make a table to force an error */
        *(*table)++ = this;
        *bits = 1;
        return 0;     /* no symbols, but wait for decoding to report error */
    }
    for (min = 1; min <= MAXBITS; min++)
        if (count[min] != 0) break;
    if (root < min) root = min;

    // check for an over-subscribed or incomplete set of lengths
    left = 1;
    for (len = 1; len <= MAXBITS; len++) {
        left <<= 1;
        left -= count[len];
        if (left < 0)
          return -1;        // over-subscribed
    }
    if (left > 0 && (type == CODES || (codes - count[0] != 1)))
        return -1;                      // incomplete set

    // generate offsets into symbol table for each length for sorting
    offs[1] = 0;
    for (len = 1; len < MAXBITS; len++)
        offs[len + 1] = offs[len] + count[len];

    // sort symbols by length, by symbol order within each length
    for (sym = 0; sym < codes; sym++)
        if (lens[sym] != 0) work[offs[lens[sym]]++] = (unsigned short)sym;

    // set up for code type
    switch (type) {
    case CODES:
        base = extra = work;    // dummy value--not used
        end = 19;
        break;
    case LENS:
        if ( strm->flags & Z_FLG_DEFLATE64 )
        {
          base = lbase64;
          extra = lext64;
        }
        else
        {
          base = lbase32;
          extra = lext32;
        }
        base -= 257;
        extra -= 257;
        end = 256;
        break;
    default:            // DISTS
        if ( strm->flags & Z_FLG_DEFLATE64 )
        {
          base = dbase64;
          extra = dext64;
        }
        else
        {
          base = dbase32;
          extra = dext32;
        }
        end = -1;
    }

    // initialize state for loop
    huff = 0;                   // starting code
    sym = 0;                    // starting code symbol
    len = min;                  // starting code length
    next = *table;              // current table to fill in
    curr = root;                // current table index bits
    drop = 0;                   // current bits to drop from code for index
    low = (unsigned)(-1);       // trigger new sub-table when len > root
    used = 1U << root;          // use root table entries
    mask = used - 1;            // mask for comparing low

    // check available table space
    if (type == LENS && used >= ENOUGH - MAXD)
        return 1;

    // process all codes and make table entries
    for (;;) {
        // create table entry
        this.bits = (unsigned char)(len - drop);
        if ((int)(work[sym]) < end) {
            this.op = (unsigned char)0;
            this.val = work[sym];
        }
        else if ((int)(work[sym]) > end) {
            this.op = (unsigned char)(extra[work[sym]]);
            this.val = base[work[sym]];
        }
        else {
            this.op = (unsigned char)(32 + 64);         // end of block
            this.val = 0;
        }

        // replicate for those indices with low len bits equal to huff
        incr = 1U << (len - drop);
        fill = 1U << curr;
        do {
            fill -= incr;
            next[(huff >> drop) + fill] = this;
        } while (fill != 0);

        // backwards increment the len-bit code huff
        incr = 1U << (len - 1);
        while (huff & incr)
            incr >>= 1;
        if (incr != 0) {
            huff &= incr - 1;
            huff += incr;
        }
        else
            huff = 0;

        // go to next symbol, update count, len
        sym++;
        if (--(count[len]) == 0) {
            if (len == max) break;
            len = lens[work[sym]];
        }

        // create new sub-table if needed
        if (len > root && (huff & mask) != low) {
            // if first time, transition to sub-tables
            if (drop == 0)
                drop = root;

            // increment past last table
            next += (SHIFT_UNIT << curr);

            // determine length of next table
            curr = len - drop;
            left = (int)(1 << curr);
            while (curr + drop < max) {
                left -= count[curr + drop];
                if (left <= 0) break;
                curr++;
                left <<= 1;
            }

            // check for enough space
            used += 1U << curr;
            if (type == LENS && used >= ENOUGH - MAXD)
                return 1;

            // point entry in root table to sub-table
            low = huff & mask;
            (*table)[low].op = (unsigned char)curr;
            (*table)[low].bits = (unsigned char)root;
            (*table)[low].val = (unsigned short)(next - *table);
        }
    }

    this.op = (unsigned char)64;                // invalid code marker
    this.bits = (unsigned char)(len - drop);
    this.val = (unsigned short)0;
    while (huff != 0) {
        // when done with sub-table, drop back to root table
        if (drop != 0 && (huff & mask) != low) {
            drop = 0;
            len = root;
            next = *table;
            this.bits = (unsigned char)len;
        }

        // put invalid code marker in table
        next[huff >> drop] = this;

        // backwards increment the len-bit code huff
        incr = 1U << (len - 1);
        while (huff & incr)
            incr >>= 1;
        if (incr != 0) {
            huff &= incr - 1;
            huff += incr;
        }
        else
            huff = 0;
    }

    // set return parameters
    *table += used;
    *bits = root;
    return 0;
}

/* ------------------------------------------------------------------------- */

#define OFF 0
#define PUP(a) *(a)++

static void inflate_fast32(z_stream* strm,unsigned int start)
{
    struct inflate_state* state;
    unsigned char* in;          // local strm->next_in
    unsigned char* last;        // while in < last, enough input available
    unsigned char* out;         // local strm->next_out
    unsigned char* beg;         // inflate()'s initial strm->next_out
    unsigned char* end;         // while out < end, enough space available
    unsigned wsize;             // window size or zero if not using window
    unsigned whave;             // valid bytes in the window
    unsigned write;             // window write index
    unsigned char* window;      // allocated sliding window, if wsize != 0
    unsigned long hold;         // local strm->hold
    unsigned bits;              // local strm->bits
    code const* lcode;          // local strm->lencode
    code const* dcode;          // local strm->distcode
    unsigned lmask;             // mask for first level of length codes
    unsigned dmask;             // mask for first level of distance codes
    code this;                  // retrieved table entry
    unsigned op;                // code bits, operation, extra bits, or
                                //  window position, window bytes to copy
    unsigned len;               // match length, unused bytes
    unsigned dist;              // match distance
    unsigned char* from;        // where to copy match from

    // copy state to local variables
    state = (struct inflate_state*)(&strm->state);
    in = strm->next_in - OFF;
    last = in + (strm->avail_in - 5);
    out = strm->next_out - OFF;
    beg = out - (start - strm->avail_out);
    end = out + (strm->avail_out - 257);
    wsize = state->wsize;
    whave = state->whave;
    write = state->write;
    window = state->window;
    hold = state->hold;
    bits = state->bits;
    lcode = state->lencode;
    dcode = state->distcode;
    lmask = (1U << state->lenbits) - 1;
    dmask = (1U << state->distbits) - 1;

    // decode literals and length/distances until end-of-block or not enough
    // input data or output space
    do {
        if (bits < 15) {
            hold += (unsigned long)(PUP(in)) << bits;
            bits += 8;
            hold += (unsigned long)(PUP(in)) << bits;
            bits += 8;
        }
        this = lcode[hold & lmask];
      dolen:
        op = (unsigned)(this.bits);
        hold >>= op;
        bits -= op;
        op = (unsigned)(this.op);
        if (op == 0) {                          // literal
            PUP(out) = (unsigned char)(this.val);
        }
        else if (op & 16) {                     // length base
            len = (unsigned)(this.val);
            op &= 15;                           // number of extra bits
            if (op) {
                if (bits < op) {
                    hold += (unsigned long)(PUP(in)) << bits;
                    bits += 8;
                }
                len += (unsigned)hold & ((1U << op) - 1);
                hold >>= op;
                bits -= op;
            }
            if (bits < 15) {
                hold += (unsigned long)(PUP(in)) << bits;
                bits += 8;
                hold += (unsigned long)(PUP(in)) << bits;
                bits += 8;
            }
            this = dcode[hold & dmask];
          dodist:
            op = (unsigned)(this.bits);
            hold >>= op;
            bits -= op;
            op = (unsigned)(this.op);
            if (op & 16) {                      // distance base
                dist = (unsigned)(this.val);
                op &= 15;                       // number of extra bits
                if (bits < op) {
                    hold += (unsigned long)(PUP(in)) << bits;
                    bits += 8;
                    if (bits < op) {
                        hold += (unsigned long)(PUP(in)) << bits;
                        bits += 8;
                    }
                }
                dist += (unsigned)hold & ((1U << op) - 1);
                hold >>= op;
                bits -= op;
                op = (unsigned)(out - beg);     // max distance in output
                if (dist > op) {                // see if copy from window
                    op = dist - op;             // distance back in window
                    if (op > whave) {
                        state->mode = BAD;  // invalid distance too far back
                        break;
                    }
                    from = window - OFF;
                    if (write == 0) {           // very common case
                        from += wsize - op;
                        if (op < len) {         // some from window
                            len -= op;
                            do {
                                PUP(out) = PUP(from);
                            } while (--op);
                            from = out - dist;  // rest from output
                        }
                    }
                    else if (write < op) {      // wrap around window
                        from += wsize + write - op;
                        op -= write;
                        if (op < len) {         // some from end of window
                            len -= op;
                            do {
                                PUP(out) = PUP(from);
                            } while (--op);
                            from = window - OFF;
                            if (write < len) {  // some from start of window
                                op = write;
                                len -= op;
                                do {
                                    PUP(out) = PUP(from);
                                } while (--op);
                                from = out - dist;      // rest from output
                            }
                        }
                    }
                    else {                      // contiguous in window
                        from += write - op;
                        if (op < len) {         // some from window
                            len -= op;
                            do {
                                PUP(out) = PUP(from);
                            } while (--op);
                            from = out - dist;  // rest from output
                        }
                    }
                    while (len > 2) {
                        PUP(out) = PUP(from);
                        PUP(out) = PUP(from);
                        PUP(out) = PUP(from);
                        len -= 3;
                    }
                    if (len) {
                        PUP(out) = PUP(from);
                        if (len > 1)
                            PUP(out) = PUP(from);
                    }
                }
                else {
                    from = out - dist;          // copy direct from output
                    do {                        // minimum length is three
                        PUP(out) = PUP(from);
                        PUP(out) = PUP(from);
                        PUP(out) = PUP(from);
                        len -= 3;
                    } while (len > 2);
                    if (len) {
                        PUP(out) = PUP(from);
                        if (len > 1)
                            PUP(out) = PUP(from);
                    }
                }
            }
            else if ((op & 64) == 0) {          // 2nd level distance code
                this = dcode[this.val + (hold & ((1U << op) - 1))];
                goto dodist;
            }
            else {
                state->mode = BAD;  // invalid distance code
                break;
            }
        }
        else if ((op & 64) == 0) {              // 2nd level length code
            this = lcode[this.val + (hold & ((1U << op) - 1))];
            goto dolen;
        }
        else if (op & 32) {                     // end-of-block
            state->mode = TYPE;
            break;
        }
        else {
            state->mode = BAD;  // invalid literal/length code
            break;
        }
    } while (in < last && out < end);

    // return unused bytes (on entry, bits < 8, so in won't go too far back)
    len = bits >> 3;
    in -= len;
    bits -= len << 3;
    hold &= (1U << bits) - 1;

    // update state and return
    strm->next_in = in + OFF;
    strm->next_out = out + OFF;
    strm->avail_in = (unsigned)(in < last ? 5 + (last - in) : 5 - (in - last));
    strm->avail_out = (unsigned)(out < end ?
                                 257 + (end - out) : 257 - (out - end));
    state->hold = hold;
    state->bits = bits;
    return;
}

/* ------------------------------------------------------------------------- */

static const unsigned short order[19] = { // permutation of code lengths
     16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

static tINT InflateData(z_stream* strm, tINT flush)
{
    struct inflate_state* state;
    unsigned char* next;        // next input
    unsigned char* put;         // next output
    unsigned have, left;        // available input and output
    unsigned long hold;         // bit buffer
    unsigned bits;              // bits in bit buffer
    unsigned in, out;           // save starting available input and output
    unsigned copy;              // number of stored or match bytes to copy
    unsigned char* from;        // where to copy match bytes from
    code this;                  // current decoding table entry
    code last;                  // parent table entry
    unsigned len;               // length to copy for repeats, bits to drop
    int ret;                    // return code
    unsigned ext_mask;          // extra bits mask (deflate 64)

    // check for deflate 64
    if ( strm->flags & Z_FLG_DEFLATE64 )
      ext_mask = 31;
    else
      ext_mask = 15;

    if ( strm == Z_NULL || strm->next_out == Z_NULL ||
        (strm->next_in == Z_NULL && strm->avail_in != 0) )
    {
        return Z_STREAM_ERROR;
    }

    state = (struct inflate_state*)(&strm->state);
    if (state->mode == TYPE)
      state->mode = TYPEDO;      // skip check
    LOAD();
    in = have;
    out = left;
    ret = Z_OK;
    for (;;)
        switch (state->mode) {
        case HEAD:
            if (state->wrap == 0) {
                state->mode = TYPEDO;
                break;
            }
            NEEDBITS(16);
            if (!(state->wrap & 1) ||   // check if zlib header allowed
                ((BITS(8) << 8) + (hold >> 8)) % 31) 
            {
                state->mode = BAD;  // incorrect header check
                break;
            }
            if (BITS(4) != Z_DEFLATED) {
                state->mode = BAD;  // unknown compression method
                break;
            }
            DROPBITS(4);
            if (BITS(4) + 8 > state->wbits) {
                state->mode = BAD;  // invalid window size
                break;
            }
            if ( Z_NULL != state->adler )
              state->check = state->adler(0L, Z_NULL, 0);
            state->mode = hold & 0x200 ? DICTID : TYPE;
            INITBITS();
            break;
        case DICTID:
            NEEDBITS(32);
            state->check = REVERSE(hold);
            INITBITS();
            state->mode = DICT;
        case DICT:
            if (state->havedict == 0) {
                RESTORE();
                return Z_NEED_DICT;
            }
            if ( Z_NULL != state->adler )
              state->check = state->adler(0L, Z_NULL, 0);
            state->mode = TYPE;
        case TYPE:
            if (flush == Z_BLOCK)
              goto inf_leave;
        case TYPEDO:
            if (state->last) {
                BYTEBITS();
                state->mode = CHECK;
                break;
            }
            NEEDBITS(3);
            state->last = BITS(1);
            DROPBITS(1);
            if ( strm->flags & Z_FLG_PETITE )
            {
              // petite stream
              switch (BITS(2)) {
              case 0:                             // stored block
                  state->mode = STORED;
                  break;
              case 1:                             // fixed block
                  state->mode = TABLE;            // dynamic block
                  break;
              case 2:
              case 3:
                  state->mode = BAD;              // invalid block type
              }
            }
            else
            {
              switch (BITS(2)) {
              case 0:                             // stored block
                  state->mode = STORED;
                  break;
              case 1:                             // fixed block
                  fixedtables(strm);
                  state->mode = LEN;              // decode codes
                  break;
              case 2:                             // dynamic block
                  state->mode = TABLE;
                  break;
              case 3:
                  state->mode = BAD;              // invalid block type
              }
            }
            DROPBITS(2);
            break;
        case STORED:
            BYTEBITS();                         // go to byte boundary
            if ( strm->flags & Z_FLG_STORE16 )
            {
              // hacked block (nsis)
              NEEDBITS(16);
            }
            else
            {
              NEEDBITS(32);
              if ((hold & 0xffff) != ((hold >> 16) ^ 0xffff)) {
                  state->mode = BAD; // invalid stored block lengths
                  break;
              }
            }
            state->length = (unsigned)hold & 0xffff;
            INITBITS();
            state->mode = COPY;
        case COPY:
            copy = state->length;
            if (copy) {
                if (copy > have) copy = have;
                if (copy > left) copy = left;
                if (copy == 0) goto inf_leave;
                zmemcpy(put, next, copy);
                have -= copy;
                next += copy;
                left -= copy;
                put += copy;
                state->length -= copy;
                break;
            }
            state->mode = TYPE;
            break;
        case TABLE:
            NEEDBITS(14);
            state->nlen = BITS(5) + 257;
            DROPBITS(5);
            state->ndist = BITS(5) + 1;
            DROPBITS(5);
            state->ncode = BITS(4) + 4;
            DROPBITS(4);
            state->have = 0;
            state->mode = LENLENS;
        case LENLENS:
            while (state->have < state->ncode) {
                NEEDBITS(3);
                state->lens[order[state->have++]] = (unsigned short)BITS(3);
                DROPBITS(3);
            }
            while (state->have < 19)
                state->lens[order[state->have++]] = 0;
            state->next = state->codes;
            state->lencode = (code const*)(state->next);
            state->lenbits = 7;
            ret = inflate_table(strm, CODES, state->lens, 19, &(state->next),
                                &(state->lenbits), state->work);
            if (ret) {
                state->mode = BAD;  // invalid code lengths set
                break;
            }
            state->have = 0;
            state->mode = CODELENS;
        case CODELENS:
            while (state->have < state->nlen + state->ndist) {
                for (;;) {
                    this = state->lencode[BITS(state->lenbits)];
                    if ((unsigned)(this.bits) <= bits) break;
                    PULLBYTE();
                }
                if (this.val < 16) {
                    NEEDBITS(this.bits);
                    DROPBITS(this.bits);
                    state->lens[state->have++] = this.val;
                }
                else {
                    if (this.val == 16) {
                        NEEDBITS(this.bits + 2);
                        DROPBITS(this.bits);
                        if (state->have == 0) {
                            state->mode = BAD; // invalid bit length repeat
                            break;
                        }
                        len = state->lens[state->have - 1];
                        copy = 3 + BITS(2);
                        DROPBITS(2);
                    }
                    else if (this.val == 17) {
                        NEEDBITS(this.bits + 3);
                        DROPBITS(this.bits);
                        len = 0;
                        copy = 3 + BITS(3);
                        DROPBITS(3);
                    }
                    else {
                        NEEDBITS(this.bits + 7);
                        DROPBITS(this.bits);
                        len = 0;
                        copy = 11 + BITS(7);
                        DROPBITS(7);
                    }
                    if (state->have + copy > state->nlen + state->ndist) {
                        state->mode = BAD; // invalid bit length repeat
                        break;
                    }
                    while (copy--)
                        state->lens[state->have++] = (unsigned short)len;
                }
            }

            /* handle error breaks in while */
            if (state->mode == BAD) break;

            /* build code tables */
            state->next = state->codes;
            state->lencode = (code const*)(state->next);
            state->lenbits = 9;
            ret = inflate_table(strm, LENS, state->lens, state->nlen,
              &(state->next), &(state->lenbits), state->work);
            if (ret) {
                state->mode = BAD;  // invalid literal/lengths set
                break;
            }
            state->distcode = (code const*)(state->next);
            state->distbits = 6;
            ret = inflate_table(strm, DISTS, state->lens + state->nlen,
              state->ndist, &(state->next), &(state->distbits), state->work);
            if (ret) {
                state->mode = BAD;  // invalid distances set
                break;
            }
            state->mode = LEN;
        case LEN:
            if (have >= 6 && left >= 258 && !(strm->flags & Z_FLG_DEFLATE64))
            {
                RESTORE();
                inflate_fast32(strm, out);
                LOAD();
                break;
            }
            for (;;) {
                this = state->lencode[BITS(state->lenbits)];
                if ((unsigned)(this.bits) <= bits) break;
                PULLBYTE();
            }
            if (this.op && (this.op & 0xf0) == 0) {
                last = this;
                for (;;) {
                    this = state->lencode[last.val +
                            (BITS(last.bits + last.op) >> last.bits)];
                    if ((unsigned)(last.bits + this.bits) <= bits) break;
                    PULLBYTE();
                }
                DROPBITS(last.bits);
            }
            DROPBITS(this.bits);
            state->length = (unsigned)this.val;
            if ((int)(this.op) == 0) {
                state->mode = LIT;
                break;
            }
            if (this.op & 32) {
                state->mode = TYPE;  // end of block
                break;
            }
            if (this.op & 64) {
                state->mode = BAD; // invalid literal/length code
                break;
            }
            state->extra = (unsigned)(this.op) & ext_mask;
            state->mode = LENEXT;
        case LENEXT:
            if (state->extra) {
                NEEDBITS(state->extra);
                state->length += BITS(state->extra);
                DROPBITS(state->extra);
            }
            state->mode = DIST;
        case DIST:
            for (;;) {
                this = state->distcode[BITS(state->distbits)];
                if ((unsigned)(this.bits) <= bits) break;
                PULLBYTE();
            }
            if ((this.op & 0xf0) == 0) {
                last = this;
                for (;;) {
                    this = state->distcode[last.val +
                            (BITS(last.bits + last.op) >> last.bits)];
                    if ((unsigned)(last.bits + this.bits) <= bits) break;
                    PULLBYTE();
                }
                DROPBITS(last.bits);
            }
            DROPBITS(this.bits);
            if (this.op & 64) {
                state->mode = BAD; // invalid distance code
                break;
            }
            state->offset = (unsigned)this.val;
            state->extra = (unsigned)(this.op) & 15;
            state->mode = DISTEXT;
        case DISTEXT:
            if (state->extra) {
                NEEDBITS(state->extra);
                state->offset += BITS(state->extra);
                DROPBITS(state->extra);
            }
            if (state->offset > state->whave + out - left) {
                state->mode = BAD; // invalid distance too far back
                break;
            }
            state->mode = MATCH;
        case MATCH:
            if (left == 0) goto inf_leave;
            copy = out - left;
            if (state->offset > copy) {         // copy from window
                copy = state->offset - copy;
                if (copy > state->write) {
                    copy -= state->write;
                    from = state->window + (state->wsize - copy);
                }
                else
                    from = state->window + (state->write - copy);
                if (copy > state->length) copy = state->length;
            }
            else {                              // copy from output
                from = put - state->offset;
                copy = state->length;
            }
            if (copy > left) copy = left;
            left -= copy;
            state->length -= copy;
            do {
                *put++ = *from++;
            } while (--copy);
            if (state->length == 0) state->mode = LEN;
            break;
        case LIT:
            if (left == 0) goto inf_leave;
            *put++ = (unsigned char)(state->length);
            left--;
            state->mode = LEN;
            break;
        case CHECK:
            if ( state->wrap && !(strm->flags & Z_FLG_SKIPCHECK) )
            {
                NEEDBITS(32);
                out -= left;
                strm->total_out += out;
                if (out && Z_NULL != state->adler)
                    state->check = UPDATE(state->check, put - out, out);
                out = left;
                if ( REVERSE(hold) != state->check && 
                     !(strm->flags & Z_FLG_DUMMYCHECK) )
                {
                    state->mode = BAD; // incorrect data check
                    break;
                }
                INITBITS();
            }
            state->mode = DONE;
        case DONE:
            ret = Z_STREAM_END;
            goto inf_leave;
        case BAD:
            ret = Z_DATA_ERROR;
            goto inf_leave;
        case MEM:
            return Z_MEM_ERROR;
        case SYNC:
        default:
            return Z_STREAM_ERROR;
        }

     // return from inflate(), updating the total counts and the check value.
     // if there was no progress during the inflate() call, return a buffer
     // error.  Call updatewindow() to create and/or update the window state.
     // note: a memory error from inflate() is non-recoverable.
  inf_leave:
    RESTORE();
    if ( strm->self->bForceWnd || state->wsize || (state->mode < CHECK && out != strm->avail_out))
    {
        if (updatewindow(strm, out)) {
            state->mode = MEM;
            return Z_MEM_ERROR;
        }
    }
    in -= strm->avail_in;
    out -= strm->avail_out;
    strm->total_in += in;
    strm->total_out += out;
    if ( state->wrap && out && Z_NULL != state->adler )
         state->check = UPDATE(state->check, strm->next_out - out, out);
    if (((in == 0 && out == 0) || flush == Z_FINISH) && ret == Z_OK)
        ret = Z_BUF_ERROR;
    return ret;
}

/* ------------------------------------------------------------------------- */

static tERROR InflateRotate(hi_Transformer _this, hSEQ_IO hRotObj)
{
  tERROR Error;

  if ( NULL != hRotObj )
  {
    /* send yeld message */
    if ( PR_FAIL(Error = CALL_SYS_SendMsg(hRotObj,
         pmc_PROCESSING, pm_PROCESSING_YIELD, _this, 0 ,0)) )
    {
      return(Error); /* stopped */
    }
  }

  return(errOK);
}

/* ------------------------------------------------------------------------- */

#define INT_RBUF_SIZE  (8*1024)
#define INT_WBUF_SIZE  (8*1024)

#ifndef MIN
# define MIN(a,b)  ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b)  ((a)>(b)?(a):(b))
#endif

static tERROR InflateDecode(hi_Transformer _this, tQWORD PSize, tQWORD USize,
                            tQWORD* Written, tQWORD* Readed)
{
  tINFLATE_DATA* Data;
  tERROR  Error = errNOT_OK;
  tDWORD  Cur, Rdt, Wrt;
  tINT    Ret = Z_ERRNO;
  tUINT   RotCnt;
  tQWORD  RetVal = 0, InProcessed = 0;
  tQWORD  RdrOfs = 0;
  z_stream* Stm;
  hSEQ_IO hSrc;
  hSEQ_IO hDst;
  hSEQ_IO hRot;

  /* setup buffs */
  Data = _this->data;
  if ( !Data->RBuf || !Data->RBufSize || !Data->WBuf || !Data->WBufSize )
  {
    tBYTE* Buf;
    tDWORD BufSize;

    /* alloc buffers */
    if ( NULL == (Buf = Data->IntIOBuf) )
    {
      BufSize = (INT_RBUF_SIZE + INT_WBUF_SIZE);
      if ( PR_FAIL(Error = CALL_SYS_ObjHeapAlloc(_this, &Buf, BufSize)) )
        goto loc_exit;
    }

    Data->RBuf = Buf;
    Data->WBuf = &Buf[INT_RBUF_SIZE];
    Data->RBufSize = INT_RBUF_SIZE;
    Data->WBufSize = INT_WBUF_SIZE;
    Data->IntIOBuf = Buf;
  }

  /* init zlib stream */
  Stm = &Data->Stream;
  Stm->next_in   = Data->RBuf;
  Stm->next_out  = Data->WBuf;
  Stm->avail_in  = 0;
  Stm->avail_out = (tUINT)MIN(Data->WBufSize, USize);

  /* init context */
  if ( !Data->Inited )
  {
    tUINT Flags = 0;
    tINT  Mul;

    /* calc flags */
    if ( Data->WndMul <= 0 )
      Flags |= Z_FLG_NOWRAP;
    Mul = (Data->WndMul < 0) ? (0-Data->WndMul) : (Data->WndMul);
    if ( Data->bInf64 || Mul >= 16 )
      Flags |= Z_FLG_DEFLATE64;
    if ( Data->bNsis )
      Flags |= Z_FLG_STORE16;
    if ( Data->bPetite )
      Flags |= Z_FLG_PETITE;
    if ( Data->bChkZLib && Data->WndMul >= 0 )
      Flags |= Z_FLG_DUMMYCHECK;

    /* initialize */
    Stm->flags = Flags;
    if ( Z_OK != (Ret = InflateInit(Stm, Flags)) )
      goto loc_exit; /* error */

    Data->Inited = cTRUE;
  }

  RotCnt = 0;
  hSrc = Data->hInputIO;
  hDst = Data->hOutputIO;
  InProcessed = Stm->total_in;

  /* get reader offset */
  if ( hSrc ) CALL_SeqIO_Seek(hSrc, &RdrOfs, 0, cSEEK_FORWARD);

  /* process stream */
  Ret   = Z_OK;
  Error = errOK;
  while ( Stm->total_in < PSize && Stm->total_out < USize )
  {
    /* yeld */
    hRot = (hSrc != NULL) ? (hSrc) : (hDst);
    if ( PR_FAIL(Error = InflateRotate(_this, hRot)) )
      return(Error);

    /* read block if need */
    if ( Stm->avail_in == 0 )
    {
      Cur = (tDWORD)MIN(PSize - Stm->total_in, Data->RBufSize);

      if ( NULL == hSrc )
        Rdt = Cur;
      else
      {
        Error = CALL_SeqIO_Read(hSrc, &Rdt, Data->RBuf, Cur);
        if ( PR_FAIL(Error) )
          goto loc_exit; /* error */
        if ( Rdt == 0 )
          break; /* done */
      }
      if ( NULL != Data->InputCB )
      {
        if ( 0 == (Rdt = Data->InputCB(Data->CBUser, Data->RBuf, Rdt)) )
          break; /* done */
      }

      Stm->avail_in = Rdt;
      Stm->next_in  = Data->RBuf;
    }

    /* unpack buffer */
    Ret = InflateData(Stm, Z_PARTIAL_FLUSH);
    if ( Ret == Z_STREAM_END )
      break; /* done */
    else if ( Ret != Z_OK )
      break; /* error */

    /* flush output if need */
    if ( Stm->avail_out == 0 )
    {
      Cur = (tDWORD)(Stm->next_out - Data->WBuf);
      Cur = (tDWORD)MIN(USize - RetVal, Cur);

      if ( NULL == Data->OutputCB )
        Wrt = Cur;
      else
      {
        if ( 0 == (Wrt = Data->OutputCB(Data->CBUser, Data->WBuf, Cur)) )
          break; /* done */
      }
      if ( NULL != hDst )
      {
        Error = CALL_SeqIO_Write(hDst, &Wrt, Data->WBuf, Wrt);
        if ( PR_FAIL(Error) )
          goto loc_exit; /* error */
        if ( Wrt == 0 )
          break; /* done */
      }

      RetVal += Wrt;
      if ( Wrt != Cur )
        break; /* done */

      Stm->next_out  = Data->WBuf;
      Stm->avail_out = (tDWORD)MIN(USize - Stm->total_out, Data->WBufSize);
    }
    
		if ( NULL == Data->OutputCB && NULL == hDst )
      break; /* done */
  }

  /* seek reader to next block */
  if ( hSrc ) CALL_SeqIO_Seek(hSrc, NULL, RdrOfs + Stm->total_in, cSEEK_SET);

  /* flush output if need */
  if ( 0 != (Cur = (tDWORD)(Stm->next_out - Data->WBuf)) )
  {
    Cur = (tDWORD)MIN(USize - RetVal, Cur);
    if ( NULL == Data->OutputCB )
      Wrt = Cur;
    else
    {
      Wrt = Data->OutputCB(Data->CBUser, Data->WBuf, Cur);
    }
    if ( NULL != hDst )
    {
      Error = CALL_SeqIO_Write(hDst, &Wrt, Data->WBuf, Wrt);
      if ( PR_FAIL(Error) )
        goto loc_exit; /* error */
    }

    RetVal += Wrt;
  }

  /* flush dst stream */
  if ( hDst ) CALL_SeqIO_Flush(hDst);

loc_exit:
  if ( NULL != Written )
    *Written = RetVal;
  if ( NULL != Readed )
    *Readed = Stm->total_in - InProcessed;
  if ( PR_FAIL(Error) )
    return(Error);
  if ( Ret != Z_STREAM_END && Ret != Z_OK )
    Error = errOBJECT_DATA_CORRUPTED;
  if ( NULL == Data->OutputCB && NULL == hDst && Ret == Z_STREAM_END )
    Error = errEOF;
  return(Error);
}

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
  tINFLATE_DATA* Data;
  tERROR Error = errOK;
  PR_TRACE_A0( _this, "Enter Transformer::ObjectInit method" );

  Data = _this->data;
  memset(Data, 0, sizeof(tINFLATE_DATA));
  Data->Self = (hOBJECT)(_this);
  Data->Stream.self = Data;

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
  tINFLATE_DATA* Data;
  tERROR Error = errOK;
  PR_TRACE_A0( _this, "Enter Transformer::ObjectPreClose method" );

  Data = _this->data;
  if ( NULL != Data->IntWnd )
  {
    CALL_SYS_ObjHeapFree(_this, Data->IntWnd);
    Data->IntWnd = NULL;
  }
  if ( NULL != Data->IntIOBuf )
  {
    CALL_SYS_ObjHeapFree(_this, Data->IntIOBuf);
    Data->IntIOBuf = NULL;
  }

  PR_TRACE_A1( _this, "Leave Transformer::ObjectPreClose method, ret %terr", Error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessChunck )
// Result comment
//    количество реально выданных байт (>=dwSize)
//
// Parameters are:
static tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* Result, tDWORD dwSize )
{
  tINFLATE_DATA* Data;
  tERROR Error = errOK;
  tQWORD RetVal = 0;
  tQWORD PSize;
  PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

  Data  = _this->data;
  PSize = (Data->ObjPSize) ? (Data->ObjPSize) : (0xffffffffffffffff);
  Error = InflateDecode(_this, PSize, dwSize, &RetVal, NULL);

  if ( Result )
    *Result = (tDWORD)(RetVal);
  PR_TRACE_A2( _this, "Leave Transformer::ProcessChunck method, ret tDWORD = %u, %terr", RetVal, Error );
  return(Error);
}
// AVP Prague stamp end



/* ------------------------------------------------------------------------- */

// AVP Prague stamp begin( External (user called) interface method implementation, Proccess )
// Result comment
//    размер распакованных данных
// Parameters are:
static tERROR pr_call Transformer_Proccess(hi_Transformer _this, tQWORD* Result)
{
  tINFLATE_DATA* Data;
  tERROR Error  = errOK;
  tQWORD RetVal = 0;
  tQWORD PSize;
  tQWORD USize;
  PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

  Data = _this->data;
  PSize = (Data->ObjPSize) ? (Data->ObjPSize) : (0xffffffffffffffff);
  USize = (Data->ObjUSize) ? (Data->ObjUSize) : (0xffffffffffffffff);
  Error = InflateDecode(_this, PSize, USize, &RetVal, NULL);

  if ( Result )
    *Result = RetVal;
  PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, %terr", RetVal, Error);
  return(Error);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ProcessQChunck )
// Parameters are:
static tERROR pr_call Transformer_ProcessQChunck( hi_Transformer _this, tQWORD* Result, tQWORD qwSize )
{
  tINFLATE_DATA* Data;
  tERROR Error  = errOK;
  tQWORD RetVal = 0;
  tQWORD PSize;
  PR_TRACE_A0( _this, "Enter Transformer::ProcessQChunck method" );

  Data  = _this->data;
  PSize = (Data->ObjPSize) ? (Data->ObjPSize) : (0xffffffffffffffff);
  Error = InflateDecode(_this, PSize, qwSize, &RetVal, NULL);

  if ( Result )
    *Result = RetVal;
  PR_TRACE_A2( _this, "Leave Transformer::ProcessQChunck method, ret tQWORD = %I64u, %terr", RetVal, Error );
  return(Error);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Decode )
// Parameters are:
static tERROR pr_call Transformer_Decode(hi_Transformer _this, tQWORD* Result)
{
  tINFLATE_DATA* Data;
  tERROR Error  = errOK;
  tQWORD RetVal = 0;
  tQWORD PSize;
  tQWORD USize;
  PR_TRACE_A0( _this, "Enter Transformer::Decode method" );

  Data  = _this->data;
  PSize = (Data->ObjPSize) ? (Data->ObjPSize) : (0xffffffffffffffff);
  USize = (Data->ObjUSize) ? (Data->ObjUSize) : (0xffffffffffffffff);
  Error = InflateDecode(_this, PSize, USize, &RetVal, NULL);

  if ( Result )
    *Result = RetVal;
  PR_TRACE_A2( _this, "Leave Transformer::Decode method, ret tQWORD = %I64u, %terr", ret_val, Error );
  return(Error);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Parameters are:
static tERROR pr_call Transformer_Reset( hi_Transformer _this, tINT Method )
{
  tINFLATE_DATA* Data;
  tERROR Error = errOK;
  PR_TRACE_A0( _this, "Enter Transformer::Reset method" );

  Data = _this->data;
  switch ( Method )
  {
  case INFLATE_RESET_STATE:
    InflateReset(&Data->Stream, cTRUE);
    Data->ObjPSize = 0;
    Data->ObjUSize = 0;
    break;
  case INFLATE_RESET_FULL:
    if ( NULL != Data->IntWnd )
    {
      CALL_SYS_ObjHeapFree(_this, Data->IntWnd);
      Data->IntWndSize = 0;
      Data->IntWnd = NULL;
    }
    if ( NULL != Data->IntIOBuf )
    {
      CALL_SYS_ObjHeapFree(_this, Data->IntIOBuf);
      Data->IntIOBuf = NULL;
    }

    memset(Data, 0, sizeof(tINFLATE_DATA));
    Data->Self = (hOBJECT)(_this);
    Data->Stream.self = Data;
    break;
  case INFLATE_RESET_STATEIGNOREWND:
    InflateReset(&Data->Stream, cFALSE);
    Data->ObjPSize = 0;
    Data->ObjUSize = 0;
    break;
  default:
    Error = errNOT_IMPLEMENTED;
  }

  PR_TRACE_A1( _this, "Leave Transformer::Reset method, ret %terr", Error );
  return(Error);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ProcessEx )
// Parameters are:
tERROR pr_call Transformer_ProcessEx( hi_Transformer _this, tQWORD* p_Written, tQWORD* p_Readed )
{
  tINFLATE_DATA* Data;
  tERROR Error = errOK;
  tQWORD PSize;
  tQWORD USize;
  PR_TRACE_A0( _this, "Enter Transformer::ProcessEx method" );

  Data  = _this->data;
  PSize = (Data->ObjPSize) ? (Data->ObjPSize) : (0xffffffffffffffff);
  USize = (Data->ObjUSize) ? (Data->ObjUSize) : (0xffffffffffffffff);
  Error = InflateDecode(_this, PSize, USize, p_Written, p_Readed);

  PR_TRACE_A1( _this, "Leave Transformer::ProcessEx method, ret %terr", Error );
  return Error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, Transformer_VERSION )
  mSHARED_PROPERTY_PTR(pgINTERFACE_COMMENT, InflateTransInterfCmt, 21 )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, tINFLATE_DATA, ObjUSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, tINFLATE_DATA, hInputIO, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, tINFLATE_DATA, hOutputIO, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_BUFF, tINFLATE_DATA, RBuf, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_BUFF_SIZE, tINFLATE_DATA, RBufSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_BUFF, tINFLATE_DATA, WBuf, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_BUFF_SIZE, tINFLATE_DATA, WBufSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_WINDOW, tINFLATE_DATA, Window, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_WINDOW_SIZE, tINFLATE_DATA, WindowSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgINPUT_CALLBACK, tINFLATE_DATA, InputCB, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgOUTPUT_CALLBACK, tINFLATE_DATA, OutputCB, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgCALLBACK_USER, tINFLATE_DATA, CBUser, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgTRANSFORM_INPUT_SIZE, tINFLATE_DATA, ObjPSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgFORCE_WINDOW_CREATION, tINFLATE_DATA, bForceWnd, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( plINFLATE_PETITE_FLAG, tINFLATE_DATA, bPetite, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( plINFLATE_WINDOW_SIZE, tINFLATE_DATA, WindowSize, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( plINFLATE_WINDOW, tINFLATE_DATA, Window, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( plINFLATE_WINDOW_MULTIPIER, tINFLATE_DATA, WndMul, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( plINFLATE_INF64_FLAG, tINFLATE_DATA, bInf64, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( plINFLATE_NSIS_FLAG, tINFLATE_DATA, bNsis, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( plINFLATE_ZLIBCHECK_FLAG, tINFLATE_DATA, bChkZLib, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(Transformer_PropTable)
// AVP Prague stamp end



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
static iInflateTransformerVtbl e_Transformer_vtbl = 
{
  (fnpTransformer_ProcessChunck) Transformer_ProcessChunck,
  (fnpTransformer_Proccess) Transformer_Proccess,
  (fnpTransformer_ProcessQChunck) Transformer_ProcessQChunck,
  (fnpTransformer_Decode)   Transformer_Decode,
  (fnpTransformer_Reset)    Transformer_Reset,
  (fnpInflateTransformer_ProcessEx) Transformer_ProcessEx
};
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "Transformer". Register function
tERROR pr_call Transformer_Register( hROOT root ) 
{
  tERROR error;

  PR_TRACE_A0( root, "Enter Transformer::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_TRANSFORMER,                        // interface identifier
    PID_INFLATE,                            // plugin identifier
    0x00000000,                             // subtype identifier
    Transformer_VERSION,                    // interface version
    VID_ANDY,                               // interface developer
    &i_Transformer_vtbl,                    // internal(kernel called) function table
    (sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
    &e_Transformer_vtbl,                    // external function table
    (sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
    Transformer_PropTable,                  // property table
    mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
    sizeof(tINFLATE_DATA),                  // memory size
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




#ifndef _CONST_H_
#define _CONST_H_

#include <Prague/prague.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_string.h>

#ifdef _DEBUG
# include <windows.h>
# include <stdio.h>
#endif

// ----------------------------------------------------------------------------
#if defined (_WIN32)
#include <pshpack1.h>
#endif
// ----------------------------------------------------------------------------

#pragma pack(1)

//#define DEBUG_DUMP
//#define DEBUG_PASS
//#define DEBUG_PASS_LOG
//#define DEBUG_MEMALLOC

// global definitions
#define MAX_NAME      (260)            // maximum file name length
#define RD_BUFF_SIZE  (0x8000)         // size of input buffer
#define TMP_BUFF_SIZE (0x10000)        // size of temp  buffer
#ifndef MAX_PATH
# define MAX_PATH     (0x1000)         // max path buffer
#endif

// ----------------------------------------------------------------------------

#define ALIGN(X,Y)      (((X)+(Y)-1)&(~((Y)-1)))

typedef tUINT    uint;       // at least 32 bit
typedef tWORD  ushort;

typedef tBYTE    byte;       // 8 bit
typedef tWORD    word;
typedef tDWORD  dword;
typedef tBOOL rarbool;

#ifndef TRUE
# define FALSE (0)
# define TRUE  (!FALSE)
#endif

#ifndef NULL
# define NULL ((tVOID*)(0))
#endif

//#define SA_DEBUG        // sub allocater debug
//#define VM_DEBUG        // virtual machine debug
//#define MEM_DEBUG       // own memory management debug
//#define HDR_DEBUG       // RAR headers debug

#ifndef RAR_LARGE
# define RAR_LARGE
#endif
#ifndef AVPLOAD
# define AVPLOAD
#endif

#define crlf

//#define MACHINE_IS_STRICT_ALIGNMENT
//#define MACHINE_IS_BIG_ENDIAN

//typedef __int64 Int64;
//typedef __int64 filesize;
typedef tDWORD filesize;
#define FILE_SIZE_MAX ((filesize)(-1))

#ifdef _DEBUG
# define RATTRIB 
# define RINLINE
# define RSTATIC
#else
# define RATTRIB  __fastcall 
# define RINLINE  __inline
# define RSTATIC
#endif

#define VM_TEMPCODE_SIZE (0x10000)

#define Min(x,y) (((x)<(y)) ? (x):(y))
#define Max(x,y) (((x)>(y)) ? (x):(y))

#define CODEBUFSIZE     0x4000

#define MAXWINSIZE      (data->WindowSize)
#define MAXWINMASK      (data->WindowMask)

#define LOW_DIST_REP_COUNT 16

#define N1          4
#define N2          4
#define N3          4
#define N4          ((128+3-1*N1-2*N2-3*N3)/4)
#define N_INDEXES   (N1+N2+N3+N4)

#ifdef  MACHINE_IS_STRICT_ALIGNMENT
# define DUMMY   ushort dummy;
# define DUMMY1  ushort dummy1;
#else
# define DUMMY
# define DUMMY1
#endif

#ifndef _DEBUG
# ifndef SEEK_SET
#  define SEEK_SET  (0)
#  define SEEK_CUR  (1)
#  define SEEK_END  (2)
# endif
#endif

// ----------------------------------------------------------------------------
// *** from input.h ***
// ----------------------------------------------------------------------------

typedef enum
{
  cINPUT_FILE,
  cINPUT_BUFF,
  cINPUT_SOURCE
} eINPUT_TYPE   ;

typedef struct sINPUT
{
  tBYTE  (RATTRIB *ByteGet)(struct sINPUT *source);    // this input procedure
  eINPUT_TYPE type;               // type of this structure

  tUINT       limit;              // max bytes to read from source
  tUINT       bitbuf;             // bit buffer
  tUINT       bitcnt;             // bit count in bit buffer
  tBOOL       eof;                // eof of source reached

  // source for this source data
  struct sINPUT   *input;     // source 'this' for SOURCE mode
  struct sRAR     *rardata;   // rardata for FILE mode;

  // file part
  tBYTE    *inbuf;             // pointer to input buffer
  tUINT     bufsize;           // input buffer size
  tUINT     inptr;             // current input buffer pointer
  tUINT     incnt;             // bytes in input buffer
  tDWORD file_pos;             // file position of begin of input buffer

  tDWORD   hd_crc;             // crc while reading hdr
  tDWORD   rd_crc;             // buffer reading crc
} tINPUT;

// ----------------------------------------------------------------------------
// *** from mem.h ***
// ----------------------------------------------------------------------------

#define MEMENTRIES 16

typedef rarbool (RATTRIB *tMEMFIXPROC)(struct sRAR *data, void *newptr, void *oldptr);

typedef struct sMEMENTRY {
  void    *ptr;
  uint    size;
  tMEMFIXPROC MemFix;
} tMEMENTRY;

typedef struct sARRAY {
  void *ptr  ;      // pointer to allocated array
  word  count;      // number of elements in array
  word  size ;      // sizeof element
} tARRAY   ;

#define ArrayGet(array, index, type)  ((type*)(array.ptr)+(index))
#ifdef MEM_DEBUG
#define ArrayAdd iArrayAdd
#else
#define ArrayAdd(data,array,count,comment) iArrayAdd(data,array,count)
#endif

typedef struct sUNIT {
  #ifdef MEM_DEBUG
  char *comment;
  #endif
  word state;     // number of units in group, 0 - unit in group
} tUNIT   ;

#define UNIT_COUNT  0x7FFF
#define UNIT_USED   0x8000

typedef struct sMEMPAGE
{
  struct sMEMPAGE *prev;  // prev allocated page
  struct sMEMPAGE *next;  // next allocated page
  uint    free_count   ;  // count of free units in page
  uint    unitsize     ;  // allocation unit size
  uint    count        ;  // units in one page
  uint    size         ;  // end of page
  byte    *start       ;  // start of page
  byte    *old         ;  // old start of page
  tUNIT   *units       ;  // units descriptors
  tUNIT   *units_end   ;
} tMEMPAGE;

#ifdef MEM_DEBUG
#define MemRealloc iMemRealloc
#else
#define MemRealloc(data, retval, oldptr, newsize, comment) iMemRealloc(data, retval, oldptr, newsize)
#endif

// debug procedures
#ifdef MEM_DEBUG
// in proto.h
#else
#define MemCheck(a)
#define MemDump(a)
#define ArrayDump(a,b)
#endif

// ----------------------------------------------------------------------------
// *** suballoc.h ***
// ----------------------------------------------------------------------------

typedef struct sMEM_BLK {
  struct sMEM_BLK *next;
  ushort Stamp;
  DUMMY
  struct sMEM_BLK *prev;
  ushort NU;
  DUMMY1
} tMEM_BLK;

// memory fix structure, should be compatible with:
// tMEM_BLK (sMEM_BLK)  (2 fixes)
// tSTATE               (1 fix)
// tPPM_CONTEXT         (2 fixes)

typedef struct sMEM_FIX
{
    byte    *ptr;
    ushort  none;
    DUMMY
}  tMEM_FIX  ;

typedef struct sNODE {
  struct sNODE *next;
} tNODE;


typedef struct sSUB_ALLOCATOR {
  uint    SubAllocatorSize;
  uint    AllocSize;              // only for exact reallocation
  tNODE   FreeList[N_INDEXES];
  byte    *HeapStart;
  byte    *LoUnit;
  byte    *HiUnit;
  byte    Units2Indx[128];
  byte    Indx2Units[N_INDEXES];
  byte    GlueCount;
  byte    *pText;
  byte    *UnitsStart;
  byte    *HeapEnd;
  byte    *FakeUnitsStart;
} tSUB_ALLOCATOR, tSubAllocator;

#define SA_U2B(NU) ((UNIT_SIZE) * (NU))

// ----------------------------------------------------------------------------
// *** from model.h ***
// ----------------------------------------------------------------------------

#define TOP (1 << 24)
#define BOT (1 << 15)

#define MAX_O (64)                   // maximum allowed model order

#define INT_BITS (7)
#define PERIOD_BITS (7)
#define TOT_BITS (INT_BITS+PERIOD_BITS)
#define INTERVAL (1 << INT_BITS)
#define BIN_SCALE (1 << TOT_BITS)
#define MAX_FREQ (124)

typedef struct sRANGE_CODER {
  uint low;
  uint code;
  uint range;
  struct SUBRANGE
  {
    uint LowCount;
    uint HighCount;
    uint Scale;
  } SubRange;
} tRANGE_CODER, tRangeCoder;

typedef struct sSEE2_CONTEXT {
  ushort  Summ;
  byte    Shift;
  byte    Count;
} tSEE2_CONTEXT;

#pragma pack(1)
typedef struct sSTATE {
  struct sPPM_CONTEXT *Successor;
  byte    Symbol;
  byte    Freq;
  DUMMY
} tSTATE;

typedef struct sPPM_CONTEXT
{
  struct sPPM_CONTEXT *Suffix;
  ushort NumStats;
  DUMMY
  union
  {
    struct
    {
      tSTATE *Stats;
      ushort SummFreq;
      DUMMY
    } U;
    tSTATE OneState;
  } U;
} tPPM_CONTEXT   ;
#pragma pack()

#define UNIT_SIZE (sizeof(tPPM_CONTEXT))
#define FIXED_UNIT_SIZE 12        //!!! warning ppm size !!

typedef struct sPPM {
  tSEE2_CONTEXT   SEE2Cont[25][16];
  tSEE2_CONTEXT   DummySEE2Cont;

  tPPM_CONTEXT   *MinContext;
  tPPM_CONTEXT   *MedContext;
  tPPM_CONTEXT   *MaxContext;

  tSTATE         *FoundState;

  int             NumMasked;
  int             InitEsc;
  int             OrderFall;
  int             MaxOrder;
  int             RunLength;
  int             InitRL;

  byte            CharMask[256];
  byte            NS2Indx[256];
  byte            NS2BSIndx[256];
  byte            HB2Flag[256];

  byte            EscCount;
  byte            PrevSuccess;
  byte            HiBitsFlag;

  ushort          BinSumm[128][64];               // binary SEE-contexts

  tSubAllocator   SubAlloc;
  tRangeCoder     Coder;
} tPPM   ;

// ----------------------------------------------------------------------------
// *** from rarhdr.h ***
// ----------------------------------------------------------------------------

#define  SIZEOF_MARKHEAD         7
#define  SIZEOF_OLDMHD           7
#define  SIZEOF_NEWMHD          13
#define  SIZEOF_OLDLHD          21
#define  SIZEOF_NEWLHD          32
#define  SIZEOF_SHORTBLOCKHEAD   7
#define  SIZEOF_LONGBLOCKHEAD   11
#define  SIZEOF_SUBBLOCKHEAD    14
#define  SIZEOF_COMMHEAD        13
#define  SIZEOF_PROTECTHEAD     26
#define  SIZEOF_AVHEAD          14
#define  SIZEOF_SIGNHEAD        15
#define  SIZEOF_UOHEAD          18
#define  SIZEOF_MACHEAD         22
#define  SIZEOF_EAHEAD          24
#define  SIZEOF_BEEAHEAD        24
#define  SIZEOF_STREAMHEAD      26

#define  PACK_VER               29
#define  PACK_CRYPT_VER         29
#define  UNP_VER                29
#define  CRYPT_VER              29
#define  AV_VER                 20
#define  PROTECT_VER            20

#define  MHD_VOLUME         0x0001
#define  MHD_COMMENT        0x0002
#define  MHD_LOCK           0x0004
#define  MHD_SOLID          0x0008
#define  MHD_PACK_COMMENT   0x0010
#define  MHD_NEWNUMBERING   0x0010
#define  MHD_AV             0x0020
#define  MHD_PROTECT        0x0040
#define  MHD_PASSWORD       0x0080
#define  MHD_FIRSTVOLUME    0x0100

#define  LHD_SPLIT_BEFORE   0x0001
#define  LHD_SPLIT_AFTER    0x0002
#define  LHD_PASSWORD       0x0004
#define  LHD_COMMENT        0x0008
#define  LHD_SOLID          0x0010

#define  LHD_WINDOWMASK     0x00e0
#define  LHD_WINDOW64       0x0000
#define  LHD_WINDOW128      0x0020
#define  LHD_WINDOW256      0x0040
#define  LHD_WINDOW512      0x0060
#define  LHD_WINDOW1024     0x0080
#define  LHD_WINDOW2048     0x00a0
#define  LHD_WINDOW4096     0x00c0
#define  LHD_DIRECTORY      0x00e0

#define  LHD_LARGE          0x0100
#define  LHD_UNICODE        0x0200
#define  LHD_SALT           0x0400
#define  LHD_VERSION        0x0800
#define  LHD_EXTTIME        0x1000
#define  LHD_EXTFLAGS       0x2000

#define  SKIP_IF_UNKNOWN    0x4000
#define  LONG_BLOCK         0x8000

#define  SALT_SIZE          8

enum HEADER_TYPE
{
  MARK_HEAD     = 0x72,
  MAIN_HEAD     = 0x73,
  FILE_HEAD     = 0x74,
  COMM_HEAD     = 0x75,
  AV_HEAD       = 0x76,
  SUB_HEAD      = 0x77,
  PROTECT_HEAD  = 0x78,
  SIGN_HEAD     = 0x79,
  NEWSUB_HEAD   = 0x7a,
  ENDARC_HEAD   = 0x7b
};

#define LOW_HEADER_TYPE     0x72
#define HIGH_HEADER_TYPE    0x7B

enum
{
  EA_HEAD     = 0x100,
  UO_HEAD     = 0x101,
  MAC_HEAD    = 0x102,
  BEEA_HEAD   = 0x103,
  NTACL_HEAD  = 0x104,
  STREAM_HEAD = 0x105
};

#define SUBHEAD_TYPE_CMT      "CMT"
#define SUBHEAD_TYPE_ACL      "ACL"
#define SUBHEAD_TYPE_STREAM   "STM"
#define SUBHEAD_TYPE_UOWNER   "UOW"
#define SUBHEAD_TYPE_AV       "AV"
#define SUBHEAD_TYPE_RR       "RR"
#define SUBHEAD_TYPE_OS2EA    "EA2"
#define SUBHEAD_TYPE_BEOSEA   "EABE"

#define SUBHEAD_FLAGS_INHERITED    0x80000000
#define SUBHEAD_FLAGS_CMT_UNICODE  0x00000001

#pragma pack(1)
typedef struct sBASE_HDR {
  tWORD  crc;         // crc - read from file
  tBYTE  type;        // type of header - read from file
  tWORD  flags;       // flags of header
  tWORD  size;        // size of header - still in file
  tUINT  hdr_size;
  tDWORD extra_size;
} tBASE_HDR;
#pragma pack()

// ----------------------------------------------------------------------------
// *** from rarvm.h ***
// ----------------------------------------------------------------------------

#define VM_STANDARDFILTERS
#define VM_OPTIMIZE
#define VM_VALUE_DEFINE
#define VM_PREPARE_OPERAND

#define VM_MEMSIZE                  0x40000
#define VM_MEMMASK           (VM_MEMSIZE-1)
#define VM_GLOBALMEMADDR            0x3C000
#define VM_GLOBALMEMSIZE             0x2000
#define VM_FIXEDGLOBALSIZE               64

//  enum eVM_COMMANDS
#define    VM_MOV      0x00
#define    VM_CMP      0x01
#define    VM_ADD      0x02
#define    VM_SUB      0x03
#define    VM_JZ       0x04
#define    VM_JNZ      0x05
#define    VM_INC      0x06
#define    VM_DEC      0x07
#define    VM_JMP      0x08
#define    VM_XOR      0x09
#define    VM_AND      0x0A
#define    VM_OR       0x0B
#define    VM_TEST     0x0C
#define    VM_JS       0x0D
#define    VM_JNS      0x0E
#define    VM_JB       0x0F
#define    VM_JBE      0x10
#define    VM_JA       0x11
#define    VM_JAE      0x12
#define    VM_PUSH     0x13
#define    VM_POP      0x14
#define    VM_CALL     0x15
#define    VM_RET      0x16
#define    VM_NOT      0x17
#define    VM_SHL      0x18
#define    VM_SHR      0x19
#define    VM_SAR      0x1A
#define    VM_NEG      0x1B
#define    VM_PUSHA    0x1C
#define    VM_POPA     0x1D
#define    VM_PUSHF    0x1E
#define    VM_POPF     0x1F
#define    VM_MOVZX    0x20
#define    VM_MOVSX    0x21
#define    VM_XCHG     0x22
#define    VM_MUL      0x23
#define    VM_DIV      0x24
#define    VM_ADC      0x25
#define    VM_SBB      0x26
#define    VM_PRINT    0x27

#ifdef VM_OPTIMIZE

#define    VM_MOVB     0x28
#define    VM_MOVD     0x29
#define    VM_CMPB     0x2A
#define    VM_CMPD     0x2B

#define    VM_ADDB     0x2C
#define    VM_ADDD     0x2D
#define    VM_SUBB     0x2E
#define    VM_SUBD     0x2F
#define    VM_INCB     0x30
#define    VM_INCD     0x31
#define    VM_DECB     0x32
#define    VM_DECD     0x33
#define    VM_NEGB     0x34
#define    VM_NEGD     0x35

#endif // VM_OPTIMIZE

#define VM_STANDARD 0x36

#define VMCF_OP0             0
#define VMCF_OP1             1
#define VMCF_OP2             2
#define VMCF_OPMASK          3
#define VMCF_byteMODE        4
#define VMCF_JUMP            8
#define VMCF_PROC           16
#define VMCF_USEFLAGS       32
#define VMCF_CHFLAGS        64

//enum eVM_FLAGS
#define    VM_FC 1
#define    VM_FZ 2
#define    VM_FS 0x80000000

//enum eVM_OPTYPE
#define    VM_OPMEMREG      0x00
#define    VM_OPREG         0x01
#define    VM_OPINT         0x02
#define    VM_OPMEMBASE     0x03
#define    VM_OPNONE        0x04

typedef struct sOPERAND {
  byte Type;      // eVM_OPTYPE
  uint Data;
  uint Base;
  #ifdef VM_PREPARE_OPERAND
  uint *Addr;
  #endif
} tOPERAND;

typedef struct sCOMMAND {
  byte      OpCode;     // eVM_COMMANDS
  rarbool   ByteMode;
  tOPERAND  Op1;
  tOPERAND  Op2;
} tCOMMAND;


typedef struct sFILTER {
  uint        ExecCount;
  tARRAY      Cmd;
  tARRAY      StaticData;
} tFILTER;

typedef struct sSTACK_FILTER {
  uint        BlockStart;
  uint        BlockLength;
  uint        ExecCount;
  uint        NextWindow;

  tCOMMAND    *Cmd;               // instead of array
  uint        CmdCount;

  tARRAY      GlobalData;         // always VM_FIXEDGLOBALSIZE
  tARRAY      StaticData;

  uint        InitR[7];

  byte        *FilteredData;
  uint        FilteredDataSize;
} tSTACK_FILTER;

typedef struct sVM {
  byte  *Mem;
  uint  R[8];  // registers
  uint  Flags;
} tVM;

// ----------------------------------------------------------------------------
// *** from stdflt.h ***
// ----------------------------------------------------------------------------

#ifdef VM_STANDARDFILTERS

//enum eVM_STD_FILTERS
#define    VMSF_NONE     0x00
#define    VMSF_E8       0x01
#define    VMSF_E8E9     0x02
#define    VMSF_ITANIUM  0x03
#define    VMSF_RGB      0x04
#define    VMSF_AUDIO    0x05
#define    VMSF_DELTA    0x06
#define    VMSF_UPCASE   0x07

#endif // VM_STANDARTFILTERS

// ----------------------------------------------------------------------------
// *** all compressor related shit ***
// ----------------------------------------------------------------------------

#define NC 298  // alphabet = { 0, 1, 2, ..., NC - 1 }
#define DC 48
#define RC 28
#define BC 19
#define MC 257

#pragma pack(1)

// compressed file header
typedef struct sNEW_FILE_HEADER {
  tWORD   HeadCRC;
  tBYTE   HeadType;
  tWORD   Flags;
  tWORD   HeadSize;
  tDWORD  PackSize;
  tDWORD  UnpSize;
  tBYTE   HostOS;
  tDWORD  FileCRC;
  tDWORD  FileTime;
  tBYTE   UnpVer;
  tBYTE   Method;
  tWORD   NameSize;
  tDWORD  FileAttr;
} tNEW_FILE_HEADER, *pNEW_FILE_HEADER;

#pragma pack()

typedef struct sBIT_LENGTHS {
  tBYTE  LitLength[NC];
  tBYTE  DistLength[DC];
  tBYTE  RepLength[RC];
} BitLengths, tBIT_LENGTHS;

// main packer context
typedef struct sPACK20 {
  tUINT   (*PackReadPtr)(struct sPACK20*, tBYTE*, tUINT);
  tVOID   (*PackWritePtr)(struct sPACK20*);
  tBYTE*  Window;
  tBYTE*  LastOc2;
  tUINT*  LastOcc;
  tUINT*  List;
  tBYTE*  CodeBuf;
  tBYTE*  CodeHBuf;
  tUINT   CodeSize;
  tUINT   CodePtr;
  tUINT   CurPtr;
  tUINT   SavePtr;
  tINT    UnpAllBuf;
  tINT    SomeRead;
  tUINT   LastReadSize;
  tLONG   MaxDist;
  tINT    MaxLength;
  tINT    CurPtrChanged;
  tINT    CmpLength;
  tINT    AllBuf;
  tUINT   CurHashValue;
  tUINT   NextReadAddr;
  tUINT   WinSize;
  tUINT   WinMask;
  tUINT   WinMaxDist;
  tINT    PackLevel;
  tINT    PackLevel2;
  tINT    SearchCount;
  tUINT   CurLastOcc;
  tBYTE   OutBuf[8192];
  tUINT   subbitbuf;
  tUINT   OutPtr;
  tINT    bitcount;
  tUINT   LastBlockSize;
  tUINT   PutBlockSym;
  tUINT   BlockSymCode;
  tUINT   BlockSymLength;
  tBYTE   PackOldTable[MC*4];
  tUINT   MaxPackRead;
  tUINT   CurPackRead;
  tDWORD  PackFileCRC;
  tINT    FilesTableSize;
  tINT    CurTableFile;
  tDWORD  PackSize;

  // for mtree.c
  tINT    Depth;
  tINT    FreqTableSize;
  tINT    TreeHeapSize;
  tINT    TreeHeap[NC+1];
  tUINT*  TreeFreq;
  tUINT*  SortPtr;
  tBYTE*  TreeLen;
  tUINT   LenCount[16];
  tUINT   Left[2*NC-1];
  tUINT   Right[2*NC-1];

  // for prague
  struct  sRAR* RAR;
  hIO     hInpIO;
  hIO     hOutIO;
  hSEQ_IO hSeqInp;
  hSEQ_IO hSeqOut;
} tPACK20, *pPACK20;

// ----------------------------------------------------------------------------
// *** from sha1.h ***
// ----------------------------------------------------------------------------

typedef struct sSHA1_CTX {
  tUINT  state[5];
  tUINT  count[2];
  tBYTE  buffer[64];
} tSHA1_CTX, *pSHA1_CTX;

// ----------------------------------------------------------------------------
// *** from rijndael.h ***
// ----------------------------------------------------------------------------

#define AES_MAX_KEY_COLUMNS (256/32)
#define AES_MAX_ROUNDS      14
#define AES_MAX_IV_SIZE     16

#define AES_ENCRYPT  (0)
#define AES_DECRYPT  (1)

typedef struct sAES_CTX {
  tINT   m_direction;
  tBYTE  m_initVector[AES_MAX_IV_SIZE];
  tBYTE  m_expandedKey[AES_MAX_ROUNDS+1][4][4];
  tBYTE  S[256], S5[256], rcon[30];
  tBYTE  T1[256][4], T2[256][4], T3[256][4], T4[256][4];
  tBYTE  T5[256][4], T6[256][4], T7[256][4], T8[256][4];
  tBYTE  U1[256][4], U2[256][4], U3[256][4], U4[256][4];
} tAES_CTX, *pAES_CTX;

// ----------------------------------------------------------------------------
// *** from crypt.h ***
// ----------------------------------------------------------------------------

enum { OLD_DECODE=0,OLD_ENCODE=1,NEW_CRYPT=2 };

#define MAXPASSWORD  (260)

typedef struct sCRYPT_KEY_CASHE_ITEM {
  tBYTE AESKey[16];
  tBYTE AESInit[16];
  tCHAR Password[MAXPASSWORD];
  tBOOL SaltPresent;
  tBYTE Salt[SALT_SIZE];
} tCRYPT_KEY_CASHE_ITEM, *pCRYPT_KEY_CASHE_ITEM;

struct tag_RAR_Data;

typedef struct sCRYPT_DATA {
  tUINT  Version;
  tAES_CTX rin;
  tBYTE  SubstTable[256];
  tUINT  Key[4];
  tWORD  OldKey[4];
  tBYTE  PN1,PN2,PN3;
  tBYTE  AESKey[16], AESInit[16];
  struct sCRYPT_KEY_CASHE_ITEM Cache[4];
  tINT   CachePos;
  hSTRING hPswd;
} tCRYPT_DATA, *pCRYPT_DATA;

// ----------------------------------------------------------------------------
// *** from rar.h ***
// ----------------------------------------------------------------------------

// method 29
#define NC29 299  // alphabet = {0, 1, 2, ..., NC - 1}
#define DC29  60
#define LDC29 17
#define RC29  28
#define HUFF_TABLE_SIZE29 (NC29+DC29+RC29+LDC29)
#define BC29  20

// method 20
#define NC20 298  // alphabet = {0, 1, 2, ..., NC - 1}
#define DC20 48
#define RC20 28
#define BC20 19
#define MC20 257

enum {
  CODE_HUFFMAN=0,CODE_LZ=1,CODE_LZ2=2,CODE_REPEATLZ=3,CODE_CACHELZ=4,
  CODE_STARTFILE=5,CODE_ENDFILE=6,CODE_VM=7,CODE_VMDATA=8
};

enum FilterType {
  FILTER_NONE, FILTER_PPM /*dummy*/, FILTER_E8, FILTER_E8E9,
  FILTER_UPCASETOLOW, FILTER_AUDIO, FILTER_RGB,  FILTER_DELTA,
  FILTER_ITANIUM
};

enum BLOCK_TYPES {BLOCK_LZ,BLOCK_PPM};

typedef struct sDECODE {
  uint MaxNum;
  uint DecodeLen[16];
  uint DecodePos[16];
  uint DecodeNum[2];
} tDECODE, Decode;

typedef struct sLIT_DECODE {
  uint MaxNum;
  uint DecodeLen[16];
  uint DecodePos[16];
  uint DecodeNum[NC29];
} tLIT_DECODE, LitDecode;

typedef struct sDIST_DECODE {
  uint MaxNum;
  uint DecodeLen[16];
  uint DecodePos[16];
  uint DecodeNum[DC29];
} tDIST_DECODE, DistDecode;

typedef struct sLOW_DIST_DECODE {
  uint MaxNum;
  uint DecodeLen[16];
  uint DecodePos[16];
  uint DecodeNum[LDC29];
} tLOW_DIST_DECODE, LowDistDecode;

typedef struct sREP_DECODE {
  uint MaxNum;
  uint DecodeLen[16];
  uint DecodePos[16];
  uint DecodeNum[RC29];
} tREP_DECODE, RepDecode;

typedef struct sBIT_DECODE {
  uint MaxNum;
  uint DecodeLen[16];
  uint DecodePos[16];
  uint DecodeNum[BC29];
} tBIT_DECODE, BitDecode;

typedef struct sMULT_DECODE {
  uint MaxNum;
  uint DecodeLen[16];
  uint DecodePos[16];
  uint DecodeNum[MC20];
} tMULT_DECODE, MultDecode;

typedef struct sAUDIO_VARIABLES {
  int K1,K2,K3,K4,K5;
  int D1,D2,D3,D4;
  int LastDelta;
  unsigned int Dif[11];
  unsigned int ByteCount;
  int LastChar;
} tAUDIO_VARIABLES, AudioVariables;

typedef struct sRAR_FILE_HEADER {
  tBASE_HDR base;
  filesize  psize;       // qword
  filesize  usize;       // qword
  byte      host_os;
  dword     file_crc;
  dword     file_time;
  byte      unp_ver;
  byte      method;
  byte      base_type;
  word      name_size;
  dword     file_attr;
  byte      name[MAX_NAME+4];
  tBYTE     salt[SALT_SIZE];
} tRAR_FILE_HEADER;

typedef struct sRAR {
  tBOOL   Error;          // some error
  tBOOL   InvalidCrc;     // invalid file checksum
  tINPUT  infile;         // file input structure
  dword   wr_crc32;       // unpacked file crc
  tQWORD  TotPSize;       // total packed size (for vols)
  
  tMEMENTRY   MemList[MEMENTRIES];
  uint    MemEntryCount;
  byte    FullName[256];
  
  // archive related data
  int     skip;
  int     last_file;
  
  word    MHD_Flags;      // flags from current main file header
  
  filesize NextBlock;
  
  tRAR_FILE_HEADER CurrentFile;   // header of current file
  
  rarbool    Volume;
  rarbool    Save;                   // save context
  
  rarbool    FileExtracted;
  rarbool    Suspended;
  rarbool    UnpSomeRead;
  rarbool    UnpAllBuf;
  
  // Virtual Machine begin
  tMEMPAGE   MemPage;
  
  uint    LastFilter;
  tARRAY  Filters;            // tFILTER*
  tARRAY  PrgStack;           // tSTACK_FILTER*
  tARRAY  OldFilterLengths;   // uint
  tVM     VM;
  // Virtual Machine end
  
  filesize WrittenFileSize;
  filesize DestUnpSize20;
  filesize DestUnpSize;
  
  rarbool TablesRead;
  struct  sLIT_DECODE  LD;
  struct  sDIST_DECODE DD;
  struct  sLOW_DIST_DECODE LDD;
  struct  sREP_DECODE  RD;
  struct  sBIT_DECODE  BD;
  
  byte    UnpOldTable[HUFF_TABLE_SIZE29];
  
  uint    OldDist[4];
  uint    OldDistPtr;
  uint    LastDist;
  uint    LastLength;
  
  int     PrevLowDist;
  int     LowDistRepCount;
  
  uint    WrPtr;
  uint    UnpPtr;
  int     UnpBlockType;
  
  byte    *Window;
  uint    WindowSize;
  uint    WindowMask;
  
  // ---- RAR 2.0 ----
  struct  sMULT_DECODE MD[4];
  byte    UnpOldTable20[MC20*4];
  int     UnpAudioBlock;
  int     UnpChannels;
  int     UnpCurChannel;
  int     UnpChannelDelta;
  struct  sAUDIO_VARIABLES AudV[4];
  
  tPPM    PPM;
  int     PPMEscChar;
  
  // ---- RAR 1.5 ----
  tUINT   ChSet[256], ChSetA[256], ChSetB[256], ChSetC[256];
  tUINT   Place[256], PlaceA[256], PlaceB[256], PlaceC[256];
  tUINT   NToPl[256], NToPlB[256], NToPlC[256];
  tUINT   FlagBuf, AvrPlc, AvrPlcB, AvrLn1, AvrLn2, AvrLn3;
  tUINT   Buf60, NumHuf, StMode, LCount, FlagsCnt;
  tUINT   Nhfb, Nlzb, MaxDist3;
  tINT    ReadTop;
  
  // addon for prague
  tBYTE*  RD_Buff;    // buffer for input
  hOBJECT hThis;      // for rotate()
  hHEAP   hHeap;      // working heap
  hSEQ_IO hSeqIn;     // input seq_io
  hSEQ_IO hSeqOut;    // output seq_io
  tUINT   CurrFNode;  // current file node
  tUINT   CurrPNode;  // current part node (file-thunk)
  tERROR  LastError;  // last error container
  tCRYPT_DATA Crypt;  // decryptor context
  tCHAR   Password[MAXPASSWORD]; // password string
  tBOOL   Decrypt;
#ifdef DEBUG_PASS
  tBOOL   PswSwap;    // password swapper
  tUINT   PswCnt;     // password counter
#endif
} tRAR;

#pragma pack()

// ----------------------------------------------------------------------------
#if defined (_WIN32)
#include <poppack.h>
#endif
// ----------------------------------------------------------------------------

#endif // _CONST_H_

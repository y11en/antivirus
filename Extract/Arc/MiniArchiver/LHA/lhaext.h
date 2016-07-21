#ifndef _LHA_EXT_H_
#define _LHA_EXT_H_  "LHA Extractor base definitions"

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>

#include <Extract/iface/i_uacall.h>

#include <memory.h>
/* ------------------------------------------------------------------------- */

#define CSTATIC  static
#define CATTRIB  __fastcall
#define CINLINE  __inline

typedef unsigned char   kav_uint8;
typedef unsigned short  kav_uint16;
typedef unsigned int    kav_uint32;
typedef signed   char   kav_sint8;
typedef signed   short  kav_sint16;
typedef signed   int    kav_sint32;
typedef unsigned int    kav_uint;
typedef signed   int    kav_sint;
typedef int             kav_int;

#define FILENAME_LENGTH  (1024)

#ifndef ARCHIVENAME_EXTENTION
# define ARCHIVENAME_EXTENTION  ".lzh"
#endif
#ifndef BACKUPNAME_EXTENTION
# define BACKUPNAME_EXTENTION   ".bak"
#endif

#define LZHUFF0_METHOD          "-lh0-"
#define LZHUFF1_METHOD          "-lh1-"
#define LZHUFF2_METHOD          "-lh2-"
#define LZHUFF3_METHOD          "-lh3-"
#define LZHUFF4_METHOD          "-lh4-"
#define LZHUFF5_METHOD          "-lh5-"
#define LZHUFF6_METHOD          "-lh6-"
#define LZHUFF7_METHOD          "-lh7-"
#define LARC_METHOD             "-lzs-"
#define LARC5_METHOD            "-lz5-"
#define LARC4_METHOD            "-lz4-"
#define LZHDIRS_METHOD          "-lhd-"

#define METHOD_TYPE_STRAGE      (5)

#define LZHUFF0_METHOD_NUM    0
#define LZHUFF1_METHOD_NUM    1
#define LZHUFF2_METHOD_NUM    2
#define LZHUFF3_METHOD_NUM    3
#define LZHUFF4_METHOD_NUM    4
#define LZHUFF5_METHOD_NUM    5
#define LZHUFF6_METHOD_NUM    6
#define LZHUFF7_METHOD_NUM    7
#define LARC_METHOD_NUM       8
#define LARC5_METHOD_NUM      9
#define LARC4_METHOD_NUM      10
#define LZHDIRS_METHOD_NUM    11

#define I_HEADER_SIZE         0
#define I_HEADER_CHECKSUM     1
#define I_METHOD              2
#define I_PACKED_SIZE         7
#define I_ORIGINAL_SIZE       11
#define I_LAST_MODIFIED_STAMP 15
#define I_ATTRIBUTE           19
#define I_HEADER_LEVEL        20
#define I_NAME_LENGTH         21
#define I_NAME                22

#define I_CRC                       22  /* + name_length */
#define I_EXTEND_TYPE               24  /* + name_length */
#define I_MINOR_VERSION             25  /* + name_length */
#define I_UNIX_LAST_MODIFIED_STAMP  26  /* + name_length */
#define I_UNIX_MODE                 30  /* + name_length */
#define I_UNIX_UID                  32  /* + name_length */
#define I_UNIX_GID                  34  /* + name_length */
#define I_UNIX_EXTEND_BOTTOM        36  /* + name_length */

#define I_GENERIC_HEADER_BOTTOM   I_EXTEND_TYPE

#define EXTEND_GENERIC    0
#define EXTEND_UNIX      'U'
#define EXTEND_MSDOS     'M'
#define EXTEND_MACOS     'm'
#define EXTEND_OS9       '9'
#define EXTEND_OS2       '2'
#define EXTEND_OS68K     'K'
#define EXTEND_OS386     '3'  /* OS-9000??? */
#define EXTEND_HUMAN     'H'
#define EXTEND_CPM       'C'
#define EXTEND_FLEX      'F'
#define EXTEND_RUNSER    'R'

/* this OS type is not official */
#define EXTEND_TOWNSOS   'T'
#define EXTEND_XOSK      'X'

#define GENERIC_ATTRIBUTE             0x20
#define GENERIC_DIRECTORY_ATTRIBUTE   0x10
#define HEADER_LEVEL0                 0x00
#define HEADER_LEVEL1                 0x01
#define HEADER_LEVEL2                 0x02

#define CURRENT_UNIX_MINOR_VERSION    0x00

#define DELIM     ('/')
#define DELIM2    (0xff)
#define DELIMSTR   "/"

#define OSK_RW_RW_RW              0000033
#define OSK_FILE_REGULAR          0000000
#define OSK_DIRECTORY_PERM        0000200
#define OSK_SHARED_PERM           0000100
#define OSK_OTHER_EXEC_PERM       0000040
#define OSK_OTHER_WRITE_PERM      0000020
#define OSK_OTHER_READ_PERM       0000010
#define OSK_OWNER_EXEC_PERM       0000004
#define OSK_OWNER_WRITE_PERM      0000002
#define OSK_OWNER_READ_PERM       0000001

#define UNIX_FILE_TYPEMASK        0170000
#define UNIX_FILE_REGULAR         0100000
#define UNIX_FILE_DIRECTORY       0040000
#define UNIX_FILE_SYMLINK         0120000
#define UNIX_SETUID               0004000
#define UNIX_SETGID               0002000
#define UNIX_STYCKYBIT            0001000
#define UNIX_OWNER_READ_PERM      0000400
#define UNIX_OWNER_WRITE_PERM     0000200
#define UNIX_OWNER_EXEC_PERM      0000100
#define UNIX_GROUP_READ_PERM      0000040
#define UNIX_GROUP_WRITE_PERM     0000020
#define UNIX_GROUP_EXEC_PERM      0000010
#define UNIX_OTHER_READ_PERM      0000004
#define UNIX_OTHER_WRITE_PERM     0000002
#define UNIX_OTHER_EXEC_PERM      0000001
#define UNIX_RW_RW_RW             0000666

#define LZHEADER_STRAGE       4096

#ifndef CHAR_BIT
# define CHAR_BIT  8
#endif

/* from crcio.c */
#define CRCPOLY       0xA001  /* CRC-16 */
#define UPDATE_CRC(c) crc = crctable[(crc ^ (c)) & 0xFF] ^ (crc >> CHAR_BIT)

/* dhuf.c */
#define N_CHAR      (256 + 60 - THRESHOLD + 1)
#define TREESIZE_C  (N_CHAR * 2)
#define TREESIZE_P  (128 * 2)
#define TREESIZE    (TREESIZE_C + TREESIZE_P)
#define ROOT_C      0
#define ROOT_P      TREESIZE_C

/* header.c */
#define setup_get(mptr) (get_ptr = (mptr))
#define get_byte()      (*get_ptr++ & 0xff)
#define put_ptr          get_ptr
#define setup_put(mptr) (put_ptr = (mptr))
#define put_byte(c)     (*put_ptr++ = (tBYTE)(c))

/* huf.c */
#define NP      (MAX_DICBIT + 1)
#define NT      (USHRT_BIT + 3)

#define PBIT    5   /* smallest integer such that (1 << PBIT) > * NP */
#define TBIT    5   /* smallest integer such that (1 << TBIT) > * NT */

#ifndef UCHAR_MAX
# define UCHAR_MAX 0xff
#endif

#define NC      (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)

#define NPT     0x80

/* larc.c */
#define MAGIC0    18
#define MAGIC5    19

/* shuf.c */
#define N1       286          /* alphabet size               */
#define N2      (2 * N1 - 1)  /* # of nodes in Huffman tree  */
#define EXTRABITS 8           /* >= log2(F-THRESHOLD+258-N1) */
#define BUFBITS   16          /* >= log2(MAXBUF)             */
#define LENFIELD  4           /* bit size of length field for tree output */

/* util.c */
#define BUFFERSIZE  2048
#define MAXSFXCODE  1024*64

#define LZHUFF0_DICBIT           0      /* no compress */
#define LZHUFF1_DICBIT          12      /* 2^12 =  4KB sliding dictionary */
#define LZHUFF2_DICBIT          13      /* 2^13 =  8KB sliding dictionary */
#define LZHUFF3_DICBIT          13      /* 2^13 =  8KB sliding dictionary */
#define LZHUFF4_DICBIT          12      /* 2^12 =  4KB sliding dictionary */
#define LZHUFF5_DICBIT          13      /* 2^13 =  8KB sliding dictionary */
#define LZHUFF6_DICBIT          15      /* 2^15 = 32KB sliding dictionary */
#define LZHUFF7_DICBIT          16      /* 2^16 = 64KB sliding dictionary */
#define LARC_DICBIT             11      /* 2^11 =  2KB sliding dictionary */
#define LARC5_DICBIT            12      /* 2^12 =  4KB sliding dictionary */
#define LARC4_DICBIT             0      /* no compress */

/* slide.c */
#define MAX_DICBIT     16      /* lh7 use 16bits */

#define MAX_DICSIZ   (1 << MAX_DICBIT)
#define MATCHBIT      8    /* bits for MAXMATCH - THRESHOLD            */
#define MAXMATCH      256  /* formerly F (not more than UCHAR_MAX + 1) */
#define THRESHOLD     3    /* choose optimal value                     */

/* alphabet = {0, 1, 2, ..., NC - 1} */
#define CBIT          9   /* $\lfloor \log_2 NC \rfloor + 1$ */
#define USHRT_BIT    (CHAR_BIT * sizeof(tWORD))

#define RD_BUFLEN  2048
#define WR_BUFLEN  2048
#define TM_BUFLEN  (LZHEADER_STRAGE + 2*MAX_LHA_NAME)

#define SUPPORT_LH7

#ifdef SUPPORT_LH7
# define DICSIZ (1L << 16)
# define TXTSIZ (DICSIZ * 2L + MAXMATCH)
#else
# define DICSIZ (((unsigned long)1) << 15)
# define TXTSIZ (DICSIZ * 2 + MAXMATCH)
#endif

#define HSHSIZ (((unsigned long)1) <<15)
#define NIL 0
#define LIMIT 0x100 /* chain limit */

/* stream reader context */
typedef struct sIO_CTX {
  tQWORD  FPos;        /* file position of begin of read buffer    */
  hSEQ_IO hSeqIn;        /* input sequential reader                */
  tBYTE*  Buff;          /* read data buffer                       */
  tDWORD  Rdt;           /* bytes in read buffer                   */
  tDWORD  Limit;         /* max bytes for read (buffer size)       */
  tDWORD  BPos;          /* current position in read buffer        */
  tBOOL   REOF;          /* end-of-file flag                       */
  tBOOL   NoRdt;         /* don't read new blocks                  */
  hSEQ_IO hSeqOut;       /* output sequential reader               */
  tBYTE*  WBuf;          /* output buffer                          */
  tDWORD  WBufLen;       /* size of write buffer                   */
  tDWORD  WBufPos;       /* current position in write buffer       */
  struct  sLHA_CTX* Lha; /* parent object                          */
} tIO_CTX, *pIO_CTX;

#define MAX_LHA_NAME  260

typedef struct sINTERFACING {
  tIO_CTX*    infile;
  kav_uint32  original;
  kav_uint32  packed;
  kav_uint32  crc;
  kav_int     dicbit;
  kav_int     method;
} tINTERFACING, *pINTERFACING;

typedef struct sDECODE_OPTION {
  kav_uint16 (CATTRIB *decode_c)(struct sLHA_CTX* Ctx);
  kav_uint16 (CATTRIB *decode_p)(struct sLHA_CTX* Ctx);
  void       (CATTRIB *decode_start)(struct sLHA_CTX* Ctx);
} tDECODE_OPTION, *pDECODE_OPTION;

typedef struct sENCODE_OPTION {
  void  (CATTRIB *output)(struct sLHA_CTX* Ctx, kav_uint32 c, kav_uint32 p);
  void  (CATTRIB *encode_start)(struct sLHA_CTX* Ctx);
  void  (CATTRIB *encode_end)(struct sLHA_CTX* Ctx);
} tENCODE_OPTION, pENCODE_OPTION;

/* packed data header */
typedef struct sLZ_HEADER {
  kav_int     crc_fail;
  kav_int     hdr_parse_fail;
  kav_uint32  packed_size;
  kav_uint32  original_size;
  kav_uint32  last_modified_stamp;
  kav_uint16  crc;
  kav_uint16  hdr_ext_len;
  kav_uint8   header_size;
  kav_uint8   attribute;
  kav_uint8   name_length;
  kav_uint8   header_level;
  kav_uint8   has_crc;
  kav_uint8   extend_type;
  kav_uint8   minor_version;
  kav_uint8   hdr_crc_len;
  kav_uint8   name[MAX_LHA_NAME];
  kav_uint8   method[METHOD_TYPE_STRAGE];

  /* extend_type == EXTEND_UNIX and convert from other type */
  kav_uint32  unix_last_modified_stamp;
  kav_uint16  unix_mode;
  kav_uint16  unix_uid;
  kav_uint16  unix_gid;
} tLZ_HEADER, *pLZ_HEADER;

/* global variables */
typedef struct sGLOBALS {
  kav_uint8*  text;
  kav_uint32  origsize;
  kav_uint32  compsize;
  kav_uint32  count;
  kav_uint32  loc;
  kav_sint32  prev_char;
  kav_sint32  dispflg;
  kav_int     unpackable;
  kav_uint32  reading_size;
  kav_uint32  n_max;
  kav_uint32  np;                /* from shuf.c */
  kav_uint16  dicbit;
  kav_uint16  maxmatch;
  kav_uint16  crc;
  kav_uint16  bitbuf;
  kav_uint8   subbitbuf;
  /* pack */
  kav_uint*   hash;
  kav_uint*   prev;
  kav_uint8*  too_flag;
  int         method;
  kav_uint    txtsiz;
  kav_uint32  bufsiz;
  kav_uint8*  buf;
  kav_uint16  output_mask;
  kav_uint16  output_pos;
  kav_uint16  cpos;
  kav_int     remainder;
  kav_int     matchlen;
  kav_uint32  encoded_origsize;
  kav_uint    pos;
  kav_uint    hval;
  /* pack end */
  kav_uint8   bitcount;
  kav_uint8   c_len[NC];
  kav_uint8   pt_len[NPT];
  kav_uint16  left[2*NC-1];      /* from huf.c  */
  kav_uint16  right[2*NC-1];
  kav_uint16  c_freq[2*NC-1];
  kav_uint16  c_table[4096];
  kav_uint16  c_code[NC];
  kav_uint16  p_freq[2*NP-1];
  kav_uint16  pt_table[256];
  kav_uint16  pt_code[NPT];
  kav_uint16  t_freq[2*NT-1];
  kav_sint16  child[TREESIZE];   /* from dhuf.c */
  kav_sint16  parent[TREESIZE];
  kav_sint16  block[TREESIZE];
  kav_sint16  edge[TREESIZE];
  kav_sint16  stock[TREESIZE];
  kav_sint16  s_node[TREESIZE/2];
  kav_uint16  freq[TREESIZE];

  /* make tree */
  kav_sint16  n;
  kav_sint16  heapsize;
  kav_sint16  heap[NC + 1];
  kav_uint16* tfreq;
  kav_uint16* sort;
  kav_uint8*  len;
  kav_uint16  len_cnt[17];
  kav_uint8   depth;
} tGLOBALS, *pGLOBALS;

/* from slide.c */
typedef struct sSLIDE {
  kav_uint32  dicsiz;
} tSLIDE, *pSLIDE;

/* from shuf.c */
typedef struct sSHUF {
  kav_uint32  np;
  kav_sint32  pbit;
  kav_sint32  matchpos;  /* from lzs */
  kav_sint32  flag;
  kav_sint32  flagcnt;
  kav_uint16  blocksize;
} tSHUF, *pSHUF;

/* from dhuf.c */
typedef struct sDHUF {
  kav_sint32  avail;
  kav_sint32  n1;
  kav_sint32  most_p;
  kav_sint32  nn;
  kav_uint32  nextcount;
  kav_uint16  total_p;
} tDHUF, *pDHUF;

struct sLHA_ENCODER;

/* main unpacker context */
typedef struct sLHA_CTX {
  tBYTE*  Window;
  tDWORD  WndSize;
  tERROR  Error;
  hOBJECT hParent;
  hSEQ_IO hWriter;
  hHEAP   hHeap;
  struct  sGLOBALS    Glob;
  struct  sSLIDE      Slide;
  struct  sDHUF       DHuf;
  struct  sSHUF       SHuf;
  struct  sIO_CTX     IO;
  struct  sLZ_HEADER  LzHdr;
  struct  sENCODE_OPTION encode_set;
  tBYTE   TmBuf[TM_BUFLEN];
  tBYTE   RdBuf[RD_BUFLEN];
  tBYTE   WrBuf[WR_BUFLEN];
  struct  sLHA_ENCODER* Encoder;
} tLHA_CTX, *pLHA_CTX;

/* one node in file array */
typedef struct sFILE_NODE {
  tQWORD HdrOfs;             /* raw header offset    */
  tDWORD HdrSize;            /* total size of header */
  tBOOL  ReadOnly;           /* readonly flag        */
  tBOOL  IsDirectory;        /* file is directory    */
  tLZ_HEADER LzHdr;          /* file entry header    */
} tFILE_NODE, *pFILE_NODE;

/* ------------------------------------------------------------------------- */

#define Min(x,y) (((x)<(y)) ? (x):(y))
#define Max(x,y) (((x)>(y)) ? (x):(y))

/* external functions */
tQWORD LHA_Tell(tIO_CTX* IoCtx);
tERROR LHA_Rotate(tLHA_CTX* Ctx);
tDWORD LHA_ReadBlock(tIO_CTX* IoCtx, tVOID* Buff, tUINT Count);
tERROR LHA_InitFile(tIO_CTX* Ctx, hSEQ_IO hSeqIn, hSEQ_IO hSeqOut);
tQWORD LHA_Seek(tIO_CTX* IoCtx, tQWORD Pos);
tERROR LHA_GetHeader(tIO_CTX* IO, tLZ_HEADER* hdr);
tERROR LHA_ExtractOne(tLHA_CTX* Ctx, tLZ_HEADER* hdr);
tERROR CopySeqIOBlock(tLHA_CTX* Ctx, hSEQ_IO hSrc, hSEQ_IO hDst,
                      tQWORD SrcOfs, tQWORD DstOfs, tDWORD BlkSize);
tERROR LHA_CompressOne(tLHA_CTX* Ctx, tFILE_NODE* FNode, hSEQ_IO hSeqSrc,
                       hSEQ_IO hSeqIn, hSEQ_IO hSeqOut, tQWORD OutOfs,
                       tDWORD* Written);

/* ------------------------------------------------------------------------- */

#endif /* _LHA_EXT_H_ */

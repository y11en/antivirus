/*
#define  Min(x,y) (((x)<(y)) ? (x):(y))
#define  Max(x,y) (((x)>(y)) ? (x):(y))
*/
#define RAR_SIG ('R' | ('a' << 8) | ('r' << 16) | ('!' << 24))
/*
#define GetHeaderByte(N) Header[N]

#define GetHeaderWord(N) (Header[N]+((WORD)Header[N+1]<<8))

#define GetHeaderDword(N) (Header[N]+((WORD)Header[N+1]<<8)+\
	((DWORD)Header[N+2]<<16)+\
((DWORD)Header[N+3]<<24))


#define  NM  260

#define  SIZEOF_MARKHEAD         7
#define  SIZEOF_OLDMHD           7
#define  SIZEOF_NEWMHD          13
#define  SIZEOF_OLDLHD          21
#define  SIZEOF_NEWLHD          32
#define  SIZEOF_SHORTBLOCKHEAD   7
#define  SIZEOF_LONGBLOCKHEAD   11
#define  SIZEOF_COMMHEAD        13
#define  SIZEOF_PROTECTHEAD     26
#define  SIZEOF_AVHEAD          14
#define  SIZEOF_UOHEAD          18


#define  PACK_VER       20
#define  UNP_VER        20
#define  AV_VER         20
#define  PROTECT_VER    20


enum { M_DENYREAD,M_DENYWRITE,M_DENYNONE,M_DENYALL };

enum { FILE_EMPTY,FILE_ADD,FILE_UPDATE,FILE_COPYOLD,FILE_COPYBLOCK };

enum { ARG_RESET,ARG_NEXT,ARG_REMOVE };

enum { RECURSE_ALWAYS=1,RECURSE_WILDCARDS };

enum { ARC=1,VOL,SFX };

enum { SUCCESS,WARNING,FATAL_ERROR,CRC_ERROR,LOCK_ERROR,WRITE_ERROR,
       OPEN_ERROR,USER_ERROR,MEMORY_ERROR,USER_BREAK=255,IMM_ABORT=0x8000 };

enum { EN_LOCK=1,EN_VOL=2 };

enum { SD_MEMORY=1,SD_FILES=2 };

enum { ASK_OVERWR=0,ALL_OVERWR=1,NO_OVERWR=2 };

enum { ALARM_SOUND,ERROR_SOUND };

enum { MSG_STDOUT,MSG_STDERR,MSG_NULL };

enum { COMPARE_PATH,NOT_COMPARE_PATH };

enum { NAMES_DONTCHANGE,NAMES_UPPERCASE,NAMES_LOWERCASE };

enum { LOG_ARC=1,LOG_FILE=2 };

enum { CRC16=1,CRC32=2 };

enum { OLD_DECODE=0,OLD_ENCODE=1,NEW_CRYPT=2 };

enum { OLD_UNPACK,NEW_UNPACK };

enum { KEEP_TIME=1,LATEST_TIME };

enum {SOLID_NONE=0,SOLID_NORMAL,SOLID_COUNT,SOLID_FILEEXT};

#define  MHD_MULT_VOL       1
#define  MHD_COMMENT        2
#define  MHD_LOCK           4
#define  MHD_SOLID          8
#define  MHD_PACK_COMMENT   16
#define  MHD_AV             32
#define  MHD_PROTECT        64

#define  LHD_SPLIT_BEFORE   1
#define  LHD_SPLIT_AFTER    2
#define  LHD_PASSWORD       4
#define  LHD_COMMENT        8
#define  LHD_SOLID          16

#define  LHD_WINDOWMASK     0x00e0
#define  LHD_WINDOW64       0
#define  LHD_WINDOW128      32
#define  LHD_WINDOW256      64
#define  LHD_WINDOW512      96
#define  LHD_WINDOW1024     128
#define  LHD_DIRECTORY      0x00e0

#define  SKIP_IF_UNKNOWN    0x4000
#define  LONG_BLOCK         0x8000

#define READSUBBLOCK  0x8000

enum { ALL_HEAD=0,MARK_HEAD=0x72,MAIN_HEAD=0x73,FILE_HEAD=0x74,
       COMM_HEAD=0x75,AV_HEAD=0x76,SUB_HEAD=0x77,PROTECT_HEAD=0x78};

enum { EA_HEAD=0x100,UO_HEAD=0x101 };

enum { MS_DOS=0,OS2=1,WIN_32=2,UNIX=3 };

enum { OLD=1,NEW=2};

enum { EEMPTY=-1,EBREAK=1,EWRITE,EREAD,EOPEN,ECREAT,ECLOSE,ESEEK,
       EMEMORY,EARCH };


typedef struct _tag_MarkHeader
{
  tBYTE Mark[7];
}MarkHeader;


typedef struct _tag_OldMainHeader
{
  tBYTE Mark[4];
  tWORD HeadSize;
  tBYTE Flags;
}OldMainHeader;


typedef struct _tag_NewMainArchiveHeader
{
  tWORD HeadCRC;
  tBYTE HeadType;
  tWORD Flags;
  tWORD HeadSize;
  tWORD Reserved;
  tDWORD Reserved1;
}NewMainArchiveHeader;


typedef struct _tag_OldFileHeader
{
  tDWORD PackSize;
  tDWORD UnpSize;
  tWORD FileCRC;
  tWORD HeadSize;
  tDWORD FileTime;
  tBYTE FileAttr;
  tBYTE Flags;
  tBYTE UnpVer;
  tBYTE NameSize;
  tBYTE Method;
}OldFileHeader;


typedef struct _tag_NewFileHeader
{
  tWORD HeadCRC;
  tBYTE HeadType;
  tWORD Flags;
  tWORD HeadSize;
  tDWORD PackSize;
  tDWORD UnpSize;
  tBYTE HostOS;
  tDWORD FileCRC;
  tDWORD FileTime;
  tBYTE UnpVer;
  tBYTE Method;
  tWORD NameSize;
  tDWORD FileAttr;
}NewFileHeader;


typedef struct _tag_BlockHeader
{
  tWORD HeadCRC;
  tBYTE HeadType;
  tWORD Flags;
  tWORD HeadSize;
  tDWORD DataSize;
}BlockHeader;


typedef struct _tag_SubBlockHeader
{
  tWORD HeadCRC;
  tBYTE HeadType;
  tWORD Flags;
  tWORD HeadSize;
  tDWORD DataSize;
  tWORD SubType;
  tBYTE Level;
}SubBlockHeader;


typedef struct _tag_RAROptions
{
  tDWORD ExclFileAttr;
  tDWORD WinSize;
  tCHAR TmpPath[NM];
  tCHAR NameSFXCode[NM];
  tCHAR ExtrPath[NM];
  tCHAR CommentFile[NM];
  tCHAR Password[128];
  tINT Overwrite;
  tINT Method;
  tINT RecSectors;
  tINT Sound;
  tINT Log;
  tINT MsgStream;
  tINT MakeSolid;
  tINT MakeSolidCount;
  tINT ClearArchive;
  tINT AddArchiveOnly;
  tINT AVMode;
  tINT MMCompress;
  tINT DisableComment;
  tINT FreshFiles;
  tINT UpdateFiles;
  tINT ExclPath;
  tINT CreateSFX;
  tINT Recurse;
  tINT PackVolume;
  tINT AutoSize;
  tINT AllYes;
  tINT DisableViewAV;
  tINT DisableSortSolid;
  tINT ArcTime;
  tINT ConvertNames;
  tINT ProcessOwners;
  tINT SaveLinks;
  tINT Priority;
  tINT SleepTime;
  tINT KeepBroken;
  tINT DosMode;
}RAROptions;


typedef struct _tag_FileStat
{
  tDWORD FileAttr;
  tDWORD FileTime;
  tDWORD FileSize;
  tINT IsDir;
}FileStat;


typedef struct _tag_FileData
{
  tCHAR FileName[NM];
  tCHAR RealFileName[NM];
  NewFileHeader NewLhd;
  tDWORD UnpSize;
  tDWORD FileCommentPos;
  tINT NewFile;
}FileData;


typedef struct _tag_AddFileInfo
{
  tLONG OffPath;
  tINT SpecPathLen;
  tDWORD FileAttr;
  tDWORD FileTime;
  tWORD Include;
  tINT SortPos;
  tLONG Order;
  tLONG ArcPos;
}AddFileInfo;

typedef struct _tag_CommentHeader
{
	tWORD HeadCRC;
	tBYTE HeadType;
	tWORD Flags;
	tWORD HeadSize;
	tWORD UnpSize;
	tBYTE UnpVer;
	tBYTE Method;
	tWORD CommCRC;
}CommentHeader;


typedef struct _tag_ProtectHeader
{
	tWORD HeadCRC;
	tBYTE HeadType;
	tWORD Flags;
	tWORD HeadSize;
	tDWORD DataSize;
	tBYTE Version;
	tWORD RecSectors;
	tDWORD TotalBlocks;
	tBYTE Mark[8];
}ProtectHeader;
*/
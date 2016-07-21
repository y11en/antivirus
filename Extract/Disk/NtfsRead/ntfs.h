#include <windows.h>

#include <PshPack1.h>

#if defined(USE_LL_DISK_IO)
#include "llio.h"
#endif

#ifndef countof
#define countof(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

typedef LONGLONG MFT_REF;
typedef LONGLONG VCN;
typedef LONGLONG LCN;

#define MFT_REC_NUMBER(mft_ref) ((mft_ref) & 0x0000FFFFFFFFFFFFLL)  // strip sequence number
#define INVALID_MFT_REF ((MFT_REF)(-1LL))

#define LARGE_INDEX 1
#define INDEX_NODE  1

#define INDEX_ENTRY_NODE 1
#define INDEX_ENTRY_END  2

// forward declarations
typedef struct NTFS_FILE NTFS_FILE; 

typedef enum {
	MFT_RECORD_IN_USE        = 0x0001,
	MFT_RECORD_IS_DIRECTORY  = 0x0002,
	MFT_RECORD_IS_4          = 0x0004,
	MFT_RECORD_IS_VIEW_INDEX = 0x0008,
	MFT_REC_SPACE_FILLER     = 0xffff, // Just to make flags 16-bit
} MFT_RECORD_FLAGS;

typedef enum
{
	AttribStandartInfo = 0x10,
	AttribAttribList = 0x20,
	AttribFileName = 0x30,
	AttribObjectId = 0x40,
	AttribSecurityDescriptor = 0x50,
	AttribVolumeName = 0x60,
	AttribVolumeInfo = 0x70,
	AttribData = 0x80,
	AttribIndexRoot = 0x90,
	AttribIndexAllocation = 0xA0,
	AttribBitmap = 0xB0,
	AttribReparsePoint = 0xC0,
	AttribEAInfo = 0xD0,
	AttribEA = 0xE0,
	AttribPropertySet = 0xF0,
	AttribLoggedUtilityStream = 0x100,
	AttribNoMoreAttrib = 0xffffffff
} ATTRIB_TYPE;


typedef struct _MFT_
{
	DWORD	MagicNumber;	//0
	WORD	OffsetToFixup;	//4
	WORD	NumberOfFixups;	//6
	LONGLONG	res0;		//8
	WORD	SequenceNumber;	//16
	WORD	HardLinkCount;
	WORD	OffsetToAttributes;
	WORD	Flags;	// 1-non-resident attributes, 2-record is directory
	DWORD	LengthOfPart;	// of the record (header + attributes)
	DWORD	AllocatedLength;	// of the record
	LONGLONG MainMFTRecord;	//, if continuation record, or 0
	WORD	MaximumAttr;		//ibute number + 1
	BYTE	Fixup[1];	// (length depends on record size)
}MFT;


typedef struct NtfsAttribList_
{
	ATTRIB_TYPE AttrType;
	WORD	SizeOfRecord;
	BYTE	SizeOfAttrName;
	BYTE	NameOffset;
	LONGLONG StartVCN;
	LONGLONG MFTRecord;	//
	//DWORD	Res1;
	WORD	AttrID;
	WCHAR	AttrName[1];
} NTFSAttribList;

typedef struct Ntfs_CommonAttributes_
{
	ATTRIB_TYPE	AttrType;
	DWORD	Length;
	BYTE	IsNonResident;		//0=resident, 1=data stored in runs
	BYTE	NameLength;
	BYTE	Offset;			// to name or resident data
	BYTE	Res0;
	BYTE	IsCompressed;	//1=compressed
	BYTE	Res2;
	WORD	AttributeID;
} NTFSCommonAttributes;

typedef struct Ntfs_ResAttributes_
{
	ATTRIB_TYPE	AttrType;
	DWORD	Length;
	BYTE	IsNonResident;		//0=resident, 1=data stored in runs
	BYTE	NameLength;
	BYTE	Offset;			// to name or resident data
	BYTE	Res0;
	BYTE	IsCompressed;	//1=compressed
	BYTE	Res1;
	WORD	AttributeID;
//Resident attributes:
	WORD	DataLength;
	WORD	Res2;
	WORD	OffsetToData;
	WORD	Res3;
	WORD	IsIndexed;
}NTFSResAttrib;

typedef struct Ntfs_NonResAttributes_
{
	ATTRIB_TYPE	AttrType;
	DWORD	Length;
	BYTE	IsNonResident;		//0=resident, 1=data stored in runs
	BYTE	NameLength;
	BYTE	Offset;			// to name or resident data
	BYTE	Res0;
	BYTE	IsCompressed;	//1=compressed
	BYTE	Res2;
	WORD	AttributeID;
//Nonresident attributes:
	LONGLONG StartVCN;
	LONGLONG LastVCN;
	WORD	OffsetToData;
	WORD	ComprEngine;	// ? '4' for compressed files
	BYTE	Res1[4];
	LONGLONG SizeOfAttrib;
	LONGLONG DiskSpace;
	LONGLONG LengthInitData;
	LONGLONG ComprSize;
}NTFSNonResAttrib;


typedef struct tag_NTFS_RUN_LIST {
	DWORD cluster;
	DWORD len;
	struct tag_NTFS_RUN_LIST* Next;
} NTFS_RUN_LIST;

typedef struct tag_NTFS_CACHE_DATA
{
	BYTE* compressed;
	BYTE* decompressed;
	BYTE* cached_data; // points into 'compressed' buffer
	UINT  cached_lcn;
	UINT  cached_clusters;
} NTFS_CACHE_DATA;

typedef struct tag_NTFS_ATTR 
{
	union {
		NTFSNonResAttrib* nra;
		NTFSResAttrib* ra;
	};
	WCHAR* pName;
	void* pData;
	LONGLONG nDataLen;
	NTFS_RUN_LIST* rl;
	NTFS_CACHE_DATA* pComp;
	NTFS_FILE* pFile;
} NTFS_ATTR;

typedef struct tag_NTFS_ATTR_LIST
{
	NTFS_ATTR* Attributes;
	UINT nAttrCount;
	UINT nDAAllocated;
	void** ppMFTRecords;
	UINT nMFTRecordsCount;
	UINT nMFTRecordsAllocated;
} NTFS_ATTR_LIST;

typedef struct Ntfs_StInfo
{
	FILETIME CreateTime;
	FILETIME LastModTime;
	FILETIME LastModMFT;
	FILETIME LastAccess;
	DWORD	FileAttrib;
}NTFSStandartInfo;

typedef struct NTFS_FILEName_
{
	LONGLONG MFTRec;
	FILETIME CreateTime;	//8
	FILETIME LastModTime;	//10
	FILETIME LastModMFT;	//18
	FILETIME LastAccess;	//20
	LONGLONG FileSize;		//28
	LONGLONG AttribSize;	//30
	LONGLONG Flags;			//38
	BYTE NameLen;		//In words//40
	BYTE TypeOfName;
	WCHAR Name[1];
} NTFS_FILE_NAME;

typedef struct _Data_
{
	UINT data;
	LPSTR name;
	UINT Father;
}data;

typedef struct _Tree_
{
	UINT k;
	int weight;
	void* left;
	void* right;
	data* data;
}tree;

#define NO_CRC 103
#define FAST_CRC 104
#define FULL_CRC 105

typedef struct _DirectoryInfo_
{
	char Name[14];
	WORD Father;
	WORD Flag;
	BYTE LFNbyte;	// For future LFN Support
	BYTE Reserved;	// Reserved for future use
} DirInfo;

typedef struct _FileInfo_
{
	char Name[8];
	char Ext[3];
	BYTE atr;
	WORD time;
	WORD date;
	WORD cluster;
	long size;
	DWORD CRC;
	WORD Dir;
	WORD Flag;
	WORD Res;		// Hi part for FAT32 StartCluster
	BYTE Reserved;	// Reserved for future use
	BYTE LFNbyte;	// For future LFN Support
} FileInfo;


#define IS_MAGIC(a,b)		(*(DWORD*)(a) == b)//(*(int*)(a)==*(int*)(b))
//#define IS_MFT_RECORD(a)	IS_MAGIC((a),"FILE")
//#define IS_NTFS_VOLUME(a)	IS_MAGIC((a)+3,"NTFS")
//#define IS_INDEX_RECORD(a) IS_MAGIC((a),"INDX")
#define MFT_MAGIC_FILE 'ELIF'
#define MFT_MAGIC_NTFS 'SFTN'
#define MFT_MAGIC_INDX 'XDNI'

typedef struct _HNTFS_
{
	//HFILE hVol;
	void *nls_map;
	/* Data read from the boot file */
	DWORD blocksize;
	DWORD clusterfactor;
	DWORD clustersize;
	DWORD mft_recordsize;
	INT mft_clusters_per_record;
	DWORD index_recordsize;
	DWORD index_clusters_per_record;
	LONGLONG mft_cluster;
	/* data read from special files */
	//unsigned char *mft;
	//void* VCN;
	//NTFS_RUN_LIST* MFTRl;
	//UINT TotalRecordOfMFT;
	//LPVOID hMFT;
	NTFS_FILE* pMFT;
	HANDLE hDrive;
	void* llio; 
}NTFS, *HNTFS;

typedef  struct NtfsBOOT_{
	BYTE jmp[3];
	char oem[8];
	WORD secsize;
	char clustsize;
	BYTE res[7];
	BYTE media;
	WORD res1;
	WORD seccnt;
	WORD headcnt;
	BYTE res2[8];
	WORD Un0;
	WORD Un1;
	LONGLONG TotalSectors;
	LONGLONG MFTCluster;
	LONGLONG MFTMirrCluster;
	char MFTRecordSize;
	BYTE Res2[3];
	DWORD IndexBufferSize;
	DWORD VolSerial;
	DWORD VolSerialHi;
	BYTE Res3[0xD];
	BYTE BoorPrg[0x1A1];
	WORD Magic;
} NTFSBOOT;

#define DOSACCES    0		// Через DOS
#define DRIVERACCES 1		// Int 25h
#define BIOSACCES   2		// Int 13h
#define WDCTLACCES  3		// Через VxD
#define MAXACCESSMODE 3

#define FDD35	  0			// Это данные для типа диска
#define FDD525	  1			// Используется в структуре
#define HDD		  2			// DriverInfo
#define NETDRIVE  3			// Remote
#define CDDISK	  4			// CD-ROM
#define RAMDISK   5			// RAM-Drive
#define COMPRDISK 6			// Compressed disk
#define COMPRDISKDRSPACE 8	// Compressed disk with PM Compress prog
#define SUBSTDRIVE	9		// Substed Drive
#define RMMDRIVE	10		// Real Mode mapped Drive
#define NTFSDRIVE 11

#define FILE_SYSTEM_FAT		0	// Всякие разные файловые
#define FILE_SYSTEM_VFAT	1	// системы, поддерживаемые Win95
#define FILE_SYSTEM_NTFS	2
#define FILE_SYSTEM_HPFS	3
#define FILE_SYSTEM_CDFS	4
#define FILE_SYSTEM_UNKNOWN	5
#define FILE_SYSTEM_VFAT32	6
#define FILE_SYSTEM_NW		7	// Novell Netware FS

typedef struct _HDDPARAM_ {
	WORD Sectors;
	WORD Cylinders;
	WORD Heads;
	DWORD StartSector[32];
	WORD HowPartisions;
	BYTE HDDId;
}HDDPARAM;

typedef struct _HDDINFO_ {
	WORD Sectors;
	WORD Cylinders;
	WORD Heads;
	WORD HowHDD;
}HDDINFO;

typedef struct tag_INDEX_HEADER
{
	DWORD dwEntriesOffset;
	DWORD dwIndexLen;
	DWORD dwAllocatedSize;
	BYTE  nFlags; // LARGE_INDEX, INDEX_NODE
	BYTE  reserved[3];     // align
} INDEX_HEADER;

typedef struct _tag_NTFSIndexRoot_
{
        ATTRIB_TYPE IndexedAttr;        // AttrType of the indexed attribute. Always $FILE_NAME for directories.
        DWORD   dwCollationRule;        // COLLATION_FILE_NAME=1
        DWORD   dwSizeOfIndexRecord;    // Size of index record ???
        BYTE    dwClusterPerIndexBlock; // Number of clusters per index record ???
		BYTE    reserved[3];            // align
		INDEX_HEADER IndexHeader;
} NTFS_INDEX_ROOT;

typedef struct _tag_NTFSIndex_
{
        BYTE Magic[4];  //'INDX'
        WORD OffsetToFixup;
        WORD NrOfFixups;
        LONGLONG lsn;
        LONGLONG BufferVCN;
		INDEX_HEADER IndexHeader;
        //WORD HeaderSize;
        //WORD res2;
        //DWORD InUseBuffer;
        //DWORD LengthOfBuffer;
        //DWORD res3;
        //BYTE FixUp[0xa];
//18+header size         Entry list
} NTFS_INDEX;

typedef struct tag_NTFS_INDEX_ENTRY_HEADER
{
	MFT_REF MftRef;
	WORD    EntrySize;
	WORD    Reserved;
	WORD    Flags;           //Flags: INDEX_ENTRY_NODE, INDEX_ENTRY_END
	WORD    res;
} NTFS_INDEX_ENTRY_HEADER;

#define FILENAME_LONG  1
#define FILENAME_SHORT 2

typedef struct tag_NTFS_INDEX_ENTRY
{
		NTFS_INDEX_ENTRY_HEADER Header;
        LONGLONG DirWhereEntryAppears;
        FILETIME CreateTime;
        FILETIME LastModTime;
        FILETIME LastModMFT;
        FILETIME LastAccess;
        LONGLONG AllotedDataSize;
        LONGLONG DataSize;
        DWORD   Attrib;
        DWORD   res2;
        BYTE    NameLen;
        BYTE    FileNameAttrib;
        WCHAR   FileName[1];    //52(2*namelen)          File name
        //entry size-8 (8)       Virtual cluster number of index buffer with subnodes
} NTFS_INDEX_ENTRY;

typedef struct NTFS_FILE
{
	HNTFS pVolume;
	LONGLONG nMftRec;
	NTFS_ATTR_LIST* pAttrList;
	BOOL  bDir;
	NTFS_INDEX_ROOT* pIndexRoot;
	NTFS_ATTR* pAttrIndexAllocation;
	NTFS_INDEX_ENTRY** ppIndexEntries;
	ULONG nIndexEntriesCount;
} NTFS_FILE;

#include <PopPack.h>

HNTFS NtfsOpen(LPCWSTR wcsDriveName);
void  NtfsClose(HNTFS pVolume);

NTFS_FILE* NtfsOpenFile(HNTFS pVolume, MFT_REF MftRef);
void NtfsCloseFile(NTFS_FILE* pFile);

MFT_REF NtfsFindFileW(HNTFS pVolume, LPCWSTR szPath);
MFT_REF NtfsFindDirEntryW(NTFS_FILE* pDir, LPCWSTR szFileName);

HANDLE NtfsEnumDirFirst(NTFS_FILE* pDir, NTFS_INDEX_ENTRY** ppIndexEntry);
BOOL NtfsEnumDirNext(HANDLE hEnumDir, NTFS_INDEX_ENTRY** ppIndexEntry);
BOOL NtfsEnumDirClose(HANDLE hEnumDir);

typedef BOOL (__cdecl *NTFS_ENUM_DIR_PROC)(void* context, NTFS_INDEX_ENTRY* ie); // callback prototype, return FALSE to stop enumeration
BOOL NtfsEnumDirCB(NTFS_FILE* pDir, void* pEnumContext, NTFS_ENUM_DIR_PROC fEnumProc);


BOOL NtfsReadFile(NTFS_FILE* pFile, LONGLONG Position, LPVOID lpBuffer, DWORD nBytesToRead, DWORD* pnBytesRead);
BOOL NtfsReadAttribute(NTFS_FILE* pFile, ATTRIB_TYPE AttrType, LPCWSTR szAttribName, LONGLONG nPos, LPVOID pData, DWORD nBytesToRead, DWORD* pnBytesRead);
BOOL NtfsReadAttributeEx(NTFS_ATTR* pAttr, LONGLONG nPos, void* pData, DWORD nBytesToRead, DWORD* pnBytesRead);

NTFS_ATTR* NtfsFindAttr(NTFS_FILE* pFile, ATTRIB_TYPE AttrType, LPCWSTR szAttribName);

NTFS_ATTR* NtfsEnumAttributes(NTFS_FILE* pFile, NTFS_ATTR* pPrevAttr);
const char* NtfsGetAttrName(ATTRIB_TYPE nAttrType);

BOOL NtfsFlushVolume(CHAR cDrive);
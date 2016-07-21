#pragma pack(1)

// =================================================================
// CAB FILE LAYOUT
// =================================================================
/*
(1) CAB_HEADER structure
(2) Reserved area, if CAB_HEADER.flags & CAB_FLAG_RESERVE
(3) Previous cabinet name, if CAB_HEADER.flags & CAB_FLAG_HASPREV
(4) Previous disk name, if CAB_HEADER.flags & CAB_FLAG_HASPREV
(5) Next cabinet name, if CAB_HEADER.flags & CAB_FLAG_HASNEXT
(6) Next disk name, if CAB_HEADER.flags & CAB_FLAG_HASNEXT
(7) CAB_FOLDER structures (n = CAB_HEADER.cFolders)
(8) CAB_ENTRY structures / file names (n = CAB_HEADER.cFiles)
(9) File data (offset = CAB_FOLDER.coffCabStart)
*/

// =================================================================
// MACROS
// =================================================================
#define SWAPWORD(x)  ((tWORD) (((x) << 8) | ((x) >> 8)))
#define SWAPDWORD(x) ((SWAPWORD ((tWORD) (x)) << 16) | \
                      (SWAPWORD ((tWORD) ((x) >> 16))))


// =================================================================
// CONSTANTS
// =================================================================
#define CAB_SIGNATURE        SWAPDWORD ( 0x4d534346 /* 'MSCF' */)
#define CAB_VERSION          0x0103

#define CAB_FLAG_HASPREV     0x0001
#define CAB_FLAG_HASNEXT     0x0002
#define CAB_FLAG_RESERVE     0x0004

#define CAB_ATTRIB_READONLY  0x0001
#define CAB_ATTRIB_HIDDEN    0x0002
#define CAB_ATTRIB_SYSTEM    0x0004
#define CAB_ATTRIB_VOLUME    0x0008
#define CAB_ATTRIB_DIRECTORY 0x0010
#define CAB_ATTRIB_ARCHIVE   0x0020

#define CAB_FILE_FIRST       0x0000
#define CAB_FILE_NEXT        0x0001
#define CAB_FILE_SPLIT       0xFFFE
#define CAB_FILE_CONTINUED   0xFFFD
#define CAB_FILE_PREV_NEXT   0xFFFF

#define CAB_NOTIFY_OK        1
#define CAB_NOTIFY_ERROR     0
#define CAB_NOTIFY_SKIP      0
#define CAB_NOTIFY_ABORT     (-1)

// =================================================================
// CABINET STRUCTURES
// =================================================================
typedef struct _CAB_HEADER
    {
    tDWORD sig;              // file signature 'MSCF' (CAB_SIGNATURE)
    tDWORD csumHeader;       // header checksum (0 if not used)
    tDWORD cbCabinet;        // cabinet file size
    tDWORD csumFolders;      // folders checksum (0 if not used)
    tDWORD coffFiles;        // offset of first CAB_ENTRY
    tDWORD csumFiles;        // files checksum (0 if not used)
    tWORD  version;          // cabinet version (CAB_VERSION)
    tWORD  cFolders;         // number of folders
    tWORD  cFiles;           // number of files
    tWORD  flags;            // cabinet flags (CAB_FLAG_*)
    tWORD  setID;            // cabinet set id
    tWORD  iCabinet;         // zero-based cabinet number
    }
    CAB_HEADER, *PCAB_HEADER;
#define CAB_HEADER_ sizeof (CAB_HEADER)

typedef struct _CAB_HEADER_RES
{
    tDWORD sig;              // file signature 'MSCF' (CAB_SIGNATURE)
    tDWORD csumHeader;       // header checksum (0 if not used)
    tDWORD cbCabinet;        // cabinet file size
    tDWORD csumFolders;      // folders checksum (0 if not used)
    tDWORD coffFiles;        // offset of first CAB_ENTRY
    tDWORD csumFiles;        // files checksum (0 if not used)
    tWORD  version;          // cabinet version (CAB_VERSION)
    tWORD  cFolders;         // number of folders
    tWORD  cFiles;           // number of files
    tWORD  flags;            // cabinet flags (CAB_FLAG_*)
    tWORD  setID;            // cabinet set id
    tWORD  iCabinet;         // zero-based cabinet number

	tWORD cbCFHeader;
	tBYTE cbCFFolder;
	tBYTE cbCFData;
} CAB_HEADER_RES, *PCAB_HEADER_RES;


// -----------------------------------------------------------------
// CFFolder as MS called it
typedef struct _CAB_FOLDER
{
    tDWORD coffCabStart;     // offset of folder data
    tWORD  cCFData;          // ???
    tWORD  typeCompress;     // compression type (tcomp* in FDI.h)
}CAB_FOLDER, *PCAB_FOLDER;
#define CAB_FOLDER_ sizeof (CAB_FOLDER)

// -----------------------------------------------------------------
typedef struct _CAB_ENTRY
{
	tDWORD cbFile;           // uncompressed file size
	tDWORD uoffFolderStart;  // file offset after decompression
	tWORD  iFolder;          // file control id (CAB_FILE_*)
	tWORD  date;             // file date stamp, as used by DOS
	tWORD  time;             // file time stamp, as used by DOS
	tWORD  attribs;          // file attributes (CAB_ATTRIB_*)
}CAB_ENTRY, *PCAB_ENTRY;
#define CAB_ENTRY_ sizeof (CAB_ENTRY)

typedef struct _CFDATA_
{
	tDWORD	csum;							// Checksum      (0 if split)
	tWORD	cbData;						// Cooked Length
	tWORD	cbUncomp;					// Raw Length    (0 if split)
//	tWORD	Sign;
} CFDATA;


#define cCopyBufferSize 0x1000

tERROR ioCopyFile(tBYTE* Buffer, hIO Src, tQWORD dwSrcStartPos, tDWORD dwSize, hIO Dest, tQWORD dwDestStartPos);
tDWORD SkipCABname(hSEQ_IO hIo, tCHAR* szBuffer);
tDWORD SkipCABnameIo(hIO hArcIO, tCHAR* Buffer, tDWORD dwPos);
tERROR SeqIoCopyFile(tBYTE* Buffer, hIO Src, tQWORD qwSrcStartPos, tDWORD dwSize, hSEQ_IO Dest);
tDWORD checksum(tBYTE *buf, tDWORD len, tDWORD csum);
tDWORD checksumIo(hSEQ_IO hOutputSeqIo, tBYTE* Buffer, tDWORD BufferSize, tDWORD len, tDWORD csum);
tVOID FillCABHeader(PCAB_HEADER pch);

#pragma pack()

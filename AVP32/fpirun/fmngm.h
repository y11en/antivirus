/****************************************************************************\
  @doc            FMNGM
  @module         FMNGM.H | This module contains the File Management module
                  functions declarations, error codes and structures.
  @end
  AUTHOR........: Alex Starkov (AS)
  COPYRIGHT.....: Copyright (c) 1997, Data Fellows Ltd.
  VERSION.......: 1.00
  HISTORY.......: DATE     COMMENT
                  -------- --------------------------------------------------
                  03-01-97 Created.
                  09-06-97 Changed to integrate ZIP and ARJ components (alk)
                  25-05-98 Considerably changed to extend functionality (alk)
				  16-02-01 turbpa2001-02-16[scr-14191] - UNICODE support
				  26-02-01 turbpa2001-02-26[scr-14379] - OS File handle
				  06-03-01 turbpa2001-06-03[scr-14526] - FM API calling 
												convention should be explicitly
				  25-06-01 turbpa250601[scr-15926] Infections inside TAR/TGZ/RAR/ARC archives are not found
				  27-06-01 Sigma If wanted allow also __fastcall usage in NT Kernel mode
				  28-06-01 turbpa250601[scr-15926] CAB file format
                  08-03-02 turbpa2002-03-08#20384 FM API fails to process files under heavy loading  
                  15-04-02 #21411
                  10-01-03 #24091 
\****************************************************************************/

/*===========================================[ REDEFINITION DEFENCE ]=======*/
#ifndef FMNGM_H
#define FMNGM_H

/*===========================================[ SPECIAL ]====================*/

#ifndef UNIX
#pragma pack(push, 1)
#endif

#ifdef VTOOLSD

#define FMAPI

#else // ! VTOOLSD

#if defined UNIX

#ifdef FMAPI
#undef FMAPI
#endif
#define FMAPI

//Sigma2001-06-27{
#elif defined(NT_DRIVER)
#if !defined(FM_STATIC_LIBRARY) && !defined(__FM_DRIVER_SOURCE__)
#define FMAPI _declspec(dllimport)
#else
#define FMAPI
#endif  // FM_STATIC_LIBRARY, __FM_DRIVER_SOURCE__
#else   // ! NT_DRIVER
//Sigma2001-06-27}

#ifndef WIN32
 #undef FMAPI
 #define FMAPI _far _pascal _export
#else
    #ifdef __BORLANDC__
     #define FMAPI _cdecl
    #else
//turbpa2001-06-03[scr-14526]{
//     #define FMAPI
	#undef FMAPI
	#define FMAPI _cdecl
//turbpa2001-06-03[scr-14526]}

    #endif
#endif
#endif  // NT_DRIVER && ! FM_STATIC_LIBRARY
#endif  // ! VTOOLSD


#ifdef NT_DRIVER
//
// Debugging and assert macros - NT kernel - specific

#define THIS_FILE_BUG_CHECK 0xDF41

#define ThisFileId() ((ULONG) ((THIS_FILE_BUG_CHECK)<<16|__LINE__))

#define DFBugCheck() KeBugCheck(ThisFileId())

extern char * DFDriverName;

#if 1 // DBG > 0 // for now I want the DbgPrints all the time

    #define DFAssert(x) if (!(x)) DFBugCheck()
    // NOTE: DFDbgPrint requires double parentheses
    #define DFDbgPrintWIthLines(x) \
        do { \
            DbgPrint("%s: (%s:%d) ", DFDriverName, __FILE__,__LINE__); \
            DbgPrint x; \
            DbgPrint("\n"); \
        } while (0)

    #define DFDbgPrint(x) \
        do { \
            DbgPrint("%s: ", DFDriverName); \
            DbgPrint x; \
            DbgPrint("\n"); \
        } while (0)

#else 

    // no asserts in the free build
    #define DFAssert(x)

    // nor debug outputs
    #define DFDbgPrint(x)

#endif

#endif  // NT_DRIVER

/*===========================================[ IMPORT DECLARATIONS ]========*/
/*===========================================[ C++ DEFENCE ]================*/
#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/*===========================================[ PUBLIC CONSTANTS ]===========*/

#include "stdfm.h"

#define MAX_LEN_FMFILENAME  MAX_PATH  // max pathname length of files
#define MAX_LEN_FMPASSWORD  256 // max password length
#define MAX_LEN_PACKERNAME  256 // max length of packer name

// types of items in archives (FMFINDDATA.ulFileType); note that
// "FMFTYPE_DIR" is NOT the same thing as "isContainer" == TRUE in
// "FMFILEINFO" structure; good example is directories packed inside
// ZIP archives - those are of FMFTYPE_DIR type, but "isContainer" == FALSE
// for them meaning we don't need to run fmFindFirst/NextFile inside them
#define FMFTYPE_FILE   0x00000000L // item is a file
#define FMFTYPE_DIR    0x00000001L // item is a directory (can't be extracted)

// FM components' IDs - these MUST be consecutive numbers starting with 0;
// it is an ABSOLUTE REQUIREMENT that EXE component has index of 0 !
// "EXE" stands for an _arbitrary_ ordinary disk file, that is, for any file
// that can be handled without decompressing / decoding / special processing;
// this is because of historical reasons - originally FM was targeted to
// provide AV scanners to scan not only disk files (usually, executables)
// but also entries inside compressed files, etc.
#define COMPONENT_TYPE_EXE  0   // disk files processing
// due to historical reasons, we have a single component for LZH and ZIP
#define COMPONENT_TYPE_ZIP  1   // ZIP or LZH compressed data processing
#define COMPONENT_TYPE_ARJ  2   // ARJ compressed data processing
//turbpa250601[#15926]{
#define COMPONENT_TYPE_TAR  3   // TAR compressed data processing
#define COMPONENT_TYPE_CAB  4   // CAB compressed data processing
#define COMPONENT_TYPE_RAR  5   // RAR compressed data processing
#define COMPONENT_TYPE_BZIP 6   // RAR compressed data processing
#define COMPONENT_TYPE_RAR30  7   // RAR30 compressed data processing
//turbpa250601[#15926]}

// constants (bits) that can be set into the corresponding parameter of
// "fmOpenFile" to skip type recognition for a particular file format, or
// parameter of "fmInitializeModule" to skip initialization of certain
// components (those will be unavailable for the entire FM session);
// "exe" recognition (ordinary disk files) may never be skipped !
// It is an ABSOLUTE REQUIREMENT that :
//  SKIP_CHECK4XXX is defined as ((ulong) 1 << (COMPONENT_TYPE_XXX - 1)) !
#define SKIP_CHECK4ZIP          ((ulong) 1 << (COMPONENT_TYPE_ZIP - 1))
#define SKIP_CHECK4ARJ          ((ulong) 1 << (COMPONENT_TYPE_ARJ - 1))
//turbpa020415-#21411{
#define SKIP_CHECK4TAR          ((ulong) 1 << (COMPONENT_TYPE_TAR - 1))
#define SKIP_CHECK4CAB          ((ulong) 1 << (COMPONENT_TYPE_CAB - 1))
#define SKIP_CHECK4RAR          ((ulong) 1 << (COMPONENT_TYPE_RAR - 1))
#define SKIP_CHECK4BZIP         ((ulong) 1 << (COMPONENT_TYPE_BZIP - 1))
#define SKIP_CHECK4RAR30        ((ulong) 1 << (COMPONENT_TYPE_RAR30 - 1))
//turbpa020415-#21411}

#define SKIP_ALL_TYPECHECKS     0xFFFFFFFF

// data masks for items in archives (FMFINDDATA.ulDataMask)
#define FMFDATAMASK_DOSTIME         0x00000001L // ulDosTime is valid
#define FMFDATAMASK_UNIXTIME        0x00000002L // ulUnixTime is valid
#define FMFDATAMASK_PACKEDSIZE      0x00000004L // ulPackedSize is valid
#define FMFDATAMASK_UNPACKEDSIZE    0x00000008L // ulUnpackedSize is valid
#define FMFDATAMASK_CRC32           0x00000010L // ulCrc32 is valid
#define FMFDATAMASK_CRC16           0x00000020L // usCrc16 is valid
#define FMFDATAMASK_PACKRATIO       0x00000040L // usPackRatio is valid
#define FMFDATAMASK_PACKMETHOD      0x00000080L // usPackMethod is valid
#define FMFDATAMASK_FILEATTRIBUTES  0x00000100L // usFileAttributes is valid

// access mode flags for fmOpenFile - self-explanatory
#define FM_READONLY_ACCESS          0x0000
#define FM_WRITEONLY_ACCESS         0x0001
#define FM_RDWR_ACCESS              0x0002
#define FM_CREATE_ACCESS            0x0100

// 990208, alk, values for share mode ("fmOpenFile") added
// share mode for fmOpenFile - self-explanatory
#define FM_SHARE_COMPAT             0x0000
#define FM_SHARE_DENYRW             0x0010
#define FM_SHARE_DENYALL            FO_SHARE_DENYRW
#define FM_SHARE_DENYWR             0x0020
#define FM_SHARE_DENYRD             0x0030
#define FM_SHARE_DENYNONE           0x0040

// packer type
#define FM_PACKER_NONE          0           // None
#define FM_PACKER_ZIP           1           // PKZIP packer
#define FM_PACKER_LHA           2           // LHA packer
#define FM_PACKER_ARJ           3           // ARJ packer
//turbpa250601[#15926]{
#define FM_PACKER_TAR           4           // TAR packer
#define FM_PACKER_CAB           5           // CAB packer
#define FM_PACKER_RAR           6           // RAR packer
#define FM_PACKER_BZIP          7           // BZIP2 packer
#define FM_PACKER_RAR30         8           // RAR packer
//turbpa250601[#15926]}
// Error codes
#define FM_ERR_NONE           0x0000  // no error
#define FM_ERR_FILEOPENED     0x0001  // no error
#define FM_ERR_ARCHIVEOPENED  0x0002  // no error
#define FM_ERR_NOMEM          0x0003  // not enough memory
#define FM_ERR_OPENFAILED     0x0004  // error opening file
#define FM_ERR_NOMOREHANDLES  0x0005  // no more handles available
#define FM_ERR_FILEIO         0x0006  // file I/O error
#define FM_ERR_WASINITED      0x0007  // FM has already been initialized
#define FM_ERR_INITFAILED     0x0008  // FM initialization failed
#define FM_ERR_UNINITFAILED   0x0009  // FM uninitialization failed
#define FM_ERR_BADARCHIVE     0x0011  // archive is corrupted
#define FM_ERR_NOMOREFILES    0x0012  // no more files in archive
#define FM_ERR_NOTINARCHIVE   0x0013  // no such file in archive
#define FM_ERR_BADFILE        0x0014  // file is corrupted in archive
#define FM_ERR_BADHANDLE      0x0015  // invalid handle passed
#define FM_ERR_BADOFFSET      0x0016  // seek/read offset beyond the file size
#define FM_ERR_ENCRYPTED      0x0017  // bad argument passed
#define FM_ERR_BADARGUMENT    0x0018  // seek/read offset beyond the file size
#define FM_ERR_READONLY       0x0019  // attempt to write to "ReadOnly" file or
                                      // open "ReadOnly" file in "write" mode
#define FM_ERR_INAPPROPRIATE  0x0020  // attempt to perform inappropriate
                                      // operation
#define FM_ERR_UNIMPLEMENTED  0x0029  // feature is not implemented yet
#define FM_ERR_PROGRAM_ERR    0x0079  // an error in calling FM API routine,
                                      // in FM internal logic
#define FM_ERR_UNKNOWN        0xffff  // unknown error

//well, for now ...
#if !defined NT_DRIVER && !defined VTOOLSD
#define ASSERT(f)                  ((void)(0))
#endif  // NT_DRIVER
#define VERIFY(f)                  ((void)(f))
#define DEBUG_ONLY(f)              ((void)(0))

#define TRACE(sz)
#define TRACE0                     TRACE
#define TRACE1(sz, p1)
#define TRACE2(sz, p1, p2)
#define TRACE3(sz, p1, p2, p3)
#define TRACE4(sz, p1, p2, p3, p4)

//turbpa2002-09-24[scr-23454]{

// this option defines ZIP decompress cache limit. 
// By defult cache is not exceed file size or 20 Mb
#define FM_OPTION_ZIP_CACHE_SIZE_LIMIT 0x00000001
//turbpa2002-09-24[scr-23454]}

//turbpa2002-10-15[scr-20360 ]{
#define FM_OPTION_ZIP_DISABLE_CACHE    0x00000002
//turbpa2002-10-15[scr-20360 ]}

/*===========================================[ PUBLIC TYPES ]===============*/

// @type HFMSEARCH | Search handle.
typedef int         HFMSEARCH;


// @struct FMFILEINFO | This structure contains information about file.
typedef struct FMFILEINFO_struct
{
// @field Is it a container, that is, do we need to run fmFindFirst/NextFile
//  for this entry (for example, compressed file, OLE storage, etc.)
    BOOL            isContainer;
// @field Identificator of packer type. 0 if not an archive.
    unsigned long   ulPackerTypeId;
    unsigned long   ulPackerVersion;  // @field Version of the packer.
// @field Name of the packer.
    char            pszPackerName[MAX_LEN_PACKERNAME];

} FMFILEINFO;


// @struct FMFINDDATA | This structure holds information about an archived file.
typedef struct FMFINDDATA_struct
{
// @field full pathname of file in archive
char            pszFileName[MAX_LEN_FMFILENAME * 2];
unsigned long   ulFileType;      // @field file type flags (FMFTYPE_xxx)
// @field specifies which values of this structure are valid (FMFDATAMASK_xxx)
unsigned long   ulDataMask;

unsigned long   ulDosTime;         // @field time and date in DOS format
unsigned long   ulUnixTime;        // @field time and date in UNIX format
unsigned long   ulPackedSize;      // @field file size as packed
unsigned long   ulUnpackedSize;    // @field file size as unpacked
unsigned long   ulCrc32;           // @field CRC 32
unsigned short  usCrc16;           // @field CRC 16
// @field pack ratio (per thousand, negative if packed size is larger)
unsigned short  usPackRatio;
unsigned short  usPackMethod;      // @field pack method (packer-dependent)
unsigned int    usFileAttributes;  // @field DOS file attributes

} FMFINDDATA;


// @struct FMOFSTRUCT | This structure holds information that might be
// required to open an entry.
typedef struct FMOFSTRUCT_struct
{
    char    pszPassword[MAX_LEN_FMPASSWORD]; // @field password

    // @field ... | ... | to be specified

} FMOFSTRUCT;

/*===========================================[ PUBLIC VARIABLES ]===========*/
/*===========================================[ PRIVATE FUNCTIONS ]==========*/
BOOL fmCheckHandle(HFMFILE hfmFile);

/*===========================================[ PUBLIC FUNCTIONS ]===========*/

//turbpa2002-12-30#24091{


#if defined VTOOLSD
	#define FMLIBExport
#elif defined _MSC_VER
	#ifndef WIN32
		#define FMLIBExport        _export
	#else
		#define FMLIBExport
	#endif 
#else
	#define FMLIBExport         _export
#endif

#if defined VTOOLSD
	#define FMFILEIOFN _stdcall FMLIBExport
#elif defined WIN32
	#define FMFILEIOFN _stdcall FMLIBExport
#elif defined OS2
	#define FMFILEIOFN _stdcall FMLIBExport
#elif defined UNIX
	#define FMFILEIOFN
#else // win 16
	#define FMFILEIOFN _far _pascal FMLIBExport
#endif


#define FMAPI_VERSION    0x00010001     // current FM LIB API version

#if FMAPI_VERSION > 0x00010000


//
// File IO callback prototypes
//

// file handling functions
typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_OPEN)( HFMFILE *pFileHandle,   //out. Opened handle
                                              // pointer holds the same address as pszFileName
                                              // of fmOpenFile
                                              void    *ptrFileName,   

                                              // indicates that file name pointer is Unicode based
                                              // (passed with fmOpenFileW)
                                              BOOL     IsUnicodeName,
                                              int      accessMode,     // same as accessMode of fmOpenFile
                                              BOOL     createReadOnly, // same as createReadOnly of fmOpenFile
                                              int      shareMode );    // same as shareMode of fmOpenFile

typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_CLOSE)( HFMFILE  FileHandle );  // the handle retuned by open callback

typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_SEEK)( HFMFILE  FileHandle, // the handle retuned by open callback
                                             long      lOffset,    // same as lOffset of fmSeekFile
                                             int       iOrigin);   // same as iOrigin of fmSeekFile

typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_TELL)( HFMFILE   FileHandle, // the handle retuned by open callback
                                              long      *pos );     // same as pos of fmTellFile

typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_READ)( HFMFILE   FileHandle, // the handle retuned by open callback
                                              void *    pBuf,       // same as pBuf of fmReadFile
                                       unsigned long    uNBytes,    // same as uNBytes of fmReadFile
                                       unsigned long*   puNBytesRead ); // same as puNBytesRead of fmReadFile

typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_WRITE)( HFMFILE   FileHandle, // the handle retuned by open callback
                                              void *    pBuf,        // same as pBuf of fmWriteFile
                                       unsigned long    uNBytes,     // same as uNBytes of fmWriteFile
                                       unsigned long*   puNBytesWritten ); // same as puNBytesWritten of fmWriteFile

typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_GET_FILE_SIZE)( HFMFILE   FileHandle, // the handle retuned by open callback
                                                       long * plFileSize);  // same as plFileSize of fmGetFileSize

typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_SET_FILE_SIZE)( HFMFILE   FileHandle, // the handle retuned by open callback
                                                       long      lFileSize);// same as lFileSize fmSetFileSize

// directory interation routines

// the function starts directory interation. It returns interation handle or -1 in case of error
typedef long (FMFILEIOFN *LPFMFILEIOFN_FIND_FIRST_FILE)( char * pathname_, FILEFINDDATA * fData_ );
// the function fetches next file
typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_FIND_NEXT_FILE) ( long searchHandle_, FILEFINDDATA * fData_ );
// the function closes interation handle
typedef BOOL (FMFILEIOFN *LPFMFILEIOFN_FIND_CLOSE) ( long searchHandle_ );

// the function returns last file IO error code
typedef ulong (FMFILEIOFN *LPFMFILEIOFN_GET_LAST_ERROR)();  

typedef void *(FMFILEIOFN *LPFMALLOC)( unsigned long  uNBytes ); // allocate memory block
typedef BOOL  (FMFILEIOFN *LPFMFREE)( void *pMemBlock );         // free memory block

//
// File IO callback structure
//


typedef struct FMFILEIOFNTABLE_struct
{
    ulong                             fStructureSize;   // = sizeof(FMFILEIOFNTABLE) 
    LPFMFILEIOFN_GET_LAST_ERROR       fFnGetLastError;    
    LPFMFILEIOFN_OPEN                 fFnOpen;    
    LPFMFILEIOFN_READ                 fFnRead;    
    LPFMFILEIOFN_WRITE                fFnWrite;   
    LPFMFILEIOFN_CLOSE                fFnClose;   
    LPFMFILEIOFN_GET_FILE_SIZE        fFnGetSize; 
    LPFMFILEIOFN_TELL                 fFnTell;  
    LPFMFILEIOFN_SEEK                 fFnSeek;  
    LPFMFILEIOFN_SET_FILE_SIZE        fFnSetFileSize;  
    LPFMALLOC                         fFnAlloc;
    LPFMFREE                          fFnFree;


    LPFMFILEIOFN_FIND_FIRST_FILE      fFindFirst;
    LPFMFILEIOFN_FIND_NEXT_FILE       fFindNext;
    LPFMFILEIOFN_FIND_CLOSE           fFindClose;
} // if function should not be called the corresponed pointer is filled with NULL
FMFILEIOFNTABLE, * LPFMFILEIOFNTABLE, * const LPCFMFILEIOFNTABLE;



BOOL FMAPI fmInitializeModuleEx( ulong                  FMApiVersion,    // must be filled in with FMAPI_VERSION
                                 ulong                  exclusionList,   // same as exclusionList of fmInitializeModule
                                 LPFMFILEIOFNTABLE      lpFMFileIoTable, // pointer to file IO callback table
                                 void *                 reserved         // must be NULL
                                );
#endif 

//turbpa2002-12-30#24091}





/* File Management Module API */

//---------------------------------------------------------------------------
// @func  BOOL | fmInitializeModule | This function is called to initialize
//        File Management module - start FM session.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//---------------------------------------------------------------------------
BOOL FMAPI fmInitializeModule(ulong exclusionList);
// @parm "exclusionList" is a bit mask specifying which components of the FM
// won't be initialized and used (see SKIP_CHECK4ZIP etc. above).

//---------------------------------------------------------------------------
// @func  BOOL | fmUninitializeModule | This function is called to terminate
//        File Management session and release all its resources.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//---------------------------------------------------------------------------
BOOL FMAPI fmUninitializeModule(void);
// @parm || This function has no parameters.

//---------------------------------------------------------------------------
// @func  BOOL | fmOpenFile | The <f fmOpenFile> function opens a file.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//
// @comm  The function opens the file named <p pszFileName>, creates new file
//        handle, determines the file type, and fills in the <f FMFILEINFO>
//        structure to provide the caller with information about the opened
//        file.<nl><nl>
//        If the file is a container (archive, OLE storage, ...) <p pFileInfo>
//        will be filled in with relevant info, and the caller will know it
//        should use findfirst/next functions to process the container's
//        entries.<nl><nl>
//        The <p pszFileName> parameter may not contain wildcard characters.
//        <nl><nl>
//        To close the opened file, call the <f fmCloseFile> function.
//---------------------------------------------------------------------------
BOOL FMAPI fmOpenFile(
// @parm To keep file handle of a freshly-opened file for the caller.
    HFMFILE *     phfmFile,
    HFMFILE       hfmParent,   // @parm Specifies the parent's file handle.
// @parm Points to a null-terminated string specifying name of the file to
// be opened
    const char *  pszFileName,
    int           accessMode,  // @parm Specifies the required access mode
// @parm Specifies the access for a newly created file; ignored if
// ! (accessMode & FM_CREATE_ACCESS)
    BOOL          createReadOnly,
    int           shareMode,   // @parm Specifies the required share mode
// @parm Components whose recognition routines NOT to be called for the file
// to be opened (similar with "exclusionList" parameter of fmInitializeModule)
    ulong         skipTypeCheck,
// @parm Points to the <f FMOFSTRUCT> structure that possibly contains
// information required to open the file (for example, password)
    const FMOFSTRUCT *  pOpenStruct,
// @parm Points to the <f FMFILEINFO> structure that will be filled in with
// information about the opened file
    FMFILEINFO *  pFileInfo
);





//turbpa2001-02-16[scr-14191]{
#ifdef FMUNICODE_SUPPORT
//---------------------------------------------------------------------------
// @func  BOOL | fmOpenFileW | The <f fmOpenFileW> function opens a file
// with Unicode file name. Calling of this fuction is valid only on Win32
// Unicode file name inside archives is not supported.
BOOL FMAPI fmOpenFileW(
// @parm To keep file handle of a freshly-opened file for the caller.
    HFMFILE *     phfmFile,
    HFMFILE       hfmParent,   // @parm Specifies the parent's file handle.
// @parm Points to a null-terminated Unicode string specifying name of the file to
// be opened. 
    const FMWCHAR *  pwszFileName,
    int           accessMode,  // @parm Specifies the required access mode
// @parm Specifies the access for a newly created file; ignored if
// ! (accessMode & FM_CREATE_ACCESS)
    BOOL          createReadOnly,
    int           shareMode,   // @parm Specifies the required share mode
// @parm Components whose recognition routines NOT to be called for the file
// to be opened (similar with "exclusionList" parameter of fmInitializeModule)
    ulong         skipTypeCheck,
// @parm Points to the <f FMOFSTRUCT> structure that possibly contains
// information required to open the file (for example, password)
    const FMOFSTRUCT *  pOpenStruct,
// @parm Points to the <f FMFILEINFO> structure that will be filled in with
// information about the opened file
    FMFILEINFO *  pFileInfo
);
#endif /* FMUNICODE_SUPPORT */
//turbpa2001-02-16[scr-14191]}


//---------------------------------------------------------------------------
// @func  BOOL | fmCloseFile | The <f fmCloseFile> function closes an open
//        file handle.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//
// @comm  The <f fmCloseFile> function closes handles of files opened with
//        "fmOpenFile".<nl><nl>
//        <f fmCloseFile> invalidates the specified file handle.
//---------------------------------------------------------------------------
BOOL FMAPI fmCloseFile(
    HFMFILE hfmFile // @parm Specifies handle of the file to be closed.
);

//---------------------------------------------------------------------------
// @func  BOOL | fmFindFirstFile | The <f fmFindFirstFile> function searches
//        an opened file, which must be a container (possibly inside another
//        container), for entries-files. <f fmFindFirstFile> examines
//        subdirectory names as well as filenames.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//
// @comm  The <f FindFirstFile> function opens a search handle and returns
//        information about the first entry found inside the container. Once
//        the search handle is established, you can use the
//        <f fmFindNextFile> function to search for other files. When the
//        search handle is no longer needed, close it by using the
//        <f fmFindClose> function.
//---------------------------------------------------------------------------
BOOL FMAPI fmFindFirstFile(
// @parm Search handle used in a subsequent call to <f fmFindNextFile> or
// call to <f fmFindClose>
    HFMSEARCH *     phfmSearch,
// @parm Handle of the container to search for files in.
    HFMFILE         hfmFile,
// @parm Points to the <f FMFINDDATA> structure that receives information
// about the found file. The structure can be used in subsequent calls to
// <f fmFindNextFile> or <f fmFindClose> function to refer to the file.
    FMFINDDATA *    pFindBuf
);

//---------------------------------------------------------------------------
// @func  BOOL | fmFindNextFile | The <f fmFindNextFile> function continues
//        a file search process started with a call to <f fmFindFirstFile>
//        function.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//---------------------------------------------------------------------------
BOOL FMAPI fmFindNextFile(
// @parm Specifies a search handle returned by a call to <f fmFindFirstFile>.
    HFMSEARCH       hfmSearch,
// @parm Points to the <f FMFINDDATA> structure that receives information
// about the found file. The structure can be used in subsequent calls to
// <f fmFindNextFile> or <f fmFindClose> function to refer to the file.
    FMFINDDATA *    pFindBuf
);

//---------------------------------------------------------------------------
// @func  BOOL | fmFindClose | The <f fmFindClose> function closes the
//        specified search handle. The <f fmFindFirstFile> and
//        <f fmFindNextFile> functions use search handle to locate files with
//        attributes that match a given set of values (like name, etc.).
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//
// @comm  After the <f fmFindClose> function is called, the handle specified
//        by the <p hfmSerach> parameter cannot be used any more in calls
//        to <f fmFindNextFile> and <f fmFindClose> functions.
//---------------------------------------------------------------------------
BOOL FMAPI fmFindClose(
// @parm Specifies a search handle returned by a call to <f fmFindFirstFile>.
    HFMSEARCH   hfmSearch
);

//---------------------------------------------------------------------------
// @func  BOOL | fmSeekFile | The <f fmSeekFile> function moves the file
//        pointer of an open file.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//---------------------------------------------------------------------------
BOOL FMAPI fmSeekFile(
// @parm Handle of the file whose file pointer is to be moved.
    HFMFILE hfmFile,
    long    lOffset, // @parm Specifies the number of bytes to move the
                     //       pointer. A positive value moves the pointer
                     //       forward in the file and a negative value moves
                     //       it backward.
    int     iOrigin  // @parm Specifies the starting point for the file
                     //       pointer move. This parameter can be one of the
                     //       following values:
                     // @flag FILESEEK_SET | The starting point is zero or
                     //       the beginning of the file. If FILESEEK_SET is
                     //       specified, <p lOffset> is interpreted as an
                     //       unsigned location for the new file pointer.
                     // @flag FILESEEK_CUR | The current position of the file
                     //       pointer is the starting point.
                     // @flag FILESEEK_END | The current end-of-file position
                     //       is the starting point.
);

//---------------------------------------------------------------------------
// @func  BOOL | fmTellFile | The <f fmTellFile> function gets the current
//        position of the file pointer of an open file.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//---------------------------------------------------------------------------
BOOL FMAPI fmTellFile(
// @parm Handle of the file whose file pointer position is requested.
    HFMFILE hfmFile,
// @parm To be set to the value of the current file pointer position
    long    *pos
);


//---------------------------------------------------------------------------
// @func  BOOL | fmReadFile | The <f fmReadFile> function reads specified
//        number of bytes from a file, starting at the current file pointer
//        position. After the read operation has been completed, the file
//        pointer is adjusted by the actual number of bytes read.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//---------------------------------------------------------------------------
BOOL FMAPI fmReadFile(
// @parm Handle of the file to read data from.
    HFMFILE         hfmFile,
// @parm Points to the buffer that receives the data read from the file.
    void *          pBuf,
// @parm Specifies the number of bytes to be read from the file.
    unsigned long   uNBytes,
// @parm Points to the actual number of bytes read (may be less than
// "uNBytes").
    unsigned long * puNBytesRead
);

//---------------------------------------------------------------------------
// @func  BOOL | fmWriteFile | The <f fmWriteFile> function writes specified
//        number of bytes to a file, starting at the current file pointer
//        position. After the write operation has been completed, the file
//        pointer is adjusted by the actual number of bytes written.
//
// @rdesc If the function succeeds, the return value is TRUE.<nl><nl>
//        If the function fails, the return value is FALSE. To get extended
//        error information, call <f fmGetLastError>.
//---------------------------------------------------------------------------
BOOL FMAPI fmWriteFile(
// @parm Handle of the file to write data to.
    HFMFILE         hfmFile,
// @parm Points to the buffer containing the data to be written to the file.
    const void *          pBuf,
// @parm Specifies the number of bytes to be written to the file.
    unsigned long   uNBytes,
// @parm Points to the actual number of bytes written (may be less than
// "uNBytes").
    unsigned long * puNBytesWritten
);

//---------------------------------------------------------------------------
// @func  ulong | fmGetLastError | The <f fmGetLastError> function returns
//        the last-error code value.<nl><nl>
//        Most File Management module functions set their last-error value
//        when they fail; a few functions set it when they succeed.
//
// @rdesc The return value is the last-error code value. The <f Return Value>
//        section of each File Management function's reference page notes
//        the conditions under which the function sets the last-error code.
//
// @comm  You should call the <f fmGetLastError> function immediately when
//        some function's return value indicates such a call will return
//        useful data. That is because some functions set last-error to zero
//        when they succeed, wiping out the error code set by the most
//        recently failed function.<nl><nl>
//        As noted, most File Management functions set last-error when they
//        fail. Function failure is typically indicated by a return value
//        code such a FALSE, NULL, 0xFFFFFFFF, or -1. Some functions set
//        last-error under conditions of success; those cases are noted
//        in each function's reference page.<nl><nl>
//        Error codes are 32-bit values and specified above in this file.
//---------------------------------------------------------------------------
ulong FMAPI fmGetLastError(void); // @parm || This function has no parameters.

//turbpa080302[scr-20384]{
//---------------------------------------------------------------------------
// @func  ulong | fmGetLastErrorEx | The <f fmGetLastErrorEx> function returns
//        the Operating System API last-error code value.<nl><nl>
//        Most File Management module functions set their last-error value
//        when they fail after OS function call; 
//
// @rdesc The return of the function is platform specific. Under Win 32 plarform
//        it corresponds GetLastError() under Linux/Unix it calls errono
//
//---------------------------------------------------------------------------
ulong FMAPI fmGetLastErrorEx(void); // @parm || This function has no parameters.
//turbpa080302[scr-20384]}


//---------------------------------------------------------------------------
// FUNCTION......: fmGetFileStat
// DESCRIPTION...: This obtains info about a specified file.
//---------------------------------------------------------------------------
BOOL FMAPI fmGetFileStat(
// @parm Handle of the file to obtain information about.
    HFMFILE hfmFile,
// @parm Points to the <f FMFILEINFO> structure that will be filled in with
// information about the file
    FMFILEINFO * fileInfo
);

//---------------------------------------------------------------------------
// FUNCTION......: fmGetFileSize
// DESCRIPTION...: This obtains the size of a given file.
//---------------------------------------------------------------------------
BOOL FMAPI fmGetFileSize(
// @parm Handle of the file to get size value of.
    HFMFILE hfmFile,
// @parm To be set to the size value of the specified file.
    long * plFileSize
);

//---------------------------------------------------------------------------
// FUNCTION......: fmSetFileSize
// DESCRIPTION...: This sets size of the specified file to a given value.
//---------------------------------------------------------------------------
BOOL FMAPI fmSetFileSize(
// @parm Handle of the file to set size value of.
    HFMFILE hfmFile,
// @parm Specifies new size of the file.
    long lFileSize
);

//turbpa2001-02-26[scr-14379]{
#ifdef WIN32
#if !defined NT_DRIVER &&  !defined UNIX && !defined __BORLANDC__
//---------------------------------------------------------------------------
// FUNCTION......: fmGetOSFileHandle
// DESCRIPTION...: Function returns Operating system file handle
//---------------------------------------------------------------------------
BOOL FMAPI fmGetOSFileHandle(
// @parm FM Lib Handle of file to retrive handle of
    HFMFILE hfmFile,
// @parm Result OS handle
    int *handle
);
#endif
#endif
//turbpa2001-02-26[scr-14379]}

//turbpa2002-09-24[scr-23454]{

BOOL FMAPI fmSetOption(
// @parm FM Lib Handle of file to apply option, Optional parameter. 
// If the option has Library wide scope, set the value to 0
    HFMFILE hfmFile,
// @parm Bit mask specifies what option should be taken to use
    ulong   uOptionCode, 

// @parm, the value to be applied. The sematic depends on the option
    ulong   uValue1, 
// @parm, the value to be applied. The sematic depends on the option
    ulong   uValue2, 
// @parm, the value to be applied. The sematic depends on the option
    void    *Value3
);
//turbpa2002-09-24[scr-23454]}


/*===========================================[ PRIVATE FUNCTIONS ]==========*/

//---------------------------------------------------------------------------
// FUNCTION......: fmGetFileName
// DESCRIPTION...: This function returns pointer to file name associated with
//                 hfmFile file handle, or NULL if an error occurred.
//---------------------------------------------------------------------------
#ifdef _WIN32
#ifdef __BORLANDC__
char * FMAPI fmGetFileName(HFMFILE hfmFile);
#else
//turbpa2001-06-03[scr-14526]{
//char FMAPI * fmGetFileName(HFMFILE hfmFile);
char * FMAPI fmGetFileName(HFMFILE hfmFile);
//turbpa2001-06-03[scr-14526]}
#endif
#else
char * FMAPI fmGetFileName(HFMFILE hfmFile);
#endif

//---------------------------------------------------------------------------
// FUNCTION......: fmGetExtraDataPtr
// DESCRIPTION...: This function returns pointer to the "extraData" block
//                 associated with hfmFile file handle.
//---------------------------------------------------------------------------
void ** fmGetExtraDataPtr(HFMFILE hfmFile);

//---------------------------------------------------------------------------
// FUNCTION......: fmGetFullFileName
// DESCRIPTION...: This function puts to "pszFullFileName" buffer full file
//                 name associated with hfmFile file handle. Function also
//                 converts all slashes to backslashes. Recommended size for
//                 "pszFullFileName" is MAX_LEN_FMFILENAME * 2.
//---------------------------------------------------------------------------
BOOL FMAPI fmGetFullFileName(HFMFILE hfmFile,
                             char * pszFullFileName);

// testing testing testing testing testing testing testing testing testing
//---------------------------------------------------------------------------
// FUNCTION......: fmGetNumberOfAllocs
// DESCRIPTION...: This function returns the current number of dynamically
//                 allocated memory blocks (those allocated with "FMAlloc").
//---------------------------------------------------------------------------
void FMAPI fmGetNumberOfAllocs(long *number);
// testing testing testing testing testing testing testing testing testing

/*===========================================[ PSEUDO/INLINE FUNCTIONS ]====*/
/*===========================================[ END C++ DEFENCE ]============*/
#ifdef  __cplusplus
}
#endif/*__cplusplus*/

#ifndef UNIX
#pragma pack(pop)
#endif

/*===========================================[ END REDEFINITION DEFENCE ]===*/
#endif/*FMNGM_H*/

/** (END OF FILE  : fmngm.h) ************************************************/

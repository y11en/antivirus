// fpiapi.h
//
// Copyright (c) 1998-2000 F-Secure Corp.
//
// This file is part of the FPI API, which is part of the CounterSign framework.
// See the appropriate documents for more information.


#ifndef FPIAPI_H
#define FPIAPI_H

#ifndef __FPIAPI_H
#define __FPIAPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined UNIX || defined OS2

#if defined OS2
#include <os2.h>
#endif
#if defined UNIX
#define _pascal
#define _export
#define _far
#endif

#if defined UNIX
	typedef unsigned char BYTE;
	typedef int BOOL;
#endif

    typedef unsigned short WORD;
    typedef unsigned long DWORD;
    typedef unsigned int UINT;
    typedef const void* LPCVOID;
    typedef WORD* LPWORD;
    typedef DWORD* LPDWORD;
    typedef DWORD HANDLE;
    typedef HANDLE HGLOBAL;
    typedef const char* LPCSTR;
    typedef char* LPSTR;
    typedef void* LPVOID;
#define LONG long
#define CHAR char
#define FAR

#elif defined VTOOLSD

#ifdef __cplusplus
#include <vtoolscp.h>
#else // !__cplusplus
#include <vtoolsc.h>
#endif
    typedef void* LPVOID;
    typedef const void* LPCVOID;
    typedef WORD* LPWORD;
    typedef DWORD* LPDWORD;

#elif defined NT_DRIVER

#ifdef OSRDDK
#include <osrddk.h>			// jmk990928: This is for OSR DDK debug builds
#else
#include <ntddk.h>
#endif
    typedef int BOOL;
    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned long DWORD;
    typedef unsigned int UINT;
    typedef void* LPVOID;
    typedef const void* LPCVOID;
    typedef WORD* LPWORD;
    typedef DWORD* LPDWORD;
#define FAR
    typedef DWORD HGLOBAL;
    typedef DWORD HWND;

#else

#ifndef __WINDOWS_H
#ifndef _WINDOWS_
#include <windows.h>
#endif
#endif

#endif


// Compiler-specific additions

#ifdef _WIN32
 // (MSVC defines _WIN32.)
#ifndef __WIN32__
#define __WIN32__
#endif
#endif

#ifdef __BORLANDC__
#ifndef __WIN32__
    typedef const LPVOID LPCVOID;
#endif
    typedef char CHAR;
#endif

#ifdef _MSC_VER
#ifndef __WIN32__
    typedef char CHAR;
    typedef const LPVOID LPCVOID;
#endif
#endif

#ifndef __WIN32__
typedef struct _LARGE_INTEGER { 
    DWORD LowPart; 
    LONG  HighPart; 
} LARGE_INTEGER; 
#endif


// API version
#define FPIAPI_VER_MAJOR                1U
#define FPIAPI_VER_MINOR                17U


// Limit constants
#define FPIMAX_MODULENAME               32U
#define FPIMAX_VENDORNAME               82U
#define FPIMAX_READLENGTH               65534UL
#define FPIMAX_WRITELENGTH              65534UL
#define FPIMAX_VIRUSDESCRIPTION         65534UL

// Simple types
typedef int FPICONTAINERHANDLE;
typedef int FPIFILEHANDLE;


// Flags and other codes

#define FPIFILEIO_CURRENT               1U
#define FPIFILEIO_END                   2U
#define FPIFILEIO_BEGIN                 0U

#define FPIFILEIO_READ                  0U
#define FPIFILEIO_WRITE                 1U
#define FPIFILEIO_RDWR                  2U
// Sigma2000-02-15 a new file access mode parameter since FPI API 1.17.
#define FPIFILEIO_CREATE                3U

#define FPIFILEIO_FILEATTR_ARCHIVE      0x00000020
#define FPIFILEIO_FILEATTR_DIRECTORY    0x00000010
#define FPIFILEIO_FILEATTR_HIDDEN       0x00000002
#define FPIFILEIO_FILEATTR_READONLY     0x00000001
#define FPIFILEIO_FILEATTR_SYSTEM       0x00000004
#define FPIFILEIO_FILEATTR_LABEL        0x00000008
#define FPIFILEIO_FILEATTR_TEMPORARY    0x00000100
#define FPIFILEIO_FILEATTR_COMPRESSED   0x00000800

// jmk991105: This flag can be passed in the reservedA
// parameter in fnOpen, to specify that the file
// is one of the scanning engine's own database files.
#define FPIFILEIO_OPENFLAG_DATABASE	1U

#define FPIACTION_NONE              0U
#define FPIACTION_DISINFECT         1U
//#define   FPIACTION_DELETE        2U
//#define   FPIACTION_RENAME        3U

#define FPIEVENT_INFECTION          1U
#define FPIEVENT_QUERY              2U
#define FPIEVENT_ERROR              3U
#define FPIEVENT_QUERYABORT         4U
#define FPIEVENT_DBVALIDATION       5U

#define FPIFLAG_SUSPECTED           0x1
#define FPIFLAG_HEURISTIC           (0x2 | FPIFLAG_SUSPECTED)
#define FPIFLAG_INTEGRITY           (0x4 | FPIFLAG_SUSPECTED)
#define FPIFLAG_MSGEXTRA            0x100

#define FPIVI_NONE                  0U
#define FPIVI_INTERNAL              1U
#define FPIVI_EXTERNAL              2U

#define FPIBOOTBLOCK_MBR            1U
#define FPIBOOTBLOCK_BOOT           2U

#define FPIREFDATA_GETMODULEINFORMATION    1U
#define FPIREFDATA_INITIALIZE       2U
#define FPIREFDATA_UNINITIALIZE     3U
// jmk: FPIREFDATA_VALIDATEDATABASES removed in FPI API 1.15.

// Copied from GKAPI.H
// database validation status codes (values with 0x100 cause update to be invalid)
#define FSAV_DB_VALID			0x10		// file is valid for plug-in
#define FSAV_DB_NOTCHECKED		0x11		// not checked
#define FSAV_DB_DELETE			0x12		// delete from runtime directory
#define FSAV_DB_MISSING			0x114	// file required, not available
#define FSAV_DB_INVALID			0x115	// database invalid
#define FSAV_DB_NOTDB			0x116	// does not seem to be a database file
#define FSAV_DB_CORRUPT			0x117	// integrity check failed (/eg. crc)
#define FSAV_DB_VERSION			0x118	// file version incompatible with plug-in version


// Invalid values
#define FPIFILEHANDLE_ERROR         (-1)
#define FPIFILESIZE_ERROR           ((DWORD)0xffffffff)
#define FPIFILEATTR_ERROR           ((DWORD)0xffffffff)

#define FPICONTAINERHANDLE_NULL     (0)

// General error codes

#define FPIERROR_SUCCESS        0
#define FPIERROR_UNKNOWN        1
#define FPIERROR_INIT_MULTIPLE          2
#define FPIERROR_NOTINIT        3
#define FPIERROR_NOMEM          4
#define FPIERROR_NOPGMFILE      5
#define FPIERROR_NODBFILE       6
#define FPIERROR_BADDBFILE      7
#define FPIERROR_FILEOPEN       8
#define FPIERROR_FILEENCRYPTED          9
#define FPIERROR_FILEREAD       10
#define FPIERROR_FILEWRITE      11
#define FPIERROR_SECTORREAD     12
#define FPIERROR_SECTORWRITE            13
#define FPIERROR_WRITEPROTECT           14
#define FPIERROR_NOTSUPPORTED           15
#define FPIERROR_INVALIDPARAMETER       16
#define FPIERROR_WRONGAPI               17
#define FPIERROR_ABORTED		18

#define FPIERROR_LASTERROR      18


// File and sector I/O error codes

#define FPIFILEIO_ERROR_NONE            1000
#define FPIFILEIO_ERROR_UNKNOWN         1001
#define FPIFILEIO_ERROR_NOMEM           1002
#define FPIFILEIO_ERROR_OPENFAILED      1003
#define FPIFILEIO_ERROR_IOERROR         1004
#define FPIFILEIO_ERROR_BADARCHIVE      1005
#define FPIFILEIO_ERROR_NOTINARCHIVE            1006
#define FPIFILEIO_ERROR_BADFILE         1007
#define FPIFILEIO_ERROR_BADHANDLE       1008
#define FPIFILEIO_ERROR_BADARGUMENT     1009
#define FPIFILEIO_ERROR_ENCRYPTED       1010
#define FPIFILEIO_ERROR_READONLY        1011
#define FPIFILEIO_ERROR_DISKWRITEPROT   1012

#define FPIFILEIO_ERROR_LASTERROR       1012

// Function calling convention type definitions

#if defined VTOOLSD
	#define DllExport
	#define Export
#elif defined _MSC_VER
	#ifndef __WIN32__
		#define DllExport
		#define Export        _export
	#else
		#define DllExport   __declspec( dllexport )
		#define Export
	#endif 
#else
	#define DllExport
	#define Export         _export
#endif

#if defined VTOOLSD
	#define FPIFN _cdecl Export
	#define FPIFILEIOFN _cdecl Export
#elif defined __WIN32__
	#define FPIFN _cdecl Export
	#define FPIFILEIOFN _cdecl Export
#elif defined OS2
	#define FPIFN _cdecl Export
	#define FPIFILEIOFN _cdecl Export
#elif defined UNIX
	#define FPIFN
	#define FPIFILEIOFN
#else
	#define FPIFN _far _pascal Export
	#define FPIFILEIOFN _far _pascal Export
#endif

// Forward declarations
struct FPI_DISK_GEOMETRY_struct;


// FPI callback

typedef BOOL  (FPIFN * LPFPICALLBACK) (WORD /*event_*/, WORD /*param_*/, DWORD /*flags_*/, LPCSTR /*message_*/, DWORD /*refdata_*/);


// FPI file and sector I/O callback functions

typedef WORD  (FPIFILEIOFN * LPFPIFILEIOFN_GETLASTERROR) (void);
typedef FPIFILEHANDLE  (FPIFILEIOFN * LPFPIFILEIOFN_OPEN) (FPICONTAINERHANDLE /*parent_*/, LPCSTR /*filename_*/, DWORD /*accessmode_*/, DWORD /*reservedA_*/, DWORD /*reservedB_*/, DWORD /*reservedC_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_READ) (FPIFILEHANDLE /*handle_*/, LPVOID /*buf_*/, DWORD /*bytestoread_*/, LPDWORD /*bytesread_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_WRITE) (FPIFILEHANDLE /*handle_*/, LPCVOID /*buf_*/, DWORD /*bytestowrite_*/, LPDWORD /*byteswritten_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_CLOSE) (FPIFILEHANDLE /*handle_*/);
typedef DWORD  (FPIFILEIOFN * LPFPIFILEIOFN_GETSIZE) (FPIFILEHANDLE /*handle_*/);
typedef DWORD  (FPIFILEIOFN * LPFPIFILEIOFN_GETPOS) (FPIFILEHANDLE /*handle_*/);
typedef DWORD  (FPIFILEIOFN * LPFPIFILEIOFN_SETPOS) (FPIFILEHANDLE /*handle_*/, LONG /*distance_*/, WORD /*fromwhere_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_SETEND) (FPIFILEHANDLE /*handle_*/);
typedef DWORD (FPIFILEIOFN * LPFPIFILEIOFN_GETATTR) (FPIFILEHANDLE /*handle_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_RWBOOTBLOCK) (WORD /*drive_*/, WORD /*blocktype_*/, BOOL /*write_*/, LPVOID /*buf_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_RWSECTORPHYS) (WORD /*physdrive_*/, WORD /*head_*/, WORD /*cylinder_*/, WORD /*physsector_*/, BOOL /*write_*/, LPVOID /*buf_*/, WORD /*size_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_RWSECTORLOGC) (WORD /*drive_*/, DWORD /*sector_*/, BOOL /*write_*/, LPVOID /*buf_*/, WORD /*size_*/);
typedef BOOL  (FPIFILEIOFN * LPFPIFILEIOFN_GETDRIVEGEOMETRY) (WORD /*drive_*/, struct FPI_DISK_GEOMETRY_struct* /*geom_*/);


// Structures

typedef struct FPIMODULEINFO_struct
{
    WORD    fStructSize;    // size of the structure
    WORD    fModuleID;  // module ID
    CHAR    fModuleName [FPIMAX_MODULENAME];    // module name
    CHAR    fVendorName [FPIMAX_VENDORNAME];    // vendor name
    WORD    fVerMajor;  // major version number
    WORD    fVerMinor;  // minor version number
    WORD    fVerBuild;  // build number
    WORD    fDatabaseYear;  // signatures database year
    BYTE    fDatabaseMonth; // database month (Jan=1 etc)
    BYTE    fDatabaseDay;   // database day of the month
}
FPIMODULEINFO, FAR * LPFPIMODULEINFO;

typedef struct FPIFILEIOFNTABLE_struct
{
    DWORD   fFunctionCount; // function pointers counter
    LPFPIFILEIOFN_GETLASTERROR  fFnGetLastError;    // FPIFILEIOFN_GETLASTERROR
    LPFPIFILEIOFN_OPEN  fFnOpen;    // FPIFILEIOFN_OPEN
    LPFPIFILEIOFN_READ  fFnRead;    // FPIFILEIOFN_READ
    LPFPIFILEIOFN_WRITE fFnWrite;   // FPIFILEIOFN_WRITE
    LPFPIFILEIOFN_CLOSE fFnClose;   // FPIFILEIOFN_CLOSE
    LPFPIFILEIOFN_GETSIZE   fFnGetSize; // FPIFILEIOFN_GETSIZE
    LPFPIFILEIOFN_GETPOS    fFnGetPos;  // FPIFILEIOFN_GETPOS
    LPFPIFILEIOFN_SETPOS    fFnSetPos;  // FPIFILEIOFN_SETPOS
    LPFPIFILEIOFN_SETEND    fFnSetEnd;  // FPIFILEIOFN_SETEND
    LPFPIFILEIOFN_GETATTR   fFnGetAttr; // FPIFILEIOFN_GETATTR
    LPFPIFILEIOFN_RWBOOTBLOCK   fFnRWBootBlock; // FPIFILEIOFN_RWBOOTBLOCK
    LPFPIFILEIOFN_RWSECTORPHYS  fFnRWSectorPhys;    // FPIFILEIOFN_RWSECTORPHYS
    LPFPIFILEIOFN_RWSECTORLOGC  fFnRWSectorLogc;    // FPIFILEIOFN_RWSECTORLOGC
    LPFPIFILEIOFN_GETDRIVEGEOMETRY  fFnGetDriveGeometry;    // FPIFILEIOFN_GETDRIVEGEOMETRY
}
FPIFILEIOFNTABLE, FAR * LPFPIFILEIOFNTABLE, FAR * const LPCFPIFILEIOFNTABLE;

// Note: This is modeled after the Win32 type MEDIA_TYPE.
typedef enum FPI_MEDIA_TYPE_enum {
    FPI_Unknown,                // Format is unknown
    FPI_F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    FPI_F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    FPI_F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    FPI_F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    FPI_F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    FPI_F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    FPI_F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    FPI_F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    FPI_F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    FPI_F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    FPI_RemovableMedia,         // Removable media other than floppy
    FPI_FixedMedia,             // Fixed hard disk media
    FPI_F3_120M_512             // 3.5", 120M Floppy
} FPI_MEDIA_TYPE;

// Note: This is modeled after the Win32 struct DISK_GEOMETRY.
typedef struct FPI_DISK_GEOMETRY_struct
{ 
   LARGE_INTEGER  Cylinders; 
   FPI_MEDIA_TYPE  MediaType; 
   DWORD  TracksPerCylinder; 
   DWORD  SectorsPerTrack; 
   DWORD  BytesPerSector; 
} FPI_DISK_GEOMETRY, FAR * LPFPI_DISK_GEOMETRY;


// General and scanning functions (typedefs defined for convenience of ODS development)

DllExport BOOL FPIFN FPI_GetModuleInformation (DWORD apiversion_, LPFPIMODULEINFO moduleinfo_, LPCFPIFILEIOFNTABLE fileiotable_, LPFPICALLBACK reportcallback_);
typedef BOOL (FPIFN * LPFPI_GetModuleInformation) (DWORD /*apiversion_*/, LPFPIMODULEINFO /*moduleinfo_*/, LPCFPIFILEIOFNTABLE /*fileiotable_*/, LPFPICALLBACK /*reportcallback_*/);

DllExport BOOL FPIFN FPI_Initialize ();
typedef BOOL (FPIFN * LPFPI_Initialize) ();

DllExport BOOL FPIFN FPI_Uninitialize ();
typedef BOOL (FPIFN * LPFPI_Uninitialize) ();

DllExport BOOL FPIFN FPI_ValidateDatabases (LPCSTR /*dbdir_*/, DWORD refdata_);
typedef BOOL (FPIFN * LPFPI_ValidateDatabases) (LPCSTR /*dbdir_*/, DWORD /*refdata_*/);

// jmk000114 {
DllExport BOOL FPIFN FPI_ReportScanningStatus (void);
typedef BOOL (FPIFN * LPFPI_ReportScanningStatus) (void);

DllExport void FPIFN FPI_ReloadDatabases (void);
typedef void (FPIFN * LPFPI_ReloadDatabases) (void);

DllExport void FPIFN FPI_AutoReloadOff (void);
typedef void (FPIFN * LPFPI_AutoReloadOff) (void);

DllExport void FPIFN FPI_AutoReloadOn (void);
typedef void (FPIFN * LPFPI_AutoReloadOn) (void);
// jmk000114 }

//urr2000-02-09{ new function in API ver 1.16
DllExport void FPIFN FPI_CustomSettings (LPCSTR settings_);
typedef void (FPIFN * LPFPI_CustomSettings) (LPCSTR);
//urr2000-02-09} new function in API ver 1.16

DllExport BOOL FPIFN FPI_ScanFile (LPCSTR filename_, FPICONTAINERHANDLE parent_, WORD action_, DWORD refdata_);
typedef BOOL (FPIFN * LPFPI_ScanFile) (LPCSTR /*filename_*/, FPICONTAINERHANDLE /*parent_*/, WORD /*action_*/, DWORD /*refdata_*/);

DllExport BOOL FPIFN FPI_ScanMemory (WORD action_, DWORD refdata_);
typedef BOOL (FPIFN * LPFPI_ScanMemory) (WORD /*action_*/, DWORD /*refdata_*/);

DllExport BOOL FPIFN FPI_ScanBootBlock (WORD drive_, WORD blocktype_, WORD blocksize_, WORD action_, DWORD refdata_);
typedef BOOL (FPIFN * LPFPI_ScanBootBlock) (WORD /*drive_*/, WORD /*blocktype_*/, WORD /*blocksize_*/, WORD /*action_*/, DWORD /*refdata_*/);

//turbpa2001-10-24#18515{
DllExport DWORD FPIFN FPI_QuerySignatureCount (void);
typedef DWORD (FPIFN * LPFPI_QuerySignatureCount) (void);
//turbpa2001-10-24#18515}


// Virus description information functions

#if !defined(NT_DRIVER) && !defined(VTOOLSD) && !defined(UNIX)
DllExport WORD FPIFN FPIVI_GetInterfaceType (void);
DllExport UINT FPIFN FPIVI_InternalGetIndexCount (void);
DllExport UINT FPIFN FPIVI_InternalGetName (UINT index_, LPSTR name_, UINT namelen_);
DllExport HGLOBAL FPIFN FPIVI_InternalGetDescription (UINT index_);
DllExport void FPIFN FPIVI_ExternalShowDescription (HWND hwnd_, LPCSTR name_);
#endif // !NT_DRIVER && !VTOOLSD && !UNIX


#ifdef __cplusplus
}
#endif

#endif // __FPIAPI_H

#endif // FPIAPI_H

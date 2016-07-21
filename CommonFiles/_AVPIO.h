////////////////////////////////////////////////////////////////////
//
//  _AVPIO.H
//  AVP 3.0 Internal IO prototypes definition and declaration
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef ___AVPIO_H
#define ___AVPIO_H

#ifndef __TCHAR_DEFINED
typedef char            _TCHAR;
typedef signed char     _TSCHAR;
typedef unsigned char   _TUCHAR;
typedef unsigned char   _TXCHAR;
typedef unsigned int    _TINT;
#define __TCHAR_DEFINED
#endif

#ifndef _TCHAR_DEFINED
#if     !__STDC__
typedef char            TCHAR;
#endif
#define _TCHAR_DEFINED
#endif

#ifndef _WINBASE_
#define _WINBASE_
#if !(defined (__unix__))
  #define WINAPI __stdcall
#else
  #if defined (BASEWORK_4_PRAGUE)
    #define WINAPI *
  #else
    #define WINAPI
  #endif
#endif

#ifdef PRAGUE
#if !defined (__unix__)
	#define MAX_PATH 260
	#define FALSE cFALSE
	#define TRUE cTRUE
#endif
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100  
#define EXCEPTION_EXECUTE_HANDLER       1
#define EXCEPTION_CONTINUE_SEARCH       0
#define EXCEPTION_CONTINUE_EXECUTION    -1
#define BYTE tBYTE
#define CHAR tCHAR 
#define INT tINT
#define UINT tUINT
#define WORD tWORD
#define DWORD tDWORD 
#define LONG tLONG
#define ULONG tDWORD
#define BOOL tINT
#define FAR
#define PLONG tLONG*
#define LRESULT tLONG
#define LPBYTE tBYTE*
#define LPWORD tWORD*
#define LPDWORD tDWORD*
#define LPVOID tVOID*
#define LPCVOID const tVOID*
#define HANDLE tVOID*
#define LPCTSTR const tCHAR*
#define LPTSTR  tCHAR*
#define PCHAR  tCHAR*
#define VOID  tVOID
#ifndef __OVERLAPPED_DEFINED
	typedef struct _OVERLAPPED {
    	DWORD   Internal;
	    DWORD   InternalHigh;
    	DWORD   Offset;
    	DWORD   OffsetHigh;
    	HANDLE  hEvent;
	} OVERLAPPED, *LPOVERLAPPED;
	#define __OVERLAPPED_DEFINED
#endif	
#endif //PRAGUE


#ifdef _NTDDK_
#define DWORD ULONG 
#define WORD USHORT 
#define BOOL int
#define FAR
#define UINT unsigned int
#define BYTE unsigned char
#define INT int
#define LRESULT LONG
#define LPBYTE BYTE*
#define LPWORD WORD*
#define LPDWORD DWORD*
#define LPVOID void*
#define LPCVOID const void*
#ifndef __OVERLAPPED_DEFINED
	typedef struct _OVERLAPPED {
    	DWORD   Internal;
    	DWORD   InternalHigh;
    	DWORD   Offset;
    	DWORD   OffsetHigh;
    	HANDLE  hEvent;
	} OVERLAPPED, *LPOVERLAPPED;
	#define __OVERLAPPED_DEFINED
#endif	
#endif//_NTDDK_

#ifdef __MWERKS__
#include "ntypedef.h"
#endif//__MWERKS__

#ifndef WPARAM
#define WPARAM unsigned int
#endif

#ifndef LPARAM
#define LPARAM DWORD
#endif

#ifndef LPBYTE
#define LPBYTE BYTE FAR*
#define LPWORD WORD FAR*
#define LPDWORD DWORD FAR*
#define LPVOID void FAR*
#define LPCVOID const void FAR*
#endif

#define _SECURITY_ATTRIBUTES_
typedef struct  _SECURITY_ATTRIBUTES
    {
    DWORD nLength;
    /* [size_is] */ LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
    }   SECURITY_ATTRIBUTES;

typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR   szCSDVersion[ 128 ];       // Maintenance string for PSS usage
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;

typedef struct _SECURITY_ATTRIBUTES FAR *PSECURITY_ATTRIBUTES;
typedef struct _SECURITY_ATTRIBUTES FAR *LPSECURITY_ATTRIBUTES;

#endif //_WINBASE_

#ifndef GENERIC_READ
#define GENERIC_READ            0x80000000L
#define GENERIC_WRITE           0x40000000L
#define GENERIC_EXECUTE         0x20000000L
#define GENERIC_ALL             0x10000000L

#define FILE_SHARE_READ         0x00000001
#define FILE_SHARE_WRITE        0x00000002
#define FILE_SHARE_DELETE       0x00000004
#endif

#ifndef CREATE_NEW
#define CREATE_NEW              0x00000001
#define CREATE_ALWAYS           0x00000002
#define OPEN_EXISTING           0x00000003
#define OPEN_ALWAYS             0x00000004
#define TRUNCATE_EXISTING       0x00000005
#define INVALID_HANDLE_VALUE    ((HANDLE)-1)
#endif

#ifndef FILE_FLAG_WRITE_THROUGH
#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000
#endif

#ifndef FILE_BEGIN
#define FILE_BEGIN           0
#endif

#ifndef FILE_CURRENT
#define FILE_CURRENT         1
#endif

#ifndef FILE_END
#define FILE_END             2
#endif

#ifndef FILE_ATTRIBUTE_TEMPORARY
#define FILE_ATTRIBUTE_TEMPORARY 0x00000100
#endif


typedef LRESULT (WINAPI t_AvpCallback)(WPARAM wParam,LPARAM lParam);

typedef _TCHAR* (WINAPI t_AvpStrrchr)(_TCHAR* str_,INT chr_);
typedef _TCHAR* (WINAPI t_AvpStrlwr)(_TCHAR* str_);
typedef _TCHAR* (WINAPI t_AvpConvertChar)(_TCHAR* str_,INT ct,_TCHAR* cp_);

typedef BOOL (WINAPI t_AvpRead13 )(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, LPBYTE Buffer);
typedef BOOL (WINAPI t_AvpWrite13)(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, LPBYTE Buffer);
typedef BOOL (WINAPI t_AvpRead25)(
        BYTE Drive,
        DWORD Sector,
        WORD NumSectors,
        LPBYTE Buffer
        );

typedef BOOL (WINAPI t_AvpWrite26)(
        BYTE Drive,
        DWORD Sector,
        WORD NumSectors,
        LPBYTE Buffer
        );

typedef BOOL (WINAPI t_AvpReadFile )(
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,        // number of bytes to read
    LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
    OVERLAPPED FAR*  lpOverlapped       // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
        );

typedef BOOL (WINAPI t_AvpWriteFile )(
    HANDLE  hFile,      // handle to file to write to
    LPCVOID  lpBuffer,  // pointer to data to write to file
    DWORD  nNumberOfBytesToWrite,       // number of bytes to write
    LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
        );

typedef BOOL (WINAPI t_AvpCloseHandle)(
    HANDLE  hObject     // handle to object to close
        );

typedef DWORD (WINAPI t_AvpSetFilePointer)(
    HANDLE  hFile,      // handle of file
    LONG  lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh,        // address of high-order word of distance to move
    DWORD  dwMoveMethod         // how to move
        );

typedef BOOL (WINAPI t_AvpSetEndOfFile)(
    HANDLE  hFile       // handle of file
        );

typedef DWORD (WINAPI t_AvpGetDosMemSize )(
        void
        );

typedef DWORD (WINAPI t_AvpGetFirstMcbSeg )(
        void
        );

typedef DWORD (WINAPI t_AvpGetIfsApiHookTable )(
        DWORD* table,
        DWORD size //size in DWORDS !!!!!!!!
        );

typedef DWORD (WINAPI t_AvpGetDosTraceTable )(
        DWORD* table,
        DWORD size //size in DWORDS !!!!!!!!
        );

typedef DWORD (WINAPI t_AvpMemoryRead )(
    DWORD   dwOffset,     // offset
    DWORD  dwSize,      // size in bytes
    LPBYTE  lpBuffer    // pointer to buffer to read to
        );

typedef DWORD (WINAPI t_AvpMemoryWrite )(
    DWORD   dwOffset,     // offset
    DWORD  dwSize,      // size in bytes
    LPBYTE  lpBuffer    // pointer to buffer to write from
        );

typedef BOOL (WINAPI t_AvpGetDiskParam )(
    BYTE   disk,
    BYTE   drive,
    WORD*   CX,
    BYTE*       DH
        );

typedef DWORD (WINAPI t_AvpGetFileSize)(
    HANDLE  hFile,      // handle of file
    LPDWORD  lpHigh
    );
    
typedef DWORD (WINAPI t_AvpGetFullPathName)(
        LPCTSTR lpFileName,     // address of name of file to find path for
    DWORD nBufferLength,        // size, in characters, of path buffer
    LPTSTR lpBuffer,    // address of path buffer
    LPTSTR *lpFilePart  // address of filename in path
        );
 
typedef BOOL (WINAPI t_AvpDeleteFile)(
        LPCTSTR lpFileName      // pointer to name of file to delete
        );

typedef HANDLE (WINAPI t_AvpCreateFile )(
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD  dwDesiredAccess,     // access (read-write) mode
    DWORD  dwShareMode, // share mode
    SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
    DWORD  dwCreationDistribution,      // how to create
    DWORD  dwFlagsAndAttributes,        // file attributes
    HANDLE  hTemplateFile       // handle to file with attributes to copy
        );

//additional
typedef BOOL (WINAPI t_AvpGetDiskFreeSpace)(
	LPCTSTR lpRootPathName, // address of root path
    LPDWORD	lpSectorsPerCluster,        // address of sectors per cluster
    LPDWORD lpBytesPerSector,   // address of bytes per sector
    LPDWORD lpNumberOfFreeClusters,     // address of number of free clusters
    LPDWORD lpTotalNumberOfClusters     // address of total number of clusters
        );

/*
typedef BOOL (WINAPI t_AvpMoveFile)(
        LPCTSTR lpExistingFileName,     // address of name of the existing file
    LPCTSTR lpNewFileName       // address of new name for the file
        );
*/

typedef BOOL (WINAPI t_AvpSetFileAttributes)(
        LPCTSTR lpFileName,     // address of filename
    DWORD dwFileAttributes      // address of attributes to set
        );

typedef DWORD (WINAPI t_AvpGetFileAttributes)(
        LPCTSTR lpFileName      // address of the name of a file or directory
        );

typedef DWORD (WINAPI t_AvpGetTempPath)(
        DWORD nBufferLength,    // size, in characters, of the buffer
    LPTSTR lpBuffer     // address of buffer for temp. path
        );

typedef UINT (WINAPI t_AvpGetTempFileName)(
        LPCTSTR lpPathName,     // address of directory name for temporary file
    LPCTSTR lpPrefixString,     // address of filename prefix
    UINT uUnique,       // number used to create temporary filename
    LPTSTR lpTempFileName       // address of buffer that receives the new filename
        );

typedef DWORD (WINAPI t_AvpExecSpecial)(
		char* FuncName, 
		DWORD wParam, 
		DWORD lParam
		);


#ifdef __cplusplus
#    define EXTERN extern "C"
#else
#    define EXTERN extern
#endif


#ifdef AVPIOSTATIC
//WINAPI _dumm(){return 0;}
//#   define F_INIT(x) =(x)_dumm
#   define F_INIT(x) = NULL
#else
#       define F_INIT(x) 
 #endif


#ifdef _MSC_VER
 #define declare(x)\
 EXTERN t_Avp##x _Avp##x;\
 EXTERN t_Avp##x Cache_Avp##x;\
 EXTERN t_Avp##x * Avp##x F_INIT(t_Avp##x *);
#else //Watcom
 #if (defined(SYSTOS2) || defined(__unix__))
    #define declare(x)                            \
    EXTERN t_Avp##x Avp##x;
//    EXTERN t_Avp##x * _Avp##x F_INIT(t_Avp##x *);
//  #define declare(x)    EXTERN t_Avp##x Avp##x;   EXTERN t_Avp##x * _Avp##x F_INIT(t_Avp##x *);
 #else
 	#ifdef __MWERKS__
		#ifdef AVPIOSTATIC
			#define declare(x)\
	 		EXTERN t_Avp##x _Avp##x;\
			EXTERN t_Avp##x Cache_Avp##x;\
			extern "C" t_Avp##x * Avp##x = NULL; 
		#else
			#define declare(x)\
 			EXTERN t_Avp##x _Avp##x;\
			EXTERN t_Avp##x Cache_Avp##x;\
			EXTERN t_Avp##x * Avp##x; 
		#endif
 	#else
 	  	#define declare(x)\
	   	EXTERN t_Avp##x Avp##x;\
		EXTERN t_Avp##x * _Avp##x F_INIT(t_Avp##x *);
 	#endif
  //#define d_declare(x)
 #endif
 #if defined (__unix__) && defined (BASEWORK_4_PRAGUE)

    #undef declare
    #define declare(x) \
    EXTERN t_Avp##x Avp##x;
    #undef WINAPI
    #define WINAPI
 #endif
#endif

declare( Callback )
declare( Strrchr )
declare( Strlwr )
declare( ConvertChar )

declare( CreateFile )
declare( CloseHandle )
declare( ReadFile )
declare( WriteFile )
declare( GetFileSize )
declare( SetFilePointer )
declare( SetEndOfFile )
declare( GetDiskFreeSpace )
declare( GetFullPathName )
declare( DeleteFile )
//declare( MoveFile )
declare( GetFileAttributes )
declare( SetFileAttributes )
declare( GetTempPath )
declare( GetTempFileName )

declare( MemoryRead )
declare( MemoryWrite )
declare( GetDosMemSize )
declare( GetFirstMcbSeg )
declare( GetIfsApiHookTable )
declare( GetDosTraceTable )
declare( GetDiskParam )
declare( Read13 )
declare( Write13 )
declare( Read25 )
declare( Write26 )
declare( ExecSpecial )

EXTERN BOOL LoadIO(void);

#define AvpIo_Callback          1
#define AvpIo_Strrchr           2
#define AvpIo_Strlwr            3
#define AvpIo_ConvertChar       4

#define AvpIo_CreateFile        6
#define AvpIo_CloseHandle       7
#define AvpIo_ReadFile          8
#define AvpIo_WriteFile         9
#define AvpIo_GetFileSize       10
#define AvpIo_SetFilePointer    11
#define AvpIo_SetEndOfFile      12
#define AvpIo_GetDiskFreeSpace  13
#define AvpIo_GetFullPathName   14
#define AvpIo_DeleteFile        15
//#define AvpIo_MoveFile                16
#define AvpIo_GetFileAttributes 17
#define AvpIo_SetFileAttributes 18
#define AvpIo_GetTempPath               19
#define AvpIo_GetTempFileName   20

#define AvpIo_MemoryRead                21
#define AvpIo_MemoryWrite               22
#define AvpIo_GetDosMemSize             23
#define AvpIo_GetFirstMcbSeg    24
#define AvpIo_GetIfsApiHookTable        25
#define AvpIo_GetDosTraceTable  26
#define AvpIo_GetDiskParam              27
#define AvpIo_Read13                    28
#define AvpIo_Write13                   29
#define AvpIo_Read25                    30
#define AvpIo_Write26                   31


#endif //___AVPIO_H

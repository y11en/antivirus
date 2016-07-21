#ifndef _COMPAT_H_
#define _COMPAT_H_

#if defined (_REENTRANT) || defined (_THREAD_SAFE) || ( defined (__OpenBSD__) && defined(_POSIX_THREADS))
#define THREADS
#include <pthread.h>
#endif

#include <Serialize/KLDTDefs.h>
#include <_AVPIO.h>

#define APIENTRY  __attribute__((stdcall))
#define WINBASEAPI
#define HGLOBAL  HANDLE
#define HRSRC    HANDLE
#define PVOID VOID*

#define lstrcmp strcmp
#define lstrcmpi strcasecmp
#define lstrlen strlen
#define wsprintf sprintf
#define wcsicmp wcscasecmp
#define strcmpi strcasecmp
#define strnicmp strncasecmp
#define lstrcpy strcpy
#define wvsprintf vsprintf
#define wcsnicmp wcsncasecmp

#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

#define ZeroMemory(pb,cb)           memset((pb),0,(cb))

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define LPHANDLE HANDLE*
#define LONGLONG long long
#define HMODULE  HANDLE
#define HINSTANCE HANDLE
#define USHORT unsigned short

typedef struct  _GUID
    {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[ 8 ];
    }	GUID;

typedef GUID IID;
typedef IID* LPIID;
typedef GUID CLSID;
typedef DWORD LCID;
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;


#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define _T(a) (a)
#define TCHAR char

#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)  
#define WAIT_FAILED (DWORD)0xFFFFFFFF
#define WAIT_OBJECT_0       ((STATUS_WAIT_0 ) + 0 )

#define WAIT_ABANDONED         ((STATUS_ABANDONED_WAIT_0 ) + 0 )
#define WAIT_ABANDONED_0       ((STATUS_ABANDONED_WAIT_0 ) + 0 )

#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define WAIT_TIMEOUT                        STATUS_TIMEOUT

#define HEAP_NO_SERIALIZE               0x00000001      
#define HEAP_GROWABLE                   0x00000002  
#define HEAP_ZERO_MEMORY                0x00000008      
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010      

#define SEMAPHORE_ALL_ACCESS 0x00
#define EVENT_ALL_ACCESS 0x00
#define MUTEX_ALL_ACCESS 0x00

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

#define INVALID_FILE_SIZE (DWORD)0xFFFFFFFF

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

#define FILE_SHARE_READ                 0x00000001  
#define FILE_SHARE_WRITE                0x00000002  
#define FILE_SHARE_DELETE               0x00000004  
#define FILE_ATTRIBUTE_READONLY             0x00000001  
#define FILE_ATTRIBUTE_HIDDEN               0x00000002  
#define FILE_ATTRIBUTE_SYSTEM               0x00000004  
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020  
#define FILE_ATTRIBUTE_ENCRYPTED            0x00000040  
#define FILE_ATTRIBUTE_NORMAL               0x00000080  
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100  
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800  
#define FILE_ATTRIBUTE_OFFLINE              0x00001000  
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  
#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001   
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002   
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004   
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008   
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010   
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020   
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040   
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100   
#define FILE_ACTION_ADDED                   0x00000001   
#define FILE_ACTION_REMOVED                 0x00000002   
#define FILE_ACTION_MODIFIED                0x00000003   
#define FILE_ACTION_RENAMED_OLD_NAME        0x00000004   
#define FILE_ACTION_RENAMED_NEW_NAME        0x00000005   
#define MAILSLOT_NO_MESSAGE             ((DWORD)-1) 
#define MAILSLOT_WAIT_FOREVER           ((DWORD)-1) 
#define FILE_CASE_SENSITIVE_SEARCH      0x00000001  
#define FILE_CASE_PRESERVED_NAMES       0x00000002  
#define FILE_UNICODE_ON_DISK            0x00000004  
#define FILE_PERSISTENT_ACLS            0x00000008  
#define FILE_FILE_COMPRESSION           0x00000010  
#define FILE_VOLUME_QUOTAS              0x00000020  
#define FILE_SUPPORTS_SPARSE_FILES      0x00000040  
#define FILE_SUPPORTS_REPARSE_POINTS    0x00000080  
#define FILE_SUPPORTS_REMOTE_STORAGE    0x00000100  
#define FILE_VOLUME_IS_COMPRESSED       0x00008000  
#define FILE_SUPPORTS_OBJECT_IDS        0x00010000  
#define FILE_SUPPORTS_ENCRYPTION        0x00020000  

#define PAGE_READONLY          0x02     
#define PAGE_READWRITE         0x04     
#define PAGE_WRITECOPY         0x08     


#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010
#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)
#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)


#define FILE_MAP_COPY       SECTION_QUERY
#define FILE_MAP_WRITE      SECTION_MAP_WRITE
#define FILE_MAP_READ       SECTION_MAP_READ
#define FILE_MAP_ALL_ACCESS SECTION_ALL_ACCESS


// Directory Entries

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor


typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    WORD   e_magic;                     // Magic number
    WORD   e_cblp;                      // Bytes on last page of file
    WORD   e_cp;                        // Pages in file
    WORD   e_crlc;                      // Relocations
    WORD   e_cparhdr;                   // Size of header in paragraphs
    WORD   e_minalloc;                  // Minimum extra paragraphs needed
    WORD   e_maxalloc;                  // Maximum extra paragraphs needed
    WORD   e_ss;                        // Initial (relative) SS value
    WORD   e_sp;                        // Initial SP value
    WORD   e_csum;                      // Checksum
    WORD   e_ip;                        // Initial IP value
    WORD   e_cs;                        // Initial (relative) CS value
    WORD   e_lfarlc;                    // File address of relocation table
    WORD   e_ovno;                      // Overlay number
    WORD   e_res[4];                    // Reserved words
    WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
    WORD   e_oeminfo;                   // OEM information; e_oemid specific
    WORD   e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
    DWORD   BaseOfData;

    //
    // NT additional fields.
    //

    DWORD   ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_OPTIONAL_HEADER IMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_EXPORT_DIRECTORY {
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   Name;
    DWORD   Base;
    DWORD   NumberOfFunctions;
    DWORD   NumberOfNames;
    DWORD   AddressOfFunctions;     // RVA from base of image
    DWORD   AddressOfNames;         // RVA from base of image
    DWORD   AddressOfNameOrdinals;  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct _FIXED {
#ifndef _MAC
    WORD    fract;
    short   value;
#else
    short   value;
    WORD    fract;
#endif
} FIXED;

#define IMAGE_SCN_MEM_WRITE                 0x80000000  // Section is writeable.
#define IMAGE_DOS_SIGNATURE                 0x5A4D      // MZ
#define IMAGE_OS2_SIGNATURE                 0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE              0x454C      // LE
#define IMAGE_NT_SIGNATURE                  0x00004550  // PE00

typedef unsigned long UINT_PTR, *PUINT_PTR;
typedef long INT_PTR, *PINT_PTR;

#define FIELD_OFFSET(type, field)    ((LONG)(INT_PTR)&(((type *)0)->field))

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((UINT_PTR)ntheader +                                                  \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((PIMAGE_NT_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))


typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

#ifndef MAX_PATH
#include <limits.h>
#if defined(__OpenBSD__) || defined(__FreeBSD__)
#include <sys/syslimits.h> 
#endif
#define MAX_PATH PATH_MAX
#endif

#define LOAD_LIBRARY_AS_DATAFILE        0x00000002
#define DLL_PROCESS_ATTACH 1    
#define DLL_THREAD_ATTACH  2    
#define DLL_THREAD_DETACH  3    
#define DLL_PROCESS_DETACH 0  

#define MAKEINTRESOURCE(i) (LPSTR)((DWORD)((WORD)(i)))
#define RT_RCDATA           MAKEINTRESOURCE(10)

typedef struct _WIN32_FINDDATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR   cFileName[ MAX_PATH ];
    CHAR   cAlternateFileName[ 14 ];
} WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA, WIN32_FIND_DATAW, *PWIN32_FIND_DATAW;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _BY_HANDLE_FILE_INFORMATION {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

typedef struct _SYSTEM_INFO {
/*     union { */
/*         DWORD dwOemId;          // Obsolete field...do not use */
/*         struct { */
/*             WORD wProcessorArchitecture; */
/*             WORD wReserved; */
/*         }; */
/*     }; */
    DWORD dwPageSize;
/*     LPVOID lpMinimumApplicationAddress; */
/*     LPVOID lpMaximumApplicationAddress; */
/*     DWORD dwActiveProcessorMask; */
/*     DWORD dwNumberOfProcessors; */
/*     DWORD dwProcessorType; */
/*     DWORD dwAllocationGranularity; */
/*     WORD wProcessorLevel; */
/*     WORD wProcessorRevision; */
} SYSTEM_INFO, *LPSYSTEM_INFO;

#define VER_PLATFORM_WIN32s             0
#define VER_PLATFORM_WIN32_WINDOWS      1
#define VER_PLATFORM_WIN32_NT           2

typedef struct _OSVERSIONINFO {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR   szCSDVersion[ 128 ];     // Maintenance string for PSS usage
} OSVERSIONINFO, *POSVERSIONINFO, *LPOSVERSIONINFO;

#if defined (__cplusplus)
extern "C" {
#endif

#if defined (THREADS)
typedef pthread_mutex_t CRITICAL_SECTION;
typedef CRITICAL_SECTION* LPCRITICAL_SECTION;
// critical section functions
void InitializeCriticalSection ( LPCRITICAL_SECTION lpCriticalSection);
void DeleteCriticalSection ( LPCRITICAL_SECTION lpCriticalSection );
void EnterCriticalSection ( LPCRITICAL_SECTION lpCriticalSection ) ;
BOOL TryEnterCriticalSection ( LPCRITICAL_SECTION lpCriticalSection ) ;
void LeaveCriticalSection ( LPCRITICAL_SECTION lpCriticalSection );
#endif
// handle functions

BOOL CloseHandle ( HANDLE hObject );

BOOL DuplicateHandle ( HANDLE hSourceProcessHandle,
                       HANDLE hSourceHandle,
                       HANDLE hTargetProcessHandle,
                       LPHANDLE lpTargetHandle,
                       DWORD dwDesiredAccess,
                       BOOL bInheritHandle,
                       DWORD dwOptions );

BOOL GetHandleInformation ( HANDLE hObject, LPDWORD lpdwFlags );

BOOL SetHandleInformation ( HANDLE hObject, 
                            DWORD dwMask, 
                            DWORD dwFlags );


// mutex functions  
HANDLE OpenMutex ( DWORD dwDesiredAccess,
                   BOOL bInheritHandle,
                   const char* lpName );

HANDLE CreateMutex( void* lpMutexAttributes,
                    BOOL bInitialOwner,
                    const char* lpName );

BOOL ReleaseMutex ( HANDLE hMutex );


// event functions
HANDLE CreateEvent( void* lpEventAttributes,
                    BOOL bManualReset,
                    BOOL bInitialState,
                    const char* lpName );

HANDLE OpenEvent ( DWORD dwDesiredAccess, 
                   BOOL bInheritHandle, 
                   const char* lpName );

BOOL ResetEvent( HANDLE hEvent );

BOOL SetEvent ( HANDLE hEvent );

BOOL PulseEvent ( HANDLE hEvent );

// semaphore functions
HANDLE CreateSemaphore ( void* lpSemaphoreAttributes,
                         LONG lInitialCount,
                         LONG lMaximumCount,
                         const char* lpName );

HANDLE OpenSemaphore ( DWORD dwDesiredAccess,
                       BOOL bInheritHandle,
                       const char* lpName );

BOOL ReleaseSemaphore ( HANDLE hSemaphore,
                        LONG lReleaseCount,
                        LONG* lpPreviousCount );

// wait functions

DWORD WaitForSingleObject ( HANDLE hHandle,
                            DWORD dwMilliseconds );

DWORD WaitForMultipleObjects( DWORD nCount,
                              HANDLE *lpHandles,
                              BOOL bWaitAll,
                              DWORD dwMilliseconds );

// thread functions

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

HANDLE GetCurrentThread ( VOID );

DWORD GetCurrentThreadId( VOID );

HANDLE CreateThread ( void* lpThreadAttributes,
                      DWORD dwStackSize,
                      LPTHREAD_START_ROUTINE lpStartAddress,
                      LPVOID lpParameter,
                      DWORD dwCreationFlags,
                      LPDWORD lpThreadId );


// file finctions

HANDLE CreateFile ( const char* lpFileName,
                    DWORD dwDesiredAccess,
                    DWORD dwShareMode,
                    void* lpSecurityAttributes,
                    DWORD dwCreationDisposition,
                    DWORD dwFlagsAndAttributes,
                    HANDLE hTemplateFile );

BOOL ReadFile ( HANDLE hFile,
                LPVOID lpBuffer,
                DWORD nNumberOfBytesToRead,
                LPDWORD lpNumberOfBytesRead,
                LPOVERLAPPED lpOverlapped );

BOOL WriteFile ( HANDLE hFile,
                 LPCVOID lpBuffer,
                 DWORD nNumberOfBytesToWrite,
                 LPDWORD lpNumberOfBytesWritten,
                 LPOVERLAPPED lpOverlapped );

DWORD SetFilePointer ( HANDLE hFile,
                       LONG lDistanceToMove,
                       PLONG lpDistanceToMoveHigh,
                       DWORD dwMoveMethod );

DWORD GetFileSize ( HANDLE hFile,
                    LPDWORD lpFileSizeHigh );

BOOL SetEndOfFile ( HANDLE hFile );

BOOL GetFileTime ( HANDLE hFile,
                   LPFILETIME lpCreationTime,
                   LPFILETIME lpLastAccessTime,
                   LPFILETIME lpLastWriteTime );

BOOL SetFileTime ( HANDLE hFile,
                   const FILETIME *lpCreationTime,
                   const FILETIME *lpLastAccessTime,
                   const FILETIME *lpLastWriteTime );

BOOL GetFileInformationByHandle ( HANDLE hFile,
                                  LPBY_HANDLE_FILE_INFORMATION lpFileInformation );

BOOL DeleteFile ( const LPCSTR lpFileName );

BOOL CopyFile ( LPCSTR lpExistingFileName,
                LPCSTR lpNewFileName,
                BOOL bFailIfExists );

BOOL MoveFile ( LPCSTR lpExistingFileName,
                LPCSTR lpNewFileName );

HANDLE FindFirstFile ( const char* lpFileName,
                       LPWIN32_FIND_DATA lpFindFileData );

BOOL FindNextFile ( HANDLE hFindFile,
                    LPWIN32_FIND_DATA lpFindFileData );

BOOL FindClose ( HANDLE hFindFile );

BOOL SystemTimeToFileTime ( const SYSTEMTIME *lpSystemTime,
                            LPFILETIME lpFileTime );

BOOL FileTimeToSystemTime ( const FILETIME *lpFileTime,
                            LPSYSTEMTIME lpSystemTime );

HANDLE CreateFileMapping ( HANDLE hFile,
                           void* lpFileMappingAttributes,
                           DWORD flProtect,
                           DWORD dwMaximumSizeHigh,
                           DWORD dwMaximumSizeLow,
                           LPCSTR lpName );

HANDLE OpenFileMapping ( DWORD dwDesiredAccess,
                         BOOL bInheritHandle,
                         LPCSTR lpName );

LPVOID MapViewOfFile ( HANDLE hFileMappingObject,
                       DWORD dwDesiredAccess,
                       DWORD dwFileOffsetHigh,
                       DWORD dwFileOffsetLow,
                       DWORD dwNumberOfBytesToMap );

BOOL FlushViewOfFile ( LPVOID lpBaseAddress,
                       DWORD dwNumberOfBytesToFlush );

BOOL UnmapViewOfFile ( LPVOID lpBaseAddress  );

UINT GetTempFileName ( LPCSTR lpPathName,
                       LPCSTR lpPrefixString,
                       UINT uUnique,
                       LPSTR lpTempFileName );

DWORD GetTempPath ( DWORD nBufferLength,
                    LPSTR lpBuffer );

BOOL FlushFileBuffers ( HANDLE hFile );

BOOL SetFileAttributes ( LPCSTR lpFileName,
                         DWORD dwFileAttributes );

DWORD GetFileAttributes ( LPCSTR lpFileName );

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

BOOL GetFileAttributesEx ( LPCSTR lpFileName,
                           GET_FILEEX_INFO_LEVELS fInfoLevelId,
                           LPVOID lpFileInformation );
// heap functions

HANDLE GetProcessHeap( VOID );

HANDLE HeapCreate ( DWORD flOptions,
                    DWORD dwInitialSize,
                    DWORD dwMaximumSize );

BOOL HeapDestroy ( HANDLE hHeap );

LPVOID HeapAlloc ( HANDLE hHeap,
                   DWORD dwFlags,
                   DWORD dwBytes );


BOOL HeapFree ( HANDLE hHeap,
                DWORD dwFlags,
                LPVOID lpMem );

LPVOID HeapReAlloc ( HANDLE hHeap,
                     DWORD dwFlags,
                     LPVOID lpMem,
                     DWORD dwBytes );

DWORD HeapSize ( HANDLE hHeap,
                 DWORD dwFlags,
                 LPVOID lpMem );

/// Thread Local Storage manipulation functions

#define TLS_OUT_OF_INDEXES (DWORD)0xFFFFFFFF

DWORD TlsAlloc ();

LPVOID TlsGetValue ( DWORD dwTlsIndex );

BOOL TlsSetValue ( DWORD dwTlsIndex, LPVOID lpTlsValue );

BOOL TlsFree( DWORD dwTlsIndex );


//
DWORD GetCurrentProcessId ();


VOID OutputDebugString ( const char* lpOutputString );

DWORD GetLastError ( VOID );

DWORD CoInitialize ( LPVOID );

void CoUninitialize ();

DWORD IIDFromString ( const char* lpsz, LPIID lpiid);

VOID GetLocalTime ( LPSYSTEMTIME lpSystemTime );

VOID GetSystemTime( LPSYSTEMTIME lpSystemTime );


DWORD GetModuleFileName ( HMODULE hModule,
                          char* lpFilename,
                          DWORD nSize );


VOID GetSystemInfo ( LPSYSTEM_INFO lpSystemInfo );

DWORD GetVersion( VOID );

BOOL GetVersionEx ( LPOSVERSIONINFO lpVersionInformation );

UINT SetErrorMode ( UINT uMode );

BOOL QueryPerformanceCounter ( LARGE_INTEGER *lpPerformanceCount );

BOOL QueryPerformanceFrequency ( LARGE_INTEGER *lpFrequency  );

DWORD GetCurrentDirectory ( DWORD nBufferLength,
                            LPSTR lpBuffer );

DWORD GetTickCount ();

LONG InterlockedIncrement ( LONG* lpAddend );

LONG InterlockedDecrement ( LONG* lpAddend );

LONG InterlockedExchange ( LONG* Target,
                           LONG Value );

PVOID InterlockedCompareExchange ( PVOID *Destination,
                                   PVOID Exchange,
                                   PVOID Comperand );

LONG InterlockedExchangeAdd ( LONG* Addend,
                              LONG Value );

// library manipulation functions

HMODULE LoadLibrary ( LPCSTR lpLibFileName );

HMODULE LoadLibraryEx ( LPCSTR lpLibFileName, 
                        HANDLE hFile, 
                        DWORD dwFlags );

BOOL FreeLibrary ( HMODULE hLibModule );

#define FARPROC void*
FARPROC GetProcAddress ( HMODULE hModule,
                         LPCSTR lpProcName );
// error mode 
#define SEM_FAILCRITICALERRORS      0x0001
#define SEM_NOGPFAULTERRORBOX       0x0002
#define SEM_NOALIGNMENTFAULTEXCEPT  0x0004
#define SEM_NOOPENFILEERRORBOX      0x8000


// resources manipulation functions
HRSRC FindResource ( HMODULE hModule,
                     LPCSTR lpName,
                     LPCSTR lpType );

HGLOBAL LoadResource ( HMODULE hModule,
                       HRSRC hResInfo );

BOOL FreeResource ( HGLOBAL hResData );

DWORD SizeofResource ( HMODULE hModule,
                       HRSRC hResInfo );

LPVOID LockResource ( HGLOBAL hResData );


// unix specific functions
HMODULE _CreateModule ( const char* aModuleName );

char* strlwr ( char* aSource );

#if defined (__cplusplus)
}
#endif



#define NO_ERROR            0
#define ERROR_INVALID_FUNCTION      1
#define ERROR_FILE_NOT_FOUND        2
#define ERROR_PATH_NOT_FOUND        3
#define ERROR_TOO_MANY_OPEN_FILES   4
#define ERROR_ACCESS_DENIED     5
#define ERROR_INVALID_HANDLE        6
#define ERROR_ARENA_TRASHED     7
#define ERROR_NOT_ENOUGH_MEMORY     8
#define ERROR_INVALID_BLOCK     9
#define ERROR_BAD_ENVIRONMENT       10
#define ERROR_BAD_FORMAT        11
#define ERROR_INVALID_ACCESS        12
#define ERROR_INVALID_DATA      13
/* **** reserved            14  ; ***** */
#define ERROR_INVALID_DRIVE     15
#define ERROR_CURRENT_DIRECTORY     16
#define ERROR_NOT_SAME_DEVICE       17
#define ERROR_NO_MORE_FILES     18
/* */
/* These are the universal int 24 mappings for the old INT 24 set of errors */
/* */
#define ERROR_WRITE_PROTECT     19
#define ERROR_BAD_UNIT          20
#define ERROR_NOT_READY         21
#define ERROR_BAD_COMMAND       22
#define ERROR_CRC           23
#define ERROR_BAD_LENGTH        24
#define ERROR_SEEK          25
#define ERROR_NOT_DOS_DISK      26
#define ERROR_SECTOR_NOT_FOUND      27
#define ERROR_OUT_OF_PAPER      28
#define ERROR_WRITE_FAULT       29
#define ERROR_READ_FAULT        30
#define ERROR_GEN_FAILURE       31
/* */
/* These are the new 3.0 error codes reported through INT 24 */
/* */
#define ERROR_SHARING_VIOLATION     32
#define ERROR_LOCK_VIOLATION        33
#define ERROR_WRONG_DISK        34
#define ERROR_FCB_UNAVAILABLE       35
#define ERROR_SHARING_BUFFER_EXCEEDED   36
/* */
/* New OEM network-related errors are 50-79 */
/* */
#define ERROR_NOT_SUPPORTED     50
/* */
/* End of INT 24 reportable errors */
/* */
#define ERROR_FILE_EXISTS       80
#define ERROR_DUP_FCB           81 /* ***** */
#define ERROR_CANNOT_MAKE       82
#define ERROR_FAIL_I24          83
/* */
/* New 3.0 network related error codes */
/* */
#define ERROR_OUT_OF_STRUCTURES     84
#define ERROR_ALREADY_ASSIGNED      85
#define ERROR_INVALID_PASSWORD      86
#define ERROR_INVALID_PARAMETER     87
#define ERROR_NET_WRITE_FAULT       88
/* */
/* New error codes for 4.0 */
/* */
#define ERROR_NO_PROC_SLOTS     89  /* no process slots available */
#define ERROR_NOT_FROZEN        90
#define ERR_TSTOVFL         91  /* timer service table overflow */
#define ERR_TSTDUP          92  /* timer service table duplicate */
#define ERROR_NO_ITEMS          93  /* There were no items to operate upon */
#define ERROR_INTERRUPT         95  /* interrupted system call */

#define ERROR_TOO_MANY_SEMAPHORES   100
#define ERROR_EXCL_SEM_ALREADY_OWNED    101
#define ERROR_SEM_IS_SET        102
#define ERROR_TOO_MANY_SEM_REQUESTS 103
#define ERROR_INVALID_AT_INTERRUPT_TIME 104

#define ERROR_SEM_OWNER_DIED        105 /* waitsem found owner died */
#define ERROR_SEM_USER_LIMIT        106 /* too many procs have this sem */
#define ERROR_DISK_CHANGE       107 /* insert disk b into drive a */
#define ERROR_DRIVE_LOCKED      108 /* drive locked by another process */
#define ERROR_BROKEN_PIPE       109 /* write on pipe with no reader */
/* */
/* New error codes for OS/2 */
/* */
#define ERROR_OPEN_FAILED       110 /* open/created failed due to */
                        /* explicit fail command */
#define ERROR_BUFFER_OVERFLOW       111 /* buffer passed to system call */
                        /* is too small to hold return */
                        /* data. */
#define ERROR_DISK_FULL         112 /* not enough space on the disk */
                        /* (DOSNEWSIZE/w_NewSize) */
#define ERROR_NO_MORE_SEARCH_HANDLES    113 /* can't allocate another search */
                        /* structure and handle. */
                        /* (DOSFINDFIRST/w_FindFirst) */
#define ERROR_INVALID_TARGET_HANDLE 114 /* Target handle in DOSDUPHANDLE */
                        /* is invalid */
#define ERROR_PROTECTION_VIOLATION  115 /* Bad user virtual address */
#define ERROR_VIOKBD_REQUEST        116
#define ERROR_INVALID_CATEGORY      117 /* Category for DEVIOCTL in not */
                        /* defined */
#define ERROR_INVALID_VERIFY_SWITCH 118 /* invalid value passed for */
                        /* verify flag */
#define ERROR_BAD_DRIVER_LEVEL      119 /* DosDevIOCTL looks for a level */
                        /* four driver.   If the driver */
                        /* is not level four we return */
                        /* this code */
#define ERROR_CALL_NOT_IMPLEMENTED  120 /* returned from stub api calls. */
                        /* This call will disappear when */
                        /* all the api's are implemented. */
#define ERROR_SEM_TIMEOUT       121 /* Time out happened from the */
                        /* semaphore api functions. */
#define ERROR_INSUFFICIENT_BUFFER   122 /* Some call require the  */
                        /* application to pass in a buffer */
                        /* filled with data.  This error is */
                        /* returned if the data buffer is too */
                        /* small.  For example: DosSetFileInfo */
                        /* requires 4 bytes of data.  If a */
                        /* two byte buffer is passed in then */
                        /* this error is returned.   */
                        /* error_buffer_overflow is used when */
                        /* the output buffer in not big enough. */
#define ERROR_INVALID_NAME      123 /* illegal character or malformed */
                        /* file system name */
#define ERROR_INVALID_LEVEL     124 /* unimplemented level for info */
                        /* retrieval or setting */
#define ERROR_NO_VOLUME_LABEL       125 /* no volume label found with */
                        /* DosQFSInfo command */
#define ERROR_MOD_NOT_FOUND     126 /* w_getprocaddr,w_getmodhandle */
#define ERROR_PROC_NOT_FOUND        127 /* w_getprocaddr */

#define ERROR_WAIT_NO_CHILDREN      128 /* CWait finds to children */

#define ERROR_CHILD_NOT_COMPLETE    129 /* CWait children not dead yet */

/*This is a temporary fix for the 4-19-86 build this should be changed when */
/* we get the file from MS */
#define ERROR_DIRECT_ACCESS_HANDLE  130 /* handle operation is invalid */
                        /* for direct disk access */
                        /* handles */
#define ERROR_NEGATIVE_SEEK     131 /* application tried to seek  */
                        /* with negative offset */
#define ERROR_SEEK_ON_DEVICE        132 /* application tried to seek */
                        /* on device or pipe */
/* */
/* The following are errors generated by the join and subst workers */
/* */
#define ERROR_IS_JOIN_TARGET        133
#define ERROR_IS_JOINED         134
#define ERROR_IS_SUBSTED        135
#define ERROR_NOT_JOINED        136
#define ERROR_NOT_SUBSTED       137
#define ERROR_JOIN_TO_JOIN      138
#define ERROR_SUBST_TO_SUBST        139
#define ERROR_JOIN_TO_SUBST     140
#define ERROR_SUBST_TO_JOIN     141
#define ERROR_BUSY_DRIVE        142
#define ERROR_SAME_DRIVE        143
#define ERROR_DIR_NOT_ROOT      144
#define ERROR_DIR_NOT_EMPTY     145
#define ERROR_IS_SUBST_PATH     146
#define ERROR_IS_JOIN_PATH      147
#define ERROR_PATH_BUSY         148
#define ERROR_IS_SUBST_TARGET       149
#define ERROR_SYSTEM_TRACE      150 /* system trace error */
#define ERROR_INVALID_EVENT_COUNT   151 /* DosMuxSemWait errors */
#define ERROR_TOO_MANY_MUXWAITERS   152
#define ERROR_INVALID_LIST_FORMAT   153
#define ERROR_LABEL_TOO_LONG        154
#define ERROR_TOO_MANY_TCBS     155
#define ERROR_SIGNAL_REFUSED        156
#define ERROR_DISCARDED         157
#define ERROR_NOT_LOCKED        158
#define ERROR_BAD_THREADID_ADDR     159
#define ERROR_BAD_ARGUMENTS     160
#define ERROR_BAD_PATHNAME      161
#define ERROR_SIGNAL_PENDING        162
#define ERROR_UNCERTAIN_MEDIA       163
#define ERROR_MAX_THRDS_REACHED     164
#define ERROR_MONITORS_NOT_SUPPORTED    165

#define ERROR_INVALID_SEGMENT_NUMBER    180
#define ERROR_INVALID_CALLGATE      181
#define ERROR_INVALID_ORDINAL       182
#define ERROR_ALREADY_EXISTS        183
#define ERROR_NO_CHILD_PROCESS      184
#define ERROR_CHILD_ALIVE_NOWAIT    185
#define ERROR_INVALID_FLAG_NUMBER   186
#define ERROR_SEM_NOT_FOUND     187

/*  following error codes have added to make the loader error
    messages distinct
*/

#define ERROR_INVALID_STARTING_CODESEG  188
#define ERROR_INVALID_STACKSEG      189
#define ERROR_INVALID_MODULETYPE    190
#define ERROR_INVALID_EXE_SIGNATURE 191
#define ERROR_EXE_MARKED_INVALID    192
#define ERROR_BAD_EXE_FORMAT        193
#define ERROR_ITERATED_DATA_EXCEEDS_64k 194
#define ERROR_INVALID_MINALLOCSIZE  195
#define ERROR_DYNLINK_FROM_INVALID_RING 196
#define ERROR_IOPL_NOT_ENABLED      197
#define ERROR_INVALID_SEGDPL        198
#define ERROR_AUTODATASEG_EXCEEDS_64k   199
#define ERROR_RING2SEG_MUST_BE_MOVABLE  200
#define ERROR_RELOC_CHAIN_XEEDS_SEGLIM  201
#define ERROR_INFLOOP_IN_RELOC_CHAIN    202

#define ERROR_ENVVAR_NOT_FOUND      203
#define ERROR_NOT_CURRENT_CTRY      204
#define ERROR_NO_SIGNAL_SENT        205
#define ERROR_FILENAME_EXCED_RANGE  206 /* if filename > 8.3 */
#define ERROR_RING2_STACK_IN_USE    207 /* for FAPI */
#define ERROR_META_EXPANSION_TOO_LONG   208 /* if "*a" > 8.3 */

#define ERROR_INVALID_SIGNAL_NUMBER 209
#define ERROR_THREAD_1_INACTIVE     210
#define ERROR_INFO_NOT_AVAIL        211 /*@@ PTM 5550 */
#define ERROR_LOCKED            212
#define ERROR_BAD_DYNALINK      213 /*@@ PTM 5760 */
#define ERROR_TOO_MANY_MODULES      214
#define ERROR_NESTING_NOT_ALLOWED   215

/*
 * Error codes 230 - 249 are reserved for MS Networks
 */

#define ERROR_USER_DEFINED_BASE     0xF000

#define ERROR_I24_WRITE_PROTECT     0
#define ERROR_I24_BAD_UNIT      1
#define ERROR_I24_NOT_READY     2
#define ERROR_I24_BAD_COMMAND       3
#define ERROR_I24_CRC           4
#define ERROR_I24_BAD_LENGTH        5
#define ERROR_I24_SEEK          6
#define ERROR_I24_NOT_DOS_DISK      7
#define ERROR_I24_SECTOR_NOT_FOUND  8
#define ERROR_I24_OUT_OF_PAPER      9
#define ERROR_I24_WRITE_FAULT       0x0A
#define ERROR_I24_READ_FAULT        0x0B
#define ERROR_I24_GEN_FAILURE       0x0C
#define ERROR_I24_DISK_CHANGE       0x0D
#define ERROR_I24_WRONG_DISK        0x0F
#define ERROR_I24_UNCERTAIN_MEDIA   0x10
#define ERROR_I24_CHAR_CALL_INTERRUPTED 0x11
#define ERROR_I24_NO_MONITOR_SUPPORT    0x12
#define ERROR_I24_INVALID_PARAMETER 0x13

#define ERROR_NETWORK_NOT_AVAILABLE 0x000013AVL
#define ERROR_NETWORK_ACCESS_DENIED 65L
#define ERROR_SUCCESS               0l

#define ALLOWED_FAIL        0x0001
#define ALLOWED_ABORT       0x0002
#define ALLOWED_RETRY       0x0004
#define ALLOWED_IGNORE      0x0008

#define I24_OPERATION       0x1
#define I24_AREA        0x6
                    /* 01 if FAT */
                    /* 10 if root DIR */
                    /* 11 if DATA */
#define I24_CLASS       0x80


/* Values for error CLASS */

#define ERRCLASS_OUTRES     1   /* Out of Resource */
#define ERRCLASS_TEMPSIT    2   /* Temporary Situation */
#define ERRCLASS_AUTH       3   /* Permission problem */
#define ERRCLASS_INTRN      4   /* Internal System Error */
#define ERRCLASS_HRDFAIL    5   /* Hardware Failure */
#define ERRCLASS_SYSFAIL    6   /* System Failure */
#define ERRCLASS_APPERR     7   /* Application Error */
#define ERRCLASS_NOTFND     8   /* Not Found */
#define ERRCLASS_BADFMT     9   /* Bad Format */
#define ERRCLASS_LOCKED     10  /* Locked */
#define ERRCLASS_MEDIA      11  /* Media Failure */
#define ERRCLASS_ALREADY    12  /* Collision with Existing Item */
#define ERRCLASS_UNK        13  /* Unknown/other */
#define ERRCLASS_CANT       14
#define ERRCLASS_TIME       15

/* Values for error ACTION */

#define ERRACT_RETRY        1   /* Retry */
#define ERRACT_DLYRET       2   /* Delay Retry, retry after pause */
#define ERRACT_USER     3   /* Ask user to regive info */
#define ERRACT_ABORT        4   /* abort with clean up */
#define ERRACT_PANIC        5   /* abort immediately */
#define ERRACT_IGNORE       6   /* ignore */
#define ERRACT_INTRET       7   /* Retry after User Intervention */

/* Values for error LOCUS */

#define ERRLOC_UNK      1   /* No appropriate value */
#define ERRLOC_DISK     2   /* Random Access Mass Storage */
#define ERRLOC_NET      3   /* Network */
#define ERRLOC_SERDEV       4   /* Serial Device */
#define ERRLOC_MEM      5   /* Memory */

/* Abnormal termination codes */

#define TC_NORMAL   0
#define TC_HARDERR  1
#define TC_GP_TRAP  2
#define TC_SIGNAL   3


#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((DWORD   )0xC00002C0L)     


#define MOVEFILE_REPLACE_EXISTING       0x00000001
#define MOVEFILE_COPY_ALLOWED           0x00000002
#define MOVEFILE_DELAY_UNTIL_REBOOT     0x00000004
#define MOVEFILE_WRITE_THROUGH          0x00000008

#define _MAX_DRIVE 3

#define RPC_E_UNEXPECTED                 0x8001FFFFL

#define S_OK                                   0x00000000L
#define E_INVALIDARG                           0x80070057L

#endif //_COMPAT_H_


#ifndef WINAPI
#define DWORD ULONG 
#define WORD USHORT 
#define BOOL int
#define FAR
#define WINAPI __stdcall
#define UINT unsigned int
#define BYTE unsigned char
#define WPARAM unsigned int
//#define _TCHAR char
#define INT int
#define LPARAM LONG
#define LRESULT LONG
#define LPBYTE BYTE*
#define LPWORD WORD*
#define LPDWORD DWORD*
#define LPVOID void*
#define LPCVOID const void*
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

typedef struct _OVERLAPPED {
    DWORD   Internal;
    DWORD   InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _SECURITY_ATTRIBUTES FAR *PSECURITY_ATTRIBUTES;
typedef struct _SECURITY_ATTRIBUTES FAR *LPSECURITY_ATTRIBUTES;
#endif 

BOOL LoadIO();

/*
#define GENERIC_READ            0x80000000L
#define GENERIC_WRITE           0x40000000L
#define GENERIC_EXECUTE         0x20000000L
#define GENERIC_ALL             0x10000000L

#define FILE_SHARE_READ         0x00000001
#define FILE_SHARE_WRITE        0x00000002
#define FILE_SHARE_DELETE       0x00000004
*/

#define CREATE_NEW              0x00000001
#define CREATE_ALWAYS           0x00000002
#define OPEN_EXISTING           0x00000003
#define OPEN_ALWAYS             0x00000004
#define TRUNCATE_EXISTING       0x00000005
#define INVALID_HANDLE_VALUE	((HANDLE)-1)

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2


#if		NTVERSION==350
#define NT_ROOT_PREFIX   "\\DosDevices\\"
#define LNT_ROOT_PREFIX L"\\DosDevices\\"
#elif	NTVERSION==351
#define NT_ROOT_PREFIX   "\\DosDevices\\"
#define LNT_ROOT_PREFIX L"\\DosDevices\\"
#else //	NTVERSION==400
#define NT_ROOT_PREFIX   "\\??\\"
#define LNT_ROOT_PREFIX L"\\??\\"
#endif

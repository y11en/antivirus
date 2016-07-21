#ifndef _NTYPEDEF_H
#define _NTYPEDEF_H

#ifndef DWORD
# define DWORD	unsigned int
#endif

#ifndef ULONG
# define ULONG	unsigned long
#endif

#ifndef BYTE
# define BYTE	unsigned char
#endif

#ifndef HANDLE
# define HANDLE	void*
#endif

#ifndef BOOL
# define BOOL	int
#endif

#ifndef FALSE
# define FALSE	0
#endif

#ifndef TRUE
# define TRUE	1
#endif

#ifndef UINT
# define UINT	unsigned int
#endif

#ifndef LONG
#define LONG 	long 
#else
#undef LONG
#define LONG 	long 
#endif

#ifndef WORD
#define WORD 	unsigned short 
#endif

#ifndef CHAR
#define CHAR 	char 
#endif

#ifndef BOOL
#define BOOL 	int
#endif

#define FAR

#ifndef NEAR
#define NEAR
#endif

#ifndef INT
#define INT int
#endif

#ifndef LRESULT
#define LRESULT LONG
#endif

#ifndef LPBYTE
#define LPBYTE BYTE*
#endif

#ifndef LPWORD
#define LPWORD WORD*
#endif

#ifndef LPDWORD
#define LPDWORD DWORD*
#endif

#ifndef LPVOID
#define LPVOID void*
#endif

#ifndef PLONG
#define PLONG  LONG*
#endif

#ifndef LPCVOID
#define LPCVOID const void*
#endif

#ifndef HANDLE
 #define HANDLE void*
#endif

#ifndef PCHAR
 #define PCHAR char *
#endif

#ifndef VOID
 #define VOID void
#endif 

#ifndef LPTSTR
#define LPTSTR char*
#endif

#ifndef LPCTSTR
#define LPCTSTR const char*
#endif

#ifndef __OVERLAPPED_DEFINED
typedef struct _OVERLAPPED {
    DWORD   Internal;
    DWORD   InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE 	hEvent;
} OVERLAPPED, *LPOVERLAPPED;
#define __OVERLAPPED_DEFINED
#endif

#ifndef __TCHAR_DEFINED
typedef char            _TCHAR;
typedef signed char     _TSCHAR;
typedef unsigned char   _TUCHAR;
typedef unsigned char   _TXCHAR;
typedef unsigned int    _TINT;
#define __TCHAR_DEFINED
#endif

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

typedef struct  _SYSTEMTIME
    {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
    }   SYSTEMTIME;

#define INVALID_HANDLE_VALUE (HANDLE)-1
#define INVALID_FILE_SIZE (DWORD)0xFFFFFFFF

        //#define FILE_BEGIN           0
        //#define FILE_CURRENT         1
        //#define FILE_END             2

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004
#define FILE_ATTRIBUTE_READONLY         0x00000001
#define FILE_ATTRIBUTE_HIDDEN           0x00000002
#define FILE_ATTRIBUTE_SYSTEM           0x00000004
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010
#define FILE_ATTRIBUTE_ARCHIVE          0x00000020
#define FILE_ATTRIBUTE_NORMAL           0x00000080
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100
#define FILE_ATTRIBUTE_COMPRESSED       0x00000800
#define FILE_ATTRIBUTE_OFFLINE          0x00001000
#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100
#define FILE_ACTION_ADDED               0x00000001
#define FILE_ACTION_REMOVED             0x00000002
#define FILE_ACTION_MODIFIED            0x00000003
#define FILE_ACTION_RENAMED_OLD_NAME    0x00000004
#define FILE_ACTION_RENAMED_NEW_NAME    0x00000005
#define MAILSLOT_NO_MESSAGE             ((DWORD)-1)
#define MAILSLOT_WAIT_FOREVER           ((DWORD)-1)
#define FILE_CASE_SENSITIVE_SEARCH      0x00000001
#define FILE_CASE_PRESERVED_NAMES       0x00000002
#define FILE_UNICODE_ON_DISK            0x00000004
#define FILE_PERSISTENT_ACLS            0x00000008
#define FILE_FILE_COMPRESSION           0x00000010
#define FILE_VOLUME_IS_COMPRESSED       0x00008000

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#endif
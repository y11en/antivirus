#ifndef __TYPEDEF_H
#define __TYPEDEF_H

#ifdef N_PLAT_NLM
#include <wchar_t.h>
#include <nwtypes.h>
#else
#include <wchar.h>
#endif

//#define gettext(String) (String)
#define _T(String) (String)

#define __stdcall

#define MAX_PATH	260
#define _MAX_PATH	MAX_PATH

#ifndef WINAPI
 #define WINAPI __stdcall
#endif  

#ifndef FALSE
 #define FALSE 0
#endif
#ifndef TRUE
 #define TRUE 1
#endif

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned long	uint32;
typedef unsigned int	UINT;

#define VOID		void
#define CHAR    	char		// ch  
#define LONG    	long		// l   

typedef VOID *		MRESULT;	// mres  
typedef UINT		WPARAM;

typedef char 		TCHAR;
typedef unsigned long	ULONG;
typedef LONG *		PLONG;
#ifndef N_PLAT_NLM
typedef unsigned short  WORD;
#endif
typedef unsigned long   DWORD;
typedef unsigned long	BOOL;		// f   
typedef unsigned long	APIRET;
typedef int		HANDLE;

#ifndef FAR
 #define FAR         		/* this will be deleted shortly */
#endif
#define far         		/* this will be deleted shortly */
#define near        		/* this will be deleted shortly */
#define CONST		const

typedef BOOL far            *LPBOOL;

typedef WORD far        *LPWORD;
typedef DWORD far       *LPDWORD;
typedef void far	*LPVOID;
typedef CONST void far	*LPCVOID;

#define __RPC_FAR

typedef /* [string] */ const CHAR __RPC_FAR *LPCSTR;
typedef LPCSTR LPCTSTR;
typedef CHAR *		LPSTR;
typedef LPSTR 		LPTSTR;

typedef void __RPC_FAR *HMODULE;

typedef wchar_t WCHAR;
typedef /* [string] */ const WCHAR __RPC_FAR *LPCWSTR;
typedef WCHAR *LPWSTR, *PWSTR;
//typedef LPCWSTR LPCTSTR;
//typedef WCHAR *LPSTR;
//typedef LPWSTR LPTSTR;

typedef struct _OVERLAPPED
 {
    DWORD Internal;
    DWORD InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    HANDLE  hEvent;
 } OVERLAPPED;

#ifndef _SECURITY_ATTRIBUTES_
#define _SECURITY_ATTRIBUTES_
typedef struct _SECURITY_ATTRIBUTES
 {
    DWORD  hLength;
    LPWORD lpSecurityAttributes;
    BOOL   hInheritHandle;
 } SECURITY_ATTRIBUTES;
#endif

#define INVALID_HANDLE_VALUE (HANDLE)-1
#define INVALID_FILE_SIZE (DWORD)0xFFFFFFFF

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004
#ifdef __LINUX__
#include <sys/stat.h>
 #define FILE_ATTRIBUTE_READONLY         S_IRUSR|S_IRGRP|S_IROTH
 #define FILE_ATTRIBUTE_TEMPORARY        0x00000100
#else					
 #define FILE_ATTRIBUTE_READONLY         0x00000001
 #define FILE_ATTRIBUTE_HIDDEN           0x00000002
 #define FILE_ATTRIBUTE_SYSTEM           0x00000004
 #define FILE_ATTRIBUTE_DIRECTORY        0x00000010
 #define FILE_ATTRIBUTE_ARCHIVE          0x00000020
 #define FILE_ATTRIBUTE_NORMAL           0x00000080
 #define FILE_ATTRIBUTE_TEMPORARY        0x00000100
 #define FILE_ATTRIBUTE_COMPRESSED       0x00000800
 #define FILE_ATTRIBUTE_OFFLINE          0x00001000
#endif					
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

#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000

#define FILE_FLAG_DELETE_ON_CLOSE		0x00000000
#define FILE_FLAG_WRITE_THROUGH			0x00000000

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

//
// dwPlatformId defines:
//

#define VER_PLATFORM_WIN32s             0
#define VER_PLATFORM_WIN32_WINDOWS      1
#define VER_PLATFORM_WIN32_NT           2

/* These are the new 3.0 error codes reported through INT 24 */
#define ERROR_SHARING_VIOLATION     32
#define ERROR_LOCK_VIOLATION        33

#endif

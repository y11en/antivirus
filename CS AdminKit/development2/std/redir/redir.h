#ifndef __REDIR_H
#define __REDIR_H

#include "typedef.h"

void KLSTD_InitAlternativeFileLocker();

void KLSTD_DeInitAlternativeFileLocker();

// returns 0 - already is locked, 1 - succesefuly locked, -1 - error
int KLSTD_AlternativeLockFile( int fdesc, DWORD shareMode, DWORD access );

int KLSTD_AlternativeUnLockFile( int fdesc );

BOOL WINAPI ReadFile(
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,  // number of bytes to read
    LPDWORD  lpNumberOfBytesRead, // address of number of bytes read
    OVERLAPPED *  lpOverlapped);   // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)

BOOL WINAPI WriteFile(
    HANDLE  hFile,      // handle to file to write to
    LPCVOID lpBuffer,  // pointer to data to write to file
    DWORD   nNumberOfBytesToWrite,    // number of bytes to write
    LPDWORD lpNumberOfBytesWritten, // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped);   // addr. of structure needed for overlapped I/O

BOOL __stdcall CloseHandle(HANDLE hObject);     // handle to object to close

HANDLE WINAPI CreateFile(
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD    dwDesiredAccess,   // access (read-write)
    DWORD    dwShareMode,       // share mode
    SECURITY_ATTRIBUTES FAR* lpSecurityAttributes,     // pointer to security descriptor
    DWORD    dwCreationDistribution,      // how to create
    DWORD    dwFlagsAndAttributes,        // file attributes
    HANDLE   hTemplateFile,
    bool&    bFileLocked);       // handle to file with attributes to copy

DWORD WINAPI SetFilePointer(
    HANDLE hFile,                // handle of file
    LONG   lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh, // address of high-order word of distance to move
    DWORD  dwMoveMethod);         // how to move

DWORD WINAPI GetFileSize(HANDLE  hFile,      // handle of file
                            LPDWORD lpHigh);

DWORD WINAPI SetFilePointer(
    HANDLE hFile,                // handle of file
    LONG   lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh, // address of high-order word of distance to move
    DWORD  dwMoveMethod);         // how to move

BOOL WINAPI SetEndOfFile(HANDLE hFile);       // handle of file

DWORD WINAPI GetTempPath(
        DWORD nBufferLength,  // size, in characters, of the buffer
        LPTSTR lpBuffer);      // address of buffer for temp. path

UINT WINAPI GetTempFileName(
        LPCTSTR lpPathName,     // address of directory name for temporary file
        LPCTSTR lpPrefixString, // address of filename prefix
        UINT uUnique,           // number used to create temporary filename
        LPTSTR lpTempFileName);  // address of buffer that receives the new filename

BOOL WINAPI FlushFileBuffers (
	HANDLE hFile );				// handle of file

BOOL WINAPI LockFile(
		HANDLE hFile,                   // handle to file
		DWORD dwFileOffsetLow,          // low-order word of offset
		DWORD dwFileOffsetHigh,         // high-order word of offset
		DWORD nNumberOfBytesToLockLow,  // low-order word of length
		DWORD nNumberOfBytesToLockHigh  // high-order word of length
);

BOOL WINAPI UnlockFile(
		HANDLE hFile,                    // handle to file
		DWORD dwFileOffsetLow,           // low-order word of start
		DWORD dwFileOffsetHigh,          // high-order word of start
		DWORD nNumberOfBytesToUnlockLow, // low-order word of length
		DWORD nNumberOfBytesToUnlockHigh // high-order word of length
);

void _wsplitpath(
   const wchar_t *path,
   wchar_t *drive,
   wchar_t *dir,
   wchar_t *fname,
   wchar_t *ext 
);

void _wmakepath(
   wchar_t *path,
   const wchar_t *drive,
   const wchar_t *dir,
   const wchar_t *fname,
   const wchar_t *ext 
);

int _waccess( 
   const wchar_t *path, 
   int mode 
);

int _wrename(
   const wchar_t *oldname,
   const wchar_t *newname 
);

DWORD GetLastError(VOID);

int MultiByteToWideChar(
  UINT CodePage,         // code page
  DWORD dwFlags,         // character-type options
  LPCSTR lpMultiByteStr, // string to map
  int cbMultiByte,       // number of bytes in string
  LPWSTR lpWideCharStr,  // wide-character buffer
  int cchWideChar        // size of buffer
);

int WideCharToMultiByte(
  UINT CodePage,            // code page
  DWORD dwFlags,            // performance and mapping flags
  LPCWSTR lpWideCharStr,    // wide-character string
  int cchWideChar,          // number of chars in string
  LPSTR lpMultiByteStr,     // buffer for new string
  int cbMultiByte,          // size of buffer
  LPCSTR lpDefaultChar,     // default for unmappable chars
  LPBOOL lpUsedDefaultChar  // set when default char used
);

//CP_ACP ANSI code page 
//CP_MACCP Macintosh code page 
//CP_OEMCP OEM code page 
//CP_SYMBOL Windows 2000/XP: Symbol code page (42) 
//CP_THREAD_ACP Windows 2000/XP: The current thread's ANSI code page 
//CP_UTF7 Windows 98/Me, Windows NT 4.0 and later: Translate using UTF-7 
//CP_UTF8 Windows 98/Me, Windows NT 4.0 and later: Translate using UTF-8. When this is set, dwFlags must be zero. 

//MB_PRECOMPOSED Always use precomposed characters-that is, characters in which a base character and a nonspacing character have a single character value. This is the default translation option. Cannot be used with MB_COMPOSITE. 
//MB_COMPOSITE Always use composite characters-that is, characters in which a base character and a nonspacing character have different character values. Cannot be used with MB_PRECOMPOSED. 
//MB_ERR_INVALID_CHARS If the function encounters an invalid input character, it fails and GetLastError returns ERROR_NO_UNICODE_TRANSLATION. 
//MB_USEGLYPHCHARS Use glyph characters instead of control characters. 

//WC_NO_BEST_FIT_CHARS Windows 2000/XP: Any Unicode characters that do not translate directly to multibyte equivalents will be translated to the default character (see lpDefaultChar parameter). In other words, if translating from Unicode to multibyte and back to Unicode again does not yield the exact same Unicode character, the default character is used. 
//WC_COMPOSITECHECK Convert composite characters to precomposed characters. 
//WC_DISCARDNS Discard nonspacing characters during conversion. 
//WC_SEPCHARS Generate separate characters during conversion. This is the default conversion behavior. 
//WC_DEFAULTCHAR Replace exceptions with the default character during conversion. 

#endif

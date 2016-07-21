#if !defined (__NOVELL_IO_REDIR_H__)
#define __NOVELL_IO_REDIR_H__

#if defined (__MWERKS__)

#include <stdlib.h>
#include <stdio.h>
#include <_avpio.h>
#include <fcntl.h>
#include <mode.h>
#include <nwfattr.h>
#include <nwfileng.h>
#include <nwfileio.h>
#include <platform_compat.h>


HANDLE WINAPI nlmAvpCreateFile (
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD  dwDesiredAccess,     // access (read-write)
    DWORD  dwShareMode,         // share mode
    SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
    DWORD  dwCreationDistribution,      // how to create
    DWORD  dwFlagsAndAttributes,        // file attributes
    HANDLE  hTemplateFile               // handle to file with attributes to copy
        );

BOOL __stdcall nlmAvpCloseHandle(HANDLE  hObject);     // handle to object to close

DWORD WINAPI nlmAvpGetFileSize(HANDLE  hFile,      // handle of file
                             LPDWORD  lpHigh);

BOOL WINAPI nlmAvpReadFile(
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,        // number of bytes to read
    LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
    OVERLAPPED *  lpOverlapped       // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
        );

BOOL WINAPI nlmAvpWriteFile (
    HANDLE  hFile,      // handle to file to write to
    LPCVOID  lpBuffer,  // pointer to data to write to file
    DWORD  nNumberOfBytesToWrite,       // number of bytes to write
    LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
        );

#endif // defined (__MWERKS__)

#endif

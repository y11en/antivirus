/*
 * This file was designed to use for UNIX only!
 */
#ifndef INCLUDE_FILEIO_H
#define INCLUDE_FILEIO_H

#include <AVPPort.h>
#include <compat_unix.h>
#include <_AVPIO.h>

BOOL WINAPI Cache_AvpReadFile (
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,        // number of bytes to read
    LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
    OVERLAPPED FAR*  lpOverlapped       // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
        );

BOOL  WINAPI Cache_AvpWriteFile (
    HANDLE  hFile,      // handle to file to write to
    LPCVOID  lpBuffer,  // pointer to data to write to file
    DWORD  nNumberOfBytesToWrite,       // number of bytes to write
    LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
        );

BOOL  WINAPI Cache_AvpDeleteFile (
    LPCTSTR  lpFileName        // pointer to name of the file
	);

HANDLE  WINAPI Cache_AvpCreateFile (
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD  dwDesiredAccess,     // access (read-write) mode
    DWORD  dwShareMode, // share mode
    SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
    DWORD  dwCreationDistribution,      // how to create
    DWORD  dwFlagsAndAttributes,        // file attributes
    HANDLE  hTemplateFile       // handle to file with attributes to copy
        );

BOOL  WINAPI Cache_AvpCloseHandle(
    HANDLE  hObject     // handle to object to close
        );

DWORD  WINAPI Cache_AvpSetFilePointer(
    HANDLE  hFile,      // handle of file
    LONG  lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh,        // address of high-order word of distance to move
    DWORD  dwMoveMethod         // how to move
        );

BOOL  WINAPI Cache_AvpSetEndOfFile(
    HANDLE  hFile       // handle of file
        );

DWORD WINAPI Cache_AvpGetFileSize(
    HANDLE  hFile,      // handle of file
    LPDWORD  lpHigh
        );

BOOL WINAPI AvpFlush(
    HANDLE  hFile       // handle of file
        );

BOOL WINAPI Cache_AvpFlush(
    HANDLE  hFile       // handle of file
        );

#endif /* INCLUDE_FILEIO_H */

#ifndef _AL_FILE_WIN_H_4541231843434_2158123
#define _AL_FILE_WIN_H_4541231843434_2158123

#include "al_file.h"

#include <windows.h>

AL_DECLARE_INTERFACE(alFileWin32) AL_INTERFACE_BASED(alFile, file)
AL_DECLARE_INTERFACE_BEGIN
AL_DECLARE_METHOD7(bool, CreateFileA, LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
AL_DECLARE_METHOD7(bool, CreateFileW, LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
AL_DECLARE_INTERFACE_END

EXTERN_C alFileWin32* AL_CALLTYPE new_alFileWin32();

#endif // _AL_FILE_WIN_H_4541231843434_2158123

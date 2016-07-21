#ifndef __SPECIALNAME_H__
#define __SPECIALNAME_H__

#if defined (_WIN32)

#include <windows.h>

bool check_dir_writable(LPCTSTR szDir);
bool GetSpecialDir(LPTSTR strBuffer, DWORD nBufferSizeInTChars);
int MakeSpecialName(LPTSTR strBuffer, DWORD nBufferSizeInTChars, LPCTSTR suffix1, LPCTSTR suffix2, bool bNoDateTime = false);

#endif //_WIN32

#endif // __SPECIALNAME_H__

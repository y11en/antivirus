#ifndef __GETFILEVER_H__
#define __GETFILEVER_H__

DWORD _GetFileVersionInfoSizeW(LPCWSTR lptstrFilename);
BOOL _GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwLen, LPVOID lpData);


#endif // __GETFILEVER_H__
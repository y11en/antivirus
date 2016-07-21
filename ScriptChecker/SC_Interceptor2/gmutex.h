#include <windows.h>

HANDLE OpenGlobalMutex(DWORD dwDesiredAccess, BOOL bInheritHandle, const CHAR* lpName);
BOOL IsGlobalMutexExist(const CHAR* pMutexName);

//#include "windows.h"
#include "defines.h"

#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

long __stdcall	RegEnumCallback(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
long __stdcall	RegEnumCallbackErrMessage(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error);

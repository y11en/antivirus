#include "defines.h"

#include <Prague/prague.h>
#include <Prague/iface/i_inifile.h>

tERROR __stdcall cbIniEnum(const wchar_t* sFileName,const tCHAR* sSection,const tCHAR* sValue,tDWORD dwFlag,EnumContext* pContext);
tERROR __stdcall cbBatEnum(const wchar_t* sFileName,const tCHAR* sSection,const tCHAR* sValue,tDWORD dwFlag,EnumContext* pContext);

long __stdcall	RegEnumCallbackHosts(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
long __stdcall	RegEnumCallbackLSP(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
long __stdcall	RegEnumCallbackLSP2(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
long __stdcall	RegEnumCallbackKeyRename(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
long __stdcall	RegEnumCallbackErrMessageLSP(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error);

tDWORD	GetLine(tCHAR* ptr, tCHAR** line, tCHAR* EndChar, tBOOL* bToMiss);

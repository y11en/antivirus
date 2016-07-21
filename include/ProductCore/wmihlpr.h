#ifndef __wmihlpr__
#define __wmihlpr__

#include <windows.h>

HRESULT wmih_Install(LPCGUID pguidID, const CHAR *pszLegacyRegKey, const WCHAR *pszCompanyName, const WCHAR *pszProductVersion, const WCHAR *pszDisplayName);
HRESULT wmih_UpdateStatusAsync(LPCGUID pguidID, DWORD p_eComponent, DWORD p_eStatus);
HRESULT wmih_Uninstall(LPCGUID pguidID);

typedef HRESULT (__cdecl *_t_wmih_Install)(LPCGUID pguidID, const CHAR *pszLegacyRegKey, const WCHAR *pszCompanyName, const WCHAR *pszProductVersion, const WCHAR *pszDisplayName);
typedef HRESULT (__cdecl *_t_wmih_UpdateStatus)(LPCGUID pguidID, DWORD p_eComponent, DWORD p_eStatus);
typedef HRESULT (__cdecl *_t_wmih_Uninstall)(LPCGUID pguidID);

#define __wmih_exec(_func, _func_name) { HMODULE hwmihlp = LoadLibrary(_T("wmihlpr.ppl")); if (hwmihlp) { _func _f = (_func)GetProcAddress(hwmihlp, _func_name); if( _f ) _f
#define __wmih_end()   FreeLibrary(hwmihlp); } }

#define _wmih_Install(pguidID, pszLegacyRegKey, pszCompanyName, pszProductVersion, pszDisplayName) \
	__wmih_exec(_t_wmih_Install, "wmih_InstallAV")((LPCGUID)pguidID, pszLegacyRegKey, pszCompanyName, pszProductVersion, pszDisplayName); __wmih_end()

#define _wmih_UpdateStatus(pguidID, eComponent, eStatus) \
	__wmih_exec(_t_wmih_UpdateStatus, "wmih_UpdateStatus")((LPCGUID)pguidID, eComponent, eStatus); __wmih_end()

#define _wmih_Uninstall(pguidID) \
	__wmih_exec(_t_wmih_Uninstall, "wmih_Uninstall")((LPCGUID)pguidID); __wmih_end()

#endif // __wmihlpr__

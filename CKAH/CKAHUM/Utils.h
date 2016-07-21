#if !defined(AFX_UTILS_H__AD851603_20FB_4F73_B71C_DEBE9E93856C__INCLUDED_)
#define AFX_UTILS_H__AD851603_20FB_4F73_B71C_DEBE9E93856C__INCLUDED_

#include <Stuffio/Xerrors.h>
#include "../../windows/Hook/Plugins/Include/fw_ev_api.h"

bool CreateCleanTempIDSFolder (LPCWSTR szwFolderPath);
bool DeleteTempIDSFolder (LPCWSTR szwFolderPath);
void RemoveFileAttribs (LPCWSTR szFilePath);
IOS_ERROR_CODE RemoveAttribsDeleteFileW (LPCWSTR szFilePath);

class CKAHTempFolder
{
public:
	CKAHTempFolder (LPCWSTR szwFolderPath)
	{
		CreateCleanTempIDSFolder (m_szwFolderPath = _wcsdup (szwFolderPath));
	}

	~CKAHTempFolder ()
	{
		DeleteTempIDSFolder (m_szwFolderPath);
		free ((void *)m_szwFolderPath);
	}

protected:
	LPCWSTR m_szwFolderPath;
};

#define MAKEVER64(Hi, Lo) (((__int64)(Hi) << 32) | (Lo))
#define GETVER64HI(v) ((ULONG)((__int64)(v) >> 32))
#define GETVER64LO(v) ((ULONG)((__int64)(v)))


__int64 GetFileVersion64 (LPCWSTR szwFileName);

class CWow64FsRedirectorDisabler
{
	typedef BOOL (WINAPI *tWow64DisableWow64FsRedirection)(PVOID *OldValue);
	typedef BOOL (WINAPI *tWow64RevertWow64FsRedirection)(PVOID OlValue);

	tWow64DisableWow64FsRedirection m_fnWow64DisableWow64FsRedirection;
	tWow64RevertWow64FsRedirection m_fnWow64RevertWow64FsRedirection;

	PVOID m_pOldValue;
	BOOL m_bWow64FsRedirectorDisabled;

public:

	CWow64FsRedirectorDisabler() :
		m_fnWow64DisableWow64FsRedirection(NULL),
		m_fnWow64RevertWow64FsRedirection(NULL),
		m_pOldValue(NULL),
		m_bWow64FsRedirectorDisabled(FALSE)
	{
		if(g_bIsNT)
		{
			HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
			if(hKernel32)
			{
				m_fnWow64DisableWow64FsRedirection = (tWow64DisableWow64FsRedirection)GetProcAddress(hKernel32, "Wow64DisableWow64FsRedirection");
				m_fnWow64RevertWow64FsRedirection = (tWow64RevertWow64FsRedirection)GetProcAddress(hKernel32, "Wow64RevertWow64FsRedirection");

				if(m_fnWow64DisableWow64FsRedirection)
					m_bWow64FsRedirectorDisabled = m_fnWow64DisableWow64FsRedirection(&m_pOldValue);
			}
		}
	}
	
	~CWow64FsRedirectorDisabler()
	{
		if(m_fnWow64RevertWow64FsRedirection && m_bWow64FsRedirectorDisabled)
			m_fnWow64RevertWow64FsRedirection(m_pOldValue);
	}
};

#endif // !defined(AFX_UTILS_H__AD851603_20FB_4F73_B71C_DEBE9E93856C__INCLUDED_)

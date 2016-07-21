// OsVersion.cpp: implementation of the COsVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OsVersion.h"

//////////////////////////////////////////////////////////////////////
// COSVersion Class

KLUTIL::COSVersion::COSVersion()
{
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
	
	if (!GetVersionEx(&vi))
	{
		_RPT0(_CRT_ERROR, "GetVersionEx failed!");
		return;
	}
	if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		m_dwMajor = 0;
		
		if (vi.dwMajorVersion == 4 && vi.dwMinorVersion == 0)
			m_dwMajor = 95;
		else if (vi.dwMajorVersion == 4 && vi.dwMinorVersion == 10)
			m_dwMajor = 98;
		else if (vi.dwMajorVersion == 4 && vi.dwMinorVersion == 90)
			m_dwMajor = 100;
		
		m_dwMinor = HIWORD(vi.dwBuildNumber);
		m_dwBuild = LOWORD(vi.dwBuildNumber);
	}
	else
	{
		m_dwMajor = vi.dwMajorVersion;
		m_dwMinor = vi.dwMinorVersion;
		m_dwBuild = vi.dwBuildNumber & 0xFFFF;
	}
	
	m_dwPlatformID = vi.dwPlatformId;
	m_sCSVersion = vi.szCSDVersion;
}

bool KLUTIL::COSVersion::IsNT() const
{
	return (m_dwPlatformID == VER_PLATFORM_WIN32_NT);
}

bool KLUTIL::COSVersion::IsVista() const
{
	return IsNT() && m_dwMajor >= 6;
}

bool KLUTIL::COSVersion::IsWin64()
{
	static BOOL bIsWow64 = -1;
	if (bIsWow64 == -1)
	{
		typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

		LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
			GetModuleHandle("kernel32"), "IsWow64Process");

		if (!fnIsWow64Process || !fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
			bIsWow64 = FALSE;
	}
	return bIsWow64 != FALSE;
}

bool KLUTIL::COSVersion::IsGlobalNamespaceSupported() const
{
	return (IsNT() && (GetMajor() >= 5));
}

std::string& KLUTIL::COSVersion::GetStrInfo() const
{
	if (!m_sVersion.empty())
		return m_sVersion;
	
	TCHAR szBuffer[256];
	
	if (m_dwPlatformID == VER_PLATFORM_WIN32_WINDOWS)
	{
		if (m_dwMajor == 100)
			_stprintf (szBuffer, _T("Microsoft Windows Me (%ld.%ld.%ld %s)"), (m_dwMinor & 0xFF00) >> 8, m_dwMinor & 0xFF, m_dwBuild, m_sCSVersion);
		else
			_stprintf (szBuffer, _T("Microsoft Windows %u (%ld.%ld.%ld %s)"), m_dwMajor, (m_dwMinor & 0xFF00) >> 8, m_dwMinor & 0xFF, m_dwBuild, m_sCSVersion);
	}
	else
	{
		if (m_dwMajor == 5)
		{
			if (m_dwMinor == 0)
				_stprintf (szBuffer, _T("Microsoft Windows 2000 %s (Build %ld)"), m_sCSVersion, m_dwBuild);
			else if (m_dwMinor == 1)
				_stprintf (szBuffer, _T("Microsoft Windows XP %s (Build %ld)"), m_sCSVersion, m_dwBuild);
			else
				_stprintf (szBuffer, _T("Microsoft Windows NT %s (Build %ld)"), m_sCSVersion, m_dwBuild);
		}
		else
			_stprintf (szBuffer, _T("Microsoft Windows NT %ld.%ld %s (Build %ld)"), m_dwMajor, m_dwMinor, m_sCSVersion, m_dwBuild);
	}
	
	m_sVersion = szBuffer;
	
	return m_sVersion;
}

/*
COSVersion& KLUTIL::COSVersion::operator=(COSVersion& OSVersion)
{
	m_dwMajor = OSVersion.m_dwMajor;
	m_dwMinor = OSVersion.m_dwMinor;
	m_dwBuild = OSVersion.m_dwBuild;
	m_dwPlatformID = OSVersion.m_dwPlatformID;
	m_sCSVersion = OSVersion.m_sCSVersion;
	
	return *this;
}
*/

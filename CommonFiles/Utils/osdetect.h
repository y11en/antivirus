/*!
 * Copyright (c) 2004 Kaspersky Lab
 *
 * \file osdetect.h
 * \author Alexey Zlatov
 * \date 2004-10
 * \brief  ласс проверки версий операционной системы
 * 
 */

#if !defined(__KL_OSDETECT_H)
#define __KL_OSDETECT_H

namespace Utility
{

class OSDetect
{
public:
	OSDetect()
	{
		ZeroMemory(&m_info, sizeof(OSVERSIONINFO));
		m_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		m_isValid = (GetVersionEx(&m_info) ? true : false);
	}
	bool IsValid()
	{
		return m_isValid;
	}
	bool IsPlatform9x()  // Windows 95, Windows 98, or Windows Me
	{
		return (m_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
	}
	bool IsPlatformNT()  // Windows NT, Windows 2000, Windows XP, or Windows .NET Server 2003 family
	{
		return (m_info.dwPlatformId == VER_PLATFORM_WIN32_NT);
	}
	bool IsWinXPOrLater()  // Windows XP or Windows .NET Server 2003 family
	{
		return ((m_info.dwMajorVersion > 5) || (m_info.dwMajorVersion == 5 && m_info.dwMinorVersion > 0));
	}
private:
	bool m_isValid;
	OSVERSIONINFO m_info;
};

}

#endif  // __KL_OSDETECT_H

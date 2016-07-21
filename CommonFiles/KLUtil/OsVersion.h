#if !defined(AFX_OSVERSION_H__DE1392F5_18CC_4CC5_BE7A_39B8FB35C12D__INCLUDED_)
#define AFX_OSVERSION_H__DE1392F5_18CC_4CC5_BE7A_39B8FB35C12D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "defs.h"

namespace KLUTIL
{
	class KLUTIL_API COSVersion
	{
		DWORD m_dwMajor, m_dwMinor, m_dwBuild, m_dwPlatformID;
		std::string m_sCSVersion;
		mutable std::string m_sVersion;
		
	public:
		COSVersion();
		std::string& GetStrInfo() const;
		DWORD GetMajor() const { return m_dwMajor; }
		
		bool IsNT() const;
		bool IsVista() const;
		static bool IsWin64();
		
		bool IsGlobalNamespaceSupported() const;
	};
} // namespace KLUTIL

#endif // !defined(AFX_OSVERSION_H__DE1392F5_18CC_4CC5_BE7A_39B8FB35C12D__INCLUDED_)

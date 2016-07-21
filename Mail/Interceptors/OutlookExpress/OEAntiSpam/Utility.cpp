// Utility.cpp: implementation of the CUtility class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Utility.h"
#include <boost/scoped_array.hpp>
#include <vector>

tstring Utility::GetRegStringValue(CRegKey& reg, LPCTSTR szValueName)
{
	DWORD dwBufSize = 512;
	TCHAR szBuf[512] = {0};
	
	LONG lRes = reg.QueryStringValue(szValueName, szBuf, &dwBufSize);
	if (lRes == ERROR_SUCCESS)
		return szBuf;
	
	if (lRes != ERROR_MORE_DATA)
		return _T("");	// unknown error
	
	// allocate more memory
	boost::scoped_array<TCHAR> newBuf(new TCHAR[dwBufSize]);
	lRes = reg.QueryStringValue(szValueName, newBuf.get(), &dwBufSize);
	if (lRes != ERROR_SUCCESS)
		return _T("");	// unknown error
	
	return newBuf.get();
}

tstring Utility::GetModulePath(HMODULE hModule)
{
	TCHAR szPath[MAX_PATH] = {0};
	if (!GetModuleFileName(hModule, szPath, MAX_PATH))
		return _T("");
	
	TCHAR *szPos = _tcsrchr(szPath, _T('\\'));
	if (szPos) 
		*szPos = _T('\0');
	
	tstring strRet(szPath);
	strRet += _T('\\');
	return strRet;
}

bool Utility::DeleteRegSubkeys(CRegKey& reg)
{
	DWORD dwIndex = 0;
	TCHAR szName[MAX_PATH];
	DWORD dwBufSize = MAX_PATH;
	FILETIME ftDummy;

	std::vector<tstring> arrSubKeys;
	while (RegEnumKeyEx(reg, dwIndex, szName, &dwBufSize, NULL, NULL, NULL, &ftDummy) == ERROR_SUCCESS)
	{
		arrSubKeys.push_back(szName);

		++dwIndex;
		dwBufSize = MAX_PATH;
	}

	std::vector<tstring>::const_iterator it = arrSubKeys.begin();
	for (; it != arrSubKeys.end(); ++it)
	{
		if (RegDeleteKey(reg, it->c_str()) != ERROR_SUCCESS)
		{
			// try to delete subkeys
			CRegKey subkey;
			if (subkey.Open(reg, it->c_str(), KEY_READ | KEY_WRITE) == ERROR_SUCCESS)
			{
				DeleteRegSubkeys(subkey);

				// try again
				RegDeleteKey(reg, it->c_str());
			}
		}
	}

	return true;
}

static bool _IsWin9x()
{
	OSVERSIONINFO vi = {sizeof(vi)};
	GetVersionEx(&vi); // Jeffrey Richter's method of detecting Win9x
	if (vi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
		return false; // it's not Win9x
	
	return true;
}

bool Utility::IsWin9x()
{
	static bool bIsWin9x = _IsWin9x();
	return bIsWin9x;
}

void Utility::MakeStrUpper(tstring &str)
{
	if(!str.empty())
	{
		std::_USE(std::locale(), std::ctype<tstring::traits_type::char_type>).
			toupper(&str[0], &str[str.size()]);
	}
}

void Utility::MakeStrLower(tstring &str)
{
	if(!str.empty())
	{
		std::_USE(std::locale(), std::ctype<tstring::traits_type::char_type>).
			tolower(&str[0], &str[str.size()]);
	}
}

// Utility.h: interface for the CUtility class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITY_H__268E0456_1247_43C6_B781_F951A9A194F1__INCLUDED_)
#define AFX_UTILITY_H__268E0456_1247_43C6_B781_F951A9A194F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Utility  
{
	tstring GetRegStringValue(CRegKey& reg, LPCTSTR szValueName);
	tstring GetModulePath(HMODULE hModule);
	bool DeleteRegSubkeys(CRegKey& reg);
	bool IsWin9x();

	void MakeStrUpper(tstring &str);
	void MakeStrLower(tstring &str);
};

#endif // !defined(AFX_UTILITY_H__268E0456_1247_43C6_B781_F951A9A194F1__INCLUDED_)

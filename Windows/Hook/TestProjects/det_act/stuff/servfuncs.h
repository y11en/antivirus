#ifndef __SERVFUNCS_H
#define __SERVFUNCS_H

#include <windows.h>
#include "mm.h"

typedef WINBASEAPI DWORD (WINAPI* _tpGetLongPathName)(LPCWSTR lpszShortPath, LPWSTR lpszLongPath, DWORD cchBuffer);

class CService
{
public:	
	CService();
	virtual ~CService();

	PWCHAR GetLongPathName(LPWSTR lpszShortPath);
	PWCHAR ReparseExecutePath(PWCHAR pwchPath);
	PWCHAR RegQueryValue(PWCHAR pwchKeyName, PWCHAR pwchValueName);
	INT ParseCmdLine(PWCHAR pwchCmdLine, PWCHAR* params, INT max_params_count);
	unsigned long GetSubstringPosBM(__int8* Str, unsigned long sourcse_strlen, __int8* SubStr, unsigned long SubStrLen);

private:
	_tpGetLongPathName m_pfGetLongPathName;
};

PWCHAR wcsistr(PWCHAR pwcsStr, PWCHAR pwcsSubstr); // no codepage support here!!!
bool MatchWithPattern(PWCHAR String, PWCHAR Pattern, bool bCaseSensetivity); // no codepage support here!!!
bool ComparePath(PWCHAR pwchPath, PWCHAR pwchPath2, bool bPartialMatch = false);

#if !defined (_SERVICE_IMP_)
extern 
#endif
CService g_Service;


#endif // __SERVFUNCS_H
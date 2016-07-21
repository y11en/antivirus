#include "servfuncs.h"
#include "debug.h"
#include "memory.h"
#include "mm.h"
#include <tchar.h>

static DWORD WINAPI GetLongPathNameLocal(LPCWSTR lpszShortPath, LPWSTR lpszLongPath, DWORD cchBuffer)
{
	DWORD retLen = 0;

	PWCHAR pwchMask = (PWCHAR) CopyBufToNewBlock((void*) lpszShortPath, _WSTR_LEN_B(lpszShortPath));
	if (NULL == pwchMask)
		return 0;

	_autoFree aupTmp(&pwchMask);

	WIN32_FIND_DATA df;
	WCHAR wch;
	int cou = 0;

	HANDLE hFind = INVALID_HANDLE_VALUE;
	while (true)
	{
		while (0 != pwchMask[cou])
		{
			if ('\\' == pwchMask[cou])
				break;
			if (':' != pwchMask[cou])
				cou++;
			else
			{
				cou++;
				if ('\\' == pwchMask[cou])
					cou++;

				if (NULL != lpszLongPath)
				{
					lstrcpyn(lpszLongPath, lpszShortPath, cou);
					retLen = cou - 1;
				}

			}
		}

		wch = pwchMask[cou];
		pwchMask[cou] = 0;

		hFind = FindFirstFile(pwchMask, &df);
		if (INVALID_HANDLE_VALUE == hFind)
			retLen = 0;
		else
		{
			retLen++;		// '\'
			retLen += lstrlen(df.cFileName);

			if (lpszLongPath != 0)
			{
				if (cchBuffer <= retLen)
				{
					return 0;
				}
				else
				{
					lstrcat(lpszLongPath, _T("\\"));
					lstrcat(lpszLongPath, df.cFileName);
				}
			}
		}

		pwchMask[cou] = wch;

		if (0 == pwchMask[cou])
			break;

		cou++;
	}
	
	return retLen;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CService::CService()
{
	m_pfGetLongPathName = NULL;

	OSVERSIONINFO OsInfo;
	OsInfo.dwOSVersionInfoSize = sizeof(OsInfo);
	GetVersionEx(&OsInfo);

	if (OsInfo.dwMajorVersion >= 5)
	{
		HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
		if (NULL != hKernel32)
			m_pfGetLongPathName = (_tpGetLongPathName) GetProcAddress(hKernel32, "GetLongPathNameW");
	}
	
	if (NULL == m_pfGetLongPathName)
		m_pfGetLongPathName = GetLongPathNameLocal;
}

CService::~CService()
{
}

unsigned long CService::GetSubstringPosBM(__int8* Str, unsigned long sourcse_strlen, __int8* SubStr, unsigned long SubStrLen)
{
	unsigned __int8 Table[0x100];
	int i;
	__int8 *PStr,*PEndStr,*PSubStr,*PEndSubStr;
	__int8 *PStrCur;
	if ((SubStrLen==0) || (sourcse_strlen<SubStrLen)) 
		return -1;
	FillMemory(Table, sizeof(Table), SubStrLen);
	for(i = SubStrLen - 2; i >= 0; i--)
	{
		if (Table[SubStr[i]] == SubStrLen)
			Table[SubStr[i]] = (__int8) (SubStrLen - i - 1);
	}
	PSubStr = PEndSubStr = SubStr+SubStrLen - 1;
	PStrCur = PStr=Str + SubStrLen - 1;
	PEndStr = Str + sourcse_strlen;
	do {
		if((*PStr) == (*PSubStr))
		{
			if (PSubStr == SubStr) 
				return PStr - Str;
			PStr--;
			PSubStr--;
		} 
		else 
		{
			PSubStr = PEndSubStr;
			PStr = PStrCur += Table[(*PStrCur)];
		}
	} while (PStr<PEndStr);
	
	return -1;
}

#define IS_DELIM(wc) (wc <= ' ' || wc == ',')
#define NEXT_CHAR() *(++pwchPtr)
// !! this function modifies data pointed with pwchCmdLine parameter!
INT CService::ParseCmdLine(PWCHAR pwchCmdLine, PWCHAR* params, INT max_params_count)
{
	INT params_count = 0;
	PWCHAR pwchPtr = pwchCmdLine;
	if (pwchCmdLine == NULL)
		return params_count;
	while (params_count < max_params_count)
	{
		WCHAR wc = *pwchPtr;
		
		// skip delimiters at start
		while (wc != 0 && IS_DELIM(wc))
			wc = NEXT_CHAR();
		
		if (wc == 0) // end reached
			break; 

		if (wc == '\"') // quoted param
		{
			wc = NEXT_CHAR();
			if (wc == 0) // end reached
				break;
			params[params_count] = pwchPtr;
			params_count++;
			while (wc != 0 && wc != '\"')
				wc = NEXT_CHAR();
		}
		else
		{
			if (wc != '/') // if not switch
			{
				params[params_count] = pwchPtr;
				params_count++;
			}
			while (wc != 0 && !IS_DELIM(wc))
				wc = NEXT_CHAR();
		}
		if (wc == 0) // end reached
			break;
		*pwchPtr = 0; // zero terminate parameter
		wc = NEXT_CHAR();
	}
	return params_count;
}
#undef IS_DELIM
#undef NEXT_CHAR

//+ ------------------------------------------------------------------------------------------

#define _REGISTRY_USER_				(_T("\\REGISTRY\\USER\\"))
#define _REGISTRY_MACHINE_			(_T("\\REGISTRY\\MACHINE\\"))

PWCHAR CService::RegQueryValue(PWCHAR pwchKeyName, PWCHAR pwchValueName)
{
	HKEY KeyRoot = 0;
	HKEY hreg = 0;
	int KeyNameLen = 0;
	int len = lstrlen(pwchKeyName);
	
	unsigned long pos = GetSubstringPosBM((__int8*)pwchKeyName,  len * sizeof(WCHAR), (__int8*)_REGISTRY_USER_, 
		_WSTR_CONST_LEN_B(_REGISTRY_USER_));
	if (-1 != pos)
	{
		KeyRoot = HKEY_USERS;
		KeyNameLen = _WSTR_CONST_LEN_WC(_REGISTRY_USER_);
	}
	else
	{
		pos = GetSubstringPosBM((__int8*)pwchKeyName,  len * sizeof(WCHAR), (__int8*)_REGISTRY_MACHINE_, 
			_WSTR_CONST_LEN_B(_REGISTRY_MACHINE_));
		
		KeyRoot = HKEY_LOCAL_MACHINE;
		KeyNameLen = _WSTR_CONST_LEN_WC(_REGISTRY_MACHINE_);
	}

	if (-1 == pos)
	{
		DbPrint((DCB_BL, DL_WARNING, _T("_RegQueryValue: can't recognize reg prefix for '%s'"), pwchKeyName));
		return NULL;
	}

	LONG res = RegOpenKey(KeyRoot, pwchKeyName + KeyNameLen, &hreg);
	
	if (ERROR_SUCCESS != res)
	{
		DbPrint((DCB_BL, DL_WARNING, _T("_RegQueryValue: can't open key %s"), pwchKeyName + KeyNameLen));
		return NULL;
	}

	DWORD type = REG_SZ;
	DWORD dwValLen;
	PWCHAR pVal = NULL;
	
	res = RegQueryValueEx(hreg, pwchValueName, NULL, &type, NULL, &dwValLen);
	if (ERROR_SUCCESS != res)
	{
		DbPrint((DCB_BL, DL_WARNING, _T("_RegQueryValue: can't query value err %#x for %s"), res, pwchValueName));
	}
	else
	if (type == REG_SZ || type == REG_EXPAND_SZ || type == REG_MULTI_SZ)
	{
		pVal = (PWCHAR) global_Alloc(dwValLen);
		if (NULL != pVal)
		{
			res = RegQueryValueEx(hreg, pwchValueName, NULL, &type, (LPBYTE) pVal, &dwValLen);
			if (ERROR_SUCCESS != res)
			{
				DbPrint((DCB_BL, DL_WARNING, _T("_RegQueryValue: can't query value(buff) err %#x for %s"), res, pwchValueName));
				global_Free(&pVal);
			}
		}
	}
	
	RegCloseKey(hreg);
	return pVal;
}

//+ ------------------------------------------------------------------------------------------
static int reverse_find(PWCHAR pwchStr, WCHAR symb)
{
	for (int cou = lstrlen(pwchStr); cou > 0; cou--)
	{
		if (pwchStr[cou] == symb)
			return cou;
	}
	
	return 0;
}

#define _STR_QUOTA_					(_T("\""))

PWCHAR CService::GetLongPathName(PWCHAR pwchShortPath)
{
	//! return new allocated string with long file name
	PWCHAR pwchRet = NULL;
	
	if (wcschr(pwchShortPath, '%'))
	{
		DWORD size = ExpandEnvironmentStrings(pwchShortPath, NULL, 0);
		pwchRet = (PWCHAR)global_Alloc(size*sizeof(WCHAR));
		if (NULL != pwchRet)
		{
			if (ExpandEnvironmentStrings(pwchShortPath, pwchRet, size))	
				pwchShortPath = pwchRet;
			else
				FreeStr(pwchRet);
		}
	}

	if (wcschr(pwchShortPath, '~'))
	{
		DWORD new_len = m_pfGetLongPathName(pwchShortPath, NULL, 0);
		if (0 != new_len)
		{
			new_len++;
			PWCHAR pwchLongPath = (PWCHAR)global_Alloc(new_len * sizeof(WCHAR));
			if (NULL != pwchLongPath)
			{
				if (m_pfGetLongPathName(pwchShortPath, pwchLongPath, new_len))
				{
					FreeStr(pwchRet);
					pwchRet = pwchLongPath;
				}
				else
					FreeStr(pwchLongPath);
			}
		}
	}

	if (NULL == pwchRet)
		pwchRet = GetCopyStr(pwchShortPath);
	else
		DbPrint((DCB_BL, DL_IMPORTANT, _T("GetLongPathName: GetLongPathName result '%s' (was:%s)"), (pwchRet ? pwchRet : L"<NULL>"), pwchShortPath));
	return pwchRet;
}


PWCHAR CService::ReparseExecutePath(PWCHAR pwchPath)
{
	//! return new allocated string with file name
	int len = wcslen(pwchPath);
	int tmplen = len;
	int spaces = 0;
	int end_spaces = 0;

	while (spaces < len && pwchPath[spaces] == ' ')
		spaces++;

	while (len > 0 && pwchPath[--len] == ' ')
		end_spaces++;

	len = tmplen - spaces - end_spaces;

	PWCHAR pwchVal = (PWCHAR) CopyBufToNewBlock(pwchPath + spaces, (len + 1) * sizeof(WCHAR));
	
	if (NULL == pwchVal)
		return NULL;

	pwchVal[len] = 0;

	_autoFree aupVal(&pwchVal);

	PWCHAR pwchRet = NULL;

	if (len < MAX_PATH)
	{
		PWCHAR pwchRetTmp = (PWCHAR) global_Alloc(MAX_PATH * sizeof(WCHAR));
		if (NULL != pwchRetTmp)
		{
			_autoFree aupRetTmp(&pwchRetTmp);

			while (len > 0)
			{
				DWORD hins = (DWORD) FindExecutable(pwchVal, NULL, pwchRetTmp);
				if (hins > 32)
				{
					DWORD new_len = m_pfGetLongPathName(pwchRetTmp, NULL, 0);
					if (0 != new_len)
					{
						new_len++;
						pwchRet = (PWCHAR) global_Alloc(new_len * sizeof(WCHAR));
						if (NULL != pwchRet)
						{
							m_pfGetLongPathName(pwchRetTmp, pwchRet, new_len);
							DbPrint((DCB_BL, DL_IMPORTANT, _T("Reparse_ExecutePath: GetLongPathName result'%s'"), pwchRet));
						}

						return pwchRet;
					}
				}
				else
				{
					DWORD pos;
					WCHAR symb = pwchVal[len - 1];
					pwchVal[len - 1] = 0;
					if (symb == '\"')
						pos = reverse_find(pwchVal, '\"');
					else
						pos = reverse_find(pwchVal, ' ');

					pwchVal[pos--] = 0;
					while (pos > 0 && pwchVal[pos] == ' ')
						pwchVal[pos--] = 0;

					len = lstrlen(pwchVal);
					DbPrint((DCB_BL, DL_IMPORTANT, _T("Reparse_ExecutePath: FindExecutable failed. Manual parsed to '%s'"), pwchVal));
				}
			}
		}

		return pwchRet;
	}

	unsigned long pos;

//+ ------------------------------------------------------------------------------------------
//+ "long file name"
	if ('\"' == pwchVal[0])
	{
		// very simple case: LongFileName
		pos = g_Service.GetSubstringPosBM((__int8*)(pwchVal + 1),  len * sizeof(WCHAR), (__int8*)_STR_QUOTA_, 
			_WSTR_CONST_LEN_B(_STR_QUOTA_));
		if (-1 == pos)
			return pwchRet;

		pwchRet = (PWCHAR) global_Alloc(pos + sizeof(WCHAR));
		if (NULL != pwchRet)
		{
			memcpy(pwchRet, pwchVal + 1, pos);
			pwchRet[pos / sizeof(WCHAR)] = 0;

			//! check - regsvr32, rundll32 etc...
			return pwchRet;
		}
		return pwchRet;
	}


//- "long file name"
//+ ------------------------------------------------------------------------------------------

	//! other way -  search as CreateProcess works
	DbPrint((DCB_BL, DL_WARNING, _T("Reparse_ExecutePath: //! other way -  search as CreateProcess works")));
	
	return pwchRet;
}

#define LCASE(c) (c>='A'? (c<='Z'? (c|0x20) : c) : c)

PWCHAR wcsistr(PWCHAR s, PWCHAR ss) // no codepage support here!!!
{
	WCHAR c, cs;
	if (!s || !ss)
		return NULL;
	cs = LCASE(*ss);
	while (*s)
	{
		c = LCASE(*s);
		if (c == cs)
		{
			int i = 1;
			while (true) 
			{
				WCHAR  cs = LCASE(ss[i]);
				c = LCASE(s[i]);
				if (0 == cs) // found
					return s;
				if (0 == c)  // end of string
					return NULL;
				if (c != cs) // not matched
					break;
				i++;
			}
		}
		s++;
	}
	return NULL;
}

bool MatchOkay(PWCHAR Pattern)
{
	if(*Pattern && *Pattern != L'*')
		return FALSE;
	return TRUE;
}

bool MatchWithPattern(PWCHAR String, PWCHAR Pattern, bool CaseSensetivity)
{
	WCHAR ch;
	if(*Pattern==L'*')
	{
		Pattern++;
		while(*String && *Pattern)
		{
			if (CaseSensetivity == TRUE)
				ch = *String;
			else
				ch = LCASE(*String);
			if ((ch == L'*') || (ch == *Pattern) || (*Pattern == '?'))
			{
				if(MatchWithPattern(String+1, Pattern+1, CaseSensetivity)) 
					return TRUE;
			}
			String++;
		}
		return MatchOkay(Pattern);
	} 
	
	while(*String && *Pattern != L'*')
	{
		if (CaseSensetivity == TRUE)
			ch = *String;
		else
			ch = LCASE(*String);
		
		if((*Pattern == ch) || (*Pattern == '?'))
		{
			Pattern++;
			String++;
		} else
			return FALSE;
	}
	
	if(*String)
		return MatchWithPattern(String, Pattern, CaseSensetivity);
	
	return MatchOkay(Pattern);
}


static PWCHAR CutUnicodePrefix(PWCHAR pwcsPath)
{
	PWCHAR p = pwcsPath;
	if (NULL == p)
		return p;
	if (p[0] == '\\' && p[1] == '\\' && p[2] == '?' && p[3] == '\\')
		return p+4;
	return p;
}

bool ComparePath(PWCHAR pwchPath, PWCHAR pwchPath2, bool bPartialMatch)
{
	PWCHAR p1, p2, cp1, cp2;
	int cmpres = 1;

	if (pwchPath == NULL || pwchPath2 == NULL)
		return false;
	
	p1 = g_Service.GetLongPathName(pwchPath);
	p2 = g_Service.GetLongPathName(pwchPath2);

	if (p1 && p2)
	{
		cp1 = CutUnicodePrefix(p1);
		cp2 = CutUnicodePrefix(p2);

		if (bPartialMatch == false)
			cmpres = wcsnicmp(cp1, cp2, wcslen(cp2));
		else
			cmpres = wcscmp(cp1, cp2);
	}
	FreeStr(p1);
	FreeStr(p2);
	return (cmpres == 0);
}

#include "defines.h"

#include "Initialization.h"
#include "regenum.h"

#define false 0
#define true 1

#define PATH_PREALLOC_SIZE ((max(MAX_VALUE_NAME, MAX_KEY_LENGTH) + 2)*sizeof(wchar_t))

int _wcscpync(wchar_t* wcsDst, const wchar_t* wcsSrc, int nMax, wchar_t wcStopChar)
{
	int i;
	for (i=0; i<nMax; i++)
	{
		wchar_t wc;
		wc = wcsSrc[i];
		if (wcStopChar == wc)
			wc = 0;
		wcsDst[i] = wc;
		if (0 == wc)
			break;
	}
	return i;
}

int _wcscmpc(const wchar_t* sStr1, const wchar_t* sStr2, wchar_t wcStopChar)
{
	wchar_t wc;
	wchar_t wc2;
	if (!sStr1 || !sStr2)
		return -1;
	do
	{
		wc = *sStr1++;
		if (wc == wcStopChar)
			wc = 0;
		wc2 = *sStr2++;
		if (wc2 == wcStopChar)
			wc2 = 0;
		if (wc < wc2)
			return -1;
		if (wc > wc2)
			return 1;
	} while (wc);

	return 0;
}

bool _ispattern(const wchar_t* sName, wchar_t wcStopChar)
{
	wchar_t wc;
	if (!sName)
		return false;
	do
	{
		wc = *sName;
		if (wc == wcStopChar)
			return false;
		if (wc == '*' || wc == '?')
			return true;
		sName++;
	} while (wc);
	
	return false;
}

bool MatchOkay(wchar_t* Pattern, int chStopChar)
{
	if (*Pattern == L'*')
		Pattern++;
	if (*Pattern == chStopChar || *Pattern == 0)
		return true;
	return false;
}

bool MatchWithPattern(wchar_t* String, wchar_t* Pattern, bool CaseSensetivity, int chStopChar, DWORD* step)
{
	wchar_t ch;
	wchar_t chp;
	if(*Pattern == '*')
	{
		Pattern++;
		ch = *String;
		chp = *Pattern;
		while(chp != 0 && ch != 0 && ch != chStopChar)
		{
			if (!CaseSensetivity)
			{
				ch = LCASE(ch);
				chp = LCASE(chp);
			}
			if ((ch == '*') || (ch == chp) || (chp == '?'))
			{
				if(MatchWithPattern(String+1, Pattern+1, CaseSensetivity, chStopChar, step)) 
					return TRUE;
			}
			String++;
			ch = *String;
		}
		return MatchOkay(Pattern, chStopChar);
	} 
	
	ch = *String;
	chp = *Pattern;
	while(chp != '*' && ch != 0 && ch != chStopChar)
	{
		if (!CaseSensetivity)
		{
			ch = LCASE(ch);
			chp = LCASE(chp);
		}
		
		if(chp == ch || chp == '?')
		{
			Pattern++;
			String++;
			if (step)
				(*step)++;
			ch = *String;
			chp = *Pattern;
		} else
			return FALSE;
	}
	
	if(*String && *String != chStopChar)
		return MatchWithPattern(String, Pattern, CaseSensetivity, chStopChar, step);
	
	return MatchOkay(Pattern, chStopChar);
}


HKEY cRegEnumCtx::GetRegistryRoot(const wchar_t* sRoot, wchar_t** ppSubPath, const wchar_t** ppPath)
{
	if (!_wcscmpc(sRoot, L"HKLM", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 5;
		if (ppSubPath)
			*ppSubPath = NULL;
		return HKEY_LOCAL_MACHINE;
	}
	if (!_wcscmpc(sRoot, L"HKEY_LOCAL_MACHINE", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 19;
		if (ppSubPath)
			*ppSubPath = NULL;
		return HKEY_LOCAL_MACHINE;
	}
	if (!_wcscmpc(sRoot, L"HKU", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 4;
		if (ppSubPath)
			*ppSubPath = NULL;
		return HKEY_USERS;
	}
	if (!_wcscmpc(sRoot, L"HKEY_USERS", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 11;
		if (ppSubPath)
			*ppSubPath = NULL;
		return HKEY_USERS;
	}
	if (!_wcscmpc(sRoot, L"HKCR", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 5;
		if (ppSubPath)
			wcscpy(*ppSubPath,L"*\\Software\\Classes");
		return HKEY_ALL;
	}
	if (!_wcscmpc(sRoot, L"HKEY_CLASSES_ROOT", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 18;
		if (ppSubPath)
			wcscpy(*ppSubPath,L"*\\Software\\Classes");
		return HKEY_ALL;
	}
	if (!_wcscmpc(sRoot, L"HKCU", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 5;
		if (ppSubPath)
			wcscpy(*ppSubPath,L"*");
		return HKEY_USERS;
	}
	if (!_wcscmpc(sRoot, L"HKEY_CURRENT_USER", '\\')) 
	{
		if (ppPath)
			*ppPath = sRoot + 18;
		if (ppSubPath)
			wcscpy(*ppSubPath,L"*");
		return HKEY_USERS;
	}
	return NULL;
}

long cRegEnumCtx::_iRegEnum(HKEY hKey, const wchar_t* sKey, const wchar_t* sSubKeys, DWORD dwPathSize, tRegEnumCtx* pCtx, wchar_t*& sName, BYTE*& pValue)
{
	bool		bPattern;
	HKEY		hSubKey = NULL;
	long		win32err;
	DWORD		dwSize;
	int			i;
	int			nEnumErrors;
	bool		bSubkeyRoot=0;
	bool		bInsKey=1;
	long		Ret=ERROR_REGOP_FALSE;

	if (pCtx->m_dwPathAllocated < dwPathSize + PATH_PREALLOC_SIZE)
	{
		dwSize = pCtx->m_dwPathAllocated;
		pCtx->m_dwPathAllocated = dwPathSize + PATH_PREALLOC_SIZE + 4*1024;
		if (dwSize) 
			pCtx->m_sPath = (wchar_t*)HeapReAlloc(GetProcessHeap(), 0, pCtx->m_sPath, pCtx->m_dwPathAllocated*sizeof(wchar_t));
		else 
			pCtx->m_sPath = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, pCtx->m_dwPathAllocated*sizeof(wchar_t));
		if (!pCtx->m_sPath) 
			return Ret; // errNOT_ENOUGH_MEMORY;
	}

	// try retrieve next key to open/enum
	if (sKey == NULL)
	{
		sKey = sSubKeys;
		bInsKey=0;
		if (sSubKeys)
		{
			sSubKeys = wcschr(sSubKeys, '\\');
			if (sSubKeys) sSubKeys++;
		}
	}

	// try retrieve next key to open/enum
	if (hKey == NULL) // root
	{
		if (sKey == NULL) return Ret; // errPARAMETER_INVALID;
		if (sKey[0] == '*' && (sKey[1] == 0 || sKey[1] == '\\'))
		{
			wcscpy(pCtx->m_sPath, L"HKEY_LOCAL_MACHINE");
			if ((wcslen(sKey)>2) && bInsKey)
				win32err=_RegEnum(HKEY_LOCAL_MACHINE, sKey+2, sSubKeys, 18, pCtx);
			else
				win32err=_RegEnum(HKEY_LOCAL_MACHINE, NULL, sSubKeys, 18, pCtx);
			if (win32err==ERROR_REGOP_CANCELED)
			{
				Ret=win32err;
				return Ret;
			}
			if (!win32err) return Ret;
			wcscpy(pCtx->m_sPath, L"HKEY_USERS");
//			win32err=_RegEnum(HKEY_USERS, L"*", sSubKeys, 10, pCtx);
			cPrStrW sNewKey=L"*";
			if ((wcslen(sKey)>2) && bInsKey)
				sNewKey.append_path(sKey+2);
			win32err=_RegEnum(HKEY_USERS, (wchar_t*)sNewKey, sSubKeys, 10, pCtx);
			if (win32err==ERROR_REGOP_CANCELED)
			{
				Ret=win32err;
				return Ret;
			}
			if (!win32err) return Ret;
		}
		else
		{
			wchar_t  pSubKey[30]=L"";
			wchar_t* lpSubKey=pSubKey;
			const wchar_t* sRootSubKey;
			Ret=ERROR_REGOP_TRUE;
			hKey = GetRegistryRoot(sKey, &lpSubKey, &sRootSubKey);
			if (!hKey) return Ret; // errPARAMETER_INVALID;
			while (sKey != sRootSubKey-1)
			{
				pCtx->m_sPath[dwPathSize] = *sKey;
				dwPathSize++;
				sKey++;
			}
			if (hKey==HKEY_ALL)
				hKey=NULL;
			win32err=_RegEnum(hKey, lpSubKey, sRootSubKey, dwPathSize, pCtx); // stop precessing
			if (win32err==ERROR_REGOP_CANCELED)
			{
				Ret=win32err;
				return Ret;
			}
			if (!win32err) return Ret;
		}
		return Ret; // errOK;
	}

	// enumerate next key level
	if (sKey != NULL && *sKey != 0) 
	{
		nEnumErrors = 0;
		pCtx->m_sPath[dwPathSize++] = '\\';
		pCtx->m_sPath[dwPathSize] = 0;
		sName = pCtx->m_sPath + dwPathSize;
		bPattern = _ispattern(sKey, '\\');
		for (i=0;;i++)
		{
			if (bPattern)
			{
				cPrStrW wsName((wchar_t*)sName,300);
				win32err = pfRegEnumKey(hKey, i, wsName);
//				dwSize = wsName.getlen();
				if (win32err == ERROR_NO_MORE_ITEMS) break;
				if (win32err != ERROR_SUCCESS)
				{
					if (nEnumErrors > 5/* && nEnumErrors == i*/) break;
					nEnumErrors++;
					continue;
				}
				if (wsName.allocated_len()>300)
					wcscpy(sName,(wchar_t*)wsName);
				dwSize=1;
				if (!MatchWithPattern(sName, const_cast<wchar_t*>(sKey), false, '\\', &dwSize)) continue;
			}
			else 
			{
				if (i != 0) break;
				dwSize = _wcscpync(sName, sKey, MAX_KEY_LENGTH, '\\');
			}
			if ((pCtx->m_dwFlags&FLAG_ENUM_KEYS)&&(pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED))
			{
				pCtx->m_sPath[dwPathSize-1]=0;
//INT3;
				win32err=pCtx->m_pEnumCallbackFuncW(pCtx->m_pEnumCallbackContext,hKey,pCtx->m_sPath,sName,(BYTE*)sName,(wcslen(sName)+1)*2,REG_KEY);
				if (win32err==ERROR_REGOP_DEL_THISKEY)
				{
//INT3;
					win32err=pfRegDeleteKey(hKey,sName);
					if (win32err==ERROR_SUCCESS) i--;
					win32err=pCtx->m_pEnumCallbackErrMessageFuncW(pCtx->m_pEnumCallbackContext,pCtx->m_sPath,sName,win32err,win32err!=ERROR_SUCCESS);
					if ((!win32err)|(win32err==ERROR_REGOP_CANCELED))
					{
						Ret=win32err;
						return Ret;
					}
					continue;
				}
				if ((win32err==ERROR_REGOP_RECOUNT_KEY)|(!win32err))
				{
					Ret=win32err;
					return Ret;
				}
				continue;
			}
			win32err = pfRegOpenKey(hKey, sName, &hSubKey);
			if (win32err != ERROR_SUCCESS) continue;
			if (pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED)
			{
				wchar_t	CurChar=pCtx->m_sPath[dwPathSize-1];
				pCtx->m_sPath[dwPathSize-1]=0;
				win32err=pCtx->m_pEnumCallbackFuncW(pCtx->m_pEnumCallbackContext,hKey,pCtx->m_sPath,sName,NULL,0,REG_OPEN_KEY);
				if (win32err==ERROR_REGOP_CANCELED)
				{
					pfRegCloseKey(hSubKey);
					Ret=win32err;
					return Ret;
				}
				pCtx->m_sPath[dwPathSize-1]=CurChar;
			}
			if (m_bWOW3264Exist && pfCheckHkey(hKey,HKEY_LOCAL_MACHINE) && !_wcsicmp(sName,L"software"))
			{
				win32err=ERROR_REGOP_RECOUNT_KEY;
				while (win32err==ERROR_REGOP_RECOUNT_KEY)
				{
					if (bInsKey && (((*(sKey+dwSize)!=0) && (*(sKey+dwSize)=='\\')) || ((*(sKey+dwSize)=='\\') && (*(sKey+dwSize+1)!=0))))
					{
						DWORD dwAllocated = 1024; // for test only
						wchar_t* sKeyCopy = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, dwAllocated);
						if (!sKeyCopy)
							break;
						wcscpy(sKeyCopy,L"Wow6432Node\\");
						if (*(sKey+dwSize)=='\\')
							wcscpy(sKeyCopy+12,sKey+dwSize+1);
						else
							wcscpy(sKeyCopy+12,sKey+dwSize);
						if (sKeyCopy[11]==0)
							sKeyCopy[10]=0;
						win32err=_RegEnum(hSubKey, sKeyCopy, sSubKeys, dwPathSize+wcslen(sName)/*dwSize*/, pCtx);
//						win32err=_RegEnum(hSubKey, *(sKey+dwSize)=='\\' ? sKey+dwSize+1 : sKey+dwSize, sSubKeys, dwPathSize+dwSize, pCtx);
						HeapFree(GetProcessHeap(),0,sKeyCopy);
					}
					else
						win32err=_RegEnum(hSubKey, L"Wow6432Node", sSubKeys, dwPathSize+wcslen(sName)/*dwSize*/, pCtx);
					if (((win32err==ERROR_REGOP_RECOUNT_KEY)&(pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED))|(win32err==ERROR_REGOP_CANCELED)|(!win32err)|(win32err==ERROR_REGOP_RECOUNT_KEY_FROM_VALUE))
					{
						if (win32err==ERROR_REGOP_RECOUNT_KEY_FROM_VALUE)
							win32err=ERROR_REGOP_RECOUNT_KEY;
						if (win32err)
							Ret=win32err;
						/* VIK 04.09.07 TT #14624
						Значение REG_CLOSE_KEY нигде не отрабатывается. Но здесь сбивается значение win32err и ключ не удаляется
						if (pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED)
						{
							win32err=pCtx->m_pEnumCallbackFuncW(pCtx->m_pEnumCallbackContext,hKey,NULL,NULL,NULL,0,REG_CLOSE_KEY);
							if (win32err==ERROR_REGOP_CANCELED) 
								Ret=win32err;
						}
						*/
						pfRegCloseKey(hSubKey);
						return Ret;
					}
				}
			}
			win32err=ERROR_REGOP_RECOUNT_KEY;
			while (win32err==ERROR_REGOP_RECOUNT_KEY)
			{
				if (bInsKey && (((*(sKey+dwSize)!=0) && (*(sKey+dwSize)=='\\')) || ((*(sKey+dwSize)=='\\') && (*(sKey+dwSize+1)!=0))))
					win32err=_RegEnum(hSubKey, *(sKey+dwSize)=='\\' ? sKey+dwSize+1 : sKey+dwSize, sSubKeys, dwPathSize+wcslen(sName)/*dwSize*/, pCtx);
				else
					win32err=_RegEnum(hSubKey, NULL, sSubKeys, dwPathSize+wcslen(sName)/*dwSize*/, pCtx);
				if (((win32err==ERROR_REGOP_RECOUNT_KEY)&(pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED))|(win32err==ERROR_REGOP_CANCELED)|(!win32err)|(win32err==ERROR_REGOP_RECOUNT_KEY_FROM_VALUE))
				{
					if (win32err==ERROR_REGOP_RECOUNT_KEY_FROM_VALUE)
						win32err=ERROR_REGOP_RECOUNT_KEY;
					if (win32err)
						Ret=win32err;
					/* VIK 04.09.07 TT #14624
					Значение REG_CLOSE_KEY нигде не отрабатывается. Но здесь сбивается значение win32err и ключ не удаляется
					if (pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED)
					{
						win32err=pCtx->m_pEnumCallbackFuncW(pCtx->m_pEnumCallbackContext,hKey,NULL,NULL,NULL,0,REG_CLOSE_KEY);
						if (win32err==ERROR_REGOP_CANCELED) 
							Ret=win32err;
					}
					*/
					pfRegCloseKey(hSubKey);
					return Ret;
				}
			}
			if (win32err == ERROR_REGOP_NOT_CLOSE_THIS_KEY)
				return ERROR_REGOP_TRUE;
			/* VIK 04.09.07 TT #14624
			   Значение REG_CLOSE_KEY нигде не отрабатывается. Но здесь сбивается значение win32err и ключ не удаляется
			if (pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED)
			{
				win32err=pCtx->m_pEnumCallbackFuncW(pCtx->m_pEnumCallbackContext,hKey,NULL,NULL,NULL,0,REG_CLOSE_KEY);
				if (win32err==ERROR_REGOP_CANCELED) return win32err;
			}
			*/
			pfRegCloseKey(hSubKey);
			if /*(*/(win32err==ERROR_REGOP_DEL_KEY)/*|(win32err==ERROR_REGOP_LSP_RESTORE_KEY))*/
			{
				if (pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED)
				{
					Ret=win32err;
					return Ret;
				}
//INT3;
				if (pCtx->m_dwFlags&FLAG_REG_FILTERS_EXIST)
				{
//INT3;
					wchar_t Enum[]=L"HKLM\\SYSTEM\\*\\Enum\\*";
					sSubKeyName=sName;
					win32err=RegEnumWW(Enum,L"LowerFilters",FLAG_REG_WITH_SUBKEYS,this,CheckFilter,pCtx->m_pEnumCallbackErrMessageFuncW);
					win32err=RegEnumWW(Enum,L"UpperFilters",FLAG_REG_WITH_SUBKEYS,this,CheckFilter,pCtx->m_pEnumCallbackErrMessageFuncW);
					win32err=RegEnumWW(Enum,L"ActiveChannel",FLAG_REG_WITH_SUBKEYS,this,CheckFilter,pCtx->m_pEnumCallbackErrMessageFuncW);
					win32err=RegEnumWW(Enum,L"ActiveServer",FLAG_REG_WITH_SUBKEYS,this,CheckFilter,pCtx->m_pEnumCallbackErrMessageFuncW);
					sSubKeyName=NULL;
				}
				win32err=pfRegDeleteKey(hKey,sName);
				if (win32err==ERROR_SUCCESS) i--;
				win32err=pCtx->m_pEnumCallbackErrMessageFuncW(pCtx->m_pEnumCallbackContext,pCtx->m_sPath,sName,win32err,win32err!=ERROR_SUCCESS);
				if ((!win32err)|(win32err==ERROR_REGOP_CANCELED))
				{
					Ret=win32err;
					return Ret;
				}
			}
		}
		Ret=ERROR_REGOP_TRUE;
		return Ret;
	}

	if (pCtx->m_dwFlags&FLAG_REG_CLSID_INIT)
	{
		if (PR_FAIL(m_pStartUpEnum->heapRealloc((void**)&CLSIDKeys.pKeys,CLSIDKeys.pKeys,(CLSIDKeys.Count+1)*sizeof(DWORD))))
			return ERROR_REGOP_FALSE;
		CLSIDKeys.pKeys->hKey[CLSIDKeys.Count]=hKey;
		CLSIDKeys.Count++;
		return ERROR_REGOP_NOT_CLOSE_THIS_KEY;
	}
	if (!(pCtx->m_dwFlags&FLAG_ENUM_KEYS))
	{
		// if no more keys - enum values
		DWORD dwAllocated = 4096; // for test only
		DWORD dwType;
		const wchar_t* sValue = pCtx->m_sValue;
		pValue = (BYTE*)HeapAlloc(GetProcessHeap(), 0, dwAllocated);
		Ret=ERROR_REGOP_TRUE;
		
		if (!pValue) return Ret; // errNOT_ENOUGH_MEMORY;
		nEnumErrors = 0;
		(pCtx->m_sPath)[dwPathSize] = 0;
		sName = pCtx->m_sPath + dwPathSize+1;
		bPattern = _ispattern(sValue, 0);
		for (i=0;;i++)
		{	
			if (bPattern)
			{
				cPrStrW wsName((wchar_t*)sName,300);
				dwSize = MAX_VALUE_NAME;
				win32err = pfRegEnumValue(hKey, (DWORD)i, wsName);
				if (win32err == ERROR_NO_MORE_ITEMS) break;
				if (win32err != ERROR_SUCCESS)
				{
					if (nEnumErrors > 5/* && nEnumErrors == i*/) break;
					nEnumErrors++;
					continue;
				}
				if (wsName.allocated_len()>300)
					wcscpy(sName,(wchar_t*)wsName);
				if (!MatchWithPattern(sName, const_cast<wchar_t*>(sValue), false, 0, NULL)) continue;
			}
			else 
			{
				if (i != 0) break;
				dwSize = _wcscpync(sName, sValue, MAX_VALUE_NAME, 0);
			}
			dwSize = dwAllocated;
			win32err = pfRegQueryValueEx(hKey, sName, &dwType, pValue, &dwSize);
			if (win32err == ERROR_MORE_DATA || dwAllocated < (g_bIsWin9x ? dwSize+dwSize : dwSize) + 4) 
			{
				if (g_bIsWin9x) dwSize += dwSize; // reserve space for unicode
				dwAllocated = dwSize+4096;
				pValue = (BYTE*)HeapReAlloc(GetProcessHeap(), 0, pValue, dwAllocated);
				if (!pValue) return Ret; // errNOT_ENOUGH_MEMORY;
				dwSize = dwAllocated;
				win32err = pfRegQueryValueEx(hKey, sName, &dwType, pValue, &dwSize);
			}
			if (win32err != ERROR_SUCCESS) continue;
			
			win32err=pCtx->m_pEnumCallbackFuncW(pCtx->m_pEnumCallbackContext, hKey, pCtx->m_sPath, sName, pValue, dwSize, dwType);
			if (win32err==ERROR_REGOP_MISS_VALUE) i--;
			if ((win32err==ERROR_REGOP_RECOUNT_KEY)|(win32err==ERROR_REGOP_DEL_KEY)|(!win32err)|(win32err==ERROR_REGOP_CANCELED)) 
			{
				if (win32err==ERROR_REGOP_RECOUNT_KEY)
					Ret=ERROR_REGOP_RECOUNT_KEY_FROM_VALUE;
				else
					Ret=win32err;
				return Ret;
			}
		}
	}
	if (pCtx->m_dwFlags & FLAG_REG_WITH_SUBKEYS)
	{
		if (!(pCtx->m_dwFlags&FLAG_REG_SUBKEYS_PROCIED))
		{
			bSubkeyRoot=1;
			pCtx->m_dwFlags=pCtx->m_dwFlags|FLAG_REG_SUBKEYS_PROCIED;
		}
		Ret=_RegEnum(hKey, L"*", sSubKeys, dwPathSize, pCtx);
		if (bSubkeyRoot)
		{
			bSubkeyRoot=0;
			pCtx->m_dwFlags=pCtx->m_dwFlags&FLAG_CLEAR_REG_SUBKEYS_PROCIED;
		}
		return Ret;
	}
	return Ret;
}

long cRegEnumCtx::_RegEnum(HKEY hKey, const wchar_t* sKey, const wchar_t* sSubKeys, DWORD dwPathSize, tRegEnumCtx* pCtx)
{
	wchar_t*	sName=NULL;
	BYTE*		pValue=NULL;
	long Ret = _iRegEnum(hKey, sKey, sSubKeys, dwPathSize, pCtx, sName, pValue);
	if (pValue) HeapFree(GetProcessHeap(), 0, pValue);
	if (sName) sName[0]=0;
	return Ret;
}	

LONG cRegEnumCtx::RegEnumWW(const wchar_t* sKey, const wchar_t* sValue, DWORD dwFlags, LPVOID m_pEnumCallbackContext, tEnumCallbackFuncW m_pEnumCallbackFuncW, tEnumCallbackErrMessageFuncW pEnumCallbackErrMessageFunc)
{
	tRegEnumCtx RegEnumCtx;
	LONG		Ret;

	if (!sKey || !m_pEnumCallbackFuncW)
		return ERROR_INVALID_PARAMETER;
	if (!sValue)
		sValue = L""; // default value 
	memset(&RegEnumCtx, 0, sizeof(RegEnumCtx));
	RegEnumCtx.m_dwFlags = dwFlags;
	RegEnumCtx.m_pEnumCallbackContext = m_pEnumCallbackContext;
	RegEnumCtx.m_pEnumCallbackFuncW = m_pEnumCallbackFuncW;
	RegEnumCtx.m_pEnumCallbackErrMessageFuncW = pEnumCallbackErrMessageFunc;
	RegEnumCtx.m_sValue = sValue;
	Ret=_RegEnum(NULL, NULL, sKey, 0, &RegEnumCtx);
	if (RegEnumCtx.m_sPath) HeapFree(GetProcessHeap(), 0, RegEnumCtx.m_sPath);
	if (Ret==ERROR_REGOP_CANCELED) return errOPERATION_CANCELED;
	if (!Ret) return ERROR_CANCELLED;
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::RegEnumAW(const char* sKey, const char* sValue, DWORD dwFlags, LPVOID m_pEnumCallbackContext, tEnumCallbackFuncW m_pEnumCallbackFuncW, tEnumCallbackErrMessageFuncW pEnumCallbackErrMessageFunc)
{
	LONG error = ERROR_SUCCESS;
	wchar_t* wcsKey = NULL;
	wchar_t* wcsValue = NULL;
	int nLen;

	if (sKey)
	{
		nLen = strlen(sKey);
		wcsKey = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (nLen+1)*sizeof(wchar_t));
		if (!wcsKey)
			error = ERROR_NOT_ENOUGH_MEMORY;
		else
			FastAnsi2Uni(sKey, wcsKey, nLen+1);
	}
	if (error == ERROR_SUCCESS && sValue)
	{
		nLen = strlen(sValue);
		wcsValue = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (nLen+1)*sizeof(wchar_t));
		if (!wcsValue)
			error = ERROR_NOT_ENOUGH_MEMORY;
		else
			FastAnsi2Uni(sValue, wcsValue, nLen+1);
	}
	if (error==ERROR_SUCCESS)
	{
		error=RegEnumWW(wcsKey,wcsValue,dwFlags,m_pEnumCallbackContext,m_pEnumCallbackFuncW,pEnumCallbackErrMessageFunc);
		if (error==ERROR_REGOP_CANCELED) error=errOPERATION_CANCELED;
	}
	if (wcsKey) HeapFree(GetProcessHeap(),0,wcsKey);
	if (wcsValue) HeapFree(GetProcessHeap(),0,wcsValue);
	return error;
}

long __stdcall	CheckFilter(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
//INT3;
	cRegEnumCtx* pContext=(cRegEnumCtx*)pEnumCallbackContext;
	return pContext->iCheckFilter(hKey, sPath, sName, pValue, dwSize, dwType);
}

long cRegEnumCtx::iCheckFilter(HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	DWORD		dCount=dwSize/2;
	LONG		OpRet=ERROR_SUCCESS;
	long		Ret=ERROR_REGOP_TRUE;
	wchar_t*	pData=(wchar_t*)pValue;
	BYTE*		pNewMultiSZ=NULL;
	DWORD		dNewMultiSZ=0;
	ActionType	Action=UnknownAct;

	if ((dwSize==0) || (!sSubKeyName))
		return ERROR_REGOP_TRUE;
	if ((dwType==REG_SZ) || (dwType==REG_EXPAND_SZ))
	{
		if (!wcscmp(sSubKeyName,pData))
			Action=DelData;
	}
	if (dwType==REG_MULTI_SZ)
	{
		wchar_t* nP=NULL;
		wchar_t* nD=pData;
		wchar_t* nS=pData;
		while ((DWORD)(nD-pData)<dCount)
		{
			bool ChangeStart=false;
			if (!wcscmp(sSubKeyName,nD))
			{
				if (!pNewMultiSZ)
				{
					pNewMultiSZ=(BYTE*)HeapAlloc(GetProcessHeap(),0,dwSize);
					nP=(wchar_t*)pNewMultiSZ;
				}
				if (!pNewMultiSZ)
					return ERROR_NOT_ENOUGH_MEMORY;
				memcpy(nP,nS,(DWORD)nD-(DWORD)nS);
				ChangeStart=true;
				nP=nP+(nD-nS);
				dNewMultiSZ+=(DWORD)(nD-nS);
			}
			while(*nD!=0)
				nD++;
			nD++;
			if (*nD==0)
			{
				if (dNewMultiSZ==0)
					Action=DelData;
				else
				{
					Action=CutData;
					memcpy(nP,nS,(DWORD)nD-(DWORD)nS);
					nP=nP+(nD-nS);
					dNewMultiSZ+=(DWORD)(nD-nS);
					if (*nP)
					{
						*nP=0;
						dNewMultiSZ++;
					}
				}
				break;
			}
			if (ChangeStart)
				nS=nD;
		}
	}
	switch(Action) 
	{
	case CutData:
		if (dNewMultiSZ)
		{
			OpRet=pfRegSetValueEx(hKey,sName,dwType,pNewMultiSZ,dNewMultiSZ*2);
			if (OpRet==ERROR_SUCCESS) 
				Ret=ERROR_SUCCESS;
			break;
		}
	case DelData:
		OpRet=pfRegDeleteValue(hKey,sName);
		if (OpRet==ERROR_SUCCESS) 
			Ret=ERROR_REGOP_MISS_VALUE;
		break;
	default:
		Ret=errNOT_FOUND;
	}
	if (pNewMultiSZ) 
		HeapFree(GetProcessHeap(),0,pNewMultiSZ);
	if (OpRet==errOPERATION_CANCELED) 
		Ret=ERROR_REGOP_CANCELED;
	return Ret;
}

long __stdcall RegEnumCallbackErrMessageNULL(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error)
{
	return ERROR_REGOP_TRUE;
}

long __stdcall RegEnumCallbackNULL(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	return ERROR_REGOP_TRUE;
}

void cRegEnumCtx::CLSIDInit(void)
{
	if (CLSIDInited)
		return;
	wchar_t	str[]=L"*\\Software\\Classes\\CLSID\\";
	RegEnumWW(str,L"*",FLAG_REG_CLSID_INIT,NULL,RegEnumCallbackNULL,RegEnumCallbackErrMessageNULL);
	CLSIDInited=cTRUE;
	return;
}

void cRegEnumCtx::CLSIDDeinit(void)
{
	if (CLSIDInited)
	{
		DWORD i;
		for (i=0;i<CLSIDKeys.Count;i++)
		{
			if (CLSIDKeys.pKeys->hKey[i])
				pfRegCloseKey(CLSIDKeys.pKeys->hKey[i]);
		}
		if (CLSIDKeys.pKeys)
			m_pStartUpEnum->heapFree(CLSIDKeys.pKeys);
		CLSIDInited=cFALSE;
	}
	return;
}

#define CLSID_VALUE_COUNT	4

long cRegEnumCtx::CLSIDScan(const wchar_t* chCLSID,LPVOID m_pEnumCallbackContext,tEnumCallbackFuncW m_pEnumCallbackFuncW,tEnumCallbackErrMessageFuncW pEnumCallbackErrMessageFunc)
{
	tRegEnumCtx RegEnumCtx;
	LONG		Ret;
	DWORD		i;
	const wchar_t		chCLSIDValue[CLSID_VALUE_COUNT][15]={
		L"InprocServer32",
			L"InprocServer",
			L"LocalServer",
			L"LocalServer32",
	};
	
	if (!chCLSID || !m_pEnumCallbackFuncW)
		return ERROR_INVALID_PARAMETER;
	memset(&RegEnumCtx, 0, sizeof(RegEnumCtx));
	RegEnumCtx.m_dwFlags = FLAG_REG_WITH_SUBKEYS;
	RegEnumCtx.m_pEnumCallbackContext = m_pEnumCallbackContext;
	RegEnumCtx.m_pEnumCallbackFuncW = m_pEnumCallbackFuncW;
	RegEnumCtx.m_pEnumCallbackErrMessageFuncW = pEnumCallbackErrMessageFunc;
	RegEnumCtx.m_sValue = L"*";
	for (i=0;i<CLSIDKeys.Count;i++)
	{
		if (CLSIDKeys.pKeys->hKey[i])
		{
			DWORD j;
			for (j=0;j<CLSID_VALUE_COUNT;j++)
				Ret=_RegEnum(CLSIDKeys.pKeys->hKey[i], chCLSID, chCLSIDValue[j], 0, &RegEnumCtx);
		}
	}
	if (RegEnumCtx.m_sPath) HeapFree(GetProcessHeap(), 0, RegEnumCtx.m_sPath);
	if (Ret==ERROR_REGOP_CANCELED) return errOPERATION_CANCELED;
	if (!Ret) return ERROR_CANCELLED;
	return ERROR_SUCCESS;
}

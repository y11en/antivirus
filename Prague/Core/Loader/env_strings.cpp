#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_remote.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_receiv.h>
#include <Prague/iface/i_string.h>

#include <stdio.h>
#include <loader.h>

#if defined (_WIN32)
#include <windows.h>
#include <shlobj.h>
#include <../remote/pr_process.h>
static TCHAR   g_sProcessName[MAX_PATH] = {0, };
static WTSApi  g_WTSApi;

extern "C" BOOL     g_bWOW64;

extern "C" BOOL ImpersonateToUser(HANDLE* pToken)
{
	HANDLE hTokenNew = NULL, hTokenOld = NULL;
	g_WTSApi.ImpersonateToUser(hTokenNew, hTokenOld);

	if( hTokenOld )
		CloseHandle(hTokenOld);

	if( !hTokenNew )
		return FALSE;

	if( *pToken )
		CloseHandle(*pToken);

	*pToken = hTokenNew;
	return TRUE;
}

struct CUserToken
{
	CUserToken()
	{
		m_hToken = NULL;
		OpenThreadToken(GetCurrentThread(), TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, TRUE, &m_hToken);
	}

	~CUserToken()
	{
		if( m_hToken )
			CloseHandle(m_hToken);
	}

	operator HANDLE()
	{
		return m_hToken;
// 		if( m_hToken )
// 			return m_hToken;
// 
// 		static DWORD  dwLastRequest = 0;
// 		static HANDLE hUserToken = NULL;
// 
// 		if( hUserToken )
// 			return hUserToken;
// 
// 		DWORD dwNow = GetTickCount();
// 		if( dwNow - dwLastRequest > 10000 )
// 		{
// 			dwLastRequest = dwNow;
// 
// 			HANDLE hTokenOld = NULL;
// 			g_WTSApi.GetUserToken(hUserToken, hTokenOld);
// 
// 			if( hTokenOld )
// 				CloseHandle(hTokenOld);
// 		}
// 		return hUserToken;
	}

	HANDLE m_hToken;
};

extern "C" BOOL GetPersonalFolder(tCHAR* buf, tCODEPAGE* cp)
{
	static HRESULT (STDAPICALLTYPE* fnSHGetFolderPath)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPVOID pszPath);
	static BOOL (STDAPICALLTYPE* fnSHGetSpecialFolderPath)(HWND hwndOwner, LPVOID lpszPath, int nFolder, BOOL fCreate);

	CUserToken hToken;

	if( !fnSHGetFolderPath && !fnSHGetSpecialFolderPath )
	{
		HMODULE hMod = ::LoadLibraryA("shell32.dll");
		if( hMod )
		{
			*(void**)&fnSHGetFolderPath = ::GetProcAddress(hMod, g_bUnderNT ? "SHGetFolderPathW" : "SHGetFolderPathA");
			if( !fnSHGetFolderPath )
				*(void**)&fnSHGetSpecialFolderPath = ::GetProcAddress(hMod, g_bUnderNT ? "SHGetSpecialFolderPathW" : "SHGetSpecialFolderPathA");
		}
	}

	BOOL bRet = FALSE;
	if( fnSHGetSpecialFolderPath )
		bRet = fnSHGetSpecialFolderPath(NULL, buf, CSIDL_PERSONAL, FALSE);
	else if( fnSHGetFolderPath )
		bRet = fnSHGetFolderPath(NULL, CSIDL_PERSONAL, hToken, SHGFP_TYPE_CURRENT, buf) == S_OK;

	if( bRet && g_bUnderNT )
		*cp = cCP_UNICODE;
	return bRet;
}

#define ENVWOW64_ENV_PRGFLS_W		L"%ProgramFiles%"
#define ENVWOW64_ENV_PRGFLS_LEN		14
#define REGWOW64_ENV_PRGFLS_SUBST_W	L"%ProgramW6432%"

extern "C" tERROR ExpandSystemStringFormBuff(hSTRING hStr, const tVOID* str_to_expand, tCODEPAGE cp)
{
	tDWORD ret = 0;
	tBYTE  dst[0x8000];

	static BOOL (WINAPI *fnExpandEnvironmentStringsForUserA)(HANDLE hToken, LPCSTR lpSrc, LPSTR lpDest, DWORD dwSize);
	static BOOL (WINAPI *fnExpandEnvironmentStringsForUserW)(HANDLE hToken, LPCWSTR lpSrc, LPWSTR lpDest, DWORD dwSize);
	static BOOL bInited = FALSE;

	if( !bInited )
	{
		bInited = TRUE;
		if( HMODULE hUserEnv = ::LoadLibrary("UserEnv.dll") )
		{
			*(void**)&fnExpandEnvironmentStringsForUserA = ::GetProcAddress(hUserEnv, "ExpandEnvironmentStringsForUserA");
			*(void**)&fnExpandEnvironmentStringsForUserW = ::GetProcAddress(hUserEnv, "ExpandEnvironmentStringsForUserW");
		}
	}

	CUserToken hToken;

	if( cp == cCP_UNICODE )
	{
		LPWSTR _strToExpand = (LPWSTR)str_to_expand;
		
		if( g_bWOW64 )
		{
			sString strSrc;
			tDWORD nPos;
			tERROR err;

			strSrc.m_data = _strToExpand;
			strSrc.m_len = wcslen(_strToExpand);
			strSrc.m_size = 0;

			nPos = cSTRING_EMPTY_LENGTH;
			FindBuff(&strSrc, 0, cSTRING_WHOLE_LENGTH, ENVWOW64_ENV_PRGFLS_W, 0, cCP_UNICODE, fSTRING_COMPARE_CASE_INSENSITIVE, &nPos);
			if( nPos != cSTRING_EMPTY_LENGTH )
			{
				tDWORD size = (strSrc.m_len + 1)*sizeof(tWCHAR);
				
				err = CALL_SYS_ObjHeapAlloc(hStr, (tPTR *)&_strToExpand, size);
				if( PR_FAIL(err) )
					return err;

				memcpy(_strToExpand, str_to_expand, size);

				do 
				{
					memcpy((tWCHAR *)_strToExpand + nPos, REGWOW64_ENV_PRGFLS_SUBST_W, ENVWOW64_ENV_PRGFLS_LEN*sizeof(tWCHAR));
					nPos += ENVWOW64_ENV_PRGFLS_LEN;

					FindBuff(&strSrc, nPos, cSTRING_WHOLE_LENGTH, ENVWOW64_ENV_PRGFLS_W, 0, cCP_UNICODE, fSTRING_COMPARE_CASE_INSENSITIVE, &nPos);
				} while( nPos != cSTRING_EMPTY_LENGTH );
			}
		}
		
		HANDLE hTokenHandle = (HANDLE)hToken;
		if( fnExpandEnvironmentStringsForUserW && hTokenHandle )
		{
			ret = fnExpandEnvironmentStringsForUserW(hTokenHandle, _strToExpand, (LPWSTR)dst, sizeof(dst)/sizeof(tWCHAR));
			
			//+ под сервисом на 64 бит не экспандятся некоторые строчки типа programfiles(x86)... но экспандятся без токена
			if (ret && (NULL != wcschr((LPWSTR)dst, '%')))
			{
				tBYTE  dst2[0x8000];
				if (ExpandEnvironmentStringsW((LPWSTR)dst, (LPWSTR)dst2, sizeof(dst2)/sizeof(tWCHAR)))
					lstrcpyW((LPWSTR)dst, (LPWSTR)dst2);
			}
			//-

		}
		else
			ret = ExpandEnvironmentStringsW(_strToExpand, (LPWSTR)dst, sizeof(dst)/sizeof(tWCHAR));

		if( _strToExpand != str_to_expand && _strToExpand )
			CALL_SYS_ObjHeapFree(hStr, _strToExpand);
	}
	else
	{
		HANDLE hTokenHandle = (HANDLE)hToken;
		if( fnExpandEnvironmentStringsForUserA && hTokenHandle )
		{
			ret = fnExpandEnvironmentStringsForUserA(hTokenHandle, (LPSTR)str_to_expand, (LPSTR)dst, sizeof(dst)/sizeof(tWCHAR));
			//+ под сервисом на 64 бит не экспандятся некоторые строчки типа programfiles(x86)... но экспандятся без токена
			if (ret && strchr((LPSTR)dst, '%'))
			{
				tBYTE  dst2[0x8000];
				if (ExpandEnvironmentStringsA((char*)dst, (char*)dst2, sizeof(dst2)))
					lstrcpy((LPSTR)dst, (LPSTR)dst2);
			}
			//-

		}
		else
			ret = ExpandEnvironmentStringsA((LPSTR)str_to_expand, (LPSTR)dst, sizeof(dst));
	}

	if( !ret )
		return errUNEXPECTED;

	return CALL_String_ImportFromBuff(hStr, NULL, dst, 0, cp, cSTRING_Z);
}

#else // !_WIN32

static char    g_sProcessName[] = "Application";

extern "C" tBOOL GetPersonalFolder(tCHAR* buff, tCODEPAGE* cp)
{
	return cFALSE;
}

extern "C" tERROR ExpandSystemStringFormBuff(hSTRING hStr, const tVOID* str_to_expand, tCODEPAGE cp)
{
	return CALL_String_ImportFromBuff( hStr, NULL, const_cast<tVOID*> (str_to_expand), 0, cp, cSTRING_Z );
}

extern "C" tBOOL GetSystemDirectory(tCHAR* buff, tDWORD size)
{
	return cFALSE;
}

extern "C" tBOOL GetWindowsDirectory(tCHAR* buff, tDWORD size)
{
	return cFALSE;
}
#endif

static hREGISTRY      g_hEnviroment = NULL;

tERROR GetEnviromentString(hREGISTRY hReg, hSTRING sName, hSTRING sValue, bool bSysOnly)
{
	tERROR error = errUNEXPECTED;
	tCHAR lbuf[0x400];
	tCHAR* buf = lbuf;
	tCHAR lname[0x100];
	tCHAR* name = lname;
	tDWORD dwSize, dwNameSize;
	tTYPE_ID type_id;
	tCODEPAGE cp = cCP_ANSI;

	PR_TRACE((g_root, prtSPAM, "env\tGetRegistryEnvString from %tstr", sName));

	error = sName->ExportToBuff(&dwNameSize, cSTRING_WHOLE, lname, sizeof(lname), cCP_ANSI, cSTRING_Z);
	if (error == errBUFFER_TOO_SMALL)
	{
		error = sName->heapAlloc((tPTR*)&name, dwNameSize);
		if (PR_SUCC(error))
			error = sName->ExportToBuff(&dwNameSize, cSTRING_WHOLE, name, dwNameSize, cCP_ANSI, cSTRING_Z);
	}

	if (PR_SUCC(error))
	{
		if( hReg && !bSysOnly )
		{
			if( dwNameSize <= 1 )
				return errNOT_FOUND;

			type_id=tid_STRING;
			error = hReg->GetValue(&dwSize, cRegRoot, name, &type_id, lbuf, sizeof(lbuf));
			if (error == errBUFFER_TOO_SMALL)
			{
				error = sName->heapAlloc((tPTR*)&buf, dwSize);
				if (PR_SUCC(error))
				{
					type_id=tid_STRING;
					error = hReg->GetValue(&dwSize, cRegRoot, name, &type_id, buf, dwSize);
				}
			}
		}
		else
			error = errNOT_FOUND;

		if( PR_FAIL(error) )
		{
			if( !_stricmp(name, "personal") )
			{
				if( GetPersonalFolder(buf, &cp) )
					error = errOK;
			}
			else if( !_stricmp(name, "sysdir") || !_stricmp(name, "systemdir"))
			{
				if( GetSystemDirectory(buf, sizeof(lbuf)) )
					error = errOK;
			}
#if defined (_WIN32)
			else if( !_stricmp(name, "sysdirWOW64"))
			{
				HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
				if (hKernel32)
				{
					typedef UINT (WINAPI *tGetSystemWow64DirectoryA)(LPSTR lpBuffer, UINT uSize);
					tGetSystemWow64DirectoryA fnGetSystemWow64DirectoryA = (tGetSystemWow64DirectoryA)GetProcAddress(hKernel32, "GetSystemWow64DirectoryA");
					if (fnGetSystemWow64DirectoryA && fnGetSystemWow64DirectoryA(buf, sizeof(lbuf)))
						error = errOK;
				}
			}
#endif //_WIN32
			else if( !_stricmp(name, "windir") || !_stricmp(name, "systemroot") )
			{
				if( GetWindowsDirectory(buf, sizeof(lbuf)) )
					error = errOK;
			}
			else if( !_stricmp(name, "currentdir") )
			{
				error = sName->heapAlloc((tPTR*)&buf, sizeof(g_sProcessName)+1);
				if (PR_SUCC(error))
				{
					char* lastslash = strrchr(g_sProcessName, '\\');
					if ( lastslash )
					{
						memcpy(buf, g_sProcessName, lastslash - g_sProcessName);
						cp = cCP_ANSI;
					}
				}
				
			}
		}
	}

	if (PR_SUCC(error))
		error = sValue->ImportFromBuff(NULL, buf, dwSize, cp, cSTRING_Z);

	if (name && name != lname)
		sName->heapFree(name);
	if (buf && buf != lbuf)
		sName->heapFree(buf);

	PR_TRACE((g_root, prtSPAM, "env\tGetRegistryEnvString result %tstr", sValue));
	return error;
}

tERROR ExpandSystemString(hSTRING hStr)
{
	tDWORD  len = 0;
	tBYTE   src[0x100];
	tBYTE*  psrc = src;

	if( !hStr )
		return errPARAMETER_INVALID;

	tERROR error = CALL_String_LengthEx( hStr, &len, cSTRING_WHOLE, cCP, cSTRING_Z );
	if( PR_SUCC(error) && (len > sizeof(src)) )
		error = CALL_SYS_ObjHeapAlloc( hStr, (tPTR*)&psrc, len );
	if( PR_SUCC(error) )
		error = CALL_String_ExportToBuff( hStr, NULL, cSTRING_WHOLE, psrc, len, cCP, cSTRING_Z );
	if ( PR_SUCC(error) )
		error = ExpandSystemStringFormBuff( hStr, psrc, cCP );

	if( psrc && (psrc != src) )
		CALL_SYS_ObjHeapFree( hStr, psrc );
	return error;
}

extern "C" tERROR ExpandString(hREGISTRY hReg, hSTRING hStr, hSTRING hResult, bool bSysOnly)
{
	if( !hReg )
		hReg = g_hEnviroment;
	if( !hReg )
		return errOBJECT_NOT_INITIALIZED;

	tERROR error = errOK;
	tBOOL  bExpanded;
	tDWORD lastpos, pos1, pos2, unresolved_vars_count;
	cStrObj name, val, result;

	do 
	{
		unresolved_vars_count = 0;
		lastpos = 0;
		bExpanded = cFALSE;
		result.clean();
		PR_TRACE((g_root, prtSPAM, "env\tExpanding string from %tstr", hStr));
		do
		{
			tSTR_RANGE range;
			tCHAR dummy [] = "%"; 
			hStr->FindBuff(&range, STR_RANGE(lastpos, cSTRING_WHOLE_LENGTH), dummy, 0, cCP_ANSI, fSTRING_FIND_FORWARD);
			pos1 = STR_RANGE_POS(range);
			if (range != cSTRING_EMPTY)
			{
				hStr->FindBuff(&range, STR_RANGE(pos1+1, cSTRING_WHOLE_LENGTH), dummy, 0, cCP_ANSI, fSTRING_FIND_FORWARD);
				pos2 = STR_RANGE_POS(range);
			}
			else
			{
				if (!bExpanded) // nothing was expanded
				{
					if (hResult)
						error = hStr->ExportToStr(NULL, cSTRING_WHOLE, hResult);
					
					if (0 == unresolved_vars_count)
					{
						PR_TRACE((g_root, prtNOTIFY, "env\tExpandString: no more vars to resolve"));
						return errOK;
					}

					// expand system environment variables
					return ExpandSystemString(hStr);
				}
				pos2 = cSTRING_EMPTY_LENGTH;
			}

			if (pos2 == cSTRING_EMPTY_LENGTH) // no more %var%
			{
				error = val.assign ( hStr,lastpos, cSTRING_WHOLE_LENGTH );
				PR_TRACE((g_root, prtSPAM, "env\tExpandString: (no more %var%) ExportToStr from %tstr, pos=%d => %S, %terr", (hOBJECT)hStr, lastpos, val.data(), error));
				if (error == errSTR_OUT_OF_RANGE)
				{
					val.clear ();
					error = errOK;
				}
				if (PR_FAIL(error))
				{
					PR_TRACE((g_root, prtERROR, "env\tCannot extract least of string from %tstr, pos=%d, %terr", hStr, pos1, error));
					return error;
				}
			}
			else
			{
				pos2++;
				if (pos1 != lastpos)
				{
					error = val.assign ( hStr, lastpos, pos1-lastpos );
					if (PR_FAIL(error))
					{
						PR_TRACE((g_root, prtERROR, "env\tCannot extract part of string from %tstr, pos=%d, len=%d, %terr", hStr, pos1, pos1-lastpos, error));
						return error;
					}
					result.init();
					if (PR_FAIL(error = result.append(val)))
					{
						PR_TRACE((g_root, prtERROR, "env\tCannot append value to result string, %terr", error));
						return error;
					}
					PR_TRACE((g_root, prtSPAM, "env\tExpandString: (append part) result: %S", result.data()));
				}
				name.init();
				error = name.assign ( hStr, pos1+1, pos2-pos1-2 );
				if (PR_FAIL(error))
				{
					PR_TRACE((g_root, prtERROR, "env\tCannot extract value name from %tstr, pos=%d, size=%d, %terr", hStr, pos1+1, pos2-pos1-2, error));
					return error;
				}
				if (PR_FAIL(GetEnviromentString(hReg, cAutoString(name ), cAutoString(val), bSysOnly)))
				{
					PR_TRACE((g_root, prtNOTIFY, "env\tProduct environment cannot resolve name %%%S%%", name.data()));
					error = val.assign ( hStr, pos1, pos2-pos1 );
					unresolved_vars_count++;
					if (PR_FAIL(error))
					{
						PR_TRACE((g_root, prtERROR, "env\tCannot extract unresolved value name from %tstr, pos=%d, size=%d, %terr", hStr, pos1, pos2-pos1, error));
						return error;
					}
				}
				else
					bExpanded = cTRUE;
			}
			result.init();
			if (PR_FAIL(error = result.append(val)))
			{
				PR_TRACE((g_root, prtERROR, "env\tCannot append value to result string, %terr", error));
				return error;
			}
			PR_TRACE((g_root, prtSPAM, "env\tExpandString: (append val) result: %S", result.data()));
			lastpos = pos2;
		} while (pos2 != cSTRING_EMPTY_LENGTH);

		PR_TRACE((g_root, prtSPAM, "env\tExpandString: (finished) result: %S", result.data()));
		if (PR_FAIL(error = result.copy(hResult ? hResult : hStr)))
		{
			PR_TRACE((g_root, prtERROR, "env\tCannot append value to result string, %terr", error));
			return error;
		}
		if (hResult)
		{
			hStr = hResult;
			hResult = NULL;
		}
	} while (bExpanded);

	if (0 == unresolved_vars_count)
	{
		PR_TRACE((g_root, prtNOTIFY, "env\tExpandString: no more vars to resolve"));
		return errOK;
	}
	return ExpandSystemString(hStr);
}

static tERROR pr_call MsgReceive( hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen ) 
{
	tERROR error = errOK;
	if(msg_cls != pmc_PRODUCT_ENVIRONMENT )
		return error;

	if( msg_id == pm_EXPAND_ENVIRONMENT_STRING || msg_id == pm_EXPAND_SYSTEM_ONLY_STRING )
	{
		hREGISTRY hReg = (hREGISTRY)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
		if (!hReg)
			return errOBJECT_NOT_INITIALIZED;
		error = _this->sysCheckObject( send_point, IID_STRING );
		if (PR_FAIL(error))
		{
			send_point = ctx;
			ctx = NULL;
			error = _this->sysCheckObject( send_point, IID_STRING );
		}
		if (PR_SUCC(error) && ctx)
			error = _this->sysCheckObject( ctx, IID_STRING );
		if (PR_SUCC(error))
		{
			tCHAR dummy [] = "%host%";                        
			if (PR_SUCC(((cString*)send_point)->CompareBuff(cSTRING_WHOLE, dummy, 0, cCP_ANSI, fSTRING_COMPARE_CASE_INSENSITIVE)))
			{
				((cString*)send_point)->ImportFromBuff(NULL, g_sProcessName, 0, cCP_ANSI, cSTRING_Z);
				if (ctx) {
					tCHAR dummy [] = "-host";                        
					((cString*)ctx)->ImportFromBuff(NULL, dummy, 0, cCP_ANSI, cSTRING_Z);
                                }

			}
			PR_TRACE((g_root, prtSPAM, "env\tpm_EXPAND_ENVIRONMENT_STRING: send_point=%08X, ctx=%08X", send_point, ctx));
			error = ExpandString(hReg, (hSTRING)send_point, (hSTRING)ctx, msg_id == pm_EXPAND_SYSTEM_ONLY_STRING );
			PR_TRACE((g_root, prtNOTIFY, "env\tpm_EXPAND_ENVIRONMENT_STRING: ExpandString result: %tstr", ctx ? (hSTRING)ctx : (hSTRING)send_point));
		}
		if (PR_SUCC(error))
			error = errOK_DECIDED;
	}

	return error;
}

extern "C" tERROR SetEnviroment(hREGISTRY hReg, hOBJECT hSubscriber)
{
	if( !hSubscriber )
		hSubscriber = (hOBJECT)g_root;

	g_hEnviroment = hReg;
#if defined (_WIN32)	
	::GetModuleFileName(NULL, g_sProcessName, countof(g_sProcessName));
#else
        
#endif
	// the receiver object will be deleted on hReg destroing
	tERROR err = errOK;
	cMsgReceiver * hMsgRcv = NULL;
	if( PR_SUCC(err) ) err = hReg->sysCreateObject((hOBJECT*)&hMsgRcv,IID_MSG_RECEIVER,PID_ANY,SUBTYPE_ANY);
	if( PR_SUCC(err) ) err = hMsgRcv->propSetPtr(pgRECEIVE_PROCEDURE, (void*)MsgReceive);
	if( PR_SUCC(err) ) err = hMsgRcv->propSetPtr(pgRECEIVE_CLIENT_ID, hReg);
	if( PR_SUCC(err) ) err = hMsgRcv->sysCreateObjectDone();
	if( PR_SUCC(err) ) err = hMsgRcv->sysRegisterMsgHandler(pmc_PRODUCT_ENVIRONMENT, rmhDECIDER, hSubscriber, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if( PR_FAIL(err) && hMsgRcv )
		hMsgRcv->sysCloseObject();
	return err;
}


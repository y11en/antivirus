// Tracer.cpp: implementation of the cPrTracer class.
//

#include <PPP/common/Tracer.h>
#include <Prague/iface/i_root.h>

#include <stdio.h>
#include <malloc.h>
#include <PPP/common/SpecialName.h>
#include <tchar.h>
#include <version/ver_product.h>

#include <atlbase.h>

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

////////////////////////////////////////////////////////////////////////////
// cPrTracer

void * cPrTracer::g_pTracer = NULL;

PR_MAKE_TRACE_FUNC

#pragma comment( lib, "advapi32.lib" )

namespace
{
	LPCTSTR AVP6_KEY = _T(VER_PRODUCT_REGISTRY_PATH "\\Trace");
	LPCTSTR SETTINGS_VALUE = _T("Settings");

	LPCTSTR TRACETOFILE_VALUE = _T("TraceFileEnable");
	LPCTSTR TRACETOFILE_MAXLEVEL_VALUE = _T("TraceFileMaxLevel");
	LPCTSTR TRACETOFILE_MINLEVEL_VALUE = _T("TraceFileMinLevel");

	LPCTSTR TRACEDBG_VALUE = _T("TraceDebugEnable");
	LPCTSTR TRACEDBG_MAXLEVEL_VALUE = _T("TraceDebugMaxLevel");
	LPCTSTR TRACEDBG_MINLEVEL_VALUE = _T("TraceDebugMinLevel");
}

//static void _GetTracingParams(tBOOL *pbFileTracingEnabled, tDWORD *pdwFileTracingMaxLevel, tDWORD *pdwFileTracingMinLevel,
//							tBOOL *pbDbgTracingEnabled, tDWORD *pdwDbgTracingMaxLevel, tDWORD *pdwDbgTracingMinLevel)
//{
//	if(pbFileTracingEnabled)
//		*pbFileTracingEnabled = cFALSE;
//	if(pbDbgTracingEnabled)
//		*pbDbgTracingEnabled = cFALSE;
//	if(pdwFileTracingMaxLevel)
//		*pdwFileTracingMaxLevel = prtIMPORTANT;
//	if(pdwFileTracingMinLevel)
//		*pdwFileTracingMinLevel = prtMIN_TRACE_LEVEL;
//	if(pdwDbgTracingMaxLevel)
//		*pdwDbgTracingMaxLevel = prtIMPORTANT;
//	if(pdwDbgTracingMinLevel)
//		*pdwDbgTracingMinLevel = prtMIN_TRACE_LEVEL;
//
//	HKEY hkTrace = NULL;
//	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, AVP6_KEY, 0, KEY_READ, &hkTrace) != ERROR_SUCCESS)
//		return;
//	TCHAR szBufferSettings[512];
//	DWORD Type, Size;
//	Size = sizeof(szBufferSettings);
//	if(RegQueryValueEx(hkTrace, SETTINGS_VALUE, 0, &Type, (LPBYTE)szBufferSettings, &Size) != ERROR_SUCCESS ||
//			Type != REG_SZ)
//	{
//		RegCloseKey(hkTrace);
//		return;
//	}
//	RegCloseKey(hkTrace);
//	hkTrace = NULL;
//
//	TCHAR szBuffer[512];
//	_tcscpy_s(szBuffer, countof(szBuffer), AVP6_KEY);
//	_tcscat_s(szBuffer, countof(szBuffer), _T("\\"));
//	_tcscat_s(szBuffer, countof(szBuffer), szBufferSettings);
//	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, szBuffer, 0, KEY_READ, &hkTrace) != ERROR_SUCCESS)
//		return;
//	DWORD dwTraceToFile = 0;
//	Size = sizeof(dwTraceToFile);
//	if(RegQueryValueEx(hkTrace, TRACETOFILE_VALUE, 0, &Type, (LPBYTE)&dwTraceToFile, &Size) == ERROR_SUCCESS &&
//			Type == REG_DWORD)
//	{
//		if(pbFileTracingEnabled)
//			*pbFileTracingEnabled = dwTraceToFile != 0;
//		if(dwTraceToFile != 0)
//		{
//			if(pdwFileTracingMaxLevel)
//			{
//				Size = sizeof(*pdwFileTracingMaxLevel);
//				RegQueryValueEx(hkTrace, TRACETOFILE_MAXLEVEL_VALUE, 0, &Type, (LPBYTE)pdwFileTracingMaxLevel, &Size);
//			}
//			if(pdwFileTracingMinLevel)
//			{
//				Size = sizeof(*pdwFileTracingMinLevel);
//				RegQueryValueEx(hkTrace, TRACETOFILE_MINLEVEL_VALUE, 0, &Type, (LPBYTE)pdwFileTracingMinLevel, &Size);
//			}
//		}
//	}
//	DWORD dwTraceDbg = 0;
//	Size = sizeof(dwTraceDbg);
//	if(RegQueryValueEx(hkTrace, TRACEDBG_VALUE, 0, &Type, (LPBYTE)&dwTraceDbg, &Size) == ERROR_SUCCESS &&
//		Type == REG_DWORD)
//	{
//		if(pbDbgTracingEnabled)
//			*pbDbgTracingEnabled = dwTraceDbg != 0;
//		if(dwTraceDbg != 0)
//		{
//			if(pdwDbgTracingMaxLevel)
//			{
//				Size = sizeof(*pdwDbgTracingMaxLevel);
//				RegQueryValueEx(hkTrace, TRACEDBG_MAXLEVEL_VALUE, 0, &Type, (LPBYTE)pdwDbgTracingMaxLevel, &Size);
//			}
//			if(pdwDbgTracingMinLevel)
//			{
//				Size = sizeof(*pdwDbgTracingMinLevel);
//				RegQueryValueEx(hkTrace, TRACEDBG_MINLEVEL_VALUE, 0, &Type, (LPBYTE)pdwDbgTracingMinLevel, &Size);
//			}
//		}
//	}
//	RegCloseKey(hkTrace);
//}

cPrTracer::cPrTracer() :
	m_hTraceFile(INVALID_HANDLE_VALUE)
	, m_nTraceDebugEnable(cFALSE)
	, m_nTraceFileEnable(cFALSE)
	, m_nTraceFileMaxLevel(prtIMPORTANT)
	, m_nTraceFileMinLevel(prtMIN_TRACE_LEVEL)
	, m_nTraceDebugMaxLevel(prtIMPORTANT)
	, m_nTraceDebugMinLevel(prtMIN_TRACE_LEVEL)
	, m_hMtxFileAccess(NULL)
{
}

cPrTracer::~cPrTracer()
{
	FreeResources();
}

void cPrTracer::FreeResources()
{
	if(m_hTraceFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hTraceFile);
		m_hTraceFile = INVALID_HANDLE_VALUE;
	}

	if(m_hMtxFileAccess)
	{
		CloseHandle(m_hMtxFileAccess);
		m_hMtxFileAccess = NULL;
	}
}

tERROR cPrTracer::Init(HINSTANCE hInst /*= NULL*/, bool bNoDateTime /*= false*/, tCHAR *pPrefix /*= NULL*/)
{
	if(!g_root)
		return errNOT_OK;
	
	tCHAR strModulePath[2 * MAX_PATH] = {0};
	GetModuleFileName(hInst, strModulePath, countof(strModulePath));
	tCHAR * pLastSlash = strrchr(strModulePath, '\\');
	tCHAR * strModule = NULL;
	if(pLastSlash)
		strModule = pLastSlash + 1;
	else
		strModule = strModulePath;

	std::basic_string<tCHAR> fileID = pPrefix ? pPrefix : "";
	fileID += ".";
	fileID += strModule;

	//tBOOL bFileTracingEnabled;
	GetTraceLevels();
	//_GetTracingParams(&bFileTracingEnabled, &m_nTraceFileMaxLevel, &m_nTraceFileMinLevel,
	//	&m_bDbgTracingEnabled, &m_nTraceDbgMaxLevel, &m_nTraceDbgMinLevel);

	if (m_nTraceFileEnable)
	{
		if (m_hTraceFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hTraceFile);
		if (m_hMtxFileAccess)
		{
			CloseHandle(m_hMtxFileAccess);
			m_hMtxFileAccess = NULL;
		}
		tCHAR strLogFile[MAX_PATH] = {0};
		MakeSpecialName(strLogFile, MAX_PATH, fileID.c_str(), "log", bNoDateTime);
		m_hTraceFile = CreateFile(strLogFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(m_hTraceFile != INVALID_HANDLE_VALUE)
		{
			tCHAR *pLastSlash = strrchr(strLogFile, L'\\');
			if(pLastSlash)
				strModule = pLastSlash + 1;
			else
				strModule = strLogFile;
			m_hMtxFileAccess = CreateMutex(NULL, FALSE, strModule);
		}
	}

	g_pTracer = this;

	tERROR err = errNOT_OK;
	if (sizeof(tDWORD*) == sizeof(DWORD))
		err = g_root->propSetDWord(pgOUTPUT_FUNC, (tDWORD)&cPrTracer::TraceLink);
	else
	if (sizeof(tDWORD*) == sizeof(tQWORD))
		err = g_root->propSetQWord(pgOUTPUT_FUNC, (tQWORD)&cPrTracer::TraceLink);

	if(PR_SUCC(err))
	{
		err = g_root->sysSetTraceLevel(tlsALL_OBJECTS,
			max(m_nTraceFileMaxLevel, m_nTraceDebugMaxLevel), min(m_nTraceFileMinLevel, m_nTraceDebugMinLevel));
	}

	if(PR_SUCC(err))
	{
		PR_TRACE((g_root, prtIMPORTANT, "Trc\t==================================="));
		PR_TRACE((g_root, prtIMPORTANT, "Trc\tTracer for '%s' started", strModule));
		PR_TRACE((g_root, prtIMPORTANT, "Trc\t==================================="));
	}
	else
		PR_TRACE((g_root, prtERROR, "Trc\tPrTracer failed to start (%terr)", err));

	return err;
}

tERROR cPrTracer::Deinit()
{
	tERROR err = errOK;

	if (g_root)
	{
		PR_TRACE((g_root, prtIMPORTANT, "Trc\tcPrTracer::Deinit. start deinitializing"));
		if(PR_SUCC(err))
			err = g_root->sysSetTraceLevel(tlsALL_OBJECTS, prtMIN_TRACE_LEVEL, prtMIN_TRACE_LEVEL);
		if (PR_SUCC(err))
		{
			if (sizeof(tDWORD*) == sizeof(DWORD))
				err = g_root->propSetDWord(pgOUTPUT_FUNC, (tDWORD)NULL);
			else
				if (sizeof(tDWORD*) == sizeof(tQWORD))
					err = g_root->propSetQWord(pgOUTPUT_FUNC, (tQWORD)NULL);
		}
	}
	FreeResources();
	
	return err;
}

tDWORD cPrTracer::MakeTracePrefix( tCHAR* OUT p_str, tDWORD size, hOBJECT hObject, tTRACE_LEVEL dwLevel) 
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	tCHAR* l_pLevelName;
	tCHAR buf[20];
	switch((dwLevel / 100) * 100)
	{
	case prtALWAYS_REPORTED_MSG: l_pLevelName = "ALW"; break;
	case prtFATAL              : l_pLevelName = "FTL"; break;
	case prtDANGER             : l_pLevelName = "DNG"; break;
	case prtERROR              : l_pLevelName = "ERR"; break;
	case prtIMPORTANT          : l_pLevelName = "IMP"; break;
	case prtNOTIFY             : l_pLevelName = "NTF"; break;
	case prtNOT_IMPORTANT      : l_pLevelName = "NOI"; break;
	case prtSPAM               : l_pLevelName = "SPM"; break;
	case prtANNOYING           : l_pLevelName = "ANN"; break;
	case prtFUNCTION_FRAME     : l_pLevelName = "FFR"; break;
	default                    : l_pLevelName = _itoa(dwLevel,buf,10); break;
	}
	
	return sprintf(p_str, "%02d:%02d:%02d.%03d\t%03x\t%d\t%s\t",
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds,
		GetCurrentThreadId(),
		g_root?CALL_SYS_PropertyGetDWord(g_root,pgOBJECT_COUNT):0,
		l_pLevelName
		);
}

void cPrTracer::TraceLink(tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel)
{
	if(g_pTracer)
		((cPrTracer *)g_pTracer)->Trace(str, hObject, dwLevel);
}

void cPrTracer::Trace1(tSTRING szMsg)
{
	if (g_pTracer)
		((cPrTracer *)g_pTracer)->Trace(szMsg, NULL, prtALWAYS_REPORTED_MSG);
}

void cPrTracer::Trace2(tSTRING szMsg, tSTRING szSourceFile, int nSourceLine)
{
	if (!szMsg)
		return;

	USES_CONVERSION;
	std::basic_string<tCHAR> strMsg(szMsg);
	strMsg += _T(" (");
	strMsg += szSourceFile;
	strMsg += _T(", line ");
	strMsg += A2CT((boost::lexical_cast<std::basic_string<tCHAR> >(nSourceLine)).c_str());
	strMsg += _T(")");

	cPrTracer::Trace1((tSTRING)strMsg.c_str());
}

#define TRACE_EOL "\r\n"

void cPrTracer::Trace(tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel)
{	
	if(m_hTraceFile == INVALID_HANDLE_VALUE && !m_nTraceDebugEnable ||
		(tDWORD)dwLevel < min(m_nTraceFileMinLevel, m_nTraceDebugMinLevel) ||
		(tDWORD)dwLevel > max(m_nTraceFileMaxLevel, m_nTraceDebugMaxLevel))
	{
		return;
	}

	tCHAR  strTrace[0x400];
	tDWORD nSize = countof(strTrace) - countof(TRACE_EOL);
	tDWORD nLen = 0;
	
	nLen += MakeTracePrefix(strTrace, sizeof(strTrace), hObject, dwLevel);
	int res = _snprintf(strTrace + nLen, nSize - nLen, "%s" TRACE_EOL, str);
	if(res == -1)
		strcpy(strTrace + nSize, TRACE_EOL), nLen = nSize + countof(TRACE_EOL);
	else
		nLen += res;

	if (m_hTraceFile != INVALID_HANDLE_VALUE)
	{
		if(!m_hMtxFileAccess || WaitForSingleObject(m_hMtxFileAccess, 2000) == WAIT_OBJECT_0)
		{
			LONG dwHighPos = 0;
			SetFilePointer(m_hTraceFile, 0, &dwHighPos, FILE_END);
			DWORD nWritten;
			WriteFile(m_hTraceFile, strTrace, nLen, &nWritten, NULL);
			if(m_hMtxFileAccess)
				ReleaseMutex(m_hMtxFileAccess);
		}
	}

	if (m_nTraceDebugEnable)
	{
		OutputDebugString(strTrace);
	}
}

void cPrTracer::GetTraceLevels()
{
	bool bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;

	CRegKey keyTrace;
	if (ERROR_SUCCESS == keyTrace.Open(HKEY_LOCAL_MACHINE, AVP6_KEY, KEY_READ | (bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0)) != ERROR_SUCCESS)
	{
		CHAR	settingsName[MAX_PATH];
		DWORD	dwSize = _countof(settingsName) - 1;
		DWORD	dwType = 0;
		if (ERROR_SUCCESS != keyTrace.QueryStringValue(_T("Settings"), settingsName, &dwSize))
		{
			lstrcpy(settingsName, "Default");
		}

		CRegKey keySettings;
		if (ERROR_SUCCESS != keySettings.Open(keyTrace, settingsName, KEY_READ | (bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0)))
			return;


#define RegGetDword(val, def)\
		{\
		if (ERROR_SUCCESS != keySettings.QueryDWORDValue(#val, (DWORD&)m_n##val))\
			m_n##val = def;\
		if (m_n##val > prtANNOYING)\
			m_n##val = prtANNOYING;\
		}
		RegGetDword( TraceFileEnable,      cFALSE );
		RegGetDword( TraceDebugEnable,     cFALSE );
		RegGetDword( TraceFileMaxLevel,    prtNOTIFY );
		RegGetDword( TraceDebugMaxLevel,   prtIMPORTANT );
		RegGetDword( TraceFileMinLevel,    prtMIN_TRACE_LEVEL );
		RegGetDword( TraceDebugMinLevel,   prtMIN_TRACE_LEVEL );
#undef RegGetDword

	}
}


////////////////////////////////////////////////////////////////////////////

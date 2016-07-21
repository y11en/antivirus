// ActivityCheck.cpp: implementation of the CActivityCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <shlobj.h>
#include "ActivityCheck.h"

#include "stuff/mm.h"
#include "stuff/servfuncs.h"


#include "cmpimages.h"

#define _SUBIMAGE_SEARCH_SIZE_ 0//(32 * 1024 * 1024)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


PWCHAR pwcsP2PRegKeys[] = {
	L"\\KAZAA\\",
	L"\\IMESH\\CLIENT\\",
	L"\\GROKSTER\\",
};

PWCHAR pwcsP2PRegKeys2[] = {
	L"CLOUDLOAD",
	L"LOCALCONTENT",
	L"TRANSFER",
};

bool CActivityCheck::IsP2PKey(PWCHAR pwchKeyName, PWCHAR pwchValueName)
{
	PWCHAR p = wcsistr(pwchKeyName, L"\\SOFTWARE\\");
	if (!p)
		return false;
	p+=9;
	for (int i=0; i<countof(pwcsP2PRegKeys); i++)
	{
		int len = wcslen(pwcsP2PRegKeys[i]);
		if (0 == wcsnicmp(p, pwcsP2PRegKeys[i], len))
		{
			p += len;
			for (int j=0; j<countof(pwcsP2PRegKeys2); j++)
			{
				if (0 == wcsicmp(p, pwcsP2PRegKeys2[j]))
					return true;
			}
			break;
		}
	}
	return false;
}

bool CActivityCheck::CheckP2PKey(CSingleProcess* pProcess, PWCHAR pwchKeyName, PWCHAR pwchValueName)
{
	bool bResult = false;
	if (!pProcess)
		return false;
	if (IsP2PKey(pwchKeyName, pwchValueName))
	{
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("! BL: Check P2P Key:")
			_T("\n\t\tprocess:\t%s (pid=%d)")
			_T("\n\t\tkey:\t%s\\%s")
			_T("\n\t\tparent:\tpid=%d"),
			pProcess->m_pwchImagePath, pProcess->m_ProcessId, 
			pwchKeyName, pwchValueName,
			pProcess->m_ParentProcessId);
		PWCHAR pwcsRegVal = g_Service.RegQueryValue(pwchKeyName, pwchValueName);
		if (!pwcsRegVal)
		{
			pProcess->m_Log.AddFormatedString(DCB_BL, DL_ERROR, _T("! BL: Check P2P Key: cannot query value"));
		}
		else
		{
			PWCHAR pwcsPath = NULL;
			if (!AddP2PPath(pwcsRegVal, &pwcsPath))
			{
				pProcess->m_Log.AddFormatedString(DCB_BL, DL_ERROR, _T("! BL: Check P2P Key: false = AddP2PPath(%s)"), pwcsRegVal);
			}
			else
			{
				PWCHAR pwcsImagePath = NULL;
				bResult = true;
				if (!m_SelfCopyHistory.IsCopy2Path(pwcsPath, &pwcsImagePath))
				{
					pProcess->m_Log.AddFormatedString(DCB_BL, DL_ERROR, _T("! BL: Check P2P Key: false = IsCopy2Path(%s)"), pwcsPath);
				}
				else
				{
					pProcess->m_Log.AddFormatedString(DCB_BL, DL_ERROR, _T("! BL: Check P2P Key: true = IsCopy2Path(%s)\n\t\timage:\t%s"), pwcsPath, pwcsImagePath);
					if (pwcsImagePath)
					{
						bool bCmp;
						bCmp = (0 == wcscmp(pwcsImagePath, pProcess->m_pwchImagePath));
						if (!bCmp)
						{
							CSingleProcess* pParent = pProcess->GetParent();
							if (pParent)
							{
								bCmp = (0 == wcscmp(pwcsImagePath, pParent->m_pwchImagePath));				
								pParent->Release();
							}
						}
						if (bCmp)
						{
							pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("! BL: Check P2P Key:")
								_T("\n\t\tprocess:\t%s (pid=%d)")
								_T("\n\t\tkey:\t%s\\%s")
								_T("\n\t\tvalue:\t%s")
								_T("\n\t\tpath:\t%s")
								_T("\n\t\tparent:\tpid=%d"),
								pProcess->m_pwchImagePath, pProcess->m_ProcessId, 
								pwchKeyName, pwchValueName,
								pwcsRegVal, pwcsPath,
								pProcess->m_ParentProcessId);
							pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED P2P SC+RDL+ ***"));					
						}
					}
				}
			}
			FreeStr(pwcsPath);
			FreeStr(pwcsRegVal);
		}
	}
	return bResult;
}

bool CActivityCheck::CheckP2PRegistry()
{
	bool bResult = false;
	int i;
	for (i=0; i<countof(pwcsP2PRegKeys); i++)
	{
		WCHAR wcsRegPath[MAX_PATH];
		HKEY hKeyRoot;
		HKEY hKeyLocal;
		wcscpy(wcsRegPath, L"SOFTWARE");
		wcscat(wcsRegPath, pwcsP2PRegKeys[i]);
		PWCHAR pwcsRegKey = wcsRegPath + wcslen(wcsRegPath);
		for (int j=0; j<countof(pwcsP2PRegKeys2); j++)
		{
			wcscpy(pwcsRegKey, pwcsP2PRegKeys2[j]);
			for (int k=0; k<2; k++)
			{
				if (k==0)
					hKeyRoot = HKEY_LOCAL_MACHINE;
				else
					hKeyRoot = HKEY_CURRENT_USER;
				if (ERROR_SUCCESS == RegOpenKey(hKeyRoot, wcsRegPath, &hKeyLocal))
				{
					int v=0;
					while (true)
					{
						WCHAR wcsValueName[MAX_PATH];
						DWORD size = countof(wcsValueName);
						DWORD type;
						DWORD datasize = 0;
						
						if (ERROR_SUCCESS != RegEnumValue(hKeyLocal, v, wcsValueName, &size, NULL, &type, NULL, &datasize))
							break;
						if (type == REG_SZ || type == REG_EXPAND_SZ || type == REG_MULTI_SZ)
						{
							WCHAR wcsData[MAX_PATH];
							datasize = sizeof(wcsData);
							if (ERROR_SUCCESS == RegQueryValueEx(hKeyLocal, wcsValueName, NULL, &type, (LPBYTE)wcsData, &datasize))
							{
								if (AddP2PPath(wcsData))
									bResult = true;
							}
						}
						v++;
					}
					RegCloseKey(hKeyLocal);
				}
			}
		}
	}
	return bResult;
}


CActivityCheck::CActivityCheck()
{
	int i;
	PWCHAR pwcsDefaultNetPaths[] = {
		L"\\BEARSHARE\\SHARED\\",
		L"\\EDONKEY2000\\INCOMING\\",
		L"\\GNUCLEUS\\DOWNLOADS\\",
		L"\\GROKSTER\\MY GROKSTER\\",
		L"\\ICQ\\SHARED FILES\\",
		L"\\ICQ\\SHARED FOLDER\\",
		L"\\KAZAA LITE K++\\MY SHARED FOLDER\\",
		L"\\KAZAA LITE\\MY SHARED FOLDERS\\",
		L"\\KAZAA LITE\\MY SHARED FOLDER\\",
		L"\\KAZAA\\MY SHARED FOLDER\\",
		L"\\KMD\\MY SHARED FOLDER\\",
		L"\\LIMEWIRE\\SHARED\\",
		L"\\MORPHEUS\\MY SHARED FOLDER\\",
		L"\\MY DOWNLOADS\\",
		L"\\MY SHARED FOLDER\\",
		L"\\OVERNET\\INCOMING\\",
		L"\\RAPIGATOR\\SHARE\\",
		L"\\SHAREAZA\\DOWNLOADS\\",
		L"\\TESLA\\FILES\\",
		L"\\WINMX\\MY SHARED FOLDER\\",
		L"\\WINMX\\SHARED\\",
		L"\\XOLOX\\DOWNLOADS\\",
	};

	PWCHAR pwcsDefaultStartupPaths[] = {
		L"\\ALL USERS\\START MENU\\PROGRAMS\\STARTUP\\",
		L"\\STARTMENU\\PROGRAMME\\AUTOSTART\\",
	};

	m_Log.InitLog(_T("det_act.log"), &::m_Log);
	
	for (i=0; i<countof(pwcsDefaultNetPaths); i++)
		m_NetPaths.Add(pwcsDefaultNetPaths[i]);

	CheckP2PRegistry();

	WCHAR wcsPath[MAX_PATH];
	if (SHGetSpecialFolderPath(NULL, &wcsPath[0], CSIDL_STARTUP, FALSE))
		m_StartupPaths.Add(wcsPath);
	if (SHGetSpecialFolderPath(NULL, &wcsPath[0], CSIDL_ALTSTARTUP, FALSE))
		m_StartupPaths.Add(wcsPath);
	if (SHGetSpecialFolderPath(NULL, &wcsPath[0], CSIDL_COMMON_STARTUP, FALSE))
		m_StartupPaths.Add(wcsPath);
	if (SHGetSpecialFolderPath(NULL, &wcsPath[0], CSIDL_COMMON_ALTSTARTUP, FALSE))
		m_StartupPaths.Add(wcsPath);
	for (i=0; i<countof(pwcsDefaultStartupPaths); i++)
		m_StartupPaths.Add(pwcsDefaultStartupPaths[i]);
}

CActivityCheck::~CActivityCheck()
{

}

//+ ------------------------------------------------------------------------------------------

bool CActivityCheck::CheckSelfCopy(CSingleProcess* pProcess,
								   PWCHAR pwchDestinationPath)
{
	bool bFound;
	DWORD dwPos = 0;
	PWCHAR ParentImagePath = NULL;
	ULONG ParentId = 0;
	CSingleProcess* pParent;
	tStrReg reg;
	
	if (NULL == pProcess)
	{
		DbPrint((DCB_BL, DL_ERROR, _T("CheckSelfCopy: pProcess == NULL")));
		return false;
	}

	pParent = pProcess->GetParent();
	if (pParent)
	{
		ParentImagePath = pParent->m_pwchImagePath;
		ParentId = pParent->m_ParentProcessId;
	}

	//bFound = DoesImageContain2nd(pProcess->m_pwchImagePath, pwchDestinationPath, _SUBIMAGE_SEARCH_SIZE_, &dwPos);
	bFound = false;

	pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Check selfcopy:")
							_T("\n\t\tsrc:\t%s (pid=%d)")
							_T("\n\t\tdst:\t%s")
							_T("\n\t\tparent:\t%s (pid=%d)")
							_T("\n\t\tresult: %s (pos=0x%X)"),
		pProcess->m_pwchImagePath, pProcess->m_ProcessId, 
		pwchDestinationPath, 
		ParentImagePath, ParentId, 
		(bFound ? _T("true") : _T("false")), dwPos);

	if (!bFound)
	{
		PWCHAR pwchFilePart = wcsrchr(pProcess->m_pwchImagePath, '\\');
		if (!pwchFilePart)
			pwchFilePart = pProcess->m_pwchImagePath;
		else
			pwchFilePart++;
		if (0 == lstrcmpi(pwchFilePart, _T("CMD.EXE"))
			|| 0 == lstrcmpi(pwchFilePart, _T("COMMAND.COM")))
		{
			bFound = CheckSelfCopy(pParent, pwchDestinationPath);
		}
		else
		if (0 == lstrcmpi(pwchFilePart, _T("CSCRIPT.EXE"))
			|| 0 == lstrcmpi(pwchFilePart, _T("WSCRIPT.EXE")))
		{
			PWCHAR pwcsCmdLine = GetCopyStr( pProcess->m_pwchCmdLine );
			if (pwcsCmdLine)
			{
				INT i;
				PWCHAR params[10];
				INT nArgc = g_Service.ParseCmdLine(pwcsCmdLine, params, countof(params));
				for (i=1; i<nArgc; i++)
				{
					//bFound = DoesImageContain2nd(params[i], pwchDestinationPath, _SUBIMAGE_SEARCH_SIZE_, &dwPos);
					bFound = false;

					pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Check selfcopy:")
											_T("\n\t\tsrc:\t%s (param=%d, pid=%d)")
											_T("\n\t\tdst:\t%s")
											_T("\n\t\tparent:\t%s (pid=%d)")
											_T("\n\t\tresult: %s (pos=0x%X)"),
						params[i], i, pProcess->m_ProcessId, 
						pwchDestinationPath, 
						ParentImagePath, ParentId, 
						(bFound ? _T("true") : _T("false")), dwPos);			
				}

				FreeStr( pwcsCmdLine );
			}
		}
	}

	if (!bFound)
	{
		if (pParent)
			pParent->Release();
		return bFound;
	}

	pProcess->m_SelfCopyList.Add(pwchDestinationPath);

	m_SelfCopyHistory.AddEvent(pProcess->m_pwchImagePath, pwchDestinationPath, ParentImagePath, dwPos);
	if (pParent)
		pParent->Release();

	if (m_RegHistory.Contain(pwchDestinationPath, &reg))
	{
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: Copy found in auto-registry:")
			_T("\n\t\treg:\t%s\\%s")
			_T("\n\t\tval:\t%s")
			_T("\n\t\tfile:\t%s"),
			reg.m_pwchRegKeyName, reg.m_pwchRegValueName,
			reg.m_pwchRegValue,
			pwchDestinationPath);
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED SC+AR+ ***"));
	}

	if (m_StartupPaths.ContainSubstrForPath(pwchDestinationPath))
	{
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: Selfcopy into startup folder:")
			_T("\n\t\tsrc:\t%s")
			_T("\n\t\tdst:\t%s"),
			pProcess->m_pwchImagePath,
			pwchDestinationPath);
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED SC2STARTUP+ ***"));
	}

// possible false positives
//	if (m_RegHistory.Contain(pProcess->m_pwchImagePath, &reg))
//	{
//		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: Source found in auto-registry:")
//			_T("\n\t\treg:\t%s\\%s")
//			_T("\n\t\tval:\t%s")
//			_T("\n\t\tfile:\t%s"),
//			reg.m_pwchRegKeyName, reg.m_pwchRegValueName,
//			reg.m_pwchRegValue,
//			pProcess->m_pwchImagePath);
//		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED SC+AR+(src) ***"));
//	}	
//
	if (m_NetPaths.IsNetworkPath(pwchDestinationPath))
	{
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: selfcopy to network path:")
			_T("\n\t\tsrc:\t%s")
			_T("\n\t\tdst:\t%s"),
			pProcess->m_pwchImagePath,
			pwchDestinationPath);
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED SCN+ ***"));
	}	

	if (m_NetPaths.ContainSubstrForPath(pwchDestinationPath))
	{
		WIN32_FILE_ATTRIBUTE_DATA fad;
		LARGE_INTEGER nFileSize = {-1, -1}; 
		if (GetFileAttributesEx(pwchDestinationPath, GetFileExInfoStandard, &fad))
		{
			nFileSize.HighPart = fad.nFileSizeHigh;
			nFileSize.LowPart = fad.nFileSizeLow;
		}
		if ((nFileSize.QuadPart < 100*1024)                        // it's not installer
			|| pProcess->m_SelfCopyList.Count() >= 2)     // || many copies 
		{
			pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: selfcopy to network path(P2P):")
				_T("\n\t\tsrc:\t%s")
				_T("\n\t\tdst:\t%s"),
				pProcess->m_pwchImagePath,
				pwchDestinationPath);
			pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED SCN+(P2P) ***"));
		}
	}	

	if (pProcess->m_SelfCopyList.Count() >= 5)
	{
		pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED SC+(MULTIPLE) ***"));
	}

	return bFound;
}

//+ ------------------------------------------------------------------------------------------

void CActivityCheck::RedirectedOutput(CSingleProcess* pProcess, PWCHAR pwchComandLine)
{
	if (NULL == pProcess)
		return;
	CSingleProcess* pParent = pProcess->GetParent();
	PWCHAR pwchParentImage = _T("<unknown>");
	ULONG  ParentId = 0;

	if (pParent)
	{
		pwchParentImage = pParent->m_pwchImagePath;
		ParentId = pParent->m_ProcessId;
	}

	pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("! BL: Check redirected cmd:")
		_T("\n\t\tprocess:\t%s (pid=%d)")
		_T("\n\t\tcmdline:\t%s")
		_T("\n\t\tparent:\t%s (pid=%d)"),
		pProcess->m_pwchImagePath, pProcess->m_ProcessId, 
		pwchComandLine, 
		pwchParentImage, ParentId);
	pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED RDR+ ***"));
	if (pParent)
		pParent->Release();
}


//+ ------------------------------------------------------------------------------------------

void CActivityCheck::CheckAutorunKey(CSingleProcess* pProcess, 
									 PWCHAR pwchKeyName,
									 PWCHAR pwchValueName,
									 PWCHAR pwchValue)
{
	if (NULL == pProcess)
		return;
	ULONG ProcessId = pProcess->m_ProcessId;
	PWCHAR pwchImagePath = pProcess->m_pwchImagePath;
	ULONG ParentId = pProcess->m_ParentProcessId;
	PWCHAR pValAlloc = NULL;
	_autoFree aupVal(&pValAlloc);
	int results;

	if (NULL == pwchValue)
		pValAlloc = pwchValue = g_Service.RegQueryValue(pwchKeyName, pwchValueName);

	pProcess->m_Log.AddFormatedString(DCB_BL, DL_NOTIFY, _T("BL: Autorun key modify:")
							_T("\n\t\tparent:\tpid=%d")
							_T("\n\t\tprocess:\t%s (pid=%d)")
							_T("\n\t\tkey:\t%s\\%s")
							_T("\n\t\tvalue:\t%s"),
		ParentId,
		pwchImagePath, ProcessId, 
		pwchKeyName, pwchValueName,
		(pwchValue == NULL ? _T("<can't query>") : pwchValue));

	if (NULL == pwchValue)
		return;

	if (wcschr(pwchValue, '%')) // environment vars?
	{
		PWCHAR pwchTmp = g_Service.GetLongPathName(pwchValue); // also expands environment variables
		if (pwchTmp)
		{
			FreeStr(pValAlloc);
			pwchValue = pValAlloc = pwchTmp;
		}
	}

	tStrReg reg[10];
	results = m_RegHistory.AddEvent(pwchKeyName, pwchValueName, pwchValue, reg, countof(reg));

	for (int i=0; i<results; i++)
	{
		if (reg[i].m_pwchRegValueReparsedFilename)
		{
			if (m_SelfCopyHistory.IsCopy2Path(reg[i].m_pwchRegValueReparsedFilename, NULL))
			{
				//! BL - self copy with reg autostart
				pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: Registred object found in selfcopy history:")
										_T("\n\t\treg:\t%s\\%s")
										_T("\n\t\tval:\t%s")
										_T("\n\t\tfile:\t%s"),
					reg[i].m_pwchRegKeyName, reg[i].m_pwchRegValueName,
					reg[i].m_pwchRegValue, 
					reg[i].m_pwchRegValueReparsedFilename);
				pProcess->m_Log.AddFormatedString(DCB_BL, DL_IMPORTANT, _T("! BL: *** DETECTED SC+AR+ ***"));
			}
		}	
	}
}

// caller must free string returned in ppwcsLongPath
bool CActivityCheck::AddP2PPath(PWCHAR pwcsPath, PWCHAR* ppwcsLongPath)
{
	bool bResult = false;
	if (ppwcsLongPath)
		*ppwcsLongPath = NULL;
	if (NULL == pwcsPath)
		return false;
	if (*pwcsPath >= '0' && *pwcsPath <= '9') // skip folder id
	{
		PWCHAR p = pwcsPath+1;
		while (*p >= '0' && *p <= '9')
			p++;
		if (*p == ':')
			pwcsPath = p+1;
	}
	DWORD size = ExpandEnvironmentStrings(pwcsPath, NULL, 0);
	PWCHAR pwcsExpPath = (PWCHAR)global_Alloc(size*sizeof(WCHAR));
	if (size <= ExpandEnvironmentStrings(pwcsPath, pwcsExpPath, size))
	{
		PWCHAR pwcsLongPath = g_Service.GetLongPathName(pwcsExpPath);
		if (!pwcsLongPath)
			pwcsLongPath = GetCopyStr(pwcsExpPath);
		if (pwcsLongPath)
		{
			bResult = m_NetPaths.Add(pwcsLongPath);
			if (bResult && ppwcsLongPath)
				*ppwcsLongPath = pwcsLongPath;
			else
				FreeStr(pwcsLongPath);
		}								
	}
	FreeStr(pwcsExpPath);
	return bResult;
}



//+ ------------------------------------------------------------------------------------------


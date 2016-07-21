// Kis6Log.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "kavlog.h"
#include "gui.h"
#include "SpecialName6.h"
#include <common/SpecialName.h>

#include <vector>
typedef std::vector<std::string> strings;

extern "C"
{
	#define API
	#include "..\zip32\api.h"
}

#include <wininet.h>

CComModule _Module;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	_Module.Init(NULL, hInstance);


	CTraceManager tm;
	if( !tm.Init() )
	{
		::MessageBox(NULL, LoadRS(IDS_PRODUCT_NOT_FOUND).c_str(), LoadRS(IDS_TOOL_CAPTION).c_str(), MB_ICONERROR|MB_OK);
		return ERROR_NOT_FOUND;
	}

	CLogSettingsDlg(&tm).DoModal();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Some String Helpers

void RemoveLastSlash(string &strPath)
{
}

string GetFileFormPath(const string &strPath)
{
	string::size_type posSlash = strPath.find_last_of("\\/");
	posSlash = posSlash == -1 ? 0 : posSlash + 1;
	return strPath.substr(posSlash, strPath.size());
}


string LoadResourceString(UINT nStringID)
{
	char strBuffer[0x4000] = {0, };
	LoadString(_Module.GetModuleInstance(), nStringID, strBuffer, sizeof(strBuffer));
	return strBuffer;
}

//////////////////////////////////////////////////////////////////////////
// Dummies. Needs for ziplib reasons

int WINAPI DummyPassword(LPSTR, int, LPCSTR, LPCSTR)
{
	return 1;
}

int WINAPI DummyPrint(char far *buf, unsigned long size)
{
	return (unsigned int) size;
}

int WINAPI WINAPI DummyComment(char far *szBuf)
{
	szBuf[0] = '\0';
	return TRUE;
}

void SaveLicStorage(const char* p_pszFileName)
{
	HKEY hKey = 0 ;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\environment", 0, KEY_READ, &hKey) != ERROR_SUCCESS &&
		::RegOpenKeyEx(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_6 "\\environment", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return ;
	}

	DWORD dwBuffSize = 0 ;
	DWORD dwType     = REG_SZ ;
	if (::RegQueryValueEx(hKey, "ProductType", 0, &dwType, 0, &dwBuffSize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey) ;
		return ;
	}

	std::auto_ptr<TCHAR> pszBuff(new TCHAR[dwBuffSize]) ;
	if (::RegQueryValueEx(hKey, "ProductType", 0, &dwType, (BYTE*)pszBuff.get(), &dwBuffSize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey) ;
		return ;
	}

	::RegCloseKey(hKey) ;
	hKey = 0 ;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\KasperskyLab\\LicStorage", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return ;
	}

	dwBuffSize = 0 ;
	dwType     = REG_BINARY ;
	if (::RegQueryValueEx(hKey, pszBuff.get(), 0, &dwType, 0, &dwBuffSize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey) ;
		return ;
	}

	std::auto_ptr<BYTE> pBuff(new BYTE[dwBuffSize]) ;
	if (!pBuff.get())
	{
		::RegCloseKey(hKey) ;
		return ;
	}

	if (::RegQueryValueEx(hKey, pszBuff.get(), 0, &dwType, pBuff.get(), &dwBuffSize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey) ;
		return ;
	}

	::RegCloseKey(hKey) ;
	hKey = 0 ;

	FILE* pFile = fopen(p_pszFileName, "wb") ;
	if (!pFile)
	{
		return ;
	}

	fwrite(pBuff.get(), dwBuffSize, 1, pFile) ;
	fclose(pFile) ;
}

BOOL IsGlobalNamespaceSupported()
{
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ver);
	return (ver.dwPlatformId == VER_PLATFORM_WIN32_NT && ver.dwMajorVersion >= 5);
}


//////////////////////////////////////////////////////////////////////////
// CTraceManager

CTraceManager::CTraceManager() :
	m_bTraceEnabled(FALSE),
	m_nTraceLevel(400),
	m_nSRF(0),
	m_bIsDumpEnabled(FALSE)
{
}

CTraceManager::~CTraceManager()
{
	DeleteZippedTraces();
}

bool CTraceManager::Init()
{
	CRegKey Key;
	if( Key.Open(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\environment") != ERROR_SUCCESS &&
		Key.Open(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_6 "\\environment") != ERROR_SUCCESS)
		return false;

	m_strProductLocation.reserve(5 * MAX_PATH);
	m_strProductVersion.reserve(50);

	char strBuffer[5 * MAX_PATH];

	DWORD nLen;
	nLen = 5 * MAX_PATH;
	if( Key.QueryValue(strBuffer, "ProductRoot", &nLen) != ERROR_SUCCESS )
		return false;
	m_strProductLocation = strBuffer;

	nLen = 5 * MAX_PATH;
	if( Key.QueryValue(strBuffer, "ProductType", &nLen) != ERROR_SUCCESS )
		return false;
	m_strProductVersion = strBuffer;

	nLen = 5 * MAX_PATH;
	if( Key.QueryValue(strBuffer, "ProductVersion", &nLen) != ERROR_SUCCESS )
		return false;
	m_strProductVersion += " ";
	m_strProductVersion += strBuffer;

	bool bNewFormat = false;

	int nVerMajor = 6, nVerMinor = 0, nVerBuild = 0, nVerCompilation = 0;
#define MAKEVER64(a, b, c, d) ((((__int64)(WORD)(a)) << 48) | (((__int64)(WORD)(b)) << 32) | (((__int64)(WORD)(c)) << 16) | ((__int64)(WORD)(d)))
	if(sscanf(strBuffer, "%d.%d.%d.%d", &nVerMajor, &nVerMinor, &nVerBuild, &nVerCompilation) == 4)
	{
		if (MAKEVER64(nVerMajor, nVerMinor, nVerBuild, nVerCompilation) >= MAKEVER64(6, 0, 2, 540))
		{
			bNewFormat = true;
		}
		if (nVerMajor == 6 && nVerBuild >= 4 || nVerMajor == 7 && nVerBuild >= 0/*1*/ || nVerMajor >= 8)
		{
			m_bIsDumpEnabled = true;
		}
	}

	Key.Close();
	
	bool bIs6;
	if(!(bIs6 = Key.Open(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_6 "\\Trace\\Default") == ERROR_SUCCESS) &&
		Key.Open(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default") != ERROR_SUCCESS)
	{
		return false;
	}
	
	if( !Key || (Key.QueryValue(*(DWORD *)&m_bTraceEnabled, "TraceFileEnable") != ERROR_SUCCESS) )
		m_bTraceEnabled = FALSE;
	
	if( !Key || (Key.QueryValue(m_nTraceLevel, "TraceFileMaxLevel") != ERROR_SUCCESS) )
		m_nTraceLevel = 400;

	CRegKey pid;
	if (ERROR_SUCCESS == pid.Open(Key, "pid"))
	{
		m_PidToLevel.clear();
		for (DWORD i = 0; ; ++i)
		{
			char ValName[256];
			DWORD dwValNameSize = _countof(ValName);
			DWORD dwType;
			DWORD dwLevel = 0;
			DWORD dwLevelSize = sizeof(dwLevel);
			if (ERROR_SUCCESS != RegEnumValue(pid, i, ValName, &dwValNameSize, NULL, &dwType, (LPBYTE)&dwLevel, &dwLevelSize))
				break;
			DWORD pid = atol(ValName);
			m_PidToLevel[pid] = dwLevel;
		}
	}
	
	Key.Close();

	if( m_strProductLocation.size() )
	{
		string::iterator iLastChar = m_strProductLocation.end() - 1;
		if( *iLastChar == '\\' || *iLastChar == '/' )
			m_strProductLocation.erase(iLastChar);
	}
	
	m_strAvpExeLocation = m_strProductLocation + "\\avp.exe";
	if( GetFileAttributes(m_strAvpExeLocation.c_str()) == INVALID_FILE_ATTRIBUTES )
		return false;

	if(bNewFormat)
	{
		if(bIs6)
			KAV6::MakeSpecialName(strBuffer, _countof(strBuffer), "", "");
		else
			::MakeSpecialName(strBuffer, _countof(strBuffer), "", "");
		char *pLastSlash = strrchr(strBuffer, '\\');
		if(pLastSlash)
			*pLastSlash = 0;
		m_strTraceLocation = strBuffer;
	}
	else
	{
		m_strTraceLocation = m_strProductLocation.substr(0, m_strProductLocation.find_last_of("\\/"));
	}

	return true;
}

string CTraceManager::GetProductLocation()
{
	return m_strProductLocation;
}

string CTraceManager::GetProductVersion()
{
	return m_strProductVersion;
}

void CTraceManager::EnableTrace(bool bEnable)
{
	m_bTraceEnabled = bEnable;

	bool bSuccess = false;
	
	// try to modify registry, first
	CRegKey Key;
	if( Key.Open(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default") == ERROR_SUCCESS ||
		Key.Open(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_6 "\\Trace\\Default") == ERROR_SUCCESS)
	{
		bSuccess = 
			(Key.SetValue((DWORD)m_bTraceEnabled, "TraceFileEnable") == ERROR_SUCCESS) &&
			(Key.SetValue(m_nTraceLevel, "TraceFileMaxLevel")        == ERROR_SUCCESS);
	}

	CRegKey pid;
	if (ERROR_SUCCESS == pid.Open(Key, "pid"))
	{
		while (1)
		{
			char ValName[256];
			DWORD dwValNameSize = _countof(ValName);
			DWORD dwType, dwLevelSize;
			if (ERROR_SUCCESS != RegEnumValue(pid, 0, ValName, &dwValNameSize, NULL, &dwType, NULL, &dwLevelSize))
				break;
			pid.DeleteValue(ValName);
		}
		for (TPidToLevel::const_iterator it = m_PidToLevel.begin(); it != m_PidToLevel.end(); ++it)
		{
			if (it->second == 0)
				continue;
			char value[128];
			itoa(it->first, value, 10);
			pid.SetDWORDValue(value, it->second);
		}
	}

	// start 'avp.exe trace on|off file <level>'
	if( !bSuccess )
	{
		char strExecutable[5 * MAX_PATH];
		sprintf(strExecutable, "%s\\avp.com trace %s file %d", m_strProductLocation.c_str(), m_bTraceEnabled ? "on" : "off", m_nTraceLevel);

		PROCESS_INFORMATION pi = {0, };
		STARTUPINFO si = {sizeof(STARTUPINFO), 0, };
		if( CreateProcess(NULL, strExecutable, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, m_strProductLocation.c_str(), &si, &pi) )
		{
			bSuccess = true;
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
}

bool CTraceManager::IsTraceEnabled()
{
	return !!m_bTraceEnabled;
}

void CTraceManager::SetLevel(UINT nLevel)
{
	if( !IsTraceEnabled() )
		m_nTraceLevel = nLevel;
}

UINT CTraceManager::GetLevel()
{
	return (UINT)m_nTraceLevel;
}

void CTraceManager::SetSRF(UINT nSRF)
{
	m_nSRF = nSRF;
}

UINT CTraceManager::GetSRF()
{
	return m_nSRF;
}

bool CTraceManager::IsAnyTracesExists()
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile((m_strTraceLocation + "\\avp*.log").c_str(), &fd);
	if( hFind != INVALID_HANDLE_VALUE )
		::FindClose(hFind);
	return hFind != INVALID_HANDLE_VALUE;
}

bool CTraceManager::LogRegistry(LPCSTR strFilePrefix, LPCSTR strRegPath, string &strResultFile)
{
	SYSTEMTIME st; GetLocalTime(&st);
	strResultFile.resize(MAX_PATH);
	strResultFile.resize(sprintf((CHAR *)strResultFile.data(), "%s\\%s.%04d.%02d.%02d.%02d.%02d.%02d.%03d.log", m_strTraceLocation.c_str(), strFilePrefix, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
	
	CHAR strCmdLine[5 * MAX_PATH];
	sprintf(strCmdLine, "regedit /e \"%s\" %s", strResultFile.c_str(), strRegPath);
	
	PROCESS_INFORMATION pi = {0, };
	STARTUPINFO si = {sizeof(STARTUPINFO), 0, };
	if( ::CreateProcess(NULL, strCmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi) )
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);

		return true;
	}
	return false;
}

bool CTraceManager::ZipTraces()
{
	DeleteZippedTraces();

	string strAvp6Lic, strAvp5Lic, strAvp6LicBin;
	LogRegistry("avp.lic", "HKEY_LOCAL_MACHINE\\SOFTWARE\\KasperskyLab\\LicStorage", strAvp6Lic);
	LogRegistry("avp5.lic", "HKEY_CLASSES_ROOT\\LK.Auto",                             strAvp5Lic);

	SYSTEMTIME st; GetLocalTime(&st);
	strAvp6LicBin.resize(MAX_PATH);
	strAvp6LicBin.resize(sprintf((CHAR *)strAvp6LicBin.data(), "%s\\avp.lic.dat.%04d.%02d.%02d.%02d.%02d.%02d.%03d.log", m_strTraceLocation.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
	SaveLicStorage(strAvp6LicBin.c_str());

	strings files;
	
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile((m_strTraceLocation + "\\avp*.log").c_str(), &fd);
	if( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			string file = m_strTraceLocation;
			file += "\\";
			file += fd.cFileName;
			files.push_back(file);
		
		}
		while(::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	if( files.size() )
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		char strBuffer[5 * MAX_PATH];
		DWORD nChars = GetTempPath(sizeof strBuffer, strBuffer);
		sprintf(strBuffer + nChars, "avp.logs.%04d.%02d.%02d.%02d.%02d.%02d.%03d.zip", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		string strRootDir = m_strTraceLocation;
		if( strRootDir.size() == 2 )
			strRootDir += "\\";
		
		ZIPUSERFUNCTIONS ZipUserFunctions = {0, };
		ZipUserFunctions.print    = DummyPrint;
		ZipUserFunctions.password = DummyPassword;
		ZipUserFunctions.comment  = DummyComment;
		if( !ZpInit(&ZipUserFunctions) )
			return false;

		ZPOPT ZpOpt = {0, };
		ZpOpt.fSuffix = FALSE;        // include suffixes (not yet implemented)
		ZpOpt.fEncrypt = FALSE;       // true if encryption wanted
		ZpOpt.fSystem = FALSE;        // true to include system/hidden files
		ZpOpt.fVolume = FALSE;        // true if storing volume label
		ZpOpt.fExtra = FALSE;         // true if including extra attributes
		ZpOpt.fNoDirEntries = FALSE;  // true if ignoring directory entries
		ZpOpt.fVerbose = FALSE;       // true if full messages wanted
		ZpOpt.fQuiet = FALSE;         // true if minimum messages wanted
		ZpOpt.fCRLF_LF = FALSE;       // true if translate CR/LF to LF
		ZpOpt.fLF_CRLF = FALSE;       // true if translate LF to CR/LF
		ZpOpt.fJunkDir = FALSE;       // true if junking directory names
		ZpOpt.fGrow = FALSE;          // true if allow appending to zip file
		ZpOpt.fForce = FALSE;         // true if making entries using DOS names
		ZpOpt.fMove = FALSE;          // true if deleting files added or updated
		ZpOpt.fUpdate = FALSE;        // true if updating zip file--overwrite only if newer
		ZpOpt.fFreshen = FALSE;       // true if freshening zip file--overwrite only
		ZpOpt.fJunkSFX = FALSE;       // true if junking sfx prefix
		ZpOpt.fLatestTime = FALSE;    // true if setting zip file time to time of latest file in archive
		ZpOpt.fComment = FALSE;       // true if putting comment in zip file
		ZpOpt.fOffsets = FALSE;       // true if updating archive offsets for sfx files
		ZpOpt.fDeleteEntries = FALSE; // true if deleting files from archive
		ZpOpt.fRecurse = 0;           // subdir recursing mode: 1 = "-r", 2 = "-R"
		ZpOpt.fRepair = 0;            // archive repair mode: 1 = "-F", 2 = "-FF"
		ZpOpt.Date = NULL;            // Not using, set to NULL pointer
		ZpOpt.szRootDir =             // Set directory to current directory
			(char *)strRootDir.c_str();
		ZpSetOptions(&ZpOpt);

		ZCL zcl;
		zcl.lpszZipFN = strBuffer;
		zcl.argc = files.size();
		zcl.FNV = new char *[zcl.argc];
		for(int i = 0; i < zcl.argc; i++)
			zcl.FNV[i] = (char *)files[i].c_str();

		if( !ZpArchive(zcl) )
			m_strZippedLogs = zcl.lpszZipFN;
	}

	::DeleteFile(strAvp6Lic.c_str());
	::DeleteFile(strAvp5Lic.c_str());
	::DeleteFile(strAvp6LicBin.c_str());
	
	return !m_strZippedLogs.empty();
}

string CTraceManager::GetZippedTraces()
{
	return m_strZippedLogs;
}

void CTraceManager::DeleteZippedTraces()
{
	if( !m_strZippedLogs.empty() )
		DeleteFile(m_strZippedLogs.c_str());
}

bool CTraceManager::Send2KL()
{
	if( m_strZippedLogs.empty() )
		return false;

	bool bSuccess = false;
	
	HINTERNET hInternet = InternetOpen(NULL, PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, 0);
	if( hInternet )
	{
		HINTERNET hConnection = InternetConnect(hInternet, AVP6_TRACE_SERVER, INTERNET_INVALID_PORT_NUMBER, NULL, NULL, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if( hConnection )
		{
			char strRemoteFileName[5 * MAX_PATH];
			sprintf(strRemoteFileName, "/%u/%s", GetSRF(), GetFileFormPath(m_strZippedLogs).c_str());

			if( FtpPutFile(hConnection, m_strZippedLogs.c_str(), strRemoteFileName, FTP_TRANSFER_TYPE_BINARY, 0) )
				bSuccess = true;

			InternetCloseHandle(hConnection);
		}
		InternetCloseHandle(hInternet);
	}
	return bSuccess;
}

struct CComponentInfo
{
	LPCSTR pszName;
	DWORD  pid;
};

static const CComponentInfo g_ComponentInfo[] = {
	{ "BL", 34 },
	{ "Task Manager", 22 },
	{ "GUI", 19 },
	{ "AVEngine", 15 },
	{ "Licensing", 38013 },
	{ "Process Monitor", 49035 },
	{ "Scheduler", 58040 },
	{ "File Monitor", 49023 },
	{ "PDM", 58046 },
	{ "PDM2", 61013 },
	{ "On-demand scan", 49022 },
};

UINT CTraceManager::GetComponentCount()
{
	return _countof(g_ComponentInfo);
}

string CTraceManager::GetComponentName(UINT index)
{
	return g_ComponentInfo[index].pszName;
}

void CTraceManager::SetComponentLevel(UINT index, UINT nLevel)
{
	m_PidToLevel[g_ComponentInfo[index].pid] = nLevel;
}

UINT CTraceManager::GetComponentLevel(UINT index)
{
	return m_PidToLevel[g_ComponentInfo[index].pid];
}

bool CTraceManager::IsDumpEnabled() const
{
	return !!m_bIsDumpEnabled;
}

void CTraceManager::TryToCreateDump()
{
	OSVERSIONINFO osversioninfo = { 0 };
	osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);
	::GetVersionEx(&osversioninfo);

	std::string dumpEventName;
	if (IsGlobalNamespaceSupported())
	{
		dumpEventName = _T("Global\\6953EA60-8D5F-4529-8710-42F8ED3E8CDA");
	}
	else
	{
		dumpEventName = _T("6953EA60-8D5F-4529-8710-42F8ED3E8CDA");
	}

	HANDLE hDumpNotifyEvent = ::OpenEvent(EVENT_MODIFY_STATE, FALSE, dumpEventName.c_str());
	if (hDumpNotifyEvent != NULL)
	{
		::SetEvent(hDumpNotifyEvent);
		::Sleep(1500);
		::ResetEvent(hDumpNotifyEvent);
		::CloseHandle(hDumpNotifyEvent);
		TCHAR pathToDump[MAX_PATH];
		if (GetSpecialDir(pathToDump, MAX_PATH))
		{
			::ShellExecute(NULL, "explore", pathToDump, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}
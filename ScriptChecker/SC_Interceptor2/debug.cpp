#include "windows.h"
#include "debug.h"
#include <stdio.h>
#include <Version\ver_product.h>

#ifndef countof
#define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

static bool g_bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;

class cCriticalSection
{
public:
	cCriticalSection() { InitializeCriticalSection(&m_cs); };
	~cCriticalSection() { DeleteCriticalSection(&m_cs); };
	operator CRITICAL_SECTION*() { return &m_cs; };
	operator CRITICAL_SECTION&() { return m_cs; };

	void Lock() { EnterCriticalSection(&m_cs); };
	void Release() { LeaveCriticalSection(&m_cs); };
private:
	CRITICAL_SECTION m_cs;
};

class cAutoCS
{
public:
	cAutoCS(CRITICAL_SECTION& cs)
	{
		m_cs = &cs;
		EnterCriticalSection(m_cs);
	}
	~cAutoCS()
	{
		LeaveCriticalSection(m_cs);
	}
private:
	CRITICAL_SECTION* m_cs;
};

cCriticalSection g_cs;

bool WriteTraceFile(char* str)
{
	cAutoCS lock(g_cs);
	static HANDLE hFile = INVALID_HANDLE_VALUE;
	if (!str)
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
		return false;
	}
	if (INVALID_HANDLE_VALUE == hFile)
	{
		char sTraceFileName[MAX_PATH*2];
		char sExpand[MAX_PATH*2];
		SYSTEMTIME st;
		GetSystemTime(&st);
		HKEY hEnvKey;
		sExpand[0] = 0;
		sTraceFileName[0] = 0;
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\environment", 0, KEY_READ | (g_bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0), &hEnvKey))
		{
			DWORD dwType = REG_SZ;
			DWORD dwSize = MAX_PATH;
			if (ERROR_SUCCESS == RegQueryValueEx(hEnvKey, "TraceRoot", 0, &dwType, (LPBYTE)&sTraceFileName, &dwSize))
			{
				sTraceFileName[dwSize<countof(sTraceFileName) ? dwSize : countof(sTraceFileName)-1] = 0;
			}
			RegCloseKey(hEnvKey);
		}
		if (sTraceFileName[0]==0 || sTraceFileName[0]=='%')
		{
			strcpy(sExpand, "%ALLUSERSPROFILE%\\Application Data\\Kaspersky Lab\\");
			ExpandEnvironmentStrings(sExpand, sTraceFileName, countof(sTraceFileName));
		}
		wsprintf(sTraceFileName+strlen(sTraceFileName), "AVP.%d.%d_%02d.%02d_%02d.%02d_%x.scrchpg.dll.log", VER_PRODUCTVERSION_HIGH, VER_PRODUCTVERSION_COMPILATION, st.wMonth, st.wDay, st.wHour, st.wMinute, GetCurrentProcessId());
		if (sTraceFileName[0]=='%' || sTraceFileName[0]==0)
			return false;
		hFile = CreateFile(sTraceFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
	}
	if (INVALID_HANDLE_VALUE == hFile)
		return false;
	SetFilePointer(hFile, 0, 0, SEEK_END);
	DWORD nBytes;
	WriteFile(hFile, str, strlen(str), &nBytes, 0);
	return true;
}

bool IsTraceEnabled(bool* pbTraceFile, bool* pbTraceDbg )
{
	cAutoCS lock(g_cs);
	static DWORD nCheckTicks = 0;
	static int bTraceFile = 0;
	static int bTraceDbg = 0;

	if (!pbTraceFile && !pbTraceDbg)
		return false;

	if (GetTickCount() - nCheckTicks > 1000)
	{
		HKEY hTraceKey;
		nCheckTicks = GetTickCount();
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Trace", 0, KEY_READ | (g_bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0), &hTraceKey))
		{
			char sTraceProfile[0x100];
			DWORD dwType = REG_SZ;
			DWORD dwSize = countof(sTraceProfile);
			if (ERROR_SUCCESS != RegQueryValueEx(hTraceKey, "Settings", 0, &dwType, (LPBYTE)&sTraceProfile, &dwSize))
				lstrcpy(sTraceProfile, "Default");
			else
				sTraceProfile[dwSize<countof(sTraceProfile) ? dwSize : countof(sTraceProfile)-1] = 0;
			HKEY hTraceProfileKey;
			if (ERROR_SUCCESS == RegOpenKey(hTraceKey, sTraceProfile, &hTraceProfileKey))
			{
				if (pbTraceFile)
				{
					dwType = REG_DWORD;
					dwSize = sizeof(DWORD);
					bTraceFile = 0;
					RegQueryValueEx(hTraceProfileKey, "TraceFileEnable", 0, &dwType, (LPBYTE)&bTraceFile, &dwSize);
					if (!bTraceFile)
						WriteTraceFile(0); // close trace
				}
				if (pbTraceDbg)
				{
					dwType = REG_DWORD;
					dwSize = sizeof(DWORD);
					bTraceDbg = 0;
					RegQueryValueEx(hTraceProfileKey, "TraceDebugEnable", 0, &dwType, (LPBYTE)&bTraceDbg, &dwSize);
				}
				RegCloseKey(hTraceProfileKey);
			}
			RegCloseKey(hTraceKey);
		}
	}
	if (pbTraceFile)
		*pbTraceFile = !!bTraceFile;
	if (pbTraceDbg)
		*pbTraceDbg = !!bTraceDbg;
	return bTraceFile || bTraceDbg;
}

void DbgOutputString(char* szFormat, ...)
{
	char szDbgStr[0x100];
	char* pDbgStr = szDbgStr; 
	int size, n;
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	//size = _vscprintf(szFormat, marker) + 64;
	size = sizeof(szDbgStr)-10;
	n = _vsnprintf(pDbgStr, size, szFormat, marker);
	while (n < 0)
	{
		size += size;
		if (pDbgStr != szDbgStr)
			HeapFree(GetProcessHeap(),0,pDbgStr);
		pDbgStr = (char*)HeapAlloc(GetProcessHeap(),0,size+10);
		if (!pDbgStr)
			return;
		n = _vsnprintf(pDbgStr, size, szFormat, marker);
	} 
	va_end(marker);  
#if (defined(_DEBUG) || defined(ENABLE_ODS))
	OutputDebugString(pDbgStr);
	Sleep(0);
#endif
	if (pDbgStr != szDbgStr)
		HeapFree(GetProcessHeap(),0,pDbgStr);
}

void DbgOutputStringLF(char* szFormat, ...)
{
	bool bTraceFile;
	bool bTraceDbg;
	if (!IsTraceEnabled(&bTraceFile, &bTraceDbg))
		return;
	//static long count = 0;
	SYSTEMTIME st;
	GetSystemTime(&st);
	char szDbgStr[0x100];
	char* pDbgStr = szDbgStr; 
	int n1, n2, size;
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	size = sizeof(szDbgStr)-10;
	n1 = _snprintf(pDbgStr, size, "%02d:%02d:%02d.%03d\t%x.%x\t", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId(), GetCurrentThreadId());
	if (n1 > 0)
		n2 = _vsnprintf(pDbgStr+n1, size-n1, szFormat, marker);
	while (n1 < 0 || n2 < 0 || n2 == size-n1)
	{
		size += 0x100;
		if (pDbgStr != szDbgStr)
			HeapFree(GetProcessHeap(),0,pDbgStr);
		pDbgStr = (char*)HeapAlloc(GetProcessHeap(),0,size+10);
		if (!pDbgStr)
			return;
		n1 = _snprintf(pDbgStr, size, "%02d:%02d:%02d.%03d\t%x.%x\t", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId(), GetCurrentThreadId());
		if (n1 > 0)
		{
			n2 = _vsnprintf(pDbgStr+n1, size-n1, szFormat, marker);
			if (n2 == -1 && size > 1024*1024)
			{
				pDbgStr[n1] = 0;
				break;
			}
		}
	} 
	va_end(marker);  
	lstrcat(pDbgStr, "\n"); 
#if (defined(_DEBUG) || defined(ENABLE_ODS))
	if (bTraceDbg)
		OutputDebugString(pDbgStr);
	if (bTraceFile)
		WriteTraceFile(pDbgStr);
#endif
	if (pDbgStr != szDbgStr)
		HeapFree(GetProcessHeap(),0,pDbgStr);
}

char* DbgStringFromGUID(char* szBuff, DWORD dwBuffSize, const GUID* pGUID)
{
	OLECHAR* pguid;
	TCHAR Guid[MAX_PATH];
	TCHAR Subkey[MAX_PATH];
	TCHAR Subkey2[MAX_PATH];
	LONG  uKeyLen;

	if (!pGUID)
	{
		strncpy(szBuff, "{NULL}", dwBuffSize);
		return szBuff;
	}

	__try
	{
	StringFromCLSID(*pGUID, &pguid);
	wsprintf(Guid, "%S", pguid);
	CoTaskMemFree(pguid);

	uKeyLen = dwBuffSize;
	wsprintf(Subkey, "CLSID\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey, szBuff, &uKeyLen) == ERROR_SUCCESS)
		return szBuff;

	uKeyLen = dwBuffSize;
	wsprintf(Subkey2, "Interface\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey2, szBuff, &uKeyLen) == ERROR_SUCCESS)
		return szBuff;

	uKeyLen = dwBuffSize;
	wsprintf(Subkey2, "Filter\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey2, szBuff, &uKeyLen) == ERROR_SUCCESS)
		return szBuff;

	uKeyLen = dwBuffSize;
	wsprintf(Subkey2, "TypeLib\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey2, szBuff, &uKeyLen) == ERROR_SUCCESS)
		return szBuff;

	strncpy(szBuff, Guid, min(sizeof(Guid), dwBuffSize));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		szBuff[0]=0;
	}
	return szBuff;
}

void DbgPrintGUID(REFIID riid)
{
	char szBuff[0x100];
	OutputDebugString(DbgStringFromGUID(szBuff, sizeof(szBuff), &riid));
	return;
}
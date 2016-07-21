#include "windows.h"
#include "debug.h"

#if (defined(_DEBUG) || defined(ENABLE_ODS))

void DbgOutputString(char* szFormat, ...)
{
	char szDbgStr[MAX_PATH];
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	wvsprintf(szDbgStr, szFormat, marker);
	va_end(marker);  
	OutputDebugString(szDbgStr);
}

void DbgOutputStringLF(char* szFormat, ...)
{
	static long count = 0;
	char szDbgStr[MAX_PATH];
	int n;
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	n = wsprintf(szDbgStr, "%04d: %03x: ", InterlockedIncrement(&count), GetCurrentThreadId());
	wvsprintf(szDbgStr + n, szFormat, marker);
	va_end(marker);  
	lstrcat(szDbgStr, "\n"); 
	OutputDebugString(szDbgStr);
	Sleep(0);
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

#endif

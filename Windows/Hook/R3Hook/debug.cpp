#define _CRTIMP
#include "windows.h"
#include "debug.h"
#include <stdio.h>

#if (defined(_DEBUG) || defined(ENABLE_ODS))

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
	OutputDebugString(pDbgStr);
	if (pDbgStr != szDbgStr)
		HeapFree(GetProcessHeap(),0,pDbgStr);
}

void DbgOutputStringLF(char* szFormat, ...)
{
	static long count = 0;
	char szDbgStr[0x100];
	char* pDbgStr = szDbgStr; 
	int n1, n2, size;
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	size = sizeof(szDbgStr)-10;
	n1 = _snprintf(pDbgStr, size, "%04d: %03x: ", InterlockedIncrement(&count), GetCurrentThreadId());
	if (n1 > 0)
		n2 = _vsnprintf(pDbgStr+n1, size-n1, szFormat, marker);
	while (n1 < 0 || n2 < 0)
	{
		size += 0x10;
		if (pDbgStr != szDbgStr)
			HeapFree(GetProcessHeap(),0,pDbgStr);
		pDbgStr = (char*)HeapAlloc(GetProcessHeap(),0,size+10);
		if (!pDbgStr)
			return;
		n1 = _snprintf(pDbgStr, size, "%04d: %03x: ", InterlockedIncrement(&count), GetCurrentThreadId());
		if (n1 > 0)
			n2 = _vsnprintf(pDbgStr+n1, size-n1, szFormat, marker);
	} 
	va_end(marker);  
	lstrcat(pDbgStr, "\n"); 
	OutputDebugString(pDbgStr);
	if (pDbgStr != szDbgStr)
		HeapFree(GetProcessHeap(),0,pDbgStr);
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

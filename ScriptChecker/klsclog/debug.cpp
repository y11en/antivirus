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
//	lstrcat(szDbgStr, "\n"); 
	OutputDebugString(szDbgStr);
}

void DbgPrintGUID(REFIID riid)
{
	OLECHAR* pguid;
	TCHAR Guid[MAX_PATH];
	TCHAR Buffer[MAX_PATH];
	TCHAR Subkey[MAX_PATH];
	TCHAR Subkey2[MAX_PATH];
//	DWORD TypeStr = REG_SZ;
	LONG uKeyLenght;

	StringFromCLSID(riid, &pguid);
	wsprintf(Guid, "%S", pguid);
	CoTaskMemFree(pguid);

	uKeyLenght = sizeof(Buffer);
	wsprintf(Subkey, "CLSID\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey, Buffer, &uKeyLenght) == ERROR_SUCCESS)
	{
		OutputDebugString(Buffer);
		return;
	}

	uKeyLenght = sizeof(Buffer);
	wsprintf(Subkey2, "Interface\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey2, Buffer, &uKeyLenght) == ERROR_SUCCESS)
	{
		OutputDebugString(Buffer);
		return;
	}

	uKeyLenght = sizeof(Buffer);
	wsprintf(Subkey2, "Filter\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey2, Buffer, &uKeyLenght) == ERROR_SUCCESS)
	{
		OutputDebugString(Buffer);
		return;
	}

	uKeyLenght = sizeof(Buffer);
	wsprintf(Subkey2, "TypeLib\\%s", Guid);
	if (RegQueryValue(HKEY_CLASSES_ROOT, Subkey2, Buffer, &uKeyLenght) == ERROR_SUCCESS)
	{
		OutputDebugString(Buffer);
		return;
	}

	OutputDebugString(Guid);
	return;
}

#endif

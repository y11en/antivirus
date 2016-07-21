#include "vassist.h"
#include "Shlwapi.h"

//char g_DbgMsg[0x200];

DWORD GetVisualAssistHash(CHAR* pStr)
{
	DWORD l = lstrlen(pStr);
	DWORD d = 0;
	DWORD h = 0;
	
	while(l)
	{
		DWORD t;
		BYTE b;
		b = (BYTE)pStr[l-1];
		if (d && b==':')
			break;
		if (b >= 'A' && b <= 'Z')
			b |= 0x20;
		t = h + b;
		t <<= 5;
		t += b;
		h += t;
		l--;
		d++;
	};
	return h;
}

BOOL GetVisualAssistCachePath(LPBYTE lpVACachePath, LPDWORD lpcbPath)
{
	BOOL bRes = FALSE;
	HKEY hKey;
	if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Whole Tomato\\Visual Assist", &hKey))
	{
		DWORD dwType = REG_SZ;
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, "Data Dir", NULL, &dwType, lpVACachePath, lpcbPath))
		{
			bRes = TRUE;
		}
		RegCloseKey(hKey);
	}
	return bRes;
}

INT GetVisualAssistCacheFile(char* pCacheFileName, char* pSourceFileName)
{
	static CHAR szVACachePath[MAX_PATH] = "";
	DWORD len = sizeof(szVACachePath);
	if (!pCacheFileName || !pSourceFileName || !pSourceFileName[0])
		return 0;
	if (!szVACachePath[0] && GetVisualAssistCachePath((LPBYTE)szVACachePath, &len)==FALSE)
		return 0;
	return wsprintf(pCacheFileName, "%s\\vc6\\cache\\%c%u", szVACachePath, pSourceFileName[0] | 0x20, GetVisualAssistHash(pSourceFileName));
}

INT GetDefinitionFromVisualAssistCacheFile(char* pDefinition, DWORD dwMaxLen, char* pSymbolName, char* pSourceFileName)
{
	CHAR szCacheFile[MAX_PATH];
	HANDLE hFile;
	static CHAR* pFileData = NULL;
	static WIN32_FILE_ATTRIBUTE_DATA sLastFAD = {0,0,0,0,0,0};
	WIN32_FILE_ATTRIBUTE_DATA sFAD;
	CHAR* ptr;
	int len;

	len = GetVisualAssistCacheFile(szCacheFile, pSourceFileName);
	if (len == 0)
		return 0;
	lstrcpy(szCacheFile+len, ".d2");
	if (!GetFileAttributesEx(szCacheFile, GetFileExInfoStandard, &sFAD))
	{
		lstrcpy(szCacheFile+len, ".d1");
		if (!GetFileAttributesEx(szCacheFile, GetFileExInfoStandard, &sFAD))
			return 0;
	}
	if (sFAD.nFileSizeLow == 0)
		return 0;
	sFAD.ftLastAccessTime.dwLowDateTime = 0;
	sFAD.ftLastAccessTime.dwHighDateTime = 0;

	if (memcmp(&sFAD, &sLastFAD, sizeof(sFAD)) != 0)
	{
		hFile = CreateFile(szCacheFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwRead;
			if (pFileData == NULL)
				pFileData = (CHAR*)HeapAlloc(GetProcessHeap(), 0, sFAD.nFileSizeLow+0x10);
			else
				pFileData = (CHAR*)HeapReAlloc(GetProcessHeap(), 0, pFileData, sFAD.nFileSizeLow+0x10);
			if (pFileData == NULL)
			{
				CloseHandle(hFile);
				return 0;
			}
			pFileData[sFAD.nFileSizeLow] = 0;
			ReadFile(hFile, pFileData, sFAD.nFileSizeLow, &dwRead, NULL);
			CloseHandle(hFile);
			if (dwRead != sFAD.nFileSizeLow)
				return 0;
			memcpy(&sLastFAD, &sFAD, sizeof(sFAD));
		}
	}

	ptr = pFileData;
	while (ptr)
	{
		ptr = StrStr(ptr, pSymbolName);
		if (ptr && *(ptr-1)==0x0A)
		{
			int len;
			//CHAR* end = ptr-1;
			//do {
			//	end = strchr(end+1, 0x0A);
			//} while (end && *(end+1) != ':' && *(end+1) != 0);
			CHAR* end = strchr(ptr, 0x0A);

			if (!end)
				return 0;
			len = min((DWORD)(end - ptr + 1), dwMaxLen);
			//wsprintf(g_DbgMsg, "pDefinition=%08X, pFileData=%08X(%d), ptr=%08X(%d), end=%08X(%d), len=%08X\n", pDefinition, pFileData, sFAD.nFileSizeLow, ptr, ptr-pFileData, end, end-pFileData, len );
			//OutputDebugString(g_DbgMsg);
			lstrcpyn(pDefinition, ptr, len);
			return len;
		}
		else if (ptr)
			ptr++;
	}

	return 0;
}

BOOL IsTypeModifier(CHAR* pType, DWORD dwLen)
{
	DWORD i;
	const CHAR* pTypeModifiers[] = {
		"auto",
		"register",
		"static",
		"extern",
		"const",
		"volatile",
		"class",
		"struct",
		"union",
	};

	for (i=0; i<(sizeof(pTypeModifiers)/sizeof(pTypeModifiers[0])); i++)
	{
		DWORD l = lstrlen(pTypeModifiers[i]);
		if (memcmp(pType, pTypeModifiers[i], l) == 0)
			return TRUE;
	}
	return FALSE;
}

INT GetTypeFromVisualAssistDefinition(CHAR* pTypeStr, DWORD dwMaxLen, CHAR* pDefinition)
{
	CHAR* ptr;

	ptr = strchr(pDefinition, VISUAL_ASSIST_STRING_DELIMITER);
	if (ptr == NULL)
		return 0;
	pDefinition = ptr+1;

	while (*ptr)
	{
		if (*ptr == 0x20 || *ptr == 0x09 || *ptr == VISUAL_ASSIST_STRING_DELIMITER)
		{
			DWORD len = min((DWORD)(ptr-pDefinition), dwMaxLen-1);
			if (*ptr != VISUAL_ASSIST_STRING_DELIMITER && IsTypeModifier(pDefinition, len))
			{
				pDefinition+=len;
			}
			else
			{
				lstrcpyn(pTypeStr, pDefinition, len+1);
				return len;
			}
		}
		ptr++;
	}
	return 0;
}


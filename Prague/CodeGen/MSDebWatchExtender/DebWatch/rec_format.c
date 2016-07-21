#include <windows.h>
#include "Shlwapi.h"

#include "..\common\custview.h"
#include "..\common\evalinfo.h"
#include "vassist.h"

CHAR g_szAutoExpFullPathName[MAX_PATH];

CHAR* RetriveNextSrcFileFormSrcInfo(EVAL_INFO* pEvalInfo, DWORD dwStartPos, DWORD* pdwFoundPos )
{
	if (pdwFoundPos)
		*pdwFoundPos = 0;
	
	if (pEvalInfo == NULL || pEvalInfo->pPdbInfo == NULL || pEvalInfo->pPdbInfo->pPdbSrcInfo == NULL || pEvalInfo->pPdbInfo->dwSrcInfoSize == 0)
		return NULL;

	dwStartPos &= ~3;

	for (dwStartPos+=4; dwStartPos < pEvalInfo->pPdbInfo->dwSrcInfoSize; dwStartPos+=4)
	{
		BYTE* pSrcInfo = (BYTE*)pEvalInfo->pPdbInfo->pPdbSrcInfo;
		if (pSrcInfo[dwStartPos+2] == ':' && 
			pSrcInfo[dwStartPos+3]=='\\' && 
			pSrcInfo[dwStartPos] > 0 && 
			pSrcInfo[dwStartPos]+dwStartPos < pEvalInfo->pPdbInfo->dwSrcInfoSize &&
			pSrcInfo[pSrcInfo[dwStartPos]+dwStartPos] != 0 )
		{
			if (pdwFoundPos)
				*pdwFoundPos = dwStartPos;
			return pSrcInfo+dwStartPos;
		}
	}

	return NULL;
}

VOID CpySymName(CHAR* pDest, CHAR* pSrc, DWORD dwSymLen)
{
	DWORD i;
	*pDest++ = ':';
	for (i=0; i<dwSymLen; i++)
	{
		if (pSrc[i]==':' && pDest[-1]==':')
			continue;
		*pDest++ = pSrc[i];
	}
	*pDest++ = 0;
	return;
}

DWORD GetVisualAssistType(CHAR* szType, DWORD dwMaxLen, EVAL_INFO* pEvalInfo)
{
	LONG l;
	CHAR szDefinition[MAX_PATH];
	CHAR szSymName[0x100];
	CHAR szSourceFileName[MAX_PATH];
	DWORD dwPos = 0;
	CHAR* pSrcFile = NULL;
	
	if (!pEvalInfo || !SYM_NAME_VALID(pEvalInfo->pSymInfoFunc) || !SYM_NAME_VALID(pEvalInfo->pSymInfoVar))
		return 0;
	
	CpySymName(szSymName, pEvalInfo->pSymInfoFunc->sName, pEvalInfo->pSymInfoFunc->bNameLen);
	if ((PVOID)pEvalInfo->pSymInfoFunc != (PVOID)pEvalInfo->pSymInfoVar)
	{
		CpySymName(szSymName + strlen(szSymName), pEvalInfo->pSymInfoVar->sName, pEvalInfo->pSymInfoVar->bNameLen);
	}
/*	
#ifdef _DEBUG
	{
		DWORD dwPos = 0;
		char szBuffer[0x100];
		char szSrc[0x100];

		while (NULL != (pSrcFile = RetriveNextSrcFileFormSrcInfo(pEvalInfo, dwPos, &dwPos)))
		{
			lstrcpyn(szSrc, pSrcFile+1, *pSrcFile + 1);
			wsprintf(szBuffer, "%s\n", szSrc);
			OutputDebugString(szBuffer);
		}
	}
#endif
*/
	
	while (NULL != (pSrcFile = RetriveNextSrcFileFormSrcInfo(pEvalInfo, dwPos, &dwPos)))
	{
		lstrcpyn(szSourceFileName, pSrcFile+1, *(BYTE*)pSrcFile+1);
		
		if (l = GetDefinitionFromVisualAssistCacheFile(szDefinition, sizeof(szDefinition), szSymName, szSourceFileName))
		{
			if (l = GetTypeFromVisualAssistDefinition(szType, dwMaxLen, szDefinition))
			{
				return l;
			}
		}
	}
	return 0;
}

// this function formats pointer to HRESULT value (passed in dwValue parameter) into readable string
HRESULT WINAPI format_vatype( 
							 DWORD dwAddr,      // low 32-bits of address
							 DEBUGHELPER *pHelper, // callback pointer to access helper functions
							 int nBase,            // decimal or hex
							 BOOL bIgnore,         // not used
							 char *pResult,        // where the result needs to go
							 size_t max,           // how large the above buffer is
							 EVAL_INFO* pEvalInfo      // always pass zero
							 ) {
	
	CHAR szType[0x100];
	if (GetVisualAssistType(szType, sizeof(szType), pEvalInfo))
	{
		lstrcpyn(pResult, szType, max);
		return S_OK;
	}
	
	return E_FAIL;
}


// this function called from evaluator dll to recognize specified format string
// for example, if in Watch windows written "0x80004005, hr", format string is "hr"
// the function enumerates formats table and return format function if any
fnCustomViewer __stdcall RecognizeFormat(char* szFormat, EVAL_INFO* pEvalInfo) {
	CHAR szAddinString[0x100];
	CHAR* pDllName = NULL;
	CHAR* pFuncName = NULL;
	BOOL  bRetrived = FALSE;
	BOOL  bParsedOk = FALSE;
	DWORD error = 0;
	
	_try
	{
		szAddinString[0] = 0;
		
		if (szFormat!=NULL && *szFormat!=0) // some type specified
		{
			if (!GetPrivateProfileString("AutoExpandByFormatString", szFormat, NULL, szAddinString, sizeof(szAddinString), g_szAutoExpFullPathName))
			{
				error = GetLastError();
				if (StrCmpI(szFormat, "vatype") == 0)
					return &format_vatype;
			}
			else
				bRetrived = TRUE;
		}
		
		// try to retrive type from Visual Assist cache
		if (!bRetrived)
		{
			CHAR szType[MAX_PATH];
			DWORD dwTypeLen;
			
			if (dwTypeLen = GetVisualAssistType(szType, sizeof(szType), pEvalInfo))
			{
				if (GetPrivateProfileString("AutoExpand", szType, NULL, szAddinString, sizeof(szAddinString), g_szAutoExpFullPathName))
					bRetrived = TRUE;
				else
				{
					if (szType[dwTypeLen-1] == '*')
					{
						szType[dwTypeLen-1] = 0;
						if (GetPrivateProfileString("AutoExpand", szType, NULL, szAddinString, sizeof(szAddinString), g_szAutoExpFullPathName))
							bRetrived = TRUE;
					}
				}
			}
		}
		
		// No type specified - look for symbol name
		if (!bRetrived && pEvalInfo && SYM_NAME_VALID(pEvalInfo->pSymInfoVar))
		{
			CHAR szVarName[0x100];
			memcpy(szVarName, pEvalInfo->pSymInfoVar->sName, pEvalInfo->pSymInfoVar->bNameLen);
			szVarName[pEvalInfo->pSymInfoVar->bNameLen] = 0;
			if (!GetPrivateProfileString("AutoExpandBySymbolName", szVarName, NULL, szAddinString, sizeof(szAddinString), g_szAutoExpFullPathName))
				error = GetLastError();
			else
				bRetrived = TRUE;
		}
		
		// No type specified - look for register name
		if (!bRetrived && pEvalInfo && pEvalInfo->dwRegisterIdx != 0)
		{
			DWORD i;
			CHAR szRegName[0x10];
			for (i=0; i<sizeof(Registers)/sizeof(Registers[0]);i++)
			{ 
				if (Registers[i].dwIdx == pEvalInfo->dwRegisterIdx)
				{
					memcpy(szRegName, Registers[i].sName, Registers[i].bNameLen);
					szRegName[Registers[i].bNameLen] = 0;
					break;
				}
			}
			if (!GetPrivateProfileString("AutoExpandBySymbolName", szRegName, NULL, szAddinString, sizeof(szAddinString), g_szAutoExpFullPathName))
				error = GetLastError();
			else
				bRetrived = TRUE;
		}
		
		// Try to parse retrived string
		if (bRetrived)
		{
			CHAR* ptr;
			
			ptr = StrStrI(szAddinString, "$ADDIN(");
			if (ptr)
			{
				ptr += 7;
				while (*ptr && (*ptr==0x20 || *ptr==0x09))
					ptr++;
				pDllName = ptr;
				ptr = StrChr(ptr, ',');
				if (ptr)
				{
					*ptr++=0;
					while (*ptr && (*ptr==0x20 || *ptr==0x09))
						ptr++;
					pFuncName = ptr;
					ptr = StrStr(ptr, ")");
					if (ptr)
					{
						*ptr = 0;
            while(ptr>pFuncName && (*(ptr-1)==0x20 || *(ptr-1)==0x09))
              *--ptr=0;
						bParsedOk = TRUE;
					}
				}
			}
		}
		
		// Get parsed function and call it
		if (bParsedOk)
		{
			HMODULE hLib;
			BOOL bLibraryLoaded = FALSE;
			hLib = GetModuleHandle(pDllName);
			if (!hLib)
			{
				hLib = LoadLibrary(pDllName);
				bLibraryLoaded = TRUE;
			}
			if (hLib)
			{
				fnCustomViewer pCustomViewerProc;
				if (pCustomViewerProc = (fnCustomViewer)GetProcAddress(hLib, pFuncName))
					return pCustomViewerProc;
				
				if (bLibraryLoaded)
					FreeLibrary(hLib);
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// unexpected exception occured
	}
	
	return NULL;
}

BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
	CHAR* pFileName;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		GetModuleFileName(NULL, g_szAutoExpFullPathName, sizeof(g_szAutoExpFullPathName));
		pFileName = StrRChr(g_szAutoExpFullPathName, NULL, '\\');
		if (pFileName) 
			pFileName++;
		else
			pFileName = g_szAutoExpFullPathName;
		lstrcpy(pFileName, "AUTOEXP.DAT");
	}
	return TRUE;
}

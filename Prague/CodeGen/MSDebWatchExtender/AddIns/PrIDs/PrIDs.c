#include <windows.h>
#include <stdio.h>
#include "shlwapi.h"
#include "..\..\common\custview.h"
#include "..\..\common\fcache.h"

#define DEF_HEADERS_PATH "o:\\prague\\include\\"

#ifdef __cplusplus
extern "C" {
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	WIN32_FIND_DATA fd;
	HANDLE hFind;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hFind = FindFirstFile(DEF_HEADERS_PATH "pr_d_*.h", &fd );
		if (hFind == INVALID_HANDLE_VALUE)
			return FALSE; // there is no any IDs headers
		FindClose(hFind);
		break;
	default:
		break;
	}

    return TRUE;
}

#define DEF_ID_STR "DEF_IDENTIFIER( "
#define DEF_ID_SIZE (sizeof(DEF_ID_STR)-1)

BOOL GetDefinitionByName(IN CACHED_FILE* pCachedFile, IN CHAR* pID, OUT CHAR* pDefinition)
{
	CHAR* ptr = (CHAR*)pCachedFile->pFileData-1;
	while (ptr)
	{
		CHAR* end; 
		ptr = StrStr(ptr+1, pID);
		if (ptr && lstrcmp(ptr-DEF_ID_SIZE, DEF_ID_STR)==0)
		{
			ptr -= DEF_ID_SIZE;
			if (NULL == (end = strchr(ptr, 0x0D)))
				return FALSE;
			lstrcpyn(pDefinition, ptr, end - ptr + 1);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GetDefinitionByValue(IN CACHED_FILE* pCachedFile, IN DWORD dwID, OUT CHAR* pDefinition)
{
	CHAR  szID[0x20];
	CHAR* ptr = (CHAR*)pCachedFile->pFileData-1;

	wsprintf(szID, " ) // 0x%08X", dwID);
	while (ptr)
	{
		CHAR* end; 
		CHAR* start; 
		ptr = StrStr(ptr+1, szID);
		if (ptr)
		{
			start = StrRChr((CHAR*)pCachedFile->pFileData, ptr, 0x0A);
			if (!start || StrCmpN(start+1, DEF_ID_STR, DEF_ID_SIZE) != 0)
				continue;
			start++;
			if (NULL == (end = strchr(start, 0x0D)))
				return FALSE;
			lstrcpyn(pDefinition, start, end - start + 1);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL RetriveDefineName(IN CHAR* pDefinition, OUT CHAR* pName)
{
	CHAR* ptr = pDefinition;
	CHAR* end;
	if (StrCmpN(ptr, DEF_ID_STR, DEF_ID_SIZE) != 0)
		return FALSE;
	ptr+=DEF_ID_SIZE;
	end = StrStr(ptr, ",");
	if (!end)
		return FALSE;
	lstrcpyn(pName, ptr, end-ptr+1);
	return TRUE;
}


// this function formats value (passed in dwValue and pEvalInfo parameters) into readable string
HRESULT WINAPI format_id( 
	CACHED_FILE* pCachedFile, // cached file structure
	DWORD dwValue,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	EVAL_INFO* pEvalInfo  // always pass zero
) {
	CHAR szDefine[0x200];
	CHAR szName[0x200];
	CHAR szValue[0x40];
	CHAR szPointedValue[0x40];
	DWORD dwAddress = dwValue;
	DWORD dwErr;

	szValue[0] = 0;
	szPointedValue[0] = 0;

	switch (nBase)
	{
	case 16:
		wsprintf(szValue, "0x%08x", dwValue);
		break;
	case 10:
		wsprintf(szValue, "%d", dwValue);
		break;
	default:
		break;
	}
	
	__try {
		
		if (!pEvalInfo) // EECXX.DLL must be patched
		{
			_snprintf(pResult, max, "%s, (EECXX.DLL is incompatible)", szValue);
			return S_OK;
		}
		
		if (!RefreshCachedFile(pCachedFile))
		{
			_snprintf(pResult, max, "%s, unresolved identifier (file not found: %s)", szValue, pCachedFile->szFileName);
			return S_OK;
		}
		
		dwAddress = pEvalInfo->dwAddress;

		if (dwAddress == 0)
		{
			if (pEvalInfo->dwRegisterIdx!=0)
				dwValue = pEvalInfo->dwValue[0];
			else
			if (pEvalInfo->dwUnknown0 == 3) // const?
				dwValue = pEvalInfo->dwValue[0];
		}
		else
		{
		
			if ( FAILED( pHelper->ReadDebuggeeMemory( pHelper, dwAddress, sizeof(dwValue), &dwValue, &dwErr ) ) )
			{
				_snprintf(pResult, max, "%s, cannot retrive value", szValue);
				return S_OK;
			}
			
			if ( dwValue == 0xcccccccc )
			{
				_snprintf(pResult, max, "%s, not initialized", szValue);
				return S_OK;
			}
			
			if (pEvalInfo->SymType.ptr == ePTR_NearPtr32)
			{
				dwAddress = dwValue;
				
				if ( FAILED( pHelper->ReadDebuggeeMemory( pHelper, dwAddress, sizeof(dwValue), &dwValue, &dwErr ) ) )
				{
					_snprintf(pResult, max, "%s, cannot retrive value", szValue);
					return S_OK;
				}
				
				switch (nBase)
				{
				case 16:
					wsprintf(szPointedValue, "0x%08x", dwValue);
					break;
				case 10:
					wsprintf(szPointedValue, "%d", dwValue);
					break;
				default:
					break;
				}

			}
		}

		if ( dwValue == 0xcccccccc )
		{
			_snprintf(pResult, max, "*%s = %s, not initialized", szValue, szPointedValue);
			return S_OK;
		}
		
		if (!GetDefinitionByValue(pCachedFile, dwValue, szDefine) || !RetriveDefineName(szDefine, szName))
		{
			_snprintf(pResult, max, "%s, unresolved identifier", szValue);
			return S_OK;
		}

		if (!*szValue)
			_snprintf(pResult, max, "%s", szName);
		else
		if (!*szPointedValue)
			_snprintf(pResult, max, "%s (%s)", szName, szValue);
		else
			_snprintf(pResult, max, "%s (*%s = %s)", szName, szValue, szPointedValue);
			
		return S_OK;
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpyn( pResult, "PrIDs: cannot resolve, exception occured", max );
		return S_OK;
	}


	return S_OK;
}

#define format_func(type) \
HRESULT WINAPI format_##type(DWORD dwValue, DEBUGHELPER *pHelper, int nBase, BOOL bIgnore, char *pResult, size_t max, EVAL_INFO* pEvalInfo) \
{ \
	static CACHED_FILE sCachedFile = { DEF_HEADERS_PATH "pr_d_" #type ".h", {0,0,0,0,0,0}, NULL	}; \
	return format_id(&sCachedFile, dwValue, pHelper, nBase, bIgnore, pResult, max, pEvalInfo); \
} \

format_func(err)
format_func(iid)
format_func(pg)
format_func(pid)
format_func(pm)
format_func(pmc)
format_func(vid)

#undef format_func

#ifdef __cplusplus
} // extern "C"
#endif

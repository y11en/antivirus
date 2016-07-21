#define _HOOKBASE_IMP_

#include <prague.h>
#include "hookbase.h"
#include "memmgr.h"
#include "debug.h"

hROOT g_root = NULL;

#pragma pack(1)
typedef struct HOOK_CODE {
	BYTE	opcodeCALL; // 0xE8
	DWORD	ThunkRelativeAddr; 
	BYTE	opcodeJMP;  // 0xE9
	DWORD	HookRelativeAddr;  
} HOOK_CODE_;

typedef struct _HOOK {
	HOOK_CODE_ Code;
	struct _HOOK*	pPrevHook;
	DWORD	OrigFuncAddr;
	DWORD	HookFuncAddr;
	DWORD	PatchedAddr;
	DWORD	hModule;
	HOOKINFO UserData;
} HOOK, *PHOOK;
#pragma pack()

PHOOK     g_pLastHook = NULL;
PMM_CTX*  g_pPMMCtx;

BOOL HookBaseInit()
{
	return PMMInit(PAGE_EXECUTE_READWRITE, &g_pPMMCtx);
}

VOID HookBaseDone()
{
	UnHookAll();
	//PMMDone(g_pPMMCtx); !!! don't free memory - overwise hook thunks become unavailable and dynamically resolved hooked exports will crash!
	g_pPMMCtx = NULL;
}

DWORD PatchDWORD(DWORD* lpAddress, DWORD dwNewValue)
{
	DWORD dwOldValue=0;
	DWORD flOldProtect;

	if (VirtualProtect(lpAddress, sizeof(DWORD), PAGE_READWRITE, &flOldProtect))
	{
		dwOldValue = *lpAddress;
		*lpAddress = dwNewValue;
		VirtualProtect(lpAddress, sizeof(DWORD), flOldProtect, &flOldProtect);
		FlushInstructionCache(GetCurrentProcess(), lpAddress, sizeof(DWORD));
	}
	return dwOldValue;
}

void __declspec(naked) Thunk(void)
{
	__asm{
//		nop
//		int 	3
		mov 	dword ptr [esp-0x10], eax // save eax
		mov 	dword ptr [esp-0x0C], ebx // save ebx
		mov 	eax, dword ptr [esp]      // get ret address = thunk address
		sub 	eax, offset (HOOK.Code.opcodeJMP)
		mov		ebx, eax
		add 	eax, offset (HOOK.UserData)
		pop 	dword ptr [esp-0x0c]      
		mov		[esp-0x20], eax           // put UserData
		add		eax, offset (HOOKINFO.m_dwFlags)
		mov		eax, [eax]                // get m_dwFlags
		and		eax, HI_JMP_HOOK          // test HI_JMP_HOOK flag
		jnz		_jmp_hook
		pop 	dword ptr [esp-0x0c]
		mov		eax, [esp-0x24]           // get UserData
		mov 	dword ptr [esp-4], eax    // push UserData
		mov 	eax, ebx
		add 	eax, offset (HOOK.OrigFuncAddr)
		add 	ebx, offset (HOOK.hModule)
		mov		eax, [eax]
		add		eax, [ebx]
		mov 	dword ptr [esp-8], eax    // push OrigFuncAddr
		mov 	ebx, dword ptr [esp-0x14] // restore ebx
		mov 	eax, dword ptr [esp-0x18] // restore eax
		sub 	esp, 0x10

		retn

_jmp_hook:
		mov		eax, [esp-0x20]           // get UserData
		mov 	dword ptr [esp-4], eax    // push UserData
		mov 	eax, ebx
		add 	eax, offset (HOOK.OrigFuncAddr)
		add 	ebx, offset (HOOK.hModule)
		mov		eax, [eax]
		add		eax, [ebx]
		mov 	dword ptr [esp-8], eax    // push OrigFuncAddr
		mov 	ebx, dword ptr [esp-0x10] // restore ebx
		mov 	eax, dword ptr [esp-0x14] // restore eax
		sub 	esp, 0x0c

		retn
	}
}


PHOOK CreateRTHook(LPVOID pFuncAddress, LPVOID pHookFunc)
{
	PHOOK pHook;
	
	//PR_TRACE((g_root, prtNOTIFY, "HookObjectMethod..."));
	// Calculate patching address
	if (pFuncAddress == NULL)
	{
		PR_TRACE((g_root, prtNOTIFY, "HookBase:CreateRTHook Error: pFuncAddress = NULL"));
		return NULL;
	}

	pHook = (PHOOK)HeapAlloc(GetProcessHeap(), 0, sizeof(HOOK)); 
	if (pHook == NULL)
	{
		PR_TRACE((g_root, prtNOTIFY, "HookBase:CreateRTHook Error allocating heap memory"));
		return NULL;
	}
		
//	PR_TRACE((g_root, prtNOTIFY, "HookBase:CreateRTHook Patching address %08X with value %08X (hModule=%08X)", lpAddress, &(pHook->Code.opcodeCALL) - (DWORD)hModule, hModule)); 
	ZeroMemory(pHook, sizeof(HOOK)); // zero memory
	pHook->pPrevHook = g_pLastHook; 
	g_pLastHook = pHook;

	// call Thunk
	pHook->Code.opcodeCALL = 0xE8; 
	pHook->Code.ThunkRelativeAddr = (DWORD)Thunk - ((DWORD)&pHook->Code.opcodeJMP); 
	// jmp HookFunc
	pHook->Code.opcodeJMP  = 0xE9; 
	pHook->Code.HookRelativeAddr = (DWORD)pHookFunc - (DWORD)&pHook->Code.HookRelativeAddr - (DWORD)sizeof(pHook->Code.HookRelativeAddr); 
	// Store values
//	pHook->hModule		 = hModule;
	pHook->HookFuncAddr = (DWORD)pHookFunc;
	pHook->OrigFuncAddr = (DWORD)pFuncAddress; //*lpAddress;
	pHook->PatchedAddr  = 0; //(DWORD)lpAddress;
	//*pRTHook = (LPVOID)((DWORD)&(pHook->Code.opcodeCALL)); // - (DWORD)hModule; 
	return pHook;
}

PHOOKINFO HookAddress(LPVOID lpAddress, LPVOID HookFunc, HMODULE hModule)
{
	DWORD flOldProtect;
	PHOOK pHook;
	
	// Change memory protection to get writing rights.
	if (VirtualProtect(lpAddress, sizeof(DWORD), PAGE_READWRITE, &flOldProtect))
	{
		pHook = (PHOOK)PMMAlloc(g_pPMMCtx, sizeof(HOOK)); 
		if (pHook == NULL)
		{
			DBG_STOP;
			VirtualProtect(lpAddress, sizeof(DWORD), flOldProtect, &flOldProtect);
			PR_TRACE((g_root, prtNOTIFY, "HookBase:HookObjectMethod Error allocating heap memory"));
			return NULL;
		}
		
		PR_TRACE((g_root, prtNOTIFY, "HookBase:HookObjectMethod Patching address %08X with value %08X (hModule=%08X)", lpAddress, &(pHook->Code.opcodeCALL) - (DWORD)hModule, hModule)); 
		ZeroMemory(pHook, sizeof(HOOK)); // zero memory
		pHook->pPrevHook = g_pLastHook; 
		g_pLastHook = pHook;

		// call Thunk
		pHook->Code.opcodeCALL = 0xE8; 
		pHook->Code.ThunkRelativeAddr = (DWORD)Thunk - ((DWORD)&pHook->Code.opcodeJMP); 
		// jmp HookFunc
		pHook->Code.opcodeJMP  = 0xE9; 
		pHook->Code.HookRelativeAddr = (DWORD)HookFunc - (DWORD)&pHook->Code.HookRelativeAddr - (DWORD)sizeof(pHook->Code.HookRelativeAddr); 
		// Store values
		pHook->hModule		 = (DWORD)hModule;
		pHook->HookFuncAddr = (DWORD)HookFunc;
		pHook->OrigFuncAddr = *(DWORD*)lpAddress;
		pHook->PatchedAddr  = (DWORD)lpAddress;
		// Patching (patched address pointing to pHook->Code.opcodeCALL.
		*(DWORD*)lpAddress = (DWORD)&(pHook->Code.opcodeCALL) - (DWORD)hModule; 
		// Restore protection
		VirtualProtect(lpAddress, sizeof(DWORD), flOldProtect, &flOldProtect);
	//	PR_TRACE((g_root, prtNOTIFY, "HookBase: Hooking object method... OK!"));
		return &pHook->UserData;
	}
	PR_TRACE((g_root, prtNOTIFY, "HookObjectMethod: Fail to change memory protection! err=%08X", GetLastError()));
	return NULL;
}

PHOOKINFO HookObjectMethod(LPVOID lpObject, DWORD Method, LPVOID HookFunc)
{
	DWORD* lpVtbl;
	DWORD* lpMethod;
	
	//PR_TRACE((g_root, prtNOTIFY, "HookObjectMethod..."));
	// Calculate patching address
	if (lpObject == NULL)
	{
		PR_TRACE((g_root, prtNOTIFY, "HookBase:HookObjectMethod Error: lpObject = NULL"));
		return NULL;
	}
	lpVtbl = (*(DWORD**)lpObject) ;
	if (lpVtbl == NULL)
	{
		PR_TRACE((g_root, prtNOTIFY, "HookBase:HookObjectMethod Error: VtblAddress = NULL"));
		return NULL;
	}
	lpMethod = lpVtbl + Method;
	return HookAddress(lpMethod, HookFunc, NULL);
}


PHOOKINFO HookObjectMethodIfNotHookedBy(LPVOID lpObject, DWORD Method, LPVOID HookFunc)
{
	if (!IsMethodHookedBy(lpObject,Method,HookFunc) )
		return HookObjectMethod(lpObject,Method,HookFunc);
	return NULL;
}



BOOL UnHook(LPVOID HookFunc)
{
	DWORD flOldProtect;
	BOOL  bSuccess = TRUE;
	PHOOK pHook;
	PHOOK pHookTmp;
	PHOOK pPrevHook;

	pHook = g_pLastHook;
	pPrevHook = NULL;

	while (pHook != NULL)
	{
		if (pHook->PatchedAddr && (pHook->HookFuncAddr == (DWORD)HookFunc || ((pHook->UserData.m_dwFlags & HI_JMP_HOOK) && pHook->UserData.m_dwFwdFuncAddr == (DWORD)HookFunc)))
		{
			if (VirtualProtect((void*)pHook->PatchedAddr, sizeof(DWORD), PAGE_READWRITE, &flOldProtect))
			{
				if (*(DWORD*)pHook->PatchedAddr + (DWORD)pHook->hModule != (DWORD)pHook)
				{
					// Other hook has been installed for this address
					BOOL bFound = FALSE;
					PHOOK pHook2 = g_pLastHook;
					while (pHook2 != NULL)
					{
						if (pHook2->OrigFuncAddr == (DWORD)pHook)
						{
							pHook2->OrigFuncAddr = pHook->OrigFuncAddr;
							bFound = TRUE;
						}
						pHook2 = pHook2->pPrevHook;
					}
					if (!bFound)
						DBG_STOP;
				}
				else
				{
					// It is only hook for this address - restore original value
					*(DWORD*)pHook->PatchedAddr = pHook->OrigFuncAddr;
				}
				VirtualProtect((void*)pHook->PatchedAddr, sizeof(DWORD), flOldProtect, &flOldProtect);
			}
			else
			{
				PR_TRACE((g_root, prtNOTIFY, "VirtualProtect failed for addr=%08X with err=%08X, Module already freed?", pHook->PatchedAddr, GetLastError()));
			}

			if (pHook->UserData.m_dwFlags & HI_SZDATA_ALLOCATED)
				HeapFree(GetProcessHeap(), 0, (LPVOID)pHook->UserData.m_dwFuncDescr);
			if (pPrevHook)
				pPrevHook->pPrevHook = pHook->pPrevHook;
			if (pHook == g_pLastHook)
				g_pLastHook = pHook->pPrevHook;
			pHookTmp = pHook;
			pHook = pHook->pPrevHook;
			if (pHookTmp->UserData.m_dwFlags | HI_DONT_FREE_ON_RELEASE)
			{
				// it was export/import hook. This memory cannot be freed!
				BYTE* pPatchStart = (BYTE*)pHookTmp;
				BYTE* pPatchEnd = pPatchStart + 5;//(BYTE*)&(pHookTmp->OrigFuncAddr); 
				//DWORD dwSize = pPatchEnd - pPatchStart - 1;
				//memset(pPatchStart, 0x90, dwSize);
				pPatchStart[0] = 0xE9;
				//pPatchStart[dwSize] = 0xE9;
				*(DWORD*)(pPatchStart+1) = (pHookTmp->OrigFuncAddr + pHookTmp->hModule) - (DWORD)pPatchEnd;
				//pHookTmp->OrigFuncAddr -= (DWORD)pPatchEnd + 4;
			}
			else
				HeapFree(GetProcessHeap(), 0, (LPVOID)pHookTmp); // free memory
			FlushInstructionCache(GetCurrentProcess(), (LPVOID)pHookTmp, sizeof(HOOK));
		}
		else
		{
			pPrevHook = pHook;
			pHook = pHook->pPrevHook;
		}
	}
	return bSuccess;
}

BOOL UnHookAll()
{
	DWORD PrevUnhookedFunc = 0;

	PR_TRACE((g_root, prtNOTIFY, "HookBase: UnHookAll..."));
	while (g_pLastHook != NULL)
	{
//		if (g_pLastHook->HookFuncAddr != PrevUnhookedFunc)
//		{
//			PrevUnhookedFunc = g_pLastHook->HookFuncAddr;
			UnHook((void*)g_pLastHook->HookFuncAddr);
//		}
//		else
//			return FALSE;
	}
	PR_TRACE((g_root, prtNOTIFY, "HookBase: Completed."));
	return TRUE;
}

BOOL IsHooked(LPVOID HookFunc)
{
	PHOOK pHook;
	
	pHook = g_pLastHook;
//	PR_TRACE((g_root, prtNOTIFY, "HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, HookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->HookFuncAddr == (DWORD)HookFunc || ((pHook->UserData.m_dwFlags & HI_JMP_HOOK) && pHook->UserData.m_dwFwdFuncAddr == (DWORD)HookFunc))
		{
			//PR_TRACE((g_root, prtNOTIFY, "HookBase:IsHooked '%s' is already hooked", HookFuncName));
			return TRUE;
		}
		pHook = pHook->pPrevHook;
	}
	//PR_TRACE((g_root, prtNOTIFY, "HookBase:IsHooked '%s' not hooked yet", HookFuncName));
	return FALSE;
} 

#ifdef _DEBUG
#define GetHookByOrigFunc(OrigFuncAddr)	Dbg_GetHookByOrigFunc(#OrigFuncAddr, OrigFuncAddr)
PHOOK Dbg_GetHookByOrigFunc(LPSTR OrigFuncAddrName, LPVOID OrigFuncAddr){
//PR_TRACE((g_root, prtNOTIFY, "HookBase:IsHooked(%s)", HookFuncName));
#else
PHOOK GetHookByOrigFunc(LPVOID OrigFuncAddr){
#endif
	PHOOK pHook;
	
	pHook = g_pLastHook;
	while (pHook != NULL)
	{
		if (pHook->OrigFuncAddr == (DWORD)OrigFuncAddr)
		{
//			PR_TRACE((g_root, prtNOTIFY, "HookBase:GetHookByOrigFunc '%s' found", OrigFuncAddrName));
			return pHook;
		}
		pHook = pHook->pPrevHook;
	}
	PR_TRACE((g_root, prtNOTIFY, "HookBase:GetHookByOrigFunc failed"));
	return NULL;
} 

#ifdef _DEBUG
#define IsAddressHooked(HookAddr)	Dbg_IsAddressHooked(#HookAddr, HookAddr)
BOOL Dbg_IsAddressHooked(LPSTR HookAddrName, LPVOID HookAddr){
//PR_TRACE((g_root, prtNOTIFY, "HookBase:IsHooked(%s)", HookFuncName));
#else
BOOL IsAddressHooked(LPVOID HookAddr){
#endif
	PHOOK pHook;
	
	pHook = g_pLastHook;
//	PR_TRACE((g_root, prtNOTIFY, "HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, HookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->PatchedAddr == (DWORD)HookAddr)
		{
//			PR_TRACE((g_root, prtNOTIFY, "HookBase:IsAddressHooked '%s' is already hooked", HookAddrName));
			return TRUE;
		}
		pHook = pHook->pPrevHook;
	}
//	PR_TRACE((g_root, prtNOTIFY, "HookBase:IsAddressHooked '%s' not hooked yet", HookAddrName));
	return FALSE;
} 

BOOL IsMethodHookedBy(LPVOID HookObj, DWORD MethodNum, LPVOID HookFunc)
{
	return IsAddressHookedBy(((*(DWORD**)HookObj)+MethodNum), HookFunc);
} 

BOOL IsAddressHookedBy(LPVOID HookAddr, LPVOID HookFunc){
	PHOOK pHook;
	
	pHook = g_pLastHook;
//	PR_TRACE((g_root, prtNOTIFY, "HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, HookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->PatchedAddr == (DWORD)HookAddr && pHook->HookFuncAddr == (DWORD)HookFunc)
		{
			//PR_TRACE((g_root, prtNOTIFY, "HookBase:IsAddressHookedBy '%s' is already hooked", HookAddrName));
			return TRUE;
		}
		pHook = pHook->pPrevHook;
	}
	//PR_TRACE((g_root, prtNOTIFY, "HookBase:IsAddressHookedBy '%s' not hooked yet", HookAddrName));
	return FALSE;
} 

#ifdef _DEBUG
/*
typedef struct _mGUID {
	DWORD dwData1;
	WORD  wData2;
	WORD  wData3;
	BYTE  wData4[8];
} mGUID, *pmGUID;

typedef struct _QPARAMS {
	DWORD* _this;
	pmGUID riid;
	DWORD** pInterface;
} QPARAMS;

void __cdecl PrintQuery(PHOOKINFO pHookInfo, QPARAMS *pParams)
{
	if (((DWORD)pParams->_this > 0x1000) && (*(pParams->_this) > 0x1000) && ((DWORD)pParams->riid > 0x1000))
	{
		PR_TRACE((g_root, prtNOTIFY, "QueryInterface from %08X for %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X)", 
			*(pParams->_this), 
			pParams->riid->dwData1, pParams->riid->wData2, pParams->riid->wData3, 
			pParams->riid->wData4[0], pParams->riid->wData4[1], pParams->riid->wData4[2], pParams->riid->wData4[3], pParams->riid->wData4[4], pParams->riid->wData4[5], pParams->riid->wData4[6], pParams->riid->wData4[7], **pInterface));
	}
//	HRESULT hRes;
//	hRes = ((HRESULT (__stdcall *)(DWORD*, pmGUID, DWORD**))OrigFuncAddr)(_this, riid, pInterface);
	if (hRes == S_OK)
	{
		if (*pInterface != NULL)
		{
			if (**pInterface != NULL)// && IsAddressHooked((void*)**pInterface) == FALSE)
			{
				PR_TRACE((g_root, prtNOTIFY, "QueryInterface from %08X for %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X returned %08X)", *_this, riid->dwData1, riid->wData2, riid->wData3, 
					riid->wData4[0], riid->wData4[1], riid->wData4[2], riid->wData4[3], riid->wData4[4], riid->wData4[5], riid->wData4[6], riid->wData4[7], 
					**pInterface));
				//HookObjectMethod((DWORD)*pInterface, 0, HookQuery);
			}
		}
	}

	return; //hRes;
}

__declspec(naked) void HookQuery(void* OrigFuncAddr, PHOOKINFO pHookInfo, char* param1)
{
	__asm {
		//int 3;
		push	ebp
		mov		ebp, esp
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi
		lea		eax, [ebp+0x10]
		push	eax
		push	[ebp+0x0c]
		call	PrintQuery
		pop		edi
		pop		edi
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		pop		ebp
		mov 	eax, [esp+4]
		pop 	dword ptr [esp]
		pop 	dword ptr [esp]
		push	eax
		mov 	eax, [esp-0xC]
		ret
	};
}

*/
void HookInterface(void* HookFunc, DWORD Interface, DWORD numMethods, char* InterfaceName, ...)
{
	DWORD i;
	char* cNumber[100];
	char* UserData;
	char* MethodName = NULL;
	PHOOKINFO pHookInfo;

	va_list marker;
	va_start(marker, InterfaceName);     /* Initialize variable arguments. */

	for (i=0; i<numMethods; i++)
	{
	    if ((DWORD)MethodName != -1)
			MethodName = va_arg(marker, char*);
		if (i>0 && i<3 && ((DWORD)MethodName == -1 || MethodName == NULL)) 
			continue;
		UserData = (char*)HeapAlloc(GetProcessHeap(), 0, 0x100);
		wsprintf(UserData, "-> %s::", InterfaceName);
		if ((DWORD)MethodName != -1 && MethodName != NULL) 
			lstrcat(UserData, MethodName);
		else
		{
			switch(i)
			{
			case 0:	lstrcat(UserData, "IUnknown::QueryInterface"); break;
			case 1:	lstrcat(UserData, "IUnknown::AddRef"); break;
			case 2:	lstrcat(UserData, "IUnknown::Release"); break;
			default:
				if (numMethods>7) 
				{
//					switch(i)
					{
/*					case 3:	lstrcat(UserData, "IDispatch::GetTypeInfoCount"); break;
					case 4:	lstrcat(UserData, "IDispatch::GetTypeInfo"); break;
					case 5:	lstrcat(UserData, "IDispatch::GetIDsOfNames"); break;
					case 6:	lstrcat(UserData, "IDispatch::Invoke"); break;
*/
//					default: 
						wsprintf((char*)cNumber, "%d", i);
						lstrcat(UserData, (char*)cNumber);
					}
				}
				else
				{
					wsprintf((char*)cNumber, "%d", i);
					lstrcat(UserData, (char*)cNumber);
				}
			}
		}
/*		if (i == 0)
			pHookInfo = HookObjectMethod((DWORD)&Interface, i, HookQuery);
		else*/
			pHookInfo = HookObjectMethod(&Interface, i, HookFunc);
		if (pHookInfo) 
		{
			pHookInfo->m_dwFuncDescr = (DWORD)UserData;
			pHookInfo->m_dwFlags |= (HI_SZDATA | HI_SZDATA_ALLOCATED);
		}
	}
	va_end( marker ); /* Reset variable arguments.      */
	return;
}

void HookJmpTable(void* HookFunc, LPVOID pJmpTableAddress, DWORD TableLen, char* TableName, ...)
{
	DWORD i;
	char* cNumber[100];
	char* UserData;
	char* MethodName = NULL;
	PHOOKINFO pHookInfo;

	va_list marker;
	va_start(marker, TableName);     /* Initialize variable arguments. */

	for (i=0; i<TableLen; i++)
	{
	    if ((DWORD)MethodName != -1)
			MethodName = va_arg(marker, char*);
		UserData = (char*)HeapAlloc(GetProcessHeap(), 0, 0x100);
		wsprintf(UserData, "-> %s::", TableName);
		wsprintf((char*)cNumber, "%03X ", i);
		lstrcat(UserData, (char*)cNumber);
		if ((DWORD)MethodName != -1 && MethodName != NULL) 
			lstrcat(UserData, MethodName);
		pHookInfo = HookObjectMethod(&pJmpTableAddress, i, HookFunc);
		if (pHookInfo) 
		{
			pHookInfo->m_dwFuncDescr = (DWORD)UserData;
			pHookInfo->m_dwFlags |= (HI_SZDATA | HI_SZDATA_ALLOCATED | HI_DWDATARULEID);
			pHookInfo->m_dwRuleID = i;
		}
	}
	va_end( marker ); /* Reset variable arguments.      */
	return;
}

#endif

void _declspec(naked) JmpTableHook (void)
{
	__asm {
		push	ebp
//		mov		ebp, esp
		push	edi
		push	esi
		push	edx
		push	ecx
		push	ebx
		push	eax
//		lea		eax, [esp+0x28]
//		push	eax
		mov		eax, [esp+0x20] // pHookInfo
//		push	[esp+0x24]
//		push	[esp+0x28]
		add		eax, offset HOOKINFO.m_dwFwdFuncAddr
		cmp		[eax], 0
		jz		l_skip
		call	[eax]
l_skip:
//		pop		eax
//		pop		eax
		pop		eax
		pop		edx
		pop		ecx
		pop		ebx
		pop		esi
		mov		edi, [esp+0x8]
		mov		[esp+0x0c], edi
		pop		edi
		pop ebp
		add esp, 4 
		ret 
	}

}

PHOOKINFO HookJmpTableAddress(LPVOID pTableAddress, DWORD dwOffsetInTable, LPVOID HookFunc)
{
	PHOOKINFO pHookInfo;
	pHookInfo = HookObjectMethod((LPVOID)&pTableAddress, dwOffsetInTable, JmpTableHook);
	pHookInfo->m_dwFwdFuncAddr = (DWORD)HookFunc;
	pHookInfo->m_dwFlags |= HI_JMP_HOOK;
	return pHookInfo;
}

#define _HOOKBASE_IMP_

#include "hookbase.h"

// =========================================================================================================
//  DEBUG
// =========================================================================================================

static void OutputDbgString(char* szFormat, ...)
{
	char szDbgStr[4096];
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	wvsprintf(szDbgStr, szFormat, marker);
	va_end(marker);  
	lstrcat(szDbgStr, "\n"); 
	OutputDebugString(szDbgStr);
	//Sleep(1);
}

static void OutputDbgString2(char* szFormat, ...)
{
	char szDbgStr[4096];
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	wvsprintf(szDbgStr, szFormat, marker);
	va_end(marker);  
	OutputDebugString(szDbgStr);
	//Sleep(1);
}

#define ODS OUTPUTDEBUGSTRING
#define ODSN OUTPUTDEBUGSTRINGN

#if defined(_DEBUG) || defined(ENABLE_ODS)

#define OUTPUTDEBUGSTRING(x) (OutputDbgString x)
#define OUTPUTDEBUGSTRINGN(x) (OutputDbgString2 x)
#define DBG_STOP {__asm int 3}
#define __try
#define __except(x) if(0)

#else

#pragma message ("debug output disabled")
#define OUTPUTDEBUGSTRING(x) 
#define OUTPUTDEBUGSTRINGN(x) 
#define DBG_STOP {}


#endif	

//#define TRACE_HOOK_CRITIAL_SECTION
#if defined(_DEBUG) && defined(TRACE_HOOK_CRITIAL_SECTION)
static int g_cs_counter = 0;
#define EnterCriticalSection(cs) {EnterCriticalSection(cs); g_cs_counter++; ODS(("%s%d: CS: %d->%d", __FILE__LINE__, GetCurrentThreadId(), g_cs_counter-1, g_cs_counter));}
#define LeaveCriticalSection(cs) {LeaveCriticalSection(cs); g_cs_counter--; ODS(("%s%d: CS: %d->%d", __FILE__LINE__, GetCurrentThreadId(), g_cs_counter+1, g_cs_counter));}
#endif

// =========================================================================================================
//  MEMORY MANAGER 
// =========================================================================================================

typedef unsigned int size_t;
typedef unsigned short alignment_type;

typedef struct tag_PageControlBlock PageControlBlock;

typedef struct tag_PageControlBlock{
	unsigned int thePagesCount;
	PageControlBlock* theNextPCB;
	alignment_type* theFirstUnusedBlock;
	unsigned int theNumberOfFreeBlocks;
}PageControlBlock;

typedef struct tag_PMM_CTX {
	DWORD flProtect;
	PageControlBlock* thePCB;
	unsigned int thePageSize;
} PMM_CTX;

// ---------------------------------------------------------------------------------------------------------

BOOL PMMInit(DWORD flProtect, PMM_CTX** ppPMMCtx)
{
    SYSTEM_INFO aSystemInfo;
	PMM_CTX* pPMMCtx;

	if (!ppPMMCtx)
		return FALSE;
	pPMMCtx = (PMM_CTX*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PMM_CTX));
	if (!pPMMCtx)
		return FALSE;
    GetSystemInfo( &aSystemInfo );
    pPMMCtx->thePageSize = aSystemInfo.dwPageSize;
	pPMMCtx->flProtect = flProtect;
	*ppPMMCtx = pPMMCtx;
	return TRUE;
}


void PMMDone(PMM_CTX* pPMMCtx)
{
    PageControlBlock* aPCB = pPMMCtx->thePCB;
    while ( aPCB ) {
		PageControlBlock* aTmpPCB = aPCB->theNextPCB;
		VirtualFree ( aPCB, 0, MEM_RELEASE );
		aPCB = aTmpPCB;
    }
}

void* PMMAlloc(PMM_CTX* pPMMCtx, size_t aSize)
{
    PageControlBlock* aPCB = pPMMCtx->thePCB;
    PageControlBlock* aPrevPCB = aPCB;
	unsigned int thePageSize = pPMMCtx->thePageSize;
	void* aResult;
	alignment_type aNumberOfNeededBlocks;
	
    // trying to find a memory block with needed size
    while ( aPCB ) {
		if ( aPCB->theNumberOfFreeBlocks * sizeof ( alignment_type ) > aSize )
			break;
		aPrevPCB = aPCB;
		aPCB = aPCB->theNextPCB;
    }
    // if a block hasn't been found, allocate new page of memory
    if ( !aPCB ) {
		unsigned int aPagesCount = ( aSize + sizeof ( PageControlBlock ) ) / thePageSize + 
			( ( ( aSize + sizeof ( PageControlBlock ) )  % thePageSize ) ? 1 : 0 );
		unsigned int aFullSize = thePageSize * aPagesCount;
		aPCB = (PageControlBlock*)( VirtualAlloc ( 0, aFullSize, MEM_RESERVE | MEM_COMMIT, pPMMCtx->flProtect ) );
		if ( !aPCB ) 
			return 0;
		if ( !pPMMCtx->thePCB )
			pPMMCtx->thePCB = aPCB;
		else
			aPrevPCB->theNextPCB = aPCB;
		aPCB->thePagesCount = aPagesCount;
		aPCB->theFirstUnusedBlock = (alignment_type*)( (unsigned char*) (aPCB) + sizeof ( PageControlBlock ) );
		aPCB->theNumberOfFreeBlocks = ( aFullSize - sizeof ( PageControlBlock ) ) / sizeof ( alignment_type );
#if defined (COMPLEX_MEMORY_MANAGER)
		// enumeration free memory in the block 
		for ( alignment_type i = 0; i < aPCB->theNumberOfFreeBlocks - 1 ; ++i )
			aPCB->theFirstUnusedBlock [ i ] = i + 1;
		aPCB->theFirstUnusedBlock [ aPCB->theNumberOfFreeBlocks - 1 ] = 0;
#endif
    }
    aResult = aPCB->theFirstUnusedBlock;
    aNumberOfNeededBlocks = aSize / sizeof ( alignment_type ) + ( ( aSize % sizeof ( alignment_type ) ) ? 1 : 0 );
    aPCB->theFirstUnusedBlock = aPCB->theFirstUnusedBlock + aNumberOfNeededBlocks;
    aPCB->theNumberOfFreeBlocks -= aNumberOfNeededBlocks;
    return aResult;
}

void PMMFree (PMM_CTX* pPMMCtx, void* mem) 
{
#if defined (COMPLEX_MEMORY_MANAGER)
	
#endif
}

BOOL PMMSetMemoryProtection(PMM_CTX* pPMMCtx, DWORD flProtect)
{
	PageControlBlock* aPCB;

    aPCB = pPMMCtx->thePCB;
    while ( aPCB ) {
		DWORD anOldProtection;
		if (!VirtualProtect ( aPCB, aPCB->thePagesCount * pPMMCtx->thePageSize, flProtect, &anOldProtection ))
			return FALSE;
		aPCB = aPCB->theNextPCB;
    }
    pPMMCtx->flProtect = flProtect ;
	return TRUE;
}

DWORD PMMGetMemoryProtection (PMM_CTX* pPMMCtx)
{
    return pPMMCtx->flProtect;
}

// =========================================================================================================
//  HOOKS IMPLEMENTATION
// =========================================================================================================

#define HI_JMP_HOOK				0x01
#define HI_DONT_FREE_ON_RELEASE	0x02

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
	DWORD   pRealHookFunc;
	DWORD   dwFlags;
	void*   pUserData;
} HOOK, *PHOOK;
#pragma pack()

static PHOOK     g_pLastHook = NULL;
static PMM_CTX*  g_pPMMCtx;

// ---------------------------------------------------------------------------------------------------------

static CRITICAL_SECTION g_hooks_critical_section;
static BOOL bHookBaseInited = FALSE;

BOOL HookBaseInit()
{
	if (!bHookBaseInited)
	{
		InitializeCriticalSection(&g_hooks_critical_section);
		bHookBaseInited = PMMInit(PAGE_EXECUTE_READWRITE, &g_pPMMCtx);
	}
	return bHookBaseInited;
}

VOID HookBaseDone()
{
	if (bHookBaseInited)
	{
		UnHookAll();
		if (g_pPMMCtx)
			PMMDone(g_pPMMCtx);
		g_pPMMCtx = NULL;
		DeleteCriticalSection(&g_hooks_critical_section);
	}
}
/*

BOOL iIsExecuteProtection(LPVOID lpAddress)
{
	MEMORY_BASIC_INFORMATION mbi;

	DWORD ret;
	if (VirtualQuery(lpAddress, &mbi, sizeof(mbi)))
	{
		switch (mbi.Protect)
		{
		case PAGE_EXECUTE:
		case PAGE_EXECUTE_READ:
		case PAGE_EXECUTE_READWRITE:
		case PAGE_EXECUTE_WRITECOPY:
			return TRUE;
		}
		return FALSE;
			
	}
	// by default- assume TRUE
	return TRUE;
}*/


DWORD PatchDWORD(DWORD* lpAddress, DWORD dwNewValue)
{
	DWORD dwOldValue=0;
	DWORD flOldProtect;

	EnterCriticalSection(&g_hooks_critical_section);
	if (VirtualProtect(lpAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &flOldProtect))
	{
		dwOldValue = *lpAddress;
		*lpAddress = dwNewValue;
		VirtualProtect(lpAddress, sizeof(DWORD), flOldProtect, &flOldProtect);
		FlushInstructionCache(GetCurrentProcess(), lpAddress, sizeof(DWORD));
	}
	LeaveCriticalSection(&g_hooks_critical_section);
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
		mov		ebx, eax                  // ebx = HOOK*
		pop 	dword ptr [esp-0x0c]      
		mov     eax, [ebx]HOOK.pUserData;
		mov		[esp-0x20], eax           // put UserData
		test    dword ptr [ebx] HOOK.dwFlags, HI_JMP_HOOK // test HI_JMP_HOOK flag
		jnz		_jmp_hook
		pop 	dword ptr [esp-0x0c]
		mov		eax, [esp-0x24]           // get UserData
		mov 	dword ptr [esp-4], eax    // push UserData
		mov		eax, [ebx]HOOK.OrigFuncAddr
		add		eax, [ebx]HOOK.hModule
		mov 	dword ptr [esp-8], eax    // push OrigFuncAddr
		mov 	ebx, dword ptr [esp-0x14] // restore ebx
		mov 	eax, dword ptr [esp-0x18] // restore eax
		sub 	esp, 0x10

		retn

_jmp_hook:
//		mov		eax, [esp-0x20]           // get UserData
//		mov 	dword ptr [esp-4], eax    // push UserData
		mov 	dword ptr [esp-4], ebx    // push UserData
		mov		eax, [ebx]HOOK.OrigFuncAddr
		add		eax, [ebx]HOOK.hModule
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
	
	//ODS(("HookObjectMethod..."))4;
	// Calculate patching address
	if (pFuncAddress == NULL)
	{
		ODS(("HookBase:CreateRTHook Error: pFuncAddress = NULL"));
		return NULL;
	}

	pHook = (PHOOK)HeapAlloc(GetProcessHeap(), 0, sizeof(HOOK)); 
	if (pHook == NULL)
	{
		ODS(("HookBase:CreateRTHook Error allocating heap memory"));
		return NULL;
	}
		
//	ODS(("HookBase:CreateRTHook Patching address %08X with value %08X (hModule=%08X)", lpAddress, &(pHook->Code.opcodeCALL) - (DWORD)hModule, hModule)); 
	ZeroMemory(pHook, sizeof(HOOK)); // zero memory

	EnterCriticalSection(&g_hooks_critical_section);
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
	LeaveCriticalSection(&g_hooks_critical_section);
	
	return pHook;
}

PHOOK iHookAddress(LPVOID lpAddress, HMODULE hModule, LPVOID HookFunc, tHOOKUSERDATA* pUserData)
{
	DWORD flOldProtect;
	PHOOK pHook;
	
	EnterCriticalSection(&g_hooks_critical_section);

	// Change memory protection to get writing rights.
	if (!VirtualProtect(lpAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &flOldProtect))
	{
		ODS(("HookObjectMethod: Fail to change memory protection! err=%08X", GetLastError()));
		LeaveCriticalSection(&g_hooks_critical_section);
		return NULL;
	}
	pHook = (PHOOK)PMMAlloc(g_pPMMCtx, sizeof(HOOK)); 
	if (pHook == NULL)
	{
		DBG_STOP;
		VirtualProtect(lpAddress, sizeof(DWORD), flOldProtect, &flOldProtect);
		ODS(("HookBase:HookObjectMethod Error allocating heap memory"));
		LeaveCriticalSection(&g_hooks_critical_section);
		return NULL;
	}
	
	// !!! petrovitch 
	// this trace caused AV fault on 7th hook installed under DEP switched on
	//ODS(("HookBase:HookObjectMethod Patching address %08X with value %08X (hModule=%08X)", lpAddress, &(pHook->Code.opcodeCALL) - (DWORD)hModule, hModule));

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
	pHook->pUserData = pUserData;
	// Patching (patched address pointing to pHook->Code.opcodeCALL.
	*(DWORD*)lpAddress = (DWORD)&(pHook->Code.opcodeCALL) - (DWORD)hModule; 
	// Restore protection
	VirtualProtect(lpAddress, sizeof(DWORD), flOldProtect, &flOldProtect);
	LeaveCriticalSection(&g_hooks_critical_section);
//	ODS(("HookBase: Hooking object method... OK!"));
	return pHook;
}

BOOL HookAddress(LPVOID lpAddress, HMODULE hModule, LPVOID HookFunc, tHOOKUSERDATA* pUserData)
{
	if (NULL == iHookAddress(lpAddress, hModule, HookFunc, pUserData))
		return FALSE;
	return TRUE;
}

PHOOK iHookObjectMethod(LPVOID lpObject, DWORD Method, LPVOID HookFunc, tHOOKUSERDATA* pUserData)
{
	DWORD* lpVtbl;
	DWORD* lpMethod;
	
	//ODS(("HookObjectMethod..."));
	// Calculate patching address
	if (lpObject == NULL)
	{
		ODS(("HookBase:HookObjectMethod Error: lpObject = NULL"));
		return NULL;
	}
	lpVtbl = (*(DWORD**)lpObject) ;
	if (lpVtbl == NULL)
	{
		ODS(("HookBase:HookObjectMethod Error: VtblAddress = NULL"));
		return NULL;
	}
	lpMethod = lpVtbl + Method;
	return iHookAddress(lpMethod, NULL, HookFunc, pUserData);
}


BOOL HookObjectMethod(LPVOID lpObject, DWORD Method, LPVOID HookFunc, tHOOKUSERDATA* pUserData)
{
	if (NULL == iHookObjectMethod(lpObject, Method, HookFunc, pUserData))
		return FALSE;
	return TRUE;
}


BOOL HookObjectMethodIfNotHookedBy(LPVOID lpObject, DWORD Method, LPVOID HookFunc, tHOOKUSERDATA* pUserData)
{
	BOOL bResult = TRUE;
	EnterCriticalSection(&g_hooks_critical_section);
	if (!IsMethodHookedBy(lpObject,Method,HookFunc) )
		bResult = HookObjectMethod(lpObject,Method,HookFunc,pUserData);
	LeaveCriticalSection(&g_hooks_critical_section);
	return bResult;
}



BOOL UnHook(LPVOID HookFunc)
{
	DWORD flOldProtect;
	BOOL  bSuccess = TRUE;
	PHOOK pHook;
	PHOOK pHookTmp;
	PHOOK pPrevHook;

	EnterCriticalSection(&g_hooks_critical_section);

	pHook = g_pLastHook;
	pPrevHook = NULL;

	while (pHook != NULL)
	{
		if (pHook->PatchedAddr && (pHook->HookFuncAddr == (DWORD)HookFunc || ((pHook->dwFlags & HI_JMP_HOOK) && pHook->pRealHookFunc == (DWORD)HookFunc)))
		{
			if (VirtualProtect((void*)pHook->PatchedAddr, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &flOldProtect))
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
				ODS(("VirtualProtect failed for addr=%08X with err=%08X, Module already freed?", pHook->PatchedAddr, GetLastError()));
			}

//			if (pHook->dwFlags & HI_SZDATA_ALLOCATED)
//				HeapFree(GetProcessHeap(), 0, (LPVOID)pHook->dwFuncDescr);
			if (pPrevHook)
				pPrevHook->pPrevHook = pHook->pPrevHook;
			if (pHook == g_pLastHook)
				g_pLastHook = pHook->pPrevHook;
			pHookTmp = pHook;
			pHook = pHook->pPrevHook;
			if (pHookTmp->dwFlags | HI_DONT_FREE_ON_RELEASE)
			{
				// it was export/import hook. This memory cannot be freed!
				BYTE* pPatchStart = (BYTE*)pHookTmp;
				BYTE* pPatchEnd = pPatchStart + 5;//(BYTE*)&(pHookTmp->OrigFuncAddr); 
				//DWORD dwSize = pPatchEnd - pPatchStart - 1;
				//memset(pPatchStart, 0x90, dwSize);
				pPatchStart[0] = 0xE9;
				//pPatchStart[dwSize] = 0xE9;
				*(DWORD*)(pPatchStart+1) = pHookTmp->OrigFuncAddr -= (DWORD)pPatchEnd;
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
	LeaveCriticalSection(&g_hooks_critical_section);

	return bSuccess;
}

BOOL UnHookAll()
{
	DWORD PrevUnhookedFunc = 0;

	ODS(("HookBase: UnHookAll..."));

	EnterCriticalSection(&g_hooks_critical_section);
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
	LeaveCriticalSection(&g_hooks_critical_section);
	ODS(("HookBase: Completed."));
	return TRUE;
}

BOOL IsHooked(LPVOID HookFunc)
{
	PHOOK pHook;
	BOOL bHooked = FALSE;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
//	ODS(("HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, HookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->HookFuncAddr == (DWORD)HookFunc || ((pHook->dwFlags & HI_JMP_HOOK) && pHook->pRealHookFunc == (DWORD)HookFunc))
		{
			//ODS(("HookBase:IsHooked '%s' is already hooked", HookFuncName));
			bHooked = TRUE;
			break;
		}
		pHook = pHook->pPrevHook;
	}
	LeaveCriticalSection(&g_hooks_critical_section);
	//ODS(("HookBase:IsHooked '%s' not hooked yet", HookFuncName));
	return FALSE;
} 

#if defined(_DEBUG) || defined(ENABLE_ODS)
#define GetHookByOrigFunc(OrigFuncAddr)	Dbg_GetHookByOrigFunc(#OrigFuncAddr, OrigFuncAddr)
PHOOK Dbg_GetHookByOrigFunc(LPSTR OrigFuncAddrName, LPVOID OrigFuncAddr){
//ODS(("HookBase:IsHooked(%s)", HookFuncName));
#else
PHOOK GetHookByOrigFunc(LPVOID OrigFuncAddr){
#endif
	PHOOK pHook;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
	while (pHook != NULL)
	{
		if (pHook->OrigFuncAddr == (DWORD)OrigFuncAddr)
		{
			ODS(("HookBase:GetHookByOrigFunc '%s' found", OrigFuncAddrName));
			break;
		}
		pHook = pHook->pPrevHook;
	}
	LeaveCriticalSection(&g_hooks_critical_section);
	return pHook;
} 

#if defined(_DEBUG) || defined(ENABLE_ODS)
#define IsAddressHooked(HookAddr)	Dbg_IsAddressHooked(#HookAddr, HookAddr)
BOOL Dbg_IsAddressHooked(LPSTR HookAddrName, LPVOID HookAddr){
//ODS(("HookBase:IsHooked(%s)", HookFuncName));
#else
BOOL IsAddressHooked(LPVOID HookAddr){
#endif
	PHOOK pHook;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
//	ODS(("HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, HookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->PatchedAddr == (DWORD)HookAddr)
		{
			ODS(("HookBase:IsAddressHooked '%s' is already hooked", HookAddrName));
			LeaveCriticalSection(&g_hooks_critical_section);
			return TRUE;
		}
		pHook = pHook->pPrevHook;
	}
	EnterCriticalSection(&g_hooks_critical_section);
	ODS(("HookBase:IsAddressHooked '%s' not hooked yet", HookAddrName));
	return FALSE;
} 

BOOL IsMethodHookedBy(LPVOID HookObj, DWORD MethodNum, LPVOID HookFunc)
{
	return IsAddressHookedBy(((*(DWORD**)HookObj)+MethodNum), HookFunc);
} 

BOOL IsAddressHookedBy(LPVOID HookAddr, LPVOID HookFunc){
	PHOOK pHook;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
//	ODS(("HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, HookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->PatchedAddr == (DWORD)HookAddr && pHook->HookFuncAddr == (DWORD)HookFunc)
		{
			//ODS(("HookBase:IsAddressHookedBy '%s' is already hooked", HookAddrName));
			LeaveCriticalSection(&g_hooks_critical_section);
			return TRUE;
		}
		pHook = pHook->pPrevHook;
	}
	LeaveCriticalSection(&g_hooks_critical_section);
	//ODS(("HookBase:IsAddressHookedBy '%s' not hooked yet", HookAddrName));
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
		ODS(("QueryInterface from %08X for %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X)", 
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
				ODS(("QueryInterface from %08X for %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X returned %08X)", *_this, riid->dwData1, riid->wData2, riid->wData3, 
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
/*
void HookInterface(void* HookFunc, DWORD Interface, DWORD numMethods, char* InterfaceName, ...)
{
	DWORD i;
	char* cNumber[100];
	char* UserData;
	char* MethodName = NULL;
	PHOOKINFO pHookInfo;

	va_list marker;
	va_start(marker, InterfaceName);     // Initialize variable arguments. 

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
//					case 3:	lstrcat(UserData, "IDispatch::GetTypeInfoCount"); break;
//					case 4:	lstrcat(UserData, "IDispatch::GetTypeInfo"); break;
//					case 5:	lstrcat(UserData, "IDispatch::GetIDsOfNames"); break;
//					case 6:	lstrcat(UserData, "IDispatch::Invoke"); break;

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
//		if (i == 0)
//			pHookInfo = HookObjectMethod((DWORD)&Interface, i, HookQuery);
//		else
			pHookInfo = HookObjectMethod(&Interface, i, HookFunc);
		if (pHookInfo) 
		{
		}
	}
	va_end( marker ); // Reset variable arguments.      
	return;
}
*/

#define ID_JMP_TABLE_USERDATA 'JTUD' 

typedef struct tag_JMP_TABLE_USERDATA {
	tHOOKUSERDATA Header; // standard userdata header
	DWORD m_nTableIdx;
	CHAR  m_Name[1];
} JMP_TABLE_USERDATA;

void __cdecl destructor_JMP_TABLE_USERDATA(tHOOKUSERDATA* pData)
{
	JMP_TABLE_USERDATA* pJmpTableUserData = (JMP_TABLE_USERDATA*)pData;
	if (pJmpTableUserData)
	{
		if (pJmpTableUserData->Header.ID != ID_JMP_TABLE_USERDATA)
		{
			DBG_STOP;
			return;
		}
		HeapFree(GetProcessHeap(), 0, pJmpTableUserData);
	}
}

BOOL HookJmpTable(void* HookFunc, tHOOKUSERDATA* pUserData, LPVOID pJmpTableAddress, DWORD TableLen, char* TableName, ...)
{
	DWORD i;
	char szName[0x100];
	char* MethodName = NULL;
	BOOL bResult = TRUE;

	va_list marker;
	va_start(marker, TableName);     // Initialize variable arguments.

	for (i=0; i<TableLen; i++)
	{
		JMP_TABLE_USERDATA* pJmpTableUserData;
		wsprintf(szName, "-> %s::", TableName);
		wsprintf(szName + strlen(szName), "%03X ", i);
		pJmpTableUserData = (JMP_TABLE_USERDATA*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(JMP_TABLE_USERDATA)+strlen(szName));
		if (pJmpTableUserData == NULL)
		{
			bResult = FALSE;
			break;
		}
		pJmpTableUserData->Header.ID = ID_JMP_TABLE_USERDATA;
		pJmpTableUserData->Header.destructor = destructor_JMP_TABLE_USERDATA;
		pJmpTableUserData->m_nTableIdx = i;
		strcpy(pJmpTableUserData->m_Name, szName);

		if ((DWORD)MethodName != -1)
			MethodName = va_arg(marker, char*);
		if ((DWORD)MethodName != -1 && MethodName != NULL) 
			lstrcat(szName, MethodName);

		if (!HookObjectMethod(&pJmpTableAddress, i, HookFunc, &pJmpTableUserData->Header))
		{
			bResult = FALSE;
			break;
		}
	}
	va_end( marker ); // Reset variable arguments.
	return bResult;
}
#endif

typedef struct tag_JMP_TABLE_DATA {
	tHOOKUSERDATA m_Header;
	void*         m_ForwardHookFunc;
} tJMP_TABLE_DATA;

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
		mov		eax, [esp+0x20] // pHook
		cmp		eax, 0
		jz		l_skip
		mov		ebx, [eax]HOOK.pUserData
		mov     [esp+0x20], ebx
		mov		eax, [eax]HOOK.pRealHookFunc
		cmp		eax, 0
		jz		l_skip
		call	eax
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

BOOL HookJmpTableAddress(LPVOID pTableAddress, DWORD dwOffsetInTable, LPVOID HookFunc, tHOOKUSERDATA* pUserData)
{
	PHOOK pHook;
	pHook = iHookObjectMethod((LPVOID)&pTableAddress, dwOffsetInTable, JmpTableHook, pUserData);
	if (NULL == pHook)
		return FALSE;
	pHook->pRealHookFunc = (DWORD)HookFunc;
	pHook->dwFlags |= HI_JMP_HOOK;
	return TRUE;
}

// =========================================================================================================
//  HOOK EXPORTS/IMPORTS
// =========================================================================================================

#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPHEAPLIST 0x00000001
#define TH32CS_SNAPPROCESS  0x00000002
#define TH32CS_SNAPTHREAD   0x00000004
#define TH32CS_SNAPMODULE   0x00000008
#define TH32CS_SNAPALL      (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
#define TH32CS_INHERIT      0x80000000

HMODULE g_hHookAwareModule = NULL;

typedef struct tagMODULEENTRY32
{
	DWORD	dwSize;
    DWORD	th32ModuleID;       // This module
    DWORD   th32ProcessID;      // owning process
    DWORD   GlblcntUsage;       // Global usage count on the module
    DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
    BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
    DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
    HMODULE hModule;            // The hModule of this module in th32ProcessID's context
    char    szModule[MAX_MODULE_NAME32 + 1];
    char    szExePath[MAX_PATH];
} MODULEENTRY32;
typedef MODULEENTRY32 *  PMODULEENTRY32;
typedef MODULEENTRY32 *  LPMODULEENTRY32;

HANDLE (WINAPI *pCreateToolhelp32Snapshot)(DWORD dwFlags,DWORD th32ProcessID);
BOOL (WINAPI *pModule32First)(HANDLE hSnapshot,LPMODULEENTRY32 lpme);
BOOL (WINAPI *pModule32Next)(HANDLE hSnapshot,LPMODULEENTRY32 lpme);

DWORD (FAR WINAPI *ZwQuerySystemInformation)(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL);
DWORD (FAR WINAPI *ZwQueryInformationProcess)(IN HANDLE ProcessHandle,IN ULONG ProcessInformationClass,OUT PVOID ProcessInformation,IN ULONG ProcessInformationLength,OUT PULONG ReturnLength OPTIONAL);

typedef struct _PROC_INFO {
	DWORD U1;
	DWORD UPEB;
	CHAR	U2[0x10];
}PROC_INFO,*PPROC_INFO;

// --------------------------------------------------------------------------------------------------------

static DWORD MyExceptionFilter(EXCEPTION_POINTERS* pExcPtrs) 
{
	static UINT i;
	ODS(("ExceptionAddress: %08X, ExceptionCode=%08X",
		pExcPtrs->ExceptionRecord->ExceptionAddress, 
		pExcPtrs->ExceptionRecord->ExceptionCode));
	for (i=0; i<pExcPtrs->ExceptionRecord->NumberParameters; i++)
		ODS(("param%d:\t%08X", i, pExcPtrs->ExceptionRecord->ExceptionInformation[i]));
	return EXCEPTION_EXECUTE_HANDLER;
}

// --------------------------------------------------------------------------------------------------------
BOOL HookImportInModule(HMODULE hModule, CHAR* szFuncName, void* HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags)
{
	tHOOK_TBL_ENTRY HookTable[] = {
		{ szFuncName, HookProc, pHookUserData },
		LAST_HOOK_TBL_ENTRY
	};
	return HookImportInModuleTbl(hModule, &HookTable[0], dwFlags);
}

BOOL HookImportInModuleTbl(HMODULE hModule, tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	
	PIMAGE_IMPORT_BY_NAME pOrdinalName;
	PIMAGE_IMPORT_DESCRIPTOR importDesc;
	PIMAGE_THUNK_DATA thunk, thunkIAT=0;
	DWORD importsStartRVA;
	DWORD importsEndRVA;
	INT delta = -1;
	BOOL bTargetFunc;
	static char szModuleName[MAX_PATH];
	PHOOK pHook;
	BOOL bResult = FALSE;
	
	if (hModule == NULL) 
	{
		DBG_STOP;
		return FALSE;
	}
	if (hModule == g_hHookAwareModule) 
		return FALSE;
	
#ifdef _DEBUG
//	if (GetModuleFileName(hModule, szModuleName, sizeof(szModuleName)))
//		ODS(("HookImportInModule: %s:%s", szModuleName, (szFuncName ? szFuncName : "<all funcs>")));
#endif

	__try
	{
		
		pDosHeader=(PIMAGE_DOS_HEADER)hModule;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			ODS(("HookImportInModule: MZ header not found"));
			return FALSE; // Does not point to a correct value
		}
		pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
			ODS(("HookImportInModule: PE header not found"));
			return FALSE; // It is not a PE header position
		}
		
		// Look up where the imports section is (normally in the .idata section)
		// but not necessarily so.  Therefore, grab the RVA from the data dir.
		importsStartRVA = (DWORD)hModule+pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
		importsEndRVA = importsStartRVA + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
		if (!importsStartRVA)
		{
			ODS(("HookImportInModule: !importsStartRVA"));
			return FALSE;
		}
		
		// Get the IMAGE_SECTION_HEADER that contains the imports.  This is
		// usually the .idata section, but doesn't have to be.
		importDesc = (PIMAGE_IMPORT_DESCRIPTOR)importsStartRVA;
		
		while ( 1 )
		{
			// See if we've reached end of imports
			if (importDesc == (PIMAGE_IMPORT_DESCRIPTOR)importsEndRVA)
				break;
			// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
			if ( (importDesc->TimeDateStamp==0 ) && (importDesc->Name==0) )
				break;
			
			//ODS(("importDesc=%08X", importDesc));
			//ODS(("importDesc->Name=%s", (importDesc->Name ? (char*)importDesc->Name+(DWORD)hModule : "NULL")));
			
			thunk    = (PIMAGE_THUNK_DATA)importDesc->Characteristics;
			thunkIAT = (PIMAGE_THUNK_DATA)importDesc->FirstThunk;
			
			if ( thunk == 0 )   // No Characteristics field?
			{
				// Yes! Gotta have a non-zero FirstThunk field then.
				thunk = thunkIAT;
				if ( thunk == 0 )   // No FirstThunk field?  Ooops!!!
					return FALSE;
			}
			
			thunk    = (PIMAGE_THUNK_DATA)( (PBYTE)thunk    + (DWORD)hModule);
			thunkIAT = (PIMAGE_THUNK_DATA)( (PBYTE)thunkIAT + (DWORD)hModule);
			
			while ( 1 ) // Loop forever (or until we break out)
			{
				tHOOK_TBL_ENTRY* pHookTblEntry;
				//ODS(("thunk=%08X", thunk));
				if ( thunk == NULL || thunk->u1.AddressOfData == 0 )
					break;
				
				for(pHookTblEntry = pHookTbl; pHookTblEntry->HookProc != NULL; pHookTblEntry++)
				{
					CHAR* szFuncName = pHookTblEntry->szFuncName;
					LPVOID HookProc = pHookTblEntry->HookProc;
					tHOOKUSERDATA* pHookUserData = pHookTblEntry->pHookUserData;
					
					bTargetFunc = FALSE;
					
					if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
					{
						// Function has no name - only ordinal
						// printf( "  %4u", IMAGE_ORDINAL(thunk->u1.Ordinal) );
						if ((DWORD)szFuncName == IMAGE_ORDINAL(thunk->u1.Ordinal))
							bTargetFunc = TRUE;
					}
					else
					{
						// nik pOrdinalName =  thunk->u1.AddressOfData;
						pOrdinalName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)(thunk->u1.AddressOfData) + (DWORD)hModule);
						// ODS(("HookImportInModule: %s", (char*)pOrdinalName->Name));
						
						if (szFuncName && strcmp(szFuncName, (char*)pOrdinalName->Name) == 0)
							bTargetFunc = TRUE;
					}
					if (szFuncName == NULL)
						bTargetFunc = TRUE;
					
					if (bTargetFunc) 
					{
						DWORD data = (DWORD)&thunkIAT->u1.AddressOfData;
						//ODS(("data=%08X", data));
						EnterCriticalSection(&g_hooks_critical_section);
						if (!IsAddressHookedBy((LPVOID)data, (LPVOID)HookProc))
						{
							pHook = iHookAddress((LPVOID)data, NULL, HookProc, pHookUserData);
							if (pHook)
							{
								bResult = TRUE;
								pHook->pUserData = pHookUserData;
								pHook->dwFlags |= HI_DONT_FREE_ON_RELEASE;
								if (thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
								{
									ODS(("Hooked Import: %s->%s:%d", szModuleName, (char*)importDesc->Name+(DWORD)hModule, IMAGE_ORDINAL(thunk->u1.Ordinal)));
								}
								else
								{
									ODS(("Hooked Import: %s->%s:%s", szModuleName, (char*)importDesc->Name+(DWORD)hModule, (char*)pOrdinalName->Name));
								}
							}
							//ODS(("Hooked"));
						}
						LeaveCriticalSection(&g_hooks_critical_section);
						//else
						//ODS(("Skipped"));
						
					}
				}
				thunk++;            // Advance to next thunk
				thunkIAT++;         // advance to next thunk
			}
			importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
		}
	}
	__except(MyExceptionFilter(GetExceptionInformation()))
	{
		ODS(("HookImportInModule: Exception occured"));
	}
	return bResult;
}

BOOL HookExportInModule(HMODULE hModule, char* szFuncName, PVOID pHookFunc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags)
{
	tHOOK_TBL_ENTRY HookTable[] = {
		{ szFuncName, pHookFunc, pHookUserData },
		LAST_HOOK_TBL_ENTRY
	};
	return HookExportInModuleTbl(hModule, &HookTable[0], dwFlags);
}

// --------------------------------------------------------------------------------------------------------
BOOL HookExportInModuleTbl(HMODULE hModule, tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	//HMODULE hModule;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_EXPORT_DIRECTORY exportDir;
	DWORD i,j;
	DWORD *functions;
	WORD *ordinals;
	PCHAR *name;
	DWORD OrdinalBase;
	static char szModuleName[MAX_PATH];
	PHOOK pHook;
	BOOL bResult = FALSE;
	DWORD exportsStartRVA;
	DWORD exportsEndRVA;
	DWORD exportsSize; 
	
	//	if (hKrnl32==NULL) 
	//		hKrnl32 = GetModuleHandle(szKernel32Dll);
	//	if (hNtDllDll==NULL) 
	//		hNtDllDll = GetModuleHandle(szNtDllDll);
	
	if (hModule == NULL) 
	{
		DBG_STOP;
		return FALSE;
	}
	if (hModule == g_hHookAwareModule) 
		return FALSE;
	
#ifdef _DEBUG
//	if (GetModuleFileName(hModule, szModuleName, sizeof(szModuleName)))
//		ODS(("HookExportInModule: %s:%s", szModuleName, (szFuncName ? szFuncName : "<all funcs>")));
#endif
	
	__try
	{
		
		// Hooking only loaded modules (else change GetModuleHandle to LoadLibrary)
		pDosHeader=(PIMAGE_DOS_HEADER)hModule;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			ODS(("HookExportInModule: MZ header not found"));
			return FALSE; // Does not point to a correct value
		}
		pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
			ODS(("HookExportInModule: PE header not found"));
			return FALSE; // It is not a PE header position
		}

		exportsStartRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		exportsSize = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		exportsEndRVA = exportsStartRVA + exportsSize; 
		if (!exportsStartRVA || !exportsSize) 
			return FALSE;
		// Now we have to parse the Export Table names
		exportDir = (PIMAGE_EXPORT_DIRECTORY)((PCHAR)hModule+exportsStartRVA);
		functions = (DWORD*)((PCHAR)hModule+exportDir->AddressOfFunctions);
		ordinals = (WORD*)((PCHAR)hModule+exportDir->AddressOfNameOrdinals);
		name = (PSTR*)((PCHAR)hModule+exportDir->AddressOfNames);
		OrdinalBase = (WORD)((PCHAR)hModule+exportDir->Base);

		for ( j=0; j < exportDir->NumberOfNames; j++ ) 
		{
			tHOOK_TBL_ENTRY* pHookTblEntry;
			for(pHookTblEntry = pHookTbl; pHookTblEntry->HookProc != NULL; pHookTblEntry++)
			{
				CHAR* szFuncName = pHookTblEntry->szFuncName;
				LPVOID pHookFunc = pHookTblEntry->HookProc;
				tHOOKUSERDATA* pHookUserData = pHookTblEntry->pHookUserData;
				
				if(szFuncName==NULL || strcmp((PSTR)((PCHAR)hModule+(DWORD)name[j]),szFuncName)==0) 
				{
					for ( i=0; i < exportDir->NumberOfFunctions; i++ )
					{
						if ( ordinals[j] == i ) 
						{					
							if (functions[i] == 0 )   // Skip over gaps in exported function
								break;               // ordinals (the entrypoint is 0 for these functions).
							if (functions[i] >= exportsStartRVA && 
								functions[i] < exportsEndRVA )
							{
								ODS(("HookExportInModule: skip forwarder %s.%s -> %s",  (char*)exportDir->Name+(DWORD)hModule, (PCHAR)hModule+(DWORD)name[j], functions[i]+(DWORD)hModule));
								break;
							}
							// Hooking exported function
							pHook = iHookAddress(functions+i, hModule, pHookFunc, pHookUserData);
							if (pHook)
							{
								bResult = TRUE;
								pHook->pUserData = pHookUserData;
								pHook->dwFlags |= HI_DONT_FREE_ON_RELEASE;
								ODS(("Hooked Export: %s->%s:%s", szModuleName, (char*)exportDir->Name+(DWORD)hModule, ((PCHAR)hModule+(DWORD)name[j])));
							}
						}
						// increment module timestamp to force import table resolving for loading modules
						PatchDWORD(&pNTHeader->FileHeader.TimeDateStamp, pNTHeader->FileHeader.TimeDateStamp+1);
					}	
				}
			}
		}
		
	}
	__except(MyExceptionFilter(GetExceptionInformation()))
	{
		ODS(("HookExportInModule: Exception occured"));
	}
	return bResult;
}

// --------------------------------------------------------------------------------------------------------

VOID W9EnumModulesByPid(DWORD PID, tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{ 
	HANDLE			hSnap=NULL;
	MODULEENTRY32	me={0};
	if((hSnap=pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE,PID))==(HANDLE)-1)
		return;
	me.dwSize=sizeof(MODULEENTRY32);
	if(pModule32First(hSnap,&me)) {
		do {
			if ((ULONG)me.modBaseAddr < (ULONG)0x70000000L) // Avoid hooking shared DLLs
			{
				//ODS(("%08x %s",me.modBaseAddr,me.szExePath));
				if (dwFlags & FLAG_HOOK_EXPORT)
					HookExportInModuleTbl((HMODULE)me.modBaseAddr, pHookTbl, dwFlags);
				if (dwFlags & FLAG_HOOK_IMPORT)
					HookImportInModuleTbl((HMODULE)me.modBaseAddr, pHookTbl, dwFlags);
			}
			else
			{
				//ODS(("%08x %s - skipped (>0x70000000), shared dll",me.modBaseAddr,me.szExePath));
			}
		}while(pModule32Next(hSnap,&me));
	}
	CloseHandle(hSnap);
}

VOID W9EnumModules(tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{ 
	W9EnumModulesByPid(GetCurrentProcessId(), pHookTbl, dwFlags);
}

BOOL GetToolHelpEP(VOID)
{
	HMODULE hModKrnl32=GetModuleHandle("kernel32.dll");
	if(!(pCreateToolhelp32Snapshot=(HANDLE (WINAPI *)(unsigned long,unsigned long))GetProcAddress(hModKrnl32,"CreateToolhelp32Snapshot")))
		return FALSE;
	if(!(pModule32First=(BOOL (WINAPI *)(void *,struct tagMODULEENTRY32 *))GetProcAddress(hModKrnl32,"Module32First")))
		return FALSE;
	if(!(pModule32Next=(BOOL (WINAPI *)(void *,struct tagMODULEENTRY32 *))GetProcAddress(hModKrnl32,"Module32Next")))
		return FALSE;
	return TRUE;
}

// ----------------------------------------------------------------------------------------------------

BOOL NTEnumModulesByPid(DWORD PID, tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	HRESULT hRes;
	HANDLE hProcess;
	PROC_INFO ProcInfoBuff;
	DWORD XZ,Mod,hModule,pModName,pFileName;
	//WCHAR ModName[MAX_PATH];
	WCHAR FileName[MAX_PATH];
	BYTE Mi[0x48];
	if(PID==0 || PID==2)
		return TRUE;
	if(!(hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID))) {
		ODS(("Unable to open process"));
		return FALSE;
	}
	if(hRes = ZwQueryInformationProcess(hProcess,0,(PVOID)&ProcInfoBuff,sizeof(ProcInfoBuff),NULL)) {
		ODS(("Unable to query process info"));
		CloseHandle(hProcess);
		return FALSE;
	}
	if(!ReadProcessMemory(hProcess,(PVOID)(ProcInfoBuff.UPEB+0x0c),&XZ,sizeof(XZ),NULL)) {
		ODS(("Unable to read process XZ"));
		CloseHandle(hProcess);
		return FALSE;
	}
	if(!ReadProcessMemory(hProcess,(PVOID)(XZ+=0x14),&Mod,sizeof(Mod),NULL)) {
		ODS(("Unable to read process Mod"));
		CloseHandle(hProcess);
		return FALSE;
	}
	//ODS(("UserPEB:%x XZ:%x Mod:%x",ProcInfoBuff.UPEB,XZ,Mod));
	while(Mod!=XZ) {
		if(!ReadProcessMemory(hProcess,(PVOID)(Mod-8),Mi,sizeof(Mi),NULL)) {
			ODS(("Unable to read process Mi"));
			CloseHandle(hProcess);
			return FALSE;
		}
		Mod=*(DWORD*)&(Mi[8]);
		hModule=*(DWORD*)&(Mi[0x18]);
		pModName=*(DWORD*)&(Mi[0x30]);
		pFileName=*(DWORD*)&(Mi[0x28]);
		//			ZeroMemory(ModName,sizeof(ModName));
		//			ReadProcessMemory(hProcess,(PVOID)(pModName),ModName,sizeof(ModName),NULL);
		ZeroMemory(FileName, sizeof(FileName));
		ReadProcessMemory(hProcess,(PVOID)(pFileName),FileName,sizeof(FileName),NULL);
		//ODS(("%08x %S",hModule,FileName));
		if (dwFlags & FLAG_HOOK_EXPORT)
			HookExportInModuleTbl((HMODULE)hModule, pHookTbl, dwFlags);
		if (dwFlags & FLAG_HOOK_IMPORT)
			HookImportInModuleTbl((HMODULE)hModule, pHookTbl, dwFlags);
	};
	CloseHandle(hProcess);
	return TRUE;
}

typedef struct _ModuleInfo{
	BYTE		U1[8];
	DWORD		NextMod;
	BYTE		U2[0xc];
	HANDLE		hModule;			// offset in process memory
	PVOID		EntryPoint;
	DWORD		VirtualImageSize;	// Module size
	WORD		FullFileNameLen_2;	// in bytes without zeroterminator
	WORD		FullFileNameLen;	// in bytes
	WCHAR*		pFullFileName;
	WORD		ModNameLen_2;
	WORD		ModNameLen;
	WCHAR*		pModName;
	DWORD		U3;					//Flags???
	DWORD		U4;					//0x0000ffff
	PVOID		u5;
	PVOID		u6;
	DWORD		U7;
} ModuleInfo,*PModuleInfo;

DWORD NTGetPebAddress()
{
	DWORD peb;
	__asm
	{
		mov  eax,fs:[0x18]; // TEB
		mov  eax,dword ptr [eax+0x30]; // PEB
		mov  peb, eax;
	}
	return peb;
}

BOOL NTEnumModules(tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	DWORD XZ,Mod,peb;
	ModuleInfo* pMi = NULL;

	__try 
	{
		peb = NTGetPebAddress();
		XZ = *(DWORD*)(peb+0x0c);
		XZ += 0x14;
		Mod = *(DWORD*)XZ;
		while(Mod!=XZ) 
		{
			pMi = (ModuleInfo*)(Mod-8);
			Mod = pMi->NextMod;
			ODS(("UserPEB:%x XZ:%x Mod:%x hMod:%x %S <%S>",peb,XZ,Mod,pMi->hModule,pMi->pModName,pMi->pFullFileName));
			if (dwFlags & FLAG_HOOK_EXPORT)
				HookExportInModuleTbl((HMODULE)pMi->hModule, pHookTbl, dwFlags);
			if (dwFlags & FLAG_HOOK_IMPORT)
				HookImportInModuleTbl((HMODULE)pMi->hModule, pHookTbl, dwFlags);
		};
	} 
	__except(1)
	{
		ODS(("HookNTEnumModules: Exception occured!!!"));
		return FALSE;
	}
	return TRUE;
}



BOOL GetNTdllEP(VOID)
{
	HMODULE hModNtDll=GetModuleHandle("ntdll.dll");
	if(!(ZwQuerySystemInformation=(DWORD (FAR WINAPI *)(unsigned long,void *,unsigned long,unsigned long *))GetProcAddress(hModNtDll,"ZwQuerySystemInformation")))
		return FALSE;
	if(!(ZwQueryInformationProcess=(DWORD (FAR WINAPI *)(void *,unsigned long,void *,unsigned long,unsigned long *))GetProcAddress(hModNtDll,"ZwQueryInformationProcess")))
		return FALSE;
	return TRUE;
}

void EnumModules(tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	OSVERSIONINFO	OSVer;
	DWORD PID = 0;
	OSVer.dwOSVersionInfoSize=sizeof(OSVer);
	if(!GetVersionEx(&OSVer))
	{
		ODS(("Cannot get version info"));
		return;
	}
	//PID = GetCurrentProcessId();
	switch (OSVer.dwPlatformId) {
	case VER_PLATFORM_WIN32s:
		ODS(("Win32S???"));
		return;
	case VER_PLATFORM_WIN32_WINDOWS:
		//			ODS(("Windows 9x detected"));
		if(!GetToolHelpEP()) {
			ODS(("Error loading ToolHelp library"));
			return;
		}
		//W9EnumProcesses();
		W9EnumModules(pHookTbl, dwFlags);
		break;
	case VER_PLATFORM_WIN32_NT:
		//			ODS(("Windows NT detected"));
		if(!GetNTdllEP()) {
			ODS(("GetNTdllEP failed"));
			return;
		}
		NTEnumModules(pHookTbl, dwFlags);
		break;
	}
	return;
}

// ----------------------------------------------------------------------------------------------------
BOOL HookProcInProcess(CHAR* szFuncName, VOID* HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags)
{
	tHOOK_TBL_ENTRY HookTbl[] = {
		{ szFuncName, HookProc, pHookUserData },
		LAST_HOOK_TBL_ENTRY
	};
	return HookProcInProcessTbl(&HookTbl[0], dwFlags);
}

// ----------------------------------------------------------------------------------------------------
BOOL HookProcInProcessTbl(tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	HMODULE hModule;
	
	if (dwFlags == 0)
	{
		DBG_STOP;
		return FALSE;
	}
	
	hModule = GetModuleHandle(NULL);
	ODS(("HookProcInProcess: Handle to the file used to create the calling process = %08X", hModule));
	EnumModules(pHookTbl, dwFlags);
	ODS(("HookProcInProcess: END"));
	
	return TRUE;
}


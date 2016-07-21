#define _HOOKBASE64_IMP_
#include <crtdbg.h>

#include "hookbase64.h"

#ifndef countof
#define countof(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

// =========================================================================================================
//  DEBUG
// =========================================================================================================

static void OutputDbgString(char* szFormat, ...)
{
	char szDbgStr[4096];
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	wvsprintfA(szDbgStr, szFormat, marker);
	va_end(marker);  
	lstrcatA(szDbgStr, "\n"); 
	OutputDebugStringA(szDbgStr);
	//Sleep(1);
}

static void OutputDbgString2(char* szFormat, ...)
{
	char szDbgStr[4096];
	va_list marker;
	va_start( marker, szFormat );     /* Initialize variable arguments. */
	wvsprintfA(szDbgStr, szFormat, marker);
	va_end(marker);  
	OutputDebugStringA(szDbgStr);
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

typedef size_t size_t;
typedef unsigned int alignment_type;

typedef struct tag_PageControlBlock PageControlBlock;

typedef struct tag_PageControlBlock{
	size_t thePagesCount;
	PageControlBlock* theNextPCB;
	alignment_type* theFirstUnusedBlock;
	size_t theNumberOfFreeBlocks;
}PageControlBlock;

typedef struct tag_PMM_CTX {
	DWORD flProtect;
	PageControlBlock* thePCB;
	size_t thePageSize;
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
	HeapFree(GetProcessHeap(), 0, pPMMCtx);
}

#define ALIGN_PTR(ptr, alignment, align_up) (void*)(((ULONG_PTR)ptr + (align_up ? (alignment-1) : 0 )) & (~(alignment-1)))

void* FindNearestFreeMemoryRange(void* pBaseAddress, size_t size)
{
	void* pStartAddress = pBaseAddress;
	MEMORY_BASIC_INFORMATION mbi;
	do
	{
		pBaseAddress = ALIGN_PTR(pBaseAddress, 64*1024, TRUE);
		if (!VirtualQuery(pBaseAddress, &mbi, sizeof(mbi)))
			break;
		if (0 == mbi.RegionSize)
			break;
		if (mbi.State == MEM_FREE && mbi.RegionSize >= size && (ULONG_PTR)mbi.BaseAddress > (ULONG_PTR)pStartAddress)
			return pBaseAddress;
		pBaseAddress = (PBYTE)pBaseAddress + mbi.RegionSize;
	} while (1);
	return 0;
}

void* PMMAlloc(PMM_CTX* pPMMCtx, size_t aSize, void* pBaseAddress)
{
    PageControlBlock* aPCB = pPMMCtx->thePCB;
    PageControlBlock* aPrevPCB = aPCB;
	size_t thePageSize = pPMMCtx->thePageSize;
	void* aResult;
	size_t aNumberOfNeededBlocks;
	
    // trying to find a memory block with needed size
    while ( aPCB ) {
		if ( aPCB->theNumberOfFreeBlocks * sizeof ( alignment_type ) > aSize &&
			 (ULONG_PTR)((BYTE*)aPCB->theFirstUnusedBlock + aSize - (BYTE*)pBaseAddress) < 0x7FFFFFFFL)
			break;
		aPrevPCB = aPCB;
		aPCB = aPCB->theNextPCB;
    }
    // if a block hasn't been found, allocate new page of memory
    if ( !aPCB ) {
		size_t aPagesCount = ( aSize + sizeof ( PageControlBlock ) ) / thePageSize + 
			( ( ( aSize + sizeof ( PageControlBlock ) )  % thePageSize ) ? 1 : 0 );
		size_t aFullSize = thePageSize * aPagesCount;
		void* pNearestRange = pBaseAddress;
		do {
			pNearestRange = FindNearestFreeMemoryRange(pNearestRange, aFullSize);
			if (!pNearestRange)
				return 0;
			aPCB = (PageControlBlock*)( VirtualAlloc ( pNearestRange, aFullSize, MEM_RESERVE | MEM_COMMIT, pPMMCtx->flProtect ) );
		} while (!aPCB);
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

#ifdef _WIN64
#define HOOK_CODE_SIZE 0x64
#else
#define HOOK_CODE_SIZE 0x50
#endif

typedef struct _HOOK {
	BYTE    Code[HOOK_CODE_SIZE];
	struct _HOOK*	pPrevHook;
	void*	pOrigFunc;
	void*	pHookFunc;
	void*	pPatch;
	void*	pBase;
	void*   pRealHookFunc;
	void*   pUserData;
	DWORD   dwFlags;
} HOOK, *PHOOK;

DWORD tlsCurrentHookIndex = TLS_OUT_OF_INDEXES;
DWORD tlsCurrentHook2Index = TLS_OUT_OF_INDEXES;

extern void HookThunk();

void* GetCurrentHook()
{
	return TlsGetValue(tlsCurrentHookIndex);
}

void* HookGetRetAddr()
{
	return TlsGetValue(tlsCurrentHook2Index);
}

void* HookGetOrigFunc()
{
	return ((PHOOK)GetCurrentHook())->pOrigFunc;
}

void __stdcall SetCurrentHook(HOOK* pHook, void* pRetAddr)
{
	TlsSetValue(tlsCurrentHookIndex, pHook);
	TlsSetValue(tlsCurrentHook2Index, pRetAddr);
}

void* GetThunkCode()
{
	BYTE* pThunkCode = (BYTE*)HookThunk;
	if (pThunkCode[0] == 0xe9) // jmp -> debug build
		pThunkCode = pThunkCode+*(LONG*)(pThunkCode+1)+5;
	return pThunkCode;
}

void DisableHook(PHOOK pHook)
{
	BYTE* pCode = &pHook->Code[0];
	*(WORD*)pCode = *(WORD*)GetThunkCode(); // jmp hook_disabled
}

void EnableHook(PHOOK pHook)
{
	BYTE* pCode = &pHook->Code[0];
	//*(WORD*)pCode = 0xFF8B; // mov edi, edi
	pCode[1] = 0; // jmp $+2
}

void* FindThunkPtrSlot(void* ptr)
{
	BYTE* pCode = (BYTE*)ptr;
	do
	{
		if (pCode[0] == 0x78
			&& pCode[1] == 0x56
			&& pCode[2] == 0x34
			&& pCode[3] == 0x12
			)
			break;
		pCode++;
	} while (1);
	return pCode;
}

//void InitHookCode(PHOOK pHook, void* pOrigFunc, void* pHookFunc, void* pUserData)
//{
//	void* pCode = &pHook->Code[0];
//	memcpy(pCode, GetThunkCode(), HOOK_CODE_SIZE);
//	pCode = FindThunkPtrSlot(pCode);
//	*(void**)pCode = pHookFunc;
//	pCode = FindThunkPtrSlot(pCode);
//	*(void**)pCode = pUserData;
//	pCode = FindThunkPtrSlot(pCode);
//	*(void**)pCode = pOrigFunc;
//	pCode = FindThunkPtrSlot(pCode);
//	*(void**)pCode = pOrigFunc;
//	EnableHook(pHook);
//}

void InitHookCode(PHOOK pHook, void* pOrigFunc, void* pHookFunc, void* pUserData)
{
	void* pCode = &pHook->Code[0];
	memcpy(pCode, GetThunkCode(), HOOK_CODE_SIZE);
	pCode = FindThunkPtrSlot(pCode);
	*(void**)pCode = pHook;
	pCode = FindThunkPtrSlot(pCode);
	*(void**)pCode = SetCurrentHook;
	pCode = FindThunkPtrSlot(pCode);
	*(void**)pCode = pHookFunc;
	pCode = FindThunkPtrSlot(pCode);
	*(void**)pCode = pOrigFunc;
	EnableHook(pHook);
}
#if defined(_WIN64)


#elif defined(_WIN32) // _WIN32 section

/*
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
		mov		eax, [ebx]HOOK.OrigFunc
		add		eax, [ebx]HOOK.hModule
		mov 	dword ptr [esp-8], eax    // push OrigFunc
		mov 	ebx, dword ptr [esp-0x14] // restore ebx
		mov 	eax, dword ptr [esp-0x18] // restore eax
		sub 	esp, 0x10

		retn

_jmp_hook:
//		mov		eax, [esp-0x20]           // get UserData
//		mov 	dword ptr [esp-4], eax    // push UserData
		mov 	dword ptr [esp-4], ebx    // push UserData
		mov		eax, [ebx]HOOK.OrigFunc
		add		eax, [ebx]HOOK.hModule
		mov 	dword ptr [esp-8], eax    // push OrigFunc
		mov 	ebx, dword ptr [esp-0x10] // restore ebx
		mov 	eax, dword ptr [esp-0x14] // restore eax
		sub 	esp, 0x0c

		retn
	}
}
*/

//void __declspec(naked) HookThunk(void)
//{
//	__asm{
//		jmp     hook_disabled
//		pop 	dword ptr [esp-0x0c]      // move retaddr up
//		mov     dword ptr [esp-0x10], 0x12345678 // push hook
//		push    0x12345678                // push user data
//		push    0x12345678                // push orig
//		sub     esp, 8;                   // jump over ret addr and hook addr
//		retn                              // jump to hook
//hook_disabled:
//		push    0x12345678                // push orig
//		retn
//	}
//}
void __declspec(naked) HookThunk(void)
{
	__asm{
		jmp     hook_disabled             // <-> mov edi, edi
		push    eax
		push    ecx
		push    edx
		push    [esp+0x0c]
		push    0x12345678                // push pHook
		mov     eax, 0x12345678
		call    eax                       // call SetCurrentHook
		pop     edx
		pop     ecx
		pop     eax
		push    0x12345678                // push HookFunc
		retn                              // jump to HookFunc
hook_disabled:
		push    0x12345678                // push OrigFunc
		retn
	}
}

#endif

static PHOOK   g_pLastHook = NULL;
static PMM_CTX*  g_pPMMCtx;

// ---------------------------------------------------------------------------------------------------------

static CRITICAL_SECTION g_hooks_critical_section;
static BOOL g_bHookBaseInited = FALSE;
static DWORD g_dwPlatformId = 0;

BOOL HookBaseInit()
{
	if (!g_bHookBaseInited)
	{
		OSVERSIONINFO	OSVer;
		OSVer.dwOSVersionInfoSize=sizeof(OSVer);
		if(!GetVersionEx(&OSVer))
		{
			ODS(("Cannot get version info"));
			return FALSE;
		}
		g_dwPlatformId = OSVer.dwPlatformId;
		if (g_dwPlatformId != VER_PLATFORM_WIN32_WINDOWS && g_dwPlatformId != VER_PLATFORM_WIN32_NT)
			return FALSE;
		
		tlsCurrentHookIndex = TlsAlloc();
		if (TLS_OUT_OF_INDEXES == tlsCurrentHookIndex)
			return FALSE;
		tlsCurrentHook2Index = TlsAlloc();
		if (TLS_OUT_OF_INDEXES == tlsCurrentHook2Index)
			return FALSE;

		InitializeCriticalSection(&g_hooks_critical_section);
		g_bHookBaseInited = PMMInit(PAGE_EXECUTE_READWRITE, &g_pPMMCtx);
	}
	return g_bHookBaseInited;
}

VOID HookBaseDone()
{
	if (g_bHookBaseInited)
	{
		UnHookAll();
		if (g_pPMMCtx)
			PMMDone(g_pPMMCtx);
		g_pPMMCtx = NULL;
		if (TLS_OUT_OF_INDEXES != tlsCurrentHookIndex)
			TlsFree(tlsCurrentHookIndex);
		if (TLS_OUT_OF_INDEXES != tlsCurrentHook2Index)
			TlsFree(tlsCurrentHook2Index);
		DeleteCriticalSection(&g_hooks_critical_section);
	}
}

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

PHOOK AllocHook(void* pAddress, void* pBase, void* pHookFunc, tHOOKUSERDATA* pUserData)
{
	PHOOK pHook = (PHOOK)PMMAlloc(g_pPMMCtx, sizeof(HOOK), pBase); 
	if (pHook == NULL)
	{
		ODS(("HookBase:HookObjectMethod Error allocating heap memory"));
		return NULL;
	}
	ZeroMemory(pHook, sizeof(HOOK)); // zero memory
	pHook->pBase     = pBase;
	pHook->pHookFunc = pHookFunc;
	if (pBase)
		pHook->pOrigFunc = (BYTE*)pBase + *(DWORD32*)pAddress;
	else
		pHook->pOrigFunc = *(void**)pAddress;
	pHook->pPatch    = pAddress;
	pHook->pUserData = pUserData;
	InitHookCode(pHook, pHook->pOrigFunc, pHookFunc, pUserData);
	return pHook;
}

BOOL SetHook(PHOOK pHook)
{
	DWORD flOldProtect;
	BOOL bRelative = (pHook->pBase != 0);
	BOOL bRes = FALSE;

	EnterCriticalSection(&g_hooks_critical_section);
	// Change memory protection to get writing rights.
//	if (pHook->pPatch == 0x000007FF7C754F18)
//		bRes = bRes;
	if (!VirtualProtect(pHook->pPatch, bRelative ? sizeof(DWORD) : sizeof(void*), PAGE_EXECUTE_READWRITE, &flOldProtect))
	{
		ODS(("HookObjectMethod: Fail to change memory protection! err=%08X", GetLastError()));
	}
	else
	{
		// Patching (patched address pointing to pHook->Code)
		if (bRelative)
			*(DWORD*)pHook->pPatch = (DWORD)(pHook->Code - (BYTE*)pHook->pBase); 
		else
			*(void**)pHook->pPatch = pHook->Code; 
		// Restore protection
		VirtualProtect(pHook->pPatch, bRelative ? sizeof(DWORD) : sizeof(void*), flOldProtect, &flOldProtect);
		// link hook
		pHook->pPrevHook = g_pLastHook; 
		g_pLastHook = pHook;
		bRes = TRUE;
	}
	LeaveCriticalSection(&g_hooks_critical_section);
	return bRes;
}

PHOOK iHookAddress(void* pAddress, void* pBase, void* pHookFunc, tHOOKUSERDATA* pUserData)
{
	PHOOK pHook = AllocHook(pAddress, pBase, pHookFunc, pUserData);
	if (pHook)
	{
		if (!SetHook(pHook))
		{
			PMMFree(g_pPMMCtx, pHook);
			pHook = NULL;
		}
	}
	return pHook;
}

BOOL HookAddress(void* pAddress, void* pBase, void* pHookFunc, tHOOKUSERDATA* pUserData)
{
	PHOOK pHook = iHookAddress(pAddress, pBase, pHookFunc, pUserData);
	if (!pHook)
		return FALSE;
	return TRUE;
}

BOOL HookObjectMethod(void* lpObject, DWORD Method, void* pHookFunc, tHOOKUSERDATA* pUserData)
{
	void** lpVtbl;
	void** lpMethod;
	
	//ODS(("HookObjectMethod..."));
	// Calculate patching address
	if (lpObject == NULL)
	{
		ODS(("HookBase:HookObjectMethod Error: lpObject = NULL"));
		return FALSE;
	}
	lpVtbl = *(void***)lpObject;
	if (lpVtbl == NULL)
	{
		ODS(("HookBase:HookObjectMethod Error: VtblAddress = NULL"));
		return FALSE;
	}
	lpMethod = lpVtbl + Method;
	return HookAddress(lpMethod, NULL, pHookFunc, pUserData);
}


BOOL HookObjectMethodIfNotHookedBy(void* lpObject, DWORD Method, void* pHookFunc, tHOOKUSERDATA* pUserData)
{
	BOOL bResult = TRUE;
	EnterCriticalSection(&g_hooks_critical_section);
	if (!IsMethodHookedBy(lpObject,Method,pHookFunc) )
		bResult = HookObjectMethod(lpObject,Method,pHookFunc,pUserData);
	LeaveCriticalSection(&g_hooks_critical_section);
	return bResult;
}



BOOL UnHook(void* pHookFunc)
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
		if (pHook->pHookFunc == pHookFunc || ((pHook->dwFlags & HI_JMP_HOOK) && pHook->pRealHookFunc == pHookFunc))
		{
			DisableHook(pHook);
			if (pHook->pPatch)
			{
				BOOL bRelative = (pHook->pBase != 0);
				if (VirtualProtect(pHook->pPatch, bRelative ? sizeof(DWORD) : sizeof(void*), PAGE_EXECUTE_READWRITE, &flOldProtect))
				{
					void* pPatchedValue;
					if (bRelative)
						pPatchedValue = (BYTE*)pHook->pBase + *(DWORD*)pHook->pPatch;
					else
						pPatchedValue = *(void**)pHook->pPatch;
					if (pPatchedValue != pHook->Code)
					{
						// Other hook has been installed for this address
						BOOL bFound = FALSE;
						PHOOK pHook2 = g_pLastHook;
						while (pHook2 != NULL)
						{
							if (pHook2->pOrigFunc == pHook->Code)
							{
								pHook2->pOrigFunc = pHook->pOrigFunc;
								bFound = TRUE;
							}
							pHook2 = pHook2->pPrevHook;
						}
						//_ASSERT(bFound);
					}
					else
					{
						// It is only hook for this address - restore original value
						if (bRelative)
							*(DWORD*)pHook->pPatch = (DWORD)((BYTE*)pHook->pOrigFunc - (BYTE*)pHook->pBase);
						else
							*(void**)pHook->pPatch = pHook->pOrigFunc;
					}
					VirtualProtect((void*)pHook->pPatch, sizeof(DWORD), flOldProtect, &flOldProtect);
				}
			}
			else
			{
				ODS(("VirtualProtect failed for addr=%08X with err=%08X, Module already freed?", pHook->pPatch, GetLastError()));
			}

//			if (pHook->dwFlags & HI_SZDATA_ALLOCATED)
//				HeapFree(GetProcessHeap(), 0, (void*)pHook->dwFuncDescr);
			
			// remove from list
			if (pPrevHook)
				pPrevHook->pPrevHook = pHook->pPrevHook;
			if (pHook == g_pLastHook)
				g_pLastHook = pHook->pPrevHook;
			pHookTmp = pHook;
			pHook = pHook->pPrevHook;
			if (pHookTmp->dwFlags | HI_DONT_FREE_ON_RELEASE)
			{
				// it was export/import hook. This memory cannot be freed!
			}
			else
				PMMFree(g_pPMMCtx, pHookTmp); // free memory
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
		UnHook((void*)g_pLastHook->pHookFunc);
	LeaveCriticalSection(&g_hooks_critical_section);
	ODS(("HookBase: Completed."));
	return TRUE;
}

BOOL IsHooked(void* pHookFunc)
{
	PHOOK pHook;
	BOOL bHooked = FALSE;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
//	ODS(("HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, pHookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->pHookFunc == pHookFunc || ((pHook->dwFlags & HI_JMP_HOOK) && pHook->pRealHookFunc == pHookFunc))
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
#define GetHookByOrigFunc(OrigFunc)	Dbg_GetHookByOrigFunc(#OrigFunc, OrigFunc)
PHOOK Dbg_GetHookByOrigFunc(LPSTR OrigFuncAddrName, void* OrigFunc){
//ODS(("HookBase:IsHooked(%s)", HookFuncName));
#else
PHOOK GetHookByOrigFunc(void* OrigFunc){
#endif
	PHOOK pHook;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
	while (pHook != NULL)
	{
		if (pHook->pOrigFunc == OrigFunc)
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
BOOL Dbg_IsAddressHooked(LPSTR HookAddrName, void* HookAddr){
//ODS(("HookBase:IsHooked(%s)", HookFuncName));
#else
BOOL IsAddressHooked(void* HookAddr){
#endif
	PHOOK pHook;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
//	ODS(("HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, pHookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->pPatch == HookAddr)
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

BOOL IsMethodHookedBy(void* HookObj, DWORD MethodNum, void* pHookFunc)
{
	return IsAddressHookedBy(((*(void***)HookObj)+MethodNum), pHookFunc);
} 

BOOL IsAddressHookedBy(void* HookAddr, void* pHookFunc){
	PHOOK pHook;
	
	EnterCriticalSection(&g_hooks_critical_section);
	pHook = g_pLastHook;
//	ODS(("HookBase:IsHooked Checking for %s(%08X) g_pLastHook=%08X", HookFuncName, pHookFunc, g_pLastHook));
	while (pHook != NULL)
	{
		if (pHook->pPatch == HookAddr && pHook->pHookFunc == pHookFunc)
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

#if !defined(_WIN64)

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
//	hRes = ((HRESULT (__stdcall *)(DWORD*, pmGUID, DWORD**))OrigFunc)(_this, riid, pInterface);
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

__declspec(naked) void HookQuery(void* OrigFunc, PHOOKINFO pHookInfo, char* param1)
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
void HookInterface(void* pHookFunc, DWORD Interface, DWORD numMethods, char* InterfaceName, ...)
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
			pHookInfo = HookObjectMethod(&Interface, i, pHookFunc);
		if (pHookInfo) 
		{
		}
	}
	va_end( marker ); // Reset variable arguments.      
	return;
}
*/
/*
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

BOOL HookJmpTable(void* pHookFunc, tHOOKUSERDATA* pUserData, void* pJmpTableAddress, DWORD TableLen, char* TableName, ...)
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

		if (!HookObjectMethod(&pJmpTableAddress, i, pHookFunc, &pJmpTableUserData->Header))
		{
			bResult = FALSE;
			break;
		}
	}
	va_end( marker ); // Reset variable arguments.
	return bResult;
}
*/
#endif // _DEBUG

/*
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


BOOL HookJmpTableAddress(void* pTableAddress, DWORD dwOffsetInTable, void* pHookFunc, tHOOKUSERDATA* pUserData)
{
	PHOOK pHook;
	pHook = iHookObjectMethod((void*)&pTableAddress, dwOffsetInTable, JmpTableHook, pUserData);
	if (NULL == pHook)
		return FALSE;
	pHook->pRealHookFunc = (DWORD)pHookFunc;
	pHook->dwFlags |= HI_JMP_HOOK;
	return TRUE;
}
*/
#endif // !_WIN64

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
	DWORD_PTR U1;
	DWORD_PTR UPEB;
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
	return HookImportInModuleEx(hModule, szFuncName, 0, HookProc, pHookUserData, dwFlags);
}

BOOL HookImportInModuleEx(HMODULE hModule, CHAR* szFuncName, CHAR* szFuncModule, void* HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags)
{
	tHOOK_TBL_ENTRY HookTable[] = {
		{ szFuncName, szFuncModule, HookProc, pHookUserData },
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
	void* importsStartRVA;
	void* importsEndRVA;
	INT delta = -1;
	BOOL bTargetFunc;
	static char szModuleName[MAX_PATH];
	PHOOK pHook;
	BOOL bResult = FALSE;
	BOOL bNoNames = FALSE;
	
	if (hModule == NULL) 
	{
//		DBG_STOP;
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
		importsStartRVA = (BYTE*)hModule + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
		importsEndRVA = (BYTE*)importsStartRVA + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
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
			
			thunk    = (PIMAGE_THUNK_DATA)(DWORD_PTR)importDesc->Characteristics;
			thunkIAT = (PIMAGE_THUNK_DATA)(DWORD_PTR)importDesc->FirstThunk;
			
			if ( thunk == 0 )   // No Characteristics field?
			{
				// Yes! Gotta have a non-zero FirstThunk field then.
				thunk = thunkIAT;
				if ( thunk == 0 )   // No FirstThunk field?  Ooops!!!
					return FALSE;
			}

			if (thunk == thunkIAT)
				bNoNames = TRUE;
			
			thunk    = (PIMAGE_THUNK_DATA)( (PBYTE)thunk    + (DWORD_PTR)hModule);
			thunkIAT = (PIMAGE_THUNK_DATA)( (PBYTE)thunkIAT + (DWORD_PTR)hModule);
			
			while ( 1 ) // Loop forever (or until we break out)
			{
				tHOOK_TBL_ENTRY* pHookTblEntry;
				//ODS(("thunk=%08X", thunk));
				if ( thunk == NULL || thunk->u1.AddressOfData == 0 )
					break;
				
				for(pHookTblEntry = pHookTbl; pHookTblEntry->HookProc != NULL; pHookTblEntry++)
				{
					CHAR* szFuncName = pHookTblEntry->szFuncName;
					void* HookProc = pHookTblEntry->HookProc;
					tHOOKUSERDATA* pHookUserData = pHookTblEntry->pHookUserData;
					
					bTargetFunc = FALSE;
					
					if (szFuncName == NULL) // hook all funcs
					{
						bTargetFunc = TRUE;
					}
					else if (bNoNames)
					{
						// there are no import names - they are replaced with resolved addresses
						// so try resolve by address
						HMODULE hModule = NULL;
						void* pProc = NULL;
						if (pHookTblEntry->szFuncModule)
							hModule = GetModuleHandle(pHookTblEntry->szFuncModule);
						if (hModule)
							pProc = GetProcAddress(hModule, pHookTblEntry->szFuncName);
						if (!pProc)
						{
							ODS(("Import: Cannot resolve orig func"));
						}
						else
						{
							if ((void*)thunkIAT->u1.Function == pProc)
								bTargetFunc = TRUE;
						}
					}
					else if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
					{
						// Function has no name - only ordinal
						// printf( "  %4u", IMAGE_ORDINAL(thunk->u1.Ordinal) );
						if ((DWORD_PTR)szFuncName == IMAGE_ORDINAL(thunk->u1.Ordinal))
							bTargetFunc = TRUE;
					}
					else
					{
						// nik pOrdinalName =  thunk->u1.AddressOfData;
						pOrdinalName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hModule + thunk->u1.AddressOfData);
						// ODS(("HookImportInModule: %s", (char*)pOrdinalName->Name));
						
						if (strcmp(szFuncName, (char*)pOrdinalName->Name) == 0)
							bTargetFunc = TRUE;
					}
					
					if (bTargetFunc) 
					{
						void* data = &thunkIAT->u1.Function;
						//ODS(("data=%08X", data));
						EnterCriticalSection(&g_hooks_critical_section);
						if (!IsAddressHookedBy(data, (void*)HookProc))
						{
							pHook = iHookAddress(data, NULL, HookProc, pHookUserData);
							if (pHook)
							{
								bResult = TRUE;
								pHook->pUserData = pHookUserData;
								pHook->dwFlags |= HI_DONT_FREE_ON_RELEASE;
								if ((DWORD_PTR)szFuncName < 0x10000)
								{
									ODS(("Hooked Import: %s->%s:%d %08X", szModuleName, (BYTE*)hModule+importDesc->Name, (DWORD_PTR)szFuncName, data));
								}
								else
								{
									ODS(("Hooked Import: %s->%s:%s %08X", szModuleName, (BYTE*)hModule+importDesc->Name, szFuncName, data));
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
	return HookExportInModuleEx(hModule, szFuncName, 0, pHookFunc, pHookUserData, dwFlags);
}

BOOL HookExportInModuleEx(HMODULE hModule, char* szFuncName, char* szFuncModule, PVOID pHookFunc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags)
{
	tHOOK_TBL_ENTRY HookTable[] = {
		{ szFuncName, szFuncModule, pHookFunc, pHookUserData },
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
	DWORD *name;
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
		//_ASSERT(hModule);
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
		name = (DWORD*)((PCHAR)hModule+exportDir->AddressOfNames);
		OrdinalBase = (WORD)((PCHAR)hModule+exportDir->Base);

		for ( j=0; j < exportDir->NumberOfNames; j++ ) 
		{
			tHOOK_TBL_ENTRY* pHookTblEntry;
			for(pHookTblEntry = pHookTbl; pHookTblEntry->HookProc != NULL; pHookTblEntry++)
			{
				CHAR* szFuncName = pHookTblEntry->szFuncName;
				void* pHookFunc = pHookTblEntry->HookProc;
				tHOOKUSERDATA* pHookUserData = pHookTblEntry->pHookUserData;
				
				if(szFuncName==NULL || strcmp((PSTR)((PCHAR)hModule+(DWORD_PTR)name[j]),szFuncName)==0) 
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
								ODS(("HookExportInModule: skip forwarder %s.%s -> %s",  (char*)hModule+exportDir->Name, (PCHAR)hModule+(DWORD_PTR)name[j], (char*)hModule+functions[i]));
								break;
							}
							// Hooking exported function
							pHook = iHookAddress(functions+i, hModule, pHookFunc, pHookUserData);
							if (pHook)
							{
								bResult = TRUE;
								pHook->pUserData = pHookUserData;
								pHook->dwFlags |= HI_DONT_FREE_ON_RELEASE;
								ODS(("Hooked Export: %s->%s:%s %08X", szModuleName, (BYTE*)hModule+exportDir->Name, (BYTE*)hModule+(DWORD_PTR)name[j], functions+i));
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
			if (g_dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // Win9x
			{
				if ((ULONG_PTR)me.modBaseAddr >= (ULONG_PTR)0x70000000L) // Avoid hooking shared DLLs
					continue;
			}

			//ODS(("%08x %s",me.modBaseAddr,me.szExePath));
			if (dwFlags & FLAG_HOOK_EXPORT)
				HookExportInModuleTbl((HMODULE)me.modBaseAddr, pHookTbl, dwFlags);
			if (dwFlags & FLAG_HOOK_IMPORT)
				HookImportInModuleTbl((HMODULE)me.modBaseAddr, pHookTbl, dwFlags);
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
	HMODULE hModKrnl32;
	static BOOL s_bToolHelpResolved = FALSE;
	if (s_bToolHelpResolved)
		return TRUE;
	hModKrnl32=GetModuleHandleA("kernel32.dll");
	if(!(pCreateToolhelp32Snapshot=(HANDLE (WINAPI *)(unsigned long,unsigned long))GetProcAddress(hModKrnl32,"CreateToolhelp32Snapshot")))
		return FALSE;
	if(!(pModule32First=(BOOL (WINAPI *)(void *,struct tagMODULEENTRY32 *))GetProcAddress(hModKrnl32,"Module32First")))
		return FALSE;
	if(!(pModule32Next=(BOOL (WINAPI *)(void *,struct tagMODULEENTRY32 *))GetProcAddress(hModKrnl32,"Module32Next")))
		return FALSE;
	s_bToolHelpResolved = TRUE;
	return TRUE;
}

// ----------------------------------------------------------------------------------------------------
/*
BOOL NTEnumModulesByPid(DWORD PID, tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	HRESULT hRes;
	HANDLE hProcess;
	PROC_INFO ProcInfoBuff;
	DWORD_PTR XZ,Mod,hModule,pModName,pFileName;
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
*/
void EnumModules(tHOOK_TBL_ENTRY* pHookTbl, DWORD dwFlags)
{
	if(!GetToolHelpEP()) 
	{
		ODS(("Error loading ToolHelp library"));
		return;
	}
	W9EnumModules(pHookTbl, dwFlags);
	return;
}

// ----------------------------------------------------------------------------------------------------
BOOL HookProcInProcess(CHAR* szFuncName, VOID* HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags)
{
	return HookProcInProcessEx(szFuncName, 0, HookProc, pHookUserData, dwFlags);
}

BOOL HookProcInProcessEx(CHAR* szFuncName, char* szFuncModule, VOID* HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags)
{
	tHOOK_TBL_ENTRY HookTbl[] = {
		{ szFuncName, szFuncModule, HookProc, pHookUserData },
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
		//_ASSERT(dwFlags);
		//DBG_STOP;
		return FALSE;
	}
	
	hModule = GetModuleHandle(NULL);
	ODS(("HookProcInProcess: Handle to the file used to create the calling process = %08X", hModule));
	EnumModules(pHookTbl, dwFlags);
	ODS(("HookProcInProcess: END"));
	
	return TRUE;
}


// ======================================================================================
//		Splice hooks
// ======================================================================================

UINT GetPrologueBytes(IN PVOID OriginalFunc, UINT nRequiredBytes)
{
	UINT nPrologueBytes = 0;
	BYTE* pCode = (BYTE*)OriginalFunc;
	UINT nOpcodeLen;
	__try
	{
		while (nPrologueBytes < nRequiredBytes)
		{
			nOpcodeLen = 0;
			switch(pCode[0])
			{
			case 0x68: // push imm32
				nOpcodeLen = 5;
				break;
			}
			if (0 == nOpcodeLen)
				break;
			nPrologueBytes += nOpcodeLen;
			pCode += nOpcodeLen;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		nPrologueBytes = 0;
	}
	if (nPrologueBytes < nRequiredBytes)
		nPrologueBytes = 0;
	return nPrologueBytes;
}

// OriginalFunc - ф-я, на которую накладываем сплайс
// HookFunc - ф-я - перехватчик
// pPrologueVariants - варианты прологов, последний элемент инициализирован как {VARIANT_END}
// p_pContext - возвращаемый контекст
BOOL SpliceHook(IN PVOID OriginalFunc, IN PVOID HookFunc, OUT PSPLICE_CONTEXT *p_pContext)
{
	PSPLICE_CONTEXT pContext;
	PVOID pThunk;
	UINT nPrologueBytes, nContextSize;
	PNEAR_JUMP pJmpOrig;
	BYTE Splice[5];
	DWORD OldProtect;

	*p_pContext = NULL;
	nPrologueBytes = GetPrologueBytes(OriginalFunc, 5);
	if (!nPrologueBytes)
		return FALSE;
	nContextSize = sizeof(SPLICE_CONTEXT)+nPrologueBytes+sizeof(NEAR_JUMP);
	pContext = (PSPLICE_CONTEXT)PMMAlloc(g_pPMMCtx, nContextSize, OriginalFunc);
	if (!pContext)
		return FALSE;
	ZeroMemory(pContext, nContextSize);
	pThunk = (PCHAR)pContext+sizeof(SPLICE_CONTEXT);

	pContext->m_OriginalFunc = OriginalFunc;
	pContext->m_HookFunc = HookFunc;
	pContext->m_nPrologueBytes = nPrologueBytes;
	pContext->m_Thunk = pThunk;
	*p_pContext = pContext;

	// делаем переходник на оригинальную ф-ю, должен быть в execute-памяти!
	memcpy(pThunk, OriginalFunc, nPrologueBytes);							// сохранили пролог
	pJmpOrig = (PNEAR_JUMP)((PCHAR)pThunk+nPrologueBytes);				// после сохраненного пролога идет jump на оригинальную ф-ю
	pJmpOrig->m_JmpNear = 0x25FF;
	pJmpOrig->m_Address = (PCHAR)OriginalFunc + nPrologueBytes;
#if !defined(_WIN64)
	pJmpOrig->m_Displacement = (ULONG)(ULONG_PTR)&pJmpOrig->m_Address;
#endif

	// делаем переходник на HookFunc через jmp near 0xE9
	Splice[0] = 0xE9;
	
#ifdef _WIN64
	*(ULONG*)(Splice+1) =  (ULONG)(ULONG_PTR)((PCHAR)&pContext->m_JmpHook - (PCHAR)OriginalFunc - 5);
	pContext->m_JmpHook.m_JmpNear = 0x25FF;
	pContext->m_JmpHook.m_Address = HookFunc;
#else
	*(ULONG*)(Splice+1) = (ULONG)(ULONG_PTR)((PCHAR)HookFunc - (PCHAR)OriginalFunc - 5);
#endif



	if (!VirtualProtect(OriginalFunc, sizeof(Splice), PAGE_EXECUTE_READWRITE, &OldProtect))
	{
		PMMFree(g_pPMMCtx, pContext);
		return FALSE;
	}
	memcpy(OriginalFunc, &Splice, sizeof(Splice)); // все, установили сплайс
	VirtualProtect(OriginalFunc, sizeof(Splice), OldProtect, &OldProtect);
	
	return TRUE;
}

BOOL SpliceUnhook(PSPLICE_CONTEXT pContext)
{
	DWORD OldProtect;
	if (*(PCHAR)pContext->m_OriginalFunc != 0xE9)
		return FALSE; // ? already unhooked ?

	if (!VirtualProtect(pContext->m_OriginalFunc, pContext->m_nPrologueBytes, PAGE_EXECUTE_READWRITE, &OldProtect))
		return FALSE;
	memcpy(pContext->m_OriginalFunc, pContext->m_Thunk, pContext->m_nPrologueBytes);
	VirtualProtect(pContext->m_OriginalFunc, pContext->m_nPrologueBytes, OldProtect, &OldProtect);
	PMMFree(g_pPMMCtx, pContext);
	return TRUE;
}
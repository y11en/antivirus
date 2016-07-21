// kd3dll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
extern "C"
{
	#include "..\kdprint\user\server\kdprint.h"
}


#include <pshpack1.h>

// код, эквивалентный near jump на FuncPtr
typedef struct _NEARJUMP
{
	UCHAR		m_PushImm_Opcode;		// push immediate = 0x68
	PVOID		m_FuncPtr;				// указатель на ф-ю
	UCHAR		m_RetNear_Opcode;		// ret near = 0xc3
} NEARJUMP, *PNEARJUMP;

#define INIT_NEARJUMP(NearJump, pFunc)		\
{											\
	NearJump.m_PushImm_Opcode = 0x68;		\
	NearJump.m_FuncPtr = pFunc;				\
	NearJump.m_RetNear_Opcode = 0xc3;		\
}

#define MAX_PROLOGUE_SIZE		20

typedef struct _ORIG_FUNC_CALL
{
	UCHAR		m_Nops[MAX_PROLOGUE_SIZE];	// NOP'ы под пролог ф-и
	UCHAR		m_PushImm_Opcode;			// push immediate = 0x68
	PVOID		m_FuncPtr;					// указатель на ф-ю
	UCHAR		m_RetNear_Opcode;			// ret near = 0xc3
} ORIG_FUNC_CALL, *PORIG_FUNC_CALL;

#define INIT_ORIG_FUNC_CALL(OrigFuncCall, pFunc)						\
{																		\
	memset(OrigFuncCall.m_Nops, 0x90/*NOP opcode*/, MAX_PROLOGUE_SIZE);	\
	OrigFuncCall.m_PushImm_Opcode = 0x68;								\
	OrigFuncCall.m_FuncPtr = pFunc;										\
	OrigFuncCall.m_RetNear_Opcode = 0xc3;								\
}

#include <poppack.h>

typedef
BOOL (WINAPI *f_GetMessage)(LPMSG lpMsg,
							HWND hWnd,
							UINT wMsgFilterMin,
							UINT wMsgFilterMax
							);
typedef
BOOL (WINAPI *f_PeekMessage)(	LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax,
								UINT wRemoveMsg
								);
typedef
BOOL (WINAPI *f_PostThreadMessageA)(	DWORD idThread,
										UINT Msg,
										WPARAM wParam,
										LPARAM lParam
										);

//-------------------------------------------------------------------------------------
#define MAX_PROLOGUE_VARIANTS	10
#define MAX_FUNC_NAME_LEN		256

typedef
struct _CHECK_FUNC_PROLOGUE
{
	UCHAR		m_CheckFuncPrologue[MAX_PROLOGUE_SIZE];
	ULONG		m_PrologueLen;
} CHECK_FUNC_PROLOGUE, *PCHECK_FUNC_PROLOGUE;

typedef
struct _SPLICE_ITEM
{
	CHAR				m_FuncName[MAX_FUNC_NAME_LEN];
	PVOID				m_pHookFunc;

	CHECK_FUNC_PROLOGUE	m_PrologueVariants[MAX_PROLOGUE_VARIANTS];
	ULONG				m_NumPrologueVariants;

	BOOL				m_bSpliceSet;
	ULONG				m_PrologueLen;
	PVOID				m_prFunc;
	ORIG_FUNC_CALL		m_rFuncCall;
	NEARJUMP			m_rFuncPrologue;
	NEARJUMP			m_NewFuncPrologue;
} SPLICE_ITEM, *PSPLICE_ITEM;

//-------------------------------------------------------------------------------------
// /SECTION:.exdata,ERW
#pragma data_seg(".exdata")

BOOL WINAPI Hooked_GetMessageA(	LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax
							);
BOOL WINAPI Hooked_GetMessageW(	LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax
							);
BOOL WINAPI Hooked_PeekMessageA(LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax,
								UINT wRemoveMsg
								);
BOOL WINAPI Hooked_PeekMessageW(LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax,
								UINT wRemoveMsg
								);

__declspec(allocate(".exdata")) SPLICE_ITEM	gGetMessageA_Splice =	{
																		"GetMessageA",
																		Hooked_GetMessageA,
																		{
																			{{0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x8B, 0x55, 0x10, 0x8B, 0x4D, 0x14}, 11}
																		},
																		1,
																		FALSE
																	};
__declspec(allocate(".exdata")) SPLICE_ITEM	gGetMessageW_Splice =	{
																		"GetMessageW",
																		Hooked_GetMessageW,
																		{
																			{{0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x8B, 0x55, 0x10, 0x8B, 0x4D, 0x14}, 11}
																		},
																		1,
																		FALSE
																	};
__declspec(allocate(".exdata")) SPLICE_ITEM	gPeekMessageA_Splice =	{
																		"PeekMessageA",
																		Hooked_PeekMessageA,
																		{
																			{{0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x53, 0x56, 0x57, 0x64, 0xA1, 0x18, 0x00, 0x00, 0x00}, 14}
																		},
																		1,
																		FALSE
																	};
__declspec(allocate(".exdata")) SPLICE_ITEM	gPeekMessageW_Splice =	{
																		"PeekMessageW",
																		Hooked_PeekMessageW,
																		{
																			{{0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x53, 0x56, 0x57, 0x64, 0xA1, 0x18, 0x00, 0x00, 0x00}, 14}
																		},
																		1,
																		FALSE
																	};
//-------------------------------------------------------------------------------------
volatile LONG gSysEnters = 0;


BOOL WINAPI Hooked_GetMessageA(	LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax
							)
{
	InterlockedIncrement((PLONG)&gSysEnters);
	
	BOOL bResult = ((f_GetMessage)&gGetMessageA_Splice.m_rFuncCall)(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

	switch ((WORD)lpMsg->message)
	{
	case WM_KEYDOWN:
		{
			char Buffer[32];
			if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
				KDPrint("KD3:GetMessageA %s pressed, PID:%d\n", Buffer, GetCurrentProcessId());

			break;
		}
	case WM_KEYUP:
		{
			char Buffer[32];
			if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
				KDPrint("KD3:GetMessageA %s released, PID:%d\n", Buffer, GetCurrentProcessId());
		}
	}


	InterlockedDecrement((PLONG)&gSysEnters);

	return bResult;
}

BOOL WINAPI Hooked_GetMessageW(	LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax
							)
{
	InterlockedIncrement((PLONG)&gSysEnters);

	BOOL bResult = ((f_GetMessage)&gGetMessageW_Splice.m_rFuncCall)(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

	switch ((WORD)lpMsg->message)
	{
	case WM_KEYDOWN:
		{
			char Buffer[32];
			if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
				KDPrint("KD3:GetMessageW %s pressed, PID:%d\n", Buffer, GetCurrentProcessId());

			break;
		}
	case WM_KEYUP:
		{
			char Buffer[32];
			if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
				KDPrint("KD3:GetMessageW %s released, PID:%d\n", Buffer, GetCurrentProcessId());
		}
	}


	InterlockedDecrement((PLONG)&gSysEnters);

	return bResult;
}

BOOL WINAPI Hooked_PeekMessageA(LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax,
								UINT wRemoveMsg
								)
{
	InterlockedIncrement((PLONG)&gSysEnters);

	BOOL bResult = ((f_PeekMessage)&gPeekMessageA_Splice.m_rFuncCall)(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

	if (bResult)
	{
		switch ((WORD)lpMsg->message)
		{
		case WM_KEYDOWN:
			{
				char Buffer[32];
				if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
					KDPrint("KD3:PeekMessageA %s pressed, RemoveMsg:%d, PID:%d\n", Buffer,
					wRemoveMsg, GetCurrentProcessId());

				break;
			}
		case WM_KEYUP:
			{
				char Buffer[32];
				if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
					KDPrint("KD3:PeekMessageA %s released, RemoveMsg:%d, PID:%d\n", Buffer,
					wRemoveMsg, GetCurrentProcessId());
			}
		}
	}

	InterlockedDecrement((PLONG)&gSysEnters);

	return bResult;
}

BOOL WINAPI Hooked_PeekMessageW(LPMSG lpMsg,
								HWND hWnd,
								UINT wMsgFilterMin,
								UINT wMsgFilterMax,
								UINT wRemoveMsg
								)
{
	InterlockedIncrement((PLONG)&gSysEnters);

	BOOL bResult = ((f_PeekMessage)&gPeekMessageW_Splice.m_rFuncCall)(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

	if (bResult)
	{
		switch ((WORD)lpMsg->message)
		{
		case WM_KEYDOWN:
			{
				char Buffer[32];
				if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
					KDPrint("KD3:PeekMessageW %s pressed, RemoveMsg:%d, PID:%d\n", Buffer,
					wRemoveMsg, GetCurrentProcessId());

				break;
			}
		case WM_KEYUP:
			{
				char Buffer[32];
				if (GetKeyNameText(lpMsg->lParam, Buffer, 32))
					KDPrint("KD3:PeekMessageW %s released, RemoveMsg:%d, PID:%d\n", Buffer,
					wRemoveMsg, GetCurrentProcessId());
			}
		}
	}

	InterlockedDecrement((PLONG)&gSysEnters);

	return bResult;
}

// установка сплайсеров..
BOOL SetSplice(IN PVOID OrigFunc, IN PVOID HookFunc, IN ULONG FuncPrologueSize,
				OUT PORIG_FUNC_CALL pOrigFuncCall, OUT PNEARJUMP pOrigFuncPrologue, OUT PNEARJUMP pNewFuncPrologue OPTIONAL)
{
	INIT_ORIG_FUNC_CALL((*pOrigFuncCall), ((PCHAR)OrigFunc+FuncPrologueSize));
	memcpy(&pOrigFuncCall->m_Nops, OrigFunc, FuncPrologueSize);
	memcpy(pOrigFuncPrologue, OrigFunc, sizeof(NEARJUMP));

	NEARJUMP NearJump;
	INIT_NEARJUMP(NearJump, HookFunc);
	if (pNewFuncPrologue)
		*pNewFuncPrologue = NearJump;

	DWORD OldProtect;
	if (VirtualProtect(OrigFunc, MAX_PROLOGUE_SIZE, PAGE_EXECUTE_READWRITE, &OldProtect))
	{
		memcpy(OrigFunc, &NearJump, sizeof(NEARJUMP));
		VirtualProtect(OrigFunc, MAX_PROLOGUE_SIZE, OldProtect, &OldProtect);
		FlushInstructionCache(GetCurrentProcess(), OrigFunc, MAX_PROLOGUE_SIZE);

		return TRUE;
	}
	else
		KDPrint("kd3dll.dll: failed to VirtualProtect for PID:%d", GetCurrentProcessId());

	return FALSE;
}

BOOL RemoveSplice(IN PVOID OrigFunc, IN PNEARJUMP pOrigFuncPrologue)
{
	DWORD OldProtect;
	if (VirtualProtect(OrigFunc, MAX_PROLOGUE_SIZE, PAGE_EXECUTE_READWRITE, &OldProtect))
	{
		memcpy(OrigFunc, pOrigFuncPrologue, sizeof(NEARJUMP));
		VirtualProtect(OrigFunc, MAX_PROLOGUE_SIZE, OldProtect, &OldProtect);
		FlushInstructionCache(GetCurrentProcess(), OrigFunc, MAX_PROLOGUE_SIZE);

		return TRUE;
	}

	return FALSE;
}


ULONG SelectPrologueVariant(IN PVOID pFunc, IN PSPLICE_ITEM pSpliceItem)
{
	for (ULONG i = 0; i < pSpliceItem->m_NumPrologueVariants; i++)
	{
		if (memcmp(pFunc, pSpliceItem->m_PrologueVariants[i].m_CheckFuncPrologue, pSpliceItem->m_PrologueVariants[i].m_PrologueLen) == 0)
			return i;
	}

	return -1L;
}

VOID SetModuleSplice(IN HMODULE hModule, IN PSPLICE_ITEM pSpliceItem)
{
	pSpliceItem->m_bSpliceSet = FALSE;

	PVOID pFunc = GetProcAddress(hModule, pSpliceItem->m_FuncName);
	if (pFunc)
	{
		pSpliceItem->m_prFunc = pFunc;

		ULONG PrologueVariant = SelectPrologueVariant(pFunc, pSpliceItem);
		if (PrologueVariant != -1L)
		{
			pSpliceItem->m_PrologueLen = pSpliceItem->m_PrologueVariants[PrologueVariant].m_PrologueLen;
			pSpliceItem->m_bSpliceSet = SetSplice(pFunc, pSpliceItem->m_pHookFunc, pSpliceItem->m_PrologueLen, 
				&pSpliceItem->m_rFuncCall, &pSpliceItem->m_rFuncPrologue, &pSpliceItem->m_NewFuncPrologue);
		}
	}
}

VOID RemoveModuleSplice(IN PSPLICE_ITEM pSpliceItem)
{
	if (pSpliceItem->m_bSpliceSet)
		RemoveSplice(pSpliceItem->m_prFunc, &pSpliceItem->m_rFuncPrologue);
}

VOID RestoreSplice(IN PSPLICE_ITEM pSpliceItem)
{
	SetSplice(pSpliceItem->m_prFunc, pSpliceItem->m_pHookFunc, pSpliceItem->m_PrologueLen, &pSpliceItem->m_rFuncCall, &pSpliceItem->m_rFuncPrologue,
		&pSpliceItem->m_NewFuncPrologue);
}

VOID CheckRepatchFunc(IN PSPLICE_ITEM pSpliceItem)
{
	if (pSpliceItem->m_bSpliceSet)
	{
		if (memcmp(pSpliceItem->m_prFunc, &pSpliceItem->m_NewFuncPrologue, sizeof(NEARJUMP)) != 0)
		{
			RestoreSplice(pSpliceItem);
			KDPrint("kd3dll.dll: splice removed from %s, repatching\n", pSpliceItem->m_FuncName);
		}
	}
}

BOOL SetSplices()
{
	HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
	if (!hUser32)
		return FALSE;

	SetModuleSplice(hUser32, &gGetMessageA_Splice);
	SetModuleSplice(hUser32, &gGetMessageW_Splice);
	SetModuleSplice(hUser32, &gPeekMessageA_Splice);
	SetModuleSplice(hUser32, &gPeekMessageW_Splice);

	return gGetMessageA_Splice.m_bSpliceSet && gGetMessageW_Splice.m_bSpliceSet &&
		gPeekMessageA_Splice.m_bSpliceSet && gPeekMessageW_Splice.m_bSpliceSet;
}

void RemoveSplices()
{
	RemoveModuleSplice(&gGetMessageA_Splice);
	RemoveModuleSplice(&gGetMessageW_Splice);
	RemoveModuleSplice(&gPeekMessageA_Splice);
	RemoveModuleSplice(&gPeekMessageW_Splice);
}

HANDLE g_hPatchCheckThread = NULL;
volatile BOOL g_bPatchCheck_Terminate = FALSE;

DWORD WINAPI PatchCheckThread(PVOID Context)
{
	while (!g_bPatchCheck_Terminate)
	{
		CheckRepatchFunc(&gGetMessageA_Splice);
		CheckRepatchFunc(&gGetMessageW_Splice);
		CheckRepatchFunc(&gPeekMessageA_Splice);
		CheckRepatchFunc(&gPeekMessageW_Splice);

		Sleep(200);
	}

	return 0;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			if (!KDPrintInit())
				return FALSE;

			if (SetSplices())
			{
				KDPrint("kd3dll.dll attached to PID:%d\n", GetCurrentProcessId());

				g_bPatchCheck_Terminate = FALSE;
				g_hPatchCheckThread = CreateThread(NULL, 0, PatchCheckThread, NULL, 0, NULL);
				if (g_hPatchCheckThread)
					return TRUE;

				RemoveSplices();
				return FALSE;
			}
			else
			{
				RemoveSplices();
				KDPrintUnInit();
			}

			return FALSE;
		}
	case DLL_PROCESS_DETACH:
		{
			g_bPatchCheck_Terminate = TRUE;
			DWORD Reason = WaitForSingleObject(g_hPatchCheckThread, 1000);
			if (Reason == WAIT_TIMEOUT)
				TerminateThread(g_hPatchCheckThread, 0);
			
			RemoveSplices();

			while (gSysEnters);
			Sleep(1000);

			KDPrint("kd3dll.dll detached from PID:%d\n", GetCurrentProcessId());		

			KDPrintUnInit();
		}
	}

    return TRUE;
}

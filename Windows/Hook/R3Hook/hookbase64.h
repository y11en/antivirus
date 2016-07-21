#ifndef _HOOKBASE64_H_
#define _HOOKBASE64_H_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

extern HMODULE g_hHookAwareModule; // module that implements hooks and which imports/exports will not be hooked

typedef struct tag_HOOKUSERDATA {
	DWORD ID; // Specifies type of data in follows structure(s)
	void  (__cdecl *destructor)(struct tag_HOOKUSERDATA*); // destructor for data (NULL if not required)
} tHOOKUSERDATA, *PHOOKUSERDATA;

BOOL HookBaseInit();
VOID HookBaseDone();

DWORD PatchDWORD(DWORD* lpAddress, DWORD dwNewValue);
//PHOOK     CreateRTHook(void* pFuncAddress, void* pHookFunc);
BOOL HookObjectMethod(void* pObject, DWORD Method, void* pHookFunc, tHOOKUSERDATA* pHookUserData); 
BOOL HookObjectMethodIfNotHookedBy(void* lpObject, DWORD Method, void* pHookFunc, tHOOKUSERDATA* pHookUserData);

#ifdef _DEBUG
BOOL HookAddress(void* pAddress, void* pBase, void* pHookFunc, tHOOKUSERDATA* pHookUserData);
BOOL HookInterface(void* pHookFunc, tHOOKUSERDATA* pHookUserData, DWORD Interface, DWORD numMethods, CHAR* InterfaceName, ...);
BOOL HookJmpTable(void* pHookFunc, tHOOKUSERDATA* pHookUserData, void* pJmpTableAddress, DWORD TableLen, CHAR* TableName, ...);
#endif
BOOL HookJmpTableAddress(void* pTableAddress, DWORD dwOffsetInTable, void* pHookFunc, tHOOKUSERDATA* pHookUserData);

BOOL UnHook(void* pHookFunc);
BOOL UnHookAll();
BOOL IsHooked(void* pHookFunc);
//PHOOK     GetHookByOrigFunc(void* OrigFunc);
BOOL IsAddressHooked(void* HookAddr);
BOOL IsAddressHookedBy(void* HookAddr, void* pHookFunc);
BOOL IsMethodHookedBy(void* HookObj, DWORD MethodNum, void* pHookFunc);

void* HookGetOrigFunc();
void* HookGetRetAddr();
tHOOKUSERDATA* HookGetUserData();

// =========================================================================================================
//  HOOK EXPORTS/IMPORTS
// =========================================================================================================

#define FLAG_HOOK_IMPORT   1
#define FLAG_HOOK_EXPORT   2

typedef struct tag_HOOK_TBL_ENTRY {
	CHAR* szFuncName;
	CHAR* szFuncModule;
	void* HookProc;
	tHOOKUSERDATA* pHookUserData;
} tHOOK_TBL_ENTRY;

#define LAST_HOOK_TBL_ENTRY {0,0,0,0}

BOOL HookImportInModule(HMODULE hModule, CHAR* szFuncName, void* HookProc,  tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookExportInModule(HMODULE hModule, CHAR* szFuncName, void* pHookFunc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookProcInProcess(CHAR* szFuncName, void* HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookImportInModuleEx(HMODULE hModule, CHAR* szFuncName, CHAR* szFuncModule, void* HookProc,  tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookExportInModuleEx(HMODULE hModule, CHAR* szFuncName, CHAR* szFuncModule, void* pHookFunc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookProcInProcessEx(CHAR* szFuncName, CHAR* szFuncModule, void* HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookImportInModuleTbl(HMODULE hModule, tHOOK_TBL_ENTRY* pHookTable, DWORD dwFlags);
BOOL HookExportInModuleTbl(HMODULE hModule, tHOOK_TBL_ENTRY* pHookTable, DWORD dwFlags);
BOOL HookProcInProcessTbl(tHOOK_TBL_ENTRY* pHookTable, DWORD dwFlags);


// ======================================================================================
//		Splice hooks
// ======================================================================================

#include <pshpack1.h>
typedef struct _NEAR_JUMP
{
	USHORT		m_JmpNear;		// jmp near opcode == ox25ff
	ULONG		m_Displacement;	// 32 - absolute jmp address / 64 - rip-relative addressing
	void*       m_Address;		// 64 - jmp address
} NEAR_JUMP, *PNEAR_JUMP;
#include <poppack.h>

typedef struct _SPLICE_CONTEXT
{
	PVOID		m_OriginalFunc;				// адрес оригинальной ф-и
	PVOID		m_HookFunc;					// адрес ф-и - перехватчика
	PVOID		m_Thunk;					// переходник
	ULONG		m_nPrologueBytes;		// сколько байт пролога переносим
	NEAR_JUMP   m_JmpHook;
} SPLICE_CONTEXT, *PSPLICE_CONTEXT;

BOOL SpliceHook(IN PVOID OriginalFunc, IN PVOID HookFunc, OUT PSPLICE_CONTEXT *p_pContext);
BOOL SpliceUnhook(PSPLICE_CONTEXT pContext);

#ifdef __cplusplus
} // extern "C" 
#endif

//#define GET_RET_ADDR(first_arg)  *((void**)&(first_arg) - 1) -- BAD: NEW COMPILER MOVES ARGS INTO FRAME!
// USE HookGetRetAddr() INSTEAD!

#endif // _HOOKBASE64_H_
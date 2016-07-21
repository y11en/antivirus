#ifndef _HOOKBASE_H_
#define _HOOKBASE_H_

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
//PHOOK     CreateRTHook(LPVOID pFuncAddress, LPVOID pHookFunc);
BOOL HookObjectMethod(LPVOID pObject, DWORD Method, LPVOID HookFunc, tHOOKUSERDATA* pHookUserData); 
BOOL HookObjectMethodIfNotHookedBy(LPVOID lpObject, DWORD Method, LPVOID HookFunc, tHOOKUSERDATA* pHookUserData);

#ifdef _DEBUG
BOOL HookAddress(LPVOID lpAddress, HMODULE hModule, LPVOID HookFunc, tHOOKUSERDATA* pHookUserData);
BOOL HookInterface(LPVOID HookFunc, tHOOKUSERDATA* pHookUserData, DWORD Interface, DWORD numMethods, CHAR* InterfaceName, ...);
BOOL HookJmpTable(LPVOID HookFunc, tHOOKUSERDATA* pHookUserData, LPVOID pJmpTableAddress, DWORD TableLen, CHAR* TableName, ...);
#endif
BOOL HookJmpTableAddress(LPVOID pTableAddress, DWORD dwOffsetInTable, LPVOID HookFunc, tHOOKUSERDATA* pHookUserData);

BOOL UnHook(LPVOID HookFunc);
BOOL UnHookAll();
BOOL IsHooked(LPVOID HookFunc);
//PHOOK     GetHookByOrigFunc(LPVOID OrigFuncAddr);
BOOL IsAddressHooked(LPVOID HookAddr);
BOOL IsAddressHookedBy(LPVOID HookAddr, LPVOID HookFunc);
BOOL IsMethodHookedBy(LPVOID HookObj, DWORD MethodNum, LPVOID HookFunc);

// =========================================================================================================
//  HOOK EXPORTS/IMPORTS
// =========================================================================================================

#define FLAG_HOOK_IMPORT   1
#define FLAG_HOOK_EXPORT   2

typedef struct tag_HOOK_TBL_ENTRY {
	CHAR* szFuncName;
	LPVOID HookProc;
	tHOOKUSERDATA* pHookUserData;
} tHOOK_TBL_ENTRY;

#define LAST_HOOK_TBL_ENTRY {0,0,0}

BOOL HookImportInModule(HMODULE hModule, CHAR* szFuncName, LPVOID HookProc,  tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookExportInModule(HMODULE hModule, CHAR* szFuncName, LPVOID pHookFunc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookProcInProcess(CHAR* szFuncName, LPVOID HookProc, tHOOKUSERDATA* pHookUserData, DWORD dwFlags);
BOOL HookImportInModuleTbl(HMODULE hModule, tHOOK_TBL_ENTRY* pHookTable, DWORD dwFlags);
BOOL HookExportInModuleTbl(HMODULE hModule, tHOOK_TBL_ENTRY* pHookTable, DWORD dwFlags);
BOOL HookProcInProcessTbl(tHOOK_TBL_ENTRY* pHookTable, DWORD dwFlags);

// =========================================================================================================
//  DEBUG TRACE
// =========================================================================================================

#if !defined(__FILE__LINE__)
	#define _QUOTE(x) #x
	#define QUOTE(x) _QUOTE(x)

	// Use __FILE__LINE__ as a string containing "File.x(line#)" for example:
	//     #pragma message( __FILE__LINE__ "Remove this line after testing.")
	#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
	// The above example will display the file name and line number in the build window
	// in such a way as to allow you to double click on it to go to the line.
#endif // __FILE__LINE__

#define _FL_ __FILE__LINE__ ": "

#if defined(ODS) && defined(_DEBUG) && !defined(_HOOKBASE_IMP_)
//#define   UnHook(HookFunc)	(ODS((_FL_ "UnHook(%s)", #HookFunc)), UnHook(HookFunc))
#define   UnHookAll()	(ODS((_FL_ "UnHookAll()", __FILE__, __LINE__)), UnHookAll())
//#define   IsHooked(HookFunc)	(ODS((_FL_ "IsHooked(%s)=%s", #HookFunc, (IsHooked(HookFunc)?"TRUE":"FALSE"))), IsHooked(HookFunc))
//#define   IsAddressHooked(HookAddr)	(ODS((_FL_ "IsAddressHooked(%s)=%s", #HookAddr, (IsAddressHooked(HookAddr)?"TRUE":"FALSE"))), IsAddressHooked(HookAddr))
//#define   IsAddressHookedBy(HookAddr, HookFunc)	(ODS((_FL_ "IsAddressHookedBy(%s, %s)=%s", #HookAddr, #HookFunc, (IsAddressHookedBy(HookAddr,HookFunc)?"TRUE":"FALSE"))), IsAddressHookedBy(HookAddr,HookFunc))
//#define   IsMethodHookedBy(HookObj, MethodNum, HookFunc)	(ODS((_FL_ "IsMethodHookedBy(%s, %s, %s)=%s", #HookObj, #MethodNum, #HookFunc, (IsMethodHookedBy(HookObj,MethodNum,HookFunc)?"TRUE":"FALSE"))), IsMethodHookedBy(HookObj,MethodNum,HookFunc))
//#define   HookObjectMethodIfNotHookedBy(HookObj, MethodNum, HookFunc, pHookUserData)	(ODS((_FL_ "HookObjectMethodIfNotHookedBy(%s, %s, %s, %s)", #HookObj, #MethodNum, #HookFunc, #pHookUserData)), HookObjectMethodIfNotHookedBy(HookObj,MethodNum,HookFunc,pHookUserData))
#endif

#ifdef __cplusplus
} // extern "C" 
#endif


#endif // _HOOKBASE_H_
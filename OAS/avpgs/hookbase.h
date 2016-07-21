#ifndef _HOOKBASE_H_
#define _HOOKBASE_H_

#include <windows.h>
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HOOK_METHOD_IF_NOT_HOOKED(ObjID, MethID, HookFunc, ID, PID) \
	if (!IsHooked(HookFunc)) { \
	pHookInfo = HookObjectMethod(ObjID, MethID, HookFunc); \
	if (pHookInfo != NULL) { \
	pHookInfo->m_dwRuleID = IDS_T_##ID; \
	pHookInfo->m_dwFuncDescr = IDS_D_##ID; \
	pHookInfo->m_dwFuncParamFmt = IDS_F_##ID; \
	pHookInfo->m_dwFlags = HI_DWDATA | HI_DWDATARULEID; \
	}  else DBG_STOP; \
	};\

#define HOOK_EXPFUNC_IF_NOT_HOOKED(hModule, FunName, HookFunc, ID, PID) \
	{pHookInfo = HookExportInModule(hModule, FunName, HookFunc, 0); \
	if (pHookInfo != NULL) { \
	pHookInfo->m_dwRuleID = IDS_T_##ID; \
	pHookInfo->m_dwFuncDescr = IDS_D_##ID;  \
	pHookInfo->m_dwFuncParamFmt = IDS_F_##ID; \
	pHookInfo->m_dwFlags = HI_DWDATA | HI_DWDATARULEID; \
	} else { DBG_STOP; bExportsHooked = FALSE; }\
	}; \

typedef struct _HOOK HOOK, *PHOOK;

#define HI_DWDATA			0x01
#define HI_SZDATA			0x02
#define HI_SZDATA_ALLOCATED	0x04
#define HI_DWDATARULEID		0x08
#define HI_FMT_FROM_PTR		0x10
#define HI_JMP_HOOK			0x20
#define HI_DONT_FREE_ON_RELEASE	0x40

#define HI_FTYPE_FLOAT		1
#define HI_FTYPE_OBJECT		2

typedef struct _HOOKINFO {
	DWORD m_dwFuncParamFmt;
	DWORD m_dwRuleID;
	DWORD m_dwFuncDescr;
	DWORD m_dwFuncFlags;
	DWORD m_dwFlags;
	DWORD m_dwFwdFuncAddr;
	char* m_szIFaceName;
} HOOKINFO, *PHOOKINFO;

BOOL      HookBaseInit();
VOID      HookBaseDone();

DWORD     PatchDWORD(DWORD* lpAddress, DWORD dwNewValue);
PHOOK     CreateRTHook(LPVOID pFuncAddress, LPVOID pHookFunc);
PHOOKINFO HookObjectMethod(LPVOID pObject, DWORD Method, LPVOID HookFunc); 
PHOOKINFO HookObjectMethodIfNotHookedBy(LPVOID lpObject, DWORD Method, LPVOID HookFunc);

PHOOKINFO HookAddress(LPVOID lpAddress, LPVOID HookFunc, HMODULE hModule);
VOID      HookInterface(LPVOID HookFunc, DWORD Interface, DWORD numMethods, CHAR* InterfaceName, ...);
VOID      HookJmpTable(LPVOID HookFunc, LPVOID pJmpTableAddress, DWORD TableLen, CHAR* TableName, ...);
PHOOKINFO HookJmpTableAddress(LPVOID pTableAddress, DWORD dwOffsetInTable, LPVOID HookFunc);

BOOL      UnHook(LPVOID HookFunc);
BOOL      UnHookAll();
BOOL      IsHooked(LPVOID HookFunc);
PHOOK     GetHookByOrigFunc(LPVOID OrigFuncAddr);
BOOL      IsAddressHooked(LPVOID HookAddr);
BOOL      IsAddressHookedBy(LPVOID HookAddr, LPVOID HookFunc);
BOOL      IsMethodHookedBy(LPVOID HookObj, DWORD MethodNum, LPVOID HookFunc);

#if !defined(__FILE__LINE__)
	#define _QUOTE(x) #x
	#define QUOTE(x) _QUOTE(x)

	// Use __FILE__LINE__ as a string containing "File.x(line#)" for example:
	//     #pragma message( __FILE__LINE__ "Remove this line after testing.")
	#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
	// The above example will display the file name and line number in the build window
	// in such a way as to allow you to double click on it to go to the line.
#endif // __FILE__LINE__

#define _FL_ //__FILE__LINE__ ": "

#if defined(_DEBUG) && !defined(_HOOKBASE_IMP_)
#define   UnHook(HookFunc)	(PR_TRACE((g_root, prtNOTIFY, _FL_ "UnHook(%s)", #HookFunc)), UnHook(HookFunc))
#define   UnHookAll()	(PR_TRACE((g_root, prtNOTIFY, _FL_ "UnHookAll()", __FILE__, __LINE__)), UnHookAll())
#define   IsHooked(HookFunc)	(PR_TRACE((g_root, prtNOTIFY, _FL_ "IsHooked(%s)=%s", #HookFunc, (IsHooked(HookFunc)?"TRUE":"FALSE"))), IsHooked(HookFunc))
#define   IsAddressHooked(HookAddr)	(PR_TRACE((g_root, prtNOTIFY, _FL_ "IsAddressHooked(%s)=%s", #HookAddr, (IsAddressHooked(HookAddr)?"TRUE":"FALSE"))), IsAddressHooked(HookAddr))
#define   IsAddressHookedBy(HookAddr, HookFunc)	(PR_TRACE((g_root, prtNOTIFY, _FL_ "IsAddressHookedBy(%s, %s)=%s", #HookAddr, #HookFunc, (IsAddressHookedBy(HookAddr,HookFunc)?"TRUE":"FALSE"))), IsAddressHookedBy(HookAddr,HookFunc))
#define   IsMethodHookedBy(HookObj, MethodNum, HookFunc)	(PR_TRACE((g_root, prtNOTIFY, _FL_ "IsMethodHookedBy(%s, %s, %s)=%s", #HookObj, #MethodNum, #HookFunc, (IsMethodHookedBy(HookObj,MethodNum,HookFunc)?"TRUE":"FALSE"))), IsMethodHookedBy(HookObj,MethodNum,HookFunc))
//#define   HookObjectMethodIfNotHookedBy(HookObj, MethodNum, HookFunc)	(PR_TRACE((g_root, prtNOTIFY, _FL_ "HookObjectMethodIfNotHookedBy(%s, %s, %s)", #HookObj, #MethodNum, #HookFunc)), HookObjectMethodIfNotHookedBy(HookObj,MethodNum,HookFunc))
#endif


#ifdef __cplusplus
} // extern "C" 
#endif


#endif // _HOOKBASE_H_
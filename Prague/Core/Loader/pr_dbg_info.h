// pr_dbg_info.h
//
//

#ifndef pr_dbg_info_h_INCLUDED
#define pr_dbg_info_h_INCLUDED

#include <Prague/prague.h>

#define SKIP_THIS_MODULE "this_module"

#define DBGINFO_CALLER_ADDR	0
#define DBGINFO_MODULE_NAME	1
#define DBGINFO_IMAGE_NAME	2
#define DBGINFO_SYMBOL_NAME	3
#define DBGINFO_SRC_FILE	4
#define DBGINFO_SRC_LINE	5

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(_DEBUG) && !defined(USE_DBG_INFO)

	#define PrDbgInit()
	#define PrDbgDone()
	#define PrDbgLoadModule(hModule)
	//#define PrDbgGetCallerInfo(szSkipModulesList, dwSkipLevels, pBuffer, dwSize)
	//#define PrDbgGetInfoEx(dwAddress, szSkipModulesList, dwSkipLevels, dwInfoType, pBuffer, dwSize, pFFrame)
		
#else // _DEBUG

	tERROR PrDbgInit();
	tERROR PrDbgDone();
	tERROR PrDbgLoadModule(HANDLE hModule);
	//tERROR PrDbgGetCallerInfo(tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tCHAR* pBuffer, tDWORD dwSize);
	//tERROR PrDbgGetInfoEx(tDWORD dwAddress, tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tDWORD dwInfoType, tVOID* pBuffer, tDWORD dwSize, tDWORD* pFFrame);
	
#endif // _DEBUG	

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // pr_dbg_info_h_INCLUDED


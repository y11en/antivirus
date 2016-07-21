
#define _DBG_LIBRARY

#include <windows.h>
#include <tchar.h>
#include "Debug.h"
#include "..\..\..\kavpf\Debug.h"


#pragma data_seg("D_INFO")
DEBUG_API ULONG g_DbgLevel = 0;
DEBUG_API ULONG g_DbgMask = (ULONG)-1;
#pragma data_seg()

#pragma comment(linker, "/Section:D_INFO,RWS")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		break;
	
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


DEBUG_API BOOL GET_MODULE_INFO(OUT TCHAR** szModuleName, OUT TCHAR*** szDbgLevels, OUT TCHAR*** szCategories, OUT TCHAR*** szCatComments){

	*szModuleName = (char*)ModuleName;
	*szDbgLevels = (char**)DebugLevelName;
	*szCategories = (char**)DebCatName;
	
	return TRUE;
}

DEBUG_API BOOL TURN_DBG_SETTINGS(IN BOOL bSetGet, IN OUT DWORD* pdwLevel, IN OUT DWORD* pdwMask){

	//set debug settings
	if(bSetGet){
		g_DbgLevel = *pdwLevel;
		g_DbgMask = *pdwMask;
	}else{
		*pdwLevel = g_DbgLevel;
		*pdwMask = g_DbgMask;
	}
	return TRUE;
}


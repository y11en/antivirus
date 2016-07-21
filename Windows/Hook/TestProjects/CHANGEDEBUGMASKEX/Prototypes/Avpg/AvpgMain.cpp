#include <windows.h>
#include <tchar.h>
#include "..\..\Common\Debug.h"
#include <hook\debug.h>
#include <hook\FSDrvLib.h>
#include <hook\IDriver.h>
#include <hook\hookspec.h>


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.
		if (FSDrvInterceptorInit() == FALSE)
		{
			MessageBox(NULL, "Error", "Init avpg driver failed! Trying tsp...", MB_ICONINFORMATION);
			if (FSDrvInterceptorInitTsp() == FALSE)
			{
				MessageBox(NULL, "Error", "Init driver failed!\n", MB_ICONINFORMATION);
				return FALSE;
			}
		}break;
	
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			// Since the dialog has been closed, return FALSE so that we exit the
			//  application, rather than start the application's message pump.
			FSDrvInterceptorDone();
					
			break;
    }
    return TRUE;
}


DEBUG_API BOOL GET_MODULE_INFO(OUT TCHAR** szModuleName, OUT TCHAR*** szDbgLevels, OUT TCHAR*** szCategories, OUT TCHAR*** szCatComments){

	static TCHAR ModuleName[] = _T("Avpg");
	*szModuleName = ModuleName;
	*szDbgLevels = DebugLevelName;
	*szCategories = DebCatName;
	return TRUE;
}

DEBUG_API BOOL TURN_DBG_SETTINGS(IN BOOL bSetGet, IN OUT DWORD* pdwLevel, IN OUT DWORD* pdwMask){

	HANDLE hDriver = FSDrvGetDeviceHandle();
	//set debug settings
	if(bSetGet){
		IDriverChangeDebugInfoLevel(hDriver, *pdwLevel);
		IDriverChangeDebugInfoMask(hDriver, *pdwMask);
	}else{
		IDriverGetDebugInfoMask(hDriver, pdwMask );
		IDriverGetDebugInfoLevel(hDriver, pdwLevel);
	}
	return TRUE;
}



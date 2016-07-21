#include <windows.h>
#include <tchar.h>
#include <hook\FSDrvLib.h>
#include <hook\IDriver.h>
#include <hook\hookspec.h>

#include "..\..\Common\Debug.h"
#include <Hook\PIDExternalAPI.H>
#include "..\..\..\..\..\Personal Firewall\Drivers\Pid_Hook\klDebug.h"



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

	static TCHAR ModuleName[] = _T("PID_Hook");

	*szModuleName = ModuleName;
	*szDbgLevels = KlpidDebugLevelName;
	*szCategories = KlpidDebCatName;
	return TRUE;
}

DEBUG_API BOOL TURN_DBG_SETTINGS(IN BOOL bSetGet, IN OUT DWORD* pdwLevel, IN OUT DWORD* pdwMask){

	HANDLE hDriver = FSDrvGetDeviceHandle();
	CHAR buff[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DebugExchange)];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) buff;
	pRequest->m_DrvID = FLTTYPE_PID;
	pRequest->m_BufferSize = sizeof(DebugExchange);
	((PDebugExchange) pRequest->m_Buffer)->m_uID = FLTTYPE_PID;



	//set debug settings
	if(bSetGet){
		pRequest->m_IOCTL = IOCTL_SET_DEBUG_LEVEL;
		((PDebugExchange) pRequest->m_Buffer)->m_uDbgValue = *pdwLevel;
		IDriverExternalDrvRequest(hDriver, pRequest, pRequest);

		((PDebugExchange) pRequest->m_Buffer)->m_uDbgValue = *pdwMask;			
		pRequest->m_IOCTL = IOCTL_SET_DEBUG_MASK;
		IDriverExternalDrvRequest(hDriver, pRequest, pRequest);
	}else{
		pRequest->m_IOCTL = IOCTL_GET_DEBUG_LEVEL;
		if (IDriverExternalDrvRequest(hDriver, pRequest, pRequest) == TRUE)
			*pdwLevel = ((PDebugExchange) pRequest->m_Buffer)->m_uDbgValue;
		else
			MessageBox(NULL, "Get debug level for KLPID failed!","Error", MB_ICONINFORMATION);
		
		pRequest->m_IOCTL = IOCTL_GET_DEBUG_MASK;
		if (IDriverExternalDrvRequest(hDriver, pRequest, pRequest) == TRUE)
			*pdwMask = ((PDebugExchange) pRequest->m_Buffer)->m_uDbgValue;
		else
			MessageBox(NULL, "Get debug mask for KLPID failed!", "Error", MB_ICONINFORMATION);
	}
	return TRUE;
}

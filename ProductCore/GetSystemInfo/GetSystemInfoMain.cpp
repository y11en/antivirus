// GetSystemInfo.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <ProductCore/GetSystemInfoMain.h>
#include "GetSystemInfo.h"

extern OSVERSIONINFOEX	versionInfo;
extern bool				ExType;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ExType=true;
		memset(&versionInfo,0,sizeof(OSVERSIONINFOEX));
		SysInfoInit();
		break;
	case DLL_PROCESS_DETACH:
		SysInfoDeInit();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
    }
    return TRUE;
}

// MAPIEDK.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MAPIEDK.h"
#include <mapiguid.h>
#include <edkutils.h>




BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


CMAPIEDK g_MAPIEDK;

bool __stdcall MAPIEDK_Init(OUT CMAPIEDK*& pMAPIEDK)
{
	static bool bInited = false;
	if ( bInited ) 
	{
		pMAPIEDK = &g_MAPIEDK;
		return true;
	}

	if ( g_MAPIEDK.Init() ) 
	{
		bInited = true;
		pMAPIEDK = &g_MAPIEDK;
		return true;
	}
	
	return false;
}


bool CMAPIEDK::Init()
{
	HMODULE hLib = FindMAPIModule();
	if (!hLib)
		return false;

	bool bInitResult = true;
	
	m_pMAPIInitialize = (MAPIInitialize_t *)GetProcAddress(hLib, "MAPIInitialize");
	bInitResult &= m_pMAPIInitialize != NULL;

	MAPIEDK_FUNC_INIT_FROM_MODULE( hLib, bInitResult, MAPIAdminProfiles		);
	//MAPIEDK_FUNC_INIT_FROM_MODULE( hLib, bInitResult, MAPIInitialize		);
	MAPIEDK_FUNC_INIT_FROM_MODULE( hLib, bInitResult, MAPIUninitialize		);
	MAPIEDK_FUNC_INIT_FROM_MODULE( hLib, bInitResult, MAPILogonEx			);
	MAPIEDK_FUNC_INIT_FROM_MODULE( hLib, bInitResult, MAPIAllocateBuffer	);
	MAPIEDK_FUNC_INIT_FROM_MODULE( hLib, bInitResult, MAPIAllocateMore		);
	MAPIEDK_FUNC_INIT_FROM_MODULE( hLib, bInitResult, MAPIFreeBuffer		);
	
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, CreateIProp, CreateIProp@24 );
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, HrQueryAllRows, HrQueryAllRows@24	);
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, UlRelease, UlRelease@4 );
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, FPropExists, FPropExists@8 );
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, FreePadrlist, FreePadrlist@4 );
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, FreeProws, FreeProws@4 );
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, ScDupPropset, ScDupPropset@16	);

	
	MAPIEDK_FUNC_INIT( HrMAPIGetPropBinary );
	MAPIEDK_FUNC_INIT( HrMAPIGetPropBoolean	);
	MAPIEDK_FUNC_INIT( HrMAPIGetPropString );
	MAPIEDK_FUNC_INIT( HrMAPIGetPropLong );
	
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, HrGetOneProp, HrGetOneProp@12 );

	MAPIEDK_FUNC_INIT( HrMAPISetPropBinary );
	MAPIEDK_FUNC_INIT( HrMAPISetPropBoolean	);
	MAPIEDK_FUNC_INIT( HrMAPISetPropString );
	MAPIEDK_FUNC_INIT( HrMAPISetPropLong );
	MAPIEDK_FUNC_INIT( HrMAPIDeleteMessage );
	MAPIEDK_FUNC_INIT( HrMAPIMoveMessage );
	MAPIEDK_FUNC_INIT( HrMAPICopyMessage );
	MAPIEDK_FUNC_INIT( HrMAPIGetPropSystime );
	MAPIEDK_FUNC_INIT( HrMAPIOpenFolderEx );
	MAPIEDK_FUNC_INIT( HrMAPICreateSizedAddressList );
	MAPIEDK_FUNC_INIT( HrMAPISetAddressList );
	MAPIEDK_FUNC_INIT( HrOpenExchangePrivateStore );
	MAPIEDK_FUNC_INIT( HrMAPIFindOutbox );
	MAPIEDK_FUNC_INIT( HrMAPICreateAddressList );

	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, LpValFindProp, LpValFindProp@12 );
	MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME( hLib, bInitResult, PpropFindProp, PpropFindProp@12 );

	MAPIEDK_FUNC_INIT( HrMAPIFindDefaultMsgStore );
	MAPIEDK_FUNC_INIT( HrMAPIFindInbox );

	if (!bInitResult)
	{
		FreeLibrary(hLib);
	}

	return bInitResult;
}


// пробуем найти модуль с достаточной реализацией MAPI
// проблема возникла из-за подмены mapi32.dll почтовым клиентом The Bat на Simple MAPI
// настоящая библиотека бекапится в mapi32tb.dll
HMODULE CMAPIEDK::FindMAPIModule()
{
	HMODULE hLib = NULL;
	
	hLib = LoadLibraryEx("mapi32.dll", NULL, 0);

	if (hLib)
	{
		// посмотрим, присутствует ли например функция MAPILogonEx
		MAPILogonEx_t * pFunc = (MAPILogonEx_t *)GetProcAddress(hLib, "MAPILogonEx");
		if (pFunc)
		{
			// считаем, что загружена нужная версия MAPI библиотеки
			return hLib;
		}

		FreeLibrary(hLib);
	}

	hLib = LoadLibraryEx("mapi32tb.dll", NULL, 0);

	if (hLib)
	{
		// посмотрим, присутствует ли например функция MAPILogonEx
		MAPILogonEx_t * pFunc = (MAPILogonEx_t *)GetProcAddress(hLib, "MAPILogonEx");
		if (pFunc)
		{
			// считаем, что загружена нужная версия MAPI библиотеки
			return hLib;
		}

		FreeLibrary(hLib);
	}

	return NULL;
}

HRESULT CMAPIEDK::HrLog(IN LPSTR lpszFile, IN ULONG ulLine, IN HRESULT hr, IN DWORD dwLastError)
{
	// not implemented
	return hr;
}

BOOL CMAPIEDK::HrFailed(IN LPSTR lpszFile, IN ULONG ulLine, IN HRESULT hr, IN DWORD dwLastError)
{
	// not implemented
	return (hr >= 0 ? FALSE : TRUE);
}

void CMAPIEDK::Assert(IN LPSTR lpszTag, IN LPSTR lpszFile, IN ULONG ulLine, IN DEBUGLEVEL Level, IN BOOL fValue, IN LPSTR lpszFormat, ...)
{
	// not implemented
}



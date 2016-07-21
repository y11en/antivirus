/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	mcou.cpp
*		\brief	Mail Checker Outlook plugin main module
*		
*		\author Nick Lebedev
*		\date	19.06.2003 15:25:52
*		
*		Example:	
*		
*		
*		
*/		


#include "stdafx.h"
#include "mcou.h"
#include "Register.h"
#include "util.h"
#include "interface.h"
#include "InitTerm.h"

namespace MCOU
{
	HINSTANCE g_hInstance, g_hResInstance;

	BOOL g_bInitSucceeded = FALSE;
	
	tstring g_sProductName;

	void *g_pvMALContext = NULL;

	CTimeBases g_TimeBases;

#ifdef _DEBUG
	BOOL g_bNoMarkMessages;
#endif
}

#include <atlbase.h>
CComModule _Module;
#include <atlcom.h>
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////
//
// DLL entry
//
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		MCOU::g_hInstance = (HINSTANCE)hModule;
        _Module.Init(ObjectMap, (HINSTANCE)hModule, NULL);
		return TRUE; //InitPlugin ( (HINSTANCE)hModule );
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		KLSTD_TRACE0 ( MCOU_TL_INFO, L"Outlook plugin terminating!\n" );
        _Module.Term();
		return TRUE; //TermPlugin ();
	}

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// Exported functions
//

//! \fn				: ExchEntryPoint
//! \brief			: Main plugin entry point
//! \return			: LPEXCHEXT CALLBACK 
//! \param          : void
LPEXCHEXT CALLBACK ExchEntryPoint(void)
{
	KLSTD_TRACE0 ( MCOU_TL_INFO, L"ExchEntryPoint called\n" );

	static IMCOUExchExt *g_pIMCOUExchExt = NULL;

	if (g_pIMCOUExchExt == NULL)
		g_pIMCOUExchExt = new IMCOUExchExt;
	else
		g_pIMCOUExchExt->AddRef ();

	return g_pIMCOUExchExt;
}


//! \fn				: DllRegisterServer
//! \brief			: Registers plugin (exported function)
//! \return			: HRESULT
//! \param          : void
HRESULT CALLBACK DllRegisterServer(void)
{
	KLSTD_TRACE0 ( MCOU_TL_INFO, L"DllRegisterServer called\n" );
	TCHAR szThisModuleFileName [MAX_PATH];
	if (MCOU::GetThisModuleFileName (szThisModuleFileName, MAX_PATH) == 0)
		return E_FAIL;

	return RegisterPlugin (szThisModuleFileName);
}


//! \fn				: DllUnregisterServer
//! \brief			: Unregisters plugin (exported function)
//! \return			: HRESULT CALLBACK 
//! \param          : void
HRESULT CALLBACK DllUnregisterServer(void)
{
	KLSTD_TRACE0 ( MCOU_TL_INFO, L"DllUnregisterServer called\n" );
	return UnregisterPlugin();
}

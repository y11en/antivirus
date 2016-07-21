// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  25 July 2006,  18:02 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- plugin_wmihlpr.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end

#include "Prague/iface/i_string.h"
#include <ProductCore/wmihlpr.h>

// AVP Prague stamp begin( Plugin extern and export declaration,  )

#define  IMPEX_EXPORT_LIB
#include <ProductCore/plugin/p_wmihlpr.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <ProductCore/plugin/p_wmihlpr.h>
EXPORT_TABLE_END
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;
hROOT g_root;

extern CRITICAL_SECTION g_csWMI;
extern HANDLE g_hWMISemaphore;
HANDLE g_hWMIUpdateStatusThread = NULL;
extern BOOL g_bExitFlag;

DWORD WINAPI g_fnUpdateWMIStatus(LPVOID lpParam);
bool IsPreXPSP2();

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	tDWORD count;
	hROOT  root;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH :
			break;
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			{
				if(!IsPreXPSP2())
				{
					InitializeCriticalSection(&g_csWMI);
					g_hWMISemaphore = CreateSemaphore(NULL, 0, 1024, NULL);

					DWORD dwID = 0;
					g_bExitFlag = FALSE;
					g_hWMIUpdateStatusThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)g_fnUpdateWMIStatus, NULL, 0, &dwID);
				}

				g_root = root = (hROOT)hInstance;
				*pERROR = errOK;

				//register my exports
				CALL_Root_RegisterExportTable( root, &count, export_table, PID_WMIHLPR );

				//register my custom property ids
				//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
				//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
				//  return cFALSE;
				//}

				// register my interfaces
				//if ( PR_FAIL(*pERROR=SomeInterface_Register(root)) ){
				//  PR_TRACE(( root, prtERROR, "cannot register \"SomeInterface\" interface"));
				//  return cFALSE;
				//}
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			if (g_hWMISemaphore)
			{
				g_bExitFlag = TRUE;
				ReleaseSemaphore(g_hWMISemaphore, 1, NULL);
				if(g_hWMIUpdateStatusThread)
				{
					if( WaitForSingleObject(g_hWMIUpdateStatusThread, 2000) == WAIT_TIMEOUT)
						TerminateThread(g_hWMIUpdateStatusThread, 0);
					CloseHandle(g_hWMIUpdateStatusThread);
					g_hWMIUpdateStatusThread = NULL;
				}

				DeleteCriticalSection(&g_csWMI);
				CloseHandle(g_hWMISemaphore);
				g_hWMISemaphore = NULL;
			}
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin export method implementation, PrWmihInstall )
// Parameters are:
tERROR PrWmihInstall( const tBYTE* p_pguidID, const tCHAR* p_pszLegacyRegKey, const tWCHAR* p_pszCompanyName, const tWCHAR* p_pszProductVersion, const tWCHAR* p_pszDisplayName )
{
	if( FAILED(wmih_Install((LPCGUID)p_pguidID, p_pszLegacyRegKey, p_pszCompanyName, p_pszProductVersion, p_pszDisplayName)) )
		return errUNEXPECTED;
		
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin export method implementation, PrWmihUpdateStatus )
// Parameters are:
tERROR PrWmihUpdateStatus( const tBYTE* p_pguidID, tDWORD p_eComponent, tDWORD p_eStatus )
{
	if( FAILED(wmih_UpdateStatusAsync((LPCGUID)p_pguidID, p_eComponent, p_eStatus)) )
		return errUNEXPECTED;
		
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin export method implementation, PrWmihUninstall )
// Parameters are:
tERROR PrWmihUninstall( const tBYTE* p_pguidID )
{
	if( FAILED(wmih_Uninstall((LPCGUID)p_pguidID)) )
		return errUNEXPECTED;
		
	return errOK;
}
// AVP Prague stamp end


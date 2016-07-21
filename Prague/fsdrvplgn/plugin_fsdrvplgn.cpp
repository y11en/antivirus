// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  22 November 2005,  13:40 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- plugin_fsdrvplgn.cpp
// -------------------------------------------
// AVP Prague stamp end



#include <tchar.h>
#include <stddef.h>


// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )

#define  IMPEX_EXPORT_LIB
#include <Prague/plugin/p_fsdrvplgn.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <Prague/plugin/p_fsdrvplgn.h>
EXPORT_TABLE_END
// AVP Prague stamp end

#include <windows.h>
#include "..\..\windows\hook\mklif\fssync\fssync.h"
#pragma comment( lib, "fssync.lib" )

void*
__cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	)
{	
	void* ptr = NULL;
	ptr = HeapAlloc( GetProcessHeap(), 0, size );
	return ptr; 
};

void __cdecl MemFree(PVOID pOpaquePtr, void** pptr)
{
	if (!*pptr)
		return;
	HeapFree( GetProcessHeap(), 0, *pptr );

	*pptr = NULL;
};

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;
PVOID gpContext = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;
	tDWORD count;

	HRESULT hResult = S_OK;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			hResult = DRV_Register (
				&gpContext,
				AVPG_Driver_Specific,
				AVPG_INFOPRIORITY,
				_CLIENT_FLAG_POPUP_TYPE | 
				_CLIENT_FLAG_WITHOUTWATCHDOG,
				0,
				DEADLOCKWDOG_TIMEOUT,
				MemAlloc,
				MemFree,
				NULL
				);

			if (!SUCCEEDED( hResult ))
				gpContext = NULL;
			break;

		case DLL_PROCESS_DETACH:
			if (gpContext)
			{
				DRV_UnRegister( &gpContext );
			}
			break;

		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_FSDRVPLGN)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			CALL_Root_RegisterExportTable( root, &count, export_table, PID_FSDRVPLGN );

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
			break;

		case PRAGUE_PLUGIN_UNLOAD :
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



// AVP Prague stamp begin( Plugin export method implementation, FSMoveFileEx )
// Parameters are:
tERROR FSMoveFileEx( hSTRING p_strSrc, hSTRING p_strDst, tDWORD p_dwFlags )
{
	// Place your code here
	tERROR error = errNOT_SUPPORTED;
	if (!p_strSrc)
		return errPARAMETER_INVALID;

	if (!gpContext)
		return errUNEXPECTED;

	WCHAR pwchFN1[MAX_PATH * 2];
	WCHAR pwchFN2[MAX_PATH * 2];

	if (p_strSrc)
		error = p_strSrc->ExportToBuff(NULL, cSTRING_WHOLE, pwchFN1, sizeof(pwchFN1), cCP_SYSTEM_DEFAULT, cSTRING_Z);
	if (PR_FAIL( error ))
		return error;

	if (p_strDst)
		error = p_strDst->ExportToBuff(NULL, cSTRING_WHOLE, pwchFN2, sizeof(pwchFN2), cCP_SYSTEM_DEFAULT, cSTRING_Z);
	if (PR_FAIL( error ))
		return error;

	HRESULT hResult = DRV_SpecFileRequest( gpContext, pwchFN1, p_strDst ? pwchFN2 : NULL );
	if (SUCCEEDED( hResult ))
		error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FSGetDriveType )
// Parameters are:
tERROR FSGetDriveType( hSTRING p_strDrive )
{
	// Place your code here
	return errNOT_SUPPORTED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FSGetDiskStat )
// Parameters are:
tERROR FSGetDiskStat( tQWORD* p_pTimeCurrent, tQWORD* p_pTotalWaitTime )
{
	// Place your code here
	if (!gpContext)
		return errUNEXPECTED;

	HRESULT hResult = DRV_GetStatCounter( gpContext, (PULONGLONG) p_pTotalWaitTime, (PULONGLONG) p_pTimeCurrent );
	if (SUCCEEDED( hResult ))
		return errOK;

	return errNOT_SUPPORTED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FSIsProcessedOnReboot )
// Parameters are:
tERROR FSIsProcessedOnReboot( hSTRING p_hObj )
{
	// Place your code here
	if (!p_hObj)
		return errPARAMETER_INVALID;

	if (!gpContext)
		return errUNEXPECTED;

	tERROR error = errNOT_OK;

	WCHAR pwchFN[1024];
	if (p_hObj)
		error = p_hObj->ExportToBuff(NULL, cSTRING_WHOLE, pwchFN, sizeof(pwchFN), cCP_UNICODE, cSTRING_Z);
	if (PR_FAIL( error ))
		return error;

	ULONG RetCode;
	HRESULT hResult = DRV_SpecFileGet( gpContext, pwchFN, &RetCode );
	if (SUCCEEDED( hResult ))
	{
		switch(RetCode)
		{
		case _sfopr_ok:             error = errOK_DECIDED; break;
		case _sfopr_file_not_found: error = errOBJECT_NOT_FOUND; break;
		case _sfopr_access_denied:  error = errACCESS_DENIED; break;
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FSDirectRead )
// Parameters are:
tERROR FSDirectRead( tPTR p_NativeHandle, tQWORD p_Offset, tPTR p_Buffer, tDWORD p_BufferSize, tDWORD* p_RetSize ) 
{
	// Place your code here
	tERROR error = errNOT_SUPPORTED;
	tDWORD RetSize = 0;

	if (p_RetSize)
		*p_RetSize = RetSize;

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin export method implementation, FSDirectOpen )
// Parameters are:
tERROR FSDirectOpen(tPTR* p_NativeHandle, tWCHAR* p_wName) 
{
	// Place your code here

	tERROR error = errNOT_SUPPORTED;

	if (!p_NativeHandle || !p_wName)
	{
		PR_TRACE((g_root, prtIMPORTANT, "FSDrv\tdirect open: invalid argument"));
		return errPARAMETER_INVALID;
	}

	return error;
}
// AVP Prague stamp end
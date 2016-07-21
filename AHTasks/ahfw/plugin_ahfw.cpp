// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  03 May 2007,  21:09 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- AHFW
// Sub project -- Not defined
// Purpose     -- задача firewall
// Author      -- Sobko
// File Name   -- plugin_ahfw.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



#include "ahfw_imp.h"

hROOT g_root;
tProcessIdToSessionId gfnProcessToSession = 0;

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Task_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <AHTasks/plugin/p_ahfw.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <AHTasks/plugin/p_ahfw.h>
EXPORT_TABLE_END
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			{
				HMODULE hModule = GetModuleHandle("Kernel32.dll");
				if( hModule )
				{
					gfnProcessToSession = (tProcessIdToSessionId)::GetProcAddress(hModule, "ProcessIdToSessionId");
				}
			}

			root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_AHFW)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			CALL_Root_RegisterExportTable( root, NULL, export_table, PID_AHFW );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=Task_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"Task\" interface"));
				return cFALSE;
			}
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



// AVP Prague stamp begin( Plugin export method implementation, ResolveNetworks )
// Parameters are:
tERROR ResolveNetworks( cSerializable* p_Networks )
{
	if( !(p_Networks && p_Networks->isBasedOn(cFwNets::eIID)) )
		return errPARAMETER_INVALID;

    AHFWNetworks_RefreshNetworks(((cFwNets*)p_Networks)->m_aNetworks);
    return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FillApplicationChecksum )
// Parameters are:
tERROR FillApplicationChecksum( cStrObj* p_FullPathName, tBYTE* p_CheckSumBuffer, tDWORD* p_CheckSumBufferLen )
{
	// Place your code here
	tERROR error = errOK;

	if (!p_CheckSumBuffer || !p_CheckSumBufferLen)
		return errPARAMETER_INVALID;

	if (*p_CheckSumBufferLen < FW_HASH_SIZE)
		error = errBUFFER_TOO_SMALL;
	else
	{
		memset( p_CheckSumBuffer, 0, *p_CheckSumBufferLen);
		if (CKAHUM::srOK != GetApplicationChecksum(p_FullPathName->data(), p_CheckSumBuffer))
			error = errUNEXPECTED;

		PR_TRACE((g_root, prtIMPORTANT, "CKAHFW\tfill checksum result 0x%x in fb 0x%x lb 0x%x", error, p_CheckSumBuffer[0], p_CheckSumBuffer[FW_HASH_SIZE - 1]));
	}

	*p_CheckSumBufferLen = FW_HASH_SIZE;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, AddNetwork )
// Parameters are:
tERROR AddNetwork( cSerializable* p_Networks, cSerializable* p_NewNet, tUINT p_InsertIndex )
{
	if( !(p_Networks && p_Networks->isBasedOn(cFwNets::eIID)) )
		return errPARAMETER_INVALID;

	if( !(p_NewNet && p_NewNet->isBasedOn(cAHFWNetwork::eIID)) )
		return errPARAMETER_INVALID;

    AHFWNetworks_AddNetwork(((cFwNets*)p_Networks)->m_aNetworks, *(cAHFWNetwork*)p_NewNet, p_InsertIndex);

    return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, DeleteNetworkWithHidden )
// Parameters are:
tERROR DeleteNetworkWithHidden( cSerializable* p_Networks, tUINT p_Index )
{
	if( !(p_Networks && p_Networks->isBasedOn(cFwNets::eIID)) )
		return errPARAMETER_INVALID;

    AHFWNetworks_DeleteNetwork(((cFwNets*)p_Networks)->m_aNetworks, p_Index, cTRUE, cTRUE);

	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, DeleteNetwork )
// Parameters are:
tERROR DeleteNetwork( cSerializable* p_Networks, tUINT p_Index )
{
	if( !(p_Networks && p_Networks->isBasedOn(cFwNets::eIID)) )
		return errPARAMETER_INVALID;

    AHFWNetworks_DeleteNetwork(((cFwNets*)p_Networks)->m_aNetworks, p_Index, cTRUE, cFALSE);

	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ChangeNetwork )
// Parameters are:
tERROR ChangeNetwork( cSerializable* p_Networks, tUINT p_Index, cSerializable* p_NewNet )
{
	if( !(p_Networks && p_Networks->isBasedOn(cFwNets::eIID)) )
		return errPARAMETER_INVALID;

	if( !(p_NewNet && p_NewNet->isBasedOn(cAHFWNetwork::eIID)) )
		return errPARAMETER_INVALID;

    AHFWNetworks_ChangeNetwork(((cFwNets*)p_Networks)->m_aNetworks, p_Index, *(cAHFWNetwork*)p_NewNet);

	return errOK;
}
// AVP Prague stamp end




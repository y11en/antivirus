// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <AV/iface/i_engine.h>

#include "avlib.h"
// AVP Prague stamp end

#if defined (__MWERKS__)
	#include <string.h>
#else	
	#include "memory.h"
#endif

#define  IMPEX_EXPORT_LIB
#include <AV/plugin/p_avlib.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE(export_table)
#include <AV/plugin/p_avlib.h>
EXPORT_TABLE_END

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

#include "../AvpMgr/psw_heuristic.h"

tERROR pr_call AVLIB_PswHeuristic( hOS p_os, hObjPtr p_objptr, hIO* p_ioptr )
{
	return PswHeuristic( p_os, p_objptr, p_ioptr );
}

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	switch( dwReason ) {
		
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH : 
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		{
			tDWORD count;
			tERROR error=errOK;
			g_root=(hROOT)hInstance;
			
//			error=CALL_Root_ResolveImportFunc( Root,&SignCheck,ECLSID_LOADER, 0x25f0bac6l, PID_L_LLIO);
			error=CALL_Root_RegisterExportTable( g_root, &count, export_table, PID_AVLIB);
			
			if(pERROR)*pERROR=error;
			if(PR_FAIL(error)) return cFALSE;
		}
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		g_root = NULL;
		break;
	}
	return cTRUE;
}

// AVP Prague stamp end


#if defined (__unix__)

#define PR_IMPEX_DEF
#include <Prague/prague.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end

#include "l_llio.h"


#define  IMPEX_EXPORT_LIB
#include <AV/plugin/p_l_llio.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE(export_table)
#include <AV/plugin/p_l_llio.h>
EXPORT_TABLE_END

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = 0;

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
			
			error=CALL_Root_ResolveImportFunc( g_root,(tFUNC_PTR*)&SignCheck,ECLSID_LOADER, 0x25f0bac6l, PID_L_LLIO);
			error=CALL_Root_RegisterExportTable( g_root, &count, export_table, PID_L_LLIO );
		  
		}
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		g_root = 0;
		break;
	}
	return cTRUE;
}


tBOOL Read13(tBYTE Disk, tWORD Sector ,tBYTE Head ,tWORD NumSectors, tBYTE* Buffer)
{
	return 0;
}
tBOOL Write13(tBYTE Disk, tWORD Sector ,tBYTE Head ,tWORD NumSectors, tBYTE* Buffer)
{
	return 0;
}
tBOOL Read25(tBYTE Drive, tDWORD Sector, tWORD NumSectors, tBYTE* Buffer)
{
	return 0;
}
tBOOL Write26(tBYTE Drive, tDWORD Sector, tWORD NumSectors, tBYTE* Buffer)
{
	return 0;
}
tBOOL GetDiskParam(tBYTE disk, tBYTE drive, tWORD* CX, tBYTE* DH)
{
	return 0;
}
tDWORD GetFirstMcbSeg( tVOID )
{
	return 0;
}
tDWORD GetDosMemSize( tVOID )
{
	return 0;
}
tDWORD GetIfsApiHookTable( tDWORD* table, tDWORD size) //size in DWORDS !!!!!!!!
{
	return 0;
}
tDWORD GetDosTraceTable( tDWORD* table, tDWORD size)//size in DWORDS !!!!!!!!
{
	return 0;
}
tDWORD MemoryRead( tDWORD   dwOffset, tDWORD  dwSize, tBYTE*  lpBuffer)// size in bytes
{
	return 0;
}
tDWORD MemoryWrite( tDWORD   dwOffset, tDWORD  dwSize, tBYTE*  lpBuffer)// size in bytes
{
	return 0;
}
tERROR ExecSpecial( hOBJECT _this, tDWORD* result, tSTRING FuncName, tDWORD wParam, tDWORD lParam)
{
      return errOK;
}

// AVP Prague stamp end

#endif //__unix__


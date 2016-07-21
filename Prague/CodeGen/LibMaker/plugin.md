#include <prague.h>
#include "plugin.h"

#define ECLSID_AVP3LINK 0x865ac586 /*is a crc32 for "ECLSID_AVP3LINK"*/
#define ECLSID_AVP3DECODE 0x896dbd24 /*is a crc32 for "ECLSID_AVP3DECODE"*/
#define ECLSID_AVP3CURE 0xcacaff9e /*is a crc32 for "ECLSID_AVP3CURE"*/

#define DLINK3_DO_NOT_DEFINE_PROTOTYPES
#include <DLINK3.H>

#include "imports.h"

#define IMPEX_EXPORT_LIB
#include "exports.h"

#define  IMPEX_TABLE_CONTENT
IMPORT_TABLE(plImportTable)
	#include "imports.h"
IMPORT_TABLE_END

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE(plExportTable)
	#include "exports.h"
EXPORT_TABLE_END

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pError ) {
	hROOT hRoot;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH : 
			return cTRUE;
		case PRAGUE_PLUGIN_LOAD:
			hRoot = (hROOT)hInstance;
			// register my interfaces
			// register my imports
			if ( PR_FAIL(*pError = CALL_Root_ResolveImportTable(hRoot, NULL, plImportTable, PID_GENPLUGIN)) )
				return cFALSE;
			// register my exports
			CALL_Root_RegisterExportTable(hRoot, NULL, plExportTable, PID_GENPLUGIN);
			// register my custom property ids
			// use some system resources
			return cTRUE;
			
		case PRAGUE_PLUGIN_UNLOAD:
			// free system resources
			// unregister my custom property ids
			// interfaces, exports and imports Loader can unregister by itself
			return cTRUE;
	}
	return cTRUE;	
}

void main(){}



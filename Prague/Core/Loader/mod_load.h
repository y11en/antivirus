#if !defined( __mod_load_h__ )
#define __mod_load_h__

#include <Prague/pr_types.h>
#include <Prague/pr_err.h>
#include <Prague/pr_reg.h>
#include <Prague/pr_cpp.h>

#define cUNEXPECTED_ERROR     0
#define cPRAGUE_PLUGIN_FOUND  1
#define cNATIVE_PLUGIN_FOUND  2
#define cELF_PLUGIN_FOUND     3


#if defined( __cplusplus )
extern "C" {
#endif

tBOOL       pr_call known_plugin_type( tDWORD plugin_type );
tINT        pr_call CheckPluginByName( const tVOID* szFileName, tCODEPAGE cp );
tERROR      pr_call LoadPlugin( hOBJECT obj, const tVOID* szPluginName, tCODEPAGE cp, tDWORD plugin_type, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint, void (__cdecl ** finish_proc)(void), tProtector* protect );
tPluginInit pr_call GetStartAddress( hPLUGININSTANCE hInst );
tERROR      pr_call UnloadPlugin( hPLUGININSTANCE hInst, tDWORD plugin_type, void (__cdecl * finish_proc)(void) );

#if defined (OLD_METADATA_SUPPORT)
tERROR      pr_call GetPrMetaData( const tVOID* szFileName, tCODEPAGE cp, tPTR hModuleInfo );
tERROR      pr_call GetDllMetaData( const tVOID* szFileName, tCODEPAGE cp, tPTR hModuleInfo  );
#endif // OLD_METADATA_SUPPORT

#if defined( __cplusplus )
}

#endif // __cplusplus
	

#endif //__PrLoader_h__

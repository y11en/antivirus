// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  06 July 2006,  12:14 --------
// File Name   -- (null)i_plugin.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_plugin__37b13819_ae7b_11d4_b757_00d0b7170e50 )
#define __i_plugin__37b13819_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Plugin interface implementation
// Short comments -- interface of load/unload plugins
//    Supplies facilities :
//      -- to load/unload certain plugin
//      -- to get shared interface properties implemented by plugin
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_PLUGIN  ((tIID)(5))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hPLUGIN;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iPluginVtbl;
typedef struct iPluginVtbl iPluginVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cPlugin;
	typedef cPlugin* hPLUGIN;
#else
	/*typedef*/ struct tag_hPLUGIN {
		const iPluginVtbl* vtbl; // pointer to the "Plugin" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	};// *hPLUGIN;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Plugin_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpPlugin_GetInterfaceProperty) ( hPLUGIN _this, tDWORD* result, tDWORD index, tPROPID prop_id, tPTR buffer, tDWORD count ); // -- ;
	typedef   tERROR (pr_call *fnpPlugin_Load)                 ( hPLUGIN _this );             // -- load the module;
	typedef   tERROR (pr_call *fnpPlugin_Unload)               ( hPLUGIN _this, tBOOL force ); // -- unload the module;
	typedef   tERROR (pr_call *fnpPlugin_RegisterInterfaces)   ( hPLUGIN _this, tDWORD* result );             // -- registers interfaces implemented by module and returns count of interfaces registered;
	typedef   tERROR (pr_call *fnpPlugin_UnregisterInterfaces) ( hPLUGIN _this );             // -- unregister interfaces implemented by module;
	typedef   tERROR (pr_call *fnpPlugin_CheckInterfaces)      ( hPLUGIN _this );             // -- check interfaces implemented by module;
	typedef   tERROR (pr_call *fnpPlugin_UnloadUnused)         ( hPLUGIN _this, tDWORD during ); // -- ;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iPluginVtbl {
	fnpPlugin_GetInterfaceProperty  GetInterfaceProperty;
	fnpPlugin_Load                  Load;
	fnpPlugin_Unload                Unload;
	fnpPlugin_RegisterInterfaces    RegisterInterfaces;
	fnpPlugin_UnregisterInterfaces  UnregisterInterfaces;
	fnpPlugin_CheckInterfaces       CheckInterfaces;
	fnpPlugin_UnloadUnused          UnloadUnused;
}; // "Plugin" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Plugin_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION   mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT   mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgDEFAULT_CODEPAGE    mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00001000 )
#define pgDEFAULT_DATE_FORMAT mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgIS_LOADED           mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001002 )
//! #define pgOBJECT_NAME         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
#define pgMODULE_NAME         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001003 )
#define pgMODULE_ID           mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001009 )
#define pgAUTO_START          mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001006 )
#define pgIS_PRAGUE_FORMAT    mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001007 )
#define pgINTERFACE_COUNT     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001008 )
#define pgTIME_STAMP          mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x0000100a )
#define pgUNLOADABLE          mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100b )
#define pgUSAGE_COUNT         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000100c )
//! #define pgPLUGIN_EXPORT_COUNT mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x000000c6 )
#define pgUNUSED_SINCE        mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x0000100d )
#define pgIS_INTEGRAL         mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100e )
#define pgLOAD_ERROR          mPROPERTY_MAKE_GLOBAL( pTYPE_ERROR   , 0x0000100f )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Plugin_GetInterfaceProperty( _this, result, index, prop_id, buffer, count )    ((_this)->vtbl->GetInterfaceProperty( (_this), result, index, prop_id, buffer, count ))
	#define CALL_Plugin_Load( _this )                                                           ((_this)->vtbl->Load( (_this) ))
	#define CALL_Plugin_Unload( _this, force )                                                  ((_this)->vtbl->Unload( (_this), force ))
	#define CALL_Plugin_RegisterInterfaces( _this, result )                                     ((_this)->vtbl->RegisterInterfaces( (_this), result ))
	#define CALL_Plugin_UnregisterInterfaces( _this )                                           ((_this)->vtbl->UnregisterInterfaces( (_this) ))
	#define CALL_Plugin_CheckInterfaces( _this )                                                ((_this)->vtbl->CheckInterfaces( (_this) ))
	#define CALL_Plugin_UnloadUnused( _this, during )                                           ((_this)->vtbl->UnloadUnused( (_this), during ))
#else // if !defined( __cplusplus )
	#define CALL_Plugin_GetInterfaceProperty( _this, result, index, prop_id, buffer, count )    ((_this)->GetInterfaceProperty( result, index, prop_id, buffer, count ))
	#define CALL_Plugin_Load( _this )                                                           ((_this)->Load( ))
	#define CALL_Plugin_Unload( _this, force )                                                  ((_this)->Unload( force ))
	#define CALL_Plugin_RegisterInterfaces( _this, result )                                     ((_this)->RegisterInterfaces( result ))
	#define CALL_Plugin_UnregisterInterfaces( _this )                                           ((_this)->UnregisterInterfaces( ))
	#define CALL_Plugin_CheckInterfaces( _this )                                                ((_this)->CheckInterfaces( ))
	#define CALL_Plugin_UnloadUnused( _this, during )                                           ((_this)->UnloadUnused( during ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iPlugin {
		virtual tERROR pr_call GetInterfaceProperty( tDWORD* result, tDWORD index, tPROPID prop_id, tPTR buffer, tDWORD count ) = 0;
		virtual tERROR pr_call Load() = 0; // -- load the module
		virtual tERROR pr_call Unload( tBOOL force = cFALSE ) = 0; // -- unload the module
		virtual tERROR pr_call RegisterInterfaces( tDWORD* result ) = 0; // -- registers interfaces implemented by module and returns count of interfaces registered
		virtual tERROR pr_call UnregisterInterfaces() = 0; // -- unregister interfaces implemented by module
		virtual tERROR pr_call CheckInterfaces() = 0; // -- check interfaces implemented by module
		virtual tERROR pr_call UnloadUnused( tDWORD during ) = 0;
	};

	struct pr_abstract cPlugin : public iPlugin, public iObj {

		OBJ_IMPL( cPlugin );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hPLUGIN()   { return (hPLUGIN)this; }

		tCODEPAGE pr_call get_pgDEFAULT_CODEPAGE() { return (tCODEPAGE)getDWord(pgDEFAULT_CODEPAGE); }
		tERROR pr_call set_pgDEFAULT_CODEPAGE( tCODEPAGE value ) { return setDWord(pgDEFAULT_CODEPAGE,(tDWORD)value); }

		tDWORD pr_call get_pgDEFAULT_DATE_FORMAT() { return (tDWORD)getDWord(pgDEFAULT_DATE_FORMAT); }
		tERROR pr_call set_pgDEFAULT_DATE_FORMAT( tDWORD value ) { return setDWord(pgDEFAULT_DATE_FORMAT,(tDWORD)value); }

		tBOOL pr_call get_pgIS_LOADED() { return (tBOOL)getBool(pgIS_LOADED); }
		tERROR pr_call set_pgIS_LOADED( tBOOL value ) { return setBool(pgIS_LOADED,(tBOOL)value); }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tERROR pr_call get_pgMODULE_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgMODULE_NAME,buff,size,cp); }
		tERROR pr_call set_pgMODULE_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgMODULE_NAME,buff,size,cp); }

		tDWORD pr_call get_pgMODULE_ID() { return (tDWORD)getDWord(pgMODULE_ID); }
		tERROR pr_call set_pgMODULE_ID( tDWORD value ) { return setDWord(pgMODULE_ID,(tDWORD)value); }

		tBOOL pr_call get_pgAUTO_START() { return (tBOOL)getBool(pgAUTO_START); }
		tERROR pr_call set_pgAUTO_START( tBOOL value ) { return setBool(pgAUTO_START,(tBOOL)value); }

		tBOOL pr_call get_pgIS_PRAGUE_FORMAT() { return (tBOOL)getBool(pgIS_PRAGUE_FORMAT); }
		tERROR pr_call set_pgIS_PRAGUE_FORMAT( tBOOL value ) { return setBool(pgIS_PRAGUE_FORMAT,(tBOOL)value); }

		tDWORD pr_call get_pgINTERFACE_COUNT() { return (tDWORD)getDWord(pgINTERFACE_COUNT); }
		tERROR pr_call set_pgINTERFACE_COUNT( tDWORD value ) { return setDWord(pgINTERFACE_COUNT,(tDWORD)value); }

		tERROR pr_call get_pgTIME_STAMP( tDATETIME* value ) { return get(pgTIME_STAMP,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgTIME_STAMP( tDATETIME* value ) { return set(pgTIME_STAMP,value,sizeof(tDATETIME)); }

		tBOOL pr_call get_pgUNLOADABLE() { return (tBOOL)getBool(pgUNLOADABLE); }
		tERROR pr_call set_pgUNLOADABLE( tBOOL value ) { return setBool(pgUNLOADABLE,(tBOOL)value); }

		tDWORD pr_call get_pgUSAGE_COUNT() { return (tDWORD)getDWord(pgUSAGE_COUNT); }
		tERROR pr_call set_pgUSAGE_COUNT( tDWORD value ) { return setDWord(pgUSAGE_COUNT,(tDWORD)value); }

		tDWORD pr_call get_pgPLUGIN_EXPORT_COUNT() { return (tDWORD)getDWord(pgPLUGIN_EXPORT_COUNT); }
		tERROR pr_call set_pgPLUGIN_EXPORT_COUNT( tDWORD value ) { return setDWord(pgPLUGIN_EXPORT_COUNT,(tDWORD)value); }

		tERROR pr_call get_pgUNUSED_SINCE( tDATETIME* value ) { return get(pgUNUSED_SINCE,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgUNUSED_SINCE( tDATETIME* value ) { return set(pgUNUSED_SINCE,value,sizeof(tDATETIME)); }

		tBOOL pr_call get_pgIS_INTEGRAL() { return (tBOOL)getBool(pgIS_INTEGRAL); }
		tERROR pr_call set_pgIS_INTEGRAL( tBOOL value ) { return setBool(pgIS_INTEGRAL,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_plugin__37b13819_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end




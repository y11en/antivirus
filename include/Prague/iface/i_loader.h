// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  13 April 2005,  18:17 --------
// File Name   -- (null)i_loader.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_loader__37b13813_ae7b_11d4_b757_00d0b7170e50 )
#define __i_loader__37b13813_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Loader interface implementation
// Short comments -- prague loadable plugin finder
//    Supplies facility to find prague loadable plugins. Must create objects of "Plugin" interafce. One object to one loadable plugin
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_LOADER  ((tIID)(6))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )

// ----------  force flags  ----------
// //rus:флаги, используемые в методах FindModules/IntegateModules
#define mLOADER_FORCE_MASK             ((tDWORD)(0x0000000C)) // mask for check "force" flags
#define fLOADER_FORCE_ALL              ((tDWORD)(0x0000000C)) // unconditionally process all plugins
#define fLOADER_FORCE_NATIVE           ((tDWORD)(0x00000008)) // unconditionally process native format plugins
#define fLOADER_FORCE_PRAGUE           ((tDWORD)(0x00000004)) // unconditionally process prague format plugins

// ----------  parameter format  ----------
#define mLOADER_PARAM_MASK             ((tDWORD)(0x00003f00L)) //  --
#define fLOADER_PARAM_IS_WSTR          ((tDWORD)(0x00000100L)) // parameter is Unicode string
#define fLOADER_PARAM_IS_FOLDER        ((tDWORD)(0x00000000L)) // parameter is a ANSI name of the folder
#define fLOADER_PARAM_IS_WFOLDER       ((tDWORD)(fLOADER_PARAM_IS_FOLDER | fLOADER_PARAM_IS_WSTR)) // parameter is a Unicode name of the folder
#define fLOADER_PARAM_IS_STR_POINTER_LIST ((tDWORD)(0x00000200L)) // parameter is list of the ANSI string pointers
#define fLOADER_PARAM_IS_WSTR_POINTER_LIST ((tDWORD)(fLOADER_PARAM_IS_STR_POINTER_LIST | fLOADER_PARAM_IS_WSTR)) // parameter is list of the unicode string pointers
#define fLOADER_PARAM_IS_STR_LIST      ((tDWORD)(0x00000400L)) // parameter is list of ANSI strings in a single buffer terminated by '\0'. Last string is terminated by double '\0\0'
#define fLOADER_PARAM_IS_WSTR_LIST     ((tDWORD)(fLOADER_PARAM_IS_STR_LIST | fLOADER_PARAM_IS_WSTR)) // parameter is list of Unicode strings in a single buffer terminated by L'\0'. Last string is terminated by double L'\0'L'\0'

// ----------  transaction flag  ----------
#define fLOADER_TRANSACTION            ((tDWORD)(0x10000000)) // IntegrateModules must be transacted. Method IntegrateCommit or IntagateRollback must be called after IntegateModules
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
#define errTRANSACTION_IN_PROGRESS               PR_MAKE_DECL_ERR(IID_LOADER, 0x001) // 0x80006001,-2147459071 (1) -- //rus:возвращается методами интерфейса, если выполняется транзакция
#define errTRANSACTION_NOT_STARTED               PR_MAKE_DECL_ERR(IID_LOADER, 0x002) // 0x80006002,-2147459070 (2) -- //rus:возвращается методами IntagrateCommit и IntagateRollback, если транзакция не была стартована
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hLOADER;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iLoaderVtbl;
typedef struct iLoaderVtbl iLoaderVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cLoader;
	typedef cLoader* hLOADER;
#else
	typedef struct tag_hLOADER {
		const iLoaderVtbl* vtbl; // pointer to the "Loader" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hLOADER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Loader_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpLoader_FindModules)                ( hLOADER _this, const tVOID* param_pool, tDWORD param_pool_size, tDWORD flags ); // -- finds plugins and create module objects correspondent to plugins;
	typedef   tERROR (pr_call *fnpLoader_LoadModules)                ( hLOADER _this, tDWORD format_flags ); // -- load found modules into memory;
	typedef   tERROR (pr_call *fnpLoader_UnloadModules)              ( hLOADER _this, tDWORD format_flags ); // -- unload all found modules from memory;
	typedef   tERROR (pr_call *fnpLoader_RegisterModuleInterfaces)   ( hLOADER _this );       // -- register interfaces in all found modules;
	typedef   tERROR (pr_call *fnpLoader_UnregisterModuleInterfaces) ( hLOADER _this );       // -- unregister interfaces in all found modules;
	typedef   tERROR (pr_call *fnpLoader_CheckAutoLoad)              ( hLOADER _this );       // -- check if plugins are autoloaded and load it if necessary;
	typedef   tERROR (pr_call *fnpLoader_IntegrateModules)           ( hLOADER _this, const tVOID* param_pool, tDWORD param_pool_size, tDWORD load_flags ); // -- finds and autoloads modules, registers interfaces and load native or prague plugins by flags;
	typedef   tERROR (pr_call *fnpLoader_InegrateCommit)             ( hLOADER _this );       // -- ;
	typedef   tERROR (pr_call *fnpLoader_IntegrateRollback)          ( hLOADER _this );       // -- ;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iLoaderVtbl {
	fnpLoader_FindModules                 FindModules;
	fnpLoader_LoadModules                 LoadModules;
	fnpLoader_UnloadModules               UnloadModules;
	fnpLoader_RegisterModuleInterfaces    RegisterModuleInterfaces;
	fnpLoader_UnregisterModuleInterfaces  UnregisterModuleInterfaces;
	fnpLoader_CheckAutoLoad               CheckAutoLoad;
	fnpLoader_IntegrateModules            IntegrateModules;
	fnpLoader_InegrateCommit              InegrateCommit;
	fnpLoader_IntegrateRollback           IntegrateRollback;
}; // "Loader" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Loader_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Loader_FindModules( _this, param_pool, param_pool_size, flags )                      ((_this)->vtbl->FindModules( (_this), param_pool, param_pool_size, flags ))
	#define CALL_Loader_LoadModules( _this, format_flags )                                            ((_this)->vtbl->LoadModules( (_this), format_flags ))
	#define CALL_Loader_UnloadModules( _this, format_flags )                                          ((_this)->vtbl->UnloadModules( (_this), format_flags ))
	#define CALL_Loader_RegisterModuleInterfaces( _this )                                             ((_this)->vtbl->RegisterModuleInterfaces( (_this) ))
	#define CALL_Loader_UnregisterModuleInterfaces( _this )                                           ((_this)->vtbl->UnregisterModuleInterfaces( (_this) ))
	#define CALL_Loader_CheckAutoLoad( _this )                                                        ((_this)->vtbl->CheckAutoLoad( (_this) ))
	#define CALL_Loader_IntegrateModules( _this, param_pool, param_pool_size, load_flags )            ((_this)->vtbl->IntegrateModules( (_this), param_pool, param_pool_size, load_flags ))
	#define CALL_Loader_InegrateCommit( _this )                                                       ((_this)->vtbl->InegrateCommit( (_this) ))
	#define CALL_Loader_IntegrateRollback( _this )                                                    ((_this)->vtbl->IntegrateRollback( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_Loader_FindModules( _this, param_pool, param_pool_size, flags )                      ((_this)->FindModules( param_pool, param_pool_size, flags ))
	#define CALL_Loader_LoadModules( _this, format_flags )                                            ((_this)->LoadModules( format_flags ))
	#define CALL_Loader_UnloadModules( _this, format_flags )                                          ((_this)->UnloadModules( format_flags ))
	#define CALL_Loader_RegisterModuleInterfaces( _this )                                             ((_this)->RegisterModuleInterfaces( ))
	#define CALL_Loader_UnregisterModuleInterfaces( _this )                                           ((_this)->UnregisterModuleInterfaces( ))
	#define CALL_Loader_CheckAutoLoad( _this )                                                        ((_this)->CheckAutoLoad( ))
	#define CALL_Loader_IntegrateModules( _this, param_pool, param_pool_size, load_flags )            ((_this)->IntegrateModules( param_pool, param_pool_size, load_flags ))
	#define CALL_Loader_InegrateCommit( _this )                                                       ((_this)->InegrateCommit( ))
	#define CALL_Loader_IntegrateRollback( _this )                                                    ((_this)->IntegrateRollback( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iLoader {
		virtual tERROR pr_call FindModules( const tVOID* param_pool, tDWORD param_pool_size, tDWORD flags ) = 0; // -- finds plugins and create module objects correspondent to plugins
		virtual tERROR pr_call LoadModules( tDWORD format_flags ) = 0; // -- load found modules into memory
		virtual tERROR pr_call UnloadModules( tDWORD format_flags ) = 0; // -- unload all found modules from memory
		virtual tERROR pr_call RegisterModuleInterfaces() = 0; // -- register interfaces in all found modules
		virtual tERROR pr_call UnregisterModuleInterfaces() = 0; // -- unregister interfaces in all found modules
		virtual tERROR pr_call CheckAutoLoad() = 0; // -- check if plugins are autoloaded and load it if necessary
		virtual tERROR pr_call IntegrateModules( const tVOID* param_pool, tDWORD param_pool_size, tDWORD load_flags ) = 0; // -- finds and autoloads modules, registers interfaces and load native or prague plugins by flags
		virtual tERROR pr_call InegrateCommit() = 0;
		virtual tERROR pr_call IntegrateRollback() = 0;
	};

	struct pr_abstract cLoader : public iLoader, public iObj {

		OBJ_IMPL( cLoader );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hLOADER()   { return (hLOADER)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_loader__37b13813_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end




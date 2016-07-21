// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Sunday,  14 March 2004,  13:10 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- loader.cpp
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Loader_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_win32loader.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include "loader.h"
// AVP Prague stamp end

#include "plugin.h"


#ifdef _PRAGUE_TRACE_
	#define CALL_FUNC( l_err, g_err, call, str )                                                 \
		if ( PR_FAIL(l_err=call) ) {                                                               \
			tCHAR  name[MAX_PATH];                                                                   \
			if ( PR_SUCC(g_err) )                                                                    \
				g_err = l_err;                                                                         \
			if ( PR_FAIL(plugin->propGetStr(0,pgMODULE_NAME,name,sizeof(name),cCP_ANSI)) )           \
				strcpy_s( name, countof(name), "unknown plugin" );                                                      \
			PR_TRACE(( this, prtERROR, __FILE__LINE__ "ldr\t(%terr) error during "str". Module \"%s\"", l_err, name )); \
		}
#else
	#define CALL_FUNC( l_err, g_err, call, str )     \
		if ( PR_FAIL(l_err=call) && PR_SUCC(g_err) ) { \
			g_err = l_err;                               \
    }
#endif



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Loader". Static(shared) property table variables
// AVP Prague stamp end



// ---
#if defined (_MSC_VER)
	#pragma warning (disable:4355)
#endif

LoaderData::LoaderData() : m_pluginList(this) {
}

#if defined (_MSC_VER)
	#pragma warning (default:4355)
#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR LoaderData::ObjectInit() {
	PR_TRACE_A0( this, "Enter LoaderData::ObjectInit method" );
	
	CALL_SYS_PropertySetUInt( this, pgTRACE_LEVEL, prtIMPORTANT );

	PR_TRACE_A0( this, "Leave LoaderData::ObjectInit method, ret errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindModules )
// Extended method comment
//    Finds plugins and creates correspondent module objects.
//    Each plugin must have its own module object
// Parameters are:
tERROR LoaderData::FindModules( const tVOID* p_param_pool, tDWORD p_param_pool_size, tDWORD p_flags ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::FindModules method" );

  if ( m_commit_flags & fLOADER_TRANSACTION )
    error = errTRANSACTION_IN_PROGRESS;
  else
    error = i_FindModules( p_param_pool, p_param_pool_size, p_flags );

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, LoadModules )
// Parameters are:
tERROR LoaderData::LoadModules( tDWORD p_format_flags ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::LoadModules method" );

  if ( m_commit_flags & fLOADER_TRANSACTION )
    error = errTRANSACTION_IN_PROGRESS;
  else
    error = i_LoadModules( p_format_flags );

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnloadModules )
// Parameters are:
tERROR LoaderData::UnloadModules( tDWORD p_format_flags ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::UnloadModules method" );

  if ( m_commit_flags & fLOADER_TRANSACTION )
    error = errTRANSACTION_IN_PROGRESS;
	
  else {
    error = errOK;

    p_format_flags &= mLOADER_FORCE_MASK;
    if ( !p_format_flags )
      ;

    else {
			for( tUINT i=0,c=m_pluginList.count(); i<c; ++i ) {
				cPlugin* plugin = m_pluginList[i];
        if ( plugin && ((p_format_flags == fLOADER_FORCE_ALL) || (!plugin->propGetBool(pgIS_PRAGUE_FORMAT) == !(p_format_flags == fLOADER_FORCE_PRAGUE))) ) {
					tERROR l_err;
          CALL_FUNC( l_err, error, plugin->Unload(cTRUE), "unloading" );
        }
			}

    }
  }

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterModuleInterfaces )
// Parameters are:
tERROR LoaderData::RegisterModuleInterfaces() {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::RegisterModuleInterfaces method" );

  if ( m_commit_flags & fLOADER_TRANSACTION )
    error = errTRANSACTION_IN_PROGRESS;
  else
    error = i_RegisterModuleInterfaces();

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterModuleInterfaces )
// Parameters are:
tERROR LoaderData::UnregisterModuleInterfaces() {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::UnregisterModuleInterfaces method" );

  if ( m_commit_flags & fLOADER_TRANSACTION )
    error = errTRANSACTION_IN_PROGRESS;
	
  else {
    error = errOK;
		for( tUINT i=0,c=m_pluginList.count(); i<c; ++i ) {
			tERROR l_err;
			cPlugin* plugin = m_pluginList[i];
			if ( !plugin )
				continue;
			CALL_FUNC( l_err, error, plugin->UnregisterInterfaces(), "unregistering interfaces" );
		}
  }

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CheckAutoLoad )
// Parameters are:
tERROR LoaderData::CheckAutoLoad() {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::CheckAutoLoad method" );

  if ( m_commit_flags & fLOADER_TRANSACTION )
    error = errTRANSACTION_IN_PROGRESS;
  else
    error = i_CheckAutoLoad();

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, IntegrateModules )
// Parameters are:
tERROR LoaderData::IntegrateModules( const tVOID* p_param_pool, tDWORD p_param_pool_size, tDWORD p_load_flags ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::IntegrateModules method" );
	
	if ( m_commit_flags & fLOADER_TRANSACTION )
		error = errTRANSACTION_IN_PROGRESS;
	
	else {
		
		//if ( !(p_load_flags & PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH) )
		//	;
		//else 
		if ( PR_FAIL(error=i_FindModules(p_param_pool,p_param_pool_size,p_load_flags)) ) {
			PR_TRACE(( this, prtERROR, "ldr\t(%terr) error during module finding", error ));
		}
		
		m_commit_flags = p_load_flags;
		
		if ( !(p_load_flags & fLOADER_TRANSACTION) )
			error = i_InegrateCommit();
		else {
			error = errOK;
			for( tUINT i=0,c=m_pluginList.count(); i<c; ++i ) {
				cPlugin* plugin = m_pluginList[i];
				if ( plugin ) {
					tERROR l_err;
					CALL_FUNC( l_err, error, plugin->CheckInterfaces(), "registering interfaces" );
				}
			}
		}
	}
	
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, InegrateCommit )
// Parameters are:
tERROR LoaderData::InegrateCommit() {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::InegrateCommit method" );

  if ( !(m_commit_flags & fLOADER_TRANSACTION) )
    error = errTRANSACTION_NOT_STARTED;
  else
    error = i_InegrateCommit();

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, IntegrateRollback )
// Parameters are:
tERROR LoaderData::IntegrateRollback() {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Loader::IntegrateRollback method" );

  if ( !(m_commit_flags & fLOADER_TRANSACTION) )
    error = errTRANSACTION_NOT_STARTED;
	
  else {
    PR_TRACE_A0( this, "Enter Loader::IntegrateRollback method" );

    error = errOK;
		for( tUINT i=0,c=m_pluginList.count(); i<c; ++i ) {
			tERROR l_err;
			cPlugin*& plugin = m_pluginList[i];
			if ( !plugin )
				continue;
      CALL_FUNC( l_err, error, plugin->sysCloseObject(), "closing plugin object" );
			plugin = 0;
		}
    m_commit_flags = 0;
  }

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Loader". Register function
tERROR LoaderData::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Loader_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Loader_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "prague loadable plugin finder", 30 )
mpPROPERTY_TABLE_END(Loader_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Loader)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_STATIC(ObjectNew)
mINTERNAL_TABLE_END(Loader)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Loader)
	mEXTERNAL_METHOD(Loader, FindModules)
	mEXTERNAL_METHOD(Loader, LoadModules)
	mEXTERNAL_METHOD(Loader, UnloadModules)
	mEXTERNAL_METHOD(Loader, RegisterModuleInterfaces)
	mEXTERNAL_METHOD(Loader, UnregisterModuleInterfaces)
	mEXTERNAL_METHOD(Loader, CheckAutoLoad)
	mEXTERNAL_METHOD(Loader, IntegrateModules)
	mEXTERNAL_METHOD(Loader, InegrateCommit)
	mEXTERNAL_METHOD(Loader, IntegrateRollback)
mEXTERNAL_TABLE_END(Loader)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Loader::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_LOADER,                             // interface identifier
		PID_WIN32LOADER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		Loader_VERSION,                         // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Loader_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Loader_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Loader_vtbl,                         // external function table
		(sizeof(e_Loader_vtbl)/sizeof(tPTR)),   // external function table size
		Loader_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Loader_PropTable), // property table size
		sizeof(LoaderData)-sizeof(cLoader),     // memory size
		IFACE_SYSTEM | IFACE_UNSWAPPABLE        // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ldr\tLoader(IID_LOADER) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Loader_Register( hROOT root ) { return LoaderData::Register(root); }
// AVP Prague stamp end



// ---
tERROR pr_call LoaderData::i_FindModules( const tVOID* param_pool, tDWORD param_pool_size, tDWORD flags ) {
  tERROR error;
	
  switch( flags & mLOADER_PARAM_MASK ) {
		case fLOADER_PARAM_IS_FOLDER :
			error = find_modules_in_folder( param_pool, param_pool_size, cCP_ANSI );
			break;
			
		case fLOADER_PARAM_IS_WFOLDER :
			error = find_modules_in_folder( param_pool, param_pool_size, cCP_UNICODE );
			break;
			
		case fLOADER_PARAM_IS_STR_POINTER_LIST :
			error = find_modules_by_ptr_list( param_pool, param_pool_size );
			break;

		case fLOADER_PARAM_IS_WSTR_POINTER_LIST :
			error = find_modules_by_ptr_list_w( param_pool, param_pool_size );
			break;
			
		case fLOADER_PARAM_IS_STR_LIST :
		case fLOADER_PARAM_IS_WSTR_LIST :
		default:
			error = errNOT_SUPPORTED;
			break;
  }
	
  return error;
}



// ---
tERROR pr_call LoaderData::i_LoadModules( tDWORD fflags ) {
  tERROR error = errOK;
  
	for( tUINT i=0,c=m_pluginList.count(); i<c; ++i ) {
		cPlugin* plugin = m_pluginList[i];
    if ( plugin && ((fflags == fLOADER_FORCE_ALL) || (!plugin->propGetBool(pgIS_PRAGUE_FORMAT) == !(fflags == fLOADER_FORCE_PRAGUE))) ) {
			tERROR l_err;
      CALL_FUNC( l_err, error, plugin->Load(), "loading" );
    }
	}

  return error;
}
// AVP Prague stamp end




// ---
tERROR pr_call LoaderData::i_RegisterModuleInterfaces() {
  tERROR  error = errOK;
	
	for( tUINT i=0,c=m_pluginList.count(); i<c; ++i ) {
		tERROR l_err;
		cPlugin* plugin = m_pluginList[i];
		if ( !plugin )
			continue;
    CALL_FUNC( l_err, error, plugin->RegisterInterfaces(0), "registering interfaces" );
	}

	return error;
}




// ---
tERROR pr_call LoaderData::i_CheckAutoLoad() {
	tERROR   error = errOK;
	tUINT    count = m_pluginList.count();
	
	for( tUINT i=0; i<count; ++i ) {
		cPlugin* plugin = m_pluginList[i];
		if ( plugin && (cTRUE == plugin->propGetBool(pgAUTO_START)) ) {
			tERROR l_err;
			CALL_FUNC( l_err, error, plugin->Load(), "plugin loading" );
		}
	}
	return error;
}




// ---
tERROR pr_call LoaderData::i_InegrateCommit() {
  tERROR err;
  tERROR ret_err = errOK;
  
  if ( PR_FAIL(err=i_RegisterModuleInterfaces()) )
    ret_err = err;
  if ( PR_FAIL(err=i_CheckAutoLoad()) ) {
    if ( PR_SUCC(ret_err) )
      ret_err = err;
    PR_TRACE(( this, prtERROR, "ldr\t(%terr) err plugin auto loading", err ));
  }
  if ( (m_commit_flags & fLOADER_FORCE_ALL) && PR_FAIL(err=i_LoadModules(m_commit_flags)) ) {
    if ( PR_SUCC(ret_err) )
      ret_err = err;
    PR_TRACE(( this, prtERROR, "ldr\t(%terr) err during module loading", err ));
  }
  
  m_commit_flags = 0;
	
  PR_TRACE_A1( this, "Leave Loader::InegrateCommit method, ret %terr", ret_err );
  return ret_err;
}



// ---
extern "C" tERROR pr_call CreateModule( hROOT root, tPTR inst, const tVOID* module, tCODEPAGE cp, tPID pid ) {
	tERROR  error;
	/* 	tCHAR   module_name[MAX_PATH]; */
	hPLUGIN plugin;
	PR_TRACE_VARS;

	/* 	if ( module.empty() ) { */
	/* 		if ( 0 == GetModuleFileName(inst,module_name,sizeof(module_name)) ) { */
	/* 			PR_TRACE(( root, prtERROR, "ldr\tCannot get module name (win error 0x%08x)", name, GetLastError() )); */
	/* 			return errMODULE_NOT_FOUND; */
	/* 		} */
	/* 		module = module_name; */
	/* 	} */
	if ( PR_FAIL(error=CALL_SYS_ObjectCreate(root,(hOBJECT*)&plugin,IID_PLUGIN,PID_LOADER,0)) ) {
		PR_TRACE(( root, prtERROR, "ldr\tCannot create module object for \"%s%S\" (%terr)", PR_TRACE_PREPARE(module), error ));
		return error;
	}
	
	tUINT size;
	#if defined( _WIN32 )
		if ( cp == cCP_UNICODE )
			size = _toui32(UNIS * (1+wcslen(UNI(module))));
		else
	#endif
			size = _toui32(MBS * (1+strlen(MB(module))));

	if ( PR_FAIL(error=CALL_SYS_PropertySetStr(plugin,0,pgMODULE_NAME,(tVOID*)module,size,cp)) ) {
		PR_TRACE(( plugin, prtERROR, "ldr\tCannot initialize module object for \"%s%S\" (%terr)", PR_TRACE_PREPARE(module), error ));
		return error;
	}
	if ( PR_FAIL(error=CALL_SYS_PropertySet(plugin,0,pgMODULE_ID,&pid,sizeof(pid))) ) {
		PR_TRACE(( plugin, prtERROR, "ldr\tCannot initialize module object for \"%s%S\" (%terr)", PR_TRACE_PREPARE(module), error ));
		return error;
	}
	if ( PR_FAIL(error=CALL_SYS_PropertySet(plugin,0,plMODULE_INSTANCE,&inst,sizeof(inst))) ) {
		PR_TRACE(( plugin, prtERROR, "ldr\tCannot initialize module object for \"%s%S\" (%terr)", PR_TRACE_PREPARE(module), error ));
		return error;
	}
	if( PR_FAIL(error=CALL_SYS_ObjectCreateDone(plugin)) ) {
		PR_TRACE(( plugin, prtERROR, "ldr\tCannot initialize module object for \"%s%S\" (%terr)", PR_TRACE_PREPARE(module), error ));
		return error;
	}
	if ( PR_FAIL(error=CALL_SYS_ParentSet(plugin,0,(hOBJECT)root)) ) {
		PR_TRACE(( plugin, prtERROR, "ldr\tCannot intagrate \"%s%S\" module into the system (%terr)", PR_TRACE_PREPARE(module), error ));
		return error;
	}
	if ( PR_FAIL(error=CALL_Plugin_Load(plugin)) ) {
		PR_TRACE(( plugin, prtERROR, "ldr\tCannot initialize module object for \"%s%S\" (%terr)", PR_TRACE_PREPARE(module), error ));
		return error;
	}
  return errOK;
}


// ---
extern "C" tERROR IntegratePlugins( tDWORD init_flags ) {
	tERROR err = errOK;
	if ( init_flags & PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH ) {
		hLOADER loader;
		init_flags |= fLOADER_PARAM_IS_FOLDER;
		if ( g_bUnderNT )
			init_flags |= fLOADER_PARAM_IS_WSTR;
		if ( PR_FAIL(err=CALL_SYS_ObjectCreateQuick(g_root,&loader,IID_LOADER,PID_WIN32_PLUGIN_LOADER,0)) ) {
			PR_TRACE(( g_root, prtERROR, "ldr\t(%terr) cannot create plugin loader", err ));
		}
		else if ( PR_FAIL(err=CALL_Loader_IntegrateModules(loader,g_plugins_path,_toui32(SLEN(g_plugins_path)),init_flags)) ) {
			PR_TRACE(( g_root, prtERROR, "ldr\t(%terr) error during module integrating", err ));
		}

		if ( loader )
			CALL_SYS_ObjectClose( loader );
	}
	return err;
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  29 March 2002,  17:55 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- impex.c
// Additional info
//  Kernel interface implementations
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Root_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include "kernel.h"
#include "k_root.h"
#include <m_utils.h>
#include <hashutil/hashutil.h>

#include <Prague/prague.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/e_loader.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktime.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktrace.h>
// AVP Prague stamp end


#define IMPEX_TABLE_CONTENT
EXPORT_TABLE(export_table)
	#include <Prague/iface/e_ktime.h>
	#include <Prague/iface/e_ktrace.h>
EXPORT_TABLE_END


// ---
tERROR pr_call Root_GetPluginInstance( hi_Root _this, hPLUGIN* result, tDATA* cookie, tPID pid );

/*
// ---
tDWORD NULL_export_func( tVOID ) {
	PR_TRACE(( g_impex, prtDANGER, "krn\tNULL_export_func: export function has been predefined but not actualized yet" ));
	return (tDWORD)-1;
}




// ---
tDWORD Realized_import_func( tVOID ) {
	PR_TRACE(( g_impex, prtDANGER, "krn\tExport function has been predefined but not actualized yet" ));
	return (tDWORD)-1;
}
*/


typedef hCRITICAL_SECTION hCS;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// ImpexManager interface implementation
// Short comments -- import/export function manager
// AVP Prague stamp end



// ---
typedef struct tag_IMPDesc {
	tFUNC_PTR* func_var;
	hPLUGIN    plugin;
	tUINT      usage_count;
} tIMPDesc;



// ---
typedef struct tag_FDesc {
  tFUNC_PTR  func_ptr;
  tDATA      func_cls;
  tDATA      func_id;
  hPLUGIN    plugin;
	tDWORD     storage_size;
	tDWORD     imp_count;
	tIMPDesc*  imports;       
} tFDesc;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface ImpexManager. Inner data structure
struct ImpexData {
  hCS     cs;           // protect object
  tDWORD  storage_size; // storage size
	tDWORD  table_size;   // size of the exports array
	tFDesc* exports;      // export function descriptor table
} g_id;
// AVP Prague stamp end




static tFDesc* find_exp_descriptor( tDATA cls, tDATA id );
static tERROR  check_exp_descriptor( tFUNC_PTR func, tDATA cls, tDATA id, hPLUGIN plugin );
static tERROR  add_exp_descriptor( tFUNC_PTR func, tDATA cls, tDATA id, hPLUGIN plugin );
static hPLUGIN resolve_plugin( hi_Root _this, tPID* pid );

static tERROR  add_imp_descriptor( tFDesc* exp, tFUNC_PTR* var, hPLUGIN plugin );
static tERROR  del_imp_descriptors( tFDesc* exp, hPLUGIN plugin );
static tERROR  renew_imports( tFDesc* exp, tFUNC_PTR ptr, hPLUGIN new_plugin );



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterExportTable )
// Parameters are:
tERROR pr_call Root_RegisterExportTable( hi_Root _this, tDWORD* result, const tEXPORT* ex_tbl, tPID exporter_pid ) {
  tERROR  error;
  tUINT	  count = 0;
  hPLUGIN plugin;
  
  PR_TRACE_A0( _this, "Enter Root::RegisterExportTable method" );

  plugin = resolve_plugin( _this, &exporter_pid );

  if ( !plugin )
    error = errPARAMETER_INVALID;
  else if ( !ex_tbl ) {
    PR_TRACE(( _this, prtERROR, "krn\tParameter \"exp_tbl\" must not be NULL" ));
    error = errPARAMETER_INVALID;
  }
  else {
    hCS            cs = g_id.cs;
    const tEXPORT* fed = ex_tbl;
    error = errOK;
    
    lock_ex( cs, SHARE_LEVEL_WRITE );
    
    for( ; fed->cls_id; fed++ ) {
      tERROR err = check_exp_descriptor( (tFUNC_PTR)fed->func, fed->cls_id, fed->fnc_id, plugin );
      if ( PR_SUCC(err) )
        count++;
      else if ( PR_SUCC(error) )
        error = err;
    }
    unlock_ex( cs, 0 );
    if ( exporter_pid == PID_STRING ) {
      tERROR pr_call _ImportStringService();
      _ImportStringService();
    }
  }
  
  if ( result )
    *result = count;
  PR_TRACE_A2( _this, "Leave Root::RegisterExportTable method, ret %terr, function count accepted - %u", error, count );
  return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterExportFunc )
// Parameters are:
tERROR pr_call Root_RegisterExportFunc( hi_Root _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tDWORD func_id, tPID pid ) {
  tERROR error;
  hPLUGIN plugin;
  PR_TRACE_A0( _this, "Enter Root::RegisterExportFunc method" );
  
  plugin = resolve_plugin( _this, &pid );
  if ( !plugin )
    error = errPARAMETER_INVALID;
  else {
    hCS cs = g_id.cs;
    lock_ex( cs, SHARE_LEVEL_WRITE );
    error = check_exp_descriptor( func_ptr, func_cls, func_id, plugin );
    unlock_ex( cs, 0 );
  }
  
  PR_TRACE_A1( _this, "Leave Root::RegisterExportFunc method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterExportFuncByName )
// Parameters are:
tERROR pr_call Root_RegisterExportFuncByName( hi_Root _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tSTRING func_name, tPID pid ) {
  tERROR  error;
  hPLUGIN plugin;
  PR_TRACE_A0( _this, "Enter Root::RegisterExportFuncByName method" );
  
  plugin = resolve_plugin( _this, &pid );
  if ( !plugin )
    error = errPARAMETER_INVALID;
  else {
    hCS cs = g_id.cs;
    lock_ex( cs, SHARE_LEVEL_WRITE );
    error = check_exp_descriptor( func_ptr, func_cls, iCountCRC32str(func_name), plugin );
    unlock_ex( cs, 0 );
  }
  
  PR_TRACE_A1( _this, "Leave Root::RegisterExportFuncByName method, ret %terr", error );
  return error;
}
// AVP Prague stamp end




#define lineno_to_str2(x)    #x
#define lineno_to_str(x)     lineno_to_str2(x)
#define __FL__               __FILE__ lineno_to_str( __LINE__ ) ": "

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ResolveImportTable )
// Parameters are:
tERROR pr_call Root_ResolveImportTable( hi_Root _this, tDWORD* result, const tIMPORT* imp_tbl, tPID pid ) {
  
  hPLUGIN plugin;
  tUINT   found = 0;
  tERROR  error;
  
  PR_TRACE_A0( _this, "Enter Root::ResolveImportTable method" );
  
  if ( !imp_tbl ) {
    PR_TRACE(( _this, prtDANGER, "krn\tParameter \"imp_tbl\" must not be NULL" ));
    error = errPARAMETER_INVALID;
  }
  else if ( 0 == (plugin = resolve_plugin(_this,&pid)) )
    error = errPARAMETER_INVALID;
  else {
    const tIMPORT* fid;
    hCS            cs = g_id.cs;

    error = errOK;
    lock_ex( cs, SHARE_LEVEL_WRITE );
    for( fid=imp_tbl; fid->func; fid++ ) {
      tFDesc* exp = find_exp_descriptor( fid->cls_id, fid->fnc_id );
      if ( exp ) {
        tERROR err = add_imp_descriptor( exp, (tFUNC_PTR*)fid->func, plugin );
        if ( PR_SUCC(err) )
          found++;
        else {
          PR_TRACE(( _this, prtDANGER, "krn\tCannot resolve import function (cls=0x%08x, id=0x%08x) error = %terr", fid->cls_id, fid->fnc_id, err ));
          error = err;
        }
      }
      else {
      #ifdef _PRAGUE_TRACE_
				tDWORD c = 0;
				tCHAR plugin_name[260];
        memset( plugin_name, 0, sizeof(plugin_name) );
        pid = CALL_SYS_PropertyGetDWord( plugin, pgMODULE_ID );
        if ( PR_SUCC(CALL_SYS_PropertyGetStr(plugin,&c,pgMODULE_NAME,plugin_name,sizeof(plugin_name),cCP_ANSI)) ) {
					tCHAR* p;
					if ( c )
						--c;
					p = plugin_name + c;
					while( p>plugin_name && (*p!='\\') )
						--p;
					if ( p > plugin_name )
						++p;
					PR_TRACE(( _this, prtDANGER, __FL__ "krn\tPlugin (%tpid,%s) cannot import function (cls=%tpid, id=0x%08x)", pid, p, fid->cls_id, fid->fnc_id ));
				}
      #endif
        error = errNOT_FOUND;
      }
    }
    unlock_ex( cs, 0 );
  }
  
  if ( result )
    *result = found;
  
  PR_TRACE_A1( _this, "Leave Root::ResolveImportTable method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ResolveImportFunc )
// Parameters are:
tERROR pr_call Root_ResolveImportFunc( hi_Root _this, tFUNC_PTR* result, tDWORD func_cls, tDWORD func_id, tPID pid ) {
  tERROR  error;
  hPLUGIN plugin;
  PR_TRACE_A0( _this, "Enter Root::ResolveImportFunc method" );
  
  if ( !result ) {
    PR_TRACE(( _this, prtDANGER, "krn\tParameter \"result\" must not be NULL" ));
    error = errPARAMETER_INVALID;
  }
  else if ( 0 == (plugin = resolve_plugin(_this,&pid)) )
    error = errPARAMETER_INVALID;
  else {
    hCS cs = g_id.cs;
		tFDesc* exp;
    lock_ex( cs, SHARE_LEVEL_WRITE );
		exp = find_exp_descriptor( func_cls, func_id );
    if ( !exp ) {
      PR_TRACE(( _this, prtDANGER, "krn\tCannot resolve function (cls=0x%08x, id=0x%08x)", func_cls, func_id ));
      error = errNOT_FOUND;
    }
    else if ( PR_FAIL(error=add_imp_descriptor(exp,result,plugin)) )
      PR_TRACE(( _this, prtDANGER, "krn\tCannot resolve import function (cls=0x%08x, id=0x%08x) error = %terr", func_cls, func_id, error ));
    unlock_ex( cs, 0 );
  }
  PR_TRACE_A2( _this, "Leave Root::ResolveImportFunc method, ret tFUNC_PTR = %p, error = %terr", result ? *result : 0, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ResolveImportFuncByName )
// Parameters are:
tERROR pr_call Root_ResolveImportFuncByName( hi_Root _this, tFUNC_PTR* result, tDWORD func_cls, const tSTRING func_name, tPID pid ) {
  tERROR error;
  PR_TRACE_A0( _this, "Enter Root::ResolveImportFuncByName method" );
  error = Root_ResolveImportFunc( _this, result, func_cls, iCountCRC32str(func_name), pid );
  PR_TRACE_A2( _this, "Leave Root::ResolveImportFuncByName method, ret tFUNC_PTR = %p, error = %terr", (result ? *result : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseImportTable )
// Parameters are:
tERROR pr_call Root_ReleaseImportTable( hi_Root _this, tDWORD* result, const tIMPORT* imp_tbl, tPID pid ) {
  tERROR  error;
  hPLUGIN plugin;
  tDWORD  found = 0;
  PR_TRACE_A0( _this, "Enter Root::ReleaseImportTable method" );
  
  if ( !imp_tbl ) {
    PR_TRACE(( _this, prtERROR, "krn\tParameter \"imp_tbl\" must not be NULL" ));
    error = errPARAMETER_INVALID;
  }
  else if ( 0 == (plugin=resolve_plugin(_this,&pid)) )
    error = errPARAMETER_INVALID;
  else {
    const tIMPORT* fid;
    hCS cs = g_id.cs;
    
    lock_ex( cs, SHARE_LEVEL_WRITE );

    error = errOK;
    for( fid=imp_tbl; fid->func; fid++ ) {
      tFDesc* exp = find_exp_descriptor( fid->cls_id, fid->fnc_id );
      if ( exp ) {
        tERROR err = del_imp_descriptors( exp, plugin );
        if ( PR_SUCC(err) )
          found++;
        else {
          PR_TRACE(( _this, prtERROR, "krn\tCannot release import function (cls=0x%08x, id=0x%08x) error = %terr", fid->cls_id, fid->fnc_id, err ));
          error = err;
        }
      }
      else {
        PR_TRACE(( _this, prtERROR, "krn\tCannot find imported function (cls=0x%08x, id=0x%08x)", fid->cls_id, fid->fnc_id ));
        error = errNOT_FOUND;
      }
    }
    unlock_ex( cs, 0 );
  }
  
  if ( result )
    *result = found;
  PR_TRACE_A2( _this, "Leave Root::ReleaseImportTable method, ret tDWORD = %u, error = %terr", found, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseImportFunc )
// Parameters are:
tERROR pr_call Root_ReleaseImportFunc( hi_Root _this, tDWORD func_cls, tDWORD func_id, tPID pid ) {
  hPLUGIN  plugin;
  tERROR   error;
  PR_TRACE_A0( _this, "Enter Root::ReleaseImportFunc method" );
  
  if ( 0 == (plugin=resolve_plugin(_this,&pid)) )
    error = errPARAMETER_INVALID;
  else { 
		tFDesc* exp;
    hCS cs = g_id.cs;
    lock_ex( cs, SHARE_LEVEL_WRITE );
		exp = find_exp_descriptor( func_cls, func_id );
    if ( !exp ) {
      PR_TRACE(( _this, prtERROR, "krn\tCannot find imported function (cls=0x%08x, id=0x%08x)", func_cls, func_id ));
      error = errNOT_FOUND;
    }
    else if ( PR_FAIL(error=del_imp_descriptors(exp,plugin)) )
      PR_TRACE(( _this, prtERROR, "krn\tCannot remove import descriptors (cls=0x%08x, id=0x%08x) - error = %terr", func_cls, func_id, error ));
    unlock_ex( cs, 0 );
  }
  PR_TRACE_A1( _this, "Leave Root::ReleaseImportFunc method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseImportFuncByName )
// Parameters are:
tERROR pr_call Root_ReleaseImportFuncByName( hi_Root _this, tDWORD func_cls, const tSTRING func_name, tPID pid ) {
  tERROR error;
  PR_TRACE_A0( _this, "Enter Root::ReleaseImportFuncByName method" );
  error = Root_ReleaseImportFunc( _this, func_cls, iCountCRC32str(func_name), pid );
  PR_TRACE_A1( _this, "Leave Root::ReleaseImportFuncByName method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// ---
static tBOOL pr_call _InterfaceFindByPlugin( tINTERFACE* iface, tPTR plugin ) {
  return (iface->plugin == (hPLUGIN)plugin) ? cTRUE : cFALSE;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, PluginUnloadNotification )
// Parameters are:
tERROR pr_call Root_PluginUnloadNotification( hi_Root _this, hPLUGIN plugin_unloaded ) {
  tDWORD     usage_count = 0;
  PR_TRACE_A0( _this, "Enter Root::PluginUnloadNotification method" );
  
  if ( PR_FAIL(CALL_SYS_PropertyGet(plugin_unloaded,0,pgUSAGE_COUNT,&usage_count,sizeof(usage_count))) || (usage_count == 0) ) {
		tPO*         po;
		tUINT        i, c;
    hCS          cs;
		tINTERFACE*  iface;
		//tINTERFACE*  local_iface_table[LOCAL_IFACE_CLUSTER];
		//tINTERFACE** pitable = local_iface_table;
		//tUINT        iface_count = 0;
		tDWORD       cookie = 0;
		tFDesc*      dsc;
		

		enter_lf();
		wlock(po);
		iface = _InterfaceFindInt( &cookie, _InterfaceFindByPlugin, (tPTR)plugin_unloaded );    // find any alive iface implemented by the plugin
		while( iface ) {
			_Interface_0_Init( po, iface );
			iface = _InterfaceFindInt( &cookie, _InterfaceFindByPlugin, (tPTR)plugin_unloaded );
		}
    wunlock(po);
		leave_lf();

		//if ( pitable != local_iface_table )
		//	PrFree( pitable );
		
    cs = g_id.cs;
    lock_ex( cs, SHARE_LEVEL_WRITE );

    for( i=0,c=g_id.table_size,dsc=g_id.exports; i<c; ++i,++dsc ) {
      del_imp_descriptors( dsc, plugin_unloaded );
      if ( dsc->plugin == plugin_unloaded ) {
        tIMPDesc* imp  = dsc->imports;
        tINT      j, k = dsc->imp_count;
        for( j=0; j<k; ++j,++imp ) {
					imp->plugin = 0;
					if ( imp->func_var ) {
#if defined (_WIN32)
						if ( PrSafelyReadMemory ) {
							tFUNC_PTR fptr = 0;
							PrSafelyReadMemory( &fptr, (tVOID*)imp->func_var, sizeof(imp->func_var), 0 ); // !!! must have ZERO pointer
						}
#endif //_WIN32
					}
					else {
						PR_TRACE(( g_root, prtERROR, "krn\tIMPEX error - imp->func_var is NULL" ));
					}
				}
      }
    }
    unlock_ex( cs, 0 );
  }
  
  PR_TRACE_A0( _this, "Leave Root::PluginUnloadNotification method, ret tERROR = errOK" );
  return errOK;
}
// AVP Prague stamp end




// ---
static tFDesc* find_exp_descriptor( tDATA cls, tDATA id ) {
  tUINT i;
  tFDesc* exp = g_id.exports;
  for( i=0; i<g_id.table_size; ++i,++exp ) {
    if ( ((exp->func_cls == 0) || (exp->func_cls == cls)) && (exp->func_id == id) )
      return exp;
  }
  return 0;
}





#define EXP_DESC_CLUST_SIZE (16)
#define IMP_DESC_CLUST_SIZE (8)


// ---
static tERROR add_exp_descriptor( tFUNC_PTR ptr, tDATA cls, tDATA id, hPLUGIN plugin ) {
  
  // parameter "plugin" is always non NULL
  
  tERROR  error;
  tFDesc* exp;
  
  if ( g_id.storage_size == g_id.table_size ) {
    error = PrRealloc( (tPTR*)&g_id.exports, g_id.exports, (g_id.storage_size + EXP_DESC_CLUST_SIZE) * sizeof(tFDesc) );
    if ( PR_FAIL(error) )
      return error;
    g_id.storage_size += EXP_DESC_CLUST_SIZE;
  }
  
  exp = g_id.exports + g_id.table_size++;
  exp->func_ptr  = ptr;
  exp->func_cls  = cls;
  exp->func_id   = id;
  exp->imp_count = 0;
  exp->imports   = 0;
  exp->plugin    = plugin;
  return errOK;
}




// ---
static tERROR check_exp_descriptor( tFUNC_PTR func, tDATA cls, tDATA fid, hPLUGIN plugin ) {
  
  // parameter "plugin" is always non NULL
  
  tERROR  error;
  tFDesc* exp = find_exp_descriptor( cls, fid );
  
  if ( !exp )                                      // first time here -- just register
    error = add_exp_descriptor( func, cls, fid, plugin );
  
  else if ( !func )																						// set registered address to NULL -- restricted
    error = errPARAMETER_INVALID;
  
  else if ( !exp->func_ptr ) {                                // renew metadata's predeclared address
    error = renew_imports( exp, func, plugin ); 
  }
  
  else if ( exp->func_ptr == func ) {                        // adresses are the same -- must be second registration 
    if ( exp->plugin == plugin )                             // am I right ? -- nothing to do
      error = errOK;
    
    else {                                                   // nonsense -- the same addresses in two different plugins -- is caller a cheater ?
      error = errPARAMETER_INVALID;
      //	error = errOK;
      //	exp->plugin = plugin;
    }
  }
  
  else if ( exp->plugin == plugin )                          // plugin is the same -- just renew address (caller updates address)
    error = renew_imports( exp, func, plugin );
  
  else {                                                     // different plugins found and both exists (update may be?)
    switch( _compare_timestamp(exp->plugin,plugin) ) {
      case  0: 
      case +1: error = errTIMESTAMP_UNSUITABLE;    break;
      default: 
        error = renew_imports( exp, func, plugin ); 
        break;
    }
  }
  return error;
}




// ---
static tERROR add_imp_descriptor( tFDesc* exp, tFUNC_PTR* var, hPLUGIN plugin ) {
	tERROR    e;
	tIMPDesc* imp;
	tUINT     i, c;
	tDATA     index;

	for( i=0,c=exp->imp_count,imp=exp->imports; i<c; ++i,++imp ) {
		
		if ( imp->plugin != plugin )
			continue;
		index = exp - g_id.exports;
		if ( PR_SUCC(e=CALL_Plugin_Load(exp->plugin)) ) {
			imp->usage_count++;
			imp->func_var = var;
			*var = g_id.exports[index].func_ptr;
			if ( !*var )
				e = errNOT_FOUND;
		}
		return e;
	}

  if ( exp->storage_size == c ) {
    tUINT  s = sizeof(tIMPDesc) * (c + IMP_DESC_CLUST_SIZE); 
    if ( PR_FAIL(e=PrRealloc((tPTR*)&exp->imports,exp->imports,s)) )
      return e;
    exp->storage_size += IMP_DESC_CLUST_SIZE;
		imp = exp->imports + c;
  }

	exp->imp_count++;
  imp->plugin = plugin;
	index = exp - g_id.exports;
	if ( PR_SUCC(e=CALL_Plugin_Load(exp->plugin)) ) {
		imp->usage_count++;
		imp->func_var = var;
		*var = g_id.exports[index].func_ptr;
		if ( !*var )
			e = errNOT_FOUND;
	}
	return e;
}




// ---
static tERROR del_imp_descriptors( tFDesc* exp, hPLUGIN plugin ) {
  tINT i;
  for( i=exp->imp_count-1; i>=0; i-- ) {
    tIMPDesc* imp = exp->imports + i;
    if ( imp->plugin == plugin ) {
			tINT count;
//			tINT j, count;
//			for( j=0, count = imp->usage_count; j<count; ++j )
//				CALL_Plugin_Unload( exp->plugin, cFALSE );
			count = --exp->imp_count - i;
			if ( count > 0 )
				memmove( imp, imp+1, count * sizeof(tIMPDesc) );
			break;
		}
  }
  return errOK;
}



// ---
static hPLUGIN resolve_plugin( hi_Root root, tPID* pid ) {

	tPO*     po;
	//tERROR   error;
	//tHANDLE* handle;
	hPLUGIN  plugin = 0;

	if ( *pid == PID_ANY ) {
		PR_TRACE(( root, prtERROR, "krn\tIMPEX cannot resolve PID_ANY to hPLUGIN" ));
		return 0;
	}

	enter_lf();
//	handle = _HandleCheck( po, (hOBJECT)*pid, &error, cTRUE );
//	if ( handle ) {
//		tDWORD size;
//		plugin = (hPLUGIN)*pid;
//		if ( (handle->iid != IID_PLUGIN) || PR_FAIL(_PropertyGet(po,handle,&size,pgMODULE_ID,pid,sizeof(*pid))) )
//			plugin = 0;
//		runlock();
//		return plugin;
//	}

	Root_GetPluginInstance( root, &plugin, 0, *pid );

	leave_lf();
	if ( !plugin )
		PR_TRACE(( root, prtERROR, "krn\tCannot resolve plugin id (%tpid)", *pid ));
	return plugin;
}




// ---
static tERROR renew_imports( tFDesc* exp, tFUNC_PTR ptr, hPLUGIN new_plugin ) {
	tUINT i;

  exp->func_ptr = ptr;
  for( i=0; i<exp->imp_count; ++i ) {
		tFUNC_PTR* funcVarAddr;
    if ( exp->plugin != new_plugin ) {
      if ( exp->plugin )
        CALL_Plugin_Unload( exp->plugin, cTRUE );
      if ( new_plugin )
        CALL_Plugin_Load( new_plugin );
    }
		funcVarAddr = exp->imports[i].func_var;
		if ( funcVarAddr )
			*funcVarAddr = ptr;
  }
	if ( exp->plugin != new_plugin )
		exp->plugin = new_plugin;
  
  return errOK;
}



// ---
tERROR pr_call _ImpexInitObj( tPO* po ) {
  static tBOOL exported;
  tERROR error;
  
  if ( g_id.cs )
    error = errOK;
  else if ( PR_FAIL(error=_SyncCreateAvailable((hOBJECT)g_root,&g_id.cs)) )
    ;
  else {
    wlock(po);
    _Remove( MakeHandle(g_id.cs) ); // cut it from the hierarchy
    wunlock(po);
  }
  
  if ( 
    PR_SUCC(error) && !exported && PR_SUCC(Root_GetPluginInstance((hi_Root)g_root,0,0,PID_KERNEL)) && 
    PR_SUCC(Root_RegisterExportTable((hi_Root)g_root,0,export_table,PID_KERNEL)) 
  )
    exported = cTRUE;
  
  return error;
}



// ---
tERROR pr_call _ImpexDeinitObj() {
  tFDesc* exp;
  hCS     cs = g_id.cs;
  
  lock_ex( cs, SHARE_LEVEL_WRITE );
	g_id.cs = 0;
  exp = g_id.exports;
  if ( exp ) {
    tUINT i, c;
    for( i=0,c=g_id.table_size; i<c; i++ ) {
      if ( exp[i].imports )
        PrFree( exp[i].imports );
    }
    PrFree( exp );
    g_id.exports = 0;
  }
  unlock_ex( cs, 0 );

	_ObjectClose( 0, MakeHandle(cs), 0 );
  return errOK;
}



// ---
tBOOL pr_call _check_plugin_exports_used( hPLUGIN plugin ) {
	tBOOL used = cFALSE;
  hCS   cs = g_id.cs;
  
#	if defined(_DEBUG) && defined(_PRAGUE_TRACE_)
	tCHAR buff[0x100] = {0};
#	endif

  lock_ex( cs, SHARE_LEVEL_READ );

  if ( g_id.exports ) {
    tUINT i, c;
	  tFDesc* exp = g_id.exports;
    for( i=0,c=g_id.table_size; (i<c) && !used; ++i,++exp ) {
			if ( exp->plugin == plugin ) {
				tUINT     j, k;
				tIMPDesc* imp = exp->imports;
				for( j=0, k=exp->imp_count; (j<k) && !used;  ++j,++imp ) {
					used = !!imp->usage_count;
#					if defined(_DEBUG) && defined(_PRAGUE_TRACE_)
						if ( used )
							CALL_SYS_PropertyGetStr( imp->plugin, 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI );
#					endif
				}
			}
    }
  }
  unlock_ex( cs, 0 );
  return used;
}




//#if defined(_DEBUG) && defined(_PRAGUE_TRACE_)

// ---
tVOID pr_call _enum_plugin_importers( hPLUGIN plugin, tVOID* ctx, tVOID (pr_call *call_back)(tVOID* ctx, tDWORD method_id, hPLUGIN) ) {
  hCS cs = g_id.cs;
  
  lock_ex( cs, SHARE_LEVEL_READ );

  if ( g_id.exports ) {
    tUINT i, c;
	  tFDesc* exp = g_id.exports;
    for( i=0,c=g_id.table_size; (i<c); ++i,++exp ) {
			if ( exp->plugin == plugin ) {
				tUINT     j, k;
				tIMPDesc* imp = exp->imports;
				for( j=0, k=exp->imp_count; (j<k);  ++j,++imp ) {
					call_back( ctx, exp->func_id, imp->plugin );
				}
			}
    }
  }
  unlock_ex( cs, 0 );
}

//#endif


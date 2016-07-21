#include <prague.h>
#include "pfinder.h"
#include <memory.h>



// ---
tERROR pr_call _FindModules( hROOT root, const tVOID* param_pool, tDWORD param_pool_size ) {

  tERROR          error;
  tDWORD          total_iface_count = 0;
  hPLUGIN_FINDER  finder = 0;
  hPLUGIN         plug;

  // get system plugin finder
  if ( PR_FAIL(error=CALL_SYS_ObjectCreateQuick(root,(hOBJECT*)&finder,IID_PLUGINFINDER,PID_WIN32_PLUGIN_LOADER,SUBTYPE_ANY)) ) 
    PR_TRACE(( root, prtDANGER, "Plugin finder(IID_PLUGINFINDER) not found" ));

	else if ( PR_FAIL(error=CALL_PluginFinder_FindModules(finder,param_pool,param_pool_size)) ) 
		PR_TRACE(( root, prtERROR, "Plugin finder's error [\"FindModules\" returned error - %u]",error ));

	else {

		error = CALL_SYS_ChildGetFirst( finder, (hOBJECT*)&plug, IID_PLUGIN, PID_ANY );
		
		while( plug ) {
			
			#ifdef _PRAGUE_TRACE_
				tCHAR plugin_name[260]; 
			#endif
			hPLUGIN                         tmp_plug;
			fnpPlugin_GetInterfaceProperty  gip_fn;
			tDWORD                          plugin_iface_count;
			tDWORD                          index;
			
			plugin_iface_count = 0;
			index   = 0;
			gip_fn = plug->vtbl->GetInterfaceProperty;
			
			#ifdef _PRAGUE_TRACE_
				if ( PR_FAIL(gip_fn(plug,0,0,pgPLUGIN_NAME,plugin_name,sizeof(plugin_name))) )
					memcpy( plugin_name, "unknown plugin", 15 );
				PR_TRACE(( finder, prtNOTIFY, "Loader::FindModules -- gathering info about \"%s\"", plugin_name ));
			#endif
			
			while( 1 ) { // go through all interfaces in the plugin
				
				tIID iid;
				
				++index;   
				error = gip_fn( plug, 0, ++index, pgINTERFACE_ID, &iid, sizeof(iid) ); // index zero is for the plugin properties
				if ( error == errINTERFACE_NO_MORE_ENTRIES ) { // got to last iface ?
					error = errOK;
					break;
				}
				else if ( error == errMODULE_NOT_VERIFIED ) {
					PR_TRACE(( finder, prtERROR, "Error reading iface description (plugin = \"%s\", item number - %u)", plugin_name, index-1 ));
					break;
				}
				else if ( PR_FAIL(error) )
					break;
				
				else if ( (iid == IID_ANY) || (iid == IID_NONE) ) { // interface type
					PR_TRACE(( finder, prtERROR, "Bad iface description - iid (plugin = \"%s\", iface number = %u)", plugin_name, index-1 ));
					error = errINTERFACE_INCOMPATIBLE;
				}
				
				else { // collect iface properties for interface table
					tPID     pid;
					tDWORD   sub;
					tVERSION ver;
					tVID     vid;
					tDWORD   flags;
					tIID     compat;
					
					gip_fn( plug, 0, index, pgPLUGIN_ID,                &pid,    sizeof(pid)    ); // plugin ID
					gip_fn( plug, 0, index, pgINTERFACE_SUBTYPE,        &sub,    sizeof(sub)    ); // subtype of interface in plugin
					gip_fn( plug, 0, index, pgINTERFACE_VERSION,        &ver,    sizeof(ver)    ); // version
					//gip_fn( plug, 0, index, pgINTERFACE_REVISION,       &ver,    sizeof(ver)    ); // revision
					gip_fn( plug, 0, index, pgVENDOR_ID,                &vid,    sizeof(vid)    ); // vendor id
					gip_fn( plug, 0, index, pgINTERFACE_FLAGS,          &flags,  sizeof(flags)  ); // interface flags
					gip_fn( plug, 0, index, pgINTERFACE_COMPATIBILITY,  &compat, sizeof(compat) ); // interface compatibility
					
					if ( ver == 0 )
						ver = 1;
					
					if ( (pid == 0) || (vid == 0) ) 
						PR_TRACE(( finder, prtERROR, "Bad iface description - (pid==0||vid==0)(plugin = \"%s\", iface number = %u, pid = %u, vid = %u", plugin_name, index-1, pid, vid ) );
					
					else if ( PR_SUCC(CALL_Root_RegisterIFaceEx(root,iid,pid,sub,ver,vid,0,0,0,0,0,0,0,flags,compat,plug,index)) ) { // are you happy ?
						total_iface_count++;
						plugin_iface_count++;
					}
				}
			} // while( 1 )
			
			CALL_SYS_ObjectGetNext( plug, (hOBJECT*)&tmp_plug, IID_PLUGIN, PID_ANY );
			if ( PR_SUCC(error) && plugin_iface_count ) // if it's ok move the plugin handle to the ROOT object
				CALL_SYS_ParentSet( plug, 0, (hOBJECT)root );
			plug = tmp_plug;
		}
	}
	CALL_SYS_ObjectClose( finder );
	
		/*
    // autoload all marked plugins or autoload it and on demand
    phandle = MakeHandle(_this)->child;
    while( phandle ) {
			if ( phandle->iid == IID_PLUGIN ) {
				tDWORD autostart;
				tBOOL  start_it = cFALSE;
				
				plug = (hPLUGIN)MakeObject( phandle );
				if ( PR_SUCC(CALL_Plugin_GetInterfaceProperty(plug,0,0,pgAUTO_START,&autostart,sizeof(autostart))) && autostart )
					start_it = cTRUE;
					if ( (start_it == cFALSE) && (_this->data->load_flags & PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_ALL) ) {
						tBOOL prague = CALL_SYS_PropertyGetBool( plug, pgIS_PRAGUE_FORMAT );
						if ( _this->data->load_flags & PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_PG ) 
							start_it = prague;
						if ( (start_it == cFALSE) && (_this->data->load_flags & PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_NATIVE) )
							start_it = !prague;
					}
					if ( start_it ) {
						#ifdef _PRAGUE_TRACE_
							if ( PR_FAIL(CALL_Plugin_GetInterfaceProperty(plug,0,0,pgPLUGIN_NAME,plugin_name,sizeof(plugin_name))) )
							mcpy( plugin_name, "unknown plugin", 15 );
							PR_TRACE( (MakeHandle(_this),prtNOTIFY,"Root::FindModules -- loading \"%s\"", plugin_name ) );
						#endif
						CALL_Plugin_Load( plug );
					}
				}
				phandle = phandle->next;
			}
		*/
    
  if ( PR_SUCC(error) && !total_iface_count )
    error = errINTERFACE_NOT_FOUND;

  return error;
}

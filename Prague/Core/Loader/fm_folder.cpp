#include <Prague/prague.h>
#include "loader.h"


#include <fm_folder_inc.c>


// ---
tBOOL pr_call LoaderData::check_plugin_by_name( const tVOID* module_name, tCODEPAGE cp ) {

	for( tUINT i=0,c=m_pluginList.count(); i<c; ++i ) {
		if ( check_name(m_pluginList[i],module_name,cp) )
			return cTRUE;
	}

  cERROR   err;
  cPlugin* plg;
  tDATA    cookie = 0;
  for( err=::g_root->GetPluginInstance(&plg,&cookie,PID_ANY); PR_SUCC(err); err=::g_root->GetPluginInstance(&plg,&cookie,PID_ANY) ) {
    if ( check_name(plg,module_name,cp) )
      return cTRUE;
  }

  return cFALSE;
}



// ---
tERROR pr_call LoaderData::create_module( const tVOID* module_name, tUINT len, tCODEPAGE cp ) {

	#if defined (_WIN32)
		if ( cp == cCP_UNICODE ) {
			if ( wcsstr(UNI(module_name),PR_KERNEL_MODULE_NAME_W) )
				return errOK;
		}
		else {
	#endif

			if ( strstr(MB(module_name),PR_KERNEL_MODULE_NAME) )
				return errOK;

	#if defined (_WIN32)
		}
	#endif

  if ( check_plugin_by_name(module_name,cp) )
    return errOK;

  tERROR   err;
	cPlugin* plg = 0;

  err = sysCreateObject( (hOBJECT*)&plg, IID_PLUGIN, PID_LOADER );
  if ( PR_SUCC(err) )
    err = plg->propSetStr( 0, pgMODULE_NAME, (tPTR)module_name, len, cp );
  if ( PR_SUCC(err) )
    err = plg->sysCreateObjectDone();
	if ( PR_SUCC(err) )
		err = plg->sysSetParent( 0, (cObject*)::g_root );

	if ( PR_SUCC(err) ) {
		cPlugin** place = m_pluginList.once_more();
		if ( place )
			*place = plg;
		else
			err = errNOT_ENOUGH_MEMORY;
	}
	else if ( plg )
    plg->sysCloseObject();

  return err;
}






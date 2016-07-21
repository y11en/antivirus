#include <Prague/prague.h>
#include "loader.h"

#include <string.h>

// ---
tERROR pr_call LoaderData::find_modules_by_ptr_list( const tVOID* param_pool, tDWORD param_pool_size ) {
	
  tERROR   error = errOK;
  tSTRING* plist = (tSTRING*)param_pool;
  tUINT    count = param_pool_size / sizeof(tSTRING);
  tUINT    i     = 0;
  
  for( ; i<count; i++, plist++ ) {
    tERROR err;
		if ( !*plist || !**plist )
			continue;
    err = create_module( *plist, _toui32((1+strlen(*plist))*MBS), cCP_ANSI );
    if ( PR_SUCC(error) && PR_FAIL(err) )
      error = err;
  }
  
  if ( PR_FAIL(error) ) {
    PR_TRACE(( this, prtERROR, "ldr\tError finding modules - %terr", error ));
  }
  
  return error;
}



// ---
tERROR pr_call LoaderData::find_modules_by_ptr_list_w( const tVOID* param_pool, tDWORD param_pool_size ) {

  tERROR    error = errOK;
#if defined (_WIN32)
  tWSTRING* plist = (tWSTRING*)param_pool;
  tUINT     i;
  tUINT     count = param_pool_size / sizeof(tWSTRING);
  
  for( i=0; i<count; ++i, ++plist ) {
    tERROR  err;
		if ( !*plist || !**plist )
			continue;
    err = create_module( *plist, _toui32((wcslen(*plist)+1)*UNIS), cCP_UNICODE );
    if ( PR_SUCC(error) && PR_FAIL(err) )
      error = err;
  }
  if ( PR_FAIL(error) ) {
    PR_TRACE(( this, prtERROR, "ldr\tError finding modules - %terr", error ));
  }
#endif // _WIN32  
  return error;
}



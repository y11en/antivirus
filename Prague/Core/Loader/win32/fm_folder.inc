#if !defined (_FM_FOLDER_WIN32_INC_)
#define _FM_FOLDER_WIN32_INC_

#include <windows.h>

// ---
static tBOOL pr_call check_name( cPlugin* plg, const tVOID* module_name, tCODEPAGE cp ) {
  cStrObj name;
  if ( PR_FAIL(name.assign(plg,pgMODULE_NAME)) )
    return cFALSE;
  tDWORD pos = name.find( module_name, cp, 0, cStrObj::whole, fSTRING_FIND_FORWARD|fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE );
  if ( pos != cStrObj::npos )
    return cTRUE;
  return cFALSE;
}


// ---
tERROR pr_call LoaderData::find_modules_in_folder( const tVOID* param_pool, tDWORD param_pool_size, tDWORD flags ) {

  HANDLE           fh;
  tERROR           error = errOK;
  cStrObj          path;
  cStrObj          mask;
  WIN32_FIND_DATAW fd;
	cCODEPAGE        cp = ( flags & fLOADER_PARAM_IS_WSTR ) ? cCP_UNICODE : cCP_ANSI;

  if ( param_pool && param_pool_size )
    path.assign( param_pool, cp );
  else
    path.assign( ::g_plugins_path, CP );

  PR_TRACE(( this, prtNOTIFY, "Search folder is \"%tS\"", path.data() ));

  mask = path;
  mask.add_path_sect( "*.ppl", cCP_ANSI );
  memset( &fd, 0, sizeof(fd) );
  
  cStrBuff name( mask, cp );
	if ( flags & fLOADER_PARAM_IS_WSTR )
		fh = FindFirstFileW( (tWCHAR*)name, &fd );
	else
    fh = FindFirstFileA( (tCHAR*)name, (WIN32_FIND_DATA*)&fd );

  if ( fh && fh != INVALID_HANDLE_VALUE ) {

    BOOL res;

    do {
      tERROR err;
      cStrObj curr = path;
      curr.add_path_sect( fd.cFileName, cp );

      cStrBuff name( curr, cp );
      err = create_module( name, name.used(), flags );

      if ( PR_SUCC(error) && PR_FAIL(err) )
        error = err;

      if ( flags & fLOADER_PARAM_IS_WSTR )
        res = FindNextFileW( fh, &fd );
      else
        res = FindNextFileA( fh, (WIN32_FIND_DATA*)&fd );
    } while( res );
    
    if ( PR_FAIL(error) ) {
      PR_TRACE(( this, prtERROR, "Error finding modules - 0x%08x", error ));
		}
    
    FindClose( fh );
  }
  else {
    error = errMODULE_NOT_FOUND;
    PR_TRACE(( this, prtERROR, "Folder has no prague modules" ));
  }
  
  return error;
}


#endif //_FM_FOLDER_WIN32_INC_

#if !defined (_FM_FOLDER_UNIX_INC_)
#define _FM_FOLDER_UNIX_INC_

#include <glob.h>
#include <string.h>
// ---
tBOOL pr_call check_name( cPlugin* plg, const tVOID* module_name, tCODEPAGE cp ) 
{
  tCHAR name[MAX_PATH] = {0};
  if ( PR_FAIL(plg->propGetStr(0,pgMODULE_NAME,name,sizeof(name),cCP_ANSI)) )
    return cFALSE;
  if ( strcmp(name,MB(module_name)) )
    return cFALSE;
  return cTRUE;
}


// ---
tERROR pr_call LoaderData::find_modules_in_folder( const tVOID* param_pool, tDWORD param_pool_size, tCODEPAGE cp ) 
{
  tERROR          error = errOK;
  tCHAR           def_path[MAX_PATH+1] = {0};
  glob_t          aGlob;

	if ( cp == cCP_UNICODE )
		return errPARAMETER_INVALID;

  if ( param_pool && param_pool_size ) {
    while( param_pool_size && *(((tBYTE*)param_pool) + (param_pool_size - 1)) == 0 )
      param_pool_size--;
    if ( param_pool_size > (MAX_PATH-7) )
      return errBUFFER_TOO_SMALL;
    if ( param_pool_size )
      memcpy( def_path, param_pool, param_pool_size );
    else
      def_path[0] = 0;
    if ( param_pool_size && def_path[param_pool_size-1] != PATH_DELIMETER_CHAR )
      def_path[param_pool_size++] = PATH_DELIMETER_CHAR;
  }
  else {
    tCHAR* p;
    strncpy ( def_path, MB(g_plugins_path), sizeof(def_path) - 1 );
    p = strrchr( def_path, PATH_DELIMETER_CHAR );
    if ( p ) {
      *(++p) = 0;
      param_pool_size = p - def_path;
    }
    else
      def_path[param_pool_size=0] = 0;
  }
  PR_TRACE( (this,prtNOTIFY,"Search folder is \"%s\"",def_path) );
  memcpy( def_path + param_pool_size, "*.ppl", 6 );

  if ( !glob ( def_path, 0, 0, &aGlob ) ) {

    for ( unsigned int i = 0; i < aGlob.gl_pathc; ++i ) {
      tERROR err = create_module(  aGlob.gl_pathv [ i ], strlen( aGlob.gl_pathv [ i ] ) + 1, cCP_ANSI );
      if ( PR_SUCC(error) && PR_FAIL(err) )
        error = err;
    } 
    
    if ( PR_FAIL(error) ) 
      PR_TRACE(( this, prtERROR, "Error finding modules - %terr", error ));
  }
  else {
    error = errMODULE_NOT_FOUND;
    PR_TRACE(( this, prtERROR, "Folder has no prague modules" ));
  }
  globfree(&aGlob);

  return error;
}

#endif //_FM_FOLDER_UNIX_INC_

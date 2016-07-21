#if ! defined (_MOD_LOAD_UNIX_INC_)
#define _MOD_LOAD_UNIX_INC_

#define TRY      
#define EXCEPT  if(0)


#include <dlfcn.h>

tERROR lockProtector( pthread_mutex_t& theProtector, hOBJECT obj, const tVOID* name, tCODEPAGE cp );
tERROR unlockProtector( pthread_mutex_t& theProtector, hOBJECT obj, const tVOID* name, tCODEPAGE cp );

// ---
tPluginInit pr_call GetStartAddress( hPLUGININSTANCE hInst ) 
{
  if( *((tDWORD*)hInst) == cHEADER_SIGNATURE )
    return (tPluginInit)(reinterpret_cast<unsigned char*>( hInst )+((PrFileHeader*)hInst)->dwEntryPoint);

  return (tPluginInit) dlsym ( (void*) hInst, "DllMain" );	
}

#define ShowPluginStartAddress(szPluginName,Addr)

// ---
static tERROR pr_call LoadDllPlugin( const tVOID* szPluginName, tCODEPAGE cp, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint ) 
{
  tERROR aReserved;  

  PR_TRACE(( 0, prtSPAM, "LoadDllPlugin: Loading native plugin \"%s\"", szPluginName ));
  
  if ( !pInst || !pEntryPoint )
    return errPARAMETER_INVALID;
  
  *pInst = 0;
  *pEntryPoint = 0;

  if (cp == cCP_UNICODE) {
	  tCHAR l_name [MAX_PATH] = {0};
	  tERROR error = CopyTo (l_name, sizeof(l_name), cCP_ANSI, (tVOID*)szPluginName, 0, cp, cSTRING_CONTENT_ONLY, 0 );
	  if (PR_FAIL(error))
		  return error;
	  *pInst = (hPLUGININSTANCE) dlopen ( l_name, RTLD_NOW );
  }
  else
	  *pInst = (hPLUGININSTANCE) dlopen ( MB(szPluginName), RTLD_NOW );

  if ( !*pInst ) {
    PR_TRACE(( 0, prtERROR, "Cannot load library \"%s\" (%s)", szPluginName, dlerror () ));
    printf ( "Can't load %s : %s\n", szPluginName, dlerror () );
    return errMODULE_CANNOT_BE_LOADED;
  }

  tPluginInit aDllMain = (tPluginInit) ( dlsym ( (void*)*pInst, PLUGIN_INIT_ENTRY ) );
  if ( !aDllMain || !aDllMain ( (void*)*pInst, DLL_PROCESS_ATTACH, &aReserved ) ) {
    dlclose ( (void*)*pInst );
    PR_TRACE(( 0, prtERROR, "An error has occured during processing DllMain of %s\n", szPluginName ));    
    return  errMODULE_CANNOT_BE_LOADED;
  }

  *pEntryPoint = GetStartAddress( *pInst );
  PR_TRACE(( 0, prtSPAM, "LoadDllPlugin.GetProcAddress \"%s\" with %X", szPluginName, pEntryPoint ));
  return errOK;
}

//
// Unloading function
//

tERROR pr_call UnloadDllPlugin(hPLUGININSTANCE hInst)
{
  dlclose ( (void*)hInst );
  return errOK;
}

int checkNativePlugin ( const void* aPlugin )
{
  unsigned char * aRaw = (unsigned char *) aPlugin;
  if ( ( ( aRaw [ 0 ] == 0x7F ) && 
         ( aRaw [ 1 ] == 'E' ) && 
         ( aRaw [ 2 ] == 'L' ) && 
         ( aRaw [ 3 ] == 'F') ) || 
       ( ( aRaw [ 2 ] == 0x01 ) && 
         ( aRaw [ 3 ] == 0x0E ) ) ){
    PR_TRACE( (0,prtSPAM,"Native plugin found") );
    return cNATIVE_PLUGIN_FOUND;
  }
#if defined (__BIG_ENDIAN__)
  if ( ( aRaw [ 3 ] == 0xce ) && 
       ( aRaw [ 2 ] == 0xfa ) && 
       ( aRaw [ 1 ] == 0xed ) && 
       ( aRaw [ 0 ] == 0xfe ) )
#else
  if ( ( aRaw [ 0 ] == 0xce ) && 
       ( aRaw [ 1 ] == 0xfa ) && 
       ( aRaw [ 2 ] == 0xed ) && 
       ( aRaw [ 3 ] == 0xfe ) )
#endif // __BIG_ENDIAN__
  {
    PR_TRACE( (0,prtSPAM,"Native plugin found") );
    return cNATIVE_PLUGIN_FOUND;
  }
  return cUNEXPECTED_ERROR;
}

#endif // _MOD_LOAD_UNIX_INC_

#if ! defined (_MOD_LOAD_WIN32_INC_)
#define _MOD_LOAD_WIN32_INC_

#include <windows.h>

#if defined (_MSC_VER)
#pragma warning( disable: 4715)
#pragma hdrstop
#endif

#define TRY      try
#define EXCEPT   catch(...)

#if defined (OLD_METADATA_SUPPORT)
extern "C" tERROR pr_call GetDllMetaData( const tVOID* szFileName, tCODEPAGE cp, tPTR hModuleInfo ) {
  HMODULE  mod;
  char*    mem;
  HGLOBAL  res;
  HRSRC    src;
  tERROR   error = errMODULE_IS_BAD;
  HDATA*   aModuleInfo = (HDATA*) hModuleInfo;
  PR_TRACE_VARS;

  if ( cp == cCP_UNICODE )
    mod = LoadLibraryExW( UNI(szFileName), 0, LOAD_LIBRARY_AS_DATAFILE );
  else
    mod = LoadLibraryExA( MB(szFileName), 0, LOAD_LIBRARY_AS_DATAFILE );
  
  if ( mod ) {
    if ((0 != (src=FindResource(mod,MAKEINTRESOURCE(1),RT_RCDATA))) &&
        (0 != (res=LoadResource(mod,src))) &&
        (0 != (mem=(char*)LockResource(res))) )
    {
      HDATA aDataTree = 0;
      if ( KLDT_DeserializeFromMemoryUsingSWM(mem,SizeofResource(mod, src), &aDataTree ) ) {
        *aModuleInfo = aDataTree;
        error = errOK;
       }
      else {
        PR_TRACE_PREPARE(szFileName);
        PR_TRACE(( 0, prtERROR, "Description for module \"%s%S\" cannot be deserialized !", p1, p2 ));
        error = errUNEXPECTED;
      }
    }
    FreeLibrary( mod );
  }
  return error;
}

#endif // OLD_METADATA_SUPPORT

// ---
extern "C" tPluginInit pr_call GetStartAddress( hPLUGININSTANCE hInst ) {
  PIMAGE_DOS_HEADER pDosHeader;
  PIMAGE_NT_HEADERS pNTHeader;
  DWORD dwEntryPoint = 0;
  
  
  _try {
    if( *((LPDWORD)hInst) == cHEADER_SIGNATURE )
      return (tPluginInit)((tBYTE*)hInst+((PrFileHeader*)hInst)->dwEntryPoint);
    
    pDosHeader = (PIMAGE_DOS_HEADER)hInst;
    if( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE ) {
      PR_TRACE(( 0, prtERROR, "PrLoader: MZ header not found" ));
      return 0; // Does not point to a correct value
    }
    
    pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hInst+pDosHeader->e_lfanew);
    if( pNTHeader->Signature != IMAGE_NT_SIGNATURE ) {
			PR_TRACE(( 0, prtERROR, "PrLoader: PE header not found" ));
			return 0; // It is not a PE header position
    }
    
    return (tPluginInit)( (DWORD)hInst + pNTHeader->OptionalHeader.AddressOfEntryPoint );
  }
  __except ( EXCEPTION_EXECUTE_HANDLER ) {
    return 0;
  }
}

#if defined(SHOW_START_ADDRESS)
void ShowPluginStartAddress(tCHAR* pluginName, tDWORD Addr)
{
	HKEY hReg = NULL;
	if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, "Software\\KasperskyLab", &hReg))
	{
		DWORD type = REG_DWORD;
		DWORD bShowAddress;
		DWORD ShowAddress_size = sizeof(bShowAddress);
		if (ERROR_SUCCESS == RegQueryValueEx(hReg, "PrLoader_AInf", 0, &type, (LPBYTE) &bShowAddress, &ShowAddress_size))
		{
			if (bShowAddress)
			{
				char addrinfo[512];
				wsprintf(addrinfo, "0x%x %.260s\n", Addr, pluginName);
				OutputDebugString(addrinfo);
			}
		}
		RegCloseKey(hReg);
	}
}
#endif

// ---
extern "C" tERROR pr_call LoadDllPlugin( const tVOID* szPluginName, tCODEPAGE cp, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint ) {
	tERROR err;
	DWORD  old_error_mode;
  PR_TRACE_VARS;
	
  PR_TRACE_PREPARE(szPluginName);
	PR_TRACE(( 0, prtSPAM, "LoadDllPlugin: Loading native plugin \"%s%S\"", p1, p2 ));

	if ( !pInst || !pEntryPoint )
		return errPARAMETER_INVALID;
	
	*pInst = 0;
	*pEntryPoint = 0;
	old_error_mode = SetErrorMode( SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX );

	try {
    if ( cp == cCP_UNICODE )
  	  *pInst = (hPLUGININSTANCE)LoadLibraryW( UNI(szPluginName) );
    else
  	  *pInst = (hPLUGININSTANCE)LoadLibraryA( MB(szPluginName) );

		if ( *pInst ) {
			err = errOK;
			*pEntryPoint = GetStartAddress( *pInst );
      PR_TRACE_PREPARE(szPluginName);
			PR_TRACE(( 0, prtSPAM, "LoadDllPlugin.GetProcAddress \"%s%S\" with %X", p1, p2, pEntryPoint ));
		}
		else {
			err = errMODULE_CANNOT_BE_LOADED;
      PR_TRACE_PREPARE(szPluginName);
			PR_TRACE(( 0, prtERROR, "Cannot load library \"%s%S\" (win error - 0x%x)", p1, p2, GetLastError() ));
		}
	}
  catch(...) {
		*pInst = 0;
    *pEntryPoint = 0;
		err = errUNEXPECTED;
		PR_TRACE( (0,prtERROR,"LoadDllPlugin: exception catched!") );
	}
	SetErrorMode( old_error_mode );
	return err;
}

//
// Unloading function
//

tERROR pr_call UnloadDllPlugin(hPLUGININSTANCE hInst)
{
  FreeLibrary( (HINSTANCE)hInst );
  return errOK;
}


int checkNativePlugin ( tPTR aPlugin )
{
  unsigned char * aRaw = (unsigned char *) aPlugin;
  if ( ( aRaw [ 0 ] == 'M') && 
       ( aRaw [ 1 ] == 'Z' ) ) {
    PR_TRACE( (0,prtSPAM,"Native plugin found") );
    return cNATIVE_PLUGIN_FOUND;
  }
  return cUNEXPECTED_ERROR;
}
#endif // _MOD_LOAD_WIN32_INC_

// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  13 July 2004,  12:13 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Unix native file IO
// Author      -- petrovich
// File Name   -- plugin_nfio.cpp
// Additional info
//    File input/output for Unix
//
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <iface/e_ktime.h>
#include "unix_utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

// AVP Prague stamp end

hROOT g_root = 0;
tDWORD g_totalWrites = 0;

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call OS_Register( hROOT root );
extern tERROR pr_call ObjPtr_Register( hROOT root );
extern tERROR pr_call IO_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <plugin/p_win32_nfio.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <plugin/p_win32_nfio.h>
EXPORT_TABLE_END
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <iface/e_ktime.h> 
IMPORT_TABLE_END

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
  tDWORD count;

  switch( dwReason ) {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;

    case PRAGUE_PLUGIN_LOAD :
      g_root = (hROOT)hInstance;
      *pERROR = errOK;
      //resolve  my imports
      if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,&count,import_table,PID_WIN32_NFIO)) ) {
         PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
         return cFALSE;
      }

      //register my exports
      CALL_Root_RegisterExportTable( g_root, &count, export_table, PID_WIN32_NFIO );

      //register my custom property ids
      //if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
      //  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
      //  return cFALSE;
      //}

      // register my interfaces
      if ( PR_FAIL(*pERROR=OS_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"OS\" interface"));
        return cFALSE;
      }

      if ( PR_FAIL(*pERROR=ObjPtr_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"ObjPtr\" interface"));
        return cFALSE;
      }

      if ( PR_FAIL(*pERROR=IO_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"IO\" interface"));
        return cFALSE;
      }
      break;

    case PRAGUE_PLUGIN_UNLOAD :
      // free system resources
      // unregister my custom property ids -- you can drop it, kernel do it by itself
      // release    my imports		         -- you can drop it, kernel do it by itself
      // unregister my exports		         -- you can drop it, kernel do it by itself
      // unregister my interfaces          -- you can drop it, kernel do it by itself
      break;
  }
  return cTRUE;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, PrCopy )
// Extended method comment
//    Copies contents from source to destination. Both source and/or destination can be represented as hIO or hSTRING object. In case of destination is hIO object parameter "flags" must be zero (errPARAMETER_INVALID otherwise). If destination is hSTRING "flags" can be one of fPR_FILE_... (p_win32_nfio.h) constants.
//
//    Coping on reboot is supported
// Parameters are:
tERROR PrCopy( hOBJECT p_src, hOBJECT p_dst, tDWORD p_flags ) 
{

  // Place your code here
  tERROR anError = errOK;

  if ( !p_src || !p_dst )
    anError = errPARAMETER_INVALID;

  if ( PR_SUCC ( anError ) ) {
    tIID anIID = p_src->propGetDWord ( pgINTERFACE_ID );
  
    if ( ( anIID != IID_STRING ) && ( anIID != IID_IO ) ) 
      anError = errPARAMETER_INVALID;
  }

  tBOOL anOverwrite = cFALSE;
  if ( PR_SUCC ( anError ) ) {
    tIID anIID = p_dst->propGetDWord ( pgINTERFACE_ID );
  
    if ( ( anIID != IID_STRING ) && ( anIID != IID_IO ) ) 
      anError = errPARAMETER_INVALID;
    
//     if ( ( anIID == IID_IO ) && p_flags ) 
//       anError = errPARAMETER_INVALID;
    
    if ( p_flags & fPR_FILE_REPLACE_EXISTING )
      anOverwrite = cTRUE;
  }

  tCHAR aSource [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( anError ) ) 
    anError = p_src->propGetStr ( 0, pgOBJECT_FULL_NAME, aSource, sizeof ( aSource ) );

  tCHAR aDestination [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( anError ) ) 
    anError = p_dst->propGetStr ( 0, pgOBJECT_FULL_NAME, aDestination, sizeof ( aDestination ) );

  tDWORD aSystemError = 0;
  
  if ( PR_SUCC ( anError ) )
    anError = _copy ( aSource, aDestination, anOverwrite, aSystemError );

  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s src %s dest %s %terr", __FUNCTION__, aSource, aDestination, anError ));
  return anError;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, PrMoveFileEx )
// Parameters are:
tERROR PrMoveFileEx( hOBJECT p_src_name, hOBJECT p_dst_name, tDWORD p_flags ) 
{

  // Place your code here
  tERROR anError = errOK;

  if ( !p_src_name || !p_dst_name )
    anError = errPARAMETER_INVALID;

  if ( PR_SUCC ( anError ) ) {
    tIID anIID = p_src_name->propGetDWord ( pgINTERFACE_ID );
    if ( ( anIID != IID_STRING ) && ( anIID != IID_IO ) ) 
      anError = errPARAMETER_INVALID;
  }

  tBOOL anOverwrite = cFALSE;
  if ( PR_SUCC ( anError ) ) {
    tIID anIID = p_dst_name->propGetDWord ( pgINTERFACE_ID );
  
    if ( ( anIID != IID_STRING ) && ( anIID != IID_IO ) ) 
      anError = errPARAMETER_INVALID;
    
    if ( ( anIID == IID_IO ) && p_flags ) 
      anError = errPARAMETER_INVALID;
    
    if ( ( anIID == IID_STRING ) && ( p_flags & fPR_FILE_REPLACE_EXISTING ) )
      anOverwrite = cTRUE;
  }

  tCHAR aSource [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( anError ) ) 
    anError = p_src_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aSource, sizeof ( aSource ) );

  tCHAR aDestination [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( anError ) ) 
    anError = p_dst_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aDestination, sizeof ( aDestination ) );

  tDWORD aSystemError = 0;
  
  if ( PR_SUCC ( anError ) )
    anError = _rename ( aSource, aDestination, anOverwrite, aSystemError );

  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));  
  return anError;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, PrSetErrorMode )
// Parameters are:
tDWORD PrSetErrorMode( tDWORD p_errMode ) 
{

  // Place your code here
  return 0;
}
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrGetTempDir )
// Parameters are:
tERROR PrGetTempDir( hOBJECT p_result_path, const tVOID* p_sub_dir, tCODEPAGE p_cp ) {
	tERROR anError = errOK;
	const char* aTempDir = 0;
	aTempDir = getenv ( "TMP" );
	if ( !aTempDir )
		aTempDir = getenv ( "TEMP" );
	if ( !aTempDir )
		aTempDir = "/tmp";
	if ( access ( aTempDir, R_OK|W_OK) ) 
		return systemErrors2PragueErrors ( errno );

 	hSTRING p_path = (hSTRING)p_result_path;

	anError = CALL_String_ImportFromBuff ( p_path, 0, (tPTR)aTempDir, 0, cCP_ANSI, cSTRING_Z );
 	if ( PR_SUCC ( anError ) && p_sub_dir ) {
 		CALL_String_ProveLastSlash ( p_path );
 		anError = CALL_String_AddFromBuff ( p_path, 0, (tPTR)p_sub_dir, 0, p_cp, cSTRING_Z );
 		if ( PR_SUCC ( anError ) )
 			anError = PrCreateDir ( (hOBJECT)p_path );
 	}
	PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));
	return anError;
}
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrPeparseFileName )
// Parameters are:
tERROR PrPeparseFileName( hSTRING p_file_name ) {
  
  tERROR  anError = errNOT_IMPLEMENTED;
// 	tUINT   adv;
// 	tWCHAR  buff[20];
// 	tUINT   prefix_len;
// 	const tWCHAR* prefix;
	
// 	if ( !run_under_nt )
// 		return errOK;
	
// 	err = CALL_String_ExportToBuff( p_file_name, 0, cSTRING_WHOLE, buff, sizeof(buff), codepage, cSTRING_Z );
// 	if ( PR_FAIL(err) && (err != errBUFFER_TOO_SMALL) )
// 		return err;
	
// 	prefix = get_prefix( buff, sizeof(buff), &adv, &prefix_len );
// 	if ( !prefix ) // we do not need prefix
// 		return errOK;

// 	if ( adv ) // we already have some prefix
// 		return errOK;
	
// 	return CALL_String_ReplaceBuff( p_file_name, STR_RANGE(0,0), (tPTR)prefix, prefix_len*sizeof(tWCHAR), cCP_UNICODE, 0 );
  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));
  return anError;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, PrGetTempFileName )
// Parameters are:
tERROR PrGetTempFileName( hOBJECT p_path, const tVOID* p_prefix, tCODEPAGE p_cp ) {

  // Place your code here
  tERROR anError = errOK;

  if ( !p_path || !p_prefix )
    anError = errPARAMETER_INVALID;

  if ( PR_SUCC ( anError ) ) {
    tIID anIID = p_path->propGetDWord ( pgINTERFACE_ID );
    
    if ( anIID != IID_STRING )
      anError = errPARAMETER_INVALID;
  }

  tCHAR aPath [ MAX_PATH ] = {0};

  tDWORD aSize = 0;
  if ( PR_SUCC ( anError ) ) 
    anError = p_path->propGetStr ( &aSize, pgOBJECT_FULL_NAME, aPath, sizeof ( aPath ) );

  const char aTemplate [] = "XXXXXX";    
  const tCHAR* aPrefix = reinterpret_cast<const tCHAR*> ( p_prefix );
  if ( PR_FAIL ( anError ) || !aSize || !*aPath ) {
    char* aTempDir = 0;
    if ( ( ( aTempDir = getenv ( "TMP" ) ) || ( aTempDir = getenv ( "TEMP" ) ) ) && 
         access ( aTempDir, R_OK|W_OK) == 0 ) {
      strncat ( aPath, aTempDir, sizeof ( aPath ) - ( ( aPrefix ) ? strlen ( aPrefix ) : 0 ) - strlen ( aTemplate ) - 2 );
    }
    else
      strcat ( aPath, "/tmp" );
    anError = errOK;
  }

  if ( PR_SUCC ( anError ) ) {
    strcat ( aPath, "/" );
    if ( aPrefix )
      strcat ( aPath, aPrefix );
    strcat ( aPath, aTemplate );
    tINT aFD = mkstemp ( aPath );
    if ( aFD == -1 ) 
      anError = systemErrors2PragueErrors ( errno );
    else {
      close ( aFD );
      aFD = -1;
    }
  } 
  
  if ( PR_SUCC ( anError ) ) 
    anError = reinterpret_cast <hSTRING> ( p_path )->ImportFromBuff ( 0, aPath, 0, cCP_ANSI, cSTRING_Z);    

  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));
  return anError;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, PrCreateDir )
// Parameters are:
tERROR PrCreateDir( hOBJECT p_path ) 
{
  // Place your code here
  tERROR anError = errOK;

  if ( !p_path )
    anError = errPARAMETER_INVALID;

  if ( PR_SUCC ( anError ) ) {
    tIID anIID = p_path->propGetDWord ( pgINTERFACE_ID );
    if ( anIID != IID_STRING )
      anError = errPARAMETER_INVALID;
  }

  tCHAR aPath [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( anError ) ) 
    anError = p_path->propGetStr ( 0, pgOBJECT_FULL_NAME, aPath, sizeof ( aPath ) );

  if ( PR_SUCC ( anError ) && mkdir ( aPath, S_IRWXU | S_IRWXG ) )
    anError = systemErrors2PragueErrors ( errno );

  if ( PR_FAIL ( anError ) && ( errno != EEXIST ) ) {
	  anError = errOK;
	  tCHAR* aPos = aPath;
	  do {
		  while ( *aPos && ( *aPos != '/' ) ) ++aPos;
		  *aPos = 0;
		  if ( mkdir ( aPath, S_IRWXU | S_IRWXG ) && ( errno != EEXIST ) ) {
			  anError = systemErrors2PragueErrors ( errno );
			  break;
		  }
		  
		  *aPos++ = '/';
	  }
	  while ( *aPos );
  }
	  
  
  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s path %s %terr", __FUNCTION__, aPath, anError ));
  return anError;
}
// AVP Prague stamp end

void SIGCHLD_handler (int)
{
  while ( waitpid ( -1, 0, WNOHANG ) );
}

// AVP Prague stamp begin( Plugin export method implementation, PrExecute )
// Parameters are:
tERROR PrExecute( hOBJECT p_executable, hSTRING p_cmd_line, hSTRING p_start_dir, tDWORD* p_result, tDWORD p_flags ) 
{

  // Place your code here
  tERROR anError = errOK;
  
  if ( !p_executable )
    anError = errPARAMETER_INVALID;
  
  tCHAR anExecutable [ MAX_PATH ] = {0};

  if ( PR_SUCC ( anError ) )
    anError =  p_executable->propGetStr ( 0, pgOBJECT_FULL_NAME, anExecutable, sizeof ( anExecutable ) );
  
  tCHAR aCmdLine [ MAX_PATH ] = {0};

  if ( PR_SUCC ( anError ) && p_cmd_line )
    anError =  p_cmd_line->propGetStr ( 0, pgOBJECT_FULL_NAME, aCmdLine, sizeof ( aCmdLine ) );    

  tCHAR aStartDir [ MAX_PATH ] = {0};

  if ( PR_SUCC ( anError ) && p_start_dir )
    anError =  p_start_dir->propGetStr ( 0, pgOBJECT_FULL_NAME, aStartDir, sizeof ( aStartDir ) );    

  switch ( pid_t aPid = fork () ) {
  case 0: 
    if ( p_start_dir && chdir ( aStartDir ) )
      exit ( -1 );
    if ( execlp ( anExecutable, aCmdLine, aCmdLine, 0 ) ) 
      exit ( -1 );
    break;
  case -1:
    anError = systemErrors2PragueErrors ( errno );
    break;
  default: 
    int aStatus = 0;
    if ( ( p_flags & fPR_EXECUTE_WAIT ) && ( waitpid ( aPid, &aStatus, 0 ) == -1 ) )
      anError = systemErrors2PragueErrors ( errno );      
    if ( WIFEXITED ( aStatus ) )
      aStatus = WEXITSTATUS ( aStatus );
    if ( p_result )
      *p_result = aStatus;
    break;
  }
  
  if ( !( p_flags & fPR_EXECUTE_WAIT ) )
    signal ( SIGCHLD, SIGCHLD_handler );

  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));    
  return anError;
}

tERROR PrGetDiskFreeSpace( hOBJECT p_src, tQWORD* p_result ) {
  tERROR anError = errNOT_IMPLEMENTED;
  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));
  return anError;
}

tERROR PrRemoveDir( hOBJECT p_path ) {
  tERROR anError = errNOT_IMPLEMENTED;
  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));
  return anError;
}

tERROR PrDeleteFile( hSTRING p_file_to_delete, tDWORD p_reserved_flags ) {
  tERROR anError = errNOT_IMPLEMENTED;
  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s %terr", __FUNCTION__, anError ));
  return anError;
}

tERROR PrGetTotalWrites( tDWORD* p_size ) {

  // Place your code here
  if( p_size )
    *p_size = g_totalWrites;
  PR_TRACE ((g_root, prtNOTIFY, "nfio_exp\t %s", __FUNCTION__ ));
  return errOK;
}
// AVP Prague stamp end




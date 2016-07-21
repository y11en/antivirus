// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Tuesday,  27 June 2006,  12:23 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- plugin_win32_nfio.c
// Additional info
//    File input/output for Win32
// -------------------------------------------
// AVP Prague stamp end


#define PR_IMPEX_DEF
#define IMPEX_DECL_IMPORT_VARS


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_token.h>
#include <Prague/plugin/p_fsdrvplgn.h>
// AVP Prague stamp end

#include "win32_os.h"
#include "win32_objptr.h"
#include "win32_io.h"

#define  IMPEX_IMPORT_LIB
#  include <Prague/plugin/p_fsdrvplgn.h> 
#  include <Prague/plugin/p_string.h> 

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
	#include <Prague/iface/e_ktime.h> 
	#include <Prague/iface/e_loader.h> 
IMPORT_TABLE_END

IMPORT_TABLE( import_table_fsdrv )
	#include <Prague/plugin/p_fsdrvplgn.h> 
IMPORT_TABLE_END

#undef   IMPEX_IMPORT_LIB
#undef   IMPEX_TABLE_CONTENT

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call OS_Register( hROOT root );
extern tERROR pr_call ObjPtr_Register( hROOT root );
extern tERROR pr_call IO_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <Prague/plugin/p_win32_nfio.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <Prague/plugin/p_win32_nfio.h>
EXPORT_TABLE_END
// AVP Prague stamp end

#include <stdio.h>

tERROR NFIO_RemoveDir( tPTR p_path, tCODEPAGE cp );

//NTSYSAPI NTSTATUS (NTAPI* tZwCreateFile)( HANDLE* FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize OPTIONAL, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions,	PVOID EaBuffer OPTIONAL, ULONG EaLength );


hROOT  g_root;
tBOOL  g_bFSDRVImportresolved = cFALSE;
tDWORD g_dwTotalWrites = 0;

tCreateFile           fCreateFile;
//tCreateFile           fNTCreateFile;
tDeleteFile           fDeleteFile;
//tGetVolumeInformation fGetVolumeInformation;
tGetFileAttributes    fGetFileAttributes;
tGetFileAttributesEx  fGetFileAttributesEx;
tSetFileAttributes    fSetFileAttributes;
tFindFirstFile        fFindFirstFile;
tFindNextFile         fFindNextFile;
tFindClose            fFindClose;
tCopyFile             fCopyFile;
tMoveFile             fMoveFile;
tMoveFileEx           fMoveFileEx;
tCreateDirectory      fCreateDirectory;
tRemoveDirectory      fRemoveDirectory;
tGetTempPath          fGetTempPath;
tGetTempFileName      fGetTempFileName;
tGetDriveType         fGetDriveType;
tGetFileSecurity      fGetFileSecurity;
tSetFileSecurity      fSetFileSecurity;
tGetFullPathName      fGetFullPathName;
tGetLongPathName      fGetLongPathName;
tGetShortPathName     fGetShortPathName;

//#if defined (_WIN32)
//// ---
//WINBASEAPI HANDLE WINAPI NTCreateFile( const tVOID* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ) {
//	return fNTCreateFile( lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
//}
//#endif


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

tERROR pr_call cIO_Register( hROOT root );

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* error ) {
	hROOT root;
	HANDLE advapi;
	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;
			
		case PRAGUE_PLUGIN_LOAD :
			
			g_root = root = (hROOT)hInstance;
			#if defined (_WIN32)
				run_under_nt = !(0x80000000 & GetVersion());
				if ( run_under_nt ) {
					HKEY   key = 0;
					tDWORD value = 1;
					if ( ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\kernel",&key) ) {
						tDWORD type = REG_DWORD;
						tDWORD size = sizeof(tDWORD);
						RegQueryValueEx( key, "ObCaseInsensitive", 0, &type, (BYTE*)&value, &size );
						RegCloseKey( key );
					}
					*(tCODEPAGE*)&codepage = cCP_UNICODE;
					g_case_sensitive = !value;
				}
				else {
					*(tCODEPAGE*)&codepage = cCP_ANSI;
					g_case_sensitive = cFALSE;
				}
				
				win_kernel = LoadLibrary( "kernel32.dll" );
				if ( !win_kernel )
					PR_TRACE(( (hROOT)hInstance, prtERROR, "Cannot load Windows kernel32.dll" ));
				else if ( run_under_nt ) {

					//fGetVolumeInformation = (tGetVolumeInformation)GetProcAddress( win_kernel, "GetVolumeInformationW" );
					//win_nt = LoadLibrary( "ntdll.dll" );

					//fCreateFile           = NTCreateFile;
					//fNTCreateFile         = (tCreateFile)          GetProcAddress( win_kernel, "CreateFileW" );

					fCreateFile           = (tCreateFile)          GetProcAddress( win_kernel, "CreateFileW" );
					fDeleteFile           = (tDeleteFile)          GetProcAddress( win_kernel, "DeleteFileW" );
					fGetFileAttributes    = (tGetFileAttributes)   GetProcAddress( win_kernel, "GetFileAttributesW" );
					fGetFileAttributesEx  = (tGetFileAttributesEx) GetProcAddress( win_kernel, "GetFileAttributesExW" );
					fSetFileAttributes    = (tSetFileAttributes)   GetProcAddress( win_kernel, "SetFileAttributesW" );
					fFindFirstFile        = (tFindFirstFile)       GetProcAddress( win_kernel, "FindFirstFileW" );
					fFindNextFile         = (tFindNextFile)        GetProcAddress( win_kernel, "FindNextFileW" );
					fFindClose            = (tFindClose)           GetProcAddress( win_kernel, "FindClose" );
					fCopyFile             = (tCopyFile)            GetProcAddress( win_kernel, "CopyFileW" );
					fMoveFileEx           = (tMoveFileEx)          GetProcAddress( win_kernel, "MoveFileExW" );
					fMoveFile             = (tMoveFile)            GetProcAddress( win_kernel, "MoveFileW" );
					fCreateDirectory      = (tCreateDirectory)     GetProcAddress( win_kernel, "CreateDirectoryW" );
					fRemoveDirectory      = (tRemoveDirectory)     GetProcAddress( win_kernel, "RemoveDirectoryW" );
					fGetTempPath          = (tGetTempPath)         GetProcAddress( win_kernel, "GetTempPathW" );
					fGetTempFileName      = (tGetTempFileName)     GetProcAddress( win_kernel, "GetTempFileNameW" );
					fGetDriveType         = (tGetDriveType)        GetProcAddress( win_kernel, "GetDriveTypeW" );
					fGetFullPathName      = (tGetFullPathName)     GetProcAddress( win_kernel, "GetFullPathNameW" );
					fGetLongPathName      = (tGetLongPathName)     GetProcAddress( win_kernel, "GetLongPathNameW" );
					fGetShortPathName     = (tGetShortPathName)    GetProcAddress( win_kernel, "GetShortPathNameW" );
					
					if( advapi = GetModuleHandle( "advapi32.dll" ) ) {
						fGetFileSecurity  = (tGetFileSecurity)     GetProcAddress( advapi, "GetFileSecurityW" );
						fSetFileSecurity  = (tSetFileSecurity)     GetProcAddress( advapi, "SetFileSecurityW" );
					}

					if (PR_SUCC(CALL_Root_ResolveImportTable(root, NULL, import_table_fsdrv, PID_NATIVE_FIO)))
					{
						PR_TRACE(( root, prtNOTIFY, "import_table_fsdrv exports imported" ));
						g_bFSDRVImportresolved = cTRUE;
					}
					else
					{
						PR_TRACE(( root, prtERROR, "import_table_fsdrv exports cannot be imported" ));
					}
				}
				else {
					//fGetVolumeInformation = (tGetVolumeInformation)GetProcAddress( win_kernel, "GetVolumeInformationA" );
					fCreateFile           = (tCreateFile)          GetProcAddress( win_kernel, "CreateFileA" );
					fDeleteFile           = (tDeleteFile)          GetProcAddress( win_kernel, "DeleteFileA" );
					fGetFileAttributes    = (tGetFileAttributes)   GetProcAddress( win_kernel, "GetFileAttributesA" );
					fGetFileAttributesEx  = (tGetFileAttributesEx) GetProcAddress( win_kernel, "GetFileAttributesExA" );
					fSetFileAttributes    = (tSetFileAttributes)   GetProcAddress( win_kernel, "SetFileAttributesA" );
					fFindFirstFile        = (tFindFirstFile)       GetProcAddress( win_kernel, "FindFirstFileA" );
					fFindNextFile         = (tFindNextFile)        GetProcAddress( win_kernel, "FindNextFileA" );
					fFindClose            = (tFindClose)           GetProcAddress( win_kernel, "FindClose" );
					fCopyFile             = (tCopyFile)            GetProcAddress( win_kernel, "CopyFileA" );
					fMoveFile             = (tMoveFile)            GetProcAddress( win_kernel, "MoveFileA" );
					fCreateDirectory      = (tCreateDirectory)     GetProcAddress( win_kernel, "CreateDirectoryA" );
					fRemoveDirectory      = (tRemoveDirectory)     GetProcAddress( win_kernel, "RemoveDirectoryA" );
					fGetTempPath          = (tGetTempPath)         GetProcAddress( win_kernel, "GetTempPathA" );
					fGetTempFileName      = (tGetTempFileName)     GetProcAddress( win_kernel, "GetTempFileNameA" );
					fGetDriveType         = (tGetDriveType)        GetProcAddress( win_kernel, "GetDriveTypeA" );
					fGetFullPathName      = (tGetFullPathName)     GetProcAddress( win_kernel, "GetFullPathNameA" );
					fGetLongPathName      = (tGetLongPathName)     GetProcAddress( win_kernel, "GetLongPathNameA" ); // there is no "A" at the end!!!
					fGetShortPathName     = (tGetShortPathName)    GetProcAddress( win_kernel, "GetShortPathNameA" );
				}
			#else
				run_under_nt = cFALSE;
				*(tCODEPAGE*)&codepage = cCP_ANSI;
				
				win_kernel = LoadLibrary( "libcompat.so" );
				if ( !win_kernel )
					PR_TRACE(( (hROOT)hInstance, prtERROR, "Cannot load Windows kernel32.dll" ));
				else {
					//fGetVolumeInformation = (tGetVolumeInformation)GetProcAddress( win_kernel, "GetVolumeInformationA" );
					fCreateFile           = (tCreateFile)          GetProcAddress( win_kernel, "CreateFile" );
					fDeleteFile           = (tDeleteFile)          GetProcAddress( win_kernel, "DeleteFile" );
					fGetFileAttributes    = (tGetFileAttributes)   GetProcAddress( win_kernel, "GetFileAttributes" );
					fGetFileAttributesEx  = (tGetFileAttributesEx) GetProcAddress( win_kernel, "GetFileAttributesEx" );
					fSetFileAttributes    = (tSetFileAttributes)   GetProcAddress( win_kernel, "SetFileAttributes" );
					fFindFirstFile        = (tFindFirstFile)       GetProcAddress( win_kernel, "FindFirstFile" );
					fFindNextFile         = (tFindNextFile)        GetProcAddress( win_kernel, "FindNextFile" );
					fFindClose            = (tFindClose)           GetProcAddress( win_kernel, "FindClose" );
					fCopyFile             = (tCopyFile)            GetProcAddress( win_kernel, "CopyFile" );
					fMoveFile             = (tMoveFile)            GetProcAddress( win_kernel, "MoveFile" );
					fGetTempPath          = (tGetTempPath)         GetProcAddress( win_kernel, "GetTempPath" );
					fGetTempFileName      = (tGetTempFileName)     GetProcAddress( win_kernel, "GetTempFileName" );
					fGetFullPathName      = (tGetFullPathName)     GetProcAddress( win_kernel, "GetFullPathName" );
					fCreateDirectory      = (tCreateDirectory)     GetProcAddress( win_kernel, "CreateDirectory" );
					fRemoveDirectory      = (tRemoveDirectory)     GetProcAddress( win_kernel, "RemoveDirectory" );
					fGetLongPathName      = (tGetLongPathName)     GetProcAddress( win_kernel, "GetLongPathName" );
					fGetShortPathName     = (tGetShortPathName)    GetProcAddress( win_kernel, "GetShortPathName" );
				}
			#endif
		
			if ( !fCreateFile )
				PR_TRACE(( root, prtERROR, "CreateFile export pointer cannot be imported from the Windows kernel" ));
			
			if ( !fFindFirstFile )
				PR_TRACE(( root, prtERROR, "FindFirstFile export pointer cannot be imported from the Windows kernel" ));
			
			if ( !fFindNextFile )
				PR_TRACE(( root, prtERROR, "FindNextFile export pointer cannot be imported from the Windows kernel" ));
			
			CALL_Root_RegisterExportTable(root, NULL, export_table, PID_NATIVE_FIO );
			
			if ( PR_FAIL( CALL_Root_ResolveImportTable(root, NULL, import_table, PID_NATIVE_FIO) ) )
				PR_TRACE(( root, prtERROR, "e_ktime exports cannot be imported" ));
			
			// register my interfaces
			if ( PR_FAIL(*error=OS_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"OS\" interface"));
				return cFALSE;
			}
			
			if ( PR_FAIL(*error=ObjPtr_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"ObjPtr\" interface"));
				return cFALSE;
			}
			
			if ( PR_FAIL(*error=cIO_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"IO\" interface"));
				return cFALSE;
			}
			// register my custom property ids
			// register my exports
			// resolve  my imports
			// use some system resources
			break;
			
		case PRAGUE_PLUGIN_PRE_UNLOAD :
			CALL_Root_ReleaseImportTable( g_root, NULL, import_table_fsdrv, PID_NATIVE_FIO );
			CALL_Root_ReleaseImportTable( g_root, NULL, import_table,       PID_NATIVE_FIO );
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			if ( win_kernel )
				FreeLibrary( win_kernel );
			
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



// ---
tERROR openByString( hIO* io, hOBJECT src, tBOOL rw, tBOOL replace, tBOOL name_preparse )
{
	tERROR err = CALL_SYS_ObjectCreate( g_root, io, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY );
	if( PR_SUCC(err) )
	{
		tDWORD dwOpenMode = 0;
		if( rw )
		{
			dwOpenMode |= replace ? (fOMODE_OPEN_ALWAYS|fOMODE_TRUNCATE) : fOMODE_CREATE_IF_NOT_EXIST;
			dwOpenMode |= fOMODE_SHARE_DENY_RW | fOMODE_SHARE_DENY_DELETE;
		}
		else
		{
			dwOpenMode |= fOMODE_OPEN_IF_EXIST;
		}
		if(name_preparse)
			dwOpenMode |= fOMODE_FILE_NAME_PREPARSING;

		err = CALL_SYS_PropertySetDWord( *io, pgOBJECT_OPEN_MODE, dwOpenMode );
		if( PR_SUCC(err) && rw ) err = CALL_SYS_PropertySetDWord(*io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
	}
	if( PR_SUCC(err) ) err = CALL_String_ExportToProp( (hSTRING)src, 0, cSTRING_WHOLE, (hOBJECT)*io, pgOBJECT_NAME );
	if( PR_SUCC(err) ) err = CALL_SYS_ObjectCreateDone( *io );
	if( PR_FAIL(err) && *io )
	{
		CALL_SYS_ObjectClose( *io );
		*io = 0;
	}	
	return err;
}


// ---
tERROR copyOnReboot( hOBJECT src, hOBJECT dst, tDWORD flags ) {

	hOBJECT dst_io = 0;
	hSTRING dst_path = 0;
	tCHAR   dst_name[MAX_PATH];
	tCHAR*  pdst_name = dst_name;
	tERROR  err = CALL_SYS_ObjectCreateQuick( g_root, &dst_path, IID_STRING, PID_ANY, SUBTYPE_ANY );

	if ( PR_SUCC(err) )
		err = _ExtractName( dst, &pdst_name, sizeof(dst_name), pgOBJECT_FULL_NAME, 0, 0 );

	if ( !(flags & fPR_FILE_REPLACE_EXISTING) ) {
		HANDLE handle = fCreateFile( dst_name, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( INVALID_HANDLE_VALUE != handle ) {
			CloseHandle( handle );
			err = errOBJECT_ALREADY_EXISTS;
		}
	}

	if ( PR_SUCC(err) )
		err = CALL_SYS_ObjectCreate( g_root, &dst_io, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY );
	if ( PR_SUCC(err) )
		err = CALL_SYS_PropertySetStr( dst_io, 0, pgOBJECT_FULL_NAME, pdst_name, 0, run_under_nt ? cCP_UNICODE : cCP_ANSI );
	if ( PR_SUCC(err) )
		err = CALL_String_ImportFromProp( dst_path, 0, dst_io, pgOBJECT_PATH );
	
	if ( dst_io )
		CALL_SYS_ObjectClose( dst_io );
	if ( (pdst_name != dst_name) && pdst_name )
		CALL_SYS_ObjHeapFree( g_root, pdst_name );

	if ( PR_SUCC(err) )
		err = PrGetTempFileName( (hOBJECT)dst_path, "pr_", cCP_ANSI );
	if ( PR_SUCC(err) )
		err = PrCopy( src, (hOBJECT)dst_path, fPR_FILE_REPLACE_EXISTING );
	if ( PR_SUCC(err) )
		err = PrMoveFileEx( (hOBJECT)dst_path, dst, flags );
	return err;
}



// ---
tERROR _revert_to_process_impersonation( hOBJECT obj, hTOKEN* ptoken ) {
	tERROR err = CALL_SYS_ObjectCreateQuick( obj, ptoken, IID_TOKEN, PID_SYSTEM, SUBTYPE_ANY );
	if ( PR_SUCC(err) )
		err = CALL_Token_Revert( *ptoken );
	return err;
}



// ---
tERROR pr_call _back_to_user( hTOKEN token ) {
	if ( !token )
		return errOK;
	return CALL_Token_Impersonate( token, 0 );
}



// ---
tERROR pr_call _copy_impl( hOBJECT p_src, tBOOL src_is_io, hOBJECT p_dst, tBOOL dst_is_io, tBOOL* try_default_impersonation, tDWORD p_flags ) {

	tERROR err;
	hIO    src = 0;
	hIO    dst = 0;
	tQWORD size;

	if ( try_default_impersonation )
		*try_default_impersonation = cFALSE;

	if ( !src_is_io && !dst_is_io && fCopyFile ) {
		tCHAR  dst[MAX_PATH];
		tCHAR  src[MAX_PATH];
		tCHAR* pdst = dst;
		tCHAR* psrc = src;
		err = _ExtractName( p_src, (tPTR)&psrc, sizeof(src), 0, 0, 0 );
		if( PR_SUCC(err) )
			err = _ExtractName( p_dst, (tPTR)&pdst, sizeof(dst), 0, 0, 0 );
		if ( PR_SUCC(err) && !fCopyFile(psrc,pdst,(p_flags&fPR_FILE_REPLACE_EXISTING)?FALSE:TRUE) ) {
			if ( try_default_impersonation )
				*try_default_impersonation = cTRUE;
			err = _ConvertWin32Err( GetLastError() );
		}
		if ( psrc != src && psrc )
			CALL_SYS_ObjHeapFree( g_root, psrc );
		if ( pdst != dst && pdst )
			CALL_SYS_ObjHeapFree( g_root, pdst );
		return err;
	}

	if ( src_is_io )
		src = (hIO)p_src;
	else if ( PR_FAIL(err=openByString(&src,p_src,cFALSE,cFALSE,(p_flags&fPR_FILE_NAME_PREPARSING)?cTRUE:cFALSE) ) ) {
		if ( try_default_impersonation )
			*try_default_impersonation = cTRUE;
		return err;
	}

	if ( dst_is_io )
		dst = (hIO)p_dst;
  else if ( PR_FAIL(err=openByString(&dst,p_dst,cTRUE,(p_flags&fPR_FILE_REPLACE_EXISTING)?cTRUE:cFALSE,(p_flags&fPR_FILE_NAME_PREPARSING)?cTRUE:cFALSE) ) ) {
		if ( try_default_impersonation )
			*try_default_impersonation = cTRUE;
		if ( !src_is_io )
			CALL_SYS_ObjectClose( src );
		return err;
	}

	err = CALL_IO_GetSize( src, &size, IO_SIZE_TYPE_EXPLICIT );
	if ( PR_SUCC(err) ) {
		tQWORD already = 0;
		tDWORD read;
		tBYTE  buff[0x400];
		tDWORD bsize;
		tPTR   bptr;

		bsize = _toui32(size / 10) + !!_toui32(size % 10);
		if ( bsize <= sizeof(buff) ) {
			bsize = sizeof(buff);
			bptr = buff;
		}
		else {
			if ( bsize > 0x8000 )
				bsize = 0x8000;
			err = CALL_SYS_ObjHeapAlloc( g_root, &bptr, bsize );
		}
		for( already=0; PR_SUCC(err) && (already<size); already+=read ) {
			err = CALL_IO_SeekRead( src, &read, already, bptr, bsize );
			if ( PR_SUCC(err) ) {
				tDWORD written = 0;
				err = CALL_IO_SeekWrite( dst, &written, already, bptr, read );
				if ( PR_SUCC(err) && (written != read) )
					err = errOBJECT_WRITE;
			}
		}
		if ( bptr != buff )
			CALL_SYS_ObjHeapFree( g_root, bptr );
	}
	if ( !src_is_io )
		CALL_SYS_ObjectClose( src );
	if ( !dst_is_io )
		CALL_SYS_ObjectClose( dst );
	return err;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrCopy )
// Parameters are:
tERROR PrCopy( hOBJECT p_src, hOBJECT p_dst, tDWORD p_flags ) {

	tBOOL  src_is_io;
	tBOOL  dst_is_io;
	tBOOL  try_default_impersonation = cFALSE;
	tERROR err = CALL_SYS_ObjectCheck( g_root, p_src, IID_IO, PID_ANY, SUBTYPE_ANY, cTRUE );

	if ( PR_SUCC(err) )
		src_is_io = cTRUE;
	else if ( err != errINTERFACE_INCOMPATIBLE )
		return errPARAMETER_INVALID;
	else if ( PR_SUCC(err=CALL_SYS_ObjectCheck(g_root,p_src,IID_STRING,PID_ANY,SUBTYPE_ANY,cTRUE)) )
		src_is_io = cFALSE;
	else
		return errPARAMETER_INVALID;

	err = CALL_SYS_ObjectCheck( g_root, p_dst, IID_IO, PID_ANY, SUBTYPE_ANY, cTRUE );
	if ( PR_SUCC(err) )
		dst_is_io = cTRUE;
	else if ( err != errINTERFACE_INCOMPATIBLE )
		return errPARAMETER_INVALID;
	else if ( PR_SUCC(err=CALL_SYS_ObjectCheck(g_root,p_dst,IID_STRING,PID_ANY,SUBTYPE_ANY,cTRUE)) )
		dst_is_io = cFALSE;
	else
		return errPARAMETER_INVALID;

	if ( p_flags & fPR_FILE_DELAY_UNTIL_REBOOT ) {
		if ( dst_is_io )
			return errPARAMETER_INVALID;
		return copyOnReboot( p_src, p_dst, p_flags );
	}

	err = _copy_impl( p_src, src_is_io, p_dst, dst_is_io, &try_default_impersonation, p_flags );
	if ( PR_FAIL(err) && (p_flags & fPR_COPY_TRY_DEFAULT_IMPERSONATION) && try_default_impersonation ) {
		hTOKEN token = 0;
		if ( PR_SUCC(_revert_to_process_impersonation(p_src,&token)) )
			err = _copy_impl( p_src, src_is_io, p_dst, dst_is_io, 0, p_flags );
		_back_to_user( token );
	}

	return err;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrMoveFileEx )
// Parameters are:
tERROR PrMoveFileEx( hOBJECT p_src_name, hOBJECT p_dst_name, tDWORD p_flags ) {
	tCHAR  dst[MAX_PATH];
	tCHAR  src[MAX_PATH];
	tCHAR* pdst = dst;
	tCHAR* psrc = src;
	tERROR error;

	// Place your code here
	if( !p_src_name )
		return errPARAMETER_INVALID;

	error=_ExtractName(p_src_name,(tPTR)&psrc,sizeof(src),0,0,0);
	if( PR_SUCC(error) ) {
		if( p_dst_name )
			error = _ExtractName(p_dst_name,(tPTR)&pdst,sizeof(dst),0,0,0);
		else
			pdst = NULL;
	}

	if( PR_FAIL(error) )
		;
	else if( fMoveFileEx ) {
		if( !fMoveFileEx(psrc, pdst, p_flags) )
			error = _ConvertWin32Err( GetLastError() );
	}
#if !defined (_WIN32)
	else
		error = errNOT_IMPLEMENTED;
#else // if defined (_WIN32)
	else { // Construct the full pathname of the WININIT.INI file.
		tCHAR         sWinDir[MAX_PATH], sRenameLine[0x400], *psWinInit;
		tCHAR         sSrcPath[MAX_PATH], sDstPath[MAX_PATH];
		HANDLE        hFile, hFileMap;
		const tCHAR   sRenameSec[] = "[Rename]\r\n";
		tDWORD        nRenameLine, nFileSize;

		GetShortPathNameA(psrc, sSrcPath, sizeof(sSrcPath));
		if( pdst )
			GetShortPathNameA(pdst, sDstPath, sizeof(sDstPath));
		else
			strcpy_s(sDstPath, sizeof(sDstPath), "NUL");

		GetWindowsDirectory( sWinDir, MAX_PATH );
		strcat_s(sWinDir, sizeof(sWinDir), "\\WinInit.Ini");

		// Open/Create the WININIT.INI file.
		hFile = CreateFile(sWinDir, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );

		if( hFile == INVALID_HANDLE_VALUE )
			error = errUNEXPECTED;
		else {
			nRenameLine = sprintf_s(sRenameLine, sizeof(sRenameLine), "%s=%s\r\n", sDstPath, sSrcPath);
			nFileSize = GetFileSize(hFile, NULL);
			hFileMap = CreateFileMapping( hFile, NULL, PAGE_READWRITE, 0,
				nFileSize + nRenameLine + sizeof(sRenameSec), NULL );

			if( hFileMap != NULL )
				psWinInit = (tCHAR*)MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

			if( psWinInit != NULL ) {
				tDWORD  nRenameLinePos;
				tCHAR *	psRenameSecInFile;

				// Search for the [Rename] section in the file.
				psRenameSecInFile = strstr(psWinInit, sRenameSec);
				if( !psRenameSecInFile ) {
					// There is no [Rename] section in the WININIT.INI file.
					// We must add the section too.
					nFileSize += sprintf_s(&psWinInit[nFileSize], strlen(sRenameSec) + 1, "%s", sRenameSec);
					nRenameLinePos = nFileSize;
				}
				else {
					// We found the [Rename] section, shift all the lines down
					tCHAR* psFirstRenameLine = strchr( psRenameSecInFile, '\n' );
					// Shift the contents of the file down to make room for
					// the newly added line.  The new line is always added
					// to the top of the list.
					psFirstRenameLine++;   // 1st char on the next line
					memmove(psFirstRenameLine + nRenameLine, psFirstRenameLine,
											psWinInit + nFileSize - psFirstRenameLine);
					nRenameLinePos = _toui32(psFirstRenameLine - psWinInit);
				}

				// Insert the new line
				memcpy(&psWinInit[nRenameLinePos], sRenameLine, nRenameLine);

				UnmapViewOfFile(psWinInit);

				// Calculate the true, new size of the file.
				nFileSize += nRenameLine;
			}

			if( hFileMap )
				CloseHandle(hFileMap);

			// Force the end of the file to be the calculated, new size.
			SetFilePointer(hFile, nFileSize, NULL, FILE_BEGIN);
			SetEndOfFile(hFile);
			CloseHandle(hFile);

			if( !psWinInit )
				error = errUNEXPECTED;
		}
	}
#endif

	if ( pdst != dst && pdst )
		CALL_SYS_ObjHeapFree( p_src_name, pdst );

	if ( psrc != src && psrc )
		CALL_SYS_ObjHeapFree( p_src_name, psrc );

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrSetErrorMode )
// Parameters are:
tDWORD PrSetErrorMode( tDWORD p_errMode ) {
	return SetErrorMode( p_errMode ); // SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX
}
// AVP Prague stamp end



// ---
tERROR _get_env_var( hSTRING str, tWCHAR* var ) {
	tERROR err = CALL_String_ImportFromBuff( str, 0, var, 0, cCP_UNICODE, 0 );
	if ( PR_SUCC(err) )
		err = CALL_SYS_SendMsg( g_root, pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, str, 0, 0 );
	if ( err != errOK_DECIDED )
		return errNOT_FOUND;
	return errOK;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrGetTempDir )
// Parameters are:
tERROR PrGetTempDir( hOBJECT p_result_path, const tVOID* p_sub_dir, tCODEPAGE p_cp ) {
	tERROR err;
	tDWORD size = 0;
	hSTRING p_path = 0;

	if ( PR_FAIL(CALL_SYS_ObjectCheck(g_root,p_result_path,IID_STRING,PID_ANY,SUBTYPE_ANY,cTRUE)) )
		return errPARAMETER_INVALID;

	err = _get_env_var( (hSTRING)p_result_path, L"%Temp%" );
	if ( PR_FAIL(err) )
		err = _get_env_var( (hSTRING)p_result_path, L"%TMP%" );
	if ( PR_FAIL(err) )
		err = _get_env_var( (hSTRING)p_result_path, L"%TMPDIR%" );

	p_path = (hSTRING)p_result_path;
	if( PR_FAIL(err) && fGetTempPath ) {
		tCHAR  dir[MAX_PATH];
		tCHAR* pdir = dir;
		tDWORD chlen = run_under_nt ? UNIS : MBS;
		
		size = fGetTempPath( sizeof(dir)/chlen, dir );
		if( size > sizeof(dir)/chlen ) {
			if ( PR_FAIL(err= CALL_SYS_ObjHeapAlloc(p_path,&pdir,size*chlen)) )
				return err;
			size = fGetTempPath( size/chlen, pdir );
		}
		if ( size ) {
			tCODEPAGE cp;
			if ( run_under_nt ) {
				cp = cCP_UNICODE;
				size *= sizeof(tWCHAR);
			}
			else
				cp = cCP_ANSI;
			err = CALL_String_ImportFromBuff( p_path, 0, pdir, size, cp, cSTRING_Z );
		}
	}

	if ( PR_SUCC(err) && p_sub_dir ) {
		CALL_String_ProveLastSlash( p_path );
		err = CALL_String_AddFromBuff( p_path, 0, (tPTR)p_sub_dir, 0, p_cp, cSTRING_Z );
	}

	if ( PR_SUCC(err) )
		err = PrCreateDir( (hOBJECT)p_path );

	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrPeparseFileName )
// Parameters are:
tERROR PrPeparseFileName( hSTRING p_file_name ) {
	tERROR  err;
	tUINT   adv;
	tWCHAR  buff[20];
	tUINT   prefix_len;
	const tWCHAR* prefix;
	
	if ( !run_under_nt )
		return errOK;
	
	err = CALL_String_ExportToBuff( p_file_name, 0, cSTRING_WHOLE, buff, sizeof(buff), codepage, cSTRING_Z );
	if ( PR_FAIL(err) && (err != errBUFFER_TOO_SMALL) )
		return err;
	
	prefix = get_prefix( buff, sizeof(buff), &adv, &prefix_len );
	if ( !prefix ) // we do not need prefix
		return errOK;

	if ( adv ) // we already have some prefix
		return errOK;
	
	return CALL_String_ReplaceBuff( p_file_name, STR_RANGE(0,0), (tPTR)prefix, prefix_len*sizeof(tWCHAR), cCP_UNICODE, 0 );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrGetDiskFreeSpace )
// Parameters are:
tERROR PrGetDiskFreeSpace( hOBJECT p_src, tQWORD* p_result ) {

	// Place your code here
	tERROR err;
	tCHAR  path[MAX_PATH];
	tCHAR* ppath = path;
	tDWORD len = 0;
	ULARGE_INTEGER r1, r2, r3;

	if( !p_src )
		return errPARAMETER_INVALID;

	err = _ExtractName( p_src, &ppath, sizeof(path), 0, NULL, NULL );
	if( PR_SUCC(err) )
	{
		if ( run_under_nt )
		{
			tWCHAR drive[_MAX_PATH]=L"";
			_wsplitpath_s((wchar_t*)path, drive, countof(drive), 0, 0, 0, 0, 0, 0);
			drive[len=_toui32(wcslen(drive))]=L'\\', drive[++len]=L'\0';
			if( !GetDiskFreeSpaceExW(drive, &r1, &r2, &r3) )
				err = errOBJECT_INCOMPATIBLE;
		}
		else
		{
			tCHAR drive[_MAX_PATH]="";
			_splitpath_s(path, drive, countof(drive), 0, 0, 0, 0, 0, 0);
			drive[len=_toui32(strlen(drive))]='\\', drive[++len]='\0';
			if( !GetDiskFreeSpaceExA(drive, &r1, &r2, &r3) )
				err = errOBJECT_INCOMPATIBLE;
		}
	}

	if( PR_SUCC(err) && p_result )
		*p_result = r1.QuadPart;

	if ( ppath && (ppath != path) )
		CALL_SYS_ObjHeapFree( p_src, ppath );

	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrGetTempFileName )
// Parameters are:
//! tERROR PrGetTempFileName( hOBJECT p_path, const tVOID* p_prefix, tCODEPAGE p_cp ) {
//! tERROR PrGetTempFileName( hOBJECT p_path, tSTRING p_prefix ) {
tERROR PrGetTempFileName( hOBJECT p_path, const tVOID* p_prefix, tCODEPAGE p_cp ) {
	tERROR err;
	tCHAR  dir[MAX_PATH];
	tCHAR* pdir = dir;
	tDWORD size = 0;
	tIID   iid;

	// Place your code here
	if( !p_path )
		return errPARAMETER_INVALID;

	if( !fGetTempFileName )
		return errUNEXPECTED;

	err = _ExtractName( p_path, &pdir, sizeof(dir), 0, &size, &iid );
	if( iid != IID_STRING )
		return errPARAMETER_INVALID;

	if ( PR_FAIL(err) || !size || !*pdir ) {
		err = PrGetTempDir( p_path, 0, 0 );
		if ( PR_SUCC(err) ) {
			pdir = dir;
			err = _ExtractName( p_path, &pdir, sizeof(dir), 0, &size, &iid );
		}
	}

	if( PR_SUCC(err) ) {
		tCHAR  filename[MAX_PATH], prefix[0x100];
		if( p_prefix ) {
			if ( run_under_nt ) {
				if ( p_cp != cCP_UNICODE ) {
					mbstowcs_s( NULL, UNI(prefix), sizeof(prefix)/UNIS, MB(p_prefix), _TRUNCATE );
					p_prefix = prefix;
				}
			}
			else if ( p_cp != cCP_ANSI ) {
				wcstombs_s( NULL, MB(prefix), sizeof(prefix)/MBS, UNI(p_prefix), _TRUNCATE );
				p_prefix = prefix;
			}
		}
		if( !fGetTempFileName(pdir,p_prefix,0,filename) )
			err = _ConvertWin32Err( GetLastError() );
		else
			err = CALL_String_ImportFromBuff((hSTRING)p_path, NULL, filename, 0, codepage, cSTRING_Z);
	}

	if ( pdir && (pdir != dir) )
		CALL_SYS_ObjHeapFree( p_path, pdir );

	return err;
}
// AVP Prague stamp end


//---
static tERROR _check_if_exists( const tVOID* name ) {
	DWORD attr = fGetFileAttributes( name );
	if ( -1 == attr )
		return _ConvertWin32Err( GetLastError() );
	return errOK;
}



//---
static tVOID* _advW( const tVOID* str, tINT symb_dist ) {
	return (tVOID*)(UNI(str) + symb_dist);
}



//---
static tUINT _getW( const tVOID* str ) {
	return *UNI(str);
}



//---
static tVOID _setW( tVOID* str, tUINT symb ) {
	*UNI(str) = symb;
}



//---
static tVOID* _advA( const tVOID* str, tINT symb_dist ) {
	return (tVOID*)(MB(str) + symb_dist);
}



//---
static tUINT _getA( const tVOID* str ) {
	return *MB(str);
}



//---
static tVOID _setA( tVOID* str, tUINT symb ) {
	*MB(str) = symb;
}



//---
static tERROR _CreateDirectoryRecursively( tVOID* sDirectoryName, tPTR lpSecurityAttributes ) {
	tERROR err;
	tVOID* ptr;
	tUINT  c;

	size_t (__cdecl *_slen)( const tVOID* );
	tVOID* (*_adv)( const tVOID*, tINT );
	tUINT  (*_get)( const tVOID* );
	tVOID  (*_set)(tVOID*,tUINT);

	PR_TRACE_VARS;

	PR_TRACE((g_root, prtNOT_IMPORTANT, "nfio\tCreateDirectoryRecursively \"%s%S\"", PR_TRACE_PREPARE_STR_CP(sDirectoryName,codepage)));

	if ( run_under_nt ) {
		_slen = (size_t (__cdecl*)(const tVOID*))wcslen;
		_adv  = _advW;
		_get  = _getW;
		_set  = _setW;
	}
	else {
		_slen = (size_t (__cdecl*)(const tVOID*))strlen;
		_adv  = _advA;
		_get  = _getA;
		_set  = _setA;
	}

	if ( !sDirectoryName || (0 == _get(sDirectoryName)) ) {
		PR_TRACE(( g_root, prtNOTIFY, "nfio\tCreateDirectoryRecursively \"%s%S\" failed, parameter invalid", PR_TRACE_PREPARE_STR_CP(sDirectoryName ? sDirectoryName : L"",codepage) ));
		return errPARAMETER_INVALID;
	}

	err = _check_if_exists( sDirectoryName );
	if ( PR_SUCC(err) ) {
		PR_TRACE((g_root, prtNOTIFY, "nfio\tCreateDirectoryRecursively \"%s%S\" exists", PR_TRACE_PREPARE_STR_CP(sDirectoryName,codepage) ));
		return warnFALSE;
	}

	ptr = sDirectoryName;
	for( c=_get(ptr); c; ptr=_adv(ptr,1),c=_get(ptr) ) {
		if ( c == '/' )
			_set( ptr, '\\' );
	}

	ptr = _adv( sDirectoryName, _slen(sDirectoryName) - 1 );
	if ( '\\' == _get(ptr) ) {
		_set( ptr, 0 );
		ptr = _adv( ptr, -1 );
	}

	while( cTRUE ) {
		while( (ptr > sDirectoryName) && ('\\' != _get(ptr)) )
			ptr = _adv( ptr, -1 );
		if ( ptr == sDirectoryName ) {
			PR_TRACE(( g_root, prtIMPORTANT, "nfio\tCreateDirectoryRecursively \"%s%S\" root??? isn't exist", PR_TRACE_PREPARE_STR_CP(sDirectoryName,codepage) ));
			break;
		}
		_set( ptr, 0 );
		err = _check_if_exists( sDirectoryName );
		_set( ptr, L'\\' );
		if ( PR_SUCC(err) ) {
			ptr = _adv( ptr, 1 );
			break;
		}
		if ( err == errOBJECT_NOT_FOUND ) {
			ptr = _adv( ptr, -1 );
			continue;
		}
		PR_TRACE(( g_root, prtERROR, "nfio\tCreateDirectoryRecursively \"%s%S\" fails, %terr", PR_TRACE_PREPARE_STR_CP(sDirectoryName,codepage), err ));
		return err;
	}

	err = errOK;
	do {
		c = _get(ptr);
		if ( (c == '\\') || (c == 0) ) {
			tBOOL res;
			if ( c )
				_set( ptr, 0 );
			res = fCreateDirectory( sDirectoryName, lpSecurityAttributes );
			if ( c )
				_set( ptr, c );
			if ( !res ) {
				tDWORD dw = GetLastError();
				if ( dw != ERROR_ALREADY_EXISTS ) {
					PR_TRACE((g_root, prtERROR, "nfio\tCreateDirectoryRecursively \"%s%S\" fail with err=0x%x", PR_TRACE_PREPARE_STR_CP(sDirectoryName,codepage), dw));
					return _ConvertWin32Err( dw );
				}
				else
					err = warnFALSE;
			}
		}
		ptr = _adv( ptr, 1 );
	} while( c != 0 );

	PR_TRACE((g_root, prtNOTIFY, "nfio\tCreateDirectoryRecursively \"%s%S\" ok", PR_TRACE_PREPARE_STR_CP(sDirectoryName,codepage) ));
	return err;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrCreateDir )
// Parameters are:
tERROR PrCreateDir( hOBJECT p_path ) {

	tCHAR  dir[MAX_PATH];
	tCHAR* pdir = dir;
	tERROR err = _ExtractName( p_path, &pdir, sizeof(dir), pgOBJECT_PATH, 0, 0 );

	#if defined (_WIN32)
		if ( PR_SUCC(err) )
			err = _CreateDirectoryRecursively( pdir, 0 );
	#else
		if ( PR_SUCC(err) && mkdir(pdir, S_IRWXU | S_IRWXG ) )
			err = _ConvertUnixErr( errno );
	#endif

	if ( pdir != dir )
		CALL_SYS_ObjHeapFree( p_path, pdir );
	return err;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrRemoveDir )
// Parameters are:
tERROR PrRemoveDir( hOBJECT p_path ) {

	tCHAR  dir[MAX_PATH];
	tCHAR* pdir = dir;
	tERROR err = _ExtractName( p_path, &pdir, sizeof(dir), pgOBJECT_PATH, 0, 0 );

	if ( PR_SUCC(err) )
		err = NFIO_RemoveDir(pdir, codepage);
	if ( pdir != dir )
		CALL_SYS_ObjHeapFree( p_path, pdir );
	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrExecute )
// Parameters are:
tERROR PrExecute( hOBJECT p_executable, hSTRING p_cmd_line, hSTRING p_start_dir, tDWORD* p_result, tDWORD p_flags ) {
	tERROR err;
#if defined (_WIN32)

	tCHAR  params[MAX_PATH];
	tPTR   pparams = params;
	tDWORD sparams = 0;
	tCHAR  dir[MAX_PATH];
	tPTR   pdir = dir;
	tCHAR  exec[MAX_PATH];
	tPTR   pexec = exec;
	tDWORD sexec = 0;
	tCHAR  cmd[MAX_PATH];
	tPTR   pcmd = cmd;

	PROCESS_INFORMATION ProcessInformation;
	ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

	if ( p_result )
		*p_result = -1;

	if ( p_executable )
		err = _ExtractName( p_executable, &pexec, sizeof(exec), pgOBJECT_FULL_NAME, &sexec, 0 );
	else
		err = errPARAMETER_INVALID;

	if ( PR_SUCC(err) && p_cmd_line )
		err = _ExtractName( (hOBJECT)p_cmd_line, &pparams, sizeof(params), pgOBJECT_FULL_NAME, &sparams, 0 );
	else
		pparams = 0;

	if ( PR_SUCC(err) && p_start_dir )
		err = _ExtractName( (hOBJECT)p_start_dir, &pdir, sizeof(dir), pgOBJECT_FULL_NAME, 0, 0 );
	else
		pdir = 0;

	if ( PR_SUCC(err) && ((sexec + sparams + 4/*+2 quotation marks - 1 term zero*/) > sizeof(cmd)) )
		err = CALL_SYS_ObjHeapAlloc( g_root, &pcmd, sexec + sparams + 4 );
		
	if ( PR_SUCC(err) ) {
		BOOL result;

		if ( run_under_nt ) {
			STARTUPINFOW StartupInfo;
			ZeroMemory(&StartupInfo, sizeof(StartupInfo));
			StartupInfo.cb = sizeof(STARTUPINFOW);
			// Remarked by Pavel Nechaev becouse of no output in console apps
			//StartupInfo.dwFlags = STARTF_USESTDHANDLES;
			StartupInfo.hStdInput = 0;
			StartupInfo.hStdOutput = 0;
			StartupInfo.hStdError = 0;
			wsprintfW( pcmd, L"\"%s\" %s", pexec, pparams );
			result = CreateProcessW( 0, pcmd, 0, 0, cFALSE, 0, 0, pdir, &StartupInfo, &ProcessInformation );
		}
		else {
			STARTUPINFOA StartupInfo;
			ZeroMemory(&StartupInfo, sizeof(StartupInfo));
			StartupInfo.cb = sizeof(STARTUPINFOA);
			// Remarked by Pavel Nechaev becouse of no output in console apps
			//StartupInfo.dwFlags = STARTF_USESTDHANDLES;
			StartupInfo.hStdInput = 0;
			StartupInfo.hStdOutput = 0;
			StartupInfo.hStdError = 0;
			wsprintfA( pcmd, "\"%s\" %s", pexec, pparams );
			result = CreateProcessA( 0, pcmd, 0, 0, cFALSE, 0, 0, pdir, &StartupInfo, &ProcessInformation );
		}

		if ( !result )
			err = _ConvertWin32Err( GetLastError() );

		else if ( p_flags & fPR_EXECUTE_WAIT ) {
			tDWORD wait = WaitForSingleObject( ProcessInformation.hProcess, INFINITE );
			if ( wait != WAIT_OBJECT_0 )
				err = _ConvertWin32Err( GetLastError() );
			else if ( p_result )
				GetExitCodeProcess( ProcessInformation.hProcess, p_result );
		}

		if ( ProcessInformation.hThread && (INVALID_HANDLE_VALUE != ProcessInformation.hThread) )
			CloseHandle( ProcessInformation.hThread );
		if ( ProcessInformation.hProcess && (INVALID_HANDLE_VALUE != ProcessInformation.hProcess) )
			CloseHandle( ProcessInformation.hProcess );
	}
	
	if ( pcmd && (pcmd != cmd) )
		CALL_SYS_ObjHeapFree( g_root, pcmd );
	if ( pparams && (pparams != params) )
		CALL_SYS_ObjHeapFree( g_root, pparams );
	if ( pdir && (pdir != dir) )
		CALL_SYS_ObjHeapFree( g_root, pdir );
	if ( pexec && (pexec != exec) )
		CALL_SYS_ObjHeapFree( g_root, pexec );
#endif
	return err;
}
// AVP Prague stamp end



//// ---
//tERROR GetLongPathNameEx( tCHAR* From, tCHAR* To ) {
//	memcpy(To, From, 3);
//	To[3] = 0;
//	
//	char *pFromLo = &From[3];
//	char *pTo = &To[3];
//	char *pFromHi;
//	
//	for( ; pFromHi = strchr(pFromLo, '\\'); pFromLo = pFromHi )	{
//		pFromHi++;
//		
//		int nLen = pFromHi - pFromLo - 1;
//		memcpy(pTo, pFromLo, nLen);
//		*(pTo + nLen) = 0;
//		
//		WIN32_FIND_DATA wfd = {0, };
//		HANDLE hFind = FindFirstFile(To, &wfd);
//		if ( hFind == INVALID_HANDLE_VALUE )
//			return errNOT_OK;
//		FindClose(hFind);
//		
//		strcpy(pTo, wfd.cFileName);
//		
//		pTo += strlen(pTo);
//		*pTo++ = '\\';
//		*pTo = 0;
//	}
//	strcpy( pTo, pFromLo );
//	
//	return errOK;
//}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrDeleteFile )
// Parameters are:
tERROR PrDeleteFile( hSTRING p_file_to_delete, tDWORD p_reserved_flags ) {
	tWCHAR  file[MAX_PATH];
	tWCHAR*	pfile = file;
	tERROR  err;
	
	if ( !fDeleteFile )
		return errNOT_SUPPORTED;
	
	err = _ExtractName( (hOBJECT)p_file_to_delete, &pfile, sizeof(file), pgOBJECT_FULL_NAME, 0, 0 );
	if ( PR_SUCC(err) ) {
		tBOOL result = fDeleteFile( pfile );
		if ( !result )
			err = _ConvertWin32Err( GetLastError() );
	}

	if ( pfile != file )
		CALL_SYS_ObjHeapFree( p_file_to_delete, pfile );
	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, PrGetTotalWrites )
// Parameters are:
tERROR PrGetTotalWrites( tDWORD* p_size ) {

	// Place your code here
	if( p_size )
		*p_size = g_dwTotalWrites;

	return errOK;
}
// AVP Prague stamp end




#include <Prague/prague.h>
#include "win32_io.h"
#include "win32_objptr.h"
#include "win32_os.h"



#define MY_SIZEOF(a)  (sizeof(a)-sizeof(a[0]))
#define MY_COUNT(a)   (MY_SIZEOF(a)/sizeof(a[0]))

tBOOL                 run_under_nt       = cFALSE;
tBOOL                 g_case_sensitive   = cFALSE;
HMODULE               win_kernel;
const tWCHAR          g_prefix[]         = L"\\\\?\\";
const tUINT           g_prefix_size      = MY_SIZEOF(g_prefix);
const tUINT           g_prefix_count     = MY_COUNT(g_prefix);
const tWCHAR          g_prefix_unc[]     = L"\\\\?\\UNC\\";
const tUINT           g_prefix_unc_size  = MY_SIZEOF(g_prefix_unc);
const tUINT           g_prefix_unc_count = MY_COUNT(g_prefix_unc);

#undef MY_SIZEOF
#undef MY_COUNT


// ---
tUINT pr_call uni_len( tWSTRING str, tUINT size ) {
	tUINT len;
	if ( !str || !*UNI(str) )
		return 0;
	for( len=UNIS; (len<size) && *UNI(MB(str)+len); len+=UNIS )
		;
	return len;
}



// ---
tUINT pr_call mb_len( tSTRING str, tUINT size ) {
	tUINT len;
	if ( !str || !*str )
		return 0;
	for( len=1; (len<size) && str[len]; len++ )
		;
	return len;
}



// ---
tSTRING pr_call FindSeparator( tSTRING buffer, tDWORD* size ) {
	tCHAR* p;
	
	if ( !*size )
		*size = (lstrlen(buffer) + 1) * MBS;
	
	for( p=buffer+((*size/MBS)-1); p>=buffer; --p ) {
		if ( *p == '/' )
			return ++p;
		#if defined (_WIN32)
			if ( *p == '\\' )
				return ++p;
			if ( *p == ':' )
				return ++p;
		#endif
	}
	return 0;
}



// ---
tWSTRING pr_call FindSeparatorW( tWSTRING buffer, tDWORD* size ) {
	tWCHAR* p;
	
	if ( !*size )
		*size = _toui32((wcslen( buffer ) + 1) * UNIS);
	
	for( p=buffer+((*size/UNIS)-1); p>=buffer; --p ) {
		if ( *p == L'\\' )
			return ++p;
		if ( *p == L'/' )
			return ++p;
		if ( *p == L':' )
			return ++p;
	}
	return 0;
}



// ---
const tWCHAR* pr_call _find_w_prefix( const tWCHAR* buffer, tUINT size ) {
	if ( (size > g_prefix_size) && !wcsncmp(buffer,g_prefix,g_prefix_count) )
		return buffer + g_prefix_count;
	return 0;
}



// ---
const tWCHAR* pr_call _find_w_unc_prefix( const tWCHAR* buffer, tUINT size ) {
	if ( (size > g_prefix_unc_size) && !_wcsnicmp(buffer,g_prefix_unc,g_prefix_unc_count) )
		return buffer + g_prefix_unc_count;
	return 0;
}




// ---
const tWCHAR* pr_call _find_w_unc_path( const tWCHAR* buffer, tUINT size, tBOOL double_slash_passed ) {
	
	const tWCHAR* sl;
	const tWCHAR* sl2;

	if ( !double_slash_passed ) {
		if ( size <= (2 * UNIS) )
			return 0;
		if ( wcsncmp(buffer,L"\\\\",2) )
			return 0;
		buffer += 2;
	}

	if ( *buffer == L'?' )
		return 0;
	sl = wcschr( buffer, PATH_DELIMETER_WIDECHAR );
	#if defined( _WIN32 )
		if ( !sl )
			sl = wcschr( buffer, L'/' );
	#endif
	if ( !sl )
		return 0;
	sl2 = wcschr( ++sl, PATH_DELIMETER_WIDECHAR );
	#if defined( _WIN32 )
		if ( !sl2 )
			sl2 = wcschr( sl, L'/' );
	#endif
	if ( sl2 )
		return sl2;
	return sl + wcslen(sl);
}




// ---
const tCHAR* pr_call _find_a_unc_path( const tCHAR* buffer, tUINT size, tBOOL double_slash_passed ) {
	
	const tCHAR* sl;
	const tCHAR* sl2;

	if ( !double_slash_passed ) {
		if ( size <= (2 * MBS) )
			return 0;
		if ( strncmp(buffer,"\\\\",2) )
			return 0;
		buffer += 2;
	}

	sl = strchr( buffer, PATH_DELIMETER_CHAR );
	#if defined( _WIN32 )
		if ( !sl )
			sl = strchr( buffer, '/' );
	#endif
	if ( !sl )
		return 0;
	sl2 = strchr( sl, PATH_DELIMETER_CHAR );
	#if defined( _WIN32 )
		if ( !sl2 )
			sl2 = strchr( sl, '/' );
	#endif
	if ( sl2 )
		return sl2;
	return sl + strlen(sl);
}




// ---
const tWCHAR* pr_call get_prefix( tWSTRING buffer, tUINT size, tUINT* adv, tUINT* len ) {
	if ( !(*buffer) ) {
		*adv = 0;
		*len = 0;
		return 0;
	}
  if ( (size >= (2 * UNIS)) && (buffer[1] == L':') ) { // absolute path
		*adv = 0;
//	if ( size > (UNIS*MAX_PATH) )
//		*len = g_prefix_size;
//	else
//		*len = 0;
//	return g_prefix;
		*len = g_prefix_size;
		return g_prefix;
  }
  if ( _find_w_unc_prefix(buffer,size) ) { // already unc prefixed
		*adv = g_prefix_unc_count;
		*len = g_prefix_unc_size;
		return g_prefix_unc;
	}
  if ( _find_w_prefix(buffer,size) ) { // already prefixed
	  *adv = g_prefix_count;
	  *len = g_prefix_size;
	  return g_prefix;
  }
  if ( (size > (2 * UNIS)) && !wcsncmp(buffer,L"\\\\",2) ) { // unc path
		*adv = 2;
		*len = g_prefix_unc_size;
		return g_prefix_unc;
	}
  *adv = 0;
	*len = 0;
	return 0;
}




// ---
tUINT pr_call calc_full_path_len( tWSTRING full, tWSTRING path, tUINT copy_len, tBOOL* need_prefix ) {
	tUINT prefix_len = _toui32(MB(path) - MB(full));
	tUINT full_len = copy_len + UNIS; // terminted zero
	if ( prefix_len && (prefix_len >= g_prefix_unc_size) && !wcsncmp(g_prefix_unc,full,g_prefix_unc_count) ) { // it is a "\\server_name\share_name" type of the path
		*need_prefix = cTRUE;
		full_len += 2 * UNIS; // L"\\\\" prefix
	}
	else
		*need_prefix = cFALSE;
	return full_len;
}




// ---
tERROR pr_call _ExtractName( hOBJECT obj, tPTR* buffer, tDWORD len, tPROPID propid, tDWORD* out_len, tIID* iid_ptr ) {
	tDWORD olen;
	tERROR error;
  tIID   iid = CALL_SYS_PropertyGetDWord( obj, pgINTERFACE_ID );

  if ( !out_len )
    out_len = &olen;

	if ( iid_ptr )
		*iid_ptr = iid;
	
  if ( (propid == 0) || (iid == IID_STRING) )
    propid = pgOBJECT_FULL_NAME;

	*out_len = 0;
  error = CALL_SYS_PropertyGetStr( obj, out_len, propid, *buffer, len, codepage );
  if ( error == errBUFFER_TOO_SMALL ) {
    if ( (*out_len == 0) && PR_FAIL(error=CALL_SYS_PropertyGetStr(obj,out_len,propid,0,0,codepage)) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(obj,buffer,*out_len)) )
      ;
    else
      error = CALL_SYS_PropertyGetStr( obj, out_len, propid, *buffer, *out_len, codepage );
  }

	return error;
}




// ---
tERROR pr_call _ConvertWin32Err( tDWORD win32err ) {
	switch( win32err ) {
		case ERROR_SUCCESS               : return errOK;

		case ERROR_FILE_NOT_FOUND        :
		case ERROR_PATH_NOT_FOUND        : 
		case ERROR_BAD_NETPATH           : return errOBJECT_NOT_FOUND;

		case ERROR_ALREADY_EXISTS        :
		case ERROR_FILE_EXISTS           : return errOBJECT_ALREADY_EXISTS;

		case ERROR_SHARING_VIOLATION     : 
		case ERROR_LOCK_VIOLATION        :
		case ERROR_DRIVE_LOCKED          : return errLOCKED;

		case ERROR_LOGON_FAILURE         :
		case ERROR_ACCESS_DENIED         : 
		case ERROR_NETWORK_ACCESS_DENIED : return errACCESS_DENIED;

		case ERROR_NO_MORE_FILES         : return errEND_OF_THE_LIST;
		case ERROR_WRITE_PROTECT         : return errWRITE_PROTECT;

		case ERROR_DISK_FULL             : return errOUT_OF_SPACE;

		case ERROR_NOT_READY             : return errNOT_READY;
		default                          : return errUNEXPECTED;
	}
}
#if defined (__unix__)
#include <errno.h>
tERROR pr_call _ConvertUnixErr( tDWORD unixErr ) {
	switch( unixErr ) {
		case ENOENT                      : return errOBJECT_NOT_FOUND;

		case EEXIST                      : return errOBJECT_ALREADY_EXISTS;

		case ETXTBSY                     : return errLOCKED;

		case EACCES                      :
		case EPERM                       : return errACCESS_DENIED;

		case EROFS                       : return errWRITE_PROTECT;

		default                          : return errUNEXPECTED;
	}
}
#endif



// ---
tBOOL pr_call is_relative_path( tCHAR* buffer, tDWORD size ) {
	if ( run_under_nt ) {
		if ( (size >= UNIS) && IS_W_DELIMETER(*UNI(buffer)) )
			return cFALSE;
		if ( (size >= 2*UNIS) && (*(UNI(buffer)+1) == L':') )
			return cFALSE;
		return cTRUE;
	}
	if ( (size > MBS) && IS_A_DELIMETER(*buffer) )
		return cFALSE;
#if defined (_WIN32)
	if ( (size >= 2*MBS) && (*(buffer+1) == ':') )
		return cFALSE;
#endif
	return TRUE;
}




// ---
tERROR pr_call is_folder( tCHAR* name, tDWORD* werr ) {
	tDWORD lwerr;
  tDWORD attr;
	HANDLE handle;
  WIN32_FIND_DATAW fd;
  
  if ( !werr )
    werr = &lwerr;

  if ( fGetFileAttributes && (((tDWORD)-1) != (attr=fGetFileAttributes(name))) )
		*werr = ERROR_SUCCESS;
  else if ( INVALID_HANDLE_VALUE != (handle=fFindFirstFile(name,&fd)) ) {
    FindClose( handle );
    *werr = ERROR_SUCCESS;
    attr = fd.dwFileAttributes;
  }
  else
    return _ConvertWin32Err( *werr=GetLastError() );
	return (attr & FILE_ATTRIBUTE_DIRECTORY) ? errOK : errOBJECT_NOT_FOUND;
}



// ---
//tERROR pr_call is_exist( tCHAR* fname, tDWORD* werr ) {
//  tDWORD lwerr;
//  tDWORD attr = -1;
//  HANDLE handle;
//  WIN32_FIND_DATAW fd;
//
//  if ( !werr )
//    werr = &lwerr;
//
//  if ( fGetFileAttributes )
//    attr = fGetFileAttributes( fname );
//  else if ( INVALID_HANDLE_VALUE != (handle=fFindFirstFile(fname,&fd)) ) {
//    FindClose( handle );
//    *werr = ERROR_SUCCESS;
//    attr = fd.dwFileAttributes;
//  }
//
//  if ( ((tDWORD)-1) == attr )
//    return _ConvertWin32Err( *werr=GetLastError() );
//  else {
//    *werr = ERROR_SUCCESS;
//    return errOK;
//  }
//}



/*
// ---
tBOOL pr_call is_current_path( tCHAR* buffer, tDWORD size ) {
	if ( (size == 1) && (*buffer == '.') )
		return TRUE;
	else if ( (size == 2) && (*buffer == '.') && (*(buffer+1) == 0) )
		return TRUE;
	else if ( (size > 2) && (*buffer == '.') && (*(buffer+1) == '\\') )
		return TRUE;
	else
		return cFALSE;
}



// ---
tBOOL pr_call current_go( tCHAR* buffer, tDWORD* size ) {
	if ( (*size == 1) && (*buffer == '.') ) {
		*buffer = 0;
		*size = 0;
		return TRUE;
	}
	else if ( (*size > 1) && (*buffer =='.') && ((*(buffer+1)==0) || (*(buffer+1)=='\\')) ) {
		memmove( buffer, buffer+1, --(*size) );
		return TRUE;
	}
	else
		return FALSE;

}


// ---
tBOOL pr_call relative_go_up( tCHAR* buffer, tDWORD* size ) {
	if ( (*size >= 2) && (*((tWORD*)buffer) == MAKEWORD('.','.')) ) {
		tDWORD s;
		if ( *size >= 3 && buffer[2] == '\\' || buffer[2] == '/' )
			s = 3;
		else
			s = 2;
		memmove( buffer, buffer+s, *size-=s );
		return cTRUE;
	}
	else
		return cFALSE;
}



// ---
tBOOL pr_call can_go_up( tCHAR* path, tDWORD size ) {
	tCHAR* p = path + size;

	while( p > path && !*p )
		p--;
	p--;

	while( p > path && *p != '\\' && *p != '/' )
		p--;

	if ( p > (path+2) )
		return cTRUE;
	else
		return cFALSE;
}



// ---
tBOOL pr_call go_up( tCHAR* path, tDWORD* size ) {
	tCHAR* p = path + *size;

	while( p > path && !*p )
		p--;
	p--;

	while( p > path && *p != '\\' && *p != '/' )
		p--;

	if ( p > (path+2) ) {
		*(++p) = 0;
		*size = p - path;
		return cTRUE;
	}
	else
		return cFALSE;
}
*/

// ---
tBOOL is_the_same_volume( tPTR first, tPTR second ) {
#if defined (WIN32)
  if ( run_under_nt ) {
    wchar_t fdr[_MAX_DRIVE];
    wchar_t sdr[_MAX_DRIVE];
    _wsplitpath_s( (const wchar_t*)first,  fdr, countof(fdr), 0, 0, 0, 0, 0, 0 );
    _wsplitpath_s( (const wchar_t*)second, sdr, countof(sdr), 0, 0, 0, 0, 0, 0 );
    _wcslwr_s( fdr, countof(fdr) );
    _wcslwr_s( sdr, countof(sdr) );
    if ( (fdr[0] == sdr[0]) && (*UNI(first) != L'\\') )
      return cTRUE;
  }
  else {
    char fdr[_MAX_DRIVE];
    char sdr[_MAX_DRIVE];
    _splitpath_s( (const char*)first,  fdr, countof(fdr), 0, 0, 0, 0, 0, 0 );
    _splitpath_s( (const char*)second, sdr, countof(sdr), 0, 0, 0, 0, 0, 0 );
    _strlwr_s( fdr, countof(fdr) );
    _strlwr_s( sdr, countof(sdr) );
    if ( (fdr[0] == sdr[0]) && (*MB(first) != '\\') )
      return cTRUE;
  }
#endif
  return cFALSE;
}



// ---
tERROR pr_call _ren_move( tPTR obj, tPTR existing, tPTR new_name, tBOOL overwrite, tDWORD* native_error ) {
  *native_error = 0;

  if ( fMoveFileEx ) {
    DWORD flags = MOVEFILE_COPY_ALLOWED;
    if ( overwrite )
      flags |= MOVEFILE_REPLACE_EXISTING;
    if ( fMoveFileEx(existing,new_name,flags) )
      return errOK;
    else {
      *native_error = GetLastError();
      PR_TRACE(( obj, prtERROR, "MoveFileEx returns error %d", *native_error ));
      return _ConvertWin32Err( *native_error );
    }
  }

  if ( is_the_same_volume(existing,new_name) ) {
    if ( !fMoveFile ) {
      PR_TRACE(( obj, prtERROR, "MoveFile export pointer cannot be imported from the Windows kernel" ));
      return errINTERFACE_NOT_LOADED;
    }
    
    if ( fMoveFile(existing,new_name) )
      return errOK;
    
    *native_error = GetLastError();
    if ( *native_error != ERROR_ALREADY_EXISTS ) {
      PR_TRACE(( obj, prtERROR, "MoveFile returns error %d", *native_error ));
      return _ConvertWin32Err( *native_error );
    }
    else if ( !fDeleteFile ) {
      PR_TRACE(( obj, prtERROR, "DeleteFile export pointer cannot be imported from the Windows kernel" ));
      return errINTERFACE_NOT_LOADED;
    }
    else if ( !fDeleteFile(new_name) ) {
      *native_error = GetLastError();
      PR_TRACE(( obj, prtERROR, "DeleteFile returns error %d", *native_error ));
      return _ConvertWin32Err( *native_error );
    }
    else if ( !fMoveFile(existing,new_name) ) {
      *native_error = GetLastError();
      PR_TRACE(( obj, prtERROR, "MoveFile returns error %d", *native_error ));
      return _ConvertWin32Err( *native_error );
    }
    else {
      *native_error = 0;
      return errOK;
    }
  }
  else if ( !fCopyFile ) {
    PR_TRACE(( obj, prtERROR, "CopyFile export pointer cannot be imported from the Windows kernel" ));
    return errINTERFACE_NOT_LOADED;
  }
  else if ( !fDeleteFile ) {
    PR_TRACE(( obj, prtERROR, "DeleteFile export pointer cannot be imported from the Windows kernel" ));
    return errINTERFACE_NOT_LOADED;
  }
  else if ( !fCopyFile(existing,new_name,!overwrite) ) {
    *native_error = GetLastError();
    PR_TRACE(( obj, prtERROR, "CopyFile returns error %d", *native_error ));
    return _ConvertWin32Err( *native_error );
  }
  else if ( !fDeleteFile(new_name) ) {
    *native_error = GetLastError();
    PR_TRACE(( obj, prtERROR, "DeleteFile returns error %d", *native_error ));
    return _ConvertWin32Err( *native_error );
  }
  else {
    *native_error = 0;
    return errOK;
  }

}




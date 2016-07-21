#error "OLD VERSION!"
/*
#if !defined (_SIGNCHK_WIN32_INC_)
#define _SIGNCHK_WIN32_INC_


#include <windows.h>

#define TRY     __try
#define EXCEPT  __except( EXCEPTION_EXECUTE_HANDLER ) \
                     { *sign_lib_error_code = SIGN_UNKNOWN_ERROR; \
                       return errUNEXPECTED; \
                     }


typedef HANDLE kl_file_t;
HANDLE INVALID_FILE = INVALID_HANDLE_VALUE;
#define closeFile(handle) CloseHandle(handle)
// ---
tINT check_cb( tVOID* params, tVOID* buffer, tINT buff_len ) {
  if ( ReadFile(*(kl_file_t*)params,buffer,buff_len,&buff_len,0) )
    return buff_len;
  else
    return -1;
}

// ---
tINT check_by_file( kl_file_t file, tVOID* buffer, tINT buff_len ) {
	DWORD read;
  if ( ReadFile(file,buffer,buff_len,&read,0) )
    return read;
  else
    return -1;
}


tERROR pr_call open_file( const tVOID* name, tCODEPAGE cp, kl_file_t* file, tDWORD* sign_lib_error_code ) {
  #if defined(_WIN32)
  	if ( cp == cCP_UNICODE )
  		*file = CreateFileW( UNI(name), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
  	else
  #endif
  		*file = CreateFileA( MB(name), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
  
  if ( *file == INVALID_HANDLE_VALUE ) {
    DWORD win32err = GetLastError();
    switch( win32err ) {
      case ERROR_FILE_NOT_FOUND :
      case ERROR_PATH_NOT_FOUND :  
        if (sign_lib_error_code) 
          *sign_lib_error_code = SIGN_CANT_READ;     
        return errOBJECT_NOT_FOUND;
      case ERROR_ACCESS_DENIED  :  
        if (sign_lib_error_code)  
          *sign_lib_error_code = SIGN_CANT_READ;     
        return errACCESS_DENIED;
      default:  
        if (sign_lib_error_code)
          *sign_lib_error_code = SIGN_UNKNOWN_ERROR; 
        return errUNEXPECTED;
    }
  }
  else
    return errOK;
}



// ---
static tERROR _getFileSign( tVOID* keys[], tDWORD key_count, const tCHAR* fname, tBYTE* buff, tUINT len, tDWORD* keys_found, tDWORD* sign_lib_error_code ) {
  tERROR  error = errOK;
  HANDLE  file = 0;
  tUINT   file_length = 0;
  tUINT   offset = 0;

  if ( !*fname ) {
    *sign_lib_error_code = SIGN_BAD_PARAMETERS;
    return errOBJECT_BAD_PTR;
  }
	
  if ( PR_FAIL(error=open_file((const tSTRING)fname,cCP_ANSI,&file,sign_lib_error_code)) )
    return error;
	
  file_length = GetFileSize( file, 0 );
  if ( file_length > len )
    offset = file_length - len;
  else {
    len = file_length;
    offset = 0;
  }
	
  if ( offset && (-1==SetFilePointer(file,offset,0,FILE_BEGIN)) )
    return errOBJECT_SEEK;
	
  if ( !ReadFile(file,buff,len,&len,0) )
    return errOBJECT_READ;
	
  CloseHandle( file );
	
  return _getBuffSign( keys, key_count, buff, len, offset, keys_found, sign_lib_error_code );
}

#endif //_SIGNCHK_WIN32_INC_
*/
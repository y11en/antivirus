#include "signport.h"


#ifdef _WIN32

#include <sign\sign_lib.h>
#include <windows.h>

// ---
static int destr( int ret, HANDLE file ) {
  if ( INVALID_HANDLE_VALUE != file )
    CloseHandle( file );
  return ret;
}


// ---
int sign_load_file_signature( CTN_Signature* sign, const char* sgn_file, const char* password ) {
  
  HANDLE file;

  if ( !sign || !sgn_file || !*sgn_file )
    return SIGN_BAD_PARAMETERS;

  file = INVALID_HANDLE_VALUE;

  MY_TRY {
    file = CreateFile( sgn_file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
    if ( INVALID_HANDLE_VALUE != file ) {

      char sgn[CT_N_LEN_PRIVATEKEYFILE];
      DWORD size = GetFileSize( file, 0 );
      
      if ( size < CT_N_LEN_PRIVATEKEYFILE )
        return destr( SIGN_BAD_FILE, file );

      if ( ReadFile(file,sgn,CT_N_LEN_PRIVATEKEYFILE,&size,0) && size == CT_N_LEN_PRIVATEKEYFILE ) {
        CloseHandle( file );
        return sign_load_signature( sign, sgn, CT_N_LEN_PRIVATEKEYFILE, password );
      }
      else 
        return destr( SIGN_CANT_READ, file );
    }
    else {
      DWORD err = GetLastError();
      if ( err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND )
        return destr( SIGN_FILE_NOT_FOUND, file );
      else
        return destr( SIGN_CANT_READ, file );
    }
  }
  MY_EXCEPT
}

#endif // _WIN32



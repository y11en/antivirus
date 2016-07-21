#include "signport.h"

#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <Sign\Sign_lib.h>

#define INVALID_HANDLE_VALUE -1


// ---
int sign_load_file_signature( CTN_Signature* sign, const char* sgn_file, const char* password ) {
  
  int file;

  if ( !sign || !sgn_file || !*sgn_file )
    return SIGN_BAD_PARAMETERS;

  file = INVALID_HANDLE_VALUE;

  MY_TRY {
    file = _open( sgn_file, O_RDONLY | O_BINARY );
    if ( INVALID_HANDLE_VALUE != file ) {

      char sgn[CT_N_LEN_PRIVATEKEYFILE];
      long size = _filelength( file );
      
      if ( size < CT_N_LEN_PRIVATEKEYFILE ) {
        _close( file );
        return SIGN_BAD_FILE;
      }
      else {
        size = _read( file, sgn, CT_N_LEN_PRIVATEKEYFILE );
        _close( file );
        if ( size == CT_N_LEN_PRIVATEKEYFILE )
          return sign_load_signature( sign, sgn, CT_N_LEN_PRIVATEKEYFILE, password );
        else 
          return SIGN_CANT_READ;
      }
    }
    else if ( errno == ENOENT )
      return SIGN_FILE_NOT_FOUND;
    else
      return SIGN_CANT_READ;
  }
  MY_EXCEPT
}




#include "signport.h"

#include <io.h>
#include <fcntl.h>
#include <Sign\Sign_lib.h>


// ---
int sign_load_pub_key_file( const char* pub_key_file, void* pub_key ) {
  int file = -1;
  MY_TRY {
    file = _open( pub_key_file, O_RDONLY | O_BINARY );
    if ( -1 != file ) {
      int  err;
      long size = _filelength( file );
      if ( size <= (PUBKEY_LEN_MIN-CT_N_LEN_SIGN) || PUBKEY_LEN_MAX < size )
        err = SIGN_BAD_OPEN_KEY;
      else if ( size != _read(file,pub_key,size) ) 
        err = SIGN_CANT_READ;
      else 
        err = SIGN_OK;
      _close( file );
      return err;
    }
    else 
      return SIGN_CANT_READ;
  }
  MY_EXCEPT
}



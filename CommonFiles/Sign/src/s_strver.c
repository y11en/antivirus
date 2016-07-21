#include "signport.h"

#include <memory.h>
#include <Sign/Sign_lib.h>

// return text stream version of streamed public key
// ---
int sign_get_txt_stream_version( const void* stream ) {
  MY_TRY {
    if ( !stream )
      return SIGN_BAD_PARAMETERS;
    else if ( !s_mcmp(stream,"0001",4) )
      return 1;
    else if ( !s_mcmp(stream,"0002",4) )
      return 2;
    else 
      return 0;
  }
  MY_EXCEPT
}


#include "signport.h"


#include <Sign/Sign_lib.h>

// ---
int sign_buffer_info( void* buffer, int buff_len, int* out_len, int* not_count ) {
  MY_TRY {

    char* ptr = ((char*)buffer) + buff_len - NOTARY_TXT_END;

    if ( not_count )
      *not_count = 0;

    if ( buff_len <= sizeof(GUID) )
      return SIGN_BAD_PARAMETERS;

    if ( !out_len && !not_count )
      return SIGN_BAD_PARAMETERS;

    //while( (ptr > (((char*)buffer) + NOTARY_TXT_LEN)) && !IS_END_OF_NOT(ptr) ) 
    //  ptr--;

    while( 
      ( ptr >= (((char*)buffer) + NOTARY_TXT_LEN) ) &&
      IS_BEG_OF_NOT( ptr-NOTARY_TXT_LEN ) &&
      IS_END_OF_NOT( ptr )
    ) {
      
      if ( not_count )
        (*not_count)++;

      ptr -= NOTARY_TXT_SIZE;
    }

    ptr ++;
    ptr ++;

    if ( out_len )
      *out_len = (int)(ptr - ((char*)buffer));

    return SIGN_OK;
  }
  MY_EXCEPT
}


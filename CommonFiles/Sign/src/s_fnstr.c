#include "signport.h"

#include <memory.h>
#include <Sign\Sign_lib.h>
#include <Sign\ct_funcs.h>

int sign_find_notary_str( const void* buffer, int buff_len, int* not_start, const char* guid ) {

  MY_TRY {
    char* ptr;

    if ( !buffer || buff_len<0 || !guid )
      return SIGN_BAD_PARAMETERS;

    ptr = ((char*)buffer) + ( buff_len - NOTARY_TXT_END );

    //while( (ptr > (((char*)buffer) + NOTARY_TXT_LEN)) && !IS_END_OF_NOT(ptr) ) 
    //  ptr--;

    while( 
      ( ptr > (((char*)buffer) + NOTARY_TXT_LEN) ) &&
      IS_BEG_OF_NOT( ptr-NOTARY_TXT_LEN ) &&
      IS_END_OF_NOT( ptr )
    ) {
      if ( !s_mcmp(ptr-NOTARY_TXT,guid,STR_GUID_TXT_LEN) ) {
        if ( not_start )
          *not_start = (int)(ptr - (char*)buffer) - NOTARY_TXT_LEN;
        return SIGN_OK;
      }
      ptr -= NOTARY_TXT_SIZE;
    }
    return SIGN_NOTARY_NOT_FOUND;
  }
  MY_EXCEPT

}




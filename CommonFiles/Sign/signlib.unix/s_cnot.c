#include "signport.h"
#include <Sign/CT_FUNCS.h>
#include <Sign/Sign_lib.h>
#include "avp_defs.h"

// ---
int sign_collect_notaries( void* buffer, int buff_len, void* notaries[], int* count, int* sign_start ) {

  MY_TRY {
    int not_count;
    char* ptr = ((char*)buffer) + ( buff_len - NOTARY_TXT_END );

    if ( buff_len <= 0 || ( notaries && !count ) )
      return SIGN_BAD_PARAMETERS;

    if ( count ) {
      not_count = *count;
      *count = 0;
    }

    //while( (ptr > (((char*)buffer) + NOTARY_TXT_LEN)) && !IS_END_OF_NOT(ptr) ) 
    //  ptr--;

    while( 
      ( !count || (*count < not_count) ) &&
      ( ptr > (((char*)buffer) + NOTARY_TXT_LEN) ) &&
      IS_BEG_OF_NOT( ptr-NOTARY_TXT_LEN ) &&
      IS_END_OF_NOT( ptr )
    ) {
      
      if( sign_start )
        *sign_start = (ptr - (char*)buffer) - NOTARY_TXT_LEN;

      if ( count ) 
        notaries[ (*count)++ ] = ptr - NOTARY_TXT_LEN;

      ptr -= NOTARY_TXT_SIZE;
    }
    return SIGN_OK;
  }
  MY_EXCEPT

}





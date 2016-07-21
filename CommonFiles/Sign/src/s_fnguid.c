#include "signport.h"

#include <memory.h>
#include <Sign/Sign_lib.h>
#include <Sign/ct_funcs.h>
#include <Stuff/text2bin.h>

int sign_find_notary( const void* buffer, int buff_len, int* not_start, GUID* guid ) {

  MY_TRY {
    char* ptr;
    char compare[ ((sizeof(GUID)+2)/3)*4 ];
    unsigned char len;

    if ( !buffer || buff_len<0 || !guid )
      return SIGN_BAD_PARAMETERS;

    ptr = ((char*)buffer) + ( buff_len - 2 );
    len = BinToText( guid, sizeof(GUID), compare, sizeof(compare) );

    //while( (ptr > (((char*)buffer) + NOTARY_TXT_LEN)) && !IS_END_OF_NOT(ptr) ) 
    //  ptr--;

    while( 
      ( ptr > (((char*)buffer) + NOTARY_TXT_LEN) ) &&
      IS_BEG_OF_NOT( ptr-NOTARY_TXT_LEN ) &&
      IS_END_OF_NOT( ptr )
    ) {
      if ( !s_mcmp(compare,ptr-NOTARY_TXT,21) ) {
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



